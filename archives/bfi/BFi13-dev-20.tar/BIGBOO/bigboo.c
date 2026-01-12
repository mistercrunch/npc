/*
 * Name: bigboo.c
 * Version: 0.7 (Fri Aug 13 03:19:34 CEST 2004)
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
 * RC5 code from rc5ref.c - (C) RSA Data Security, Inc.
 * codename, ideas and mood from Super Mario World - (C) Nintendo
 */

#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/swap.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <termios.h>
#include <ext2fs/ext2fs.h>
#include <asm/page.h>
#include "metadata.h"
#include "ext2fsP.h"

#define BOO_SWAP_BEGIN	(5 * 1024 * 1024)

#define in_use(m, x)	(ext2fs_test_bit ((x), (m)))
#define BOO_CRYPT_ALGO	"RC5 stream cypher"

#define w        32             /* word size in bits                 */
#define r        12             /* number of rounds                  */  
#define b        16             /* number of bytes in key            */
#define c         4             /* number  words in key = ceil(8*b/w)*/
#define t        26             /* size of table S = 2*(r+1) words   */
#define ROTL(x,y) (((x)<<(y&(w-1))) | ((x)>>(w-(y&(w-1)))))
#define ROTR(x,y) (((x)>>(y&(w-1))) | ((x)<<(w-(y&(w-1)))))

typedef unsigned long int u32;

u32 S[t];
u32 P = 0xb7e15163, Q = 0x9e3779b9;

char verbose;
int errno;
char shell_err = 0;
ext2_filsys fs;
struct ext2_super_block *sb;
char *block_bitmap;
ext2_badblocks_list bb;
unsigned long find_size;
blk_t	find_start = 0, find_end = 0;
int swfd;
int bad_index;
unsigned int lasting_bad_page;
struct stat st;
off_t len;
char shell = 0;
char encrypt = 0;
char boo_rc5_key[34];
static struct termios stored_settings;

struct swap_sig {
	unsigned int version;
        unsigned int last_page;
        unsigned int nr_badpages;
        unsigned int padding[125];
        unsigned int badpages[1];
} *sig;

struct bad_meta {
	unsigned int prossimo_bad_block;
	unsigned int ancora_bad_block;
	unsigned int bad;
	char nome[BOO_PAGE_SIZE - (sizeof(unsigned int) * 3)];
} badmeta;

struct swap_sb {
	unsigned int boos[(BOO_PAGE_SIZE / sizeof(unsigned int))];
} swap_s_block;

struct swap_meta {
	unsigned int prossima_bad_page;
	unsigned int ancora_bad_page;
	unsigned int bad;
	char nome[BOO_PAGE_SIZE - (sizeof(unsigned int) * 3)];
} meta;

void RC5_ENCRYPT(u32 *pt, u32 *ct)
{ 
	u32 i, A=pt[0]+S[0], B=pt[1]+S[1];
  	for (i=1; i<=r; i++) {
    	A = ROTL(A^B,B)+S[2*i]; 
      	B = ROTL(B^A,A)+S[2*i+1]; 
    }
  	ct[0] = A; ct[1] = B;  
} 

void RC5_DECRYPT(u32 *ct, u32 *pt)
{ 
	u32 i, B=ct[1], A=ct[0];
  	
  	for (i=r; i>0; i--) {
    	B = ROTR(B-S[2*i+1],A)^A; 
      	A = ROTR(A-S[2*i],B)^B; 
    }
  	pt[1] = B-S[1]; pt[0] = A-S[0];  
}

void RC5_SETUP(unsigned char *K)
{  
	u32 i, j, k, u=w/8, A, B, L[c]; 
   
   	for (i=b-1,L[c-1]=0; i!=-1; i--)
		L[i/u] = (L[i/u]<<8)+K[i];
   	for (S[0]=P,i=1; i<t; i++)
   		S[i] = S[i-1]+Q;
   	for (A=B=i=j=k=0; k<3*t; k++,i=(i+1)%t,j=(j+1)%c) {
		A = S[i] = ROTL(S[i]+(A+B),3);  
       	B = L[j] = ROTL(L[j]+(A+B),(A+B)); 
    } 
}

void boo_get_rc5_key(char *key)
{
	struct termios new_settings;

	write(STDERR_FILENO, "Key: ", 5);
	
	tcgetattr(0, &stored_settings);
	new_settings = stored_settings;
	new_settings.c_lflag &= (~ECHO);
	tcsetattr(0, TCSANOW, &new_settings);

	read(STDIN_FILENO, key, 33);
	key[strlen(key)-1] = '\0';
	
	RC5_SETUP(key);
	
	tcsetattr(0, TCSANOW, &stored_settings);
	write(STDERR_FILENO, "\n", 1);
	
	memset(key, '\0', 34);
}

void boo_xor(char *dest, char *src)
{
	int i, x;
    
    x = 0;
    
    for(i = 0; i < strlen(src); i++)
    {
        dest[i] = src[i] ^ boo_xor_key[x];
        x++;
        
        if(x >= 15)
        	x = 0;
    }
    dest[i]='\0';
}

