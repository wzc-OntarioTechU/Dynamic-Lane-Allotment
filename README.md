# Dynamic Lane Allotment IoT Final Project

The goal of this IoT Project Assignment is to develop a rudimentary system for providing an on demand bike lane.
The system is supposed to model a roadway with embedded lighting that illuminates when a bicycle or other small auto-propelled vehicle approaches to signal to drivers to leave space.

## Concept

The system should have a set of beacons or stations along the roadway which control the lighting for segments of the road.
Each user will have a bluetooth tag/beacon on their person or vehicle which simply broadcasts its ID and potentially a value representing that lighting is requested.
These beacons will scan and report the signal strength of these various BT devices to a gateway which will compare the values gathered and respond telling segments to light.
This data can be stored away from the gateway to a cloud service or database to both provide metrics for a municipality to understand the road usage, but to also define what BT addresses are valid users (as opposed to other BT devices), to define always on times, or on demand requests.