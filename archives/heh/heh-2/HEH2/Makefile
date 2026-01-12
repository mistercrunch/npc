# version 0.1
SHELL  = /bin/bash

CC = gcc
LIBS = -lncurses 
all: heh

heh:viewer.c
	@echo "# # # # # # # # # # # # # # # # # # #"
	@echo " # # # # # HEH! Viewer 1.2 # # # # # "
	@echo "# # # # # # # # # # # # # # # # # # #"
	@echo "     por Zomba (zombaq@ciudad.com.ar)"	
	@echo " "
	$(CC) viewer.c  $(LIBS) -o viewer
	$(CC) extract.c -o extract
	@echo " "
	@echo "Relajate, pone ./viewer y empeza a gozar..."