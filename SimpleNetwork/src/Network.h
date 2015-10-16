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


};

extern Network network;

#endif /* NETWORK_H_ */
