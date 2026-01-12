/*
 *
 *			    SHOW PROCCESSES  v1.2
 *					by free_hunter//x25zine
 *
 *      direct /dev/kmem process viewer, for linux kernels, intel x86.
 *      find all proccesses exist in system. fucked the ALL rootkits.
 *
 *      worked on linux 2.2.18 , needed a bit change for other.
 *   		    
 */

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define ulong   unsigned long
#define uint    unsigned int
#define ushort  unsigned short
#define uchar   unsigned char

//4 debug, change SWAPPER_OFF, cat /boot/System.map | grep " init_task_union"

#define SWAPPER_OFF   0xc0274000

// process offests , change it for u system from a dump
// if not worked, use sched.h for recognize.

#define UID_OFF    0x106
#define EUID_OFF   0x108
#define PID_OFF    0x60
#define NAME_OFF   0x1c2
#define NEXT_OFF   0x38
#define PREV_OFF   0x3c


// process limit pattern, change it for u system from a dump
// if not worked

char pattern[]=
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xff\xfe\xff\xff"
"\x00\x00\x00\x00\xff\xff\xff\xff\x00\x00\x00\x00\x00\x00\x00\x00"
"\xff\xff\xff\x7f\xff\xff\xff\x7f\xff\xff\xff\x7f\xff\xff\xff\x7f"
"\xff\xff\xff\x7f\xff\xff\xff\x7f\x00\x00\x80\x00\xff\xff\xff\x7f"
"\x00\x00\x00\x00\xff\xff\xff\x7f\xff\xff\xff\x7f\xff\xff\xff\x7f"
"\xd0\x07\x00\x00\xd0\x07\x00\x00\x00\x04\x00\x00\x00\x04\x00\x00"
"\xff\xff\xff\x7f\xff\xff\xff\x7f\xff\xff\xff\x7f\xff\xff\xff\x7f";



void * memmem(char *s1, int l1, char *s2, int l2)
{
        if (!l2) return s1;
        while (l1 >= l2) {
                l1--;
                if (!memcmp(s1,s2,l2))
                        return s1;
                s1++;
        }
        return NULL;
}

int wkm(int fd, int offset, void *buf, int size)
{
        if (lseek(fd, offset, 0) != offset) return 0;
        if (write(fd, buf, size) != size) return 0;
        return size;
}

int wkmi(int fd, int offset, ulong buf)
{
        return wkm(fd, offset, &buf, 2);
}


ulong   loc_rkm(int fd, void *buf, uint off, uint size)
{
        if (lseek(fd, off, 0) != off) return 0;
        if (read(fd, buf, size) != size) return 0;
        return size;
}

int 	rkm(int fd, int offset, void *buf, int size)
{
        if (lseek(fd, offset, 0) != offset) return 0;
        if (read(fd, buf, size) != size) return 0;
        return size;
}

ulong	rkml(int fd, int offset, ulong *buf)
{
        return rkm(fd, offset, buf, sizeof(ulong));
}

ulong	rkmi(int fd, int offset, ulong *buf)
{
        return rkm(fd, offset, buf, 2);
}

int main(int argc, char *argv[])
{
        uint            i;
        uchar           buf[0x10010],*p,*p1;
        int             kmem,fd;
        ulong           first,nr,tmp,next,swaper,pid,uidarg,pidarg,uid;
        ulong           pgoff=0xc0000000;
	char 		name[16];


	printf("	/dev/kmem process viewer: %s\n",argv[0]);
	printf("      	    4 change process uid: %s pid uid\n",argv[0]);
	printf("		      4 debuging: %s dump\n",argv[0]);


        kmem = open("/dev/kmem", O_RDWR, 0);
        if (kmem < 0) {printf("err in read/write kmem\n");exit(1);};
	
        if(argv[1]!=NULL){strncpy(name,argv[1],4);};
	if(strncmp(name,"dump",4)==0){
		printf("dumping swapper process..\n");
                fd = open("dump.txt", O_WRONLY | O_CREAT, 0);
                if (fd < 0) {printf("err create dump.txt file\n");exit(1);};
                if (!loc_rkm(kmem, buf, SWAPPER_OFF, sizeof(buf))){printf("err in read buf0\n");exit(1);}
	        write(fd,buf,4096);
        	close(fd);
		printf("OK, check dump.txt file\n");
		exit(1);
	};

	
        if(argv[1]){
	    if(argv[2]){
		pidarg=atoi(argv[1]);
		uidarg=atoi(argv[2]);
	    }else{exit(1);};
	};

        for (i = (pgoff + 0x100000); i < (pgoff + 0x1000000);i += 0x10000){
            if (!loc_rkm(kmem, buf, i, sizeof(buf))){printf("err in read buf1\n");exit(1);}
            p = (char*)memmem(buf,sizeof(buf),pattern,7*16);
	    tmp=p-buf;
            if (p) {
		if (!rkml(kmem,i+tmp-0x150+NEXT_OFF,&next)){printf("err in read next_task\n");exit(1);}
		first=i+tmp-0x150;
		break;
	    };
	};

        if (!loc_rkm(kmem, buf,first+NAME_OFF, sizeof(buf))){printf("err in read buf2\n");exit(1);}
        strncpy(name,buf,7);	    

	if(strncmp(name,"swapper",7)==0){printf("OK, swapper process found\n");}
	else{printf("FAILED, swapper process not found... change pattern\n");exit(1);};

        if (!rkml(kmem,next+PREV_OFF,&tmp)){printf("err in read prevdesc\n");exit(1);}
	if (tmp!=first){printf("next task pointer wrong. check offsets\n");exit(1);};

	i=0; printf("SWAPPER: %d:%x\n",i,first);i++;
        strncpy(name,"init",16);pid=1;uid=0;
	do{
	    printf("TASK: %.3d %x Pid: %.5d Uid: %.4d Name: %s\n",i,next,pid,uid,name);
    	    if (!rkml(kmem,next+NEXT_OFF,&tmp)){printf("err in read nextdesc\n");exit(1);}
	    next=tmp; i++;
            if (!loc_rkm(kmem, buf,tmp+NAME_OFF, sizeof(buf))){printf("err in read buf\n");exit(1);}
	    strncpy(name,buf,16);	    
    	    if (!rkml(kmem, tmp+PID_OFF, &pid)){printf("err in read pid\n");exit(1);}
	    if ((pidarg==pid) && (argc==3)) {
		    printf("Found pid %d, ",pidarg);
	            if (!wkmi(kmem,tmp+UID_OFF,uidarg)){printf("err in write uid\n");exit(1);}
	            if (!wkmi(kmem,tmp+EUID_OFF,uidarg)){printf("err in write uid\n");exit(1);}
		    printf("uid changed to %d\n",uidarg);
	    };
    	    if (!rkmi(kmem,tmp+UID_OFF,&uid)){printf("err in read uid\n");exit(1);}
	}while(first!=next);

        close(kmem);
        return 0;
}
