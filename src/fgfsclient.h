#include <stdlib.h>

void init_sockaddr(struct sockaddr_in *name, const char *hostname, unsigned port);
int fgfswrite(int sock, const char *msg, size_t msg_size);
char *fgfsread(int sock, int timeout, size_t *bytes_read);
void fgfsflush(int sock);
int fgfsconnect(const char *hostname, const int port);
