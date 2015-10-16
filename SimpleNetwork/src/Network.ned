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

// Created: 2015-9-12 for project SimpleNetwork

package simplenetwork.src;

import ned.IdealChannel;

channel DataChannel extends ned.IdealChannel
{
    parameters:
        @resource;
        double maxCapacity = default(100);
        double capacity = maxCapacity;
}

simple Switch
{
    parameters:
        @resource;
        @node;
        @display("i=device/switch");
        //int address;
        
        double maxCapacity = default(1000);
        double capacity = maxCapacity;
        
    gates:
        inout port[];
}

simple Server
{
    parameters:
        @node;
        @display("i=device/device");
        //int address;
        
    gates:
        inout port;
        
}


module Rack
{
    parameters:
        @display("i=device/router;bgb=400,200");
        int size @prompt("Number of servers in one rack") = default(10);

    gates:
        inout port;

    submodules:
        server[size]: Server {
            parameters:
                // address = index; //TODO assigning address
        }
        switch: Switch {
            gates:
                port[size+1];
        }

    connections:
        for k=1..size {
//            switch.port$o[k] --> DataChannel --> server[k-1].port$i;
//            switch.port$i[k] <-- DataChannel <-- server[k-1].port$o;
            switch.port[k] <--> DataChannel <--> server[k-1].port;
        }
        port <--> DataChannel <--> switch.port[0];
}


module Hall
{
    parameters:
        @display("i=device/lan-bus;bgb=180,130");
        int size @prompt("Number of racks in one hall") = default(5);

    gates:
        inout port;

    submodules:
        switch: Switch {
            gates:
                port[size+1];
        }

        rack[size]: Rack;
    connections:
        for k=1..size {
            switch.port[k] <--> DataChannel <--> rack[k-1].port;
        }
        port <--> switch.port[0];
}

module DataCenter
{
    parameters:
        @display("i=misc/building");
        int size @prompt("Number of halls in one data center") = default(2);

    gates:
        inout port;

    submodules:
        hall[size]: Hall;
        switch: Switch {
            gates:
                port[size+1];
        }
    connections:
        for k=1..size {
            switch.port[k] <--> DataChannel <--> hall[k-1].port;
        }
        port <--> switch.port[0];
}

module Continent
{
    parameters:
        //@display("i=maps/africa");
        int size @prompt("Number of DCs in one continent") = default(2);

    gates:
        inout port;

    submodules:
        dc[size]: DataCenter;
        switch: Switch {
            gates:
                port[size+1];
        }
    connections:
        for k=1..size {
            switch.port[k] <--> DataChannel <--> dc[k-1].port;
        }
        port <--> switch.port[0];
}

network Storage
{
    parameters:
        @display("i=misc/cloud");
        int size @prompt("Number of continents in storage") = default(2);

    gates:
        //inout port;

    submodules:
        continent[size] : Continent;
//        c1: Continent {
//            parameters:
//                @display("i=maps/australia;");
//        }
//        c2: Continent {
//            parameters:
//                @display("i=maps/australia");
//        }
//        c3: Continent {
//            parameters:
//                @display("i=maps/australia");
//        }

        switch: Switch {
            gates:
                port[size+1];
        }
    connections allowunconnected:
        for k=1..size {
            switch.port[k] <--> continent[k-1].port;
        }
//        switch.port[1] <--> DataChannel <--> c1.port;
//        switch.port[2] <--> DataChannel <--> c2.port;
//        switch.port[3] <--> DataChannel <--> c3.port;

        //port <--> switch.port[0]; INFO commented for a while

}
