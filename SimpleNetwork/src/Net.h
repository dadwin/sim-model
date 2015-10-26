/*
 * Network.h
 *
 *  Created on: Oct 12, 2015
 *      Author: aarlan
 */

#ifndef NETWORK_H_
#define NETWORK_H_

#include <mutex>
#include <vector>
#include <omnetpp.h>
#include "Routing.h"

class cComponent;
class Resource;
class Flow;

class Net : public cSimpleModule {

public:
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

protected:
    Routing* routing;
    std::mutex mutex;
    std::vector<Resource*> resources; // TODO it can be map <cComponent*, Resource*>
    std::vector<Flow*> flows;
};

#endif /* NETWORK_H_ */
