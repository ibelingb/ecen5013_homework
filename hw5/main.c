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
#include <math.h>
#include "main.h"
#include "drivers/pinout.h"
#include "utils/uartstdio.h"

// TivaWare includes
#include "driverlib/sysctl.h"
#include "driverlib/debug.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"

// Drivers added to support I2C and GPIO
#include "inc/hw_i2c.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/i2c.h"
#include "driverlib/pin_map.h"

// FreeRTOS includes
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* ----------------------------------------------------------------------------- */
/* DEFINES */
#define BAUD_RATE (9600)
#define LOG_QUEUE_SIZE (10)
#define BUSY_WAIT

#define LUX_ADDR               (0x39)
#define APDS9301_ID_REG        (0x8Aul)
#define TEMP_ADDR               (0x48)
#define TMP102_TEMP_REG         (0x00ul)
#define TMP102_TEMP_SCALEFACTOR (0.0625f)

/* ----------------------------------------------------------------------------- */
/* Task and helper function prototype declarations */
void ledTask(void *pvParameters);
void temperatureTask(void *pvParameters);
void loggerTask(void *pvParameters);
float readTemp();
void i2cInit();
uint8_t i2cRead(uint8_t addr, uint8_t reg);
uint16_t i2cRead2Bytes(uint8_t addr, uint8_t reg);
void i2cWrite(uint8_t addr, uint8_t reg, uint8_t data);

/* ----------------------------------------------------------------------------- */
/* Define Logging packet structures */
typedef struct TempLogMsg {
    uint32_t timestamp;
    float temp;
} TempLogMsg;

typedef struct LedLogMsg {
    uint32_t timestamp;
    uint8_t ledNum;
    uint16_t count;
    char name[8];
} LedLogMsg;

/* Define global variables */
QueueHandle_t gxTempLogQueue;
QueueHandle_t gxLedLogQueue;
uint32_t gui32SysClock;

