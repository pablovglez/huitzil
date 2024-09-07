#ifdef __TIME_ENDPOINT_H__
#define __TIME_ENDPOINT_H__

#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <curl/easy.h>

/*********************************************************************
 * @brief Get current time from a Time API
 *
 * @param[in]   uart_num    UART index (0-2) to configure
 */
void huitzil_sync(const char *address);


void get_time_from_endpoint(char * date_return, size_t date_return_size);


#endif
