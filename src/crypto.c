#define _POSIX_C_SOURCE 200809L
#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <string.h>

#if defined(_HAVE_SODIUM)
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

int hash_check(const char *hash, const char *passwd)
{
   /* Checking parameters */
   exit_eq(hash, NULL, -1);
   exit_eq(passwd, NULL, -1);

   /* Checking the hash */
   int err = crypto_pwhash_str_verify(hash, passwd, strlen(passwd));
   return err;
}

/* TODO:
 *  - `crypto_write` to write a complete new encrypted file;
 *  - `crypto_append` to append new data to an encrypted file;
 */
static unsigned char nonce[crypto_aead_chacha20poly1305_NPUBBYTES];
static unsigned char *key;

int encrypt_content(const unsigned char *content, unsigned char **secret,
                     const char *passwd)
{
   /* Checking if the content is not empty */
   exit_eq(content, NULL, -1);

   /* Defining parameters */
   size_t clen = strlen((const char *)content);
   size_t plen = strlen(passwd);

   unsigned char ciphertext[clen + crypto_aead_chacha20poly1305_ABYTES + 1];
   unsigned long long ciphertext_len;

   /* Initiating encryption */
   key = sodium_malloc(sizeof(unsigned char *) *
                        crypto_aead_chacha20poly1305_KEYBYTES);
   crypto_aead_chacha20poly1305_keygen(key);
   randombytes_buf(nonce, sizeof(nonce));
   key = memcpy(key, passwd, plen);

   /* Encrypting the message */
   int chacha20out =
      crypto_aead_chacha20poly1305_encrypt(ciphertext, &ciphertext_len,
                                             content, clen, NULL, 0, NULL,
                                             nonce, key);

   /* TODO:
    *  - Allows only NULL pointers as secret
    */
   *secret = (unsigned char *)strdup((const char *)ciphertext);

   /* TODO:
    *  - Write down the encrypted content;
    */
   return chacha20out;
}

int decrypt_content(const unsigned char *secret,
                     unsigned char **content, const char *passwd)
{
   /* Checking parameters */
   exit_eq(secret, NULL, -1);
   exit_eq(passwd, NULL, -1);

   /* Defining some usefull parameters */
   size_t plen = strlen(passwd);
   unsigned long long secret_len = strlen((const char *)secret);
   unsigned char cleartext[secret_len - crypto_aead_chacha20poly1305_ABYTES + 1];
   unsigned long long cleartext_len;
   /*
   unsigned char nonce[crypto_aead_chacha20poly1305_NPUBBYTES];
   unsigned char *key = sodium_malloc(sizeof(unsigned char) *
                                    crypto_aead_chacha20poly1305_KEYBYTES);
   */

   /* Initializing stuff */
   /*
   crypto_aead_chacha20poly1305_keygen(key);
   randombytes_buf(nonce, sizeof(nonce));
   key = memcpy(key, passwd, plen);
   */

   int chacha20out =
      crypto_aead_chacha20poly1305_decrypt(cleartext, &cleartext_len, NULL,
                                             secret, secret_len, NULL, 0,
                                             nonce, key);

   *content = (unsigned char *)strndup((const char *)cleartext, cleartext_len);
   return chacha20out;
}