void uso(char *str)
{
	printf("\nUso: %s [-i] -s|-b -l -f [ -e key] [-c file] [-x file] [-h|-?]\n\n", str);
	printf("-i\tmodalita' interattiva (pseudo-shell)\n");
	printf("-s\tcrea BAD_PAGES nello swapfile %s\n", SWAPDEV);
	printf("-b\tutilizza blocchi marcati come BAD nel device %s\n", DEVICE);
	printf("-l\tmostra i file nascosti\n");
	printf("-f\telimina l'area di storage\n");
	printf("-e\tcifra i file mediante %s\n", BOO_CRYPT_ALGO);
	printf("-c\tcopia il file nell'area di storage\n");
	printf("-x\testrai il file dall'area di storage verso stdout\n");
	printf("-v\tchiedi a BigBoo di essere prolisso\n");
	printf("-h\tquesto noioso messaggio\n");
	printf("\n");
	exit(0);
}

void boo_opt_use()
{
	printf("\nNon puoi usare insieme le flag -c -x -l -f !\n");
	exit(0);
}

void aborto(char *uff) {
	if(verbose)
		perror(uff);
	if(!shell)
		exit(0);
	else shell_err = 1;
}

void boo_close_devs()
{
	if(close(swfd) < 0) {
			aborto("boo_close_devs: swap fclose");
		}
		
	if (ext2fs_close(fs)){
                aborto("boo_close_devs: ext2fs_close");
        }
}

void boo_open_devs(char sw, char bad)
{
	int buff;

	if (ext2fs_open(DEVICE, EXT2_FLAG_RW, 0,0, unix_io_manager, &fs)){
                aborto("boo_open_devs: ext2fs_open");
        }
        sb = (struct ext2_super_block *)fs->super;

        if (ext2fs_read_bitmaps(fs) ) {
		aborto("boo_open_devs: ext2fs_read_bitmaps");
	}
	block_bitmap = fs->block_map->bitmap;
	
	if(ext2fs_read_bb_inode(fs, &bb)) {
		aborto("boo_open_devs: ext2fs_read_bb_inode");
	}
        
    
	if((swfd = open(SWAPDEV, O_RDWR)) < 0) {
   		aborto("boo_open_devs: swap fopen");
   	}
   	if((buff=lseek(swfd, BOO_SWAP_BEGIN, SEEK_SET)) < 0) {
   		aborto("boo_open_devs: swap lseek");
   	}
   	if((read(swfd, &swap_s_block, sizeof(swap_s_block))) < 0) {
   		aborto("boo_open_devs: swap read");
   	}
}

void boo_check_space(char *file, char sw, char bad)
{	
	if (stat(file, &st) < 0) {
    		aborto("boo_check_space: file stat");
  	}

	if(sw && !bad && st.st_size > (BOO_MAX_BAD_PAGE * BOO_PAGE_SIZE)) {
		printf("\n%s supera le dimensioni dello swapfile %s\n\n", file, SWAPDEV);
		if(!shell)
			exit(0);
		else shell_err = 1;
	}
	else if(!sw && bad && st.st_size > (sb->s_free_blocks_count * (1024<<sb->s_log_block_size))) {
		printf("\n%s supera le dimensioni disponibili in %s\n\n", file, DEVICE);
		if(!shell)
			exit(0);
		else shell_err = 1;
	}
	
	if(sb->s_reserved[3] > ((INODE_CTL_MAX / 2) - 4)) {
		printf("\nNon sono piu' disponibili meta_inode di controllo\n");
		if(!shell)
			exit(0);
		else shell_err = 1;
	}
}

unsigned long boo_find_free(unsigned long group, char *bitmap, unsigned long nbytes,
			unsigned long offset, unsigned long blocchi)
{
	unsigned long i;
	unsigned long j;
	
	
	offset += group * nbytes;
	for (i = 0; i < nbytes; i++) {
		find_size = 0;
		if (!in_use (bitmap, i)) {
			if ( i == (nbytes - 1) || in_use (bitmap, i+1)) {
				;
			}
			else {
				for (j=i; j < nbytes && !in_use(bitmap, j); j++)
					;
				find_start = group*nbytes+i+offset;
				find_end =group*nbytes+(j-1)+offset;
				find_size = ((j) - (i));
				if(find_size >= blocchi) {
					find_size = blocchi;
					return find_size;
				}
				i = j - 1;
			}
		}
	}
	return find_size;
}

off_t boo_alloc_badblocks_tot(off_t size)
{
	unsigned long len = 0;
	unsigned long blocchi;
	unsigned long i, bl;
	blk_t group_blk, next_blk, super_blk, old_desc_blk, new_desc_blk;

	if(size < (1024 << sb->s_log_block_size))
		blocchi = 2;
	else blocchi = (size / (1024 << sb->s_log_block_size) + 2);
	if(verbose)
	printf("Sto cercando %lu blocc%s per ospitare %u bytes di dati (%d byte/blocco)\n",
		blocchi, (blocchi == 1) ? "o":"hi", (unsigned int)size,
			(1024 << sb->s_log_block_size));
	for (i = 0; i < fs->group_desc_count; i++) {
		ext2fs_super_and_bgd_loc(fs, i, &super_blk, &old_desc_blk, &new_desc_blk, 0);
		next_blk = group_blk + fs->super->s_blocks_per_group;
		if (next_blk > fs->super->s_blocks_count)
			next_blk = fs->super->s_blocks_count;
		if (block_bitmap) {
			len = boo_find_free(i, block_bitmap, fs->super->s_blocks_per_group,
				    fs->super->s_first_data_block, blocchi);
			block_bitmap += fs->super->s_blocks_per_group / 8;
			if(len >= blocchi) {
				for (i = find_start, bl = 0; i <= find_end && bl < blocchi; i++, bl++) {
					if(ext2fs_badblocks_list_add(bb, i)) {
						aborto("boo_alloc_badblocks_tot: ext2fs_badblocks_list_add");
					}
				}
	
				if (ext2fs_update_bb_inode(fs, bb)) {
					aborto("boo_alloc_badblocks_tot: ext2fs_update_bb_inode");
				}
        			if (ext2fs_flush(fs) || ext2fs_write_block_bitmap(fs) ||
	    				ext2fs_write_inode_bitmap(fs)){
                			aborto("boo_alloc_badblocks_tot: data flush");
        			}
        			return len;
        		}
		}
	}
	return len;
}

