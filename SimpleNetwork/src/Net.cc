/*
 * Network.cc
 *
 *  Created on: Oct 12, 2015
 *      Author: aarlan
 */
#include "Net.h"
#include "Resource.h"
#include "Server.h"
#include <ctopology.h>

Define_Module(Net);

void Net::initialize() {
    resources.clear();
    flows.clear();

    routing = check_and_cast<Routing*>(getModuleByPath("routing"));
}

void Net::handleMessage(cMessage *msg) {
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
//    std::reverse(gatesIDs->begin(), gatesIDs->end());
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
