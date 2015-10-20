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
    std::vector<Resource*> resources;
    std::vector<Flow*> flows;

public:
    Network() {

    }


    void registerResource(Resource* r) {
        resources.push_back(r);
    }

    Resource* getResourceByNedObject(const cObject* const obj) const {
        if (obj == nullptr) {
            throw cRuntimeError("'obj' is null pointer");
        }
        for (auto r : resources) {
            if (r->getNedObj() == obj)
                return r;
        }
        return nullptr;
    }


};

extern Network network;

#endif /* NETWORK_H_ */
