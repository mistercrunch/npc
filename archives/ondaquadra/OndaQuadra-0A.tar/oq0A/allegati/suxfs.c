/*
 * suxfs.c: Semplice filesystem che runna in userspace.
 *
 *
 * Copyright (c) 2003, eazy <eazy@ondaquadra.org>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * *  Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of the Networks Associates Technology, Inc nor the
 *    names of its contributors may be used to endorse or promote
 *    products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS
 * IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define BLOCK_NO 2048
#define INODE_NO 256
#define PTR_BLOCK_NULL BLOCK_NO
#define PTR_INODE_NULL INODE_NO
#define FILENAME_LEN 15
#define PATHNAME_LEN 255
#define DIR_ENTRY 128
#define FILE_PTR_NO 10
#define DATA_SIZE 1024
#define N 100

typedef int     Ptr_Block;
typedef int     Ptr_Inode;

typedef struct {
	double          dimensione;
	Ptr_Block       ptr[FILE_PTR_NO];
	Ptr_Block       ptr_index;
}               File;

typedef union {
	File            file;
	Ptr_Block       dir;
}               Inode_Type;

typedef struct {
	int             flag;
	Inode_Type      type;
}               Inode_Used;

typedef union {
	Ptr_Inode       free;
	Inode_Used      used;
}               Inode;

typedef int     Map[BLOCK_NO];

typedef struct {
	char            filename[FILENAME_LEN];
	Ptr_Inode       inode;
}               Dirent;

typedef char    Data[DATA_SIZE];

typedef union {
	Ptr_Block       indice[N];
	Dirent          dirent[DIR_ENTRY];
	Data            data;
}               Block;

#define PTR_SIZE sizeof(Ptr_Block)
#define BLOCK_SIZE sizeof(Block)
#define INODE_SIZE sizeof(Inode)
#define MAP_SIZE sizeof(Map)


void 
moveToBlock(int fd, Ptr_Block ptr)
{

	lseek(fd, PTR_SIZE + INODE_SIZE * INODE_NO + MAP_SIZE + BLOCK_SIZE * ptr, SEEK_SET);

}

void 
moveToInode(int fd, Ptr_Inode ptr)
{

	lseek(fd, PTR_SIZE + INODE_SIZE * ptr, SEEK_SET);

}

void 
moveToMap(int fd)
{

	lseek(fd, PTR_SIZE + INODE_SIZE * INODE_NO, SEEK_SET);

}

/*
   Restituisce un puntatore al primo data block libero e aggiorna il
   relativo bit nella mappa dei blocchi. Se non sono data block liberi
   restituisce -1
 */

Ptr_Block 
getFreeBlock(int fd)
{

	int             r;
	Ptr_Block       i;
	Map             bitmap;

	moveToMap(fd);
	if ((r = read(fd, bitmap, MAP_SIZE)) <= 0) {
		printf("read error getFreeBlock: %d\n", r);
		exit(0);
	}
	for (i = 0; i < BLOCK_NO; i++)
		if (bitmap[i] == 0)
			break;
	if (i < BLOCK_NO) {
		bitmap[i] = 1;
		moveToMap(fd);
		if (write(fd, bitmap, MAP_SIZE) <= 0) {
			printf("write error\n");
			exit(0);
		}
		return i;
	} else
		return -1;

}

/*
   Restituisce un puntatore al primo inode libero e aggiorna la lista
   degli inode in maniera coerente. Se non ci sono inode liberi
   restituisce -1
 */

Ptr_Inode 
getFreeInode(int fd)
{

	Ptr_Inode       ptr;
	Inode           inode;

	lseek(fd, 0, SEEK_SET);
	if (read(fd, &ptr, PTR_SIZE) <= 0) {
		printf("read error getFreeInode\n");
		exit(0);
	}
	if (ptr != PTR_INODE_NULL) {
		moveToInode(fd, ptr);
		if (read(fd, &inode, INODE_SIZE) <= 0) {
			printf("read error getFreeInode\n");
			exit(0);
		}
		lseek(fd, 0, SEEK_SET);
		if (write(fd, &inode.free, PTR_SIZE) <= 0) {
			printf("write error\n");
			exit(0);
		}
		return ptr;
	} else
		return -1;

}

/*
   Verifica se esiste un dato filename esiste in una directory.
   L'argomento dir specifica l'inode della directory, mentre filename
   e' un puntatore al nome del file di cui eseguire il check
 */

void 
checkIfExist(int fd, Ptr_Inode dir, char *filename)
{

	int             i;
	Inode           inode;
	Block           blocco;

	moveToInode(fd, dir);
	if (read(fd, &inode, INODE_SIZE) <= 0) {
		printf("read error checkIfExist\n");
		exit(0);
	}
	/*
           Verifico che il secondo argomento passato alla funzione sia
           effettivamente una directory, in caso contrario restituisco un errore
           e termino il programma
         */
	if (inode.used.flag == 1) {
		moveToBlock(fd, inode.used.type.dir);
	} else {
		printf("checkIfExist: Impossibile creare il file\n");
		exit(0);
	}
	if (read(fd, &blocco, BLOCK_SIZE) <= 0) {
		printf("read error checkIfExist\n");
		exit(0);
	}
	/*
           Se esiste una directory entry il cui nome corrisponde a quello cercato
           restituisco un messaggio di errore e termino il programma al fine di
           garantire la coerenza del filesystem
         */
	for (i = 0; i < DIR_ENTRY; i++) {
		if (strcmp(blocco.dirent[i].filename, filename) == 0) {
			printf("%s esiste gia'\n", filename);
			exit(0);
		}
	}
}


