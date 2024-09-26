# sds - sensor data server

sds is a simple webserver that I use to gather and display the values recorded
by temperature & humidity sensors made with D1 Minis and DHT20 sensors.

It is my first attempt at coding in C, coming from OOP-Languages like Java and C#.

## Dependencies

`libcurl` is needed to compile this program

## Usage

This program can be used with any kind of sensor, that can make its data
available via HTTP. The response value should be plain text.

Sensors can be added by creating a custom sensors.h.