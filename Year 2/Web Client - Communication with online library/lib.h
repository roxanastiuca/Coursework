#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <ctype.h>
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>

#include "helpers.h"
#include "requests.h"

#define HOST_PORT 8080
#define HOST_IP "3.8.116.10"
#define HOST_NAME "ec2-3-8-116-10.eu-west-2.compute.amazonaws.com"

#define MAX_COMMAND 20
#define MAX_PROMPT 100
#define MAX_COOKIES 10
#define COOKIE_OFFSET 12

#define CONNECTION_LOST 1

/* parser.c */
void remove_whitespace(char *str);
char *extract_cookie(char *message);
char *extract_payload(char *message);
char *extract_multi_payload(char *message);
int extract_status_code(char *message);
int extract_payload_int_field(char *message, char *field);
char *extract_payload_field(char *message, char *field);