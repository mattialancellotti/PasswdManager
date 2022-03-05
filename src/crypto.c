#define _POSIX_C_SOURCE 200809L
#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <string.h>
#include <sodium.h>

#include <pass/defs.h>
#include <pass/os.h>

/*
 * TODO:
 *    - Documentation;
 *    - Common gen_func for both gen_salt and gen_nonce;
 *    - Locking memory;
 *    - Decryption function;
 *    - A bit more error checking.
 */
static unsigned char *gen_salt(void);
static unsigned char *derive_key(const char* /*password*/);

char *hash_password(const char *password)
{
   prog_err((password == NULL), "Invalid password.", return NULL);

   /* Defining parameters */
   char hash[crypto_pwhash_STRBYTES];
   unsigned long long pwlen = strlen(password);
   unsigned long long opslimit = 3;
   size_t memlimit = crypto_pwhash_MEMLIMIT_SENSITIVE/2;

   /* Hashing the password */
   int pwout = crypto_pwhash_str(hash, password, pwlen, opslimit, memlimit);
   system_err((pwout == -1), "crypto_pwhash_str", NULL);

   return strdup(hash);
}

int hash_check(const char *hash, const char *passwd)
{
   /* Checking parameters */
   prog_err((hash == NULL), "Invalid hash.", return -1);
   prog_err((passwd == NULL), "Invlaid password.", return -1);

   /* Checking the hash */
   int err = crypto_pwhash_str_verify(hash, passwd, strlen(passwd));
   return err;
}

static unsigned char *gen_salt(void)
{
   prog_err((program_db == NULL), "Unknown database.", return 0);

   char *salt = malloc(crypto_pwhash_SALTBYTES);
   char *salt_path = absolute_path(PROG_ROOT ROOT_PATH PASS_SALT);
   /*
    * The generated salt needs to be always the same to obtain the same key.
    * Becuase of that before generating some salt we gotta check if it already
    * exists.
    */
   if (exists(salt_path)) {
      file_t *salt_file = mopen(salt_path);
      if (salt_file == NULL || salt_file->file_content == NULL) {
         fprintf(stderr, "Couldn't read the salt.");
         return 0;
      }

      /* Copying the given salt to the array */
      salt = strcpy(salt, salt_file->file_content);
      mclose(salt_file);
   } else {
      unsigned char tmp[crypto_pwhash_SALTBYTES];
      randombytes_buf(tmp, sizeof(tmp));

      /* Generating the salt and saving it */
      salt = memcpy(salt, tmp, crypto_pwhash_SALTBYTES);
      file_t *salt_file = mcreate(salt_path);
      if (salt_file == NULL) {
         fprintf(stderr, "Couldn't create the salt.");
         return 0;
      }

      int err = cwrite(salt_file->fd, salt, crypto_pwhash_SALTBYTES);
      if (err == -1) {
         fprintf(stderr, "Coulnd't save the salt.");
         return 0;
      }
      mclose(salt_file);
   }

   free(salt_path);
   return (unsigned char *)salt;
}

/* TODO if the encryption system works imma create a single function that
 * accepts a pointer function to generate either the salt or the nonce */
static unsigned char *gen_nonce(void)
{
   prog_err((program_db == NULL), "Unknown database.", return 0);

   char *nonce = malloc(crypto_secretbox_NONCEBYTES);
   char *nonce_path = absolute_path(PROG_ROOT ROOT_PATH PASS_NONCE);
   /*
    * The generated salt needs to be always the same to obtain the same key.
    * Becuase of that before generating some salt we gotta check if it already
    * exists.
    */
   if (exists(nonce_path)) {
      file_t *nonce_file = mopen(nonce_path);
      if (nonce_file == NULL || nonce_file->file_content == NULL) {
         fprintf(stderr, "Couldn't read the salt.");
         return 0;
      }

      /* Copying the given salt to the array */
      nonce = strcpy(nonce, nonce_file->file_content);
      mclose(nonce_file);
   } else {
      unsigned char tmp[crypto_secretbox_NONCEBYTES];
      randombytes_buf(tmp, sizeof(tmp));

      /* Generating the salt and saving it */
      nonce = memcpy(nonce, tmp, crypto_secretbox_NONCEBYTES);
      file_t *nonce_file = mcreate(nonce_path);
      if (nonce_file == NULL) {
         fprintf(stderr, "Couldn't create the salt.");
         return 0;
      }

      int err = cwrite(nonce_file->fd, nonce, crypto_secretbox_NONCEBYTES);
      if (err == -1) {
         fprintf(stderr, "Coulnd't save the salt.");
         return 0;
      }
      mclose(nonce_file);
   }

   free(nonce_path);
   return (unsigned char *)nonce;
}

static unsigned char *derive_key(const char *password)
{
   prog_err((password == NULL), "Invalid password.", return NULL);

   /* To generate a key from the given password some things are needed */
   unsigned char *key_xchg = malloc(sizeof(unsigned char) * crypto_secretbox_KEYBYTES);
   unsigned char key[crypto_secretbox_KEYBYTES];
   const unsigned long long password_len = strlen(password);

   /* Generating the salt we need */
   unsigned char *salt = gen_salt();

   /* This function generates a decent secret key from the user's password. */
   if (crypto_pwhash(key, sizeof(key), password, password_len, salt,
            crypto_pwhash_OPSLIMIT_MODERATE, crypto_pwhash_MEMLIMIT_MODERATE,
            crypto_pwhash_ALG_ARGON2ID13) != 0) {
      fprintf(stderr, "Went out of memory while generating the secret key.\n");
      return NULL;
   }

   /* Copying the key */
   key_xchg = memcpy(key_xchg, key, crypto_secretbox_KEYBYTES);

   free(salt);
   return key_xchg;
}

unsigned char *encrypt_password(const char *auth, const char *password, unsigned long long *bytes)
{
   prog_err((auth == NULL), "Invalid password.", return NULL);
   prog_err((password == NULL), "Invalid password.", return NULL);

   /* Some things are needed before encrypting the message */
   const unsigned long long passwd_len = strlen(password);
   *bytes = crypto_secretbox_MACBYTES + passwd_len;
   unsigned char *ciphertext = malloc(sizeof(unsigned char) * bytes);

   /* Generating the secret key */
   unsigned char *key = derive_key(auth);
   unsigned char *nonce = gen_nonce();

   /* Deriving a key long enough to be used to encrypt files */
   crypto_secretbox_easy(ciphertext, (unsigned char*)password,
                           passwd_len, nonce, key);
   
   return ciphertext;
}

unsigned char *decrypt_password(const unsigned char *ciphertext, const char *auth, unsigned long long bytes)
{
   prog_err((ciphertext == NULL), "Invalid password.", return NULL);
   prog_err((auth == NULL), "Invalid password.", return NULL);

   /* We need to gather some information like the nonce and a key */
   //const unsigned long long cipherlen = ;
   unsigned char *clear_password = malloc(sizeof(unsigned char) * bytes);

   unsigned char *key = derive_key(auth);
   unsigned char *nonce = gen_nonce();

   /* Decrypting the message */
   if (crypto_secretbox_open_easy(clear_password, ciphertext,
                                    bytes, nonce, key) != 0) {
      fprintf(stderr, "Couldn't decrypt the message.");
      return NULL;
   }

   return clear_password;
}
