#ifndef RESOURCE_H_
#define RESOURCE_H_

#include <stdexcept>

class Resource {
public:

    //Resource(long id, double maxCapacity) {
    //}

    virtual ~Resource() {
    }

    virtual long getId() const = 0;

    virtual double getMaxCapacity() const = 0;

    virtual double getCapacity() const = 0;

    virtual void decreaseCapacity(const double delta) = 0;

    bool isBusy() const {
        return getMaxCapacity() == getCapacity();
    }

    bool isFree() const {
        return getCapacity() == 0.0;
    }

};

class cObject;

class SwitchResource : public Resource {

protected:
    long id;
    cObject* nedObj;
    double maxCapacity;
    double capacity;

public:

    SwitchResource(cObject* nedObj, const long id, const double maxCapacity) {
        this->nedObj = nedObj;
        this->id = id;
        this->maxCapacity = maxCapacity;
        this->capacity = maxCapacity;
    }

    virtual long getId() const {
        return id;
    }

    virtual cObject* getNedObj() const {
        return nedObj;
    }

    virtual double getMaxCapacity() const {
        return maxCapacity;
    }

    virtual double getCapacity() const {
        return capacity;
    }

    virtual void decreaseCapacity(const double delta) {
        if (delta < 0.0) {
            throw std::invalid_argument("decreasing with negative value");
        }
        if (capacity - delta < 0) {
            throw std::invalid_argument("decreasing down to negative value");
        }
        capacity -= delta;
    }

};

class LinkResource : public Resource {

protected:
    long id;
    cObject* nedObj;
    double maxCapacity;
    double capacity;

public:

    LinkResource(cObject* nedObj, const long id, const double maxCapacity) {
        this->nedObj = nedObj;
        this->id = id;
        this->maxCapacity = maxCapacity;
        this->capacity = maxCapacity;
    }

    virtual long getId() const {
        return id;
    }

    virtual cObject* getNedObj() const {
        return nedObj;
    }

    virtual double getMaxCapacity() const {
        return maxCapacity;
    }

    virtual double getCapacity() const {
        return capacity;
    }

    virtual void decreaseCapacity(const double delta) {
        if (delta < 0.0) {
            throw std::invalid_argument("decreasing with negative value");
        }
        if (capacity - delta < 0) {
            throw std::invalid_argument("decreasing down to negative value");
        }
        capacity -= delta;
    }

};


#endif

