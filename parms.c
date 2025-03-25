#include <stdio.h>      // printf, fprintf
#include <stdlib.h>     // atoi, rand
#include <unistd.h>     // getopt
#include <omp.h>

// Sort (in ascending order) a randomly generated array of integers, using Merge-Sort algorithm.

void arrMerge();
void arrDivide();
void mergeSort();

void print_help(void);

void displayArray();

void displaySubarray();
void displayTwoSubarrays();
void debugPrintGlobalFlags(void);

int quietFlag = 0;
int moreQuietFlag = 0;

int numThreads = 2;

int main(int argc, char* argv[]) {

    int inputVal;

    int timeFlag = 0;
    char *timeUnit = "";

    int randomSeed = 42069;

    int availProcessorsNum = omp_get_num_procs();
    
    // No extra command line argument passed
    if (argc == 1) {
        print_help();
        return 1;
    }

    // First argument
    int arraySize = atoi (argv[1]);                 // Interprets an integer value in a byte string
    
    // Get flags
    int flag;
    opterr = 0;                                     // getopt() does not print an error message
    while ((flag = getopt(argc, argv, ":hqtmus:n:N:")) != -1) {
        switch (flag) {
            case 'h':
                print_help();
                return 0;
            case 'q':
                if (quietFlag) moreQuietFlag = 1;
                else quietFlag = 1;
                break;
            case 't':
                timeFlag = 1;
                break;
            case 'm':
                timeFlag = 1000;
                timeUnit = "milli";
                break;
            case 'u':
                timeFlag = 1000000;
                timeUnit = "micro";
                break;
            case 's':
                inputVal = atoi(optarg);
                if (0 <= inputVal) randomSeed = inputVal;
                else printf("Invalid value (-s %d). Default used.\n", inputVal);
                break;
            case 'n':
                inputVal = atoi(optarg);
                switch (inputVal) {
                    case 2:
                    case 4:
                    case 8:
                        if (2 <= inputVal && inputVal <= availProcessorsNum) numThreads = inputVal;
                        else printf("Invalid value (-n %d), min: 2, max: %d. Default used.\n", inputVal, availProcessorsNum);
                        break;
                    default:
                        printf("Invalid value (-n %d). Code limitation. Default used.\n", inputVal);
                        break;
                }
                break;
            case 'N':
                inputVal = atoi(optarg);
                if (2 <= inputVal && inputVal <= availProcessorsNum) return 0;
                else return 1;
            case ':':                               // when missing positional argument
                print_help();
                return 1;
            case '?':
                fprintf (stderr, "Unknown option (-%c).\n", optopt);
                print_help();
                return 1;
            default:
                printf("Got a '%c'", flag);
                return 1;
        }
    }

    // Debug
    // debugPrintGlobalFlags();

    // Check array size
    if ( !(0 < arraySize && arraySize <= 1000000) ) {
        printf("\nArray size invalid: %d", arraySize);
        printf("\nMinimum array size: 1");
        printf("\nMaximum array size: 1000000\n");
        print_help();
        return 1;
    }
    if (!quietFlag) {
        printf("\nArray size: %d", arraySize);
        printf("\nNumber of threads: %d\n", numThreads);
    }
    
    // Allocating memory and creating array
    int* array = malloc(arraySize * sizeof(int));

    // Check if malloc was successful
    if (array == NULL) { 
        printf("Memory allocation failed! (array)\n"); 
        return 1;
    }

    // Initializing values
    for (int i=0; i<arraySize; i++) array[i] = i;
    if (!quietFlag) {
        printf("\nArray before randomizing:\n");
        displayArray(array, arraySize);
    }
    
    // Randomizing
    srand (randomSeed);
    int randomIndex, aux;
    for (int i=0; i<arraySize; i++) {
        randomIndex = rand() % arraySize;           // rand() returns a pseudo-random integer value
        aux = array[i];
        array[i] = array[randomIndex];
        array[randomIndex] = aux;
    }
    if (!moreQuietFlag) {
        printf("\nArray after randomizing:\n");
        displayArray(array, arraySize);
    }

    // Setting timer
    double wtime_start, wtime_taken;

    // Enableing nested parallelism
    // level = ceil(log_2(threads))
    // 2 threads -> 1 level
    // 4 threads -> 2 level
    // 8 threads -> 3 level
    // 12 threads -> 4 level
    omp_set_nested(1);
    omp_set_dynamic(0);
    omp_set_max_active_levels(
        numThreads <= 2 ? 1 :
        numThreads <= 4 ? 2 :
        numThreads <= 8 ? 3 : 4
    );
    
    // Sorting
    if (timeFlag) wtime_start = omp_get_wtime();
    mergeSort(array, arraySize);
    if (timeFlag) wtime_taken = omp_get_wtime() - wtime_start;
    
    // Outputs
    if (!moreQuietFlag) {
        printf("\nArray after sorting:\n");
        displayArray(array, arraySize);
        if (timeFlag) printf("\nTime taken sorting: %f %sseconds.\n", wtime_taken*timeFlag, timeUnit);
        printf("\n");
    } else {
        if (timeFlag) printf("%f", wtime_taken*timeFlag);
    }

    // Validation
    int incorrectSort = 0;
    for (int i=0; i<(arraySize-1); i++) {
        if (array[i] > array[i+1]) {
            incorrectSort++;
        }
    }
    if (incorrectSort) {
        printf("Warning: incorrect sorting. (%d)\n\n", incorrectSort);
    } else if (!moreQuietFlag) printf("Sorting is correct . (%d)\n\n", incorrectSort);

    // Freeing allocated memory
    free(array);
    
    return 0;
}

