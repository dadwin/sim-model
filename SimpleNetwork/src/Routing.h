/*
 * Routing.h
 *
 *  Created on: Oct 25, 2015
 *      Author: aarlan
 */

#ifndef ROUTING_H_
#define ROUTING_H_

#include <map>
#include <mutex>
#include <vector>
#include <omnetpp.h>

class Server;

class Routing : public cSimpleModule {
public:
    void initialize();
    void handleMessage(cMessage *msg);

    int assignAddress(Server* server);
    Server* getServerByAddress(const int address);

private:
    std::mutex mutex;

    int lastAddress = 0;
    std::map<int, Server*> addressMapping;
};


#endif /* ROUTING_H_ */
