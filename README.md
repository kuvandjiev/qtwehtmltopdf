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

``` qmake -o Makefile qtwehtmltopdf.pro ```

``` make clean ```

``` make ```
