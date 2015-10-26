//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "Server.h"
#include "Flow.h"
#include "Net.h"
#include <cexception.h>

#include <algorithm>
#include <map>

Define_Module(Server);

int Server::getAddress() const {
    return address;
}

void Server::initialize()
{
    routing = check_and_cast<Routing*>(getModuleByPath("routing"));
    net = check_and_cast<Net*>(getModuleByPath("net"));
    // set address
    address = routing->assignAddress(this);

    // identifying channels as resources
    // we iterate through all OUTPUT gates and those which have underlying channels,
    // since some of them have not - e.g. case of connection between rack switch and rack itself.
    for (GateIterator it(this); !it.end(); it++) {
        auto gate = it();
        if (gate->getType() != cGate::OUTPUT)
            continue;

        auto channel = gate->getChannel();
        if (channel == nullptr)
            continue;

        const double maxCapacity = channel->par("maxCapacity");
        Resource* resource = new LinkResource(channel, 0, maxCapacity); // TODO what about id? can it be 'this' pointer?
        net->registerResource(resource);
    }


//    if (address == 1) {
//        // TODO for testing
//        scheduleAt(0, new cMessage("Test1", 0));
//    }
//    if (address == 2) {
//        scheduleAt(2, new cMessage("Test1", 0));
//    }
//    if (address == 5) {
//        scheduleAt(3, new cMessage("Test1", 0));
//    }
}


void Server::handleMessage(cMessage *msg)
{
    std::cout << getFullPath() << endl;
    if (msg->isSelfMessage()) {
        handleSelfMessage(msg);
        return;
    }

    if (msg->isName("FlowMessage")) {
        // TODO what should we do here?
    }

    if (msg->isName("DataMessage")) {
        // message arrived!
        ev << "message arrived!" << endl;

        Flow* flow = (Flow*) msg->par("flow").pointerValue();
        net->removeFlow(flow);

        //delete msg; // NOTE Message is part of Flow and Flow is owner of it
    }
}

void Server::handleSelfMessage(cMessage *msg)
{
    if (msg->isName("FlowMessage")) {
        // flow is ended, we need to transmit message to destination server
        // first we need to calculate path along which message will be transmitted


        Flow* flow = (Flow*) msg->par("flow").pointerValue();

        const int gateId = flow->getNextGateId();
        msg->setName("DataMessage");
        send(msg, gateId);
    }

    if (msg->isName("Test1")) {
        net->addFlow(1, 11, 0, 1, 10);
        delete msg;
    }
}

void Server::schedule(simtime_t t, cMessage* msg) {
    Enter_Method_Silent();

    scheduleAt(t, msg);
}


