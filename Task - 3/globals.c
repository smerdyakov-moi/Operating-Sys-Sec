#include "globals.h"

int next_uid = 1001;

User users[MAX_USERS];
int totalUsers = 0;

VirtualFile files[MAX_FILES];
int totalFiles = 0;

int current_uid = -1;
int current_groupid = -1;