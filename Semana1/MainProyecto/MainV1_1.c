/* Descripción: Semáforo
 * 
 *
 * 
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_log.h"

//Constantes
#define PinLedRojo 32
#define PinLedAmarillo 33
#define PinLedVerde 25

#define MaskPinesLeds (1ULL<<PinLedRojo) | (1ULL<<PinLedAmarillo) | (1ULL<<PinLedVerde)

void app_main(void)
{
    gpio_config_t configPinesLeds = {
        .pin_bit_mask = MaskPinesLeds,
        .mode = GPIO_MODE_INPUT_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
   
    gpio_config(&configPinesLeds);
   
    //Poner todos los leds a 0
        gpio_set_level(PinLedRojo, 0);
        gpio_set_level(PinLedAmarillo, 0);
        gpio_set_level(PinLedVerde, 0);

    while(1)
    {
        //Encender led verde
        gpio_set_level(PinLedRojo, 0);
        gpio_set_level(PinLedVerde, 1);

        //Hacer delay de 5 segundos
        vTaskDelay(pdMS_TO_TICKS(5000));

        //Encender led amarillo
        gpio_set_level(PinLedVerde, 0);
        gpio_set_level(PinLedAmarillo, 1);

        //Hacer delay de 1 segundo
        vTaskDelay(pdMS_TO_TICKS(1000));

        //Encender led rojo
        gpio_set_level(PinLedAmarillo, 0);
        gpio_set_level(PinLedRojo, 1);

        //Hacer delay de 4 segundos
        vTaskDelay(pdMS_TO_TICKS(4000));
    }
}
