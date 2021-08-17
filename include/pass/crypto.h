#ifndef CRYPTO_H
#define CRYPTO_H

/* TODO:
 *  - doc
 */
char *hash_password(const char* /*password*/);
int hash_check(const char* /*hash*/, const char* /*passwd*/);

/* TODO:
 *  - Better parameter names;
 */
int encrypt_content(const unsigned char* /*content*/,
                     unsigned char** /*secret*/, const char* /*passwd*/);
int decrypt_content(const unsigned char* /*secret*/,
                     unsigned char** /*content*/, const char* /*passwd*/);

#endif
