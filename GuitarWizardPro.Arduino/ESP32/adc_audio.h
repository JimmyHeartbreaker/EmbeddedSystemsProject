#pragma once


#include <driver/adc.h>

#define AA_BUFFER_SIZE 1000
int16_t AA_buffer[AA_BUFFER_SIZE];

void AA_setup();