// --------------- MERGE SORT ALGORITHM -------------------

// Merge two subarrays of arr in order
// Internal function of mergeSort; should no be called in main
void arrMerge(int arr[], int indexL, int indexM, int indexR) {
    
    // subarrays are:
    // arrAux1[indexL .. indexM]
    // arrAux2[indexM+1..indexR]
    
    // Auxiliary arrays
    int arrAuxSize1 = indexM+1 - indexL;
    int arrAuxSize2 = indexR   - indexM;
    int arrAux1[arrAuxSize1], arrAux2[arrAuxSize2];

    for (int i = 0; i < arrAuxSize1; i++)
        arrAux1[i] = arr[indexL + i];
    for (int i = 0; i < arrAuxSize2; i++)
        arrAux2[i] = arr[indexM + i + 1];

    // Merging auxiliar arrays orderly into arr
    int i, r, l;
    i = indexL;
    r = l = 0;
    while (r < arrAuxSize1 && l < arrAuxSize2) {
        if (arrAux1[r] <= arrAux2[l]) {
            arr[i] = arrAux1[r];
            r++;
        } else {
            arr[i] = arrAux2[l];
            l++;
        }
        i++;
    }

    // Copying the remaining elements, if any
    while (r < arrAuxSize1) {
        arr[i] = arrAux1[r];
        r++;
        i++;
    }
    while (l < arrAuxSize2) {
        arr[i] = arrAux2[l];
        l++;
        i++;
    }
}

// Recursive calls dividing the array into subarrays
// Internal function of mergeSort; should no be called in main
void arrDivide(int arr[], int indexL, int indexR) {

    // If the indexes are the same, the subarray has only one element, and thus, is already sorted
    if (indexL >= indexR) return;

    int indexM = indexL + (indexR - indexL) / 2;
    
    // #pragma omp critical(printing)
    // {
    //     printDepth(1);
    //     displaySubarray(arr, indexL, indexR, "subarray");
    //     displayTwoSubarrays(arr, indexL, indexM, indexR);
    // }
    // updateDepth(1);
    
    arrDivide(arr, indexL, indexM);
    arrDivide(arr, indexM + 1, indexR);
    
    // updateDepth(0);
    // #pragma omp critical(printing)
    // {
    //     printDepth(0);
    //     displayTwoSubarrays(arr, indexL, indexM, indexR);
    // }

    arrMerge(arr, indexL, indexM, indexR);
    
    // #pragma omp critical(printing)
    // {
    //     displaySubarray(arr, indexL, indexR, "ordered");
    // }
}

void arrDivideParallel(int arr[], int indexL, int indexR) {

    // If the indexes are the same, the subarray has only one element, and thus, is already sorted
    if (indexL >= indexR) return;

    int indexM = indexL + (indexR - indexL) / 2;
    
    // #pragma omp critical(printing)
    // {
    //     printDepth(1);
    //     displaySubarray(arr, indexL, indexR, "subarray");
    //     displayTwoSubarrays(arr, indexL, indexM, indexR);
    // }
    
    #pragma omp parallel num_threads(numThreads) shared(arr)
    {
        
        #pragma omp sections
        {   
            #pragma omp section
            {
                // updateDepthParallel();
                if (omp_get_active_level() < omp_get_max_active_levels()) arrDivideParallel(arr, indexL, indexM);
                else arrDivide(arr, indexL, indexM);
            }
            
            #pragma omp section
            {
                // updateDepthParallel();
                if (omp_get_active_level() < omp_get_max_active_levels()) arrDivideParallel(arr, indexM + 1, indexR);
                else arrDivide(arr, indexM + 1, indexR);
            }
        }
    }

    // updateDepthParallel();
    // #pragma omp critical(printing)
    // {
    //     printDepth(0);
    //     displayTwoSubarrays(arr, indexL, indexM, indexR);
    // }

    arrMerge(arr, indexL, indexM, indexR);
    
    // #pragma omp critical(printing)
    // {
    //     displaySubarray(arr, indexL, indexR, "ordered");
    // }
}

