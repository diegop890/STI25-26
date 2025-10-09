/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
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

//Declaración de variables globales
int accion = 1;

//Declaración de funciones callback
void isrTimerSemaforo(void *);

//Handles
esp_timer_handle_t h_timerSemaforo;

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

    //Configuración y creación del timer del semáforo
    esp_timer_create_args_t configTimerSemaforo = {
        .callback = isrTimerSemaforo
    };

    esp_timer_create(&configTimerSemaforo, &h_timerSemaforo);

    int accionSig = 1;
    accion = 1;

    while(1)
    {
        //Encender led verde
        if (accion == 1 && accionSig == 1)
        {
            gpio_set_level(PinLedRojo, 0);
            gpio_set_level(PinLedVerde, 1);

            accionSig = 2;
            esp_timer_start_once(h_timerSemaforo, 5000000);
        }
        else if (accion == 2 && accionSig == 2)
        {
            //Encender led amarillo
            gpio_set_level(PinLedVerde, 0);
            gpio_set_level(PinLedAmarillo, 1);

            accionSig = 3;
            esp_timer_start_once(h_timerSemaforo, 1000000);
        }
        else if (accion == 3 && accionSig == 3)
        {
            //Encender led rojo
            gpio_set_level(PinLedAmarillo, 0);
            gpio_set_level(PinLedRojo, 1);

            accionSig = 1;
            esp_timer_start_once(h_timerSemaforo, 4000000);
        }
        else
        {
            vTaskDelay(10);
        }
    }
}

void isrTimerSemaforo(void * args)
{
    if (accion > 0 && accion < 3)
    {
        accion++;
    }
    else
    {
        accion = 1;
    }
}
