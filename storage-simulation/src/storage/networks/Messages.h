/*
 * Messages.h
 *
 *  Created on: Nov 28, 2015
 *      Author: aarlan
 */

#ifndef MESSAGES_H_
#define MESSAGES_H_

/*
 * This file defines message types that are used across the system
 * Message types are needed to distinguish different messages
 * For that purpose cMessage class has property 'kind'.
 * This 'kind' member is a type of short.
 * To make the system more reliable I use enum for message types -
 * you have no chances to pass wrong type.
 * But there is a drawback - we need to convert enum to short explicitly:
 * setKind(static_cast<short>(FlowMessage))
 * enum type = static_cast<enum MessageType>(m->getKind())
 */

enum MessageType {
    DoNotUseThisMessageType, // This is to avoid zero value.

    ScheduledInitialize,  // this message type is used perform scheduled initialization.
                          // This initialization is performed after initialize() of all modules are invoked
                          // so we have guarantee that all modules are initialized and can rely on it.
                          // It's a self-message.

    NewFlow,              // This type is used to put, to introduce new flow in the network.
                          // It's a self-message.

    FlowMessage,          // It's used to schedule the end of a flow.
                          // It's a self-message.

    DataMessage           // This message type is used to transmit a flow from source node to destination one.
                          // It's not a self-message.
};



#endif /* MESSAGES_H_ */
