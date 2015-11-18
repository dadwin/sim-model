/*
 * Solver.h
 *
 *  Created on: Oct 16, 2015
 *      Author: aarlan
 */

#ifndef SOLVER_H_
#define SOLVER_H_

#include <algorithm>
#include <iostream>
#include <limits>
#include <memory>
#include <stdexcept>
#include <vector>

#include "Resource.h"
#include "Flow.h"


class Solver {
protected:
        std::vector<Flow*> inputFlows;
        std::vector<Resource*> inputResources;


    static void allocateThinFlows(std::vector<Flow*>& flows,
            const std::vector<Resource*>& resources) {
        // we need flows to be ordered in terms of minimal fair share and demand
        // so we want allocated thin flows (fair share >= demand) in one pass
        // that why here we use compByFairShareDemand
        std::sort(flows.begin(), flows.end(), Flow::compByFairShareDemand);

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
                // NOTE hint for the future:
                // here should be --it; and when we come to this else branch, we can do break
                // since we sorted flows and no thin flows any more
            }
        }
    }

    static bool allocateThickFlows(std::vector<Flow*>& flows,
            const std::vector<Resource*>& resources) {

        // now we have flows all of that are with demand being greater than fair sharing (thick flows).
        // for them we allocate fair share
        std::sort(flows.begin(), flows.end(), Flow::compByFairShare);


        bool isThinFlowFound = false;
        for (auto it = flows.begin(); it != flows.end(); /* nothing */) {

            Flow* f = *it;

            auto path = f->getPath();
            const Resource* minR = *std::min_element(path->begin(), path->end(), Resource::comp2);

            const double fairShare = minR->getFairCapacity();
            if (f->getDemand() >= fairShare) {
                f->setAllocation(fairShare);

                for (auto r : *path) {
                    r->countAllottedFlow();
                    r->decreaseCapacity(fairShare);
                }

                it = flows.erase(it);
            } else {
                isThinFlowFound = true;
                break;
            }
        }

        if (isThinFlowFound)
            allocateThinFlows(flows, resources);

        return flows.size() != 0;
    }

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

        allocateThinFlows(flows, resources);
        while (allocateThickFlows(flows, resources)) {};

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
