FROM ubuntu:18.04
RUN apt update && apt install build-essential qt5-default qt5-qmake qtwebengine5-dev xvfb python3-wheel python3-venv python3.7 python3.7-venv wget -y
RUN python3.7 -m venv /py37-venv

CMD /usr/bin/Xvfb :99 -screen 0 1024x768x24