#include <iostream>
#include <driver/gpio.h>
#include <esp_adc_cal.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <thread>
#include <esp_intr_alloc.h>
#include <functional>
#include <led_controller.hpp>
#include <wc.hpp>

static void IRAM_ATTR wc_light_interrupt_handler(void *arg){
    auto now = xTaskGetTickCountFromISR();
    auto wc = reinterpret_cast<Wc *> (arg);
    xQueueSendFromISR(wc->light_queue, &now, NULL);
}

Wc::Wc(){
    Wc::wc_setUp();
}

void Wc::wc_setUp(){

    /*
    gpio_reset_pin(GPIO_NUM_26);
    ESP_ERROR_CHECK(gpio_set_direction(GPIO_NUM_26, GPIO_MODE_OUTPUT));

    gpio_reset_pin(GPIO_NUM_25);
    ESP_ERROR_CHECK(gpio_set_direction(GPIO_NUM_25, GPIO_MODE_OUTPUT));
    */


    {
    gpio_config_t wc_light_interrupt_params = {};
    wc_light_interrupt_params.pin_bit_mask = GPIO_SEL_5;
    wc_light_interrupt_params.mode = GPIO_MODE_INPUT;
    wc_light_interrupt_params.pull_down_en = GPIO_PULLDOWN_DISABLE;
    wc_light_interrupt_params.pull_up_en = GPIO_PULLUP_DISABLE;
    wc_light_interrupt_params.intr_type = GPIO_INTR_POSEDGE;
    ESP_ERROR_CHECK(gpio_config(&wc_light_interrupt_params));
    light_queue = xQueueCreate(1, sizeof(uint8_t));
    }

    gpio_isr_handler_add(GPIO_NUM_5, wc_light_interrupt_handler, this);

    std::thread t1(&Wc::light_btn_task, this);
    t1.detach();
    
}

void Wc::light_btn_task(){
    vTaskDelay(2000);
    uint8_t _pressed = 0;
    while(1){
        if(xQueueReceive(light_queue, &_pressed, portMAX_DELAY)){
            LED::getInstance() -> setLed(13, !Wc::light);
            std::cout << "WC light " << (!Wc::light ? "ON" : "OFF") << std::endl;
            //gpio_set_level(GPIO_NUM_26, !Wc::light);
            if(!Wc::light && !Wc::fan){
                std::thread t(&Wc::fan_task, this);
                t.detach();
            }
            if(Wc::light)
                Wc::startTime = time(0);
            Wc::light = !light;
            vTaskDelay(500);
            xQueueReset(light_queue);
            Wc::startTime = time(0);
        }
    }
}

void Wc::fan_task(){
    LED::getInstance() -> setLed(12, true);
    std::cout << "WC fan " << "ON" << std::endl;
    //gpio_set_level(GPIO_NUM_25, true);
    Wc::fan = true;
    while(1){
        sleep(5);
        long nowTime = time(0);
        if(!Wc::light && nowTime - Wc::startTime >= Wc::fanTime){
            LED::getInstance() -> setLed(12, false);
            std::cout << "WC fan " << "OFF" << std::endl;
            //gpio_set_level(GPIO_NUM_25, false);
            Wc::fan = false;
            break;
        }
        if(Wc::light && nowTime - Wc::startTime >= Wc::lightTime){
            LED::getInstance() -> setLed(12, false);
            std::cout << "WC fan " << "OFF" << std::endl;
            //gpio_set_level(GPIO_NUM_25, false);
            Wc::fan = false;
            LED::getInstance() -> setLed(13, false);
            std::cout << "WC light " << "OFF" << std::endl;
            //gpio_set_level(GPIO_NUM_26, false);
            Wc::light = false;
            break;
        }
    }
}



