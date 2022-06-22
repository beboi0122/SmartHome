#include <iostream>
#include <driver/gpio.h>
#include <esp_adc_cal.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <thread>
#include <esp_intr_alloc.h>
#include <kitchen.hpp>
#include <functional>
#include <led_controller.hpp>
#include <sensors.hpp>
#include <atomic>


static void IRAM_ATTR light_interrupt_handler(void *arg)
{
  auto pressed = 1;
  auto kitchen = reinterpret_cast<Kitchen *>(arg);
  xQueueSendFromISR(kitchen->light_queue, &pressed, NULL);
}

Kitchen::Kitchen(){
    Kitchen::kitchen_setup();
}

void Kitchen::kitchen_setup(){

  {
    gpio_config_t light_interrupt_params = {};
    light_interrupt_params.pin_bit_mask = GPIO_SEL_17;
    light_interrupt_params.mode = GPIO_MODE_INPUT;
    light_interrupt_params.pull_down_en = GPIO_PULLDOWN_DISABLE;
    light_interrupt_params.pull_up_en = GPIO_PULLUP_DISABLE;
    light_interrupt_params.intr_type = GPIO_INTR_POSEDGE;
    ESP_ERROR_CHECK(gpio_config(&light_interrupt_params));
    light_queue = xQueueCreate(1, sizeof(uint8_t));
  }

  std::thread{std::mem_fn(&Kitchen::light_btn_task), this}.detach();
  std::thread{std::mem_fn(&Kitchen::door_observer_task), this}.detach();

  gpio_isr_handler_add(GPIO_NUM_17, light_interrupt_handler, this);
}

std::atomic<bool> light_on(false);

std::atomic<bool> small_light_on(false);

void Kitchen::door_observer_task()
{
  vTaskDelay(500);
  std::cout << "Door observer button task started..." << std::endl;
  uint8_t _pressed = 0;
  while (1)
  {
    small_light_on = Sensors::getInstance() -> read_front_door_open();
    LED::getInstance() -> setLed(5, !small_light_on);
    //std::cout << small_light_on << std::endl;
    vTaskDelay(600);
  }
}

void Kitchen::light_btn_task()
{
  vTaskDelay(500);
  std::cout << "Kitchen light button task started..." << std::endl;
  uint8_t _pressed = 0;
  while (1)
  {
    if (xQueueReceive(light_queue, &_pressed, portMAX_DELAY))
    {
      light_on = !light_on;
      LED::getInstance() -> setLed(4, light_on);
      std::cout << "Kitchen light: " << light_on << std::endl; 
      vTaskDelay(500);
      xQueueReset(light_queue);
    }
  }
}