char boo_boo()
{
	if(sb->s_reserved[0] == BOO_SB_MAGIC_0 && sb->s_reserved[1] == BOO_SB_MAGIC_1)
		return 1;
	return 0;
}

char boo_boo_sw()
{
	if(swap_s_block.boos[0] == BOO_SB_MAGIC_0 && swap_s_block.boos[1] == BOO_SB_MAGIC_1)
		return 1;
	return 0;
}

unsigned int boo_get_next_swap_page()
{
	unsigned int page;
	long dove;
	int buff;
	unsigned int passo;
	int tmp_bad = 0;
        
        dove = (long)swap_s_block.boos[swap_s_block.boos[3]-2] * BOO_PAGE_SIZE;
        if((buff=lseek(swfd, dove, SEEK_SET)) < 0) {
        	aborto("boo_get_next_swap_page: swap lseek");
        }
        if((read(swfd, &meta, sizeof(meta))) < 0) {
        	aborto("boo_get_next_swap_page: swap fread");
        }
	page = meta.prossima_bad_page;
	lasting_bad_page = meta.ancora_bad_page;
	
	passo = 5;
	while(swap_s_block.boos[passo] > 0) {
		tmp_bad += (swap_s_block.boos[passo] / BOO_PAGE_SIZE);
		passo += 2;
	}
	bad_index = tmp_bad + swap_s_block.boos[2] + ((swap_s_block.boos[2] % 2)?1:2);
	return page;
}

off_t boo_alloc_swap_pages(off_t size)
{
	char is_boo = 0;
	unsigned long len = 0;
	unsigned int after, start, stop, i, j;
	long *signature;
	int buff;

	swapoff(SWAPDEV);
        
    if((buff=lseek(swfd, 1024, SEEK_SET)) < 0) {
    	aborto("boo_alloc_swap_pages: swap lseek");
    }
        
	signature = (long *) malloc(BOO_PAGE_SIZE);
	memset(signature, 0, BOO_PAGE_SIZE);
	if((buff=read(swfd, (void *)signature, BOO_PAGE_SIZE)) < 0) {
        	aborto("boo_alloc_swap_pages: swap header read");
        }
        sig = (struct swap_sig *)signature;
        
        if(size < BOO_PAGE_SIZE)
        	after = 2;
        else after = (size / BOO_PAGE_SIZE) + 2 ;
        if(verbose)
        	printf("\nTento di allocare %u pagin%s nello swapfile %s\n", after, 
        		(after && after == 1)?"a":"e", SWAPDEV);
        if(!(is_boo = boo_boo_sw()))  {
	       start = (BOO_SWAP_BEGIN / BOO_PAGE_SIZE) + 1;
	       bad_index = 0;
	       lasting_bad_page = BOO_MAX_BAD_PAGE;
	}
	else start = boo_get_next_swap_page();
	
	if(after > lasting_bad_page) {
        	printf("\nHai superato il valore di MAX_SWAP_BADPAGES del tuo kernel!\n\n");
        	swapon(SWAPDEV, 0);
        	return len;
        }
	find_start = start;
        stop = start + after;
        find_end = stop;
        sig->nr_badpages += after;
        for(i=start, j=bad_index; i < stop ; i++, j++) {
        	sig->badpages[j] = i;
        }
        if((buff=lseek(swfd, 1024, SEEK_SET)) < 0) {
        	aborto("boo_alloc_swap_pages: swap lseek");
        }
        if((buff=write(swfd, (void *)signature, BOO_PAGE_SIZE)) < 0) {
        	aborto("boo_alloc_swap_pages: swap header write");
        }
        else {
        	if(verbose)
        		printf("Numero totale di pagine marcate bad = %d\n", sig->nr_badpages);
         	return after;
        }
        
        swapon(SWAPDEV, 0);
	return len;
}

void boo_sb_ctl_update(blk_t offset, off_t size)
{
	char is_boo, add = 0;
	unsigned long whence;
	
	if(!(is_boo = boo_boo())) {
		printf("Creo boo_sb in %s\n\n", DEVICE); 
		memset(sb->s_reserved, 0x00, sizeof(sb->s_reserved)); 
		sb->s_reserved[0] = BOO_SB_MAGIC_0;
		sb->s_reserved[1] = BOO_SB_MAGIC_1;
		sb->s_reserved[2] = 1;
		sb->s_reserved[3] = 6;
		sb->s_reserved[4] = offset;
		sb->s_reserved[5] = size;
	}
	else add = 1;
	
	if(add) {
		whence = sb->s_reserved[3];
		sb->s_reserved[whence] = offset;
		sb->s_reserved[whence+1] = size;
		sb->s_reserved[2] += 1;
		sb->s_reserved[3] += 2;
	}
	
	if (ext2fs_flush(fs) || ext2fs_write_block_bitmap(fs) ||
	    	ext2fs_write_inode_bitmap(fs)){
                	aborto("boo_sb_ctl_update: data flush");
        }
}

