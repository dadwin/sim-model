//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "Server.h"
#include "Flow.h"
#include "Net.h"
#include "Solver.h"
#include "Messages.h"
#include <cexception.h>

#include <algorithm>
#include <map>

Define_Module(Server);

int Server::getAddress() const {
    return address;
}

void Server::initialize()
{
    routing = check_and_cast<Routing*>(getModuleByPath("routing"));
    net = check_and_cast<Net*>(getModuleByPath("net"));
    // set address
    address = routing->assignAddress(this);

    // identifying channels as resources
    // we iterate through all OUTPUT gates and those which have underlying channels,
    // since some of them have not - e.g. case of connection between rack switch and rack itself.
    for (GateIterator it(this); !it.end(); it++) {
        auto gate = it();
        if (gate->getType() != cGate::OUTPUT)
            continue;

        auto channel = gate->getChannel();
        if (channel == nullptr)
            continue;

        const double maxCapacity = channel->par("maxCapacity");
        Resource* resource = new LinkResource(channel, maxCapacity); // TODO what about id? can it be 'this' pointer?
        net->registerResource(resource);
    }

    scheduleAt(0, new cMessage("scheduledInitialize", MessageType::ScheduledInitialize));

}

void Server::scheduledInitialize() {
    // getting flows to be scheduled
    auto flowList = net->getFlowProfile(address);

    // schedule flows
    for (auto flowParameters : flowList) {

        auto event = new cMessage("NewFlow", MessageType::NewFlow);
        auto par = new cMsgPar("flowParameters");
        par->setPointerValue(flowParameters);
        event->addPar(par);

        scheduleAt(flowParameters->start, event);
    }
}

void Server::handleMessage(cMessage *msg)
{
    MessageType msgType = static_cast<MessageType>(msg->getKind());

    switch (msgType) {
    case FlowMessage: {
        if (msg->isSelfMessage() != true)
            throw cRuntimeError("FlowMessage must be self-message");
        handleFlowMessage(msg);
        break;
    }
    case NewFlow: {
        if (msg->isSelfMessage() != true)
            throw cRuntimeError("NewFlow must be self-message");
        handleNewFlow(msg);
        delete msg;
        break;
    }
    case DataMessage: {
        if (msg->isSelfMessage() == true)
            throw cRuntimeError("DataMessage must be non-self-message");
        handleDataMessage(msg);
        break;
    }
    case ScheduledInitialize: {
        if (msg->isSelfMessage() != true)
            throw cRuntimeError("ScheduledInitialize must be self-message");
        scheduledInitialize();
        delete msg;
        break;
    }
    default: {
        break;
    }
    }
}

void Server::handleDataMessage(cMessage *msg)
{
    Flow* flow = (Flow*) msg->par("flow").pointerValue();
    removeFlow(flow);

    //delete msg; // NOTE Message is part of Flow and Flow is owner of it

}

void Server::handleFlowMessage(cMessage *msg)
{
    Flow* flow = (Flow*) msg->par("flow").pointerValue();

    const int gateId = flow->getNextGateId();
    msg->setName("DataMessage");
    msg->setKind(MessageType::DataMessage);
    msg->setSchedulingPriority(-1);
    send(msg, gateId);
}

void Server::handleNewFlow(cMessage *msg)
{
    auto fp= (Net::FlowParameters*) msg->par("flowParameters").pointerValue();
    addFlow(fp->source, fp->destination, fp->start, fp->end, fp->demand);
}

void Server::addFlow(const int sourceAddress, const int destAddress,
                  const simtime_t startTime, const simtime_t endTime,
                  const double desiredAllocation) {

    // Input parameters for flow:
    // * source server - a node from which flow runs
    // * destination server - a node toward which flow runs
    // * start time - time when flow starts running
    // * desired allocation - demand of capacity of network resources that flow wants to obtain
    // * end time - time when flow ends running, if its allocation is equal to desired allocation.
    // Relation between times and allocation is as following:
    //     (end time - start time)*allocation = constant
    // This constant is a amount of data transferred by a flow.

    // calculate path from src to dst
    // path is sequence of cModules with @node property
    auto path = net->getResourcePath(sourceAddress, destAddress);
    auto gatePath = net->getGatePath(sourceAddress, destAddress);

    Flow* flow = new Flow(routing->getServerByAddress(sourceAddress), desiredAllocation, startTime, endTime, path, gatePath);

    // add flow to list of all flows
    net->flows.push_back(flow);

    Solver::solve(net->flows, net->resources);
    Solver::printFlows(net->flows);
    //Solver::printResources(net->resources);

    // find reduced flows and recalculate end time for them
    // for new flow isReduced() always returns false,
    // since its previous allocation is zero
    simtime_t now = simTime();
    flow->updateEndTime(now);
    for (auto f : net->flows) {
        if (f->isReduced()) {
            f->updateEndTime(now);


            // update flows scheduling
            auto event = f->getEvent();
            f->sourceServer()->cancelEvent(event);
            f->sourceServer()->take(event);
            f->sourceServer()->scheduleAt(f->getEndTime(), event);
        }
    }

    // TODO update capacity for switches and links
    for (auto r : net->resources) {
        r->getNedComponent()->par("capacity").setDoubleValue(r->getCapacity());
    }


    // create event for omnetpp for new flow
    flow->sourceServer()->scheduleAt(flow->getEndTime(), flow->getEvent());

    //std::cout << "Flow added. Time: " << simTime() << endl;
    ev << simTime() << ": Flow added. Time: " << simTime() << endl;
}

void Server::removeFlow(Flow* flow) {

    auto it = std::find(net->flows.begin(), net->flows.end(), flow);
    if (it == net->flows.end()) {
        throw cRuntimeError("flow to be deleted was not found");
    }
    net->flows.erase(it);
    delete flow;

    Solver::solve(net->flows, net->resources);
    Solver::printFlows(net->flows);
    //Solver::printResources(net->resources);

    // find increased flows and recalculate end time for them
    simtime_t now = simTime();
    for (auto f : net->flows) {
        if (f->getEvent()->isName("DataMessage")) {
            // this type of flow is not rescheduled
            // since flow of this type is 'almost' delivered
            // by 'almost' I mean that all recalculations were done previously
            // another way to do this - erase flow from net->flows when FlowMessages delivered
            continue;
        }

        if (f->isIncreased()) {
            f->updateEndTime(now);


            // update flows scheduling
            auto event = f->getEvent();
            event->setSchedulingPriority(0);
            f->sourceServer()->cancelEvent(event);
            f->sourceServer()->take(event);
            f->sourceServer()->scheduleAt(f->getEndTime(), event);
        }
    }

    // TODO update capacity for switches and links
    for (auto r : net->resources) {
        r->getNedComponent()->par("capacity").setDoubleValue(r->getCapacity());
    }

    //std::cout << "Flow deleted. Time: " << simTime() << endl;
    ev << simTime() << ": Flow deleted. Time: " << simTime() << endl;
}

