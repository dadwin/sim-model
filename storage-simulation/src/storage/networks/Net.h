/*
 * Network.h
 *
 *  Created on: Oct 12, 2015
 *      Author: aarlan
 */

#ifndef NETWORK_H_
#define NETWORK_H_

#include <map>
#include <mutex>
#include <vector>
#include <omnetpp.h>
#include "Routing.h"

class cComponent;
class Resource;
class Flow;

class Net : public cSimpleModule {

public:

    struct FlowParameters {
        double demand;
        simtime_t start;
        simtime_t end;
        int source;
        int destination;

        FlowParameters(double demand, simtime_t start, simtime_t end, int source, int destination) {
            this->demand = demand;
            this->start = start;
            this->end = end;
            this->source = source;
            this->destination = destination;
        }
    };

    void initialize();
    void handleMessage(cMessage *msg);

    void registerResource(Resource* r);
    Resource* getResourceByNedObject(const cComponent* const comp);

    std::vector<Resource*>* getResourcePath(const int srcAddress, const int dstAddress);
    std::vector<int>* getGatePath(const int srcAddress, const int dstAddress);

    void addFlow(const int sourceAddress, const int destAddress,
            const simtime_t startTime, const simtime_t endTime,
            const double desiredAllocation);
    void removeFlow(Flow* flow);
    std::vector<Net::FlowParameters*> getFlowProfile(const int sourceAddress) const;

protected:
    Routing* routing;
    std::mutex mutex;
    std::multimap<int, FlowParameters*> flowProfile;

    void addFlowParameters(FlowParameters* fp);
public:
    std::vector<Resource*> resources; // TODO it can be map <cComponent*, Resource*>
    std::vector<Flow*> flows;
};

#endif /* NETWORK_H_ */