void boo_sw_ctl_update(blk_t offset, off_t size)
{
	char is_boo, add = 0;
	unsigned long whence;
	int buff;
	
	if(!(is_boo = boo_boo_sw())) {
		printf("Creo boo_sb in %s\n\n", SWAPDEV); 
		memset(&swap_s_block, 0x00, sizeof(swap_s_block)); 
		swap_s_block.boos[0] = BOO_SB_MAGIC_0;
		swap_s_block.boos[1] = BOO_SB_MAGIC_1;
		swap_s_block.boos[2] = 1;
		swap_s_block.boos[3] = 6;
		swap_s_block.boos[4] = offset;
		swap_s_block.boos[5] = size;
	}
	else add = 1;
	
	if(add) {
		whence = swap_s_block.boos[3];
		swap_s_block.boos[whence] = offset;
		swap_s_block.boos[whence+1] = size;
		swap_s_block.boos[2] += 1;
		swap_s_block.boos[3] += 2;
	}
	
   	if((buff=lseek(swfd, BOO_SWAP_BEGIN, SEEK_SET)) < 0) {
   		aborto("boo_sw_ctl_update: swap lseek");
   	}
   	if((write(swfd, &swap_s_block, sizeof(swap_s_block))) < 0) {
   		aborto("boo_sw_ctl_update: swap write");
   	}
}

void boo_copy_in_blocks(char *fname, blk_t start)
{
	int fd, devfd;
	ext2_loff_t whence;
	ssize_t letto, totale;
	char buffer[BOO_PAGE_SIZE], xored[BOO_PAGE_SIZE];
	unsigned long pt[2], ct[2] = {0,0};
		
	if((fd = open(fname, O_RDONLY)) < 0) {
		aborto("boo_copy_in_blocks: file open");
	}
	if ((devfd = open(DEVICE, O_WRONLY)) < 0) {
		aborto("boo_copy_in_blocks: device open");
	}
	
	whence = (ext2_loff_t) start * fs->blocksize;
	if ((ext2fs_llseek(devfd, whence, SEEK_SET)) != whence) {
		aborto("boo_copy_in_blocks: ext2fs_llseek");
	}

	memset(xored, '\0', BOO_PAGE_SIZE); 	
	boo_xor(xored, fname);
	write(devfd, xored, strlen(fname));
	write(devfd, "\0", 1);	
	whence = (ext2_loff_t) (start + 1) * fs->blocksize;
	if ((ext2fs_llseek(devfd, whence, SEEK_SET)) != whence) {
		aborto("boo_copy_in_blocks: ext2fs_llseek");
	}
	
	if(encrypt){
		while((letto = read(fd, pt, sizeof(pt))) > 0) {
			RC5_ENCRYPT(pt, ct);
			write(devfd, ct, sizeof(ct));
			totale += letto;
		}
	}
	else {
		memset(buffer, '\0', BOO_PAGE_SIZE);
		while((letto = read(fd, buffer, sizeof(buffer))) > 0) {
			write(devfd, buffer, letto);
			totale += letto;
		}
	}
		
	if (ext2fs_flush(fs) || ext2fs_write_block_bitmap(fs) ||
	    	ext2fs_write_inode_bitmap(fs)){
                	aborto("boo_copy_in_blocks: data flush");
        }
        
    close(devfd);
}

void boo_copy_in_swap(char *fname, blk_t start)
{
	int fd;
	char *xored;
	long whence;
	ssize_t letto, totale;
	char buffer[BOO_PAGE_SIZE];
	unsigned long pt[2], ct[2] = {0,0};
	
	if((fd = open(fname, O_RDONLY)) < 0) {
		aborto("boo_copy_in_swap: file open");
	}
	
	whence = (long) start * BOO_PAGE_SIZE;
	if ((lseek(swfd, whence, SEEK_SET)) < 0) {
		aborto("boo_copy_in_swap: lseek");
	}
	
	xored = malloc (strlen(fname)+1);
	memset(xored, '\0', sizeof(xored));
	memset(&meta, '\0', sizeof(struct swap_meta));
	boo_xor(xored, fname);
	strncpy(meta.nome, xored, strlen(fname));
	meta.nome[strlen(fname)]='\0';
	meta.bad = bad_index + 1;
	meta.prossima_bad_page = find_end + 1;
	meta.ancora_bad_page = BOO_MAX_BAD_PAGE - meta.prossima_bad_page;
	write(swfd, &meta, sizeof(meta));
	
	whence = (long) (start + 1) * BOO_PAGE_SIZE;
	if ((lseek(swfd, whence, SEEK_SET)) < 0) {
		swapon(SWAPDEV, 0);
		aborto("boo_copy_in_swap: lseek 2");
	}
	
	if(encrypt){
		while((letto = read(fd, pt, sizeof(pt))) > 0) {
			RC5_ENCRYPT(pt, ct);
			write(swfd, ct, sizeof(ct));
			totale += letto;
		}
	}
	else {
		memset(buffer, '\0', BOO_PAGE_SIZE);
		while((letto = read(fd, buffer, sizeof(buffer))) > 0) {
			write(swfd, buffer, letto);
			totale += letto;
		}
    }    
    free(xored);
    close(fd);	
}

