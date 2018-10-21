#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lib/list.h"
#include "maze.h"
#include "router.h"
#include "lib/timer.h"
#include "lib/types.h"
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
int n_threads=1;


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
    printf("    t <UINT>   [t] nu of threads    (%i)\n", n_threads);
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

    opterr = 1;

    setDefaultParams();

    while ((opt = getopt(argc, argv, "hb:px:y:z:t:")) != -1) {
        switch (opt) {
            case 'b':
            case 'x':
            case 'y':
            case 'z':
            	printf("teste\n");
                global_params[(unsigned char)opt] = atol(optarg);
                break;
            case 't':
            	n_threads = atoi(optarg);
            	opterr--;
            	break;
            case 'p':
                /*global_doPrint = TRUE;*/
                break;
            case '?':
            case 'h':
            	displayUsage(argv[0]);
            	break;
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

    if (opterr) {
    	displayUsage(argv[0]);
    	return -1;
    }

    if (file_i != -1) return file_i;

    return -1;
}

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
/*
* Initialization
*/
	int i, file_i;
	pthread_t *threads;
	FILE *file_input=NULL, *file_output=NULL;

	file_i = parseArgs(argc, (char** const)argv);
	if (file_i < 0) exit(1);

	file_input = fopen(argv[file_i], "r");
	file_output = getOutputFile(argv[file_i]);


	threads = (pthread_t *) malloc(sizeof(pthread_t) * n_threads);
	
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

    pthread_mutex_t router_lock;
    pthread_mutex_init(&router_lock, NULL);

    router_solve_arg_t routerArg = {routerPtr, mazePtr, pathVectorListPtr, &router_lock};
    TIMER_T startTime;
    TIMER_READ(startTime);
	
	for (i = 0; i < n_threads; i++) {
		if (pthread_create (&threads[i], 0, (void *)(*router_solve), (void *)&routerArg) == 0)
			printf("Tarefa\n");
		else 
			/*exit(-1);*/
			printf("Erro\n");
	}

	for (i = 0; i < n_threads; i++)
		pthread_join(threads[i], NULL);

	pthread_mutex_destroy(&router_lock);

	TIMER_T stopTime;
    TIMER_READ(stopTime);

    long numPathRouted = 0;
    list_iter_t it;
    list_iter_reset(&it, pathVectorListPtr);
    while (list_iter_hasNext(&it, pathVectorListPtr)) {
        vector_t* pathVectorPtr = (vector_t*)list_iter_next(&it, pathVectorListPtr);
        numPathRouted += vector_getSize(pathVectorPtr);
	}
 	
	fprintf(file_output, "Paths routed    = %li\n", numPathRouted);
    fprintf(file_output, "Elapsed time    = %f seconds\n", TIMER_DIFF_SECONDS(startTime, stopTime));

    /*
     * Check solution and clean up
     */
    assert(numPathRouted <= numPathToRoute);

    bool_t status = maze_checkPaths(mazePtr, pathVectorListPtr, file_output); /*OUTPUT*/

    assert(status == TRUE);
    fprintf(file_output,"Verification passed.");

    maze_free(mazePtr);
    router_free(routerPtr);

    list_iter_reset(&it, pathVectorListPtr);
    while (list_iter_hasNext(&it, pathVectorListPtr)) {
        vector_t* pathVectorPtr = (vector_t*)list_iter_next(&it, pathVectorListPtr);
        vector_t* v;
        while((v = vector_popBack(pathVectorPtr))) {
            // v stores pointers to longs stored elsewhere; no need to free them here
            vector_free(v);
        }
        vector_free(pathVectorPtr);
    }
    list_free(pathVectorListPtr);

    fclose(file_input);
    fclose(file_output);
    exit(0);
}
