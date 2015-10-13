/*
 * Flow.h
 *
 *  Created on: Oct 12, 2015
 *      Author: aarlan
 */

#ifndef FLOW_H_
#define FLOW_H_


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

    Server sourceServer;
    Server destServer;

public:

    Flow(const long id, const double desiredAllocation, const std::vector<long>& path) {

        this->id = id;
        this->desiredAllocation = desiredAllocation;
        this->currentAllocation = 0.0;
        this->previousAllocation = 0.0;

    }

    Flow(long id, double demand, const long* path, const size_t path_size)
        : id(id), demand(demand) {

        allocation = 0.0;
        this->path.assign(path, path + path_size);
    }




    bool operator==(const Flow& f) {
        if (id != f.id)
            return false;

        // TODO what about other members?
        // It seems that id is enough for comparison
        return true;
    }

    long getId() const {
        return id;
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
        for (long node : path) {
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
