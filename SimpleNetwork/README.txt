TODO list:

1. Addressing:
	how address is assigned?
		via static variable.
	how find network node by address?
		map of addresses to nodes
	
2. Find nodes through which flow runs.
	path is a sequence of pointers to resources.

3. Find gates' ids for retrasmitting message from source server to destination one. 

4. Exception handling
	how C++ exception and Omnetpp co-exist?
	
	
5. Add priorities for DataMessage and FlowMessage. DataMessage should have more priority

6. Replace string names for message with integer constant (not names, but kinds)

7. why don't use inheritance from Resource and cChannel or cSimpleModule?
why use separated classes?

8. Implement reading input flows from xml file

