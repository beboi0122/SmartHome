#include <iostream>
#include <driver/gpio.h>
#include <esp_adc_cal.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <thread>
#include <esp_intr_alloc.h>
#include <garden.hpp>
#include <functional>
#include <led_controller.hpp>
#include <sensors.hpp>

static void IRAM_ATTR garden_light_interrupt_handler(void *arg)
{
  auto now = xTaskGetTickCountFromISR();
  auto garden = reinterpret_cast<Garden *>(arg);
  xQueueSendFromISR(garden->garden_queue, &now, NULL);
}

static void IRAM_ATTR irrigation_interrupt_handler(void *arg)
{
  auto pressed = 1;
  auto garden = reinterpret_cast<Garden *>(arg);
  xQueueSendFromISR(garden->irrigation_queue, &pressed, NULL);
}

Garden::Garden()
{
  garden_setup();
}

void Garden::garden_setup()
{
  {
    gpio_config_t garden_motion_interrupt_params = {};
    garden_motion_interrupt_params.pin_bit_mask = GPIO_SEL_12;
    garden_motion_interrupt_params.mode = GPIO_MODE_INPUT;
    garden_motion_interrupt_params.pull_down_en = GPIO_PULLDOWN_DISABLE;
    garden_motion_interrupt_params.pull_up_en = GPIO_PULLUP_DISABLE;
    garden_motion_interrupt_params.intr_type = GPIO_INTR_POSEDGE;
    ESP_ERROR_CHECK(gpio_config(&garden_motion_interrupt_params));
    garden_queue = xQueueCreate(10, sizeof(TickType_t));
  }

  // irrigation_en_interrupt_params
  {
    gpio_config_t irrigation_en_interrupt_params = {};
    irrigation_en_interrupt_params.pin_bit_mask = GPIO_SEL_19;
    irrigation_en_interrupt_params.mode = GPIO_MODE_INPUT;
    irrigation_en_interrupt_params.pull_down_en = GPIO_PULLDOWN_DISABLE;
    irrigation_en_interrupt_params.pull_up_en = GPIO_PULLUP_DISABLE;
    irrigation_en_interrupt_params.intr_type = GPIO_INTR_POSEDGE;
    ESP_ERROR_CHECK(gpio_config(&irrigation_en_interrupt_params));
    irrigation_queue = xQueueCreate(1, sizeof(uint8_t));
  }

  std::thread{std::mem_fn(&Garden::garden_light_task), this}.detach();
  std::thread{std::mem_fn(&Garden::irrigation_en_btn_task), this}.detach();
  std::thread{std::mem_fn(&Garden::morning_task), this}.detach();
  

  gpio_install_isr_service(0);
  gpio_isr_handler_add(GPIO_NUM_12, garden_light_interrupt_handler, this);
  gpio_isr_handler_add(GPIO_NUM_19, irrigation_interrupt_handler, this);

  ESP_ERROR_CHECK(adc1_config_width(ADC_WIDTH_BIT_12));
  ESP_ERROR_CHECK(adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_11));

  ESP_ERROR_CHECK(adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_DB_11));

  esp_adc_cal_characteristics_t adc_chars = {};
  adc_chars.coeff_b = 500;
  esp_adc_cal_value_t _val_type = esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, &adc_chars);
}

void Garden::irrigation_en_btn_task()
{
  vTaskDelay(500);
  std::cout << "Irrigation enable button task started..." << std::endl;
  uint8_t _pressed = 0;
  while (1)
  {
    if (xQueueReceive(irrigation_queue, &_pressed, portMAX_DELAY))
    {
      irrigation_en = !irrigation_en;
      LED::getInstance() -> setLed(3, !irrigation_en);
      //gpio_set_level(GPIO_NUM_17, (int)!irrigation_en);
      vTaskDelay(500);
      xQueueReset(irrigation_queue);
    }
  }
}

void Garden::irrigate()
{
  std::cout << "Irrigation task started..." << std::endl;
  if(Sensors::getInstance() -> read_soil_moisture() < 70){
    for (int i = 0; i < 15; i++)
    {
      if (irrigation_en)
      {
        LED::getInstance() -> setLed(1, true);
        //gpio_set_level(GPIO_NUM_16, 1);
        vTaskDelay(300);
        LED::getInstance() -> setLed(1, false);
        //gpio_set_level(GPIO_NUM_16, 0);
        vTaskDelay(300);
      }
      else
      {
        break;
      }
    }
  }
  std::cout << "Irrigation task finished..." << std::endl;
}

void Garden::morning_task()
{
  vTaskDelay(500);
  std::cout << "Morning observer task started..." << std::endl;
  int prev_light = 10000;
  while (1)
  {


    auto light = Sensors::getInstance() -> read_light_sensor();
    // std::cout << light << "   " << prev_light << std::endl;
    if (light - prev_light > 1000 && irrigation_en)
    {
      std::thread{std::mem_fn(&Garden::irrigate), this}.detach();
    }
    prev_light = light;
    vTaskDelay(2000);
  }
}

void Garden::garden_light_task()
{
  vTaskDelay(500);
  std::cout << "Garden light task started..." << std::endl;
  TickType_t timestamp;
  while (1)
  {
    // std::cout << "Garden light task running..." << std::endl;
    if (xQueueReceive(garden_queue, &timestamp, portMAX_DELAY))
    {
      auto light_res = Sensors::getInstance() -> read_light_sensor();
      if (light_res < 1500)
      {
        LED::getInstance() -> setLed(2, true);
        timestamp += 10000 / portTICK_PERIOD_MS;
        auto now = xTaskGetTickCount();
        if (timestamp > now)
        {
          vTaskDelay(timestamp - now);
        }
        LED::getInstance() -> setLed(2, false);
      }
    }
  }
}
