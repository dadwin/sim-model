/*
 * Flow.h
 *
 *  Created on: Oct 12, 2015
 *      Author: aarlan
 */

#ifndef FLOW_H_
#define FLOW_H_

#include <simtime.h>
#include <cmessage.h>
#include "Server.h"
#include "Resource.h"
#include <vector>

class Flow {

protected:

    double desiredAllocation;
    double previousAllocation;
    double currentAllocation;

    simtime_t startTime;
    simtime_t endTime;
    //cSimpleModule* owner;
    Server* srcServer; // owner of flow
    Server* dstServer;
    int srcAddress; // NOTE addresses can be parameters of Server,
    int dstAddress; // and be retrieved through par("address")

    cMessage* event; // NOTE don't delete it in handleMessage()
    std::vector<Resource*>* path;
    std::vector<int>* gatePath;

public:

    Flow(const double desiredAllocation, const simtime_t startTime, const simtime_t endTime, std::vector<Resource*>* path, std::vector<int>* gatePath) {

        //this->id = id; // TODO is it needed?
        this->desiredAllocation = desiredAllocation;
        this->currentAllocation = 0.0;
        this->previousAllocation = 0.0;
        this->startTime = startTime;
        this->endTime = endTime;
        this->path = path;
        this->gatePath = gatePath;

        // when event is delivered, module knows about flow via the Flow parameter
        event = new cMessage("FlowMessage", 0);
        auto par = new cMsgPar("flow");
        par->setPointerValue(this);
        event->addPar(par);
    }
/*
    Flow(long id, double demand, const long* path, const size_t path_size)
        : demand(demand) {

        allocation = 0.0;
        this->path.assign(path, path + path_size);

        event = new cMessage("FlowMessage", 0);
    }
*/

    ~Flow() {
        delete event;
    }

    cMessage* getEvent() const {
        return event;
    }

    simtime_t getStartTime() const {
        return startTime;
    }

    simtime_t getEndTime() const {
        return endTime;
    }

    Server* sourceServer() const {
        return srcServer;
    }

    int getSrcAddress() const {
        return srcAddress;
    }

    int getDstAddress() const {
        return dstAddress;
    }

    void updateEndTime() {
        simtime_t newEndTime;
        if (previousAllocation == 0) {
            // this flow just added to network
            newEndTime = startTime + (endTime - startTime)*desiredAllocation/currentAllocation;
        } else {
            newEndTime = startTime + (endTime - startTime)*previousAllocation/currentAllocation;
        }
        endTime = newEndTime;
    }

    int getNextGateId() const {
        if (gatePath->size() == 0)
            throw cRuntimeError("gatePath is zero-sized");

        int nextGateId = gatePath->front();
        gatePath->erase(gatePath->begin());
        return nextGateId;
    }


    bool operator==(const Flow& f) {
        return this == &f;
//        if (id != f.id)
//            return false;

        // TODO what about other members?
        // It seems that id is enough for comparison
        return true;
    }

    long getId() const {
        //return id;
        throw std::invalid_argument("bad method invocation");
        return 0;
    }

    double getDemand() const {
        return desiredAllocation;
    }

    const std::vector<Resource*>* getPath() const {
        return path;
    }

    double getAllocation() const {
        return currentAllocation;
    }

    void setAllocation(double allocation) {
        if (allocation > desiredAllocation) {
            throw cRuntimeError("allocation is greater than demand");
        }
        previousAllocation = currentAllocation;
        currentAllocation = allocation;
    }

    bool runThroughResource(const Resource* resource) const {
        bool running = false;

        if (resource == nullptr)
            return running;

        for (auto r : *path) {
            if (r == resource) {
                running = true;
                break;
            }
        }
        return running;
    }

    bool isReduced() const {
        const double diff = previousAllocation - currentAllocation;
        return diff > 0;
    }

    bool isIncreased() const {
        const double diff = previousAllocation - currentAllocation;
        return diff < 0;
    }

};




#endif /* FLOW_H_ */
