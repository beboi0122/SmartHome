#pragma once
#include <enums.hpp>

static void IRAM_ATTR bathroom_light_interrupt_handler(void *arg);

class Bathroom{
private:
    friend void IRAM_ATTR bathroom_light_interrupt_handler(void *arg); 
    QueueHandle_t bathroom_light_queue = nullptr;

    bool isLightOn = false;
    bool isFanOn = false;

    long startTime = 0;

    long fanTime = 10;
    long lightTime = 20;


    bool leaking = false;


public:
    Bathroom();
    void bathroom_setUp();
    void light_btn_task();
    void leaking_btn_task();
    void bathroom_light(on_off stat);
    void bathroom_fan_task();
    void bathroom_fan(on_off stat);
    void bathroom_leaking_protection_task();

};