/* ----------------------------------------------------------------------------- */
int main(void)
{
    /* Initialize system clock to 120 MHz */
    gui32SysClock = ROM_SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), SYSTEM_CLOCK);
    ASSERT(gui32SysClock == SYSTEM_CLOCK);

    /* Setup GPIO pins for the TI TIVA Launchpad */
    PinoutSet(false, false);

    /* Initialize I2C bus on TIVA board */
    i2cInit();

    /* Create logging queues to be shared across tasks */
    gxTempLogQueue = xQueueCreate(LOG_QUEUE_SIZE, sizeof(struct TempLogMsg));
    gxLedLogQueue  = xQueueCreate(LOG_QUEUE_SIZE, sizeof(struct LedLogMsg));

    /* Define tasks to be executed by scheduler */
    /* Setting LED and Temp tasks at a higher priority since these require more specific timing */
    xTaskCreate(loggerTask, (const portCHAR *)"logger_task", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
    xTaskCreate(ledTask, (const portCHAR *)"led_task", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
    xTaskCreate(temperatureTask, (const portCHAR *)"temp_task", configMINIMAL_STACK_SIZE, NULL, 2, NULL);

    vTaskStartScheduler();
    return 0;
}

/* ----------------------------------------------------------------------------- */
void ledTask(void *pvParameters)
{
    /* Create LED Struct objects for logging */
    LedLogMsg xLed1SendMsg;
    LedLogMsg xLed2SendMsg;

    /* Clear and initialize LED Structs */
    memset(&xLed1SendMsg, 0, sizeof(xLed1SendMsg));
    memset(&xLed2SendMsg, 0, sizeof(xLed1SendMsg));
    xLed1SendMsg.ledNum = 1;
    xLed2SendMsg.ledNum = 2;
    strcpy(xLed1SendMsg.name, "Brian I");
    strcpy(xLed2SendMsg.name, "Brian I");

    /* Toggle LEDs D1 and D2 at a frequency of 10 Hz and report to logger task */
    for (;;)
    {
        /* Turn D1 LED on, all others off; populate log msg packet */
        LEDWrite(0x0F, 0x01);
        /* Log D1 LED on event */
        xLed1SendMsg.count++;
        xLed1SendMsg.timestamp = xTaskGetTickCount();
        xQueueSendToBack(gxLedLogQueue, (void*)&xLed1SendMsg, (TickType_t)0);
        /* Delay 100 msec */
        vTaskDelay(100);

        /* Turn D2 LED on, all others off; populate log msg packet */
        LEDWrite(0x0F, 0x02);
        xLed2SendMsg.count++;
        xLed2SendMsg.timestamp = xTaskGetTickCount();
        /* Log D2 LED on event */
        xQueueSendToBack(gxLedLogQueue, (void*)&xLed2SendMsg, (TickType_t)0);
        /* Delay 100 msec */
        vTaskDelay(100);
    }
}

/* ----------------------------------------------------------------------------- */
void temperatureTask(void *pvParameters)
{
    TempLogMsg xTempSendMsg;
    memset(&xTempSendMsg, 0, sizeof(xTempSendMsg));

    /* Sample temperature sensor at 1Hz rate; log event to logger task */
    for (;;)
    {
        /* Read temp from sensor and load into tempStruct; Log event */
        xTempSendMsg.temp = readTemp();
        xTempSendMsg.timestamp = xTaskGetTickCount();
        xQueueSendToBack(gxTempLogQueue, (void*)&xTempSendMsg, (TickType_t)0);

        /* Delay 1 sec */
        vTaskDelay(1000);
    }
}

/* ----------------------------------------------------------------------------- */
void loggerTask(void *pvParameters)
{
    TempLogMsg receiveTempMsg;
    LedLogMsg receiveLedMsg;
    int32_t i32TempInt, i32TempFrac; // Variables used to convert temp floating point value to integral and fractional parts for UART print
    memset(&receiveTempMsg, 0, sizeof(receiveTempMsg));
    memset(&receiveLedMsg, 0, sizeof(receiveLedMsg));

    // Set up the UART which is connected to the virtual COM port
    UARTStdioConfig(0, BAUD_RATE, SYSTEM_CLOCK);

    UARTprintf("\n\n** Start HW5 FreeRTOS Application **\n");
    UARTprintf("NOTE: Timestamps using systick counter with timestamp reported as msec since system boot\n");

    for (;;)
    {
        if(xQueueReceive(gxTempLogQueue, (void*)&receiveTempMsg, (TickType_t)0) == pdTRUE){
          /* NOTE: UARTprintf() does not support printing floating point values */
          /* Convert received floating point value to separate integral and decimal portions for printing over UART - */
          i32TempInt  = (int32_t)receiveTempMsg.temp;
          i32TempFrac = (int32_t)(receiveTempMsg.temp * 1000.0f);
          i32TempFrac = i32TempFrac - (i32TempInt * 1000);
          UARTprintf("[%d] Temp Event | Temp:{%d.%d C}\n", receiveTempMsg.timestamp, i32TempInt, i32TempFrac);
        }

        if(xQueueReceive(gxLedLogQueue, (void*)&receiveLedMsg, (TickType_t)0) == pdTRUE){
          UARTprintf("[%d] LED%d Event | Count:{%d} | Name:{%s}\n", receiveLedMsg.timestamp, receiveLedMsg.ledNum, receiveLedMsg.count, receiveLedMsg.name);
        }
    }
}

/* ----------------------------------------------------------------------------- */
// HELPER METHODS
/* ----------------------------------------------------------------------------- */
float readTemp()
{
    float temp = 0.0;
    uint16_t tempReg = 0;

    /* Read Temp value from TMP102 sensor over I2C interface */
    tempReg = i2cRead2Bytes(TEMP_ADDR, 0x00);

    /* Shift offset for temperature data in register */
    tempReg >>= 4;

    /* Apply scale factor to temperature reading and convert to float */
    temp = (tempReg * TMP102_TEMP_SCALEFACTOR);

    /* Return degrees celsius data */
    return temp;
}

/* ----------------------------------------------------------------------------- */
void i2cInit()
{
    /* Enable clocks for I2C2 and GPIO */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C2);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);

    /* Configure selected GPIO pins to be I2C SDA and SCL */
    GPIOPinConfigure(GPIO_PN4_I2C2SDA);
    GPIOPinConfigure(GPIO_PN5_I2C2SCL);
    GPIOPinTypeI2CSCL(GPIO_PORTN_BASE, GPIO_PIN_5);
    GPIOPinTypeI2C(GPIO_PORTN_BASE, GPIO_PIN_4);

    /* Initialize I2C Master clock */
    I2CMasterInitExpClk(I2C2_BASE, gui32SysClock, false);
}

