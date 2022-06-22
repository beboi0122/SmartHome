#pragma once

static void IRAM_ATTR irrigation_interrupt_handler(void *arg);
static void IRAM_ATTR garden_light_interrupt_handler(void *arg);

class Garden
{
private:
  friend void IRAM_ATTR irrigation_interrupt_handler(void *arg);
  friend void IRAM_ATTR garden_light_interrupt_handler(void *arg);

  QueueHandle_t garden_queue = nullptr;
  QueueHandle_t irrigation_queue = nullptr;
  bool irrigation_en = true;

public:
  Garden();
  void garden_setup();
  void irrigation_en_btn_task();
  void irrigate();
  void morning_task();
  void garden_light_task();
  int read_light_sensor();


  //#include <DHT.h>
  //DHT dht(GPIO_NUM_33, DHT22);

  //void temperature_task();
};