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

/*
class Resource {

protected:
    long id;
    double maxCapacity;
    double capacity;

public:
    Resource(long id, double maxCapacity)
        : id(id), maxCapacity(maxCapacity) {

        this->capacity = maxCapacity;
    }

    const static Resource NullResource;

    long getId() const {
        return id;
    }

    double getMaxCapacity() const {
        return maxCapacity;
    }

    double getCapacity() const {
        return capacity;
    }

    void decreaseCapacity(double delta) {
        if (delta < 0.0) {
            throw std::invalid_argument("decreasing with negative value");
        }
        capacity -= delta;
    }
};

class Flow {
protected:
    long id;
    double demand;
    std::vector<long> path;
    double allocation;

public:
    Flow(long id, double demand, const std::vector<long>& path)
        : id(id), demand(demand), path(path) {

        allocation = 0.0;
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
};

inline bool operator==(const Flow& lhs, const Flow& rhs) {
    return lhs == rhs;
}


*/
class Solver {
protected:
    enum StageType {
        Thin,
        Thick
    };

    static Resource* getResourceById(const std::vector<Resource*>& resources, const long id) {
        for (Resource* r : resources) {
            if (r->getId() == id)
                return r;
        }
        return nullptr;
    }

    static std::vector<Flow*> solveStage(const std::vector<Flow*>& flows,
            const std::vector<Resource*>& resources, const StageType stage) {

        //TODO add checks for flows and resources with the same id's

        std::vector<Flow*> notAllottedFlows(flows);
        std::vector<Resource*> notUsedResources(resources);

        while (true) {

            if (notAllottedFlows.size() == 0) {
                break;
            }

            std::vector<std::vector<Flow*>> flowsForResources;

            for (uint j = 0; j < notUsedResources.size(); j++) {
                std::vector<Flow*> Fj;

                for (Flow* f : notAllottedFlows) {
                    if (f->runThroughResource(notUsedResources.at(j)))
                        Fj.push_back(f);
                }
                flowsForResources.push_back(Fj);
            }
            if (flowsForResources.size() == 0) {
                // size() is equal to number of resources always
                break; // TODO what should we do?
            }

            std::unique_ptr<size_t[]> Nj(new size_t[flowsForResources.size()]);
            for (uint j = 0; j < flowsForResources.size(); j++) {
                Nj[j] = flowsForResources.at(j).size();
#ifdef DEBUG
                std::cout << Nj[j] << " flows through resource " << j << "\n";
#endif
            }

            long unsigned int kArgMin = 0;
            double Ak = std::numeric_limits<double>::max() ;
            for (long unsigned int j = 0; j < flowsForResources.size(); j++) {
                if (Nj[j] == 0)
                    continue;

                double A = notUsedResources.at(j)->getCapacity() / (double) Nj[j];
                if (A <= Ak) {
                    Ak = A;
                    kArgMin = j;
                }
            }
#ifdef DEBUG
            std::cout << "Ak:" << Ak << "\n";
#endif
            std::vector<Flow*> tmpTk = flowsForResources.at(kArgMin);
            std::vector<Flow*> Tk;
            for (Flow* f : tmpTk) {

                switch (stage) {
                case Thick:
                    if (f->getDemand() >= Ak) {
                        Tk.push_back(f);
                    }
                    break;

                case Thin:
                    if (f->getDemand() < Ak) {
                        Tk.push_back(f);
                    }
                    break;
                default:
                    throw std::invalid_argument("bad stage type");
                }

            }
            tmpTk.clear(); // not needed anymore

            if (Tk.size() == 0) {
                break; // TODO stop exit from the loop
            }

            for (Flow* f : Tk) {

                switch (stage) {
                case Thick:
                    f->setAllocation(Ak);
                    break;

                case Thin:
                    f->setAllocation(f->getDemand());
                    break;
                default:
                    throw std::invalid_argument("bad stage type");
                }

                for (long rId : f->getPath()) {
                    Resource* r = resources.at(rId);//getResourceById(resources, rId);
                    if (r == nullptr)
                        throw std::runtime_error("null resource for id " +  std::to_string(rId));
                    r->decreaseCapacity(f->getAllocation());
                }
            }

            for (Flow* f : Tk) {
                auto fPos = std::remove(notAllottedFlows.begin(), notAllottedFlows.end(), f);
                notAllottedFlows.erase(fPos);
            }

            if (stage == Thick) {
                notUsedResources.erase(notUsedResources.begin() + kArgMin);
            }
            //if (notAllottedFlows.size() == 0) {
            //  break;
            //}
        }
        return notAllottedFlows;
    }

public:
    static void solve(const std::vector<Flow*>& flows,
            const std::vector<Resource*>& resources) {
        solveStage(solveStage(flows, resources, Thin), resources, Thick);
    }

    static void printFlows(const std::vector<Flow*>& flows) {
        std::cout << "Flows:\n";
        for (const Flow* f : flows)
            std::cout << "id:" << f->getId() << " d:" << f->getDemand() << " a:" << f->getAllocation() << "\n";
    }

    static void printResources(const std::vector<Resource*>& resources) {
        std::cout << "Resources:\n";
        for (const Resource* r : resources)
            std::cout << "id:" << r->getId() << " mc:" << r->getMaxCapacity() << " c:" << r->getCapacity() << "\n";
    }

};





#endif /* SOLVER_H_ */
