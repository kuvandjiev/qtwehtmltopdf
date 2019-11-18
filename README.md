# qtwehtmltopdf
QT5 Webengine HTML to PDF command-line converter.

Requires a display or xvfb.

Inspired by the wkhtmltopdf project (https://github.com/wkhtmltopdf/wkhtmltopdf)

Usage
-----

```xvfb-run qtwehtmltopdf input.html output.pdf```


Requirements to run
------------------

```Qt5 xvfb ```


Build requirements
--------------------

``` build-essential qt5-default qt5-qmake qtwebengine5-dev ```

How to build
-------------------

``` qmake -o Makefile qtwehtmltopdf.pro ```

``` make clean ```

``` make ```


Using Docker
------------

Build the provided Dockerfile with all the requirements preinstalled and execute the commands from **How to build**


Running qtwehtmltopdf as a service
---------------------------------

There is a Python3.7 example in the `service` directory using `asyncio` and `aiohttp`
Run the Docker container and execute:

``` source /py37-venv/bin/activate ```

``` python /app/service/main.py ```

POST requests to http://localhost:5555/topdf containing HTML as raw POST data will return the generated PDF
