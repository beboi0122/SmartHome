#pragma once
#include <enums.hpp>
#include <ESP32Servo.h>

static void IRAM_ATTR room_blinds_interrupt_handler(void *arg);

class Room{
private:
    friend void IRAM_ATTR room_blinds_interrupt_handler(void *arg);

    QueueHandle_t blinds_queue = nullptr;

    on_off bracket_light_status = OFF;
    //gpio_num_t bracket_light = GPIO_NUM_22;
    on_off ceiling_light_status = OFF;
    //gpio_num_t ceiling_light = GPIO_NUM_23;
    bool blinds_en = true;
    blinds_pos blind_pos = DOWN;

    Servo blind_servo;

    on_off heating_status = OFF;
    //gpio_num_t heating_pin = GPIO_NUM_25;
    on_off cooling_status = OFF;
    //gpio_num_t cooling_pin = GPIO_NUM_26;
    float ideal_temp = 23.0f;

public:
    Room();
    void room_setup();
    void light_control_task();
    void blinds(blinds_pos pos);
    void blinds_en_btn_task();
    void set_bracket_light(on_off stat);
    void set_ceiling_light(on_off stat);
    void heating(on_off stat);
    void cooling(on_off stat);
    void climatisation_task();
};
