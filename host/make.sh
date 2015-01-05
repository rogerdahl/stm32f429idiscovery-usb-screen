#!/bin/sh

g++ -o analyze -std=c++11 -I/usr/include/libusb-1.0/ *.cpp -lboost_system -lboost_regex -ljpeg -lpthread -lusb-1.0
