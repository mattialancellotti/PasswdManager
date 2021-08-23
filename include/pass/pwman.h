#ifndef PW_H
#define PW_H

/* TODO:
 *  - doc;
 *  - name;
 */
int pm_init(const char* /*hash_file*/);
char *pm_hash(const char* /*hash_file*/);

int pm_init_path(void);

#endif
