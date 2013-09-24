/*
Copyright 2013, Jernej Kovacic

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/


/**
 * @file
 * A header file that should be included into every application's source file.
 * It provides some handy definitions, e.g. for outputting messages
 * to a UART or for creating FreeRTOS tasks with proper error handling.
 *
 * @author Jernej Kovacic
 */

#ifndef _APP_INCLUDE_H_
#define _APP_INCLUDE_H_


portBASE_TYPE printInit(void);

void printGateKeeperTask(void* params);

void vPrintMsg(const char* msg);

void vPrintChar(char ch);

void vDirectPrintMsg(const char* msg);

void vDirectPrintCh(char ch);



#endif  /* _APP_INCLUDE_H_ */