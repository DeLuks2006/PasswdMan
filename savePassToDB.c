#include <sqlite3.h>
#include <stdio.h>
#include "sodium.h"
#include <string.h>
#include <stdlib.h>

#define MASTER_PASS "verySecurePassword" // totally not stored in the DB too
#define ADDITIONAL_DATA (const unsigned char*) "secretDataVerySecureAndHidden123456"

int main() {
	/*---------[ VARIABLE DECLARATION ]---------*/
	// for sql
	sqlite3* db;
	char* err_msg = 0;
	int rc = 0;
	sqlite3_int64 last_id = 0;
	sqlite3_stmt* res;
	// misc
	char	name[64];
	char	password[64];
	char	answer = '\0';
	// for encrypting
	unsigned char nonce[crypto_aead_aes256gcm_NPUBBYTES];
	unsigned char key[] = MASTER_PASS;
	unsigned char cipher[sizeof(password) + crypto_aead_aes256gcm_ABYTES];
	unsigned long long cipher_len;
	
	/*---------[ INIT LIBRARIES AND OPEN DATABASE ]---------*/
	// init sodium
	if (sodium_init() < 0) {
		fprintf(stderr, "[-] Error initializing sodium.\n");
		return 1;
	}
	// check if aes is available
	if (!crypto_aead_aes256gcm_is_available()) {
		fprintf(stderr, "[-] AES-256-GCM is not available on this CPU.");
		abort();
	}
	// generate nonce
	randombytes_buf(nonce, sizeof(nonce));

	// open db
	printf("[i] Opening Database\n");
	rc = sqlite3_open("test.db", &db);

	if (rc != SQLITE_OK) {
		fprintf(stderr, "[-] Cannot open database: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);

		return 1;
	}

	/*---------[ CREATE TABLE ]---------*/
	const char* create_table_sql = "CREATE TABLE IF NOT EXISTS passwords(Id INTEGER PRIMARY KEY, Name TEXT, Password TEXT);";
	rc = sqlite3_exec(db, create_table_sql, 0, 0, &err_msg);

	if (rc != SQLITE_OK) {
		fprintf(stderr, "[-] Failed to create table\n");
		fprintf(stderr, "[i] SQL Error: %s\n", err_msg);
		sqlite3_free(err_msg);
		sqlite3_close(db);
		return 1;
	}
	printf("[i] Table \"passwords\" created or already exists\n");


	/*---------[ READ INPUT FROM USER ]---------*/
	char* insert = "INSERT INTO passwords(Name, Password) VALUES(@name, @pass);";

	do {
		answer = 0;
		// service name
		printf("Input the Name of the Service (Max. 64 characters): ");
		fgets(name, sizeof(name), stdin);
		name[strcspn(name, "\n")] = 0;
		// password
		printf("Input the Current Password (Max. 64 characters): ");
		fgets(password, sizeof(password), stdin);
		name[strcspn(password, "\n")] = 0;

		crypto_aead_aes256gcm_encrypt(
			cipher,
			&cipher_len,
			password,
			sizeof(password),
			ADDITIONAL_DATA,
			sizeof(ADDITIONAL_DATA),
			NULL,
			nonce,
			MASTER_PASS
			);
		memset(password, 0, sizeof(password)); // get rid of the password

		rc = sqlite3_prepare_v2(db, insert, -1, &res, 0);
		if (rc != SQLITE_OK) {
			fprintf(stderr, "[-] Failed to compile statement.\n");
			sqlite3_close(db);
			return 1;
		}

		sqlite3_bind_text(res, 1, name, -1, SQLITE_STATIC);
		sqlite3_bind_text(res, 2, cipher, -1, SQLITE_STATIC);

		int step = sqlite3_step(res);
		// error happens somewhere here i think
		while (answer != 'y' && answer != 'Y' && answer != 'N' && answer != 'n') {
			printf("[i] Do you want to add another entry? (Y/N): ");
			scanf("%c", &answer);
			getchar();
		}

		memset(cipher, 0, sizeof(cipher));
		memset(name, 0, sizeof(name));

	} while (answer != 'n' && answer != 'N');


	/*---------[ FINISH ]---------*/
	// returns the id of the last inserted row
	last_id = sqlite3_last_insert_rowid(db);
	printf("[i] Last ID of inserted row is: %d\n",(int)last_id);

	sqlite3_finalize(res);
	sqlite3_close(db);
	return 0;
}
