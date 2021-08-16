#ifndef CRYPTO_H
#define CRYPTO_H

char *hash_password(const char* /*password*/);
int hash_check(const char* /*hash*/, const char* /*passwd*/);

#endif
