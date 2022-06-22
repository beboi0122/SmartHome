#pragma once

static void IRAM_ATTR wc_light_interrupt_handler(void *arg);

class Wc{
private:
    friend void IRAM_ATTR wc_light_interrupt_handler(void *arg); 
    QueueHandle_t light_queue = nullptr;

    bool light = false;
    bool fan = false;

    long startTime = 0;

    long fanTime = 10;
    long lightTime = 20;


public:
    Wc();
    void wc_setUp();
    void light_btn_task();
    void fan_task();
    

};

