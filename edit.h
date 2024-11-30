#ifndef EDIT_H
#define EDIT_H

#include "type.h"

typedef struct
{
    char *src_fname;
    FILE *fptr_src;

    char *out_fname;
    FILE *fptr_out;

    char *modify_data;
    int data_length;
    char *frame;


    int size;
} Edit_info;

status read_and_validate_edit(char *argv[], Edit_info *mp3_edit);
status check_frame(Edit_info *mp3_edit,char str[]);
status open_file(Edit_info *mp3_edit);
status edit_info(Edit_info *mp3_edit);
status check_ID3(Edit_info *mp3_edit);
status check_mp3version(Edit_info *mp3_edit);
status copy_header(FILE *fptr_dest, FILE *fptr_src);
status do_change(Edit_info *mp3_edit, char str[], char frame[], int *flag);
status modify_data(Edit_info *mp3_edit);
status copy_remaining_data(FILE *fptr_dest, FILE *fptr_src);
status copy_same(Edit_info*mp3_edit);
status file_copy(Edit_info*mp3_edit);
void convert_endianess( char *ptr,  int  size );



#endif