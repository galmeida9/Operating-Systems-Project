#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

enum param_types {
    PARAM_BENDCOST = (unsigned char)'b',
    PARAM_XCOST    = (unsigned char)'x',
    PARAM_YCOST    = (unsigned char)'y',
    PARAM_ZCOST    = (unsigned char)'z',
};

enum param_defaults {
    PARAM_DEFAULT_BENDCOST = 1,
    PARAM_DEFAULT_XCOST    = 1,
    PARAM_DEFAULT_YCOST    = 1,
    PARAM_DEFAULT_ZCOST    = 2,
};

/*bool_t global_doPrint = TRUE;*/
char* global_inputFile = NULL;
long global_params[256]; /* 256 = ascii limit */


/* =============================================================================
 * displayUsage
 * =============================================================================
 */
static void displayUsage (const char* appName){
    printf("Usage: %s [options]\n", appName);
    puts("\nOptions:                            (defaults)\n");
    printf("    b <INT>    [b]end cost          (%i)\n", PARAM_DEFAULT_BENDCOST);
    /*printf("    p          [p]rint routed maze  (false)\n");*/
    printf("    x <UINT>   [x] movement cost    (%i)\n", PARAM_DEFAULT_XCOST);
    printf("    y <UINT>   [y] movement cost    (%i)\n", PARAM_DEFAULT_YCOST);
    printf("    z <UINT>   [z] movement cost    (%i)\n", PARAM_DEFAULT_ZCOST);
    printf("    h          [h]elp message       (false)\n");
    exit(1);
}


/* =============================================================================
 * setDefaultParams
 * =============================================================================
 */
static void setDefaultParams (){
    global_params[PARAM_BENDCOST] = PARAM_DEFAULT_BENDCOST;
    global_params[PARAM_XCOST]    = PARAM_DEFAULT_XCOST;
    global_params[PARAM_YCOST]    = PARAM_DEFAULT_YCOST;
    global_params[PARAM_ZCOST]    = PARAM_DEFAULT_ZCOST;
}


/* =============================================================================
 * parseArgs
 * =============================================================================
 */
static int parseArgs (long argc, char* const argv[]){
    long i;
    long opt;
    int file_i=-1;

    opterr = 0;

    setDefaultParams();

    while ((opt = getopt(argc, argv, "hb:px:y:z:")) != -1) {
        switch (opt) {
            case 'b':
            case 'x':
            case 'y':
            case 'z':
                global_params[(unsigned char)opt] = atol(optarg);
                break;
            case 'p':
                /*global_doPrint = TRUE;*/
                break;
            case '?':
            case 'h':
            default:
                opterr++;
                break;
        }
    }

    for (i = optind; i < argc; i++) {
    	FILE* fp=NULL;
    	fp = fopen(argv[i], "r");
    	if (fp!=NULL){
    		fclose(fp);
    		if (file_i != -1){
      			return -1; /*More than one file crashes*/
    		}
    		file_i=i;
    	}
    	else{
        	fprintf(stderr, "Non-option argument: %s\n", argv[i]);
        	opterr++;
        }
    }
    if (file_i != -1) return file_i;


/* =============================================================================
 * getOutputFile
 * =============================================================================
 */

FILE* getOutputFile(char* old_path){
	FILE* output=NULL;
	char *path = strdup(old_path), *path_aux;
	const char *res=".res", *old=".old";
	strcat(path, res); 			/*Creates path for the new file*/
	path_aux = strdup(path); 	/*To check if .old exists*/
	if (access(path, F_OK)==0){
		strcat(path_aux, old);
		if (access(path_aux, F_OK)==0) remove(path_aux);
		rename(path, path_aux);
	}
	output = fopen(path, "w");
	return output;

}

/* =============================================================================
 * main
 * =============================================================================
 */

int main(int argc, char **argv) {
	char *path;
	int i, nthreads, file_i;
	pthread_t *threads;
	FILE *file_input=NULL, *file_output=NULL;

	file_i = parseArgs(argc, (char** const)argv);

	if ((argc == 4) && (strcmp(argv[2], "-t") == 0)) {
		nthreads = atoi(argv[3]);
		threads = (pthread_t *) malloc(sizeof(pthread_t) * nthreads);
		path = strdup(argv[1]);
	}
	else exit(-1);
	
	file_input = fopen(argv[file_i], "r");
	maze_t* mazePtr = maze_alloc();
    	assert(mazePtr);

    	long numPathToRoute = maze_read(mazePtr, file_input, file_output);
    	router_t* routerPtr = router_alloc(global_params[PARAM_XCOST],
                                       global_params[PARAM_YCOST],
                                       global_params[PARAM_ZCOST],
                                       global_params[PARAM_BENDCOST]);
    	assert(routerPtr);
    	list_t* pathVectorListPtr = list_alloc(NULL);
    	assert(pathVectorListPtr);

    	router_solve_arg_t routerArg = {routerPtr, mazePtr, pathVectorListPtr};
    	TIMER_T startTime;
    	TIMER_READ(startTime);

	
	for (i = 0; i < nthreads; i++) {
		if (pthread_create (&threads[i], 0, router_solve, (void *)&routerArg) == 0);
		else 
			exit(-1);
	}

	for (i = 0; i < nthreads; i++)
		pthread_join(threads[i], NULL);
	return 0;
}
