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
#include "Network.h"
#include "Solver.h"
#include <algorithm>

Define_Module(Server);

void Server::initialize()
{
}

void Server::handleMessage(cMessage *msg)
{
    if (msg->isSelfMessage()) {
        handleSelfMessage(msg);
        return;
    }

    if (msg->isName("FlowMessage")) {
        // TODO what should we do here?
    }
}

void Server::handleSelfMessage(cMessage *msg)
{
    if (msg->isName("FlowMessage")) {
        // flow is ended, we need to transmit message to destination server
        // first we need to calculate path along which message will be transmitted


        // TODO calculate path


        const int gateId = 0; // TODO get first gateId
        send(msg, gateId);
    }
}


void Server::addFlow() {

    Server src;
    Server dst;


    // calculate path from src to dst
    // path is sequence of cModules with @resource property

    std::vector<long> path;
    double desiredAllocation = 10.0; // TODO

    simtime_t startTime;
    simtime_t endTime;

    Flow* flow = new Flow(0/*id*/, desiredAllocation, path);


    // add flow to list of all flows
    network.flows.push_back(flow);

    Solver::solve(network.flows, network.resources);

    // find reduced flows and recalculate end time for them
    for (auto f : network.flows) {
        if (f->isReduced()) {
            f->updateEndTime();

            auto event = f->getEvent();
            f->sourceServer()->cancelEvent(event);
            f->sourceServer()->scheduleAt(f->getEndTime(), event);
        }
    }
    // update flows scheduling

    // create event for omnetpp for new flow
    scheduleAt(flow->getEndTime(), flow->getEvent());




}

void Server::removeFlow() {

}


std::vector<int>* calculatePathBetweenTwoModules(cModule* const srcModule, cModule* const dstModule) {
    std::vector<int>* gatesIDs = new std::vector<int>();

    cTopology topo;
    topo.extractByProperty("node");
    ev << topo.getNumNodes() << " topology size\n";

    cTopology::Node* dstNode = topo.getNodeFor(dstModule);
    cTopology::Node* srcNode = topo.getNodeFor(srcModule);
    topo.calculateUnweightedSingleShortestPathsTo(dstNode);

    if (srcNode->getNumPaths() == 0) {
        ev << "not connected\n";
        return gatesIDs;
    }

    cTopology::Node *node = srcNode;
    ev << "path from node " << node->getModule()->getFullPath() << endl;
    ev << "path   to node " << dstNode->getModule()->getFullPath() << endl;
    while (node != topo.getTargetNode()) {
        ev << "We are in " << node->getModule()->getFullPath() << endl;
        ev << node->getDistanceToTarget() << " hops to go\n";
        ev << "There are " << node->getNumPaths()
                << " equally good directions, taking the first one\n";
        cTopology::LinkOut *path = node->getPath(0);

        gatesIDs->push_back(path->getLocalGate()->getId());

        ev << "Taking gate " << path->getLocalGate()->getFullName()
                << " id:" << path->getLocalGate()->getId()
                << " we arrive in "
                << path->getRemoteNode()->getModule()->getFullPath()
                << " on its gate " << path->getRemoteGate()->getFullName()
                << " id:" << path->getRemoteGate()->getId()
                << endl;
        node = path->getRemoteNode();

    }

    std::reverse(gatesIDs->begin(), gatesIDs->end());
    for (auto i : *gatesIDs) // TODO
        ev << i << endl;

    return gatesIDs;
}