/*
   Dato in ingresso un pathname assoluto resituisce un puntatore
   all'inode del file specificato da tale pathname. Il terzo argomento
   passato alla funzione consiste in un valore di ritorno che
   restituisce il nome del file privato del proprio path, l'allocazione
   dello spazio necessario ad ospitare il valore di ritorno deve
   avvenire ad opera del chiamante
 */

Ptr_Inode 
getFileFromPath(int fd, char *pathname, char *filename)
{

	int             i;
	Ptr_Inode       ptr = 0;
	char           *str, path[PATHNAME_LEN], token[FILENAME_LEN];
	Block           blocco;
	Inode           inode;

	strncpy(path, pathname, PATHNAME_LEN - 1);

	/*
           Elimina da path la prima occorrenza del carattere slash e ritorna
           il resto della stringa fino al prossimo slash o al fine stringa.
           Se path e' composto da un singolo carattere slash (root directory)
           o e' una stringa nulla la funzione ritorna ptr = 0 ovvero l'inode
           della directory root
         */
	if ((str = strtok(path, "/")) == NULL)
		return ptr;

	strncpy(token, str, FILENAME_LEN - 1);
	moveToInode(fd, ptr);
	if (read(fd, &inode, INODE_SIZE) <= 0) {
		printf("read error getFileFromPath\n");
		exit(0);
	}
	/*
           Verifica che l'inode si riferisca effettivamente ad una directory
         */
	if (inode.used.flag == 1)
		moveToBlock(fd, inode.used.type.dir);
	else {
		printf("Non e' una directory\n");
		exit(0);
	}
	if (read(fd, &blocco, BLOCK_SIZE) <= 0) {
		printf("read error getFileFromPath\n");
		exit(0);
	}
	/*
           Cerca nella directory root se esiste una directory entry relativa
           al token ottenuto dal parsing effettuato da strtok() sul pathname
         */
	for (i = 0; i < DIR_ENTRY; i++) {

		/*
	           Se esiste ne salva il valore dell'inode in ptr
	         */
		if (strcmp(token, blocco.dirent[i].filename) == 0) {
			ptr = blocco.dirent[i].inode;
			break;
		}
	}
	if (i == DIR_ENTRY) {
		printf("getFileFromPath: Path errato\n");
		exit(0);
	}
	/*
           Chiama strtok() fino a che path contiene ulteriori token da parsare e ne
           copia il valore di ritorno nella variabile token.
           All'uscita dal while ptr puntera' all'inode del file che stiamo cercando
           e token ne conterra' il nome
         */
	while ((str = strtok(NULL, "/")) != NULL && strncpy(token, str, FILENAME_LEN - 1)) {
		moveToInode(fd, ptr);
		if (read(fd, &inode, INODE_SIZE) <= 0) {
			printf("read error getFileFromPath\n");
			exit(0);
		}
		if (inode.used.flag == 1)
			moveToBlock(fd, inode.used.type.dir);
		else {
			printf("Non e' una directory\n");
			exit(0);
		}
		if (read(fd, &blocco, BLOCK_SIZE) <= 0) {
			printf("read error getFileFromPath\n");
			exit(0);
		}
		/*
	           Cerca nella directory puntata da ptr se esiste una directory entry relativa
	           al token ottenuto dal parsing effettuato da strtok() sul pathname
	        */
		for (i = 0; i < DIR_ENTRY; i++) {

			/*
		           Se esiste ne salva il valore dell'inode in ptr
		         */
			if (strcmp(token, blocco.dirent[i].filename) == 0) {
				ptr = blocco.dirent[i].inode;
				break;
			}
		}
		if (i == DIR_ENTRY) {
			printf("getFileFromPath: Path errato\n");
			exit(0);
		}
	}

	/*
           Copia nella variabile filename il token corrispondente al file
           specificato da pathname privato del proprio percorso assoluto
         */
	strncpy(filename, token, FILENAME_LEN - 1);

	/*
	 * Vado a NULL terminare la stringa nel caso sia stata troncata a
	 * FILENAME_LEN - 1
	 */
	filename[FILENAME_LEN - 1] = 0;

	return ptr;

}


/*
   Dato in ingresso un pathname assoluto resituisce un puntatore
   all'inode della directory genitore in cui risiede il file specificato
   da tale pathname. Il terzo argomento passato alla funzione consiste in
   un valore di ritorno che restituisce il nome del file privato del
   proprio path, l'allocazione dello spazio necessario ad ospitare il
   valore di ritorno deve avvenire ad opera del chiamante
*/

