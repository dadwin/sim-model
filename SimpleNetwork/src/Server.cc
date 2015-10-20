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
#include <cexception.h>

#include <algorithm>
#include <map>

Define_Module(Server);


static int lastAddress = 0;
static std::map<int, Server*> addressMapping;

int Server::getAddress() const {
    return address;
}

void Server::initialize()
{

    // set address
    address = ++lastAddress; // TODO what about concurrency safety?
    // add to addressMapping in order to find it later quickly
    addressMapping.insert(std::pair<int, Server*>(address, this));

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
        network.registerResource(resource);
    }


    if (address == 11) {
        // TODO for testing
        scheduleAt(0, new cMessage("getResoursePath", 0));
    }


}

Server* Server::getServerByAddress(const int address) {
    // TODO what about input argument checking?
    auto it = addressMapping.find(address);
    if (it == addressMapping.end()) {
        return nullptr;
    } else {
        return it->second;
    }
}

std::vector<Resource*>* Server::getResourcePath(const int srcAddress, const int dstAddress) {
    auto srcServer = Server::getServerByAddress(srcAddress);
    auto dstServer = Server::getServerByAddress(dstAddress);

    if (srcServer == nullptr) {
        throw cRuntimeError("no server for source address: %d", srcAddress);
    }

    if (dstServer == nullptr) {
        throw cRuntimeError("no server for destination address: %d", dstAddress);
    }

    std::vector<Resource*>* path = new std::vector<Resource*>();

    cTopology topo;
    topo.extractByProperty("node");
    ev << topo.getNumNodes() << " topology size\n";

    cTopology::Node* dstNode = topo.getNodeFor(dstServer);
    cTopology::Node* srcNode = topo.getNodeFor(srcServer);
    topo.calculateUnweightedSingleShortestPathsTo(dstNode);

    if (srcNode->getNumPaths() == 0) {
        ev <<  "server:" << srcAddress << " and server:" << dstAddress << " not connected" << endl;
        return path;
    }

    cTopology::Node *node = srcNode;
    ev << "path from node " << node->getModule()->getFullPath() << endl;
    ev << "path   to node " << dstNode->getModule()->getFullPath() << endl;
    while (node != topo.getTargetNode()) {
        if (node->getNumPaths() < 1) {
            throw cRuntimeError("no path to destination server:%d", dstAddress);
        }
        ev << "We are in " << node->getModule()->getFullPath() << endl;
        // TODO we can verify that hops == 5 : node->getDistanceToTarget() == 5

        cTopology::LinkOut *link = node->getPath(0);
        auto localGate = link->getLocalGate();
        auto channel = localGate->getChannel() != nullptr ? localGate->getChannel() : localGate->getNextGate()->getChannel();
        if (channel == nullptr) {
            throw cRuntimeError("null pointer of channel for local gate %s", localGate->getFullName());
        }
        ev << "channel: " << (void*) channel << " module: " << (void*) node->getModule() << endl;


        if (node != srcNode) {
            // to skip source node and destination one
            Resource* rm = network.getResourceByNedObject(node->getModule());
            path->push_back(rm);
        }
        Resource* rc = network.getResourceByNedObject(channel);
        path->push_back(rc);

        node = link->getRemoteNode();
    }

    ev << "results:" << endl;
//    std::reverse(gatesIDs->begin(), gatesIDs->end());
    for (auto r : *path) // TODO
        ev << (void*) r << " " << (void *) (r != nullptr ? r->getNedObj() : nullptr) << endl;


    return path;
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

    if (msg->isName("getResoursePath")) {

        ev << "Resources:" << endl;
        for (auto r : network.resources) {
            ev << (void*) r << " " << (void *) r->getNedObj() << " " << r->getNedObj()->getFullPath() << endl;
        }


        auto path = getResourcePath(1, address);
        auto path1 = getGatePath(1, address);
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


            // update flows scheduling
            auto event = f->getEvent();
            f->sourceServer()->cancelEvent(event);
            f->sourceServer()->scheduleAt(f->getEndTime(), event);
        }
    }

    // create event for omnetpp for new flow
    scheduleAt(flow->getEndTime(), flow->getEvent());

    // TODO update capacity for switches and links


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


std::vector<int>* Server::getGatePath(const int srcAddress, const int dstAddress) {
    auto srcServer = Server::getServerByAddress(srcAddress);
    auto dstServer = Server::getServerByAddress(dstAddress);

    if (srcServer == nullptr) {
        throw cRuntimeError("no server for source address: %d", srcAddress);
    }

    if (dstServer == nullptr) {
        throw cRuntimeError("no server for destination address: %d", dstAddress);
    }

    std::vector<int>* path = new std::vector<int>();

    cTopology topo;
    topo.extractByProperty("node");
    ev << topo.getNumNodes() << " topology size\n";

    cTopology::Node* dstNode = topo.getNodeFor(dstServer);
    cTopology::Node* srcNode = topo.getNodeFor(srcServer);
    topo.calculateUnweightedSingleShortestPathsTo(dstNode);

    if (srcNode->getNumPaths() == 0) {
        ev <<  "server:" << srcAddress << " and server:" << dstAddress << " not connected" << endl;
        return path;
    }

    cTopology::Node *node = srcNode;
    ev << "path from node " << node->getModule()->getFullPath() << endl;
    ev << "path   to node " << dstNode->getModule()->getFullPath() << endl;
    while (node != topo.getTargetNode()) {
        if (node->getNumPaths() < 1) {
            throw cRuntimeError("no path to destination server:%d", dstAddress);
        }
        ev << "We are in " << node->getModule()->getFullPath() << endl;
        // TODO we can verify that hops == 5 : node->getDistanceToTarget() == 5

        cTopology::LinkOut *link = node->getPath(0);
        path->push_back(link->getLocalGate()->getId());

        node = link->getRemoteNode();
    }

    ev << "results:" << endl;
//    std::reverse(gatesIDs->begin(), gatesIDs->end());
    for (auto g : *path) // TODO
        ev << g << endl;


    return path;
}
