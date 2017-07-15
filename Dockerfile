FROM ubuntu:16.04
MAINTAINER Aleksi Salo <alsalo722@gmail.com>
ADD . heat-controller/
RUN apt-get update && apt-get install -y gcc cmake git libssl-dev
RUN git clone https://github.com/DaveGamble/cJSON.git
RUN mkdir -p cJSON/build && cd cJSON/build && cmake .. && make && make install && cd ../..
RUN git clone https://github.com/eclipse/paho.mqtt.c.git
RUN mkdir -p paho_mqtt/build && cd paho.mqtt.c && make && make install && cd ../..
RUN mkdir -p heat-controller/build && cd heat-controller/build && cmake .. && make && cd ../..
ENTRYPOINT ["/bin/bash","-c"]
