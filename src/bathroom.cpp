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
#include <bathroom.hpp>
#include <sensors.hpp>


static void IRAM_ATTR bathroom_light_interrupt_handler(void *arg){
    auto now = xTaskGetTickCountFromISR();
    auto bathroom = reinterpret_cast<Bathroom *> (arg);
    xQueueSendFromISR(bathroom->bathroom_light_queue, &now, NULL);
}

Bathroom::Bathroom(){
    Bathroom::bathroom_setUp();
}

void Bathroom::bathroom_setUp(){


    gpio_reset_pin(GPIO_NUM_26);
    ESP_ERROR_CHECK(gpio_set_direction(GPIO_NUM_26, GPIO_MODE_OUTPUT));

    gpio_reset_pin(GPIO_NUM_25);
    ESP_ERROR_CHECK(gpio_set_direction(GPIO_NUM_25, GPIO_MODE_OUTPUT));


    {
    gpio_config_t bathroom_light_interrupt_params = {};
    bathroom_light_interrupt_params.pin_bit_mask = GPIO_SEL_16;
    bathroom_light_interrupt_params.mode = GPIO_MODE_INPUT;
    bathroom_light_interrupt_params.pull_down_en = GPIO_PULLDOWN_DISABLE;
    bathroom_light_interrupt_params.pull_up_en = GPIO_PULLUP_DISABLE;
    bathroom_light_interrupt_params.intr_type = GPIO_INTR_POSEDGE;
    ESP_ERROR_CHECK(gpio_config(&bathroom_light_interrupt_params));
    bathroom_light_queue = xQueueCreate(1, sizeof(uint8_t));
    }

    gpio_isr_handler_add(GPIO_NUM_16, bathroom_light_interrupt_handler, this);

    std::thread light_btn_task_thread(&Bathroom::light_btn_task, this);
    light_btn_task_thread.detach();

    std::thread bathroom_fan_task_thread(&Bathroom::bathroom_fan_task, this);
    bathroom_fan_task_thread.detach();

    std::thread bathroom_leaking_protection_task_thread(&Bathroom::bathroom_leaking_protection_task, this);
    bathroom_leaking_protection_task_thread.detach();

}

void Bathroom::light_btn_task(){
    vTaskDelay(2000);
    uint8_t _pressed = 0;
    while(1){
        if(xQueueReceive(bathroom_light_queue, &_pressed, portMAX_DELAY)){
            Bathroom::bathroom_light((!Bathroom::isLightOn ? ON : OFF));
            xQueueReset(bathroom_light_queue);
        }
    }
}

void Bathroom::bathroom_fan_task(){
    while(1){
        vTaskDelay(2000);
        int hum = Sensors::getInstance() -> read_bathroom_humidity();
        if(hum > 100){
            continue;
        }
        if(Bathroom::isFanOn && hum <= 55)
             Bathroom::bathroom_fan(OFF);
        if(!Bathroom::isFanOn && hum >=65)
             Bathroom::bathroom_fan(ON);
    }

}

void Bathroom::bathroom_light(on_off stat){
    LED::getInstance() -> setLed(15, stat == ON);
    std::cout << "Bathroom light " << (stat == ON ? "ON" : "OFF") << std::endl;
    //gpio_set_level(GPIO_NUM_26, stat == ON);
    Bathroom::isLightOn = stat == ON;
    vTaskDelay(500);
}

void Bathroom::bathroom_fan(on_off stat){
    if((stat == ON) != Bathroom::isFanOn){
        LED::getInstance() -> setLed(14, stat == ON);
        std::cout << "Bathroom fan " << (stat == ON ? "ON" : "OFF") << std::endl;
        //gpio_set_level(GPIO_NUM_26, stat == ON);
        Bathroom::isFanOn = stat == ON;
        vTaskDelay(500);
    }
}

void Bathroom::bathroom_leaking_protection_task(){
    while(1){
        vTaskDelay(5000);
        if(Sensors::getInstance()->read_bathroom_moisture() >= 3000){
            Bathroom::leaking = true;
            std::cout << "LEAKING" << std::endl;
            LED::getInstance()->setLed(7, 1);
        }else if(leaking && Sensors::getInstance()->read_bathroom_moisture() <= 3000){
            leaking = false;
            LED::getInstance()->setLed(7, 0);
        }
        
    }

}

