#include <sensors.hpp>
#include <mutex>
#include <DHT.h>
#include <iostream>

//Sensors *Sensors::instance = nullptr;

static DHT room_dht(GPIO_NUM_25, DHT22);
static DHT bathroom_dht(GPIO_NUM_32, DHT11);
static DHT out_dht(GPIO_NUM_33, DHT22);

std::mutex mtx_light_sensor;
int Sensors::read_light_sensor(){
    std::lock_guard<std::mutex> lk(mtx_light_sensor);
    return adc1_get_raw(adc1_channel_t(ADC1_CHANNEL_0));
}

std::mutex mtx_room_temp;
int Sensors::read_room_temp(){
    std::lock_guard<std::mutex> lk(mtx_room_temp);
    if(!Sensors::room_DHT_sensor_setUp){
        room_dht.begin();
        room_DHT_sensor_setUp = true;
    }
    std::cout << (int)(room_dht.readTemperature()*100) << std::endl;
    return room_dht.readTemperature()*100;
}

std::mutex mtx_bathroom_humidity;
int Sensors::read_bathroom_humidity(){
    std::lock_guard<std::mutex> lk(mtx_bathroom_humidity);
    if(!Sensors::bathroom_DHT_sensor_setUp){
        bathroom_dht.begin();
        bathroom_DHT_sensor_setUp = true;
    }
    //std::cout << (int)bathroom_dht.readHumidity() << std::endl;
    return bathroom_dht.readHumidity();
}

std::mutex mtx_soil_moisture;
int Sensors::read_soil_moisture(){
    std::lock_guard<std::mutex> lk(mtx_soil_moisture);
    auto norm = adc1_get_raw(adc1_channel_t(ADC1_CHANNEL_6));
    norm *= 100;
    norm /= 3300;
    std::cout << "measuring soil moisture: " << norm << "%" << std::endl;
    return norm;
}


std::mutex mtx_front_door_open;
bool Sensors::read_front_door_open(){
    std::lock_guard<std::mutex> lk(mtx_front_door_open);
    int level = gpio_get_level(GPIO_NUM_18);
    //std::cout << "checking if door open: " << !level << std::endl;
    return (bool)level;
}

std::mutex mtx_bathroom_moisture;
int Sensors::read_bathroom_moisture(){
    std::lock_guard<std::mutex> lk(mtx_bathroom_moisture);
    return adc1_get_raw(adc1_channel_t(ADC1_CHANNEL_7));
}


std::mutex mtx_out_dht;

int Sensors::read_out_temp(){
    std::lock_guard<std::mutex> lk(mtx_out_dht);
        if(!Sensors::out_DHT_sensor_setUp){
        out_dht.begin();
        out_DHT_sensor_setUp = true;
    }
    return out_dht.readTemperature();
}

int Sensors::read_out_humidity(){
    std::lock_guard<std::mutex> lk(mtx_out_dht);
    if(!Sensors::out_DHT_sensor_setUp){
        out_dht.begin();
        out_DHT_sensor_setUp = true;
    }
    return out_dht.readHumidity();
}