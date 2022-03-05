#ifndef CRYPTO_H
#define CRYPTO_H

/* TODO:
 *  - doc
 */
char *hash_password(const char* /*password*/);
int hash_check(const char* /*hash*/, const char* /*passwd*/);

unsigned char *encrypt_password(const char* /*auth*/, const char* /*passwd*/, unsigned long long* /*bytes*/);
unsigned char *decrypt_password(const unsigned char* /*encrypted_password*/, const char* /*password*/, unsigned long long /*bytes*/);

#endif
