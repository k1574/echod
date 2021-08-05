#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>

#include <netinet/in.h>

#include <arpa/inet.h>

#include <sl/arg.h>

enum{
	BufSiz = 64 ,
	BackLog = 10,
};

char *argv0 ;

void
usage(void)
{
	fprintf(stderr, "usage: %s [-h hostname] [-p port]\n", argv0);
	exit(1);
}

int
main(int argc, char *argv[])
{
	int lsock, csock, n;
	char buf[BufSiz];
	struct sockaddr_in addr ;

	argv0 = argv[0] ;

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET ;
	addr.sin_addr.s_addr = htonl(INADDR_ANY) ;
	addr.sin_port = htons(7) ;

	ARGBEGIN {
	case 'p' :
		addr.sin_port = htons(atoi(EARGF(usage()))) ;
		break;
	case 'h' :
		addr.sin_addr.s_addr = htonl(atoi(EARGF(usage()))) ;
		break;
	default:
		usage();
	} ARGEND ;

	lsock = socket(AF_INET, SOCK_STREAM, 0) ;
	if(bind(lsock, (struct sockaddr *)&addr, sizeof(addr))){
		perror("bind");
		exit(1);
	}
	listen(lsock, 10);

	while(1){
		csock = accept(lsock, 0, 0) ;
		if(!fork()){
			do{
				n = recv(csock, buf, sizeof(buf), 0) ;	
				send(csock, buf, n, 0);
			}while(n==sizeof(buf));
			close(csock);
			goto thread_exit ;
		}
	}

thread_exit:
	return 0 ;
}