Ptr_Inode 
getParentFromPath(int fd, char *pathname, char *filename)
{

	int             i;
	Ptr_Inode       ptr = 0;
	char           *str, path[PATHNAME_LEN], token[FILENAME_LEN], token_next[FILENAME_LEN];
	Block           blocco;
	Inode           inode;

	strncpy(path, pathname, PATHNAME_LEN - 1);

	/*
           Elimina da path la prima occorrenza del carattere slash e ritorna
           il resto della stringa fino al prossimo slash o al fine stringa.
           Se path e' composto da un singolo carattere slash (root directory)
           o e' una stringa nulla la funzione ritorna ptr = 0 ovvero l'inode
           della directory root
         */
	if ((str = strtok(path, "/")) == NULL) {
		printf("strtok error\n");
		exit(0);
	}
	/*
           Salva il valore del token ottenuto dalla prima chiamata a strtok()
         */
	strncpy(token, str, FILENAME_LEN - 1);

	/*
           Chiama strtok() fino a che path contiene ulteriori token da parsare e ne
           copia il valore di ritorno nella variabile token_next.
           All'uscita dal while ptr puntera' all'inode della directory genitore del
           file che stiamo cercando e token conterra' il nome di tale file
         */
	while ((str = strtok(NULL, "/")) != NULL && strncpy(token_next, str, FILENAME_LEN - 1)) {
		moveToInode(fd, ptr);
		if (read(fd, &inode, INODE_SIZE) <= 0) {
			printf("read error parsePath\n");
			exit(0);
		}
		if (inode.used.flag == 1)
			moveToBlock(fd, inode.used.type.dir);
		else {
			printf("Non e' una directory\n");
			exit(0);
		}
		if (read(fd, &blocco, BLOCK_SIZE) <= 0) {
			printf("read error parsePath\n");
			exit(0);
		}
		/*
	           Cerca nella directory puntata da ptr se esiste una directory entry relativa
	           al token ottenuto dalla chiamata a strtok() di DUE volte fa.
	           Il valore di ritorno dell'ultima chiamata a strtok() si trova in token_next
	           mentre il valore di ritorno della chiamata precedente a strtok() si trova in
	           token ed e' di tale valore che ci serviamo per il confronto.
	           All'uscita dal while ptr conterra' il valore dell'inode della directory
	           genitore del file specificato da pathname e token conterra' il nome di tale
	           file
	        */
		for (i = 0; i < DIR_ENTRY; i++) {

			/*
		           Se esiste si sposta al valore specificato dal relativo inode
		         */
			if (strcmp(token, blocco.dirent[i].filename) == 0) {
				moveToInode(fd, blocco.dirent[i].inode);
				break;
			}
		}
		if (i < DIR_ENTRY) {
			if (read(fd, &inode, INODE_SIZE) <= 0) {
				printf("read error parsePath\n");
				exit(0);
			}
			/*
		           Il fatto che token_next non sia NULL ci assicura che il valore
		           di token si riferisce per forza di cose ad una directory.
		           Di consegueza verifichiamo che l'inode si riferisca effettivamente
		           ad una directory e ne salviamo il valore in ptr
		         */
			if (inode.used.flag == 1) {
				ptr = blocco.dirent[i].inode;
			} else {
				printf("Non e' una directory\n");
				exit(0);
			}
		} else {
			printf("getParentFromPath: Path errato\n");
			exit(0);
		}

		/*
	           Copiamo in token il valore di token_next ottenuto dall'ultima
	           chiamata a strtok()
	         */
		strncpy(token, token_next, FILENAME_LEN - 1);
	}
	if (filename != NULL) {

		/*
	           Copia nella variabile filename il token corrispondente al file
	           specificato da pathname privato del proprio percorso assoluto
	         */
		strncpy(filename, token, FILENAME_LEN - 1);

		/*
	           Vado a NULL terminare la stringa nel caso sia stata troncata
	           a FILENAME_LEN - 1
	         */
		filename[FILENAME_LEN - 1] = 0;
	}
	return ptr;

}

/*
   Provvede alla creazione di una nuova directory. Viene passato come
   secondo argomento alla funzione un puntatore all'inode della directory
   genitore e restituisce come valore di ritorno un puntatore al proprio
   inode ottenuto tramite una chiamata a getFreeInode()
*/

Ptr_Inode 
writeDirToInode(int fd, Ptr_Inode ptr_parent)
{

	Ptr_Inode       ptr;
	Inode           inode;
	Block           blocco;

	/*
           Inizializza il campo flag (file = 0, directory = 1) e il puntatore
           al data block contentente le directory entry
         */
	inode.used.flag = 1;
	if ((inode.used.type.dir = getFreeBlock(fd)) < 0) {
		printf("No free block\n");
		exit(0);
	}
	/*
           Ottiene un inode libero (se esiste), altrimenti restituisce un errore
           e termina il programma
         */
	if ((ptr = getFreeInode(fd)) < 0) {
		printf("No free inode\n");
		exit(0);
	}
	/*
           Crea le directory entry relative alla directory corrente (.) e alla
           directory genitore (..) e le inizializza ai relativi valori di inode
         */
	bzero(&blocco, sizeof(Block));
	strncpy(blocco.dirent[0].filename, ".", FILENAME_LEN);
	blocco.dirent[0].inode = ptr;
	strncpy(blocco.dirent[1].filename, "..", FILENAME_LEN);
	blocco.dirent[1].inode = ptr_parent;

	/*
           Scrive le modifiche alla lista degli inode e al nuovo data block
           allocato su file
         */
	moveToInode(fd, ptr);
	if (write(fd, &inode, INODE_SIZE) <= 0) {
		printf("write error writeDirToInode\n");
		exit(0);
	}
	moveToBlock(fd, inode.used.type.dir);
	if (write(fd, &blocco, BLOCK_SIZE) <= 0) {
		printf("write error writeDirToInode\n");
		exit(0);
	}
	return ptr;

}

/*
   Rimuove il file o la directory (e tutto il suo contenuto) puntato da
   filename. La variabile parent contiene il valore dell'inode della
   directory genitore nella quale e' contenuto il file o la directory
   da rimuovere
 */

