#ifndef RESOURCE_H_
#define RESOURCE_H_

#include <stdexcept>


class cComponent;

class Resource {
public:

    //Resource(long id, double maxCapacity) {
    //}

    virtual ~Resource() {
    }

    virtual long getId() const = 0;

    virtual cComponent* getNedComponent() const = 0;

    virtual double getMaxCapacity() const = 0;

    virtual double getCapacity() const = 0;

    virtual void changeCapacity(const double delta) = 0;

    virtual void setMaxCapacity() = 0;

    bool isBusy() const {
        return getMaxCapacity() == getCapacity();
    }

    bool isFree() const {
        return getCapacity() == 0.0;
    }

};


class SwitchResource : public Resource {

protected:
    long id;
    cComponent* nedComponent;
    double maxCapacity;
    double capacity;

public:

    SwitchResource(cComponent* nedComponent, const long id, const double maxCapacity) {
        this->nedComponent = nedComponent;
        this->id = id;
        this->maxCapacity = maxCapacity;
        this->capacity = maxCapacity;
    }

    virtual long getId() const {
        return id;
    }

    virtual cComponent* getNedComponent() const {
        return nedComponent;
    }

    virtual double getMaxCapacity() const {
        return maxCapacity;
    }

    virtual double getCapacity() const {
        return capacity;
    }

    virtual void changeCapacity(const double delta) {
        if (delta <= 0.0) {
            if (capacity + delta < 0) {
                throw std::invalid_argument("decreasing capacity down to negative value");
            }
        } else {
            if (capacity + delta > maxCapacity ) {
                throw std::invalid_argument("increasing capacity up to maxCapacity value");
            }
        }
        capacity += delta;
    }

    virtual void setMaxCapacity() {
        capacity = maxCapacity;
    }
};

class LinkResource : public Resource {

protected:
    long id;
    cComponent* nedComponent;
    double maxCapacity;
    double capacity;

public:

    LinkResource(cComponent* nedComponent, const long id, const double maxCapacity) {
        this->nedComponent = nedComponent;
        this->id = id;
        this->maxCapacity = maxCapacity;
        this->capacity = maxCapacity;
    }

    virtual long getId() const {
        return id;
    }

    virtual cComponent* getNedComponent() const {
        return nedComponent;
    }

    virtual double getMaxCapacity() const {
        return maxCapacity;
    }

    virtual double getCapacity() const {
        return capacity;
    }

    virtual void changeCapacity(const double delta) {
        if (delta <= 0.0) {
            if (capacity + delta < 0) {
                throw std::invalid_argument("decreasing capacity down to negative value");
            }
        } else {
            if (capacity + delta > maxCapacity ) {
                throw std::invalid_argument("increasing capacity up to maxCapacity value");
            }
        }
        capacity += delta;
    }

    virtual void setMaxCapacity() {
        capacity = maxCapacity;
    }
};


#endif

