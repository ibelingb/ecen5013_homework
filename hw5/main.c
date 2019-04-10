/* ECEN5013 - Advanced Embedded Software Development
 * HW5 | 4/10/2019
 *
 * @file: main.c
 * @author: Brian Ibeling
 *
 * References & Resources:
 *  - Leveraged example FreeRTOS/TI TIVA Demo code found at the following Github Link to create baseline for HW:
 *      https://github.com/akobyl/TM4C129_FreeRTOS_Demo
 */
/* ----------------------------------------------------------------------------- */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "main.h"
#include "drivers/pinout.h"
#include "utils/uartstdio.h"

// TivaWare includes
#include "driverlib/sysctl.h"
#include "driverlib/debug.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"

// FreeRTOS includes
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* ----------------------------------------------------------------------------- */
/* DEFINES */
#define LOG_QUEUE_SIZE (10)

/* ----------------------------------------------------------------------------- */
/* Task prototype declarations */
void ledTask(void *pvParameters);
void loggerTask(void *pvParameters);
void temperatureTask(void *pvParameters);

/* ----------------------------------------------------------------------------- */
/* Define Logging packet structure */
typedef struct LoggingMsg {
    uint8_t msgId;
    char ucMessage[30];
} LoggingMsg;

/* Define global variables */
QueueHandle_t gxLoggingQueue;

/* ----------------------------------------------------------------------------- */
int main(void)
{
    /* Initialize system clock to 120 MHz */
    uint32_t output_clock_rate_hz;
    output_clock_rate_hz = ROM_SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), SYSTEM_CLOCK);
    ASSERT(output_clock_rate_hz == SYSTEM_CLOCK);

    /* Setup GPIO pins for the TI TIVA Launchpad */
    PinoutSet(false, false);

    /* Create logging queue to be shared across tasks */
    gxLoggingQueue = xQueueCreate(LOG_QUEUE_SIZE, sizeof(struct LoggingMsg));

    /* Define tasks to be executed by scheduler */
    xTaskCreate(ledTask, (const portCHAR *)"led_task", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

    xTaskCreate(loggerTask, (const portCHAR *)"logger_task", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

//    xTaskCreate(temperatureTask, (const portCHAR *)"temp_task", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

    vTaskStartScheduler();
    return 0;
}

/* ----------------------------------------------------------------------------- */
// Flash the LEDs on the launchpad
void ledTask(void *pvParameters)
{
    LoggingMsg logMsg;

    /* Toggle LEDs D1 and D2 at a frequency of 10 Hz */
    for (;;)
    {
        /* Turn D1 LED on; all others off */
        LEDWrite(0x0F, 0x01);
        /* Log D1 LED on event */
        strcpy(logMsg.ucMessage, "D1 LED On\n");
        xQueueSend(gxLoggingQueue, (void*)&logMsg, (TickType_t)0);
        /* Delay 10 sec */
//        vTaskDelay(10000);
        vTaskDelay(5000);

        /* Turn D2 LED on; all others off */
        LEDWrite(0x0F, 0x02);
        /* Log D1 LED on event */
        strcpy(logMsg.ucMessage, "D2 LED On\n");
        xQueueSend(gxLoggingQueue, (void*)&logMsg, (TickType_t)0);
        /* Delay 10 sec */
//        vTaskDelay(10000);
        vTaskDelay(5000);
    }
}

/* ----------------------------------------------------------------------------- */
// Write text over the Stellaris debug interface UART port
void loggerTask(void *pvParameters)
{
    LoggingMsg logMsg;

    // Set up the UART which is connected to the virtual COM port
    UARTStdioConfig(0, 9600, SYSTEM_CLOCK);

    for (;;)
    {
//        UARTprintf("ECEN5013 Project2 Running\n");
//        vTaskDelay(5000 / portTICK_PERIOD_MS);

        xQueueReceive(gxLoggingQueue, (void*)&logMsg, (TickType_t)0);
        UARTprintf(logMsg.ucMessage);
        vTaskDelay(1000);
    }
}

/* ----------------------------------------------------------------------------- */
void temperatureTask(void *pvParameters)
{
    for (;;)
    {


    }
}
/* ----------------------------------------------------------------------------- */
/*  ASSERT() Error function
 *
 *  failed ASSERTS() from driverlib/debug.h are executed in this function
 */
void __error__(char *pcFilename, uint32_t ui32Line)
{
    // Place a breakpoint here to capture errors until logging routine is finished
    while (1)
    {
    }
}
