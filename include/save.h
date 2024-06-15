#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sodium.h>
#include <sqlite3.h>

#define SALT (const unsigned char*) "temporary value"

int save(char* sMasterPass) {
  sqlite3* db;
  char* err_msg = 0;
  int rc = 0;
  sqlite3_stmt* res;

  char name[65];
  char password[65];
  char answer = 0;
  char answerBuffer[2] = { 0 };
  int c;

  unsigned char nonce[crypto_aead_aes256gcm_NPUBBYTES];
  unsigned char key[crypto_aead_aes256gcm_KEYBYTES];
  unsigned char cipher[sizeof(password) + crypto_aead_aes256gcm_ABYTES];
  unsigned long long cipher_len;

  if (sodium_init() < 0) {
    puts("[-] Error initializing sodium");
    return 1;
  }
  if (!crypto_aead_aes256gcm_is_available()) {
    puts("[-] AES-256-GCM is not available on this system");
    abort();
  }

  memcpy(key, sMasterPass, crypto_aead_aes256gcm_KEYBYTES);

  rc = sqlite3_open("credentials.db", &db);
  if (rc != SQLITE_OK) {
    printf("[-] Cannot open database: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    return 1;
  }

  const char* create_table = "CREATE TABLE IF NOT EXISTS passwords(Id INTEGER PRIMARY KEY, Name TEXT, Password BLOB, Nonce BLOB);";
  rc = sqlite3_exec(db, create_table, 0, 0, &err_msg);
  if (rc != SQLITE_OK) {
    printf("[-] Failed to create table\n");
    printf("[i] SQL Error: %s\n", err_msg);
    sqlite3_free(err_msg);
    sqlite3_close(db);
    return 1;
  }

  const char* insert = "INSERT INTO passwords(Name, Password, Nonce) VALUES(?, ?, ?);";
  do {
    answer = 0;
    *answerBuffer = 0;
    printf("Name of Service (Max. 64): ");
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = 0;

    printf("Password (Max. 64): ");
    fgets(password, sizeof(password), stdin);
    password[strcspn(password, "\n")] = 0;

    randombytes_buf(nonce, sizeof(nonce));

    crypto_aead_aes256gcm_encrypt(
      cipher,
      &cipher_len,
      (unsigned char*)password,
      strlen(password),
      SALT,
      strlen((char*)SALT),
      NULL,
      nonce,
      key
    );
    memset(password, 0, sizeof(password));

    rc = sqlite3_prepare_v2(db, insert, -1, &res, 0);
    if (rc != SQLITE_OK) {
      printf("[-] Failed to compile statement\n");
      sqlite3_close(db);
      return 1;
    }

    sqlite3_bind_text(res, 1, name, -1, SQLITE_STATIC);
    sqlite3_bind_blob(res, 2, cipher, (int)cipher_len, SQLITE_STATIC);
    sqlite3_bind_blob(res, 3, nonce, sizeof(nonce), SQLITE_STATIC);

    int step = sqlite3_step(res);

    if (step != SQLITE_DONE) {
      printf("[-] Execution failed: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(res);

    while (answer != 'y' && answer != 'Y' && answer != 'n' && answer != 'N') {
      puts("Add another entry? (Y/N)");
      fgets(answerBuffer, sizeof(answerBuffer), stdin);
      answer = answerBuffer[0];

      while ((c = getchar()) != '\n' && c != EOF);
    }

    memset(cipher, 0, sizeof(cipher));
    memset(name, 0, sizeof(name));
  } while (answer != 'n' && answer != 'N');

  sqlite3_close(db);
  return 0;
}

int show_passwords(char* sMasterPass) {
  sqlite3* db;
  sqlite3_stmt* res;
  int rc;

  unsigned char key[crypto_aead_aes256gcm_KEYBYTES];
  //unsigned char nonce[crypto_aead_aes256gcm_NPUBBYTES];
  unsigned char decrypted[65];
  unsigned long long decrypted_len;
  const char* select = "SELECT Name, Password, Nonce FROM passwords;";

  if (sodium_init() < 0) {
    puts("[-] Error initializing sodium");
    return 1;
  }
  memcpy(key, sMasterPass, crypto_aead_aes256gcm_KEYBYTES);

  rc = sqlite3_open("credentials.db", &db);
  if (rc != SQLITE_OK) {
    printf("[-] Cannot open database: %s\n", sqlite3_errmsg(db));
    return 1;
  }

  rc = sqlite3_prepare_v2(db, select, -1, &res, 0);
  if (rc != SQLITE_OK) {
    printf("[-] Failed to compile statement\n");
    sqlite3_close(db);
    return 1;
  }

  printf("Stored Passwords:\n");
  while (sqlite3_step(res) == SQLITE_ROW) {
    const char* name = (const char*)sqlite3_column_text(res, 0);
    const unsigned char* encrypted = (const unsigned char*)sqlite3_column_blob(res, 1);
    int encrypted_len = sqlite3_column_bytes(res, 1);
    const unsigned char* stored_nonce = (const unsigned char*)sqlite3_column_blob(res, 2);

    if (crypto_aead_aes256gcm_decrypt(
          decrypted, &decrypted_len,
          NULL,
          encrypted, encrypted_len,
          SALT, strlen((char*)SALT),
          stored_nonce,
          key
        ) != 0) {
      printf("[-] Decryption failed for %s\n", name);
      continue;
    }

    decrypted[decrypted_len] = '\0';
    printf("%s: %s\n", name, decrypted);
  }

  sqlite3_finalize(res);
  sqlite3_close(db);

  return 0;
}

