#define _POSIX_C_SOURCE 200809L
#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <string.h>
#include <sodium.h>

#include <pass/defs.h>
#include <pass/os.h>

static unsigned char *gen_salt(void);

/* TODO:
 *    - Write a secure hash_password (mlock, munlock)
 */
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

int encrypt_password(const char *password)
{
   prog_err((password == NULL), "Invalid password.", return -1);

   /* Before generating a valid key we have to generate salt */
   unsigned char *salt = gen_salt();

   /* Deriving a key long enough to be used to encrypt files */
   
   return 0;
}
