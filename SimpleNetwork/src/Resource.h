#ifndef RESOURCE_H_
#define RESOURCE_H_

#include <stdexcept>


class cComponent;

class Resource {
public:

//    Resource(cComponent* nedComponent, double maxCapacity) {
//        this->capacity = maxCapacity;
//        this->maxCapacity = maxCapacity;
//        this->flows = 0;
//        this->flows_allotted = 0;
//        this->nedComponent = nedComponent;
//    }
    
    virtual ~Resource() {
    }
    
    virtual cComponent* getNedComponent() const {
        return nedComponent;
    }

    static bool comp1(const Resource* first, const Resource* second) {
        const double mc1 = first->getFairMaxCapacity();
        const double mc2 = second->getFairMaxCapacity();
        return mc1 < mc2;
    }

    static bool comp2(const Resource* first, const Resource* second) {
        const double fc1 = first->getFairCapacity();
        const double fc2 = second->getFairCapacity();
        return fc1 < fc2;
    }

    virtual double getFairMaxCapacity() const {
        return maxCapacity / (double) flows;
    }

    virtual double getFairCapacity() const {
        const double n = flows - flows_allotted;
        return capacity / n;
    }

    virtual void countFlow() {
        checkInvariant();
        flows++;
        checkInvariant();
    }

    virtual void countAllottedFlow() {
        checkInvariant();
        flows_allotted++;
        checkInvariant();
    }

    virtual double getMaxCapacity() const {
        return maxCapacity;
    }

    virtual double getCapacity() const {
        return capacity;
    }

    virtual void decreaseCapacity(const double delta) {
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

    virtual void setMaxCapacity() {
        checkInvariant();
        capacity = maxCapacity;

        flows_allotted = 0; // TODO is it OK?
        checkInvariant();
    }

    virtual void resetCounts() {
        checkInvariant();
        flows = 0;
        checkInvariant();
    }

protected:
    double maxCapacity;
    double capacity;
    int flows;
    int flows_allotted;
    cComponent* nedComponent;
    

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



class SwitchResource : public Resource {
public:
    SwitchResource(cComponent* nedComponent, double maxCapacity) {
        this->capacity = maxCapacity;
        this->maxCapacity = maxCapacity;
        this->flows = 0;
        this->flows_allotted = 0;
        this->nedComponent = nedComponent;
    }
};

class LinkResource : public Resource {
public:
    LinkResource(cComponent* nedComponent, double maxCapacity) {
        this->capacity = maxCapacity;
        this->maxCapacity = maxCapacity;
        this->flows = 0;
        this->flows_allotted = 0;
        this->nedComponent = nedComponent;
    }
};


#endif

