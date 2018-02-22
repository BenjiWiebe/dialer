//This program handles the tel: link for Firefox
//It gets the tel: link passed as its first argument.

//Uncomment the below line and put in the hostname or IP address of the machine running dialer_server.rb
//#define HOSTNAME "servernamehere"
#ifndef HOSTNAME
#  error "Please define HOSTNAME"
#endif
#define PORTNO	2893
#define _PORTNO	"2893"

void warning(char*, ...);

#ifndef __linux__
#  include "dialer_windows.h"
#else
#  include <stdio.h>
#  include <string.h>
#  include <stdlib.h>
#  include <ctype.h>
#  include <sys/types.h>
#  include <unistd.h>
#  include <netdb.h>
#  include <sys/socket.h>
#  include <arpa/inet.h>
#endif


#ifdef __linux__
#  define warning printf
#else
#  include <stdarg.h>
void warning(char *fmtstr, ...)
{
	va_list l;
	va_start(l, fmtstr);
	char buf[512];
	vsnprintf(buf, 512, fmtstr, l);
	va_end(l);
	MessageBox(NULL, buf, "Dialer warning", MB_OK|MB_ICONWARNING);
}
#endif

void connfailed(char *reason)
{
	warning("Connection to dial server failed.\n");
	warning(reason);
	exit(1);
}

int main(int argc, char *argv[])
{
	if(argc != 2)
	{
		warning("Usage: %s <tel: link or phone number>\n", argv[0]);
		return 1;
	}

	char *arg = argv[1];
	if(strncmp("tel:", arg, 4))
	{
		warning("Malformed link.\n");
		return 1;
	}
	arg += 4; // Skip past tel: part
	
	char *_t = malloc((strlen(arg) + 1 + 1) * sizeof(char));
	char *t = _t + 1;
	if(t == NULL)
	{
		warning("malloc() failed.\n");
		return 1;
	}

	int i, j = 0;
	for(i = 0; arg[i] != 0; i++)
	{
		if(isdigit(arg[i]))
		{
			t[j++] = arg[i];
		}
		else
		{
			switch(arg[i])
			{
				case '+':
				case '/':
				case '(':
				case ')':
				case ' ':
				case '-':
					continue;
				default:
					warning("Malformed telephone number.\n");
					return 1;
			}
		}
	}
	t[j] = 0;
	
	if(strlen(t) <= 10)
	{
		t--;
		t[0] = '1';
	}

//	FILE *fp = fopen("/tmp/dialer.txt", "a");
//	if(fp == NULL)
//	{
//		warning("Opening file failed.\n");
//		return 1;
//	}
//	fprintf(fp, "Dial %s\n", t);
//	fclose(fp);

#ifndef __linux__
	send_data(t);
#else
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock < 0)
		connfailed("socket() failed");

	struct sockaddr_in servaddr;
	struct hostent *server;
	server = gethostbyname(HOSTNAME);
	if(server == NULL)
		connfailed("could not find " HOSTNAME);

	servaddr.sin_family = AF_INET;
	memcpy(&servaddr.sin_addr.s_addr, server->h_addr, server->h_length);
	servaddr.sin_port = htons(PORTNO);
	if(connect(sock, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
		connfailed("connection failed");

	if(send(sock, t, strlen(t), 0) < 0)
		connfailed("could not send data");

	close(sock);
#endif
	free(_t);
	return 0;
}
