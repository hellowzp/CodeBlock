#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "config.h"
#include "sbuffer.h"
#include "util.h"

sbuffer_t* buffer;

void sbuffer_data_print(sbuffer_data_t* element) {
    sensor_data_t e = element->value;
    printf("sensor_date id:%d temperature:%4.2f %s",
            e.id, e.value, asctime(localtime(&e.ts)));
}

void sbuffer_data_copy(sbuffer_data_t** dest, sbuffer_data_t* src) {
    memcpy(*dest, src, sizeof(sbuffer_data_t));
}

void sbuffer_data_free(sbuffer_data_t** element) {
    sensor_data_t e = (*element)->value;
    printf("free sensor_date id:%d temperature:%4.2f %s",
            e.id, e.value, asctime(localtime(&e.ts)));
}


int main(void) {
    if (sbuffer_init(&buffer) != SBUFFER_SUCCESS) {
        ERROR_PRINT("error initializing sbuffer");
        exit(EXIT_FAILURE);
    }
    return 0;
}

