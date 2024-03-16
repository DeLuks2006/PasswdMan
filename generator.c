#include <sodium.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#pragma warning(disable: 4996) // disable the stinky scanf warning

int main() {
    /*---------[ VARIABLE DECLARATIONS ]---------*/
    int iInput = 0;
    int iSuccessfulRead = 0;
    int iLength = 0;
    int index = 0;
    int len = 0;

    /*---------[ INITIALIZING LIBRARY ]---------*/
    if (sodium_init() < 0) {
        printf("[-] Error initializing sodium. :(\n");
        exit(1);
    }

    /*---------[ ACTUAL LOGIC HERE ]---------*/
    printf(".---------[ Randomly Generated Passwords ]---------.\n");

    // lists of characters to pick from
    const char* sOption[] = {
        ".,-<>@+*$/()=?{}[]!#%&12345ABCDEFGHIJKLMNOPTVWXYZabcdefghijklmnopqrstuvwxyz",
        "12345ABCDEFGHIJKLMNOPTVWXYZabcdefghijklmnopqrstuvwxyz",
        "ABCDEFGHIJKLMNOPTVWXYZabcdefghijklmnopqrstuvwxyz"
    };

    // prompt user for a option to use
    printf("Please enter an option to enter your password:\n");
    printf("\t1 > Characters, Numbers and Special-Characters\n");
    printf("\t2 > Characters and Numbers\n");
    printf("\t3 > Characters\n");

    do {
        printf("input: ");
        iSuccessfulRead = scanf("%d", &iInput);
        if (iSuccessfulRead != 1) {
            // Clear the input buffer
            while (getchar() != '\n');
            printf("Invalid input. Please enter a valid option.\n");
        }
    } while (iInput != 1 && iInput != 2 && iInput != 3);

    printf("\nSelected Option: %d\n", iInput);

    // prompt user for desired password length
    do {
        printf("Input the desired password length: ");
        iSuccessfulRead = scanf("%d", &iLength);
        if (iSuccessfulRead != 1 || iLength <= 0) {
            // Clear the input buffer
            while (getchar() != '\n');
            printf("Invalid input. Please enter a positive integer.\n");
        }
    } while (iSuccessfulRead != 1 || iLength <= 0);

    char* pass = (char*)calloc((size_t)(iLength + 1), sizeof(char));
    if (!pass) {
        printf("[-] Error allocating memory with calloc. :(\n");
        exit(1);
    }

    switch (iInput) {
    case 1:
        index = 0;
        len = (int)strlen(sOption[index]);
        break;
    case 2:
        index = 1;
        len = (int)strlen(sOption[index]);
        break;
    case 3:
        index = 2;
        len = (int)strlen(sOption[index]);
        break;
    }

    // generate and display password
    for (int i = 0; i < iLength; i++) {
        pass[i] = sOption[index][randombytes_uniform(len)];
    }

    printf("Password: %s\n", pass);

    free(pass);
    return 0;
}
