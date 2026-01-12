/*
 * Name: metadata.c
 * Date: Wed Aug 11 17:27:28 CEST 2004
 * Author: fusys [fusys@s0ftpj.org, fusys@sikurezza.org]
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * If you feel generous during MOCA (http://moca.olografix.org/), and MOCA
 * only, then you can also comply to the MOJITO-WARE LICENSE :-)
 *
 * MOJITO-WARE LICENSE: This source code is like "THE BEER-WARE LICENSE" by
 * Poul-Henning Kamp <phk@FreeBSD.ORG> but you can give me in return a mojito.
 *
 * Tested on: Slackware 10 Linux 2.6.7 #6 i686 GNU/Linux
 *
 * Snippets of code modified from e2fsprogs-1.35 (e2fsprogs.sourceforge.net)
 *
 */

#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <ext2fs/ext2fs.h>
#include <asm/page.h>

#ifndef _IO
#define BLKGETSIZE 0x1260
#else
#define BLKGETSIZE _IO(0x12,96)
#endif

#define URANDOM		"/dev/urandom"

union swap_header {
        struct {
                char reserved[PAGE_SIZE - 10];
                char magic[10];                 /* SWAP-SPACE or SWAPSPACE2 */
        } magic;
        struct {
                char         bootbits[1024];    /* Space for disklabel etc. */
                unsigned int version;
                unsigned int last_page;
                unsigned int nr_badpages;
                unsigned int padding[125];
                unsigned int badpages[1];
        } info;
};

#define __swapoffset(x) ((unsigned long)&((union swap_header *)0)->x)
#define MAX_SWAP_BADPAGES \
        ((__swapoffset(magic.magic) - __swapoffset(info.badpages)) / sizeof(int))

/* questa struct e' derivata dall union definita in linux/swap.h */

struct swap_sig {
	unsigned int version;
        unsigned int last_page;
        unsigned int nr_badpages;
        unsigned int padding[125];
        unsigned int badpages[1];
} *sig;

static int pagesize;
static int k_pagesize;
static long *signature;
static unsigned long PAGES = 0;

void aborto(char *uff) {

	perror(uff);
	unlink("metadata.h");
	exit(0);
}

/* valid_offset() e find_size() provengono da mkswap.c
   ricorda: non reinventare la ruota
   Rule of Simplicity
   Rule of Reusability					*/
   
int valid_offset (FILE *fd, off_t offset) {
	char ch;

	if (fseek (fd, offset, 0) < 0)
		return 0;
	if (fread ((void *)&ch, sizeof(char), 1, fd) < 1)
		return 0;
	return 1;
}

static off_t find_size (FILE *fd) {
	off_t high, low;

	low = 0;
	for (high = 1; high > 0 && valid_offset (fd, high); high *= 2)
		low = high;
	while (low < high - 1) {
		const off_t mid = (low + high) / 2;

		if (valid_offset (fd, mid))
			low = mid;
		else
			high = mid;
	}
	return (low + 1);
}

