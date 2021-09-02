#ifndef SERVICES_H
#define SERVICES_H

/* TODO: doc */
int create_service(const char* /*service_name*/);

int delete_service(const char* /*service_name*/);

int append_service(const char* /*service_name*/, const char* /*content*/);

int expose_service(const char* /*service_name*/);

#endif
