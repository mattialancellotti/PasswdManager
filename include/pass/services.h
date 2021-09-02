#ifndef SERVICES_H
#define SERVICES_H

/*
 * This function takes a @service_name and creates it. If the service exists
 * then the user is asked whether he wants to overwrite it or not.
 *
 * @return A status flag that is 0 on success, 1 if the user refused to create
 *         the service and -1 if an error occurred.
 */
int create_service(const char* /*service_name*/);

/*
 * This function accepts a @service_name that needs to be deleted.
 * It will inform the user if the specified service doesn't exists.
 *
 * @return A status flag that is 0 on success or -1 if something happened.
 */
int delete_service(const char* /*service_name*/);

/*
 * This functions takes a buffer (as @content) that has to be writted down to
 * the service (as @service_name). It will also check whether the specified service
 * exists or not. If the service doesn't exists it can be created and if the
 * user refuses to the function exits.
 *
 * @return A status flag that is 0 on success, -1 if something bad happened and
 *         1 if the user refused to create the service.
 */
int append_service(const char* /*service_name*/, const char* /*content*/);

/*
 * This function is used to open and read a service (as @service_name) is content.
 * It uses the mopen function from src/os.c to open and read the file.
 *
 * TODO: return a duplicate of the password.
 * @return A status flag that is 0 on success and -1 otherwise.
 */
int expose_service(const char* /*service_name*/);

#endif
