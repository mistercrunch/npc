#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/poll.h>
#include <errno.h>

/* linked function from sabbia_serpent.c */
void SABBIA_key_init(char *);
unsigned char *encrypt_block(unsigned char *, int);
unsigned char *decrypt_block(unsigned char *, int);

/*
 * Claudio Agosti vecna@s0ftpj.org, for understand better
 * this code read www.s0ftpj.org or search under google 
 * "SABBIA vecna low anonymous network"
 *
 * Sat Jun 19 11:22:22 CEST 2004
 *
 * I must read encrypted/UDP traffic outgoing from openvpn
 *
 */

#define MAXPKTL		1500
struct packet
{
	unsigned char p[MAXPKTL];	/* since iphdr ... */
	unsigned int uid;		/* unique packet identification */
	int pkt_len;			/* ... */
	unsigned short counter;		/* incremental pkt cnt */
	unsigned short fragment_n;	/* if fragmentation happens */
};

/* queue used of keep local packet before delayed send */
#define MAXQLEN		32
static struct packet packetq[MAXQLEN];
static unsigned int qelements, pktcnt;

/* DEFault PacKet PayLOAD, in byte number */
#define DEFPKPLOAD	800	

/* this function is called any time timeout expire */
static void send_payload(int anon_fd, struct sockaddr *x)
{
	unsigned char udpack[DEFPKPLOAD], *new;
	unsigned short lessercnt =0xffff;
	int i, te =(-1), sl =sizeof(*x);

	/* find elder packet! */
	for(i =0; i < MAXQLEN; i++)
	{
		if(packetq[i].pkt_len)
		{
			if(lessercnt > packetq[i].counter)
			{
				lessercnt =packetq[i].counter;
				/* te is Target Element! */
				te =(i);
			}
		}
	}

	/* 
	 * if there are packet, payload length or packet length 
	 * should be fixed
	 */
	if(te >= 0)
	{
		/* I must split the packet */
		if(packetq[te].pkt_len > DEFPKPLOAD)
		{
			memcpy(udpack, packetq[te].p, DEFPKPLOAD);
			/* fix length for finish packet */
			packetq[te].pkt_len -=DEFPKPLOAD;
			/* move the other part of packet */
			memcpy( packetq[te].p, 
				packetq[te].p + DEFPKPLOAD,
				packetq[te].pkt_len	
			);
			/* after maybe you could finish the packet :) */
			packetq[te].fragment_n++;
		}
		/* else, I must decrement paylength */
		else
		{
			memcpy(udpack, packetq[te].p, packetq[te].pkt_len);
			/* unset packet len, because packet is read yet */
			packetq[te].pkt_len =0;
		}

		/* 
		 * encrpypting data, also if packet is not filled, appears
		 * as random, we must simple encrypt DEFPKPLOAD instead of 
		 * all real packets length.
		 *
		 * is must think better, but this "uid" as uniqe 
		 * identificator could be a vulnerability under active
		 * attacks.
		 */
		encrypt_block(udpack, DEFPKPLOAD);
	}
	else
		/* get alla random data */
		encrypt_block(udpack, DEFPKPLOAD);

	if((sendto(anon_fd, udpack, DEFPKPLOAD, 0x00, x, sl)) ==-1)
	{
		printf("unable to send packet on anonymous channel: %s!\n",
			strerror(errno)
		);
	}
}

/* return != 0x00 if checksum is valid! */
int verify_ip_checksum(struct iphdr *ip)
{
	/* this is not ip checksum check, but work :) */
	if(ip->ihl == 5 && ip->version ==4)
		return 1;
	else
		return 0;
}

/*
 * this function is called for get remote packets, decrypt (or discarge,
 * if used as payload 
 */
