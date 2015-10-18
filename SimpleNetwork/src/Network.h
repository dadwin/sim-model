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


class cChannel; // forward declaration

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

    Resource* getResourceByChannel(const cChannel* const ch) const {
        if (ch == nullptr) {
            throw cRuntimeError("'ch' is null pointer");
        }
        for (auto r : resources) {
            auto ri = static_cast<LinkResource*>(r);
            if (ri->getNedObj() == ch)
                return r;
        }
        return nullptr;
    }


};

extern Network network;

#endif /* NETWORK_H_ */
