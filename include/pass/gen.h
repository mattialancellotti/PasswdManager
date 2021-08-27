#ifndef GEN_H
#define GEN_H

/* TODO: doc */
char **passwd_generator(service_t* /*passwd_conf*/);

/*
 * This function creates a password of length `length`. Also  it accepts a
 * `flags` that indicates which of { 'u_char', 'l_char' ... } are not accepted
 */
char *create_passwd(const size_t /*length*/, const int /*flags*/);

#endif
