# esp32-rgb-led-controller
ESP32 + Processing Android RGB LED Controller with multiple effects
# ESP32 RGB LED Controller

This project allows controlling WS2812B RGB LED strip using an Android app made with Processing.

## Features

- Multiple LED effects
- Color selection
- WiFi control
- ESP32 Access Point mode

## Effects

- Solid color
- Rainbow
- Police lights
- Meteor
- Water ripple
- Pulse
- Breath
- Wave

## Hardware

- ESP32
- WS2812B LED strip
- 5V power supply

## Pin connection

ESP32 GPIO18 -> DIN of LED strip

## WiFi

SSID: MALATYA  
Password: 44044044

## App

Processing Android app sends commands to ESP32 via HTTP.

Examples:
/color/red
/mode/meteor
