#include <stdio.h>
#include <stdlib.h>    // atoi, rand

void print_help(void);
void displayArray();

int main(int argc, char *argv[]) {

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

    printf("Array before randomizing.\n");
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
    printf("Array after randomizing.\n");
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