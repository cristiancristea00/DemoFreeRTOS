#include <stdbool.h>

#include <pico/stdlib.h>
#include <hardware/gpio.h>

#include <FreeRTOS.h>
#include <task.h>


#define DEFAULT_COUNT      ( 5U )

#define tskLED_PRIORITY    ( tskIDLE_PRIORITY + 1U )


static void ConfigureLed(void);
static void StartBlink(uint8_t const count);
static void FreeRTOS_Application(void);
static void vTaskLed(void * pvParameters);


void main(void)
{   
    ConfigureLed();

    StartBlink(DEFAULT_COUNT);

    FreeRTOS_Application();
}

static void FreeRTOS_Application(void)
{
    BaseType_t xStatus = xTaskCreate(vTaskLed, "LED", configMINIMAL_STACK_SIZE, NULL, tskLED_PRIORITY, NULL);

    if (xStatus != pdPASS)
    {
        while (true)
        {
            tight_loop_contents();
        }
    }

    vTaskStartScheduler();

    while (true)
    {
        tight_loop_contents();
    }
}

static void ConfigureLed(void)
{
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    gpio_put(PICO_DEFAULT_LED_PIN, 0);
}

static void StartBlink(uint8_t const count)
{
    for (uint8_t idx = 1; idx <= count; ++idx)
    {
        gpio_put(PICO_DEFAULT_LED_PIN, 1);
        sleep_ms(100);
        gpio_put(PICO_DEFAULT_LED_PIN, 0);
        sleep_ms(100);
    }
}

static void vTaskLed(void * pvParameters)
{
    (void) pvParameters;

    while (true)
    {
        gpio_put(PICO_DEFAULT_LED_PIN, 1);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        gpio_put(PICO_DEFAULT_LED_PIN, 0);
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}