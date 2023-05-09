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
#include <readline/readline.h>
#include <readline/history.h>

#include "fgfsclient.h"

#define DEFHOST	"localhost"
#define DEFPORT	5501

int main(int argc, char **argv)
{
	int sock;
	unsigned port;
	const char *hostname;

    using_history();

	hostname = argc > 1 ? argv[1] : DEFHOST;
	port = argc > 2 ? atoi(argv[2]) : DEFPORT;

	sock = fgfsconnect(hostname, port);
	if (sock < 0)
		return EXIT_FAILURE;

    // Turn off prompt
    fgfswrite(sock, "data", strlen("data"));

    while (1) {
        char *l = readline("nasal> ");

        if (!strlen(l))
            continue;

        if (!l || strcmp(l, "quit") == 0) {
            free(l);
            fgfswrite(sock, "quit", strlen("quit"));
            fgfsflush(sock);
            close(sock);
            return EXIT_SUCCESS;
        }

        fgfswrite(sock, "nasal", strlen("nasal"));
        fgfswrite(sock, l, strlen(l));
        fgfswrite(sock, "##EOF##", strlen("##EOF##"));

        add_history(l);
        free(l);

        char *p = fgfsread(sock, 500, NULL);
        if (p != NULL) {
            printf("%s\n", p);
        }
        else {
            // fprintf(stderr, "<<NO READ>>\n");
            printf("\n");
        }
        free(p);
    }
}
