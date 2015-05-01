#include <string.h> // For strerror
#include <stdio.h>	// For FILE*
#include <stdlib.h> // For exit()
#include <errno.h>  // For errno
#include <stdbool.h>
#include "memsim.h"

int main(int argc, char* argv[]){
	FILE* config;
	char trace_name[50] = "trace_name";
	/* Check for a configuration file */
	if(argc < 3){
		/* No config so use default configuration parameters */
		fprintf(stdout, "possible usage: zcat <tracefile_name> | ./memsim <config_file> <trace_name>\n");
		printf("Using default configuration parameters.\n");
		exit(1);
	}else{
		/* Use parameters provided in the config file */
		config = fopen(argv[1], "r");
		if(config == NULL){
			fprintf(stderr, "Could not open the configuration file : %s\n", strerror(errno));
			exit(1);
		}
		read_parameters(config);
		fclose(config);
		strcpy(trace_name, argv[2]);
		strcat(trace_name, ".");
		strcat(trace_name, argv[1]);
	}
	//simulator();
	generate_report(trace_name);
	return EXIT_SUCCESS;
}

