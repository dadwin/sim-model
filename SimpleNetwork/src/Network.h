/*
 * Network.h
 *
 *  Created on: Oct 12, 2015
 *      Author: aarlan
 */

#ifndef NETWORK_H_
#define NETWORK_H_

#include <vector>
#include "Resource.h"
#include "Flow.h"


class cChannel;
class cModule;

class Network {

protected:
public:
    std::vector<Resource*> resources; // TODO it can be map <cComponent*, Resource*>
    std::vector<Flow*> flows;

public:
    Network() {

    }


    void registerResource(Resource* r) {
        resources.push_back(r);
    }

    Resource* getResourceByNedObject(const cComponent* const comp) const {
        if (comp == nullptr) {
            throw cRuntimeError("'comp' is null pointer");
        }
        for (auto r : resources) {
            if (r->getNedComponent() == comp)
                return r;
        }
        return nullptr;
    }


};

extern Network network;

#endif /* NETWORK_H_ */
