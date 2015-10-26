/*
 * Network.cc
 *
 *  Created on: Oct 12, 2015
 *      Author: aarlan
 */
#include "Flow.h"
#include "Net.h"
#include "Resource.h"
#include "Server.h"
#include "Solver.h"

Define_Module(Net);

void Net::initialize() {
    resources.clear();
    flows.clear();

    routing = check_and_cast<Routing*>(getModuleByPath("routing"));


    scheduleAt(0, new cMessage("Test1", 0));
}

void Net::handleMessage(cMessage *msg) {
    Enter_Method_Silent();

    if (msg->isName("Test1")) {
        addFlow(1, 11, 0, 1, 100);
        addFlow(1, 11, 0, 1, 100);
        addFlow(1, 11, 0, 1, 50);
        addFlow(1, 11, 0, 1, 25);
    }


}

void Net::registerResource(Resource* r) {
    resources.push_back(r);
}

Resource* Net::getResourceByNedObject(const cComponent* const comp) {
    mutex.lock();

    if (comp == nullptr) {
        throw cRuntimeError("'comp' is null pointer");
    }

    Resource* ret = nullptr;
    for (auto r : resources) {
        if (r->getNedComponent() == comp) {
            ret = r;
            break;
        }
    }

    mutex.unlock();
    return ret;
}


std::vector<Resource*>* Net::getResourcePath(const int srcAddress, const int dstAddress) {
    auto srcServer = routing->getServerByAddress(srcAddress);
    auto dstServer = routing->getServerByAddress(dstAddress);

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
            Resource* rm = getResourceByNedObject(node->getModule());
            path->push_back(rm);
        }
        Resource* rc = getResourceByNedObject(channel);
        path->push_back(rc);

        node = link->getRemoteNode();
    }

    ev << "results:" << endl;
    for (auto r : *path) // TODO
        ev << (void*) r << " " << (void *) (r != nullptr ? r->getNedComponent() : nullptr) << endl;

    return path;
}



std::vector<int>* Net::getGatePath(const int srcAddress, const int dstAddress) {
    auto srcServer = routing->getServerByAddress(srcAddress);
    auto dstServer = routing->getServerByAddress(dstAddress);

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

void Net::addFlow(const int sourceAddress, const int destAddress,
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
    auto path = getResourcePath(sourceAddress, destAddress);
    auto gatePath = getGatePath(sourceAddress, destAddress);

    Flow* flow = new Flow(routing->getServerByAddress(sourceAddress), desiredAllocation, startTime, endTime, path, gatePath);

    // add flow to list of all flows
    flows.push_back(flow);

    Solver::solve(flows, resources);
    Solver::printFlows(flows);
    Solver::printResources(resources);

    // find reduced flows and recalculate end time for them
    for (auto f : flows) {
        if (f->isReduced()) {
            f->updateEndTime();


            // update flows scheduling
            auto event = f->getEvent();
            f->sourceServer()->cancelEvent(event);
            f->sourceServer()->scheduleAt(f->getEndTime(), event);
        }
    }

    // TODO update capacity for switches and links
    for (auto r : resources) {
        r->getNedComponent()->par("capacity").setDoubleValue(r->getCapacity());
    }


    // create event for omnetpp for new flow
    flow->sourceServer()->schedule(flow->getEndTime(), flow->getEvent());

    ev << simTime() << ": Flow added" <<  endl;
}

void Net::removeFlow(Flow* flow) {

    // since flow should be removed,
    // its allocation from all related resources should be freed
    double allocation = flow->getAllocation();
    for (auto r : *flow->getPath()) {
        r->changeCapacity(allocation);
    }


    auto it = std::find(flows.begin(), flows.end(), flow);
    if (it == flows.end()) {
        throw cRuntimeError("flow to be deleted was not found");
    }
    flows.erase(it);
    delete flow;


    Solver::solve(flows, resources);
    Solver::printFlows(flows);
    Solver::printResources(resources);

    // find increased flows and recalculate end time for them
    for (auto f : flows) {
        if (f->isIncreased()) {
            f->updateEndTime();


            // update flows scheduling
            auto event = f->getEvent();
            f->sourceServer()->cancelEvent(event);
            f->sourceServer()->scheduleAt(f->getEndTime(), event);
        }
    }
    ev << simTime() << ": Flow deleted" << endl;
}

