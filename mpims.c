#include <stdio.h>      // printf, fprintf
#include <stdlib.h>     // atoi, rand
#include <unistd.h>     // getopt
#include <mpi.h>

// Sort (in ascending order) a randomly generated array of integers, using Merge-Sort algorithm.

void arrMerge();
void arrDivide();
int indexMiddle();
void mergeSort();

void print_help(void);

void displayArray();

void displaySubarray();
void displayTwoSubarrays();

int quietFlag = 0;
int moreQuietFlag = 0;

int mpiCommSize;
int mpiMyRank;
int mpiRootRank = 0;

int main(int argc, char* argv[]) {



    // Initialize MPI environment
    MPI_Init(&argc, &argv);

    // Getting the communicator size
    MPI_Comm_size(MPI_COMM_WORLD, &mpiCommSize);

    // Getting each process rank (id)
    MPI_Comm_rank(MPI_COMM_WORLD, &mpiMyRank);

    // Verify processes number
    switch (mpiCommSize) {
        case 2:
        case 4:
        case 8:
            // Valid processes number
            // Continue
            break;
        default:
            if (mpiMyRank == mpiRootRank) fprintf (stderr, "Invalid processes number (%d). Code limitation.\n", mpiCommSize);
            print_help();
            return 1;
    }


    
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
    opterr = 0;                                     // getopt() does not print an error message
    while ((flag = getopt(argc, argv, ":hqtmus:")) != -1) {
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

    
    // Check array size
    if ( !(0 < arraySize && arraySize <= 1000000) ) {
        
        // Tasks only for root process
        if (mpiMyRank == mpiRootRank) {
            printf("\nArray size invalid: %d", arraySize);
            printf("\nMinimum array size: 1");
            printf("\nMaximum array size: 1000000\n");
        }
        print_help();
        return 1;
    }

    // Tasks only for root process
    if (mpiMyRank == mpiRootRank) {
        if (!quietFlag) {
            printf("\nArray size: %d", arraySize);
            printf("\nNumber of processes: %d\n", mpiCommSize);
        }
    
    }

    // Allocating memory and creating array
    int* array = malloc(arraySize * sizeof(int));

    // Check if malloc was successful
    if (array == NULL) { 
        printf("Memory allocation failed! (array)\n");

        // Finalize MPI environment
        MPI_Finalize();
        return 1;
    }

    // Tasks only for root process
    if (mpiMyRank == mpiRootRank) {

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
    }

    // Broadcast the randomized array
    // int MPI_Bcast(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm)
    MPI_Bcast(array, arraySize, MPI_INT, mpiRootRank, MPI_COMM_WORLD);

    // Setting timer
    double wTimeStart, wTimeTaken;

    // Sorting
    if (timeFlag && mpiMyRank == mpiRootRank) wTimeStart = MPI_Wtime();
    mergeSort(array, arraySize);
    if (timeFlag && mpiMyRank == mpiRootRank) wTimeTaken = MPI_Wtime() - wTimeStart;

    // Tasks only for root process
    if (mpiMyRank == mpiRootRank) {
        
        // Outputs
        if (!moreQuietFlag) {
            printf("\nArray after sorting:\n");
            displayArray(array, arraySize);
            if (timeFlag) printf("\nTime taken sorting: %f %sseconds.\n", wTimeTaken*timeFlag, timeUnit);
            printf("\n");
        } else {
            if (timeFlag) printf("%f", wTimeTaken*timeFlag);
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
    }

    // Freeing allocated memory
    free(array);

    // Finalize MPI environment
    MPI_Finalize();
    
    return 0;
}

// --------------- MERGE SORT ALGORITHM -------------------



int indexMiddle(int indexL, int indexR) {
    return (indexL + (indexR - indexL) / 2);
}

void mergeSort(int *arr, int arrSize) {

    int indexL = 0;
    int indexR = arrSize-1;

    int sliceIndStr;
    int sliceIndMid;
    int sliceIndEnd;

    // The mpi function MPI_Gather won't work because sometimes the sendcount and recvcount would have different sizes.
    // Using MPI_Send and MPI_Recv for each process instead.
    // Haven't changed the variables names.
    int *gatherBuffer;
    int gatherCount;
    int gatherIndex;

    int mpiTag = 99;
    
    // If the array has only one element, it is already sorted
    if (indexL >= indexR) return;
    
    // Dividing work in case of 2, 4 or 8 processes working
    switch (mpiCommSize) {
        case 2: {
            
            int indexM = indexMiddle(indexL, indexR);

            // Setting which slice of the array each process will work on
            switch (mpiMyRank) {
                case 0: {
                    sliceIndStr = indexL;
                    sliceIndEnd = indexM;
                    break;
                }
                case 1: {
                    sliceIndStr = indexM+1;
                    sliceIndEnd = indexR;
                    break;
                }
                default: {
                    fprintf(stderr, "ERROR switch (mpiMyRank). This should never happen.\n");
                    break;
                }
            }
            
            // Everyone gets to work
            arrDivide(arr, sliceIndStr, sliceIndEnd);

            if (mpiMyRank == mpiRootRank) {

                gatherIndex = sliceIndEnd + 1;

                // Root process gather the results IN ORDER
                for (int mpiRank=1; mpiRank<mpiCommSize; mpiRank++) {

                    // Receive the size of the buffer from the other processes
                    MPI_Recv(&gatherCount, 1, MPI_INT, mpiRank, mpiTag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    
                    // Setting the gather parameters based on each process work slice
                    gatherBuffer = &arr[gatherIndex];
                    
                    // Receive sorted slice of the array from the other processes
                    MPI_Recv(gatherBuffer, gatherCount, MPI_INT, mpiRank, mpiTag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                    // Update index for next batch
                    gatherIndex += gatherCount;
                }

            } else {
                
                // Setting the gather parameters based on each process work slice
                gatherBuffer = &arr[sliceIndStr];
                gatherCount = sliceIndEnd - sliceIndStr + 1;

                // Send the size of its buffer to the root process
                MPI_Send(&gatherCount, 1, MPI_INT, mpiRootRank, mpiTag, MPI_COMM_WORLD);

                // Send its sorted slice of the array to the root process
                MPI_Send(gatherBuffer, gatherCount, MPI_INT, mpiRootRank, mpiTag, MPI_COMM_WORLD);
            }
            
            // Tasks only for root process
            if (mpiMyRank == mpiRootRank) {
                arrMerge(arr, indexL, indexM, indexR);
            }
            
            break;
        }
        case 4: {

            int indexM1;
            int indexM2;
            int indexM3;

            indexM2 = indexMiddle(indexL, indexR);
            
            indexM1 = indexMiddle(indexL, indexM2);
            indexM3 = indexMiddle(indexM2+1, indexR);
            
            // Setting which slice of the array each process will work on
            switch (mpiMyRank) {
                case 0: {
                    sliceIndStr = indexL;
                    sliceIndEnd = indexM1;
                    break;
                }
                case 1: {
                    sliceIndStr = indexM1+1;
                    sliceIndEnd = indexM2;
                    break;
                }
                case 2: {
                    sliceIndStr = indexM2+1;
                    sliceIndEnd = indexM3;
                    break;
                }
                case 3: {
                    sliceIndStr = indexM3+1;
                    sliceIndEnd = indexR;
                    break;
                }
                default: {
                    fprintf(stderr, "ERROR switch (mpiMyRank). This should never happen.\n");
                    break;
                }
            }
            
            // Everyone gets to work
            arrDivide(arr, sliceIndStr, sliceIndEnd);

            if (mpiMyRank == mpiRootRank) {

                gatherIndex = sliceIndEnd + 1;

                // Root process gather the results IN ORDER
                for (int mpiRank=1; mpiRank<mpiCommSize; mpiRank++) {

                    // Receive the size of the buffer from the other processes
                    MPI_Recv(&gatherCount, 1, MPI_INT, mpiRank, mpiTag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    
                    // Setting the gather parameters based on each process work slice
                    gatherBuffer = &arr[gatherIndex];
                    
                    // Receive sorted slice of the array from the other processes
                    MPI_Recv(gatherBuffer, gatherCount, MPI_INT, mpiRank, mpiTag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                    // Update index for next batch
                    gatherIndex += gatherCount;
                }

            } else {
                
                // Setting the gather parameters based on each process work slice
                gatherBuffer = &arr[sliceIndStr];
                gatherCount = sliceIndEnd - sliceIndStr + 1;

                // Send the size of its buffer to the root process
                MPI_Send(&gatherCount, 1, MPI_INT, mpiRootRank, mpiTag, MPI_COMM_WORLD);

                // Send its sorted slice of the array to the root process
                MPI_Send(gatherBuffer, gatherCount, MPI_INT, mpiRootRank, mpiTag, MPI_COMM_WORLD);
            }

            // Splitting the communicator into smaller communicator for the remaining work latter
            MPI_Comm mpiShorterComm;
            int color = (mpiMyRank <= 1) ? 0 : MPI_UNDEFINED;
            MPI_Comm_split(MPI_COMM_WORLD, color, mpiMyRank, &mpiShorterComm);

            // All other processes will not be necessary anymore
            if ( !(mpiMyRank <= 1) ) break;

            // Broadcast the randomized array to smaller communicator
            MPI_Bcast(arr, arrSize, MPI_INT, mpiRootRank, mpiShorterComm);

            // Setting which slice of the array each process will work on
            switch (mpiMyRank) {
                case 0: {
                    sliceIndStr = indexL;
                    sliceIndMid = indexM1;
                    sliceIndEnd = indexM2;
                    break;
                }
                case 1: {
                    sliceIndStr = indexM2+1;
                    sliceIndMid = indexM3;
                    sliceIndEnd = indexR;
                    break;
                }
                default: {
                    fprintf(stderr, "ERROR switch (mpiMyRank). This should never happen.\n");
                    break;
                }
            }

            // Everyone gets to work
            arrMerge(arr, sliceIndStr, sliceIndMid, sliceIndEnd);

            if (mpiMyRank == mpiRootRank) {

                gatherIndex = sliceIndEnd + 1;

                int mpiShorterCommSize;
                MPI_Comm_size(mpiShorterComm, &mpiShorterCommSize);

                // Root process gather the results IN ORDER
                for (int mpiRank=1; mpiRank<mpiShorterCommSize; mpiRank++) {

                    // Receive the size of the buffer from the other processes
                    MPI_Recv(&gatherCount, 1, MPI_INT, mpiRank, mpiTag, mpiShorterComm, MPI_STATUS_IGNORE);
                    
                    // Setting the gather parameters based on each process work slice
                    gatherBuffer = &arr[gatherIndex];
                    
                    // Receive sorted slice of the array from the other processes
                    MPI_Recv(gatherBuffer, gatherCount, MPI_INT, mpiRank, mpiTag, mpiShorterComm, MPI_STATUS_IGNORE);

                    // Update index for next batch
                    gatherIndex += gatherCount;
                }

            } else {
                
                // Setting the gather parameters based on each process work slice
                gatherBuffer = &arr[sliceIndStr];
                gatherCount = sliceIndEnd - sliceIndStr + 1;

                // Send the size of its buffer to the root process
                MPI_Send(&gatherCount, 1, MPI_INT, mpiRootRank, mpiTag, mpiShorterComm);

                // Send its sorted slice of the array to the root process
                MPI_Send(gatherBuffer, gatherCount, MPI_INT, mpiRootRank, mpiTag, mpiShorterComm);
            }
            
            // Tasks only for root process
            if (mpiMyRank == mpiRootRank) {
                arrMerge(arr, indexL, indexM2, indexR);
            }
            
            // printf("\nBefore new shorter comm");
            // printf("\nHello, World from rank %d out of %d processors, (color %d)\n", mpiMyRank, mpiCommSize, color);
            // color = (mpiMyRank == 0) ? 1 : 2;
            // MPI_Comm_split(mpiShorterComm, color, mpiMyRank, &mpiShorterComm);
            // int size;
            // int myrank;
            // MPI_Comm_size(mpiShorterComm, &size);
            // MPI_Comm_rank(mpiShorterComm, &myrank);
            // printf("\nAfter new shorter comm");
            // printf("\nHello, World from rank %d out of %d processors, (color %d)\n", myrank, size, color);

            MPI_Comm_free(&mpiShorterComm);
            break;
        }
        case 8: {

            int indexM1;
            int indexM2;
            int indexM3;
            int indexM4;
            int indexM5;
            int indexM6;
            int indexM7;

            indexM4 = indexMiddle(indexL, indexR);
            
            indexM2 = indexMiddle(indexL, indexM4);
            indexM6 = indexMiddle(indexM4+1, indexR);

            indexM1 = indexMiddle(indexL, indexM2);
            indexM3 = indexMiddle(indexM2+1, indexM4);
            indexM5 = indexMiddle(indexM4+1, indexM6);
            indexM7 = indexMiddle(indexM6+1, indexR);

            // Setting which slice of the array each process will work on
            switch (mpiMyRank) {
                case 0: {
                    sliceIndStr = indexL;
                    sliceIndEnd = indexM1;
                    break;
                }
                case 1: {
                    sliceIndStr = indexM1+1;
                    sliceIndEnd = indexM2;
                    break;
                }
                case 2: {
                    sliceIndStr = indexM2+1;
                    sliceIndEnd = indexM3;
                    break;
                }
                case 3: {
                    sliceIndStr = indexM3+1;
                    sliceIndEnd = indexM4;
                    break;
                }
                case 4: {
                    sliceIndStr = indexM4+1;
                    sliceIndEnd = indexM5;
                    break;
                }
                case 5: {
                    sliceIndStr = indexM5+1;
                    sliceIndEnd = indexM6;
                    break;
                }
                case 6: {
                    sliceIndStr = indexM6+1;
                    sliceIndEnd = indexM7;
                    break;
                }
                case 7: {
                    sliceIndStr = indexM7+1;
                    sliceIndEnd = indexR;
                    break;
                }
                default: {
                    fprintf(stderr, "ERROR switch (mpiMyRank). This should never happen.\n");
                    break;
                }
            }
            
            // Everyone gets to work
            arrDivide(arr, sliceIndStr, sliceIndEnd);

            if (mpiMyRank == mpiRootRank) {

                gatherIndex = sliceIndEnd + 1;

                // Root process gather the results IN ORDER
                for (int mpiRank=1; mpiRank<mpiCommSize; mpiRank++) {

                    // Receive the size of the buffer from the other processes
                    MPI_Recv(&gatherCount, 1, MPI_INT, mpiRank, mpiTag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    
                    // Setting the gather parameters based on each process work slice
                    gatherBuffer = &arr[gatherIndex];
                    
                    // Receive sorted slice of the array from the other processes
                    MPI_Recv(gatherBuffer, gatherCount, MPI_INT, mpiRank, mpiTag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                    // Update index for next batch
                    gatherIndex += gatherCount;
                }

            } else {
                
                // Setting the gather parameters based on each process work slice
                gatherBuffer = &arr[sliceIndStr];
                gatherCount = sliceIndEnd - sliceIndStr + 1;

                // Send the size of its buffer to the root process
                MPI_Send(&gatherCount, 1, MPI_INT, mpiRootRank, mpiTag, MPI_COMM_WORLD);

                // Send its sorted slice of the array to the root process
                MPI_Send(gatherBuffer, gatherCount, MPI_INT, mpiRootRank, mpiTag, MPI_COMM_WORLD);
            }

            // Splitting the communicator into smaller communicator for the remaining work latter
            MPI_Comm mpiShorterComm;
            int color = (mpiMyRank <= 3) ? 0 : MPI_UNDEFINED;
            MPI_Comm_split(MPI_COMM_WORLD, color, mpiMyRank, &mpiShorterComm);

            // All other processes will not be necessary anymore
            if ( !(mpiMyRank <= 3) ) break;

            // Broadcast the randomized array to smaller communicator
            MPI_Bcast(arr, arrSize, MPI_INT, mpiRootRank, mpiShorterComm);

            // Setting which slice of the array each process will work on
            switch (mpiMyRank) {
                case 0: {
                    sliceIndStr = indexL;
                    sliceIndMid = indexM1;
                    sliceIndEnd = indexM2;
                    break;
                }
                case 1: {
                    sliceIndStr = indexM2+1;
                    sliceIndMid = indexM3;
                    sliceIndEnd = indexM4;
                    break;
                }
                case 2: {
                    sliceIndStr = indexM4+1;
                    sliceIndMid = indexM5;
                    sliceIndEnd = indexM6;
                    break;
                }
                case 3: {
                    sliceIndStr = indexM6+1;
                    sliceIndMid = indexM7;
                    sliceIndEnd = indexR;
                    break;
                }
                default: {
                    fprintf(stderr, "ERROR switch (mpiMyRank). This should never happen.\n");
                    break;
                }
            }

            // Everyone gets to work
            arrMerge(arr, sliceIndStr, sliceIndMid, sliceIndEnd);

            if (mpiMyRank == mpiRootRank) {

                gatherIndex = sliceIndEnd + 1;

                int mpiShorterCommSize;
                MPI_Comm_size(mpiShorterComm, &mpiShorterCommSize);

                // Root process gather the results IN ORDER
                for (int mpiRank=1; mpiRank<mpiShorterCommSize; mpiRank++) {

                    // Receive the size of the buffer from the other processes
                    MPI_Recv(&gatherCount, 1, MPI_INT, mpiRank, mpiTag, mpiShorterComm, MPI_STATUS_IGNORE);
                    
                    // Setting the gather parameters based on each process work slice
                    gatherBuffer = &arr[gatherIndex];
                    
                    // Receive sorted slice of the array from the other processes
                    MPI_Recv(gatherBuffer, gatherCount, MPI_INT, mpiRank, mpiTag, mpiShorterComm, MPI_STATUS_IGNORE);

                    // Update index for next batch
                    gatherIndex += gatherCount;
                }

            } else {
                
                // Setting the gather parameters based on each process work slice
                gatherBuffer = &arr[sliceIndStr];
                gatherCount = sliceIndEnd - sliceIndStr + 1;

                // Send the size of its buffer to the root process
                MPI_Send(&gatherCount, 1, MPI_INT, mpiRootRank, mpiTag, mpiShorterComm);

                // Send its sorted slice of the array to the root process
                MPI_Send(gatherBuffer, gatherCount, MPI_INT, mpiRootRank, mpiTag, mpiShorterComm);
            }

            // Splitting the communicator into smaller communicator for the remaining work latter
            color = (mpiMyRank <= 1) ? 0 : MPI_UNDEFINED;
            MPI_Comm_split(mpiShorterComm, color, mpiMyRank, &mpiShorterComm);

            // All other processes will not be necessary anymore
            if ( !(mpiMyRank <= 1) ) {
                MPI_Comm_free(&mpiShorterComm);
                break;
            }

            // Broadcast the randomized array to smaller communicator
            MPI_Bcast(arr, arrSize, MPI_INT, mpiRootRank, mpiShorterComm);
            
            // Setting which slice of the array each process will work on
            switch (mpiMyRank) {
                case 0: {
                    sliceIndStr = indexL;
                    sliceIndMid = indexM2;
                    sliceIndEnd = indexM4;
                    break;
                }
                case 1: {
                    sliceIndStr = indexM4+1;
                    sliceIndMid = indexM6;
                    sliceIndEnd = indexR;
                    break;
                }
                default: {
                    fprintf(stderr, "ERROR switch (mpiMyRank). This should never happen.\n");
                    break;
                }
            }

            // Everyone gets to work
            arrMerge(arr, sliceIndStr, sliceIndMid, sliceIndEnd);

            if (mpiMyRank == mpiRootRank) {

                gatherIndex = sliceIndEnd + 1;
                
                int mpiShorterCommSize;
                MPI_Comm_size(mpiShorterComm, &mpiShorterCommSize);

                // Root process gather the results IN ORDER
                for (int mpiRank=1; mpiRank<mpiShorterCommSize; mpiRank++) {

                    // Receive the size of the buffer from the other processes
                    MPI_Recv(&gatherCount, 1, MPI_INT, mpiRank, mpiTag, mpiShorterComm, MPI_STATUS_IGNORE);
                    
                    // Setting the gather parameters based on each process work slice
                    gatherBuffer = &arr[gatherIndex];
                    
                    // Receive sorted slice of the array from the other processes
                    MPI_Recv(gatherBuffer, gatherCount, MPI_INT, mpiRank, mpiTag, mpiShorterComm, MPI_STATUS_IGNORE);

                    // Update index for next batch
                    gatherIndex += gatherCount;
                }

            } else {
                
                // Setting the gather parameters based on each process work slice
                gatherBuffer = &arr[sliceIndStr];
                gatherCount = sliceIndEnd - sliceIndStr + 1;

                // Send the size of its buffer to the root process
                MPI_Send(&gatherCount, 1, MPI_INT, mpiRootRank, mpiTag, mpiShorterComm);

                // Send its sorted slice of the array to the root process
                MPI_Send(gatherBuffer, gatherCount, MPI_INT, mpiRootRank, mpiTag, mpiShorterComm);
            }
            
            // Tasks only for root process
            if (mpiMyRank == mpiRootRank) {
                arrMerge(arr, indexL, indexM4, indexR);
            }
            
            MPI_Comm_free(&mpiShorterComm);
            break;
        }
        default: {
            printf("ERROR switch (mpiCommSize). This should never happen.\n");
            break;
        }
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

// --------------- MISC FUNCTIONS -------------------

void print_help(void) {

    // Tasks only for root process
    if (mpiMyRank == mpiRootRank) printf(
        "\n"
        "USAGE\n"
        "       mpirun  -n (2|4|8)  mpims arraySize [options]\n"
        "\n"
        "DESCRIPTION\n"
        "       Sort (in ascending order) a randomly generated array of integers Merge-Sort algorithm, usign OpenMPI.\n"
        "\n"
        "       arraySize must be an integer value bigger than 0, and with the maximum value of 1000000 (10^6).\n"
        "       Due to the way the algorithm is implemented, the number of processes initiated by MPI must be a power\n"
        "       of 2; currently, only up to 8.\n"
        "\n"
        "OPTIONS\n"
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

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
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
