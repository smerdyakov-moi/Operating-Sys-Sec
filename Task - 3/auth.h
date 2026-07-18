#ifndef AUTH_H
#define AUTH_H

void initialize_system();
void login_user(char *username,char *password);
void logout_user();
char *current_active_user();

#endif