void 
rm(int fd, Ptr_Inode parent, char *filename)
{

	int             i, count;
	Map             map;
	Ptr_Inode       ptr, ptr_first;
	Ptr_Block       parent_block;
	Inode           inode, inode_list[INODE_NO];
	Block           blocco, index, blocco_rm;

	moveToInode(fd, parent);
	if (read(fd, &inode, INODE_SIZE) <= 0) {
		printf("read error rm\n");
		exit(0);
	}
	/*
           Salvo il puntatore al blocco della directory genitore contenente
           la directory entry relativa al file o alla directory da rimuovere
         */
	parent_block = inode.used.type.dir;

	/*
           Controllo che il valore specificato come inode della directory
           genitore si riferisca effettivamente ad una directory
         */
	if (inode.used.flag == 1) {
		moveToBlock(fd, parent_block);
		if (read(fd, &blocco, BLOCK_SIZE) <= 0) {
			printf("read error rm\n");
			exit(0);
		}
		/*
	           Cerco il file da rimuovere tra tutte le directory entry della
	           directory genitore fornita come argomento della funzione.
	         */
		for (i = 0; i < DIR_ENTRY; i++)
			/*
		           Se il file o la directory da rimuovere esiste ne salvo il valore
		           dell'inode e azzero la directory entry relativa a tale file
		         */
			if (strcmp(blocco.dirent[i].filename, filename) == 0) {
				ptr = blocco.dirent[i].inode;
				strncpy(blocco.dirent[i].filename, "", FILENAME_LEN - 1);
				blocco.dirent[i].inode = 0;
				break;
			}
		if (i < DIR_ENTRY) {
			moveToInode(fd, ptr);
			if (read(fd, &inode, INODE_SIZE) <= 0) {
				printf("read error rm\n");
				exit(0);
			}
			/*
		           Verifico se l'elemento da rimuovere e' un file oppure una
		           directory
		         */
			if (inode.used.flag == 0) {	/* se e' un file... */
				moveToMap(fd);
				if (read(fd, &map, MAP_SIZE) <= 0) {
					printf("read error rm\n");
					exit(0);
				}
				/*
			           Vado a marcare come libero nella mask ogni data block puntato
			           dall'indice dei blocchi diretti
			         */
				count = 0;
				while (count < FILE_PTR_NO && inode.used.type.file.ptr[count] != PTR_BLOCK_NULL) {
					map[inode.used.type.file.ptr[count]] = 0;
					count++;
				}

				/*
			           Vado a marcare come libero nella mask ogni data block puntato
			           dall'indice dei blocchi indiretti
			         */
				if (count == FILE_PTR_NO && inode.used.type.file.ptr_index != PTR_BLOCK_NULL) {
					moveToBlock(fd, inode.used.type.file.ptr_index);
					if (read(fd, &index, BLOCK_SIZE) <= 0) {
						printf("read error rm\n");
						exit(0);
					}
					i = 0;
					while (i < N && index.indice[i] != PTR_BLOCK_NULL) {
						map[index.indice[i]] = 0;
						i++;
					}

					/*
			                   Dealloco il data block contenente l'indice ai blocchi
			                   indiretti
			                 */
					map[inode.used.type.file.ptr_index] = 0;
				}
			} else if (inode.used.flag == 1) {	/* altrimenti se e' una
								 * directory... */
				moveToBlock(fd, inode.used.type.dir);
				if (read(fd, &blocco_rm, BLOCK_SIZE) <= 0) {
					printf("read error rm\n");
					exit(0);
				}
				/*
			           Richiamo ricorsivamente la funzione rm() per ogni directory entry
			           della directory da rimuovere. Skippo i valori nulli e quelli relativi
			           alla directory corrente e a quella genitore
			         */
				for (i = 0; i < DIR_ENTRY; i++)
					if (strcmp(blocco_rm.dirent[i].filename, "") != 0 && strcmp(blocco_rm.dirent[i].filename, ".") != 0 &&
					    strcmp(blocco_rm.dirent[i].filename, "..") != 0)
						rm(fd, ptr, blocco_rm.dirent[i].filename);
				moveToMap(fd);
				if (read(fd, &map, MAP_SIZE) <= 0) {
					printf("read error rm\n");
					exit(0);
				}
				/*
			           Vado a marcare come libero il data block relativo alla directory
			           rimossa
			         */
				map[inode.used.type.dir] = 0;
			} else {
				printf("Tipo file non supportato\n");
				exit(0);
			}

			/*
		           Provvedo ad aggiornare in maniera coerente la lista degli inode liberi
		           inserendo l'inode liberatosi in seguito alla rimozione del file
		         */
			lseek(fd, 0, SEEK_SET);
			if (read(fd, &ptr_first, PTR_SIZE) <= 0) {
				printf("read error rm\n");
				exit(0);
			}
			moveToInode(fd, 0);
			if (read(fd, &inode_list, INODE_SIZE * INODE_NO) <= 0) {
				printf("read error rm\n");
				exit(0);
			}
			/*
		           Se l'inode che ho liberato in seguito alla rimozione del file precede
		           il primo inode libero nella lista, eseguo un inserimento in testa...
		         */
			if (ptr < ptr_first) {
				inode_list[ptr].free = ptr_first;
				ptr_first = ptr;
				lseek(fd, 0, SEEK_SET);
				if (write(fd, &ptr_first, PTR_SIZE) <= 0) {
					printf("write error rm\n");
					exit(0);
				}
			}
			/*
		           ...altrimenti eseguo un inserimento in coda
		         */
			else {
				i = ptr_first;
				while (inode_list[i].free < ptr)
					i = inode_list[i].free;
				inode_list[ptr].free = inode_list[i].free;
				inode_list[i].free = ptr;
			}

			/*
		           Scrivo su file le modifiche apportate alla mappa dei data block, alla
		           directory genitore e alla lista degli inode
		         */
			moveToMap(fd);
			if (write(fd, &map, MAP_SIZE) <= 0) {
				printf("write error rm\n");
				exit(0);
			}
			moveToBlock(fd, parent_block);
			if (write(fd, &blocco, BLOCK_SIZE) <= 0) {
				printf("write error rm\n");
				exit(0);
			}
			moveToInode(fd, 0);
			if (write(fd, &inode_list, INODE_SIZE * INODE_NO) <= 0) {
				printf("write error rm\n");
				exit(0);
			}
		} else {
			printf("Impossibile trovare il file o la dir da rimuovere\n");
			exit(0);
		}
	} else {
		printf("Non e' una directory\n");
		exit(0);
	}

}

