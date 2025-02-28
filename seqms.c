#include <stdio.h>
#include <stdlib.h>     // atoi, rand
#include <unistd.h>     // getopt
#include <omp.h>        // omp_get_wtime

// Sort (in ascending order) a randomly generated array of integers, using Merge-Sort algorithm.

void arrMerge();
void arrDivide();
void mergeSort();

void print_help(void);
void displayArray();

void updateDepth();
void printDepth();
void displaySubarray();
void displayTwoSubarrays();
void debugPrintGlobalFlags(void);

int printFlag = 0;
int printMaxDepth = 3;
int curDepth = 0;
int maxDepth = 0;

int quietFlag = 0;

int main(int argc, char* argv[]) {

    int inputVal;

    int timeFlag = 0;
    char *timeUnit = "";

    int randomSeed = 42069;

    // No extra command line argument passed
    if (argc == 1) {
        print_help();
        return 1;
    }

    // First argument
    int arraySize = atoi (argv[1]);                 // Interprets an integer value in a byte string
    
    // Get flags
    int flag;
    // optind = 2;                                     // Start index of the next element to be processed in argv
    opterr = 0;                                     // getopt() does not print an error message
    while ((flag = getopt(argc, argv, ":hp:qtmus:")) != -1) {
        switch (flag) {
            case 'h':
                print_help();
                return 1;
            case 'p':
                printFlag = 1;
                inputVal = atoi(optarg);
                if (0 <= inputVal) printMaxDepth = inputVal;
                else printf("Invalid value (%d). Default used.\n", inputVal);
                break;
            case 'q':
                quietFlag = 1;
                break;
            case 't':
                timeFlag = 1;
                break;
            case 'm':
                timeFlag = 1000;
                timeUnit = "mili";
                break;
            case 'u':
                timeFlag = 1000000;
                timeUnit = "micro";
                break;
            case 's':
                inputVal = atoi(optarg);
                if (0 <= inputVal) randomSeed = inputVal;
                else printf("Invalid value (%d). Default used.\n", inputVal);
                break;
            case ':':                               // when missing positional argument
                if (optopt == 'p') {
                    printFlag = 1;
                } else {
                    print_help();
                    return 1;
                }
                break;
            case '?':
                fprintf (stderr, "Unknown option `-%c'.\n", optopt);
                print_help();
                return 1;
            default:
                printf("Got a '%c'", flag);
                return 1;
        }
    }

    // Debug
    // debugPrintGlobalFlags();

    // Array size
    if ( !(0 < arraySize && arraySize <= 1000000) ) {
        printf("\nArray size invalid: %d", arraySize);
        printf("\nMinimum array size: 1");
        printf("\nMaximum array size: 1000000\n");
        print_help();
        return 1;
    }
    if (!quietFlag) printf("\nArray size: %d\n", arraySize);
    
    // Creating array
    int array[arraySize];
    for (int i=0; i<arraySize; i++)
        array[i] = i;

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
    if (!quietFlag) {
        printf("\nArray after randomizing:\n");
        displayArray(array, arraySize);
    }

    // Setting timer
    double wtime_start, wtime_taken;
    
    // Sorting
    if (timeFlag) wtime_start = omp_get_wtime();
    mergeSort(array, arraySize);
    if (timeFlag) wtime_taken = omp_get_wtime() - wtime_start;
    
    // Outputs
    if (!quietFlag) {
        printf("\nArray after sorting:\n");
        displayArray(array, arraySize);
        if (timeFlag) printf("\nTime taken sorting: %f %sseconds.\n", wtime_taken*timeFlag, timeUnit);
        printf("\n");
    } else {
        if (timeFlag) printf("%f", wtime_taken*timeFlag);
        else printf("\nDone!\n\n");
    }

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
    
    printDepth(1);
    displaySubarray(arr, indexL, indexR, "subarray");
    displayTwoSubarrays(arr, indexL, indexM, indexR);
    updateDepth(1);
    
    arrDivide(arr, indexL, indexM);
    arrDivide(arr, indexM + 1, indexR);
    
    updateDepth(0);
    printDepth(0);
    displayTwoSubarrays(arr, indexL, indexM, indexR);
    // displaySubarray(arr, indexL, indexR, "no order");
    
    arrMerge(arr, indexL, indexM, indexR);
    
    displaySubarray(arr, indexL, indexR, "ordered");
}

