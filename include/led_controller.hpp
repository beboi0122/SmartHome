#pragma once
#include <mutex>
#include <driver/gpio.h>
#include <thread>
#include <chrono>


class LED {
    static LED *instance;

    private:
        std::mutex led_lock;
        bool leds[15] = {false};

    public:
        static LED* getInstance(){
            if(!instance){
                instance = new LED();
                    gpio_reset_pin(GPIO_NUM_4); 
                    gpio_set_direction(GPIO_NUM_4, GPIO_MODE_OUTPUT);
                    gpio_reset_pin(GPIO_NUM_0);
                    gpio_set_direction(GPIO_NUM_0, GPIO_MODE_OUTPUT);
                    gpio_reset_pin(GPIO_NUM_2); // internal led
                    gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT);
                    gpio_reset_pin(GPIO_NUM_15);
                    gpio_set_direction(GPIO_NUM_15, GPIO_MODE_OUTPUT);
            }
            return instance;
        }

        bool setLed(int led, bool state);
};