static void get_remote_payload(int anon_fd, struct sockaddr *anon,
			       int local_fd, struct sockaddr *local)
{
	unsigned char inpkt[DEFPKPLOAD * 2];
	unsigned int nbyte, sal =sizeof(struct sockaddr_in);
	struct iphdr *ip;

	/* FIXME */
	int i;
	/* 
	 * fragment buffer and fragment presence is two static 
	 * buffer for keep incomplete fragment received 
	 */
	static unsigned char fragment[DEFPKPLOAD];
	static unsigned short fpres;

	if((nbyte =recvfrom(anon_fd, inpkt, DEFPKPLOAD, 0x00, anon, &sal)) ==-1)
	{
		printf("unable to read from remote udp socket!: %s\n",
			strerror(errno)
		);
		exit(errno);
	}

	decrypt_block(inpkt, nbyte);

	if(fpres)
	{
		memcpy(&inpkt[DEFPKPLOAD], &inpkt[0], nbyte);
		memcpy(inpkt, fragment, DEFPKPLOAD);

		fpres =0x00;

		/* resend local packet */
		if((nbyte =sendto(local_fd, inpkt, DEFPKPLOAD + nbyte, 0x00, local, sizeof(struct sockaddr_in))) ==-1)
		{
			printf("unable to resend on local vpn connection: %s\n",
				strerror(errno)
			);
			exit(errno);
		}
	}
	else
	{
		ip =(struct iphdr *)&inpkt[0];

		/* random paylod! */
		if(verify_ip_checksum(ip))
		{
			/* packet fragmented */
			if(htons(ip->tot_len) > DEFPKPLOAD)
			{
				memcpy(fragment, &inpkt[0], DEFPKPLOAD);
				fpres =0xff;
			}
			else /* packet not fragmented */
			{
				if((sendto(local_fd, inpkt, nbyte, 0x00, local, sizeof(struct sockaddr_in)) ==-1))
				{
					printf("unable to sent complete packet con vpn: %s\n",
							strerror(errno)
					);
					exit(errno);
				}
			}
		}
	}

}

static unsigned int compute_packet_uid(unsigned char *b, int len)
{
#define MINPKTLEN	56
	struct iphdr *ip;
	struct tcphdr *tcp;

	if(len < MINPKTLEN)
		return (0x6969 ^ len);

	ip =(struct iphdr *)b;
	tcp =(struct tcphdr *)(b + (ip->ihl *4));

	return ((ip->daddr * tcp->dest) ^ tcp->seq);
}

/* 
 * this function is used whenever a local packer is intercept from
 * SABBIA code and delayed, (enqueued) 
 */
static void queue_packet(int local_fd, struct sockaddr *x)
{
	int i, len, slen, *dlen =NULL;
	unsigned char looser_buffer[MAXPKTL], *dst;
	unsigned int *uidp, looser_uid;

	/*
	 * search on the queue buffer the first empty location,
	 * save pointer for use below 
	 */
	for(i =0; i < MAXQLEN; i++)
	{
		if(!packetq[i].pkt_len)
		{
			/* fill destination pointer */
			dst =&(packetq[i].p[0]);
			uidp =&(packetq[i].uid);
			dlen =&packetq[i].pkt_len;

			/* set know field */
			packetq[i].counter =pktcnt++;
			packetq[i].fragment_n =0;
			break;
		}
	}

	/*
	 * if queue is full, print error and fix pointer for loose 
	 * the packets after get him from kernel 
	 */
	if(dlen ==NULL || qelements == (MAXQLEN -1))
	{
		printf("too much small queue (MAXQLEN %d), packet lost!\n",
			MAXQLEN
		);
		dst =looser_buffer;
		dlen =&len;
		uidp =&looser_uid;
	}

	/*
	 * get packets on the correct location, this code implementation,
	 * could appear lazy and forced to be clean, but is the unique
	 * solution for avoid passive timing analisys:
	 *
	 * build software making the some operation on any event, to
	 * avoid event discrimination after execution time study.
	 */
	if((*dlen =recvfrom(local_fd, dst, MAXPKTL, 0, x, &slen)) ==-1)
	{
		/* 
		 * happens when the packet from the VPN is read from
		 * this file descriptor! 
		 *
		 * is a bug on the notification of select(2), to avoid
		 * (other than resolve bug :) I could use tcp on 
		 * local connection with openvpn, but not all release
		 * support tcp-client and tcp-server options :\
		 */
		if(errno ==EINVAL)
		{
			*dlen =0x00;
		}
		else
		{
			printf("recvfrom via local udp file descriptor! %s\n",
				strerror(errno)
			);
			exit(errno);
		}
	}

	*uidp =compute_packet_uid(dst, *dlen);
}

