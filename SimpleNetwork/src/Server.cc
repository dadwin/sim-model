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
        Resource* resource = new LinkResource(channel, 0, maxCapacity); // TODO what about id? can it be 'this' pointer?
        net->registerResource(resource);
    }


    if (address == 1) {
        // TODO for testing
        scheduleAt(0, new cMessage("Test1", 0));
    }
    if (address == 2) {
        scheduleAt(2, new cMessage("Test1", 0));
    }
    if (address == 5) {
        scheduleAt(3, new cMessage("Test1", 0));
    }
}


void Server::handleMessage(cMessage *msg)
{
    std::cout << getFullPath() << endl;
    if (msg->isSelfMessage()) {
        handleSelfMessage(msg);
        return;
    }

    if (msg->isName("FlowMessage")) {
        // TODO what should we do here?
    }

    if (msg->isName("DataMessage")) {
        // message arrived!
        ev << "message arrived!" << endl;

        Flow* flow = (Flow*) msg->par("flow").pointerValue();
        removeFlow(flow);

        //delete msg; // NOTE Message is part of Flow and Flow is owner of it
    }
}

void Server::handleSelfMessage(cMessage *msg)
{
    if (msg->isName("FlowMessage")) {
        // flow is ended, we need to transmit message to destination server
        // first we need to calculate path along which message will be transmitted


        Flow* flow = (Flow*) msg->par("flow").pointerValue();

        const int gateId = flow->getNextGateId();
        msg->setName("DataMessage");
        send(msg, gateId);
    }

    if (msg->isName("Test1")) {
        addFlow();
        delete msg;
    }
}


void Server::addFlow() {

    // Input parameters for flow:
    // * source server - a node from which flow runs
    // * destination server - a node toward which flow runs
    // * start time - time when flow starts running
    // * desired allocation - demand of capacity of network resources that flow wants to obtain
    // * end time - time when flow ends running, if its allocation is equal to desired allocation.
    // Relation between times and allocation is as following:
    //     (end time - start time)*allocation = constant
    // This constant is a amount of data transferred by a flow.

    //Server src;
    //Server dst;


    // calculate path from src to dst
    // path is sequence of cModules with @node property
    auto path = net->getResourcePath(address, 11);
    auto gatePath = net->getGatePath(address, 11);

    double desiredAllocation = 10.0;

    simtime_t startTime = 0.0;
    simtime_t endTime = 1.0;

    Flow* flow = new Flow(desiredAllocation, startTime, endTime, path, gatePath);


    // add flow to list of all flows
    network.flows.push_back(flow);

    Solver::solve(network.flows, network.resources);
    Solver::printFlows(network.flows);
    Solver::printResources(network.resources);

    // find reduced flows and recalculate end time for them
    for (auto f : network.flows) {
        if (f->isReduced()) {
            f->updateEndTime();


            // update flows scheduling
            auto event = f->getEvent();
            f->sourceServer()->cancelEvent(event);
            f->sourceServer()->scheduleAt(simTime() + f->getEndTime(), event);
        }
    }

    // TODO update capacity for switches and links
    for (auto r : network.resources) {
        r->getNedComponent()->par("capacity").setDoubleValue(r->getCapacity());
    }


    // create event for omnetpp for new flow
    scheduleAt(simTime() + flow->getEndTime(), flow->getEvent());
}

void Server::removeFlow(Flow* flow) {

    double allocation = flow->getAllocation();
    for (auto r : *flow->getPath()) {
        r->changeCapacity(allocation);
    }


    auto it = std::find(network.flows.begin(), network.flows.end(), flow);
    if (it == network.flows.end()) {
        throw cRuntimeError("impossible error");
    }
    network.flows.erase(it);
    delete flow;


    Solver::solve(network.flows, network.resources);
    Solver::printFlows(network.flows);
    Solver::printResources(network.resources);

    // find increased flows and recalculate end time for them
    for (auto f : network.flows) {
        if (f->isIncreased()) {
            f->updateEndTime();


            // update flows scheduling
            auto event = f->getEvent();
            f->sourceServer()->cancelEvent(event);
            f->sourceServer()->scheduleAt(simTime() + f->getEndTime(), event);
        }
    }
}