int main(int argc, char **argv)
{
	char *devname, *swapdev;	// device per le open()
    FILE *fdef, *sd;		// fd per metadata.h e lo swap
    int sd2;			// secondo fd per lo swapfile (ro)
    int urandom;		// fd per /dev/urandom
    int buff, i = 0, n = 0;		// conteggio e variabili per iterazioni
    ext2_filsys fs;			// cuore delle ext2fs_*
    struct ext2_super_block *sb;	// struttura del superblock della partizione
    //struct ext2_inode inode;	// struttura dell'inode nel filesystem ext2
    blk_t old, new, super;		// al servizio di ext2fs_super_and_bgd_loc
    int has_super;			// per segnare tutti i superblock, anche di backup
    int errno;			// per perror()
	int inoctl_n, modulo;		// numero massimo di inode di controllo
	unsigned long size;		// relativo al numero di pagine disponibili
	char version[10];		// v0 o v1
	unsigned char xorkey[16];
	unsigned char magic[8];
	
	
    if(getuid() || geteuid()) {
    	fprintf(stderr, "\nDevi essere root!\n\n");
        exit(0);
    }

    if(argc < 2) {
        fprintf(stderr, "\nUso: %s <device ext2> <device di swap>\n\n", argv[0]);
        exit(0);
    }

    devname = argv[1];
    swapdev = argv[2];
        
    printf("\nTento di aprire %s come partizione ext2\n", devname);
	if (ext2fs_open(devname, EXT2_FLAG_RW, 0,0, unix_io_manager, &fs)){
        perror("ext2fs_open");
        return (2);
    }
       
    if((fdef = fopen("./metadata.h", "w")) == NULL) {
      	perror("open: metadata.h");
       	exit(0);
    }
    fprintf(fdef, "#define DEVICE\t\t\t\"%s\"\n", devname);
    fprintf(fdef, "#define SWAPDEV\t\t\t\"%s\"\n\n", swapdev);
        
    /* ora lavoriamo sul device della partizione linux prescelta. Tutte
       queste funzioni sono dichiarate in ext2fs/ext2fs.h. Possono essere
       usate sia per partizioni ext2 che ext3. E' necessario installare
       nel sistema la libreria ext2fs (di solito presente di default).
       Quello che vogliamo fare e' ottenere informazioni sul superblock
       della partizione, come inode, blocchi e gruppi, ma non solo.
       Vogliamo anche sapere quali siano i backup del superblock 0.	     */
           
	sb = (struct ext2_super_block *)fs->super;
	
	fprintf(fdef, "// Partizione ext2 su %s\n", devname);
	fprintf(fdef, "#define INODE_TOT_NUM\t\t\t%u\n", sb->s_inodes_count);
	fprintf(fdef, "#define GROUP_TOT_NUM\t\t\t%u\n", sb->s_blocks_count);
	fprintf(fdef, "#define BLOCK_PER_GRP\t\t\t%u\n", sb->s_blocks_per_group);
	fprintf(fdef, "#define INODE_PER_GRP\t\t\t%u\n", sb->s_inodes_per_group);
	fprintf(fdef, "#define GRP_TOT_NUM\t\t\t%u\n",
		(sb->s_inodes_count / sb->s_inodes_per_group));
	
	inoctl_n = (sizeof(sb->s_reserved)/4);
	modulo = inoctl_n % 4;
	fprintf(fdef, "#define INODE_CTL_MAX\t\t\t%d\n", (modulo ? inoctl_n - modulo:inoctl_n));
	fprintf(fdef, "#define INODE_DAT_MAX\t\t\t%u\n", (sb->s_inodes_count -
		(unsigned int)inoctl_n - 11));

	unsigned int sbs[(sb->s_inodes_count / sb->s_inodes_per_group)];
	for (i = 0; i < fs->group_desc_count; i++) {
		ext2fs_super_and_bgd_loc(fs, i, &super, &old, &new, 0);
		has_super = ((i==0) || super);
		if (has_super)
			sbs[n++] = (long unsigned)super;
		
	}
	fprintf(fdef, "#define SUPERBLOCK_TOT\t\t\t%d\n", n);
	fprintf(fdef, "\nunsigned int sb_blocks[]={");
	for(i=0; i < n; i++)
		fprintf(fdef, "%u,", sbs[i]);
	fseek(fdef, -1, 1);
	fprintf(fdef, "};\n\n");

    if (ext2fs_close(fs)){
        aborto("ext2fs_close");
    }
        
    usleep(500);
       
    /* ora passiamo allo swapfile. in questo caso e' necessario innanzitutto
       ottenere la signature che identifica il file, la versione, il numero
       di pagine disponibili e la loro dimensione. Utili linux/swap.h, il 
       sorgente di mkswap(8) ed ovviamente mm/swapfile.c nel kernel.	*/      
      
	k_pagesize = getpagesize();
	pagesize = k_pagesize;
	
    if((sd = fopen(swapdev, "a+")) == NULL) {
      	aborto("swap fopen");
    }	
    if((sd2 = open(swapdev, O_RDONLY) < 0)) {
       	aborto("swap open");
    }
        
    if (ioctl(sd2, BLKGETSIZE, &size) >= 0) {
		int sectors_per_page = pagesize/512;
		PAGES = size /= sectors_per_page;
	}
	else {
		PAGES = size = find_size(sd) / pagesize;
	}
	
	signature = (long *) malloc(pagesize);
	memset(signature, 0, pagesize);
    if((buff=fseek(sd, pagesize-10, SEEK_SET)) < 0) {
       	aborto("swap fseek");
    }
    if((buff=fread((void *)version, 10, 1, sd)) < 0) {
       	aborto("swap header read");
    }

    if (!memcmp("SWAP-SPACE",version,10))
        printf("Ho trovato uno swapfile con MAGIC:SWAP-SPACE\n\n");
    else if (!memcmp("SWAPSPACE2",version,10))
        printf("Ho trovato uno swapfile con MAGIC:SWAPSPACE2\n\n");
    	else {
        	printf("\n%s non corrisponde ad uno swapfile noto!\n\n", swapdev);
        	unlink("metadata.h");
        	exit(1);
        }
    if((buff=fseek(sd, 1024, SEEK_SET)) < 0) {
       	aborto("swap fseek");
    }
    if((buff=fread((void *)signature, pagesize, 1, sd)) < 0) {
       	aborto("swap header read");
    }
        
    sig = (struct swap_sig *)signature;
    fprintf(fdef, "\n// Swapfile su %s\n", swapdev);
    fprintf(fdef, "#define SWAP_VERSION\t\t\t%u\n", sig->version);
    fprintf(fdef, "#define SWAP_LAST_PAGE\t\t\t%u\n", sig->last_page);
    fprintf(fdef, "#define S_BAD_PAGES\t\t\t%u\n", sig->nr_badpages);
    fprintf(fdef, "#define S_BAD_INDEX\t\t\t%u\n", sig->badpages[1]);
    fprintf(fdef, "#define SWAP_TOT_SIZE\t\t\t%lu\n\n", (sig->last_page * PAGE_SIZE));
    fprintf(fdef, "#define BOO_PAGE_SIZE\t\t\t%d\n", pagesize);
    fprintf(fdef, "#define BOO_MAX_BAD_PAGE\t\t%lu\n", MAX_SWAP_BADPAGES);
        
 	free(signature);   														
    fclose(sd);
    close(sd2);
    
    /* generiamo una chiave pseudocasuale di 16 byte; questa sara' utilizzata
       per offuscare mediante XOR i nomi file nelle metastrutture; inoltre
       utilizziamo altri 8 byte provenienti da /dev/urandom per il magic
       number del "superblocco" delle nostre strutture di controllo 		*/
       
    memset(xorkey, 0x00, 16);
    if((urandom = open(URANDOM, O_RDONLY)) < 0) {
      	aborto("swap fopen");
    }  						
    
   	for(i = 0; i <= 15; i++)
   		read(urandom, &xorkey[i], 1);
   	
   	read(urandom, &magic, 8);
   			
   	fprintf(fdef, "\n// boo_xor_key per offuscamento dei nomi file\n");
	fprintf(fdef, "unsigned char boo_xor_key[]={");
	for(i=0; i <= 15; i++)
		fprintf(fdef, "0x%x,", xorkey[i]);
	fseek(fdef, -1, 1);
	fprintf(fdef, "};\n\n");
	
	fprintf(fdef, "\n// magic number per il superblock in ext2\n");
	fprintf(fdef, "#define BOO_SB_MAGIC_0\t\t0x%x%x%x%x\n", magic[0], magic[1],
					magic[2], magic[3]);
	fprintf(fdef, "#define BOO_SB_MAGIC_1\t\t0x%x%x%x%x\n", magic[4], magic[5],
					magic[6], magic[7]);	
   	
   	close(urandom);	 			 				 			 			
	fclose(fdef);	
	exit(0);       
}
