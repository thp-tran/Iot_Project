#ifndef DATA_COLLECTOR_H
#define DATA_COLLECTOR_H

#include "global.h"
#include <FS.h>
#include <LittleFS.h>

void data_collector_task(void *pvParameters);
void data_collector(void *pvParameters);

#endif
