#include <driver/gpio.h>
#include <esp_adc_cal.h>
#include <atomic>

class Sensors{

    //static Sensors *instance;
    static std::atomic<Sensors*> instance;

    //static Sensors *instance;
    bool room_DHT_sensor_setUp = false;
    bool bathroom_DHT_sensor_setUp = false;
    bool out_DHT_sensor_setUp = false;

    Sensors(){
        ESP_ERROR_CHECK(adc1_config_width(ADC_WIDTH_BIT_12));
        ESP_ERROR_CHECK(adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_11));
    }

    public:
        //static std::mutex mtx_instance;
        static Sensors* getInstance(){
            //std::lock_guard<std::mutex> lk(mtx_instance);
           /* if(!instance){
                instance = new Sensors();
                ESP_ERROR_CHECK(adc1_config_width(ADC_WIDTH_BIT_12));
                ESP_ERROR_CHECK(adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_11));
            }
            return instance;*/
            static Sensors instance;
            return &instance;
        }




        int read_light_sensor();
        int read_room_temp();
        int read_bathroom_humidity();
        int read_soil_moisture();
        bool read_front_door_open();
        int read_bathroom_moisture();
        int read_out_temp();
        int read_out_humidity();
};