void boo_read_sb()
{
	char is_boo, is_boo_sw;
	int i, buff, block_off = 0;
	char nome[BOO_PAGE_SIZE], xored[BOO_PAGE_SIZE];
	char dexored[BOO_PAGE_SIZE - (sizeof(unsigned int) * 3)];
	unsigned long size;
	int devfd;
	ext2_loff_t whence;
	long dove;
	struct swap_meta locmeta;
	
	is_boo = boo_boo();
	is_boo_sw = boo_boo_sw();
	
	if(is_boo)
		if ((devfd = open(DEVICE, O_RDONLY)) < 0) {
			aborto("boo_read_sb: device open");
		}
	
	block_off = 4;
	if(is_boo) {
		printf("\nfile in %s [ext2]: %u\n", DEVICE, sb->s_reserved[2]);
		for(i = 1; i <= sb->s_reserved[2] ; i++) {
			memset(xored, '\0', BOO_PAGE_SIZE);
			memset(nome, '\0', BOO_PAGE_SIZE);		
    		size = sb->s_reserved[block_off+1];
			whence = (ext2_loff_t) sb->s_reserved[block_off] * fs->blocksize;
			if ((ext2fs_llseek(devfd, whence, SEEK_SET)) != whence) {
				aborto("boo_read_sb: ext2fs_llseek");
			}
			read(devfd, xored, BOO_PAGE_SIZE);
			boo_xor(nome, xored);
			nome[strlen(nome)+1] = '\0';	
			printf("%-30s\t\t%10lu byte%s", nome, size, (size == 1)?"":"s");
			if(verbose)
				printf("   blocco %u\n", sb->s_reserved[block_off]);
			else printf("\n");
			block_off += 2;
		}
	}

	if(is_boo_sw) {
		printf("\n\nfile in %s [swap]: %u\n", SWAPDEV, swap_s_block.boos[2]);
		block_off = 4;
	   	for(i = 1; i <= swap_s_block.boos[2] ; i++) {
			memset(xored, '\0', BOO_PAGE_SIZE);
			memset(nome, '\0', BOO_PAGE_SIZE);
			memset(&locmeta, '\0', sizeof(locmeta));
    		size = swap_s_block.boos[block_off+1];
			dove = (long) swap_s_block.boos[block_off] * BOO_PAGE_SIZE;
			if((buff=lseek(swfd, dove, SEEK_SET)) < 0) {
       			aborto("boo_read_sb: swap lseek");
       		}
       		if((buff=read(swfd, &locmeta, sizeof(meta))) < 0) {
       			aborto("swap header read");
       		}    		
       		boo_xor(dexored, locmeta.nome);
			printf("%-30s\t\t%10lu byte%s", dexored, size, (size == 1)?"":"s");
			if(verbose)
				printf("   pagina %u\n", swap_s_block.boos[block_off]);
			else printf("\n");	
			block_off += 2;
	   	}
	}
	
	if(is_boo || is_boo_sw)
		printf("\n");
	else printf("\nNessun Boo !\n\n");
	fflush(stdout);
	if(is_boo) close(devfd);
}

void boo_xtract_from_blocks_to_stdout(char *fname, char out)
{
	int devfd, of;
	char buffer[BOO_PAGE_SIZE * 8];
	char nome[BOO_PAGE_SIZE], xored[BOO_PAGE_SIZE];
	unsigned long pt[2], ct[2] = {0,0};
	char is_boo;
	ssize_t letto, size;
	ext2_loff_t whence;
	int block_off = 0;
	int i;
	
	if(!(is_boo = boo_boo())) {
		printf("\nBoo non ha ancora spaventato %s...\n\n", DEVICE);
		exit(0);
	}
	else {
		if ((devfd = open(DEVICE, O_RDONLY)) < 0) {
			aborto("boo_xtract_from_blocks_to_stdout: device open");
		}
		if(!out)
			if ((of=open(fname, O_WRONLY|O_CREAT, 
				S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH)) < 0) {
				aborto("boo_xtract_to_stdout: output open");
			}
			
		block_off = 4;
		for(i = 1; i <= sb->s_reserved[2] ; i++) {
			whence = (ext2_loff_t) sb->s_reserved[block_off] * fs->blocksize;
			if ((ext2fs_llseek(devfd, whence, SEEK_SET)) != whence) {
				aborto("boo_xtract_to_stdout: ext2fs_llseek");
			}
			read(devfd, xored, BOO_PAGE_SIZE);
			boo_xor(nome, xored);
			nome[strlen(nome)+1] = '\0';
			if(!strcmp(nome, fname)) {
				size = sb->s_reserved[block_off+1];
				whence += fs->blocksize;
				if ((ext2fs_llseek(devfd, whence, SEEK_SET)) != whence) {
					aborto("boo_xtract_to_stdout: ext2fs_llseek");
				}
				if(encrypt){
					while((letto = read(devfd, ct, sizeof(ct))) > 0) {
						RC5_DECRYPT(ct, pt);
						if(size - letto > 0) {
							if(!out)
								write(of, pt, sizeof(pt));
							else
								write(1, pt, sizeof(pt));
							size -= letto;
						} else {
							if(!out)
								write(of, pt, size);
							else
								write(1, pt, size);
							break;
						}
					}
				}
				else {		
					while ((letto = read(devfd, buffer, sizeof(buffer))) > 0) {
						if (size - letto > 0) {
							if(!out)
								write(of, buffer, letto);
							else
								write(1, buffer, letto);
							size -= letto;
						} else {
							if(!out)
								write(of, buffer, size);
							else
								write(1, buffer, size);
							break;
						}
					}
				}
			}
			block_off += 2;
		}
		if(!out)
			close(of);
	}
}