/*
   Crea una nuova directory entry. Attraverso la variabile dir viene
   passato alla funzione il valore dell'inode della directory in cui
   si vuole creare una entry composta dalla coppia di valori passati
   attraverso *dst e inode_entry. I due volori rappresentano
   rispettivamente il nome del file e l'inode che compongono l'entry
 */

int 
newDirEntry(int fd, Ptr_Inode dir, char *dst, Ptr_Inode inode_entry)
{

	int             i;
	Block           blocco;
	Inode           inode;

	moveToInode(fd, dir);
	if (read(fd, &inode, INODE_SIZE) <= 0) {
		printf("read error newDirEntry\n");
		exit(0);
	}
	/*
	 * Verifico che il valore passato alla funzione si riferisca
	 * effettivamente all'inode di una directory, altrimenti restituisco
	 * un errore e termino il programma
	 */
	if (inode.used.flag == 1) {
		moveToBlock(fd, inode.used.type.dir);
	} else {
		printf("newDirEntry: Impossibile creare il file\n");
		exit(0);
	}
	if (read(fd, &blocco, BLOCK_SIZE) <= 0) {
		printf("read error newDirEntry\n");
		exit(0);
	}
	/*
           Se esiste una directory entry il cui nome corrisponde a quello della
           entry da creare restituisco un messaggio di errore e termino il programma
         */
	for (i = 0; i < DIR_ENTRY; i++) {
		if (strcmp(blocco.dirent[i].filename, dst) == 0) {
			printf("%s esiste gia'\n", dst);
			exit(0);
		}
	}

	/*
           Inizializzo la prima directory entry libera nel genitore con i valori
           passati come argomento della funzione
         */
	for (i = 0; i < DIR_ENTRY; i++) {
		if (strcmp(blocco.dirent[i].filename, "") == 0) {
			strncpy(blocco.dirent[i].filename, dst, FILENAME_LEN);
			blocco.dirent[i].inode = inode_entry;
			break;
		}
	}

	/*
           Se ho trovato una directory entry libera nel genitore scrivo su file
           le modifiche apportate
         */
	if (i < DIR_ENTRY) {
		moveToBlock(fd, inode.used.type.dir);
		if (write(fd, &blocco, BLOCK_SIZE) <= 0) {
			printf("write error\n");
			exit(0);
		}
	} else {
		printf("nessuna entry disponibile nella directory\n");
		exit(0);
	}
}

/*
  Crea la directory specificata da pathname
 */

void 
makedir(int fd, char *pathname)
{

	Ptr_Inode       parent, new_inode;
	char            dirname[FILENAME_LEN];

	if (pathname == NULL) {
		printf("list: argomento non valido\n");
		exit(0);
	}
	if (strncmp(pathname, "VD:", 3) == 0)
		pathname += 3;
	else {
		printf("makedir: Path errato\n");
		exit(0);
	}

	/*
           Ricava da pathname il valore relativo all'inode della directory
           genitore che dovra' ospitare la directory creata
         */
	parent = getParentFromPath(fd, pathname, dirname);	/* dirname is a return
								 * value */

	/*
           Controlla che la directory genitore non contenga gia' un file
           o una directory con lo stesso nome della directory da creare
         */
	checkIfExist(fd, parent, dirname);

	/*
           Alloco l'inode e il data block necessari ad ospitare la nuova
           directory
         */
	new_inode = writeDirToInode(fd, parent);

	/*
           Scrivo nella directory genitore una entry relativa alla directory
           appena creata, contenente il nome della directory e il proprio inode
         */
	newDirEntry(fd, parent, dirname, new_inode);

}

/*
   Se pathaname si riferisce ad un file list() stampa il nome del file,
   altrimenti se si riferisce ad una directory list() stampa ogni directory
   entry contenuta nella directory specificata da pathname
*/

void 
list(int fd, char *pathname)
{

	int             i;
	Ptr_Inode       ptr;
	Inode           inode;
	Block           blocco;
	char            file[FILENAME_LEN];

	if (pathname == NULL) {
		printf("list: argomento non valido\n");
		exit(0);
	}
	if (strncmp(pathname, "VD:", 3) == 0)
		pathname += 3;
	else {
		printf("list: Path errato\n");
		exit(0);
	}

	ptr = getFileFromPath(fd, pathname, file);
	moveToInode(fd, ptr);
	if (read(fd, &inode, INODE_SIZE) <= 0) {
		printf("read error list\n");
		exit(0);
	}
	/*
           Se il pathname si riferisce ad un file printa a schermo
           il nome del file
         */
	if (inode.used.flag == 0)
		printf("%s\n", file);

	/*
           Se il pathname di riferisce ad una directory...
         */
	else if (inode.used.flag == 1) {
		moveToBlock(fd, inode.used.type.dir);
		if (read(fd, &blocco, BLOCK_SIZE) <= 0) {
			printf("read error list\n");
			exit(0);
		}
		/*
	           ...printa a schermo il nome di ogni entry contenuta
	           nella directory specificata da pathname
	         */
		for (i = 0; i < DIR_ENTRY; i++)
			if (strcmp(blocco.dirent[i].filename, "") != 0)
				printf("%s\n", blocco.dirent[i].filename);
	} else {
		printf("list: file type not supported\n");
		exit(0);
	}

}

