#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "Components/time_endpoint.h"


int show_help(){
	printf("Usage: huitzil [-a <argument>] | [<argument>]\n");
	printf("This program update device time by using a time API.\n");
	printf("\nOptions:\n");
	printf("  <address>       Specify the API address (default).\n");
	printf("  -a <address>    Specify the API address explicitly.\n");
	printf("  --help           Show this help message.\n");
	return 0;
}


/*********************************************************************
 * @brief Application entry point.
 */
int main(int argc, char *argv[]) {
	// Check if at least one argument is passed (excluding the program name)
	if (argc == 1 || (argc > 1 && strcmp(argv[1], "--help") == 0)) {
			show_help();
	    }
	    // If the "-a" option is provided, check for the next argument
		else if (strcmp(argv[1], "-a") == 0) {
	        if (argc > 2) {
	        	huitzil_sync(argv[2]);
	        } else {
		        printf("Option \"-a\" requires an address\n");
		    }
	    }
	    // If the first argument is not "-a", assume it is the expected argument
	    else if (argc > 1 && argv[1][0] == '-') {
	    	show_help();
	    }
	    else if(argc > 1 && argv[1][0] != '-'){
	    	huitzil_sync(argv[1]);
	    }
	    // If no arguments are provided after "-a", display an error
	    else {
	   	        printf("No argument received.\n");
		}

	    return 0;
}


/*************************** End of file ****************************/
