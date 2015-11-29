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

#include "Flow.h"
#include "Switch.h"
#include "Resource.h"


Define_Module(Switch);

void Switch::initialize()
{
    net = check_and_cast<Net*>(getModuleByPath("net"));

    const double maxCapacity = par("maxCapacity");
    Resource* resource = new SwitchResource(this, maxCapacity); // TODO what about id? can it be 'this' pointer?
    net->registerResource(resource);

    // identifying channels as resources
    // we iterate through all OUTPUT gates and those which have underlying channels,
    // since some of them have not - e.g. case of connection between rack switch and rack itself.
    for (GateIterator it(this); !it.end(); it++) {
        auto gate = it();

        if (gate->getType() != cGate::OUTPUT)
            continue;

        auto channel = gate->getChannel();
        if (channel == nullptr) {
            //continue;
            auto nextGate = gate->getNextGate();
            if (nextGate == nullptr) {
                continue;
                // I expect only one case when nextGate is null:
                // for switch of storage level. This switch may be connected to nowhere
                // (at least for beginning). However when model will become complete
                // this case will be vanished and exception should be here:
                // throw cRuntimeError("nextGate should be non-null");
            }

            channel = nextGate->getChannel();
            if (channel == nullptr) {
                // I assume that for nextGate channel must be no-null.
                // It's implied from network topology.
                // Probably I can relaxed this requirement, but I prefer to know about it explicitly
                // rather than via debugging.
                throw cRuntimeError("channel under nextGate should be non-null");
            }
        }
        const double maxCapacity = channel->par("maxCapacity");
        Resource* resource = new LinkResource(channel, maxCapacity); // TODO what about id? can it be 'this' pointer?
        net->registerResource(resource);
    }
}

void Switch::handleMessage(cMessage *msg)
{
    MessageType msgType = static_cast<MessageType>(msg->getKind());

    switch (msgType) {
    case FlowMessage: {
        if (msg->isSelfMessage() != true)
            throw cRuntimeError("FlowMessage must be self-message");
        break;
    }
    case NewFlow: {
        if (msg->isSelfMessage() != true)
            throw cRuntimeError("NewFlow must be self-message");
        break;
    }
    case DataMessage: {
        if (msg->isSelfMessage() == true)
            throw cRuntimeError("DataMessage must be non-self-message");
        handleDataMessage(msg);
        // DataMessage can be deleted by switch, since it's transmitted through it.
        // Only Node can delete DataMessage
        break;
    }
    case ScheduledInitialize: {
        if (msg->isSelfMessage() != true)
            throw cRuntimeError("ScheduledInitialize must be self-message");
        break;
    }
    default: {
        break;
    }
    }

}

void Switch::handleDataMessage(cMessage *msg) {
    Flow* flow = (Flow*) msg->par("flow").pointerValue();

    const int gateId = flow->getNextGateId();
    send(msg, gateId);
}
