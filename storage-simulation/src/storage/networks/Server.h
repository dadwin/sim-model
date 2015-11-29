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

#ifndef __SIMPLENETWORK_SERVER_H_
#define __SIMPLENETWORK_SERVER_H_

#include <omnetpp.h>
#include "Routing.h"
#include "Net.h"
/**
 * TODO - Generated class
 */

class Flow;
class Resource;

class Server : public cSimpleModule
{
  protected:
    int address;
    Routing* routing;
    Net* net;

    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    void handleFlowMessage(cMessage *msg);
    void handleDataMessage(cMessage *msg);
    void handleNewFlow(cMessage *msg);
    void scheduledInitialize();

  public:
    void schedule(simtime_t t, cMessage* msg);
    int getAddress() const;
    void addFlow(const int sourceAddress, const int destAddress,
                      const simtime_t startTime, const simtime_t endTime,
                      const double desiredAllocation);
    void removeFlow(Flow* flow);

};

#endif
