#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<stdbool.h>

//Structure detailing permissions holding basic true/false flags for file access
struct Permission{
    bool read;
    bool write;
    bool execute;
};

//Grouping permissions into 3 identities: Owner (Primarily root priviliges), Group and Others   
struct FilePermission{
    struct Permission owner;
    struct Permission group;
    struct Permission others;
};


int main(){

    return 0;
}
