#include <iostream>
#include <driver/gpio.h>
#include <esp_adc_cal.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <thread>
#include <esp_intr_alloc.h>
#include <room.hpp>
#include <functional>
#include <sensors.hpp>
#include <led_controller.hpp>



static void IRAM_ATTR room_blinds_interrupt_handler(void *arg){
    uint8_t now = 1;
    auto room = reinterpret_cast<Room *> (arg);
    xQueueSendFromISR(room->blinds_queue, &now, NULL);

}

Room::Room(){
    Room::room_setup();
}


void Room::room_setup(){

    gpio_reset_pin(GPIO_NUM_21);
    ESP_ERROR_CHECK(gpio_set_direction(GPIO_NUM_21, GPIO_MODE_OUTPUT));

    /*gpio_reset_pin(GPIO_NUM_22);
    ESP_ERROR_CHECK(gpio_set_direction(GPIO_NUM_22, GPIO_MODE_OUTPUT));

    gpio_reset_pin(GPIO_NUM_23);
    ESP_ERROR_CHECK(gpio_set_direction(GPIO_NUM_23, GPIO_MODE_OUTPUT));
    
    gpio_reset_pin(GPIO_NUM_25);
    ESP_ERROR_CHECK(gpio_set_direction(GPIO_NUM_25, GPIO_MODE_OUTPUT));
    
    gpio_reset_pin(GPIO_NUM_26);
    ESP_ERROR_CHECK(gpio_set_direction(GPIO_NUM_26, GPIO_MODE_OUTPUT));*/

    ESP_ERROR_CHECK(adc1_config_width(ADC_WIDTH_BIT_12));
    ESP_ERROR_CHECK(adc1_config_channel_atten(ADC1_CHANNEL_3, ADC_ATTEN_DB_11));
    

    {
    gpio_config_t blinds_en_interrupt_params = {};
    blinds_en_interrupt_params.pin_bit_mask = GPIO_SEL_22;
    blinds_en_interrupt_params.mode = GPIO_MODE_INPUT;
    blinds_en_interrupt_params.pull_down_en = GPIO_PULLDOWN_DISABLE;
    blinds_en_interrupt_params.pull_up_en = GPIO_PULLUP_DISABLE;
    blinds_en_interrupt_params.intr_type = GPIO_INTR_POSEDGE;
    ESP_ERROR_CHECK(gpio_config(&blinds_en_interrupt_params));
    blinds_queue = xQueueCreate(1, sizeof(uint8_t));
    }

    ESP32PWM::allocateTimer(0);
    ESP32PWM::allocateTimer(1);
    ESP32PWM::allocateTimer(2);
    ESP32PWM::allocateTimer(3);

    blind_servo.setPeriodHertz(50);
    

    std::thread t1(&Room::blinds_en_btn_task, this);
    std::thread t2(&Room::light_control_task, this);
    std::thread t3(&Room::climatisation_task, this);
    t1.detach();
    t2.detach();
    t3.detach();


    gpio_isr_handler_add(GPIO_NUM_22, room_blinds_interrupt_handler, this);
}

void Room::blinds_en_btn_task(){
    vTaskDelay(2000);
    uint8_t _pressed = 0;
    while(1){
        if (xQueueReceive(blinds_queue, &_pressed, portMAX_DELAY)){
            blinds_en = !blinds_en;
            LED::getInstance() -> setLed(6, !blinds_en);
            vTaskDelay(500);
            xQueueReset(blinds_queue);
            std::cout << "Blinding " << (blinds_en ? "Enabled" : "Disabled") << std::endl;
        }
    }
}

void Room::blinds(blinds_pos pos){
    if(pos != Room::blind_pos && Room::blinds_en){
        blind_servo.attach(GPIO_NUM_21, 700, 2500);
        if(pos == UP){
            Room::blind_pos = UP;
            std::cout << "Room Blinding UP" << std::endl;
            for(int deg = 0; deg < 180; ++deg){
                blind_servo.write(deg);
                vTaskDelay(5);
            }
        }else{
            Room::blind_pos = DOWN;
            std::cout << "Room Blinding DOWN" << std::endl;
            for(int deg = 180; deg > 0; --deg){
                blind_servo.write(deg);
                vTaskDelay(5);
            }
        }
        vTaskDelay(100);
        blind_servo.detach();
    }
}

void Room::set_bracket_light(on_off stat){
    if(stat != Room::bracket_light_status){
        LED::getInstance() -> setLed(8, stat == ON);
        //gpio_set_level(Room::bracket_light, stat == ON);
        Room::bracket_light_status = stat == ON ? ON : OFF;
        std::cout << "Room Bracket Light " << (stat == ON ? "ON" : "OFF") << std::endl;
    }
}

void Room::set_ceiling_light(on_off stat){
    if(stat != Room::ceiling_light_status){
        LED::getInstance() -> setLed(9, stat == ON);
        //gpio_set_level(Room::ceiling_light, stat == ON);
        Room::ceiling_light_status = stat == ON ? ON : OFF;
        std::cout << "Room Ceiling Light " << (stat == ON ? "ON" : "OFF") << std::endl;
    }
}


void Room::light_control_task(){
    Room::blind_pos = UP;
    Room::blinds(DOWN);
    while (1){
        int pot = adc1_get_raw(adc1_channel_t(ADC1_CHANNEL_3));
        bool isBright = Sensors::getInstance()->read_light_sensor() >= 1600;
        Room::blinds( ((pot > 800) && isBright) || ((pot > 3200) && !isBright)  ? UP : DOWN);
        Room::set_bracket_light( ((pot > 1900) && isBright) || ((pot > 800) && !isBright) ? ON : OFF);
        Room::set_ceiling_light( ((pot > 3200) && isBright) || ((pot > 1900) && !isBright) ? ON : OFF);

        vTaskDelay(200);
    }
}

void Room::heating(on_off stat){
    if(stat != Room::heating_status){
        LED::getInstance() -> setLed(10, stat == ON);
        //gpio_set_level(Room::heating_pin, stat == ON);
        Room::heating_status = stat == ON ? ON : OFF;
        std::cout << "Room Heating " << (stat == ON ? "ON" : "OFF") << std::endl;
    }
}

void Room::cooling(on_off stat){
    if(stat != Room::cooling_status){
        LED::getInstance() -> setLed(11, stat == ON);
        //gpio_set_level(Room::cooling_pin, stat == ON);
        Room::cooling_status = stat == ON ? ON : OFF;
        std::cout << "Room Cooling " << (stat == ON ? "ON" : "OFF") << std::endl;
    }
}

void Room::climatisation_task(){
    while (1){
        int temp = Sensors::getInstance()->read_room_temp();
        int ideal = (int)Room::ideal_temp*100;
        //std::cout << temp << "\t\t" << ideal << std::endl;
        if(temp >= 10000){
            continue;
        }
        if(ideal +50 <= temp){
            Room::heating(OFF);
            Room::cooling(ON);
        }else if(ideal -50 >= temp){
            Room::cooling(OFF);
            Room::heating(ON);
        }else{
            Room::heating(OFF);
            Room::cooling(OFF);
        }
        vTaskDelay(2000);
    }
}
    