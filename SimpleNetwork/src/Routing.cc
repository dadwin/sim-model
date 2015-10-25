/*
 * Routing.cc
 *
 *  Created on: Oct 25, 2015
 *      Author: aarlan
 */

#include "Routing.h"

Define_Module(Routing);

void Routing::initialize() {
    mutex.lock();
    lastAddress = 0;
    addressMapping.clear();
    mutex.unlock();
}

void Routing::handleMessage(cMessage* msg) {

}

int Routing::assignAddress(Server* server) {
    Enter_Method_Silent();
    mutex.lock();

    int address = ++lastAddress;
    addressMapping.insert(std::pair<int, Server*>(address, server));

    mutex.unlock();
    return address;
}

Server* Routing::getServerByAddress(const int address) {
    // TODO what about input argument checking?
    Enter_Method_Silent();

    mutex.lock();

    Server* server;
    auto it = addressMapping.find(address);
    if (it == addressMapping.end()) {
        server = nullptr;
    } else {
        server = it->second;
    }

    mutex.unlock();
    return server;
}
