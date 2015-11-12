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
#include <string>

Define_Module(Net);

template<typename T>
T getValueFromXmlTag(const cXMLElement* xmlElement, const char* tagName) {
    if (tagName == nullptr)
        throw cRuntimeError("null tagName");

    cXMLElement* tag = xmlElement->getFirstChildWithTag(tagName);

    if (tag == nullptr)
        throw cRuntimeError("XML read error: %s type is missing", tagName);

    std::string value = tag->getNodeValue();
    return stod(value);
}

void Net::initialize() {
    resources.clear();
    flows.clear();

    routing = check_and_cast<Routing*>(getModuleByPath("routing"));
    flowProfile.clear();

    // reading flow profile
    cXMLElement* root = par("flowprofile").xmlValue();
    cXMLElementList flows = root->getChildrenByTagName("flow");
    for (auto it = flows.cbegin(); it != flows.cend(); it++) {
        const cXMLElement* flow = *it;

        const double demand = getValueFromXmlTag<double>(flow, "demand");
        const double start = getValueFromXmlTag<double>(flow, "startTime");
        const double end = getValueFromXmlTag<double>(flow, "endTime");
        const int source = getValueFromXmlTag<int>(flow, "source");
        const int destination = getValueFromXmlTag<int>(flow, "destination");

        auto fp = new FlowParameters(demand, start, end, source, destination);
        addFlowParameters(fp);
    }
}

void Net::addFlowParameters(FlowParameters* fp) {
    if (fp == nullptr)
        throw cRuntimeError("null flowParameter");

    flowProfile.insert(std::pair<int, FlowParameters*>(fp->source, fp));
}

std::vector<Net::FlowParameters*> Net::getFlowProfile(const int sourceAddress) const {

    std::vector<FlowParameters*> list;
    for (auto it = flowProfile.cbegin(); it != flowProfile.cend(); ++it) {
        if ((*it).first == sourceAddress) {
            list.push_back((*it).second);
        }
    }
    return list;
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

std::vector<Resource*>* Net::getResourcePath(const int srcAddress,
        const int dstAddress) {
    auto srcServer = routing->getServerByAddress(srcAddress);
    auto dstServer = routing->getServerByAddress(dstAddress);

    if (srcServer == nullptr) {
        throw cRuntimeError("no server for source address: %d", srcAddress);
    }

    if (dstServer == nullptr) {
        throw cRuntimeError("no server for destination address: %d",
                dstAddress);
    }

    std::vector<Resource*>* path = new std::vector<Resource*>();

    cTopology topo;
    topo.extractByProperty("node");
#ifdef DEBUG
    ev << topo.getNumNodes() << " topology size\n";
#endif

    cTopology::Node* dstNode = topo.getNodeFor(dstServer);
    cTopology::Node* srcNode = topo.getNodeFor(srcServer);
    topo.calculateUnweightedSingleShortestPathsTo(dstNode);

    if (srcNode->getNumPaths() == 0) {
        ev << "server:" << srcAddress << " and server:" << dstAddress
                << " not connected" << endl;
        return path;
    }

    cTopology::Node *node = srcNode;
#ifdef DEBUG
    ev << "path from node " << node->getModule()->getFullPath() << endl;
    ev << "path   to node " << dstNode->getModule()->getFullPath() << endl;
#endif

    while (node != topo.getTargetNode()) {
        if (node->getNumPaths() < 1) {
            throw cRuntimeError("no path to destination server:%d", dstAddress);
        }
#ifdef DEBUG
        ev << "We are in " << node->getModule()->getFullPath() << endl;
#endif
        // TODO we can verify that hops == 5 : node->getDistanceToTarget() == 5

        cTopology::LinkOut *link = node->getPath(0);
        auto localGate = link->getLocalGate();
        auto channel =
                localGate->getChannel() != nullptr ?
                        localGate->getChannel() :
                        localGate->getNextGate()->getChannel();
        if (channel == nullptr) {
            throw cRuntimeError("null pointer of channel for local gate %s",
                    localGate->getFullName());
        }
#ifdef DEBUG
        ev << "channel: " << (void*) channel << " module: "
                << (void*) node->getModule() << endl;
#endif

        if (node != srcNode) {
            // to skip source node and destination one
            Resource* rm = getResourceByNedObject(node->getModule());
            path->push_back(rm);
        }
        Resource* rc = getResourceByNedObject(channel);
        path->push_back(rc);

        node = link->getRemoteNode();
    }
#ifdef DEBUG
    ev << "results:" << endl;
    for (auto r : *path)
        ev << (void*) r << " "
                << (void *) (r != nullptr ? r->getNedComponent() : nullptr)
                << endl;
#endif
    return path;
}

std::vector<int>* Net::getGatePath(const int srcAddress, const int dstAddress) {
    auto srcServer = routing->getServerByAddress(srcAddress);
    auto dstServer = routing->getServerByAddress(dstAddress);

    if (srcServer == nullptr) {
        throw cRuntimeError("no server for source address: %d", srcAddress);
    }

    if (dstServer == nullptr) {
        throw cRuntimeError("no server for destination address: %d",
                dstAddress);
    }

    std::vector<int>* path = new std::vector<int>();

    cTopology topo;
    topo.extractByProperty("node");
#ifdef DEBUG
    ev << topo.getNumNodes() << " topology size\n";
#endif

    cTopology::Node* dstNode = topo.getNodeFor(dstServer);
    cTopology::Node* srcNode = topo.getNodeFor(srcServer);
    topo.calculateUnweightedSingleShortestPathsTo(dstNode);

    if (srcNode->getNumPaths() == 0) {
        ev << "server:" << srcAddress << " and server:" << dstAddress
                << " not connected" << endl;
        return path;
    }

    cTopology::Node *node = srcNode;
#ifdef DEBUG
    ev << "path from node " << node->getModule()->getFullPath() << endl;
    ev << "path   to node " << dstNode->getModule()->getFullPath() << endl;
#endif

    while (node != topo.getTargetNode()) {
        if (node->getNumPaths() < 1) {
            throw cRuntimeError("no path to destination server:%d", dstAddress);
        }
#ifdef DEBUG
        ev << "We are in " << node->getModule()->getFullPath() << endl;
#endif
        // TODO we can verify that hops == 5 : node->getDistanceToTarget() == 5

        cTopology::LinkOut *link = node->getPath(0);
        path->push_back(link->getLocalGate()->getId());

        node = link->getRemoteNode();
    }

#ifdef DEBUG
    ev << "results:" << endl;
    for (auto g : *path)
        ev << g << endl;
#endif

    return path;
}

#if 0
void Net::addFlow(const int sourceAddress, const int destAddress,
        const simtime_t startTime, const simtime_t endTime,
        const double desiredAllocation) {

    Enter_Method_Silent();

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

    ev << simTime() << ": Flow added" << endl;
}

void Net::removeFlow(Flow* flow) {

    Enter_Method_Silent();

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
#endif