/*
   Scrive un file sul filesystem virtuale preoccupandosi di allocare
   l'inode e i data block necessari alla sua memorizzazione
 */

Ptr_Inode 
writeFileToInode(int fd_dst, int fd_src)
{

	Inode           inode;
	Data            buf;
	Ptr_Inode       ptr;
	int             k, j, r, i = 0, count = 0, bool = 1;
	Block           blocco[FILE_PTR_NO], index, blocco_index[N];

	bzero(buf, sizeof(Data));
	bzero(&inode, INODE_SIZE);
	bzero(blocco, BLOCK_SIZE * FILE_PTR_NO);
	bzero(blocco_index, BLOCK_SIZE * N);

	/*
           Inizializzo a 0 (file = 0, directory = 1) il campo flag della
           struct Inode_Used e a PTR_BLOCK_NULL sia i puntatori a data block
           diretti che indiretti
         */
	inode.used.flag = 0;
	inode.used.type.file.ptr_index = PTR_BLOCK_NULL;
	for (k = 0; k < FILE_PTR_NO; k++)
		inode.used.type.file.ptr[k] = PTR_BLOCK_NULL;
	for (k = 0; k < N; k++)
		index.indice[k] = PTR_BLOCK_NULL;
	lseek(fd_src, 0, SEEK_SET);

	/*
           Continua a leggere fino a che non raggiunge EOF
         */
	while ((r = read(fd_src, buf, sizeof(Data))) > 0) {

		inode.used.type.file.dimensione += r;

		/*
	           Alloca un data block e inizializza un puntatore diretto
	           a tale blocco
	         */
		if (count < FILE_PTR_NO) {
			if ((inode.used.type.file.ptr[count] = getFreeBlock(fd_dst)) < 0) {
				printf("no free block\n");
				exit(0);
			}
			/*
		           Copia nel data block i dati precedentemente letti dal
		           file tramite la chiamata a read()
		         */
			memcpy(blocco[count].data, buf, sizeof(Data));
			bzero(buf, sizeof(Data));
			count++;
			continue;
		}
		/*
	           Se FILE_PTR_NO puntatori a data block diretti non dovessero
	           bastare ad indirizzare tutti i data block necessari a contenere
	           il file vengono allocati dei puntatori a data block indiretti
	         */
		if (bool && (inode.used.type.file.ptr_index = getFreeBlock(fd_dst)) < 0) {
			printf("no free block\n");
			exit(0);
		}
		bool = 0;

		/*
	           Se N puntatori a data block indiretti non dovessero bastare ad
	           indicizzare tutti i data block necessari a contenere il file
	           visualizza un messaggio di errore e termina il programma
	         */
		if (i == N) {
			printf("file troppo grande\n");
			exit(0);
		}
		/*
	           Alloca un data block e inizializza un puntatore indiretto
	           a tale blocco
	         */
		if ((index.indice[i] = getFreeBlock(fd_dst)) < 0) {
			printf("no free block\n");
			exit(0);
		}
		/*
	           Copia nel data block i dati precedentemente letti dal
	           file tramite la chiamata a read()
	         */
		memcpy(blocco_index[i].data, buf, sizeof(Data));
		bzero(buf, sizeof(Data));
		i++;
	}
	if (r != 0) {
		printf("read error writeFileToInode\n");
		exit(0);
	}
	/*
           Se ho scritto almeno un data block alloco un inode per il file
         */
	if (count != 0) {
		if ((ptr = getFreeInode(fd_dst)) < 0) {
			printf("No free inode\n");
			exit(0);
		}
	} else {
		printf("Nessun dato da leggere\n");
		exit(0);
	}

	/*
           Scrivo su file il mio inode nel punto in cui lo avevo
           precedentemente allocato
         */
	moveToInode(fd_dst, ptr);
	if (write(fd_dst, &inode, INODE_SIZE) <= 0) {
		printf("write error\n");
		exit(0);
	}
	/*
           Scrivo su file i data block relativi ai puntatori diretti
         */
	j = 0;
	while (j < FILE_PTR_NO && inode.used.type.file.ptr[j] != PTR_BLOCK_NULL) {
		moveToBlock(fd_dst, inode.used.type.file.ptr[j]);
		if (write(fd_dst, &blocco[j], BLOCK_SIZE) <= 0) {
			printf("write error\n");
			exit(0);
		}
		j++;
	}

	/*
           Scrivo su file l'indice dei puntatori indiretti
         */
	if (inode.used.type.file.ptr_index != PTR_BLOCK_NULL) {
		moveToBlock(fd_dst, inode.used.type.file.ptr_index);
		if (write(fd_dst, &index, BLOCK_SIZE) <= 0) {
			printf("write error\n");
			exit(0);
		}
		/*
	           Scrivo su file i data block relativi ai puntatori indiretti
	         */
		j = 0;
		while (j < N && index.indice[j] != PTR_BLOCK_NULL) {
			moveToBlock(fd_dst, index.indice[j]);
			if (write(fd_dst, &blocco_index[j], BLOCK_SIZE) <= 0) {
				printf("write error\n");
				exit(0);
			}
			j++;
		}
	}
	return ptr;
}

/*
   Esporta il contenuto di un file memorizzato nel filesystem virtuale
   in un file specificato dall'utente
 */

