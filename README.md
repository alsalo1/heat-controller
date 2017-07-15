# heat-controller
Implements https://github.com/WATTx/code-challenges/blob/master/swe-challenge.md in C

## License
BSD 3-Clause License - See LICENSE for details

arrlist - See https://bitbucket.org/binarbaum/root/src/ae24235d0846748d0cbeb1e3345641e43956812a/LICENSE.md

## External dependencies
cJSON Parser https://github.com/DaveGamble/cJSON

Eclipse Paho MQTT C client https://github.com/eclipse/paho.mqtt.c/

## How to build and run
It is assumed that a MQTT broker is running on the host machine and listens to localhost:1883
### Option 1 - Build and run locally
 * Install gcc and cmake
 * Build and install cJSON and Paho MQTT libraries
 * Build with cmake
```
mkdir build
cd build
cmake ..
make
```
 * Run
```
./src/heat-controller
```
### Option 2 - Build a docker image and run in a container
 * Install docker
 * Build a docker image
```
docker build -t heat-controller .
```
 * Run in a docker container
```
docker run --rm --net=host heat-controller:latest "./heat-controller/build/src/heat-controller"
```

## Current Implementation
The current controller in pseudocode:
```
Connect to MQTT broker
Subscribe to all sensor readings
while(no error)
  Collect sensor readings for 10 seconds
  if(readings received)
    Calculate average of temperature readings
    if(average > 22.5C)
      Set valve to 0%
    else if(average < 21.5C)
      Set valve to 100%
    endif
    Clear the readings
  endif
endwhile
```
### Additional inputs
Motion sensor readings go to topic ```/readings/motion``` in the json format:
```
{
  "sensorID": "sensor-2",
  "type": "motion",
  "value": true
}
```
## TODO
 * Better detection of bad sensor data. Single bad reading can cause problems in current solution.
 * Use PID for valve adjustment instead of the simple on-off control in current implementation.
 * Create an actual room model to run and test the controller against (something simple like: https://de.mathworks.com/help/simulink/gs/define-system.html)
 * Add more sensor types
 * Support multiple rooms
 * Improve configurability (do not hardcode all the values)
 
