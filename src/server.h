#ifndef SERVER_H
#define SERVER_H

#include <stdbool.h>

bool tiwut_server_start(int port, const char *host);
void tiwut_server_stop(void);

#endif
