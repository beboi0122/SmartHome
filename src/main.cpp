#include <Arduino.h>
#include <garden.hpp>
#include <iostream>
#include <led_controller.hpp>
#include <Wire.h>
#include <room.hpp>
#include <kitchen.hpp>
#include <sensors.hpp> 
#include <wc.hpp>
#include <bathroom.hpp>


void setup(){
  Serial.begin(9600);
  Sensors::getInstance();
  Garden* garden = new Garden();
  Kitchen* kitchen = new Kitchen();
  Room* room = new Room();
  Wc* wc = new Wc();
  Bathroom* bathroom = new Bathroom();
  //TODO
  //garage_setup(); //RIP
  //? alarm_setup();
}

void loop(){
}