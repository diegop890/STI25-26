#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_log.h"

// Pines de LEDs
#define LED_ROJO     32
#define LED_AMARILLO 33
#define LED_VERDE    25

// Pines de botones
#define BTN_1        34
#define BTN_2        35

// Máscara de pines para configuración
#define LED_MASK ((1ULL << LED_ROJO) | (1ULL << LED_AMARILLO) | (1ULL << LED_VERDE))
#define BTN_MASK ((1ULL << BTN_1) | (1ULL << BTN_2))

// Estado del semáforo
int estado = 0;

// Prototipo del callback
void cambiar_estado(void* arg);

// Handle del timer
esp_timer_handle_t semaforo_timer;

void configurar_gpio() 
{
    // Configurar LEDs como salida
    gpio_config_t io_conf_leds = {
        .pin_bit_mask = LED_MASK,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = 0,
        .pull_down_en = 0,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf_leds);

    // Configurar botones como entrada (IMPORTANTE: usar resistencias externas)
    gpio_config_t io_conf_btns = {
        .pin_bit_mask = BTN_MASK,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = 0,
        .pull_down_en = 0,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf_btns);
}

void cambiar_estado(void* arg) 
{
    estado = (estado + 1) % 3;
}

void actualizar_leds() 
{
    gpio_set_level(LED_ROJO,     (estado == 2));
    gpio_set_level(LED_AMARILLO, (estado == 1));
    gpio_set_level(LED_VERDE,    (estado == 0));
}

void app_main(void)
{
    configurar_gpio();

    // Apagar todos los LEDs al inicio
    gpio_set_level(LED_ROJO, 0);
    gpio_set_level(LED_AMARILLO, 0);
    gpio_set_level(LED_VERDE, 0);

    // Crear el timer del semáforo
    esp_timer_create_args_t timer_args = {
        .callback = &cambiar_estado,
        .name = "semaforo_timer"
    };
    esp_timer_create(&timer_args, &semaforo_timer);


    while (1) 
    {
        // Leer botones
        int btn1 = gpio_get_level(BTN_1);
        int btn2 = gpio_get_level(BTN_2);

        if (btn1 == 1) 
        {
            estado = 2;
            vTaskDelay(pdMS_TO_TICKS(300));
        }

        if (btn2 == 1) 
        {
            gpio_set_level(LED_ROJO, 0);
            gpio_set_level(LED_AMARILLO, 0);
            gpio_set_level(LED_VERDE, 1);

            esp_timer_start_once(semaforo_timer, 5000000);

            gpio_set_level(LED_ROJO, 0);
            gpio_set_level(LED_AMARILLO, 1);
            gpio_set_level(LED_VERDE, 0);

            esp_timer_start_once(semaforo_timer, 1000000);

            gpio_set_level(LED_ROJO, 1);
            gpio_set_level(LED_AMARILLO, 0);
            gpio_set_level(LED_VERDE, 0);

            esp_timer_start_once(semaforo_timer, 4000000);
        }
    }
}
