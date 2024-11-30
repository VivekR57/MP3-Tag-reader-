// view.h
#ifndef VIEW_H
#define VIEW_H

#include <stdio.h>
#include "type.h"

typedef struct
{
    char *Filename;
    FILE *fptr_fname;

    char *Title;

    char *Album;

    char *Artist;

    char *Year;

    char *Genre;

    char *Comment;
} Music;

OperationType check_operation_type(char *argv);
status read_and_validate(int argc, char *argv[], Music *Music);
void printHelp();
status viewInfo(Music *music);
status openFiles(Music *music);
status checkheaderandversion(FILE *fname);
status read_info(Music *music, const char *tag);
void little_to_big(char *ptr, int size);

#endif // VIEW_H
