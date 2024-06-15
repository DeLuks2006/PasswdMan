#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sodium.h>
#include "../include/gen.h" // generate passwords
#include "../include/save.h" // save passwords
#ifdef _WIN32
  #define OS_WINDOWS
#else 
  #define OS_LINUX
#endif

#define HEX_SIZE (crypto_hash_sha512_BYTES * 2 + 1)

int checkOS() {
  #ifdef OS_WINDOWS
    return 0x4D5A;   // MZ
  #else 
    return 0x454C46; // ELF 
  #endif
}

void clearInputBuffer() {
  int c;
  while ((c = getchar()) != '\n' && c != EOF);
}

void banner() {
  (checkOS() == 0x4D5A) ? system("cls") : system("clear");
  printf("      :::::::::     :::      ::::::::   ::::::::  :::       ::: :::::::::    :::   :::       :::     ::::    ::: \n");
  printf("     :+:    :+:  :+: :+:   :+:    :+: :+:    :+: :+:       :+: :+:    :+:  :+:+: :+:+:    :+: :+:   :+:+:   :+:  \n");
  printf("    +:+    +:+ +:+   +:+  +:+        +:+        +:+       +:+ +:+    +:+ +:+ +:+:+ +:+  +:+   +:+  :+:+:+  +:+   \n");
  printf("   +#++:++#+ +#++:++#++: +#++:++#++ +#++:++#++ +#+  +:+  +#+ +#+    +:+ +#+  +:+  +#+ +#++:++#++: +#+ +:+ +#+    \n");
  printf("  +#+       +#+     +#+        +#+        +#+ +#+ +#+#+ +#+ +#+    +#+ +#+       +#+ +#+     +#+ +#+  +#+#+#     \n");
  printf(" #+#       #+#     #+# #+#    #+# #+#    #+#  #+#+# #+#+#  #+#    #+# #+#       #+# #+#     #+# #+#   #+#+#      \n");
  printf("###       ###     ###  ########   ########    ###   ###   #########  ###       ### ###     ### ###    ####       \n");
  puts("");
}

void bin2hex(unsigned char *bin, size_t bin_len, char *hex) {
  for (size_t i = 0; i < bin_len; i++) {
    sprintf(hex + (i * 2), "%02x", bin[i]);
  }
  hex[bin_len * 2] = '\0';
}

void hex2bin(const char *hex, unsigned char *bin, size_t bin_len) {
  for (size_t i = 0; i < bin_len; i++) {
    sscanf(hex + (i * 2), "%02hhx", &bin[i]);
  }
}

int main(void) {
  char input[3];
  char master[65];
  char master2[65];
  char* buf;
  int option = 0;
  int length = 0;
  FILE* file;
  unsigned char out[crypto_hash_sha512_BYTES];
  unsigned char in[crypto_hash_sha512_BYTES];
  char hex_out[HEX_SIZE];

  if (sodium_init() < 0) {
    puts("[-] Error initializing sodium");
    return 1;
  }

  banner();
  
  if ((file = fopen("master.hash", "r")) != NULL) {
    fgets(hex_out, sizeof(hex_out), file);
    fclose(file);
    hex2bin(hex_out, out, crypto_hash_sha512_BYTES);
  } else {
    if ((file = fopen("master.hash", "w+")) != NULL) {
      printf("Could not find master password\n");
      printf("Enter new master password: ");
      fgets(master2, sizeof(master2), stdin);
      master2[strcspn(master2, "\n")] = 0;

      crypto_hash_sha512(out, (unsigned char*)master2, strlen(master2));
      bin2hex(out, crypto_hash_sha512_BYTES, hex_out);
      fprintf(file, "%s\n", hex_out);
      fclose(file);
      return 0;
    }
  }

  printf("Master Password: ");
  fgets(master, sizeof(master), stdin);
  master[strcspn(master, "\n")] = 0;
  // hash input
  crypto_hash_sha512(in, (unsigned char*)master, strlen(master));

  // compare with masterpass
  if (memcmp(in, out, crypto_hash_sha512_BYTES) != 0) {
    printf("[-] Incorrect master password\n");
    return 1;
  }

  while (1) {
    // MENU 1: Store, Generate or Exit
    banner();
    puts("1 > Generate Password");
    puts("2 > Store Password");
    puts("3 > Show Passwords");
    puts("4 > Exit");
    printf(" $ ");
    fgets(input, sizeof(input), stdin);
    option = atoi(input);

    // MENU 2: Generating Password
    if (option == 1) {
      banner();
      puts("1 > Letters, Numbers and Special Characters");
      puts("2 > Letters and Numbers");
      puts("3 > Only Letters");
      printf(" $ ");
      fgets(input, sizeof(input), stdin);
      clearInputBuffer();
      option = atoi(input);
      
      printf("Length (Max. 64): ");
      fgets(input, sizeof(input), stdin);
      clearInputBuffer();
      length = atoi(input);

      if (length > 64) {
        length = 64;
      }

      buf = generatePass(option, length);
      printf("Generated Password: %s\n", buf);
      free(buf);
      
      puts("Press any key to continue...");
      getchar();
    } // MENU 3: Storing Password
    else if (option == 2) {
      banner();
      
      save(master);

      getchar();
    } 
    else if (option == 3) {
      banner();
      show_passwords(master);
      puts("Press any key to continue...");
      getchar();
    } 
    else if (option == 4) {
      (checkOS() == 0x4D5A)?system("cls"):system("clear");
      exit(EXIT_SUCCESS);
    } 
    else {
      puts("Invalid Option...");
      fgets(input, sizeof(input), stdin);
    }
  }

  return EXIT_SUCCESS;
}

