// Routing support module
// 
// The module provides support for nodes addressing and route calculations
// All other modules make direct calls to this module. It should exist in one instance.
//
// The routing support is encapsulated into the simple module instead of a global static
// variable/functions for proper initializaiton on each simulation run (static variables are not
// reset while the simulator is running). 
//
// Author:  Denis Anisimov
//
// (c) Copyright Innopolis University 2015
// 

package storage.networks;

simple Routing
{
    parameters:
        @display("i=block/table2");
}