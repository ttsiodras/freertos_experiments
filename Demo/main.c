/*
 * Copyright 2013, 2017, Jernej Kovacic
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software. If you wish to use our Amazon
 * FreeRTOS name, please do so in a fair use way that does not cause confusion.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */


/**
 * @file
 * A simple demo application.
 *
 * @author Jernej Kovacic
 */


#include <stddef.h>

#include <FreeRTOS.h>
#include <task.h>

#include "app_config.h"
#include "print.h"
#include "receive.h"


/*
 * This diagnostic pragma will suppress the -Wmain warning,
 * raised when main() does not return an int
 * (which is perfectly OK in bare metal programming!).
 *
 * More details about the GCC diagnostic pragmas:
 * https://gcc.gnu.org/onlinedocs/gcc/Diagnostic-Pragmas.html
 */
#pragma GCC diagnostic ignored "-Wmain"


/* Struct with settings for each task */
typedef struct _paramStruct
{
    portCHAR* text;                  /* text to be printed by the task */
    UBaseType_t  delay;              /* delay in milliseconds */
} paramStruct;

/* Default parameters if no parameter struct is available */
static const portCHAR defaultText[] = "<NO TEXT>\r\n";
static const UBaseType_t defaultDelay = 1000;


/* Task function - may be instantiated in multiple tasks */
void vTaskFunction( void *pvParameters )
{
    const portCHAR* taskName;
    UBaseType_t  delay;
    paramStruct* params = (paramStruct*) pvParameters;

    taskName = ( NULL==params || NULL==params->text ? defaultText : params->text );
    delay = ( NULL==params ? defaultDelay : params->delay);

    for( ; ; )
    {
        /* Print out the name of this task. */

        vPrintMsg(taskName);

        vTaskDelay( delay / portTICK_RATE_MS );
    }

    /*
     * If the task implementation ever manages to break out of the
     * infinite loop above, it must be deleted before reaching the
     * end of the function!
     */
    vTaskDelete(NULL);
}


/* Fixed frequency periodic task function - may be instantiated in multiple tasks */
void vPeriodicTaskFunction(void* pvParameters)
{
    const portCHAR* taskName;
    UBaseType_t delay;
    paramStruct* params = (paramStruct*) pvParameters;
    TickType_t lastWakeTime;

    taskName = ( NULL==params || NULL==params->text ? defaultText : params->text );
    delay = ( NULL==params ? defaultDelay : params->delay);

    /*
     * This variable must be initialized once.
     * Then it will be updated automatically by vTaskDelayUntil().
     */
    lastWakeTime = xTaskGetTickCount();

    for( ; ; )
    {
        /* Print out the name of this task. */

        vPrintMsg(taskName);

        /*
         * The task will unblock exactly after 'delay' milliseconds (actually
         * after the appropriate number of ticks), relative from the moment
         * it was last unblocked.
         */
        vTaskDelayUntil( &lastWakeTime, delay / portTICK_RATE_MS );
    }

    /*
     * If the task implementation ever manages to break out of the
     * infinite loop above, it must be deleted before reaching the
     * end of the function!
     */
    vTaskDelete(NULL);
}


/*
 * A convenience function that is called when a FreeRTOS API call fails
 * and a program cannot continue. It prints a message (if provided) and
 * ends in an infinite loop.
 */
static void FreeRTOS_Error(const portCHAR* msg)
{
    if ( NULL != msg )
    {
        vDirectPrintMsg(msg);
    }

    for ( ; ; );
}

int tom = 0;
int jerry = 1;
int popeye = 2;

void printNumber(int nr)
{
    char msg[10];
    int i=9;

    msg[i--] = 0;
    while(nr) {
        msg[i--] = '0' + (nr % 10);
        nr = nr/10;
    }
    vDirectPrintMsg(&msg[i+1]);
}

void pinky(void *pvParameters)
{
    (void)pvParameters;
    while (1)
    {
        tom++;
        if ((tom & 8388607) == 8388607) {
            vDirectPrintMsg("tom: ");
            printNumber(tom >> 23);
            vDirectPrintMsg("\r\n");
        }
    }
}
void brain(void *pvParameters)
{
    (void)pvParameters;
    while (1)
    {
        jerry++;
        if ((jerry & 8388607) == 8388607) {
            vDirectPrintMsg("jerry: ");
            printNumber(jerry >> 23);
            vDirectPrintMsg("\r\n");
        }
    }
}
void droopy(void *pvParameters)
{
    (void)pvParameters;
    while (1)
    {
        popeye++;
        if ((popeye & 8388607) == 8388607) {
            vDirectPrintMsg("popeye: ");
            printNumber(popeye >> 23);
            vDirectPrintMsg("\r\n");
        }
    }
}

