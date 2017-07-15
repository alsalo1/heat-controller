# heat-controller
Implements https://github.com/WATTx/code-challenges/blob/master/swe-challenge.md

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