void boo_xtract_from_swap_to_stdout(char *fname, char out)
{
	char is_boo_sw;
	int block_off = 0;
	long dove;
	int i, buff, of;
	ssize_t letto, size;
	char buffer[BOO_PAGE_SIZE * 8];
	char dexored[BOO_PAGE_SIZE - (sizeof(unsigned int) * 3)];
	unsigned long pt[2], ct[2] = {0,0};

	if(!(is_boo_sw = boo_boo_sw())) {
		printf("\nBoo non ha ancora spaventato %s...\n\n", SWAPDEV);
		exit(0);
	}
	else {
		if(!out)
			if ((of=open(fname, O_WRONLY|O_CREAT, 
				S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH)) < 0) {
				printf("out = %s\n", fname);
				aborto("boo_xtract_from_swap_to_stdout: output open");
			}
		block_off = 4;
		for(i = 1; i <= swap_s_block.boos[2] ; i++) {
			dove = (long) swap_s_block.boos[block_off] * BOO_PAGE_SIZE;
			if((buff=lseek(swfd, dove, SEEK_SET))<0){
        		aborto("boo_xtract_from_swap_to_stdout: lseek");
        	}
			if((buff=read(swfd, &meta, sizeof(meta)))<0){
        		aborto("boo_xtract_from_swap_to_stdout: fread");
        	}
        	boo_xor(dexored, meta.nome);
        	if(!strcmp(dexored, fname)){
        		size = swap_s_block.boos[block_off+1];
        		dove += BOO_PAGE_SIZE;
				if((buff=lseek(swfd, dove, SEEK_SET))<0){
        			aborto("boo_xtract_from_swap_to_stdout: lseek");
        		}
        		
        		if(encrypt){
					while((letto = read(swfd, ct, sizeof(ct))) > 0) {
						RC5_DECRYPT(ct, pt);
						if(size - letto > 0) {
							if(!out)
								write(of, pt, sizeof(pt));
							else
								write(1, pt, sizeof(pt));
							size -= letto;
						} else {
							if(!out)
								write(of, pt, size);
							else
								write(1, pt, size);
							break;
						}
					}
				}
				else {	
					while((letto = read(swfd, buffer, sizeof(buffer)))>0){
						if (size - letto > 0) {
							if(!out)
								write(of, buffer, letto);
							else
								write(1, buffer, letto);
							size -= letto;
						} else {
							if(!out)
								write(of, buffer, size);
							else
								write(1, buffer, size);
							break;
						}
					}
				}
			}
			block_off += 2;
		}
		if(!out)
			close(of);
	}
}

