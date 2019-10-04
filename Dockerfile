FROM ubuntu:18.04

RUN mkdir -p /app/
WORKDIR /app/

ADD . /app/
RUN apt-get update && apt-get install build-essential qt5-default qt5-qmake qtwebengine5-dev xvfb -y

CMD bash