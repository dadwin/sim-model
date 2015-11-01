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
    double desiredVolume;
    double currentVolume;

    simtime_t startTime;
    simtime_t endTime;
    Server* srcServer; // owner of flow
    //Server* dstServer;
    int srcAddress; // NOTE addresses can be parameters of Server,
    int dstAddress; // and be retrieved through par("address")

    cMessage* event; // NOTE don't delete it in handleMessage()
    std::vector<Resource*>* path;
    std::vector<int>* gatePath;

public:

    Flow(Server* sourceServer, const double desiredAllocation, const simtime_t startTime, const simtime_t endTime, std::vector<Resource*>* path, std::vector<int>* gatePath) {

        this->desiredAllocation = desiredAllocation;
        this->currentAllocation = 0.0;
        this->previousAllocation = 0.0;
        this->startTime = startTime;
        this->endTime = endTime;
        this->path = path;
        this->gatePath = gatePath;
        this->srcServer = sourceServer;
        desiredVolume = desiredAllocation*(endTime - startTime).dbl();
        currentVolume = 0.0;

        // when event is delivered, module knows about flow via the Flow parameter
        event = new cMessage("FlowMessage", 0);
        auto par = new cMsgPar("flow");
        par->setPointerValue(this);
        event->addPar(par);
    }

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

    void updateEndTime(const simtime_t now) {
        if (currentVolume < 0 || currentVolume > desiredVolume)
            throw cRuntimeError("invalid currentVolume for flow before updating");

        if (startTime > now || now > endTime)
            throw cRuntimeError("invalid times before for flow updating");

        // here we need to understand what amount of flow's data remains to transmit
        const double lastPortion = previousAllocation*(now - startTime).dbl();
        currentVolume += lastPortion;

        const double restVolume = desiredVolume - currentVolume;
        endTime = now + restVolume/currentAllocation;
        startTime = now;

        if (currentVolume < 0 || currentVolume > desiredVolume)
            throw cRuntimeError("invalid currentVolume for flow after updating");

        if (startTime > now || now > endTime)
            throw cRuntimeError("invalid times after for flow updating");
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
        // TODO what about other members?
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
        // allocation value to be set always should be between 0 and desiredAllocation
        if (allocation > desiredAllocation) {
            throw cRuntimeError("allocation is greater than demand");
        }
        if (allocation == 0) {
            throw cRuntimeError("allocation is equal to zero");
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