void 
writeInodeToFile(int dbfd, Ptr_Inode dir, char *filename, int fd)
{

	int             i, count, dim;
	Inode           dir_inode, inode;
	Block           blocco, index;

	moveToInode(dbfd, dir);
	if (read(dbfd, &dir_inode, INODE_SIZE) <= 0) {
		printf("read error writeInodeToFile\n");
		exit(0);
	}
	/*
           Verifico che il valore passato alla funzione si riferisca
           effettivamente all'inode di una directory, altrimenti restituisco
           un errore e termino il programma
         */
	if (dir_inode.used.flag == 1) {
		moveToBlock(dbfd, dir_inode.used.type.dir);
	} else {
		printf("Impossibile leggere la directory\n");
		exit(0);
	}
	bzero(&blocco, BLOCK_SIZE);
	if (read(dbfd, &blocco, BLOCK_SIZE) <= 0) {
		printf("read error writeInodeToFile\n");
		exit(0);
	}
	/*
           Cerco il file da copiare tra tutte le directory entry della
           directory genitore fornita come argomento della funzione
         */
	for (i = 0; i < DIR_ENTRY; i++)
		/*
	           Se lo trovo mi sposto al relativo inode per effettuare
	           una lettura
	         */
		if (strcmp(blocco.dirent[i].filename, filename) == 0) {
			moveToInode(dbfd, blocco.dirent[i].inode);
			break;
		}
	if (i < DIR_ENTRY) {
		if (read(dbfd, &inode, INODE_SIZE) <= 0) {
			printf("read error writeInodeToFile\n");
			exit(0);
		}
		/*
	           Verifico che l'inode si riferisca ad un file
	         */
		if (inode.used.flag == 0) {
			dim = inode.used.type.file.dimensione;

			/*
		           Scrivo sul file il contenuto dei data block relativi ai
		           puntatori diretti
		         */
			count = 0;
			while (count < FILE_PTR_NO && inode.used.type.file.ptr[count] != PTR_BLOCK_NULL && dim > 0) {
				moveToBlock(dbfd, inode.used.type.file.ptr[count]);
				bzero(&blocco, BLOCK_SIZE);
				if (read(dbfd, &blocco, BLOCK_SIZE) <= 0) {
					printf("read error writeInodeToFile\n");
					exit(0);
				}
				if (dim >= sizeof(Data)) {
					if (write(fd, &blocco.data, sizeof(Data)) <= 0) {
						printf("write error writeInodeToFile\n");
						exit(0);
					}
					dim -= sizeof(Data);
				} else {
					if (write(fd, &blocco.data, dim) <= 0) {
						printf("write error writeInodeToFile\n");
						exit(0);
					}
					dim = 0;
				}
				count++;
			}

			/*
		           Se non ci sono puntatori diretti inutilizzati ed e' stato
		           allocato un indice dei puntatori indiretti, scrivo sul file
		           il contenuto dei data block indicizzati dai puntatori
		           indiretti
		         */
			if (count == FILE_PTR_NO && inode.used.type.file.ptr_index != PTR_BLOCK_NULL && dim > 0) {
				moveToBlock(dbfd, inode.used.type.file.ptr_index);
				if (read(dbfd, &index, BLOCK_SIZE) <= 0) {
					printf("read error writeInodeToFile\n");
					exit(0);
				}
				i = 0;
				while (i < N && index.indice[i] != PTR_BLOCK_NULL && dim > 0) {
					moveToBlock(dbfd, index.indice[i]);
					if (read(dbfd, &blocco, BLOCK_SIZE) <= 0) {
						printf("read error writeInodeToFile\n");
						exit(0);
					}
					if (dim >= sizeof(Data)) {
						if (write(fd, &blocco.data, sizeof(Data)) <= 0) {
							printf("write error writeInodeToFile\n");
							exit(0);
						}
						dim -= sizeof(Data);
					} else {
						if (write(fd, &blocco.data, dim) <= 0) {
							printf("write error writeInodeToFile\n");
							exit(0);
						}
						dim = 0;
					}
					i++;
				}
			}
		} else {
			printf("Impossibile leggere il file\n");
			exit(0);
		}
	} else {
		printf("File inesistente\n");
		exit(0);
	}
}

/*
   Gestisce la copia di file da e verso il filesystem virtuale
 */

int 
copy(char *dbfile, char *src, char *dst)
{

	int             fd, fs;
	char            file[FILENAME_LEN], src_token[4], dst_token[4];
	Ptr_Inode       inode, dir;

	if (src == NULL || dst == NULL) {
		printf("copy: argomento non valido\n");
		exit(0);
	}
	if ((fs = open(dbfile, O_RDWR)) <= 0) {
		printf("open error\n");
		exit(0);
	}
	/*
           Se la sorgente e' il filesystem virtuale scrivi sul file specificato...
         */
	if (strncmp(src, "VD:", 3) == 0) {
		src += 3;
		if ((fd = open(dst, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU)) <= 0) {
			printf("open error\n");
			exit(0);
		}
		/*
	           Ricava la directory genitore e il nome del file da esportare
	           dal filesystem virtuale
	         */
		dir = getParentFromPath(fs, src, file);

		/*
	           Copia il contenuto del file contenuto nel filesystem virtuale
	           su un file di destinazione
	         */
		writeInodeToFile(fs, dir, file, fd);
	}
	/*
           ...altrimenti copia il file specificato nel filesystem virtuale
         */
	else if (strncmp(dst, "VD:", 3) == 0) {
		dst += 3;
		if ((fd = open(src, O_RDONLY)) <= 0) {
			printf("open error\n");
			exit(0);
		}
		/*
	           Ricava la directory genitore e il nome del file di destinazione
	           della copia
	         */
		dir = getParentFromPath(fs, dst, file);

		/*
	           Controllo se esiste un file con lo stesso nome e interrompo la copia
	           nel caso la destinazione contenga gia' una voce per tale file.
	           Il controllo se pur presente nella funzione newDirEntry() viene eseguito
	           anticipatamente per inibire l'allocazione di inode e data block che poi
	           risulterebbero inutilizzati
	         */
		checkIfExist(fs, dir, file);

		/*
	           Scrive il contenuto del file nel filsystem virtuale
	         */
		inode = writeFileToInode(fs, fd);

		/*
	           Aggiunge una directory entry per il file appena creato nella directory
	           restituita come valore di ritorno da getParentFromPath()
	         */
		newDirEntry(fs, dir, file, inode);
	} else {
		printf("cp: argomenti comando errati\n");
		exit(0);
	}

}

