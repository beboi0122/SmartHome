#pragma once
#include <atomic>

static void IRAM_ATTR light_interrupt_handler(void *arg);
static void IRAM_ATTR door_open_interrupt_handler(void *arg);


class Kitchen
{
private:
  friend void IRAM_ATTR light_interrupt_handler(void *arg);
  friend void IRAM_ATTR door_open_interrupt_handler(void *arg);


  QueueHandle_t light_queue = nullptr;
  QueueHandle_t door_queue = nullptr;


public:
  Kitchen();
  void kitchen_setup();
  void light_btn_task();
  void auto_light();
  void door_observer_task();
};