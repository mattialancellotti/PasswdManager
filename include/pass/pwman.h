#ifndef PW_H
#define PW_H

/* TODO:
 *  - doc;
 */
int pm_init_hash(const char* /*hash_file*/);

char *pm_hash(const char* /*hash_file*/);

int pm_init_path(void);

int pm_purge_db(const char* /*db*/);

int pm_create_service(const char* /*service_name*/);

#endif
