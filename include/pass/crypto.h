#ifndef CRYPTO_H
#define CRYPTO_H

char *hash_password(const char* /*password*/);
int check_hash(const char* /*hash*/, const char* /*passwd*/);

#endif
