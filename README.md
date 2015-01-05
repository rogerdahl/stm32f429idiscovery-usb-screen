### 32F429IDISCOVERY USB screen and MJPEG web cam streamer

This project has two parts, firmware that turns the $25 32F429IDISCOVERY board
into a USB screen and a host app for Linux that uses the USB screen for
displaying an MJPEG stream retrieved over HTTP (typically a web camera).

32F429IDISCOVERY board:

http://www.st.com/st-web-ui/static/active/en/resource/technical/document/data_brief/DM00094498.pdf

The firmware shows how to:

- Implement a simple USB protocol on the 32F429IDISCOVERY.
- Use the USB DMA to move data directly from the USB bus to the LCD screen.
- Control the LEDs.

The host app shows how to:

- Use libusb to interface with the USB screen.
- Read and process a MJPEG stream using C++ and Boost.
- Use Boost::ASIO to read an MJPEG stream.
- Parse the MJPEG stream (multipart MIME).
- Uncompress JPEGs in memory with IJG libjpeg.

#### Building the firmware

See the instructions here on how to set up an open source development
environment on Linux:

https://github.com/tomvdb/stm32l1-discovery-basic-template

When things are set up properly, build and flash with:

make flash

#### Building the host application

Currently, there's no Makefile for the host project. Build as follows:

sudo apt-get install libboost-all-dev libjpeg-dev

g++ -o analyze -std=c++11 -I/usr/include/libusb-1.0/ *.cpp -lboost_system -ljpeg -lpthread -lusb-1.0

#### Technologies

- C++
- Boost::ASIO
- IJG libjpeg
