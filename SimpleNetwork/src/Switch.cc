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

#include "Switch.h"
#include "Network.h"
#include "Resource.h"

#include <cassert>

Define_Module(Switch);

void Switch::initialize()
{
    const double maxCapacity = par("maxCapacity");
    Resource* resource = new SwitchResource(this, 0, maxCapacity); // TODO what about id? can it be 'this' pointer?
    network.registerResource(resource);

    for (cModule::GateIterator it(this); !it.end(); it++) {
        cGate* gate = it();

        assert(gate->isVector() == true);

        // TODO log something special here.
//        ev << gate->getFullName() << gate->getChannel() << gate->getFullPath() << gate->isConnectedOutside() << gate->isConnectedInside() << endl;

        auto ch = gate->getChannel();

        if (ch != nullptr) {
            // if a gate has no underlying channel
            // then this gate is not interesting for us from link point of view
            const double maxCapacity = ch->par("maxCapacity");
            Resource* resource = new LinkResource(ch, 0, maxCapacity); // TODO what about id? can it be 'this' pointer?
            network.registerResource(resource);
        }
    }
}

void Switch::handleMessage(cMessage *msg)
{
    // TODO - Generated method body
}
