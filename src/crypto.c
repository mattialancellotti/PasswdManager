#define _POSIX_C_SOURCE 200809L
#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <string.h>

#if defined(_HAVE_SODIUM)
#  include <sodium.h>
#endif

#include <pass/defs.h>
#include <pass/os.h>

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

int init_crypt(void)
{
   file_t *nonce_stream, *key_stream;
   const char *nonce_path = "nonce";
   const char *key_path = "key";
   int err = 0;

   /* Creating the paths */
   char *files = absolute_path(PROG_ROOT ROOT_PATH);
   char *nonce_file = malloc(strlen(files) + strlen(nonce_path) + 1);
   char *key_file = malloc(strlen(files) + strlen(key_path) + 1);
   nonce_file = strcat(strcpy(nonce_file, files), nonce_path);
   key_file = strcat(strcpy(key_file, files), key_path);
   
   /* Declaring the nonce and the key used by the sodium library */
   unsigned char nonce[crypto_aead_chacha20poly1305_NPUBBYTES];
   unsigned char key[crypto_aead_chacha20poly1305_KEYBYTES];

   /* Initializing stuff */
   crypto_aead_chacha20poly1305_keygen(key);
   randombytes_buf(nonce, sizeof(nonce));

   /* Saving the parameters */
   nonce_stream = mcreate(nonce_file);
   key_stream = mcreate(key_file);
   prog_err((nonce_stream == NULL || key_stream == NULL),
               "Couldn't open the files.", );

   int nout = cwrite(nonce_stream->fd, (const char *)nonce, sizeof(nonce));
   int kout = cwrite(key_stream->fd, (const char *)key, sizeof(key));
   warn_user((nout == -1 || kout == -1),
               "Couldn't generate the needed data.", -1);

   err = mclose(nonce_stream);
   err = mclose(key_stream);
   prog_err((err == -1), "Couldn't close the files.", );

   free(nonce_file);
   free(key_file);

   return 0;
}