/* Startup function that creates and runs two FreeRTOS tasks */
void main(void)
{
    /* Init of print related tasks: */
    if ( pdFAIL == printInit(PRINT_UART_NR) )
    {
        FreeRTOS_Error("Initialization of print failed\r\n");
    }

    /*
     * I M P O R T A N T :
     * Make sure (in startup.s) that main is entered in Supervisor mode.
     * When vTaskStartScheduler launches the first task, it will switch
     * to System mode and enable interrupt exceptions.
     */
    vDirectPrintMsg("= = = T E S T   S T A R T E D = = =\r\n\r\n");

    static StaticTask_t xTaskBuffer, xTaskBuffer2, xTaskBuffer3;
    static StackType_t xStack[configMINIMAL_STACK_SIZE];
    static StackType_t xStack2[configMINIMAL_STACK_SIZE];
    static StackType_t xStack3[configMINIMAL_STACK_SIZE];

    TaskHandle_t xHandle;
    TaskHandle_t xHandle2;


    /* Create the task without using any dynamic memory allocation. */
    xHandle = xTaskCreateStatic(
        pinky,                    /* Function that implements the task. */
        "Pinky",                  /* Text name for the task. */
        configMINIMAL_STACK_SIZE, /* Number of indexes in the xStack array. */
        (void *)NULL,             /* Parameter passed into the task. */
        tskIDLE_PRIORITY,         /* Priority at which the task is created. */
        &xStack[0],               /* Array to use as the task's stack. */
        &xTaskBuffer);            /* Variable to hold the task's data structure. */

    xHandle2 = xTaskCreateStatic(
        brain,                    /* Function that implements the task. */
        "Brain",                  /* Text name for the task. */
        configMINIMAL_STACK_SIZE, /* Number of indexes in the xStack array. */
        (void *)NULL,             /* Parameter passed into the task. */
        tskIDLE_PRIORITY,         /* Priority at which the task is created. */
        &xStack2[0],               /* Array to use as the task's stack. */
        &xTaskBuffer2);            /* Variable to hold the task's data structure. */

    xHandle = xTaskCreateStatic(
        droopy,                    /* Function that implements the task. */
        "Popeye",                  /* Text name for the task. */
        configMINIMAL_STACK_SIZE, /* Number of indexes in the xStack array. */
        (void *)NULL,             /* Parameter passed into the task. */
        tskIDLE_PRIORITY,         /* Priority at which the task is created. */
        &xStack3[0],               /* Array to use as the task's stack. */
        &xTaskBuffer3);            /* Variable to hold the task's data structure. */

    (void) xHandle;
    (void) xHandle2;

    /* Start the FreeRTOS scheduler */
    vTaskStartScheduler();

    /*
     * If all goes well, vTaskStartScheduler should never return.
     * If it does return, typically not enough heap memory is reserved.
     */

    FreeRTOS_Error("Could not start the scheduler!!!\r\n");

    /* just in case if an infinite loop is somehow omitted in FreeRTOS_Error */
    for ( ; ; );
}

void vApplicationGetIdleTaskMemory(
    StaticTask_t **ppxIdleTaskTCBBuffer,
    StackType_t **ppxIdleTaskStackBuffer,
    uint32_t *pulIdleTaskStackSize )
{
    /* If the buffers to be provided to the Idle task are declared inside this
    function then they must be declared static - otherwise they will be allocated on
    the stack and so not exists after this function exits. */
    static StaticTask_t xIdleTaskTCB;
    static StackType_t uxIdleTaskStack[ configMINIMAL_STACK_SIZE ];

    /* Pass out a pointer to the StaticTask_t structure in which the Idle task's
    state will be stored. */
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

    /* Pass out the array that will be used as the Idle task's stack. */
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;

    /* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
    Note that, as the array is necessarily of type StackType_t,
    configMINIMAL_STACK_SIZE is specified in words, not bytes. */
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}
