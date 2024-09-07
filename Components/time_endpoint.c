
#include "../Components/time_endpoint.h"
#include "../Components/cJSON/cJSON.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <curl/easy.h>

//#include "cJSON.h"

/*********************************************************************
 *
 * @brief Replace the first occurrence of a char in a given string
 *
 * @param[in]	given_str	The string to look into
 *
 * @param[in]	old_char	The character to find
 *
 * @param[in]	new_char	The character to replace with
 */
void replace_char(char *given_str, char old_char, char new_char) {
    char *pos = strchr(given_str, old_char); // Find the first occurrence of old_char
    if (pos) {
        *pos = new_char; // Replace it with new_char
    }
}

/**
 * @brief Retrieves the current system date in ISO 8601 format.
 *
 * This function executes the `date -I` command to get the current system date in ISO 8601 format.
 * It reads the output of the command and stores it in the provided buffer. The function ensures
 * that the output does not exceed the buffer size.
 *
 * @param[out] output_time A buffer to store the retrieved system date.
 * @param[in] output_size The size of the output_time buffer.
 */
void get_system_time(char *output_time, size_t output_size){
	// Get current system time
	FILE *fp;
	char buffer[128];
	size_t total_copied = 0;

	// Open a process by executing a command and reading its output
	fp = popen("date -u +\"%Y-%m-%d %H\"", "r");
	if (fp == NULL) {
		perror("popen failed");
	}

	// Read the output of the command and store it in a dynamically allocated string
	while (fgets(buffer, sizeof(buffer), fp) != NULL) {
		size_t buffer_len = strlen(buffer);  // Length of the current buffer chunk

		// Check if adding this chunk will exceed the output buffer size
		if (total_copied + buffer_len >= output_size - 1) {  // Leave space for null terminator
			size_t space_left = output_size - total_copied - 1;  // Calculate remaining space
			strncat(output_time, buffer, space_left);  // Copy only the remaining space
			break;  // Stop copying since we reached the buffer size limit
		} else {
			strcat(output_time, buffer);  // Concatenate the whole buffer chunk
			total_copied += buffer_len;  // Update the number of characters copied
		}
	}

	// Close the file pointer and check for errors
	if (pclose(fp) == -1) {
		perror("pclose failed");
	}
}


/**
 * @brief Sets the system time using the provided date and time string.
 *
 * This function constructs a command to set the system time using the `date` command
 * and executes it using the `system` function. It handles errors that may occur during
 * the execution of the command.
 *
 * @param[in]	date_time	A string representing the date and time to set the system clock to.
 */
void set_system_time(const char *date_time) {
	size_t output_size = 14;
	char system_date[output_size];
	system_date[0] = '\0';
	get_system_time(system_date, output_size);

	if (strncmp(date_time, system_date, output_size-1) != 0){
		char command[30];
		snprintf(command, sizeof(command), "date -s '%s'", date_time);
		int result = system(command);

		if (result == -1) {
			perror("system");
		} else if (result != 0) {
			fprintf(stderr, "Error: system command failed with code %d\n", result);
		} else {
			printf("System time set successfully.\n");
		}
	} else {
		printf("System time is up to date.\n");
	}
}


/*********************************************************************
 *
 * @brief Callback function for handling data received from a cURL request.
 *
 * This function is called by cURL as soon as there is data received that needs to be saved.
 * The data is provided in chunks, and this function will be called multiple times if the data
 * is received in multiple chunks.
 *
 * @param[in] contents Pointer to the data received.
 * @param[in] size Size of each data chunk.
 * @param[in] nmemb Number of data chunks.
 * @param[in] userp Pointer to the user-defined data, which in this case is expected to be a pointer to a char pointer.
 * @return The total size of the data processed, which is size * nmemb.
 */
size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t totalSize = size * nmemb;
    char **responsePtr = (char **)userp;

    // Allocate memory for the response
    *responsePtr = strndup(contents, totalSize);

    return totalSize;
}

/**
 * @brief Fetches the current time from a remote endpoint and stores it in the provided buffer.
 *
 * This function uses the cURL library to send an HTTP GET request to a specified time API endpoint.
 * The response is expected to be in JSON format, containing a "dateTime" field. The function parses
 * the JSON response, extracts the "dateTime" value, and copies it to the provided buffer. The 'T'
 * character in the dateTime string is replaced with a space.
 *
 * @param[in]	date_return	A buffer to store the fetched date and time string.
 * @param[in]	date_return_size	The size of the date_return buffer.
 */
void get_time_from_endpoint(const char *address, char * date_return, size_t date_return_size){
	CURL *curl = curl_easy_init();
	    if(curl) {
	        char *response = NULL;

	        curl_easy_setopt(curl, CURLOPT_URL, address);

	        // Skip SSL peer certificate verification
	        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);

	        // Skip SSL host name verification
	        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

	        // Set the write callback function
	        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);

	        // Pass the response pointer to the callback function
	        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

	        // Perform the request, res will get the return code
	        CURLcode res = curl_easy_perform(curl);

	        // Check for errors
			if(res != CURLE_OK) {
				fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
				} else {
					// Parse the response
					cJSON *json = cJSON_Parse(response);

					// Get dateTime
					cJSON *dateTime = cJSON_GetObjectItemCaseSensitive(json, "dateTime");
						if (cJSON_IsString(dateTime) && (dateTime->valuestring != NULL)) {

							// Copy valuestring of dateTime to a new string
							char *date_string = strdup(dateTime->valuestring);
							strncpy(date_return, date_string, date_return_size);

							//Make sure date_return ends properly
							date_return[date_return_size - 1] = '\0';

							// Replace 'T' with space
							replace_char(date_return, 'T', ' ');
					    } else {
					    	fprintf(stderr, "Failed to parse time response\n");
					    }

						// Cleanup
				        cJSON_Delete(json);
				        free(response);
						curl_easy_cleanup(curl);
				}

	    }
}


/**
 * @brief Synchronizes the system time with the time obtained from a remote endpoint.
 *
 * This function initializes a buffer to store the date and time string, fetches the current time
 * from a remote endpoint using `get_time_from_endpoint`, and if a valid date and time string is
 * obtained, it sets the system time using `set_system_time`.
 */
void huitzil_sync(const char *address) {

	char date_string[20] = "";
	size_t date_string_size = 20;
	get_time_from_endpoint(address, date_string, date_string_size);
	if (date_string[0] != '\0') {
		set_system_time(date_string);
	}
}


/*************************** End of file ****************************/
