#include <stdio.h>
#include <stdlib.h>    // atoi, rand

// Sort (in ascending order) a randomly generated array of integers, using Merge-Sort algorithm.

void print_help(void);
void displayArray();

void arrMerge();
void arrDivide();
void mergeSort();

int main(int argc, char* argv[]) {

    // No extra command line argument passed
    if (argc != 2) {
        print_help();
        return 0;
    }

    // Get arguments
    int arraySize = atoi (argv[1]);                 // Interprets an integer value in a byte string
    printf("Array size: %d\n", arraySize);
    printf("\n");
    if ( !(0 < arraySize && arraySize <= 1000000) ) {
        print_help();
        return 0;
    }

    // Creating array
    int array[arraySize];
    for (int i=0; i<arraySize; i++)
        array[i] = i;

    printf("Array before randomizing:\n");
    displayArray(array, arraySize);
    printf("\n");
    
    // Randomizing
    srand (42069);
    int randomIndex, aux;
    for (int i=0; i<arraySize; i++) {
        randomIndex = rand() % arraySize;           // rand() returns a pseudo-random integer value
        aux = array[i];
        array[i] = array[randomIndex];
        array[randomIndex] = aux;
    }
    printf("Array after randomizing:\n");
    displayArray(array, arraySize);

    // Sorting
    mergeSort(array, arraySize);                    // To-do: include print of sorting process
    printf("Array after sorting:\n");
    displayArray(array, arraySize);

    return 0;
}

void print_help(void) {
    printf(
        "Usage:\n"
        "   seqms arraySize\n"
        "\n"
        "Description:\n"
        "    arraySize must be an integer value bigger than 0, and with the maximum value of 10^6. Using more might overflow (for now).\n"
        "\n"
    );
}

void displayArray(int *arr, int arrSize) {
    printf("array: [");
    int counter = 0;
    for (int i=0; i<arrSize; i++) {
        printf( arrSize <=      10 ? "%1i " :
                arrSize <=     100 ? "%2i " :
                arrSize <=    1000 ? "%3i " :
                arrSize <=   10000 ? "%4i " :
                arrSize <=  100000 ? "%5i " : "%6i ", arr[i]);
        counter++;
        if (counter == 25 && i != arrSize-1) {
            printf("\n        ");
            counter = 0;
        }
    }
    printf("]\n");
}

// Merge two subarrays of arr in order
// Internal function of mergeSort; should no be called in main
void arrMerge(int arr[], int indexL, int indexM, int indexR) {
    
    // subarrays are:
    // arrAux1[indexL .. indexM]
    // arrAux2[indexM+1..indexR]
    
    // Auxiliary arrays
    int arrAuxSize1 = (indexM - indexL) + 1;
    int arrAuxSize2 =  indexR - indexM;
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

    // If the indexes are the same, the subarray has only one element, and thus, is sorted
    if (indexL < indexR) {

        int indexM = indexL + (indexR - indexL) / 2;

        arrDivide(arr, indexL, indexM);
        arrDivide(arr, indexM + 1, indexR);

        arrMerge(arr, indexL, indexM, indexR);
    }
}

void mergeSort(int *arr, int arrSize) {

    arrDivide(arr, 0, arrSize-1);
}