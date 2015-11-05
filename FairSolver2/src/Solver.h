/*
 * Solver.h
 *
 *  Created on: Nov 3, 2015
 *      Author: aarlan
 */

#ifndef SOLVER_H_
#define SOLVER_H_

#include <vector>
#include <algorithm>
#include <iostream>
#include <limits>
#include <stdexcept>

class Resource {
public:

    Resource(double maxCapacity) {
        this->capacity = maxCapacity;
        this->maxCapacity = maxCapacity;
        this->flows = 0;
        this->flows_allotted = 0;
    }

    static bool comp1(const Resource* first, const Resource* second) {
        double cn1 = first->maxCapacity / (double) first->flows;
        double cn2 = second->maxCapacity / (double) second->flows;
        return cn1 < cn2;
    }

    static bool comp2(const Resource* first, const Resource* second) {
        double n1 = first->flows - first->flows_allotted;
        double n2 = second->flows - second->flows_allotted;
        double cn1 = first->capacity / n1;
        double cn2 = second->capacity / n2;
        return cn1 < cn2;
    }

    double getFairMaxCapacity() const {
        return maxCapacity / (double) flows;
    }

    double getFairCapacity() const {
        const double n = flows - flows_allotted;
        return capacity / n;
    }

    void countFlow() {
        checkInvariant();
        flows++;
        checkInvariant();
    }

    void countAllottedFlow() {
        checkInvariant();
        flows_allotted++;
        checkInvariant();
    }

    double getMaxCapacity() const {
        return maxCapacity;
    }

    double getCapacity() const {
        return capacity;
    }

    void decreaseCapacity(const double delta) {
        checkInvariant();
        if (delta < 0)
            throw std::invalid_argument("delta is negative");

        if (capacity - delta < 0) {
            throw std::invalid_argument("decreasing capacity down to negative value");
        }
        capacity -= delta;
        //flows_allotted++; // TODO is it OK?
        checkInvariant();
    }

    void setMaxCapacity() {
        checkInvariant();
        capacity = maxCapacity;

        flows_allotted = 0; // TODO is it OK?
        checkInvariant();
    }

    void resetCounts() {
        checkInvariant();
        flows = 0;
        checkInvariant();
    }

//    void addFlow(Flow* f) {
//        fs.push_back(f);
//    }
//
//    void removeFlow(Flow* f) {
//        auto fPos = std::remove(notAllottedFlows.begin(), notAllottedFlows.end(), f);
//        fs.erase(fPos);
//    }
//
//    const std::vector<Flow*>& flows() const {
//        return fs;
//    }

protected:
    double maxCapacity;
    double capacity;
    int flows;
    int flows_allotted;
//    std::vector<Flow*> fs;

    void checkInvariant() const {
        if (flows < flows_allotted)
            throw std::invalid_argument("flows is less than flows_allotted");

        if (maxCapacity < capacity)
            throw std::invalid_argument("maxCapacity is less than capacity");

        if (flows < 0)
            throw std::invalid_argument("flows is negative");

        if (flows_allotted < 0)
            throw std::invalid_argument("flows_allotted is negative");

        if (capacity < 0)
            throw std::invalid_argument("capacity is negative");

        if (maxCapacity < 0)
            throw std::invalid_argument("maxCapacity is negative");

    }
};

class Flow {
public:

    Flow(const double desiredAllocation, std::vector<Resource*>* path) {
        this->desiredAllocation = desiredAllocation;
        this->currentAllocation = 0;
        this->previousAllocation = 0;
        this->path = path;
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

    void setAllocation(const double allocation) {
        checkInvariant();
        // allocation value to be set always should be between 0 and desiredAllocation
        if (allocation > desiredAllocation) {
            throw std::invalid_argument("allocation is greater than demand");
        }
        if (allocation == 0) {
            throw std::invalid_argument("allocation is equal to zero");
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

protected:

    double desiredAllocation;
    double previousAllocation;
    double currentAllocation;
    std::vector<Resource*>* path;

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

};

class Solver {
protected:
        std::vector<Flow*> inputFlows;
        std::vector<Resource*> inputResources;

public:

    static void solve(const std::vector<Flow*>& inputFlows,
            const std::vector<Resource*>& resources) {

        // initialization
        // TODO it's full initialization, probably it should be smarter :)
        for (auto r : resources) {
            r->setMaxCapacity();
            r->resetCounts();
        }

        for (Flow* f : inputFlows) {
            auto path = f->getPath();

            for (Resource* r : *path) {
                r->countFlow();
            }
        }
        // initialization end

        std::vector<Flow*> flows(inputFlows);

        size_t currentSize, previousSize;
        do {
            previousSize = flows.size();

            for (auto it = flows.begin(); it != flows.end(); /* nothing */) {

                Flow* f = *it;

                auto path = f->getPath();
                const Resource* minR = *std::min_element(path->begin(), path->end(), Resource::comp2);

                if (f->getDemand() <= minR->getFairCapacity()) {
                    f->setAllocation(f->getDemand());

                    for (auto r : *path) {
                        r->countAllottedFlow();
                        r->decreaseCapacity(f->getDemand());
                    }

                    it = flows.erase(it);
                } else {
                    ++it;
                }
            }

            currentSize = flows.size();
            // stop after for-loop can't make a flow alloted
            // if all flows become allotted, then both currentSize and previousSize are equal to zero and it's OK.
        } while (previousSize != currentSize);

        if (flows.size() == 0) {
            // OK, all flows are allocated, exit
            return;
        }

        // now we have flows all of that are with demand being greater than fair sharing.
        // for them we allocate fair share
        for (auto it = flows.begin(); it != flows.end(); /* nothing */) {

            Flow* f = *it;

            auto path = f->getPath();
            const Resource* minR = *std::min_element(path->begin(), path->end(), Resource::comp2);

            const double fairShare = minR->getFairCapacity();
            if (f->getDemand() > fairShare) {
                f->setAllocation(fairShare);

                for (auto r : *path) {
                    r->countAllottedFlow();
                    r->decreaseCapacity(fairShare);
                }

                it = flows.erase(it);
            } else {
                throw std::invalid_argument("impossible!");
                ++it;
            }
        }

        if (flows.size() != 0)
            throw std::invalid_argument("not all flows are allotted");

}

static void printFlows(const std::vector<Flow*>& flows) {
    std::cout << "Flows:\n";
    for (const Flow* f : flows)
        std::cout << " d:" << f->getDemand() << " a:" << f->getAllocation() << "\n";
}

static void printResources(const std::vector<Resource*>& resources) {
    std::cout << "Resources:\n";
    for (const Resource* r : resources)
        std::cout << " mc:" << r->getMaxCapacity() << " c:" << r->getCapacity() << "\n";
}
};

#endif /* SOLVER_H_ */
