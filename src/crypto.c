#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <string.h>

#if defined(__libsodium__)
#  include <sodium.h>
#endif

#include <pass/defs.h>

/* TODO:
 *    - Write a secure hash_password (mlock, munlock)
 */
char *hash_password(const char *password)
{
   just_exit(password, NULL, NULL);

   /* Defining parameters */
   char hash[crypto_pwhash_STRBYTES];
   unsigned long long pwlen = strlen(password);
   unsigned long long opslimit = 3;
   size_t memlimit = crypto_pwhash_MEMLIMIT_SENSITIVE/2;

   /* Hashing the password */
   int pwout = crypto_pwhash_str(hash, password, pwlen, opslimit, memlimit);
   fatal_err(pwout, -1, "crypto_pwhash_str", NULL);

   return strdup(hash);
}

int check_hash(const char *hash, const char *passwd)
{
   just_exit(hash, NULL, -1);
   just_exit(passwd, NULL, -1);

   /* Checking the hash */
   int err = crypto_pwhash_str_verify(hash, passwd, strlen(passwd));
   return err;
}
