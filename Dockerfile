FROM kuvandjiev/ubuntu18.04-qt5-python37:latest

RUN mkdir -p /app/
WORKDIR /app/
ADD . /app/
RUN . /py37-venv/bin/activate && pip install -r /app/service/requirements.txt

WORKDIR /root/
RUN wget https://dl.google.com/go/go1.13.4.linux-amd64.tar.gz && tar xzf ./go1.13.4.linux-amd64.tar.gz
ENV PATH="${PATH}:/root/go/bin"

CMD /usr/bin/Xvfb :99 -screen 0 1024x768x24