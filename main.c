#include <stdbool.h>

#include <pico/stdlib.h>
#include <hardware/gpio.h>

#include <FreeRTOS.h>
#include <task.h>


#define LED_PIN            ( PICO_DEFAULT_LED_PIN )

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
}

static void ConfigureLed(void)
{
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 0);
}

static void StartBlink(uint8_t const count)
{
    for (uint8_t idx = 1; idx <= count * 2; ++idx)
    {
        gpio_put(LED_PIN, idx % 2);
        sleep_ms(50);
    }
}

static void vTaskLed(void * pvParameters)
{
    (void) pvParameters;

    static const TickType_t xDelay = pdMS_TO_TICKS(1000);

    TickType_t xNextWakeTime = xTaskGetTickCount();

    while (true)
    {
        gpio_put(LED_PIN, !gpio_get(LED_PIN));
        xTaskDelayUntil(&xNextWakeTime, xDelay);
    }
}
