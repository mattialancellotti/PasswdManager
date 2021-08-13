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
   exit_eq(password, NULL, NULL);

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
   exit_eq(hash, NULL, -1);
   exit_eq(passwd, NULL, -1);
   exit_lt(strlen(hash), crypto_pwhash_STRBYTES, -1);

   /* Checking the hash */
   int err = crypto_pwhash_str_verify(hash, passwd, strlen(passwd));
   return err;
}

int encrypt_content(const unsigned char *content, const char *passwd)
{
   /* Checking if the content is not empty */
   exit_eq(content, NULL, -1);

   /* Defining parameters */
   size_t clen = strlen((const char *)content);
   size_t plen = strlen(passwd);

   unsigned char nonce[crypto_aead_chacha20poly1305_NPUBBYTES];
   unsigned char *key = sodium_malloc(sizeof(unsigned char) *
                                 crypto_aead_chacha20poly1305_KEYBYTES);
   unsigned char ciphertext[clen + crypto_aead_chacha20poly1305_ABYTES];
   unsigned long long ciphertext_len;

   /* Initiating encryption */
   crypto_aead_chacha20poly1305_keygen(key);
   randombytes_buf(nonce, sizeof(nonce));
   key = memcpy(key, passwd, plen);

   /* Encrypting the message */
   int chacha20out =
      crypto_aead_chacha20poly1305_encrypt(ciphertext, &ciphertext_len,
                                             content, clen, NULL, 0, NULL,
                                             nonce, key);
   return chacha20out;
}
