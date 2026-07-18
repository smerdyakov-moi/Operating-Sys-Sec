#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "globals.h"

bool eval_permissions(int file_index,char mode);

void file_creation(char *filename,char *content,FilePermission perm);

void file_read(char *filename);

void file_write(char *filename,char *new_content);

void file_delete(char *filename);

void file_encrypt(char *filename);

void raw_mem();

#endif