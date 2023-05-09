/* Public Domain */

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdarg.h>
#include <string.h>


#define BUFSIZE 1024
#define READSIZE 256


#include "fgfsclient.h"

int fgfswrite(int sock, const char *msg, size_t msg_size)
{
    size_t len = msg_size + 2;
    ssize_t sent = 0;
    char *buf = malloc(len);
    char *p = buf;

    memcpy(buf, msg, msg_size);
    *(buf + msg_size) = '\0';
	// fprintf(stderr, "SEND: <%s>\n", buf);
	memcpy(buf + msg_size, "\015\012", 2);

    while (len > 0) {
        // fprintf(stderr, "Sending %lu bytes\n", len);
        sent = write(sock, p, len);
        // fprintf(stderr, "Sent %li bytes\n", sent);
        if (sent < 0) {
            free(buf);
            perror("fgfswrite");
            exit(EXIT_FAILURE);
        }
        len -= sent;
        p += sent;
    }
    // fprintf(stderr, "Send complete.\n");
    free(buf);
	return 0;
}

char* find_sentinel(char* haystack, size_t haystack_size)
{
    size_t p = 0;
    for (p = 0; p < haystack_size - 1; ++p) {
        if (haystack[p] == '\015' && haystack[p+1] == '\012')
            return haystack + p;
    }
    return NULL;
}

char *fgfsread(int sock, int timeout, size_t *bytes_read)
{
	char *buf = NULL;
    size_t cap = BUFSIZE;
    char *wp = buf;

	fd_set ready;
	struct timeval tv;
	ssize_t len;
    char* sentinel;
    size_t total_read = 0;

	FD_ZERO(&ready);
	FD_SET(sock, &ready);
	tv.tv_sec = 0;
	tv.tv_usec = timeout * 1000;
	if (!select(32, &ready, 0, 0, &tv))
		return NULL;

    buf = malloc(cap);
    wp = buf;

    do {
        // fprintf(stderr, "Read so far: %lu; buffer capacity: %lu\n", total_read, cap);
        if (cap < total_read + READSIZE + 1) {
            // fprintf(stderr, "Grow buffer from %lu to %lu\n", cap, cap + BUFSIZE);
            cap += BUFSIZE;
            buf = realloc(buf, cap);
            wp = buf + total_read;
        }

        // fprintf(stderr, "Reading %i bytes\n", READSIZE);
        len = read(sock, wp, READSIZE);
        if (len < 0) {
            free(buf);
            perror("fgfsread");
            exit(EXIT_FAILURE);
        } 
        // fprintf(stderr, "Read %li bytes\n", len);
        sentinel = find_sentinel(wp, len);
        if (sentinel) {
            total_read += sentinel - wp;
            wp = sentinel;
            *wp = '\0';
            len = 0;
        }
        else {
            total_read += len;
            wp += len;
            *wp = '\0';
        }
        // fprintf(stderr, "Buffer is now: %s\n", buf);
        if (len < READSIZE) {
            if (bytes_read)
                *bytes_read = total_read;
            return buf;
        }
    } while (1);
}



void fgfsflush(int sock)
{
    size_t bytes_read = 1;
    char *p = NULL;

	while (bytes_read) {
        p = fgfsread(sock, 0, &bytes_read);
        if (p) {
            free(p);
        }
        else {
            bytes_read = 0;
        }
	}
}



int fgfsconnect(const char *hostname, const int port)
{
	struct sockaddr_in serv_addr;
	struct hostent *hostinfo;
	int sock;

	sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock < 0) {
		perror("fgfsconnect/socket");
		return -1;
	}

	hostinfo = gethostbyname(hostname);
	if (hostinfo == NULL) {
		fprintf(stderr, "fgfsconnect: unknown host: \"%s\"\n", hostname);
		close(sock);
		return -2;
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);
	serv_addr.sin_addr = *(struct in_addr *)hostinfo->h_addr;

	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
		perror("fgfsconnect/connect");
		close(sock);
		return -3;
	}
	return sock;
}
