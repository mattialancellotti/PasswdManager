#define _POSIX_C_SOURCE 200809L
#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <string.h>
#include <sodium.h>

#include <pass/defs.h>
#include <pass/os.h>

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