void mergeSort(int *arr, int arrSize) {
    
    arrDivideParallel(arr, 0, arrSize-1);
    
}

// --------------- MISC FUNCTIONS -------------------

void print_help(void) {
    printf(
        "\n"
        "USAGE\n"
        "       parms arraySize [options]\n"
        "\n"
        "DESCRIPTION\n"
        "       Sort (in ascending order) a randomly generated array of integers, using parallel Merge-Sort algorithm.\n"
        "\n"
        "       arraySize must be an integer value bigger than 0, and with the maximum value of 1000000 (10^6).\n"
        "\n"
        "OPTIONS\n"
        "       -n numThreads\n"
        "               Defines the number of threads to be used; must be even (code limitation) integer, bigger than 1.\n"
        "               Limited by the number of processors available to the device. Default is 2.\n"
        "\n"
        "       -N numThreads\n"
        "               If numThreads is bigger than 1 and smaller than the number of processors available, returns 0.\n"
        "               Otherwise, returns 1.\n"
        "               No calculation is made if this flag is used.\n"
        "\n"
        "       -q      Prints less stuff. Use twice to print nothing (except for the time, if -t is also passed).\n"
        "\n"
        "       -t      Display time taken for execution of the marge-sort algorithm part only. If passed \n"
        "               together with `-q` outputs only the time (in seconds) to stdout.\n"
        "               If used with -p, the time is unreliable, as it also has to do extra print operations.\n"
        "\n"
        "       -m      Same as -t, but milliseconds.\n"
        "\n"
        "       -u      Same as -t, but microseconds.\n"
        "\n"
        "       -h      Display this help message.\n"
        "\n"
        "       -s randomSeed\n"
        "               Sets randomSeed used when randomizing the array (must be positive integer).\n"
        "\n"
    );
}

char * printStringArrSize(int arrSize){
    return  arrSize <=        10 ? "%1i " :
            arrSize <=       100 ? "%2i " :
            arrSize <=      1000 ? "%3i " :
            arrSize <=     10000 ? "%4i " :
            arrSize <=    100000 ? "%5i " : "%6i ";
    
    // return  arrSize <=        10 ? "%1i " :
    //         arrSize <=       100 ? "%2i " :
    //         arrSize <=      1000 ? "%3i " :
    //         arrSize <=     10000 ? "%4i " :
    //         arrSize <=    100000 ? "%5i " :
    //         arrSize <=   1000000 ? "%6i " :
    //         arrSize <=  10000000 ? "%7i " :
    //         arrSize <= 100000000 ? "%8i " : "%9i ";
}

void displayArray(int *arr, int arrSize) {
    printf("array: [");
    int counter = 0;
    char *printString = printStringArrSize(arrSize);
    for (int i=0; i<arrSize; i++) {
        printf(printString, arr[i]);
        counter++;
        if ((counter == 25) && (i+1 != arrSize)) {
            printf("\n        ");
            counter = 0;
        }
    }
    printf("]\n");
}

void displaySubarray(int arr[], int indexL, int indexR, char* name) {
    
    // name should have at most 8 chars
    // int len;
    // for(len = 0; name[len] != '\0'; len++);
    // if (len > 8) name = "subarray";

    if (quietFlag) return;
    
    int counter;
    int arrSize;
    char *printString;

    counter = 0;
    arrSize = indexR - indexL;
    printString = printStringArrSize(arrSize);
    printf("%8s: [", name);
    for (int i=indexL; i<(indexR+1); i++) {
        printf(printString , arr[i]);
        counter++;
        if ((counter == 25) && (i-indexL != arrSize)) {
            // printf("\ni %d; indexL %d; arrSize %d", i, indexL, arrSize);
            printf("\n           ");
            counter = 0;
        }
    }
    printf("]\n");
}

void displayTwoSubarrays(int arr[], int indexL, int indexM, int indexR) {
    displaySubarray(arr, indexL, indexM, "L array");
    displaySubarray(arr, indexM + 1, indexR, "R array");
}


void debugPrintGlobalFlags(void) {
    printf(
        "\n"
        "FLAGS\n"
        "       quietFlag (%d)\n"
        "\n", quietFlag
    );
}
