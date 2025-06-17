#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>

fd_set reading, writing, currentfds;
int max = 0;
int counter = 0;
int ids[70000];
char *messages[70000];
char chat[1001];
char buffer[100];


int extract_message(char **buf, char **msg)
{
	char	*newbuf;
	int	i;

	*msg = 0;
	if (*buf == 0)
		return (0);
	i = 0;
	while ((*buf)[i])
	{
		if ((*buf)[i] == '\n')
		{
			newbuf = calloc(1, sizeof(*newbuf) * (strlen(*buf + i + 1) + 1));
			if (newbuf == 0)
				return (-1);
			strcpy(newbuf, *buf + i + 1);
			*msg = *buf;
			(*msg)[i + 1] = 0;
			*buf = newbuf;
			return (1);
		}
		i++;
	}
	return (0);
}

char *str_join(char *buf, char *add)
{
	char	*newbuf;
	int		len;

	if (buf == 0)
		len = 0;
	else
		len = strlen(buf);
	newbuf = malloc(sizeof(*newbuf) * (len + strlen(add) + 1));
	if (newbuf == 0)
		return (0);
	newbuf[0] = 0;
	if (buf != 0)
		strcat(newbuf, buf);
	free(buf);
	strcat(newbuf, add);
	return (newbuf);
}

int main(int argc, char **argv)
{
    if (argc != 2)
        exit (1);
    int sockfd, connfd;
    socklen_t len;
	struct sockaddr_in servaddr; 

    (void) connfd;
    (void) len;
	// socket create and verification 
	sockfd = socket(AF_INET, SOCK_STREAM, 0); 
	if (sockfd == -1) { 
		printf("socket creation failed...\n"); 
		exit(0); 
	} 
	else
		printf("Socket successfully created..\n"); 
	bzero(&servaddr, sizeof(servaddr)); 

	// assign IP, PORT 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = htonl(2130706433); //127.0.0.1
	servaddr.sin_port = htons(atoi(argv[1])); 

    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	// Binding newly created socket to given IP and verification 
	if ((bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr))) != 0) { 
		printf("socket bind failed...\n"); 
		exit(0); 
	} 
	else
		printf("Socket successfully binded..\n");
	if (listen(sockfd, SOMAXCONN) != 0) {
		printf("cannot listen\n"); 
		exit(0); 
	}
    FD_ZERO(&currentfds);
    FD_SET(sockfd, &currentfds);
    max = sockfd;

    while(1)
    {
        reading = writing = currentfds;
        select(max + 1, &reading, &writing, NULL, NULL);
        // printf("worked4\n");
        for (int fd = 0; fd <= max; fd++)
        {
            // printf("worked3\n");
            if (FD_ISSET(fd, &reading))
            {
                // printf("worked1\n");
                if (fd == sockfd)
                {
                    // new connection
                    
                    struct sockaddr_in cli; 
                    len = sizeof(cli);
                    connfd = accept(sockfd, (struct sockaddr *)&cli, &len);
                    if (connfd < 0) { 
                        printf("server acccept failed...\n"); 
                        exit(0); 
                    } 
                    else
                        printf("server acccept the client...\n");

                    FD_SET(connfd, &currentfds);
                    ids[connfd] = counter;
                    counter++;
                    if (connfd > max)
                        max = connfd;
                    sprintf(buffer, "server: client %d just arrived\n%c", ids[connfd], '\0');

                    for (int i=0; i <= max; i++)
                    {
                        if (FD_ISSET(i, &writing) && i != fd)
                            send(i, buffer, sizeof(buffer), 0); 
                    }
                    // printf("worked2\n");
                }
                else
                {
                    // exit (close fd)

                    int length = recv(fd, chat, 1000, 0);
                    if (length <= 0)
                    {
                        sprintf(chat, "server: client %d just left\n%c", ids[fd], '\0');
                        for (int j=0; j <= max; j++)
                        {
                            if (FD_ISSET(j, &writing) && j != fd)
                                send(j, chat, sizeof(chat), 0); 
                        }
                        printf("YES\n");
                        FD_CLR(fd, &currentfds);
                        close(fd);
                    }
                    else
                    {
                        // new message

                    }

                }
            }
        }
    }

}