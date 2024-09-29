#include <stdbool.h>
#include <stdio.h>

#include <pico/stdlib.h>
#include <hardware/gpio.h>

#include <FreeRTOS.h>
#include <task.h>


#define USB_WAIT_TIME      ( 1000U )

#define LED_PIN            ( PICO_DEFAULT_LED_PIN )

#define DEFAULT_COUNT      ( 5U )

#define tskLED_PRIORITY    ( tskIDLE_PRIORITY + 1U )
#define tskPRINT_PRIORITY  ( tskIDLE_PRIORITY + 1U )


#define PRINT(STRING, ...)  printf(STRING"\r\n"__VA_OPT__(,) __VA_ARGS__)


static void ConfigureLed(void);
static void StartBlink(uint8_t const count);
static void FreeRTOS_Application(void);

static void vTaskLed(void * pvParameters);
static void vPrintTask(void * pvParameters);


void main(void)
{   
    stdio_init_all();
    sleep_ms(USB_WAIT_TIME);

    ConfigureLed();

    StartBlink(DEFAULT_COUNT);

    FreeRTOS_Application();
}

static void FreeRTOS_Application(void)
{
    BaseType_t xStatus = pdPASS;

    xStatus = xTaskCreate(vTaskLed, "LED", configMINIMAL_STACK_SIZE, NULL, tskLED_PRIORITY, NULL);

    if (xStatus != pdPASS)
    {
        PRINT("Failed to create LED task!");
        goto loop;
    }
    else
    {
        PRINT("LED task created!");
    }

    xStatus = xTaskCreate(vPrintTask, "String Print", configMINIMAL_STACK_SIZE, "Cristi", tskPRINT_PRIORITY, NULL);

    if (xStatus != pdPASS)
    {
        PRINT("Failed to create String Print task!");
        goto loop;
    }
    else
    {
        PRINT("String Print task created!");
    }

    vTaskStartScheduler();

loop:
    while (true)
    {
        tight_loop_contents();
    }
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

    static const TickType_t xDelay = pdMS_TO_TICKS(200);

    TickType_t xNextWakeTime = xTaskGetTickCount();

    while (true)
    {
        gpio_put(LED_PIN, !gpio_get(LED_PIN));
        xTaskDelayUntil(&xNextWakeTime, xDelay);
    }
}

static void vPrintTask(void * pvParameters)
{
    static const TickType_t xDelay = pdMS_TO_TICKS(2000);


    char const * const name = (char const *) pvParameters;

    TickType_t xNextWakeTime = xTaskGetTickCount();

    while (true)
    {
        PRINT("Hello %s!", name);
        xTaskDelayUntil(&xNextWakeTime, xDelay);
    }
}