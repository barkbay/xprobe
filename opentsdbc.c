/*
 ============================================================================
 Name        : opentsdb.c
 Author      : Michael Morello
 Version     :
 Copyright   : (c) Michael Morello <michael.morello@gmail.com>
 Description : Client basique OpenTSDB en C
 ============================================================================
 */

#include <stdio.h> 
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h> 
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <errno.h>
#include <sys/types.h>
#include <netdb.h>

#define PORT 4242
#define TS_LENGTH 10
#define RSP_MAX_LEN 2048

unsigned short OpenTSDBPort = PORT;
char * serveurs[] = { "55.11.9.196" };
char srvrsp[RSP_MAX_LEN];
unsigned short int currentServeur = -1;
int sock = -1; /* Socket de connexion au serveur OpenTSDB */

/*
 * Donne le nom du prochain serveur OpenTSDB
 */
char * next() {
	int nbrServeurs = sizeof(serveurs) / sizeof(char *);
	currentServeur = (currentServeur + 1) % nbrServeurs;
	return serveurs[currentServeur];
}

/*
 * Tente de fermer le socket courant
 */
void try2close() {
	printf("Try to close socket....");
	if (sock < 0)
		return;
	int s = shutdown(sock, 2);
	switch (s) {
	case EBADF:
		printf("not a valid file descriptor\n");
		break;
	case ENOTSOCK:
		printf("it is not a socket !\n");
		break;
	case ENOTCONN:
		printf("not connected\n");
		break;
	default:
		break;
	}
	sock = -1;
}

/*
 * Tente une connexion vers un des serveur OpenTSDB
 */
int try2connect() {
	struct sockaddr_in sockAddr;
	int tempSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (tempSock > 0)
		sock = tempSock;
	else
		return tempSock;
	char * nextSrv = next();
	printf("Attempt to connect to server %s\n", nextSrv);
	memset(&sockAddr, 0, sizeof(sockAddr));
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_addr.s_addr = inet_addr(nextSrv);
	sockAddr.sin_port = htons(OpenTSDBPort);
	int cnt = connect(sock, (struct sockaddr *) &sockAddr, sizeof(sockAddr));
	if (cnt < 0) {
		printf("connect() failed with return code %d\n", cnt);
		try2close();
		return cnt;
	} else {
		printf("connected !\n");
		return sock;
	}
}

char *ltrim(char *s)
{
    while(isspace(*s)) s++;
    return s;
}

char *rtrim(char *s)
{
    char* back = s + strlen(s);
    while(isspace(*--back));
    *(back+1) = '\0';
    return s;
}

char *trim(char *s)
{
    return rtrim(ltrim(s));
}

int try2sendCollectedData(int now, char * metric, char * data, char * tag) {
	unsigned int sentL; /* Taille de la chaine envoye */

	while (sock < 0) {
		if (try2connect() < 0) {
			sleep(5);
		}
	}

	char * tdata = trim(data);

	int outL = 4 + strlen(metric) + 1 + TS_LENGTH + 1 + strlen(tdata) + 1
			+ strlen(tag) + strlen("\n");
	char * out = malloc(outL + 8);
	sprintf(out, "put %s %d %s %s\n", metric, now, tdata, tag);

	sentL = send(sock, out, outL, 0);
	printf("[send][S=%d][N=%d]%s", outL, sentL, out);


	free(out);
	return sentL;
}

void sendCollectedData(int now, char * metric, char * data, char * tag) {
	while (try2sendCollectedData(now, metric, data, tag) < 0) {
		try2close(sock);
	}
	/*int srvread = readServerResponse(srvrsp);
	if (srvread > 0) {
		printf("srv response :%s\n", srvrsp);
	} else if (srvread == 0) {
		printf("srv response is EMPTY\n");
	} else {
		printf("error on server read : %d\n", srvread);
		try2close(sock);
	}*/
}

/* retourne 1 si valeur mise a jour, 0 sinon */
int sendCollectedData2(int now, char * metric, char * data, char * tag, char * old_data, int lastupdate) {

	char * tdata = trim(data);
	char * told_data =  trim(old_data);


	if ( strcmp (tdata, told_data) == 0 & (lastupdate + 60) > now ) {
		/* duplicate value */
		printf("[skip]%s\n", metric);
		return 0;
	} else {
		sendCollectedData(now, metric, tdata, tag);
		return 1;
	}

}


ssize_t readServerResponse(char * rsp) {
	ssize_t n, rc;
	char c;

	for (n = 1; n < RSP_MAX_LEN; n++) {
		if ((rc = read(sock, &c, 1)) == 1) {
			*rsp++ = c;
			if (c == '\n')
				break;
		} else if (rc == 0) {
			if (n == 1)
				return 0;
			else
				break;
		} else {
			if (errno == EINTR)
				continue;
			return -1;
		}
	}
	*rsp = 0;
	return n;
}

int getHostname(char * fqdn) {

	struct addrinfo hints, *info, *p;
	int gai_result;
	char hostname[1024];
	hostname[1023] = '\0';
	gethostname(hostname, 1023);
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC; /*either IPV4 or IPV6*/
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_CANONNAME;

	if ((gai_result = getaddrinfo(hostname, "http", &hints, &info)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(gai_result));
		strcpy(fqdn, hostname);
	} else {
		for (p = info; p != NULL ; p = p->ai_next) {
			if (p->ai_canonname != NULL ) {
				strcpy(fqdn, p->ai_canonname);
				continue;
			}
		}
		freeaddrinfo(info);
		if (fqdn == NULL ) {
			return 1;
		}
	}
	return 0;
}