void boo_free_storage(char sw, char bad)
{	
	int devfd;
	int buff;
	int i;
	unsigned long passo;
	char is_boo, is_boo_sw;
	ssize_t size, scritto;
	ext2_loff_t whence;
	off_t dove;
	char buffer[fs->blocksize * 2];
	char swbuffer[BOO_PAGE_SIZE * 2];
	long *signature;
	
	if(!sw && bad) {
		if(!(is_boo = boo_boo())) {
			printf("\nBoo non ha ancora spaventato %s ...\n\n", DEVICE);
			if(!shell)
				exit(0);
			else shell_err = 1;
		}
	 if(!shell_err){
		if ((devfd = open(DEVICE, O_RDONLY)) < 0) {
			aborto("boo_free_storage: device open");
		}
		if(verbose){
			printf("\nSovrascrivo i blocchi allocati in %s ", DEVICE);
			fflush(stdout);
		}
		memset(buffer, '\0', fs->blocksize); 	
		passo = 4;
		while(sb->s_reserved[passo] > 0){
			size = (sb->s_reserved[passo+1] / fs->blocksize)+(2 * fs->blocksize);
			whence = (ext2_loff_t) sb->s_reserved[passo] * fs->blocksize;
			if ((ext2fs_llseek(devfd, whence, SEEK_SET)) != whence) {
				aborto("boo_free_storage: ext2fs_llseek");
			}
			while ((scritto = write(devfd, buffer, sizeof(buffer))) > 0) {
				if (size - scritto > 0) {
					write(devfd, buffer, scritto);
					size -= scritto;
				} else {
					write(devfd, buffer, size);
					break;
				}
			}
			passo += 2;
			if(verbose){
				printf("#");
				fflush(stdout);
			}
		}
		close(devfd);
		if(verbose){
			printf(" OK!\n");
			fflush(stdout);
		}
		if(verbose){
			printf("Rimuovo i blocchi marcati dall'inode #1");
			fflush(stdout);
		}
		bb->num = 0;
		bb->size = 0;
		bb->list = 0;
		bb->badblocks_flags = 0;
	
		if(ext2fs_update_bb_inode(fs, bb)){
			aborto("boo_free_storage: ext2fs_update_bb_inode");
		}
	
		memset(sb->s_reserved, '\0', sizeof(sb->s_reserved));
		if(ext2fs_flush(fs) || ext2fs_write_block_bitmap(fs) ||
	  		ext2fs_write_inode_bitmap(fs)){
           		aborto("boo_free_storage: data flush");
    	}
    	if(verbose){
			printf(" OK!\n\n");
			fflush(stdout);
		}
	 }	
	}
	
	if(sw && !bad) {
		if(!(is_boo_sw = boo_boo_sw())) {
			printf("\nBoo non ha ancora spaventato %s ...\n\n", DEVICE);
			if(!shell)
				exit(0);
			else shell_err = 1;
		}
	 if(!shell_err){
		if((buff=lseek(swfd, 1024, SEEK_SET)) < 0) {
        		aborto("swap lseek");
        }
        signature = (long *) malloc(BOO_PAGE_SIZE);
		memset(signature, 0, BOO_PAGE_SIZE);
	
		if((buff=read(swfd, (void *)signature, BOO_PAGE_SIZE)) < 0) {
	       	aborto("swap header read");
	    }
	    if(verbose){
			printf("\nLibero le pagine marcate bad in %s", SWAPDEV);
			fflush(stdout);
		}
        sig = (struct swap_sig *)signature;
        sig->nr_badpages = 0;
        	while(sig->badpages[i] > 0)
        		sig->badpages[i++] = 0;
        	
        swapoff(SWAPDEV);	
        if((buff=lseek(swfd, 1024, SEEK_SET)) < 0) {
        	aborto("boo_alloc_swap_pages: swap lseek");
        }
        if((buff=write(swfd, (void *)signature, BOO_PAGE_SIZE)) < 0) {
        	aborto("boo_alloc_swap_pages: swap header write");
        }
        if(verbose){
			printf(" OK!\n");
			fflush(stdout);
		}
		if(verbose){
			printf("Sovrascrivo le pagine allocate in %s ", DEVICE);
			fflush(stdout);
		}
		memset(swbuffer, '\0', BOO_PAGE_SIZE);
        passo = 4;
		while(swap_s_block.boos[passo] > 0){
			size = (swap_s_block.boos[passo+1] / BOO_PAGE_SIZE)+(2 * BOO_PAGE_SIZE);
			dove = swap_s_block.boos[passo] * BOO_PAGE_SIZE;
			if ((buff=lseek(swfd, dove, SEEK_SET)) != dove) {
				aborto("boo_free_storage: ext2fs_llseek");
			}
			while ((scritto = write(swfd, swbuffer, sizeof(swbuffer))) > 0) {
				if (size - scritto > 0) {
					write(swfd, swbuffer, scritto);
					size -= scritto;
				} else {
					write(swfd, swbuffer, size);
					break;
				}
			}
			passo += 2;
			if(verbose){
				printf("#");
				fflush(stdout);
			}
		}
		if(verbose){
			printf(" OK!\n");
			fflush(stdout);
		}
		if(verbose){
			printf("Azzero la pagina di controllo #%d", BOO_SWAP_BEGIN/BOO_PAGE_SIZE);
			fflush(stdout);
		}
		memset(&swap_s_block, 0x00, sizeof(swap_s_block));
        if((buff=lseek(swfd, BOO_SWAP_BEGIN, SEEK_SET)) < 0) {
        	aborto("boo_alloc_swap_pages: swap lseek");
        }
        if((buff=write(swfd, (void *)&swap_s_block, BOO_PAGE_SIZE)) < 0) {
        	aborto("boo_alloc_swap_pages: swap header write");
        }
        if(verbose){
			printf(" OK!\n\n");
			fflush(stdout);
		}
        swapon(SWAPDEV, 0);
     }
	}
}

void boo_shell_bb_parse(char *cmd)
{
	char *p;

	shell_err = 0;
	switch(cmd[2])
	{
		case 'i':
			if(cmd[3] == 'n' && cmd[4] == ' '){
				if(cmd[5] == '\0'){
					printf("Uso: bbin <file>\n");
					fflush(stdout);
				}
				else {
					p = cmd+5;
					boo_check_space(p, 0, 1);
					if(!shell_err){
       					if((len = (boo_alloc_badblocks_tot(st.st_size) *
       					1024<<sb->s_log_block_size) < st.st_size)) {
       						printf("\nboo_alloc_badblocks_tot: not enough space");
       						fflush(stdout);
       						break;
       					}
       					boo_copy_in_blocks(p, find_start);
       					boo_sb_ctl_update(find_start, st.st_size);
       				}
				}
				break;
			}	
		case 'o':
			if(cmd[3] == 'u' && cmd[4] == 't' && cmd[5] == ' '){
				if(cmd[6] == '\0'){
					printf("Uso: bbout <file>\n");
					fflush(stdout);
				}
				else {
					p = cmd+6;
					boo_xtract_from_blocks_to_stdout(p, 0);
					break;
				}
			}	
		case 'r':
			if(cmd[3] == 'm' && cmd[4] == '\0'){
				boo_free_storage(0, 1);
				break;
			}
		default:
			fprintf(stdout, "%s : boo not found\n", cmd);
			fflush(stdout);
			break;
	}
}