/* ----------------------------------------------------------------------------- */
uint8_t i2cRead(uint8_t addr, uint8_t reg)
{
    /* Set device addr on I2C bus to Read - specify device to read */
    I2CMasterSlaveAddrSet(I2C2_BASE, addr, false);

    /*  Specify which register to read from device */
    I2CMasterDataPut(I2C2_BASE, reg);
    I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_SINGLE_SEND);

 #ifdef BUSY_WAIT
    while(!I2CMasterBusy(I2C2_BASE));
 #endif

    /* Delay until I2C is available */
    while(I2CMasterBusy(I2C2_BASE));

    /* Read byte from I2C Bus */
    I2CMasterSlaveAddrSet(I2C2_BASE, addr, true);
    I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);

 #ifdef BUSY_WAIT
    while(!I2CMasterBusy(I2C2_BASE));
 #endif

    /* Delay until I2C is available */
    while(I2CMasterBusy(I2C2_BASE));

    /* Read data */
    return I2CMasterDataGet(I2C2_BASE);
}

/* ----------------------------------------------------------------------------- */
uint16_t i2cRead2Bytes(uint8_t addr, uint8_t reg)
{
    /* Set device addr on I2C bus to Read - specify device to read */
    I2CMasterSlaveAddrSet(I2C2_BASE, addr, false);

    /*  Specify which register to read from device */
    I2CMasterDataPut(I2C2_BASE, reg);
    I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_SINGLE_SEND);

 #ifdef BUSY_WAIT
    while(!I2CMasterBusy(I2C2_BASE));
 #endif

    /* Delay until I2C is available */
    while(I2CMasterBusy(I2C2_BASE));

    /* Prepare to read data */
    I2CMasterSlaveAddrSet(I2C2_BASE, addr, true);

    /* Read Byte */
    I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START);

 #ifdef BUSY_WAIT
    while(!I2CMasterBusy(I2C2_BASE));
 #endif

    /* Delay until I2C is available */
    while(I2CMasterBusy(I2C2_BASE));

    uint32_t data_one = I2CMasterDataGet(I2C2_BASE);

    /* Read Byte */
    I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);

 #ifdef BUSY_WAIT
    while(!I2CMasterBusy(I2C2_BASE));
 #endif

    /* Delay until I2C is available */
    while(I2CMasterBusy(I2C2_BASE));

    uint32_t data_two = I2CMasterDataGet(I2C2_BASE);

    return (data_one << 8) | data_two;
}

/* ----------------------------------------------------------------------------- */
void i2cWrite(uint8_t addr, uint8_t reg, uint8_t data)
{
    /* Set device addr on I2C bus to write */
    I2CMasterSlaveAddrSet(I2C2_BASE, addr, false);

    /* Specify which register to write to */
    I2CMasterDataPut(I2C2_BASE, reg);
    I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_SINGLE_SEND);

 #ifdef BUSY_WAIT
    /* Wait until master module is says it's busy. Errata I2C#08 */
    while(!I2CMasterBusy(I2C2_BASE));
 #endif

    /* Delay until I2C is available */
    while(I2CMasterBusy(I2C2_BASE));

    /* Write data device */
    I2CMasterDataPut(I2C2_BASE, data);
    I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_SINGLE_SEND);

 #ifdef BUSY_WAIT
    /* Wait until master module is says it's busy. Errata I2C#08 */
    while(!I2CMasterBusy(I2C2_BASE));
 #endif
    /* Delay until I2C is available */
    while(I2CMasterBusy(I2C2_BASE));
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
