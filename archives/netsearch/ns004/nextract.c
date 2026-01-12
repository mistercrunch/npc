/*  nextract.c
 *
 *  Programa original:  phrack staff (r)
 *  Modificacion CRC32: cafo
 *  Modificacion MD5 :  cafo
 *  Modificacion Split: cafo
 *
 *  El programa extrae archivos incluidos (codigos fuente y demas) incluidos
 *  dentro del e-zine. 
 *
 *  Con el modificador [-s] se separaran los articulos del e-zine en archivos
 *  separados numerados.
 *
 *  Compilacion: 
 *  gcc -o nextract nextract.c
 *
 *  ./nextract [-s] file1 file2 file3 ...
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <dirent.h>

#define BEGIN_TAG   "<++> "
#define END_TAG     "<-->"
#define BT_SIZE     strlen(BEGIN_TAG)
#define ET_SIZE     strlen(END_TAG)
#define SPLIT_TAG  "=-[ 0x"
#define SPLIT_SIZE strlen(SPLIT_TAG)

struct f_name
{
    unsigned char name[256];
    struct f_name *next;
};

#ifdef __alpha
typedef unsigned int uint32;
#else
typedef unsigned long uint32;
#endif

struct MD5Context
{
   uint32 buf[4];
   uint32 bits[2];
   unsigned char in[64];
};

void MD5Init(struct MD5Context *context);
void MD5Update(struct MD5Context *context, unsigned char const *buf,
               unsigned len);
void MD5Final(unsigned char digest[16], struct MD5Context *context);
void MD5Transform(uint32 buf[4], uint32 const in[16]);

typedef struct MD5Context MD5_CTX;

#ifndef HIGHFIRST
#define byteReverse(buf, len)
#else
void byteReverse(unsigned char *buf, unsigned longs);

#ifndef ASM_MD5
void byteReverse(unsigned char *buf, unsigned longs)
{
    uint32 t;
    do {
        t = (uint32) ((unsigned) buf[3] << 8 | buf[2]) << 16 |
            ((unsigned) buf[1] << 8 | buf[0]);
        *(uint32 *) buf = t;
        buf += 4;
    } while (--longs);
}
#endif
#endif

void MD5Init(struct MD5Context *ctx)
{
    ctx->buf[0] = 0x67452301;
    ctx->buf[1] = 0xefcdab89;
    ctx->buf[2] = 0x98badcfe;
    ctx->buf[3] = 0x10325476;

    ctx->bits[0] = 0;
    ctx->bits[1] = 0;
}

void MD5Update(struct MD5Context *ctx, unsigned char const *buf, unsigned len)
{
    uint32 t;

    t = ctx->bits[0];
    if ((ctx->bits[0] = t + ((uint32) len << 3)) < t)
        ctx->bits[1]++;
    ctx->bits[1] += len >> 29;

    t = (t >> 3) & 0x3f;


    if (t) {
        unsigned char *p = (unsigned char *) ctx->in + t;

        t = 64 - t;
        if (len < t) {
            memcpy(p, buf, len);
            return;
        }
        memcpy(p, buf, t);
        byteReverse(ctx->in, 16);
        MD5Transform(ctx->buf, (uint32 *) ctx->in);
        buf += t;
        len -= t;
    }

    while (len >= 64) {
        memcpy(ctx->in, buf, 64);
        byteReverse(ctx->in, 16);
        MD5Transform(ctx->buf, (uint32 *) ctx->in);
        buf += 64;
        len -= 64;
    }

    memcpy(ctx->in, buf, len);
}

void MD5Final(unsigned char digest[16], struct MD5Context *ctx)
{
    unsigned count;
    unsigned char *p;

    count = (ctx->bits[0] >> 3) & 0x3F;

    p = ctx->in + count;
    *p++ = 0x80;

    count = 64 - 1 - count;

    if (count < 8) {

        memset(p, 0, count);
        byteReverse(ctx->in, 16);
        MD5Transform(ctx->buf, (uint32 *) ctx->in);

        memset(ctx->in, 0, 56);
    } else {
        memset(p, 0, count - 8);
    }
    byteReverse(ctx->in, 14);

    ((uint32 *) ctx->in)[14] = ctx->bits[0];
    ((uint32 *) ctx->in)[15] = ctx->bits[1];

    MD5Transform(ctx->buf, (uint32 *) ctx->in);
    byteReverse((unsigned char *) ctx->buf, 4);
    memcpy(digest, ctx->buf, 16);
    memset(ctx, 0, sizeof(ctx));
}

#ifndef ASM_MD5

#define F1(x, y, z) (z ^ (x & (y ^ z)))
#define F2(x, y, z) F1(z, x, y)
#define F3(x, y, z) (x ^ y ^ z)
#define F4(x, y, z) (y ^ (x | ~z))

#define MD5STEP(f, w, x, y, z, data, s) ( w += f(x, y, z) + data,  w = w<<s | w>>(32-s),  w += x )

void MD5Transform(uint32 buffer[4], uint32 const in[16])
{
    register uint32 a, b, c, d;

    a = buffer[0];
    b = buffer[1];
    c = buffer[2];
    d = buffer[3];

    MD5STEP(F1, a, b, c, d, in[0] + 0xd76aa478, 7);
    MD5STEP(F1, d, a, b, c, in[1] + 0xe8c7b756, 12);
    MD5STEP(F1, c, d, a, b, in[2] + 0x242070db, 17);
    MD5STEP(F1, b, c, d, a, in[3] + 0xc1bdceee, 22);
    MD5STEP(F1, a, b, c, d, in[4] + 0xf57c0faf, 7);
    MD5STEP(F1, d, a, b, c, in[5] + 0x4787c62a, 12);
    MD5STEP(F1, c, d, a, b, in[6] + 0xa8304613, 17);
    MD5STEP(F1, b, c, d, a, in[7] + 0xfd469501, 22);
    MD5STEP(F1, a, b, c, d, in[8] + 0x698098d8, 7);
    MD5STEP(F1, d, a, b, c, in[9] + 0x8b44f7af, 12);
    MD5STEP(F1, c, d, a, b, in[10] + 0xffff5bb1, 17);
    MD5STEP(F1, b, c, d, a, in[11] + 0x895cd7be, 22);
    MD5STEP(F1, a, b, c, d, in[12] + 0x6b901122, 7);
    MD5STEP(F1, d, a, b, c, in[13] + 0xfd987193, 12);
    MD5STEP(F1, c, d, a, b, in[14] + 0xa679438e, 17);
    MD5STEP(F1, b, c, d, a, in[15] + 0x49b40821, 22);

    MD5STEP(F2, a, b, c, d, in[1] + 0xf61e2562, 5);
    MD5STEP(F2, d, a, b, c, in[6] + 0xc040b340, 9);
    MD5STEP(F2, c, d, a, b, in[11] + 0x265e5a51, 14);
    MD5STEP(F2, b, c, d, a, in[0] + 0xe9b6c7aa, 20);
    MD5STEP(F2, a, b, c, d, in[5] + 0xd62f105d, 5);
    MD5STEP(F2, d, a, b, c, in[10] + 0x02441453, 9);
    MD5STEP(F2, c, d, a, b, in[15] + 0xd8a1e681, 14);
    MD5STEP(F2, b, c, d, a, in[4] + 0xe7d3fbc8, 20);
    MD5STEP(F2, a, b, c, d, in[9] + 0x21e1cde6, 5);
    MD5STEP(F2, d, a, b, c, in[14] + 0xc33707d6, 9);
    MD5STEP(F2, c, d, a, b, in[3] + 0xf4d50d87, 14);
    MD5STEP(F2, b, c, d, a, in[8] + 0x455a14ed, 20);
    MD5STEP(F2, a, b, c, d, in[13] + 0xa9e3e905, 5);
    MD5STEP(F2, d, a, b, c, in[2] + 0xfcefa3f8, 9);
    MD5STEP(F2, c, d, a, b, in[7] + 0x676f02d9, 14);
    MD5STEP(F2, b, c, d, a, in[12] + 0x8d2a4c8a, 20);

    MD5STEP(F3, a, b, c, d, in[5] + 0xfffa3942, 4);
    MD5STEP(F3, d, a, b, c, in[8] + 0x8771f681, 11);
    MD5STEP(F3, c, d, a, b, in[11] + 0x6d9d6122, 16);
    MD5STEP(F3, b, c, d, a, in[14] + 0xfde5380c, 23);
    MD5STEP(F3, a, b, c, d, in[1] + 0xa4beea44, 4);
    MD5STEP(F3, d, a, b, c, in[4] + 0x4bdecfa9, 11);
    MD5STEP(F3, c, d, a, b, in[7] + 0xf6bb4b60, 16);
    MD5STEP(F3, b, c, d, a, in[10] + 0xbebfbc70, 23);
    MD5STEP(F3, a, b, c, d, in[13] + 0x289b7ec6, 4);
    MD5STEP(F3, d, a, b, c, in[0] + 0xeaa127fa, 11);
    MD5STEP(F3, c, d, a, b, in[3] + 0xd4ef3085, 16);
    MD5STEP(F3, b, c, d, a, in[6] + 0x04881d05, 23);
    MD5STEP(F3, a, b, c, d, in[9] + 0xd9d4d039, 4);
    MD5STEP(F3, d, a, b, c, in[12] + 0xe6db99e5, 11);
    MD5STEP(F3, c, d, a, b, in[15] + 0x1fa27cf8, 16);
    MD5STEP(F3, b, c, d, a, in[2] + 0xc4ac5665, 23);

    MD5STEP(F4, a, b, c, d, in[0] + 0xf4292244, 6);
    MD5STEP(F4, d, a, b, c, in[7] + 0x432aff97, 10);
    MD5STEP(F4, c, d, a, b, in[14] + 0xab9423a7, 15);
    MD5STEP(F4, b, c, d, a, in[5] + 0xfc93a039, 21);
    MD5STEP(F4, a, b, c, d, in[12] + 0x655b59c3, 6);
    MD5STEP(F4, d, a, b, c, in[3] + 0x8f0ccc92, 10);
    MD5STEP(F4, c, d, a, b, in[10] + 0xffeff47d, 15);
    MD5STEP(F4, b, c, d, a, in[1] + 0x85845dd1, 21);
    MD5STEP(F4, a, b, c, d, in[8] + 0x6fa87e4f, 6);
    MD5STEP(F4, d, a, b, c, in[15] + 0xfe2ce6e0, 10);
    MD5STEP(F4, c, d, a, b, in[6] + 0xa3014314, 15);
    MD5STEP(F4, b, c, d, a, in[13] + 0x4e0811a1, 21);
    MD5STEP(F4, a, b, c, d, in[4] + 0xf7537e82, 6);
    MD5STEP(F4, d, a, b, c, in[11] + 0xbd3af235, 10);
    MD5STEP(F4, c, d, a, b, in[2] + 0x2ad7d2bb, 15);
    MD5STEP(F4, b, c, d, a, in[9] + 0xeb86d391, 21);

    buffer[0] += a;
    buffer[1] += b;
    buffer[2] += c;
    buffer[3] += d;
}

#endif

int hex_digit(int c)
{
        if (c >= '0' && c <= '9')
                return c - '0';
        if (c >= 'a' && c <= 'f')
           return c - 'a' + 10;
        return -1;
}

int mdfile(FILE *archivo, unsigned char *digest)
{
        unsigned char buffer[1024];
        MD5_CTX ctx;
        int n;


        fseek(archivo,0,SEEK_SET);
        MD5Init(&ctx);
        while ((n = fread(buffer, 1, sizeof(buffer), archivo)) > 0)
                MD5Update(&ctx, buffer, n);
        MD5Final(digest, &ctx);
        if (ferror(archivo))
                return -1;
        return 0;
}

unsigned long crcTable[256];

void crcgen()
{
  unsigned long crc, poly;
  int i, j;
  poly = 0xEDB88320L;
  for (i=0; i<256; i++) {
     crc = i;
     for (j=8; j>0; j--) {
       if (crc&1) {
           crc = (crc >> 1) ^ poly;
       }
       else {
           crc >>= 1;
       }
     }
     crcTable[i] = crc;
  }
}

unsigned long check_crc( FILE *fp )
{
   register unsigned long crc;
   int c;

   crc = 0xFFFFFFFF;
   while( (c=getc(fp)) != EOF ) {
         crc = ((crc>>8) & 0x00FFFFFF) ^ crcTable[ (crc^c) & 0xFF ];
   }

   fseek(fp,0,SEEK_SET);

   return( crc^0xFFFFFFFF );

}

void tmp_digest(unsigned char *p, unsigned char *digest_tmp)
{
        int i;

        for (i = 0; i < 16; ++i)
           sprintf(&digest_tmp[i*2],"%02x", *p++);
}

int main(int argc, char **argv)
{
    unsigned char b[256], *bp, *fn, *tmp;
    unsigned char digest_f[32];
    int i, i2=1, j = 0, h_c = 0, k,h_md5,d1=0,d2=0,modo_split=0;
    unsigned long crc=0, crc_f=0;
    unsigned char digest[16], digest_tmp[32];
    FILE *in_p, *out_p = NULL, *separado;
    struct f_name *fn_p = NULL, *head = NULL;

    if (argc < 2)
      {
        printf("Uso: %s [-s] archivo1 archivo2 ... archivo-n\n", argv[0]);
        exit(0);
      }

    if((argv[1][0] == '-')&&(argv[1][1]=='s')&&(strlen(argv[1])<3))
    {
      modo_split=1;
      i2=2;
    }       
        
    for (i = i2; (fn = argv[i++]); )
      {
        if (!head)
	  {
            if (!(head = (struct f_name *)malloc(sizeof(struct f_name))))
	      {
                perror("malloc");
                exit(1);
	      }
            strncpy(head->name, fn, sizeof(head->name));
            head->next = NULL;
            fn_p = head;
	  }

        else
	  {
            if (!(fn_p->next = (struct f_name *)malloc(sizeof(struct f_name))))
	      {
                perror("malloc");
                exit(1);
	      }
            fn_p = fn_p->next;
            strncpy(fn_p->name, fn, sizeof(fn_p->name));
            fn_p->next = NULL;
	  }
      }

    if (!(fn_p->next = (struct f_name *)malloc(sizeof(struct f_name))))
      {
        perror("malloc");
        exit(1);
      }
    fn_p = fn_p->next;
    fn_p->next = NULL;

if(modo_split==1)
{
      for(fn_p = head; fn_p -> next;fn_p = fn_p->next)
      {
      	if(!(in_p=fopen(fn_p->name,"r")))
        {
          fprintf(stderr,"No se puede abrir el archivo original especificado: %s\n",fn_p->name);
          continue;
        }
        else
        { 
	  fprintf(stderr, "%s abierto.\n", fn_p->name);
        
        tmp=fn_p->name;

	separado=fopen("0x00.txt","w");
	fprintf(stderr,"Separando 0x00.txt\n");

        while(fgets(b,256,in_p))
        {
          if(!strncmp(b,SPLIT_TAG,SPLIT_SIZE))
          {
            b[strlen(b) -1 ]=0;

            if((bp=strchr(b + SPLIT_SIZE + 1,' ')))
            { 
              *bp=0;
	      if((separado=fopen(strncat(b + SPLIT_SIZE-2,".txt",4),"w"))==NULL)
              {
              	printf("No se puede crear %s\n",b);
                continue;
		}
              fprintf(stderr,"Separando %s\n",b +SPLIT_SIZE-2);
            }
	    *bp='.';
            fprintf(separado,"%s",b);
            fprintf(separado," ]-===============================================================\n");
          }
          else
              fprintf(separado,"%s",b);
	}
      }
    }
}
else
{
    for (fn_p = head; fn_p->next; fn_p = fn_p->next)
    {
        if (!(in_p = fopen(fn_p->name, "r")))
        {
            fprintf(stderr, "No se puede abrir el archivo original %s.\n", fn_p->name);
            continue;
        }
        else fprintf(stderr, "%s abierto\n", fn_p->name);

        crcgen();

        while (fgets(b, 256, in_p))
        {
             if (!strncmp (b, BEGIN_TAG, BT_SIZE))
             {
                b[strlen(b) - 1] = 0;
                j++;

                /* crc=0;
                   crc_f=0;    X-DDD */

                if ((bp = strchr(b + BT_SIZE + 1, '/')))
                {
                    while (bp)
                    {
                        *bp = 0;
                        mkdir(b + BT_SIZE, 0700);
                        *bp = '/';
                        bp = strchr(bp + 1, '/');
                    }
                }


                if((bp=strchr(b,'!')))
                {
                   crc_f=strtoul((b + (strlen(b)-strlen(bp)) + 1),NULL,16);
                   b[strlen(b) - strlen(bp) - 1 ]=0;
                   h_c=1;
                   h_md5=0;
                }
                else
                {
                   h_c=0;
                   if((bp=strchr(b,'$')))
                   {
                        d2=0;
                        if(strlen(bp) < 33)
                           d2=1;
                        memcpy(&digest_f,bp+1,32);
			digest_f[32]='\0';
                        b[strlen(b) - strlen(bp) - 1] =0;
                        h_md5=1;
                   }
                   else
                   {
                      h_c=0;
                      h_md5=0;
                   }
                }

                if ((out_p = fopen(b + BT_SIZE, "wb+")))
                   printf("- Extrayendo %s\n ", b + BT_SIZE);
                else
                {
                    printf("No se puede extraer '%s'.\n", b + BT_SIZE);
                    continue;
                }

             }

             else if (!strncmp (b, END_TAG, ET_SIZE))
             {
                if (out_p)
                {
                    if(h_c==1)
                    {
                        fseek(out_p,0l,0);
                        crc=check_crc(out_p);

                        if(crc==crc_f)
                           printf("            crc32 = %08lx  ... comprobado\n ",crc);
                        else
                           printf("            crc32 -> %08lx - %08lx  no coincide!\n ",crc,crc_f);

                    }
                    if(h_md5==1)
                    {
                        mdfile(out_p,digest);
                        tmp_digest(digest,digest_tmp);

                        for(k=0;k<32;k++)
                        {
                           if(digest_f[k] != digest_tmp[k])
                              d1=1;
                        }
                        if(d1==0)
                           printf(" md5 : %s -- ok!\n",digest_f);
                        else
                        {
                           if(d2==0)
                                printf("  md5! %s  <-- md5 suministrado.\n",digest_f);
                           else
                                printf(" md5!  Codigo MD5 suministrado incompleto.\n");
                           printf("        %s  <-- md5 correcto.\n",digest_tmp);
			}   
                        d1=0;
                    }
                    /* nos vamos */
                    fclose(out_p);

                }
                else
                {
                    fprintf(stderr, "Error al cerrar el archivo %s.\n", fn_p->name);
                    continue;
                }
            }
            else if (out_p)
            {
                fputs(b, out_p);
            }
        }
    }
    if (!j) printf("No se han encontrado etiquetas de extraccion.\n");
    else printf("Extraidos %d archivo(s).\n", j);
    return (0);
}
}
