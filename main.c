#include <stdio.h>

#include <pico/stdlib.h>
#include <hardware/gpio.h>
#include <hardware/adc.h>

#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>


#define USB_WAIT_TIME      ( 1000U )

#define LED_PIN            ( PICO_DEFAULT_LED_PIN )

#define DEFAULT_COUNT      ( 5U )

#define tskLED_PRIORITY    ( tskIDLE_PRIORITY + 1U )
#define tskPRINT_PRIORITY  ( tskIDLE_PRIORITY + 1U )
#define tskTEMP_PRIORITY   ( tskIDLE_PRIORITY + 2U )


#define PRINT(STRING, ...)  printf(STRING"\r\n"__VA_OPT__(,) __VA_ARGS__)


static void ConfigureLed(void);
static void ConfigureTemperatureSensor(void);

static void StartBlink(uint8_t const count);
static float ReadTemperature(void);

static void FreeRTOS_Application(void);

static void vTaskLed(void * pvParameters);
static void vPrintTask(void * pvParameters);
static void vTemperatureTask(void * pvParameters);


static QueueHandle_t xQueue = NULL;


void main(void)
{   
    stdio_init_all();
    sleep_ms(USB_WAIT_TIME);

    ConfigureLed();
    ConfigureTemperatureSensor();

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

    xStatus = xTaskCreate(vPrintTask, "String Print", configMINIMAL_STACK_SIZE, NULL, tskPRINT_PRIORITY, NULL);

    if (xStatus != pdPASS)
    {
        PRINT("Failed to create String Print task!");
        goto loop;
    }
    else
    {
        PRINT("String Print task created!");
    }

    xStatus = xTaskCreate(vTemperatureTask, "Temperature", configMINIMAL_STACK_SIZE, NULL, tskTEMP_PRIORITY, NULL);

    if (xStatus != pdPASS)
    {
        PRINT("Failed to create Temperature task!");
        goto loop;
    }
    else
    {
        PRINT("Temperature task created!");
    }

    xQueue = xQueueCreate(1, sizeof(float));

    if (xQueue == NULL)
    {
        PRINT("Failed to create queue!");
        goto loop;
    }
    else
    {
        PRINT("Queue created!");
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

static void ConfigureTemperatureSensor(void)
{
    adc_init();
    adc_set_temp_sensor_enabled(true);
    adc_select_input(4);
}

static void StartBlink(uint8_t const count)
{
    for (uint8_t idx = 1; idx <= count * 2; ++idx)
    {
        gpio_put(LED_PIN, idx % 2);
        sleep_ms(50);
    }
}

static float ReadTemperature(void)
{
    static constexpr float conversionFactor = 3.3f / (1 << 12);

    float const voltage = (float) adc_read() * conversionFactor;

    return ( 27.0f - (voltage - 0.706f) / 0.001721f );
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
    (void) pvParameters;

    static const TickType_t xDelay = pdMS_TO_TICKS(1000);

    TickType_t xNextWakeTime = xTaskGetTickCount();

    static float temperature = 0.0f;

    while (true)
    {
        xQueueReceive(xQueue, &temperature, portMAX_DELAY);
        PRINT("Temperature: %.2f", ReadTemperature());
        xTaskDelayUntil(&xNextWakeTime, xDelay);
    }
}

static void vTemperatureTask(void * pvParameters)
{
    (void) pvParameters;

    static const TickType_t xDelay = pdMS_TO_TICKS(2000);

    TickType_t xNextWakeTime = xTaskGetTickCount();

    static float temperature = 0.0f;

    while (true)
    {
        temperature = ReadTemperature();
        xQueueSendToBack(xQueue, &temperature, 0);
        xTaskDelayUntil(&xNextWakeTime, xDelay);
    }
}