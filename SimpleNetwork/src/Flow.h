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

class Flow {

protected:
    double demand;
    std::vector<long> path;
    double allocation;

    double desiredAllocation;
    double previousAllocation;
    double currentAllocation;

    simtime_t startTime;
    simtime_t endTime;
    //cSimpleModule* owner;
    Server* srcServer; // owner of flow
    Server* dstServer;

    cMessage* event;

public:



    Flow(const long id, const double desiredAllocation, const std::vector<long>& path) {

        //this->id = id; // TODO is it needed?
        this->desiredAllocation = desiredAllocation;
        this->currentAllocation = 0.0;
        this->previousAllocation = 0.0;

        // when event is delivered, module knows about flow via the Flow parameter
        event = new cMessage("FlowMessage", 0);
        auto par = new cMsgPar("Flow");
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

    void updateEndTime() {
        simtime_t newEndTime = startTime + (endTime - startTime)*currentAllocation/desiredAllocation;
        endTime = newEndTime;
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
        throw std::invalid_argument("bad method invokation");
        return 0;
    }

    double getDemand() const {
        return demand;
    }

    const std::vector<long> getPath() const {
        return path;
    }

    double getAllocation() const {
        return allocation;
    }

    void setAllocation(double allocation) {
        if (allocation > demand) {
            throw std::invalid_argument("allocation is greater than demand");
        }
        this->allocation = allocation;
    }

    bool runThroughResource(const Resource* r) const {
        bool running = false;
        for (auto node : path) {
            if (node == r->getId()) {
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