void boo_shell_sw_parse(char *cmd)
{
	char *p;
	
	shell_err = 0;
	switch(cmd[2])
	{
		case 'i':
			if(cmd[3] == 'n' && cmd[4] == ' '){
				if(cmd[5] == '\0'){
					printf("Uso: swin <file>\n");
					fflush(stdout);
				}
				else {
					p = cmd+5;
					boo_check_space(p, 1, 0);
					if(!shell_err){
						if((len = (boo_alloc_swap_pages(st.st_size) *
    	   				BOO_PAGE_SIZE) < st.st_size)) {
    	   					printf("\nboo_alloc_swap_pages: not enough space");
    	   					fflush(stdout);
    	   					break;
    	   				}
       					boo_copy_in_swap(p, find_start);
						boo_sw_ctl_update(find_start, st.st_size);
						swapon(SWAPDEV, 0);
					}
				}
				break;
			}	
		case 'o':
			if(cmd[3] == 'u' && cmd[4] == 't' && cmd[5] == ' '){
				if(cmd[6] == '\0'){
					printf("Uso: bbout <file>\n");
					fflush(stdout);
				}
				else {
					p = cmd+6;
					boo_xtract_from_swap_to_stdout(p, 0);
					break;
				}
			}	
		case 'r':
			if(cmd[3] == 'm' && cmd[4] == '\0'){
				boo_free_storage(1, 0);
				break;
			}
		default:
			fprintf(stdout, "%s : boo not found\n", cmd);
			fflush(stdout);
			break;
	}
}

void boo_shell()
{
	char cmd[512];
	
	fprintf(stdout, "\nWelcome to the Ghost House.\n");
	if(verbose){
		fprintf(stdout, "Comandi disponibili:\n");
		fprintf(stdout, "[swin <file>]\t[swout <file>]\t[swrm]\t[ls]\n");
		fprintf(stdout, "[bbin <file>]\t[bbout <file>]\t[bbrm]\t[exit]\n\n");
	}
	
	while(1){
		memset(cmd, '\0', 500);
		printf("boo> "); fflush(stdout);
		if(fgets(cmd, sizeof(cmd)-1, stdin) == NULL)
			aborto("boo_shell: fgets from stdin");
		else {
			cmd[strlen(cmd)-1] = '\0';
			if(!strcmp(cmd, "exit")) break;
			switch(cmd[0])
			{
				case 's':
					if(cmd[0] == 's' && cmd[1] == 'w'){
						boo_shell_sw_parse(cmd);
						continue;
					}
				case 'b':
					if(cmd[0] == 'b' && cmd[1] == 'b'){
						boo_shell_bb_parse(cmd);
						continue;
					}
				case 'l':
					if(cmd[0] == 'l' && cmd[1] == 's' && cmd[2]== '\0'){
						boo_read_sb();
						continue;
					}
				default:
					fprintf(stdout, "%s : boo not found\n", cmd); fflush(stdout);
					break;
			}
		}
	}
}

int main(int argc, char **argv)
{
	char swapit = 0;
	char badit = 0;
	char listit = 0;
	char cpit = 0;
	char extractit = 0;
	char freeit = 0;
	int opt;
	char *boo_file;
	
	if(geteuid() || getuid()) {
                printf("\nNo root, no party...\n\n");
                exit(0);
        }
    
    if(argc == 1) {
    	uso(argv[0]);
    	exit(0);
    }
       
	while((opt = getopt(argc, argv, "iesblc:fx:vh?")) != -1) {
		switch(opt)
		{
			case 'i':
				shell++;
				break;
			case 'e':
				encrypt++;
				boo_get_rc5_key(boo_rc5_key);
				break;
			case 's':
				swapit++;
				break;
			case 'b':
				badit++;
				break;
			case 'l':
				listit++;
				if(cpit || freeit || extractit)
					boo_opt_use();
				break;
			case 'c':
				cpit++;
				if(listit || freeit || extractit)
					boo_opt_use();
				boo_file = optarg;
				break;
            case 'f':
                freeit++;
                if(cpit || listit || extractit)
				boo_opt_use();
                break;
            case 'x':
               	extractit++;
               	if(cpit || freeit || listit)
				boo_opt_use();
               	boo_file = optarg;
               	break;
            case 'v':
              	verbose++;
               	break;
			case 'h':
			case '?':
				uso(argv[0]);
				exit(0);
				break;
            default:
                exit(0);
                break;
		}
	}
        
    boo_open_devs(swapit, badit);
    
    if(shell){
    	boo_shell();
    	boo_close_devs();
    	exit(0);
    }
        
    if(cpit) {
     	boo_check_space(boo_file, swapit, badit);
       	if(!swapit && badit) {
       		if((len = (boo_alloc_badblocks_tot(st.st_size) *
       			1024<<sb->s_log_block_size) < st.st_size)) {
       				printf("\nboo_alloc_badblocks_tot: not enough space");
       				boo_close_devs();
       				exit(0);
       		}
       	}
        	
       	else if(swapit && !badit) {
       		if((len = (boo_alloc_swap_pages(st.st_size) *
       			BOO_PAGE_SIZE) < st.st_size)) {
       				printf("\nboo_alloc_swap_pages: not enough space");
       				boo_close_devs();
       				exit(0);
       		}
       	}
        	
       	if(!swapit && badit) {
       		boo_copy_in_blocks(boo_file, find_start);
       		boo_sb_ctl_update(find_start, st.st_size);
		}
		
		else if(swapit && !badit) {
			boo_copy_in_swap(boo_file, find_start);
			boo_sw_ctl_update(find_start, st.st_size);
			swapon(SWAPDEV, 0);
		}        		
	}
        
    if(listit)
       	boo_read_sb();
        
    if(extractit) {
       	if(!swapit && badit)
       		boo_xtract_from_blocks_to_stdout(boo_file, 1);
       	if(swapit && !badit)
       		boo_xtract_from_swap_to_stdout(boo_file, 1);
    }
        
    if(freeit)
       	boo_free_storage(swapit, badit);
        
    /* BigBoo fuori. A te, Bowser */
    boo_close_devs();
    exit(0);
}