int main(int argc, char **argv)
{
	struct sockaddr_in local, anon, out, local_in;
	int bind_fd, local_fd, anon_fd, out_fd, ret, lin =sizeof(local_in);
	unsigned int remoteaddr;
	unsigned short lvp, ps, sendport, recvport;

	if(argc != 6)
	{
		printf( "%s local_vpn_port packet_second anonymous_peer sendport recvport\n\n"
			"you simple don't must start this software manually, "
			"use the shell script\nSABBIATEST.sh, is it "
			"to start openvpn and reading exaclty options\n", *argv 
		);
		exit(EXIT_FAILURE);
	}
	else
	{
		/* local vpn port, random used for direct vpn traffic locally */
		lvp =htons(atoi(argv[1]));
		/* packet seconds, how much do you wanna flood ? */
		ps =atoi(argv[2]);
		/* remote address, resolved in SABBIATEST.sh */
		remoteaddr =inet_addr(argv[3]);
		/* sabbia port */
		sendport =htons(atoi(argv[4]));
		recvport =htons(atoi(argv[5]));
	}

	/* 
	 * must exist a file with the name of ip address, for find 
	 * SEX!
	 * ops, for find the key related with the remote peer. the
	 * key must be pre-shared, openssl under this anonymous 
	 * tunnel could run on plain tunnelling also, this encryption
	 * allow a key more bigger. simple you need a shared key,
	 * also a /dev/urandom dump could be used, this key is NOT keep 
	 * on memory, try to use the Lord of the Ring DivX as key :)
	 */

	SABBIA_key_init(argv[3]);
#if 0
	/* set up socket for receive connection from openvpn! */
	bind_fd=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	local.sin_addr.s_addr =inet_addr("127.0.0.1");
	local.sin_family =AF_INET;
	local.sin_port =lvp;
	/* bind and wait openvpn connection! */
	bind(bind_fd, (struct sockaddr *)&local, sizeof(local));
	listen(bind_fd, 1);
	local_fd =accept(bind_fd, (struct sockaddr *)&local_in, &lin);

	if(local_fd ==-1)
	{
		printf("unable to get connection from VPN: %s\n",
			strerror(errno)
		);
		exit(errno);
	}
#else
	local_fd =socket(AF_INET, SOCK_DGRAM, 0);
	local.sin_addr.s_addr =inet_addr("127.0.0.1");
	local.sin_family =AF_INET;
	local.sin_port =lvp;
	bind(local_fd, (struct sockaddr *)&local, sizeof(local));
#endif

	/* open your anonymous socket */
	anon_fd =socket(AF_INET, SOCK_DGRAM, 0);
	anon.sin_port =recvport;
	anon.sin_addr.s_addr =INADDR_ANY;
	anon.sin_family =AF_INET;
	bind(anon_fd, (struct sockaddr *)&anon, sizeof(anon));

	/* open socket for send packet anonymously (anonymously ??) */
	out_fd =socket(AF_INET, SOCK_DGRAM, 0);
	out.sin_port =sendport;
	out.sin_addr.s_addr =remoteaddr;
	out.sin_family =AF_INET;

	while(1)
	{
		/* static because sometime I prefer use static */
		static struct timeval delay;
		static fd_set *readf;
		
		if(!delay.tv_usec)
			delay.tv_usec =(1000000 / ps);

		if(readf ==NULL)
			readf =calloc(2, sizeof(fd_set));
		else
			FD_ZERO(readf);

		FD_SET(local_fd, readf);
		FD_SET(anon_fd, readf);

		/* out_fd is anon_fd + 1 */
		ret =select(out_fd, readf, NULL, NULL, &delay);

		switch(ret)
		{
			case 0:
				/* timeout expired! */
				send_payload(out_fd, (struct sockaddr *)&out);
				break;
			case -1:
				printf("select error ?? %s\n", strerror(errno));
				exit(0);
			default:
				if(FD_ISSET(local_fd, readf))
					queue_packet(local_fd, (struct sockaddr *)&local);
				else if(FD_ISSET(anon_fd, readf))
					get_remote_payload(anon_fd, (struct sockaddr *)&anon, local_fd, (struct sockaddr *)&local);

				break;
		}
	}
}
