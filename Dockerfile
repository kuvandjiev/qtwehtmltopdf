# Running with Python service
FROM kuvandjiev/ubuntu18.04-qt5-python37:latest

RUN mkdir -p /app/
WORKDIR /app/
ADD . /app/
RUN qmake -o Makefile qtwehtmltopdf.pro
RUN make clean && make

RUN . /py37-venv/bin/activate && pip install -r /app/service-python/requirements.txt

CMD rm -f /tmp/.X99-lock && nohup /usr/bin/Xvfb :99 -screen 0 1024x768x24 > /dev/null 2>&1 & . /py37-venv/bin/activate && python3.7 /app/service-python/main.py