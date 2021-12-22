#ifndef CRYPTO_H
#define CRYPTO_H

/* TODO:
 *  - doc
 */
char *hash_password(const char* /*password*/);
int hash_check(const char* /*hash*/, const char* /*passwd*/);

int init_crypt(void);
char *encryption_key_generator(const char* /*passwd*/);

#endif
