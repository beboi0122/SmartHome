#include <led_controller.hpp>

LED *LED::instance = nullptr;

bool LED::setLed(int led, bool state){
    if(leds[led] == state){
        return false;
    }
    std::lock_guard<std::mutex> lock(led_lock);
    
    leds[led] = state;

    gpio_set_level(GPIO_NUM_4, 0);
    gpio_set_level(GPIO_NUM_0, 0);
    gpio_set_level(GPIO_NUM_2, 0);
    gpio_set_level(GPIO_NUM_15, 0);

    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    uint32_t bits[4] = {0};
    bits[0] = led%2;
    for (int i = 0; i < 4; i++){
        bits[i] = led%2;
        led = led >> 1;
    }

    gpio_set_level(GPIO_NUM_4, bits[3]);
    gpio_set_level(GPIO_NUM_0, bits[2]);
    gpio_set_level(GPIO_NUM_2, bits[1]);
    gpio_set_level(GPIO_NUM_15, bits[0]);

    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    gpio_set_level(GPIO_NUM_4, 0);
    gpio_set_level(GPIO_NUM_0, 0);
    gpio_set_level(GPIO_NUM_2, 0);
    gpio_set_level(GPIO_NUM_15, 0);

    return true;
}