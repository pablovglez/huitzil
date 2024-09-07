#ifndef __G_PARAMS_H__
#define __G_PARAMS_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define CONF_LINE_SIZE  128
#define MAX_SETTING_SIZE  128

typedef enum {
    API_HOST,
    SYNC_INTERVAL,
    PARAM_END,
} ParamEnum;

typedef struct Settings {
  char API_host[MAX_SETTING_SIZE];
  char sync_interval[MAX_SETTING_SIZE];
} PersistentSettings;

extern PersistentSettings global_params;

/**
 * @brief Read persistent parameters from a file
 *
 * @return Number of parameters successfully read
 */
int loadPersistentSettings(const char* filename);

#endif