/*
   Provvede all'inizializzazione delle strutture fondamentali
   del filesystem
 */

int 
init(char *file)
{

	int             fd, i;
	Ptr_Inode       free_inode;
	Map             map;
	Inode           inode[INODE_NO];
	Block           blocchi[BLOCK_NO];

	if ((fd = open(file, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU)) < 0) {
		printf("open error\n");
		exit(0);
	}
	/*
           Inizializzo il puntatore al primo inode libero.
           inode[0] contiene il puntatore al directory block relativo alla
           root directory e risulta pertanto occupato, inode[1] e' invece
           libero
         */
	free_inode = 1;
	lseek(fd, 0, SEEK_SET);
	if (write(fd, &free_inode, PTR_SIZE) < 0) {
		printf("write error\n");
		exit(0);
	}
	/*
           Inizializzo la lista degli inode liberi
         */
	bzero(inode, INODE_SIZE * INODE_NO);
	for (i = 0; i < INODE_NO; i++)
		inode[i].free = i + 1;

	/*
          Faccio puntare il primo inode al primo data block che conterra'
          le directory entry relative alla root directory
         */
	inode[0].used.flag = 1;
	inode[0].used.type.dir = 0;
	if (write(fd, inode, INODE_SIZE * INODE_NO) < 0) {
		printf("write error\n");
		exit(0);
	}
	/*
           Inizializzo a 0 il bitmap e pongo a 1 il bit relativo alla root dir
         */
	bzero(map, MAP_SIZE);
	map[0] = 1;
	if (write(fd, map, MAP_SIZE) < 0) {
		printf("write error\n");
		exit(0);
	}
	/*
           Inizializzo il directory block relativo alla root directory.
           Inserisco le entry relative alla current e alla parent dir
         */
	bzero(blocchi, BLOCK_SIZE * BLOCK_NO);
	strncpy(blocchi[0].dirent[0].filename, ".", FILENAME_LEN - 1);
	blocchi[0].dirent[0].inode = 0;
	strncpy(blocchi[0].dirent[1].filename, "..", FILENAME_LEN - 1);
	blocchi[0].dirent[1].inode = 0;

	/*
           Inizializzo i restanti data block del filesystem
         */
	if (write(fd, blocchi, BLOCK_SIZE * BLOCK_NO) < 0) {
		printf("write error\n");
		exit(0);
	}
}

void 
print_usage(char *prog)
{

	printf("Usage: %s [ -h ] [ -i ] [ -c [VD:]/path [VD:]/path ]", prog);
	printf(" [ -l VD:/path ] [ -m VD:/path ] [ -r VD:/path ]\n");
	printf("\t-h questo help\n");
	printf("\t-i inizializza il filesystem\n");
	printf("\t-c copia un file nel/dal filesystem\n");
	printf("\t-l elenca i file presenti nella directory\n");
	printf("\t-m crea una nuova directory\n");
	printf("\t-r rimuove il file o la directory e tutto il suo contenuto\n");

}

int 
main(int argc, char **argv)
{

	int             fd, i;
	Ptr_Inode       parent;
	Block           blocco;
	char            opt, filename[FILENAME_LEN];

	opterr = 0;
	if ((opt = getopt(argc, argv, "chilmr")) != -1) {
		switch (opt) {
		case 'c':
			argc -= optind;
			argv += optind;
			copy("dbfs", argv[0], argv[1]);
			break;
		case 'h':
			print_usage(argv[0]);
			break;
		case 'i':
			init("dbfs");
			break;
		case 'l':
			argc -= optind;
			argv += optind;
			if ((fd = open("dbfs", O_RDONLY)) < 0) {
				printf("open error\n");
				exit(0);
			}
			list(fd, argv[0]);
			break;
		case 'm':
			argc -= optind;
			argv += optind;
			if ((fd = open("dbfs", O_RDWR)) < 0) {
				printf("open error\n");
				exit(0);
			}
			makedir(fd, argv[0]);
			break;
		case 'r':
			argc -= optind;
			argv += optind;
			if ((fd = open("dbfs", O_RDWR)) < 0) {
				printf("open error\n");
				exit(0);
			}
			if (argv[0] == NULL) {
				printf("rm: argomento non valido\n");
				exit(0);
			}
			if (strncmp(argv[0], "VD:", 3) == 0)
				argv[0] += 3;
			else {
				printf("rm: Path errato\n");
				exit(0);
			}

			parent = getParentFromPath(fd, argv[0], filename);
			rm(fd, parent, filename);
			break;
		default:
			print_usage(argv[0]);
		}
	} else
		print_usage(argv[0]);

}
