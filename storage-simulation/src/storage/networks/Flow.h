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
#include "Messages.h"
#include "Server.h"
#include "Resource.h"
#include <algorithm>
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
    
    void checkInvariant() const {
        if (desiredAllocation < currentAllocation)
            throw std::invalid_argument(
                    "desiredAllocation is less then currentAllocation");

        if (desiredAllocation < previousAllocation)
            throw std::invalid_argument(
                    "desiredAllocation is less then previousAllocation");

        if (desiredAllocation < 0)
            throw std::invalid_argument("desiredAllocation is negative");

        if (currentAllocation < 0)
            throw std::invalid_argument("currentAllocation is negative");

        if (previousAllocation < 0)
            throw std::invalid_argument("previousAllocation is negative");

    }

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
        event = new cMessage("FlowMessage", MessageType::FlowMessage);
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

    static bool compByFairShare(const Flow* first, const Flow* second) {
        const auto p1 = first->getPath();
        const Resource* minR1 = *std::min_element(p1->begin(), p1->end(), Resource::comp2);
        const double fs1 = minR1->getFairCapacity();

        const auto p2 = second->getPath();
        const Resource* minR2 = *std::min_element(p2->begin(), p2->end(), Resource::comp2);
        const double fs2 = minR2->getFairCapacity();
        return fs1 < fs2;
    }

    static bool compByFairShareDemand(const Flow* first, const Flow* second) {
        const auto p1 = first->getPath();
        const Resource* minR1 = *std::min_element(p1->begin(), p1->end(), Resource::comp2);
        const double fsd1 = minR1->getFairCapacity()/first->getDemand();

        const auto p2 = second->getPath();
        const Resource* minR2 = *std::min_element(p2->begin(), p2->end(), Resource::comp2);
        const double fsd2 = minR2->getFairCapacity()/second->getDemand();
        // we want flows which are thin (share/demand >=1 or share >= demand) to be first
        return fsd1 > fsd2;
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
        checkInvariant();
        // allocation value to be set always should be between 0 and desiredAllocation
        if (allocation > desiredAllocation) {
            throw cRuntimeError("allocation is greater than demand");
        }
        if (allocation == 0) {
            throw cRuntimeError("allocation is equal to zero");
        }
        previousAllocation = currentAllocation;
        currentAllocation = allocation;
        checkInvariant();
    }

    bool isReduced() const {
        checkInvariant();
        const double diff = previousAllocation - currentAllocation;
        return diff > 0;
    }

    bool isIncreased() const {
        checkInvariant();
        const double diff = previousAllocation - currentAllocation;
        return diff < 0;
    }

};




#endif /* FLOW_H_ */
