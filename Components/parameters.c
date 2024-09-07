#include "parameters.h"

PersistentSettings global_params = {"", ""};

int loadPersistentSettings(const char* filename) {
    ParamEnum next = API_HOST;
    char line[CONF_LINE_SIZE];

    FILE *conf_file = fopen(filename, "r");
    if (conf_file == NULL) {
		perror("Error opening config file");
		return -1;
	}
    while (fgets(line, CONF_LINE_SIZE, conf_file)) {
    	// Trim leading and trailing whitespace
        char *trimmed_line = strtok(line, "\n");
        if (trimmed_line == NULL || trimmed_line[0] == '#' || strlen(trimmed_line) == 0) {
            continue; // Ignore comments and blank lines
        }

        // Split the line at the '=' character
        char *equal_sign = strchr(trimmed_line, '=');
        if (equal_sign == NULL) {
            continue; // Skip lines without '='
        }

        // Get the value part after the '=' character
        char *val = equal_sign + 1;
        switch (next) {
        case API_HOST:
            strcpy(global_params.API_host, val);
            next = SYNC_INTERVAL;
            break;
        case SYNC_INTERVAL:
            strcpy(global_params.sync_interval, val);
            next = PARAM_END;
            break;
        case PARAM_END:
        default:
            goto free_file;
            break;
        }
    }

free_file:
    fclose(conf_file);
    return (int)next;
}