void mergeSort(int *arr, int arrSize) {

    arrDivide(arr, 0, arrSize-1);
    
    if (printFlag && !quietFlag) printf("\nMax depth reached: %d\n", maxDepth);
}

// --------------- MISC FUNCTIONS -------------------

void print_help(void) {
    printf(
        "\n"
        "USAGE\n"
        "       seqms arraySize [options]\n"
        "\n"
        "DESCRIPTION\n"
        "       Sort (in ascending order) a randomly generated array of integers, using Merge-Sort algorithm.\n"
        "\n"
        "       arraySize must be an integer value bigger than 0, and with the maximum value of 10^6.\n"
        "       Using more might overflow (for now).\n"
        "\n"
        "OPTIONS\n"
        "       -p [printMaxDepth]\n"
        "               Print steps of the sorting process. Sets printMaxDepth if given (must be positive integer),\n"
        "               which defines how deep into the recurrence it will print the arrays. If printMaxDepth is not\n"
        "               given, the default value is 3.\n"
        "\n"
        "       -q      Quiet. Minimal print stuff.\n"
        "\n"
        "       -t      Display time taken for execution of the marge-sort algorithm part only. If passed \n"
        "               together with `-q` outputs only the time (in seconds) to stdout.\n"
        "               If used with -p, the time is unreliable, as it also has to do extra print operations.\n"
        "\n"
        "       -m      Same as -t, but miliseconds.\n"
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

void displayArray(int *arr, int arrSize) {
    printf("array: [");
    int counter = 0;
    char *printString = arrSize <=      10 ? "%1i " :
                        arrSize <=     100 ? "%2i " :
                        arrSize <=    1000 ? "%3i " :
                        arrSize <=   10000 ? "%4i " :
                        arrSize <=  100000 ? "%5i " : "%6i ";
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

void updateDepth(int d) {
    // d:
    // 1 for increase
    // 0 for decrease

    if (quietFlag || !printFlag) return;

    if (d) {
        curDepth++;
        if (curDepth > maxDepth)
            maxDepth = curDepth;
    } else {
        curDepth--;
    }
}

void printDepth(int d) {
    // d:
    // 1 for increase
    // 0 for decrease

    if (printFlag && !quietFlag && (curDepth <= printMaxDepth)) printf("\nCurrent depth: %c %d\n", d?'v':'^', curDepth);
}

void displaySubarray(int arr[], int indexL, int indexR, char* name) {
    
    // name should have at most 8 chars
    // int len;
    // for(len = 0; name[len] != '\0'; len++);
    // if (len > 8) name = "subarray";

    if (quietFlag || !printFlag || !(curDepth <= printMaxDepth)) return;
    
    int counter;
    int arrSize;
    char *printString;

    counter = 0;
    arrSize = indexR - indexL;
    printString =   arrSize <=      10 ? "%1i " :
                    arrSize <=     100 ? "%2i " :
                    arrSize <=    1000 ? "%3i " :
                    arrSize <=   10000 ? "%4i " :
                    arrSize <=  100000 ? "%5i " : "%6i ";
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
        "       printFlag (%d)\n"
        "       printMaxDepth (%d)\n"
        "       curDepth (%d)\n"
        "       maxDepth (%d)\n"
        "       quietFlag (%d)\n"
        "\n", printFlag, printMaxDepth, curDepth, maxDepth, quietFlag
    );
}
