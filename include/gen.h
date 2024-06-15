#include <sodium.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// TODO: 
// -  

char* generatePass(int iOption, int iLength) {
  short index = 0;
  int len = 0;

  if (sodium_init() < 0) {
    printf("[x] Error initializing sodium.\n");
    exit(1);
  }

  const char* sOption[] = {
    ".,-<>@+*$/()=?{}[]!#%&12345ABCDEFGHIJKLMNOPTVWXYZabcdefghijklmnopqrstuvwxyz",
    "12345ABCDEFGHIJKLMNOPTVWXYZabcdefghijklmnopqrstuvwxyz",
    "ABCDEFGHIJKLMNOPTVWXYZabcdefghijklmnopqrstuvwxyz"
  };

  char* pcPassword = (char*)calloc((size_t)(iLength + 1), sizeof(char));
  if (!pcPassword) {
    printf("[x] Error allocating memory.\n");
    exit(1);
  }
  
  switch (iOption) {
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

  for (int i = 0; i < iLength; i++) {
    pcPassword[i] = sOption[index][randombytes_uniform(len)];
  }

  return pcPassword;
}
