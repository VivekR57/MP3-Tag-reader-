#include <stdio.h>
#include <string.h>
#include "type.h"
#include "edit.h"
#include <ctype.h>

/**
 * Function: read_and_validate_edit
 * Description: Validates the command-line arguments to ensure the mp3 filename is provided and valid,
 *              and checks for the correct editing operation type and data.
 * Input: argv - the array of command-line arguments, mp3_edit - pointer to the Edit_info struct.
 * Output: Returns success if the arguments are valid; otherwise, returns failure.
 */
status read_and_validate_edit(char *argv[], Edit_info *mp3_edit)
{
    // Check if filename is provided in the expected position
    if (argv[4] == NULL)
    {
        printf("ERROR: ./a.out : Filename is missing\n");
        return failure;
    }

    // Check for file extension
    char *extn = strrchr(argv[4], '.');
    if (extn == NULL || strcmp(extn, ".mp3") != 0)
    {
        printf("ERROR: ./a.out : INVALID EXTENSION\n");
        return failure;
    }

    // Set the filename in the structure
    mp3_edit->src_fname = argv[4];

    // Validate the edit type
    if (strcmp(argv[2], "-t") != 0 && strcmp(argv[2], "-a") != 0 &&
        strcmp(argv[2], "-A") != 0 && strcmp(argv[2], "-m") != 0 &&
        strcmp(argv[2], "-y") != 0 && strcmp(argv[2], "-c") != 0)
    {
        printf("ERROR: ./a.out : INVALID ARGUMENTS\n");
        return failure;
    }

    // Set frame and data in the structure
    mp3_edit->frame = argv[2];
    mp3_edit->modify_data = argv[3];
    mp3_edit->data_length = strlen(mp3_edit->modify_data) + 1;

    return success;
}

/**
 * Function: edit_info
 * Description: Handles the editing of mp3 file information. Based on the user's selection,
 *              it modifies the mp3 tags (Title, Artist, Album, Year, Content, Comment) and saves the changes.
 * Input: mp3_edit - pointer to the Edit_info struct.
 * Output: Returns success if the edit is successfully performed; otherwise, returns failure.
 */
status edit_info(Edit_info *mp3_edit)
{
    // Open the source and output files
    if (open_file(mp3_edit) == failure)
    {
        printf("Error: in opening files\n");
        return failure;
    }

    // Check ID3 format and version
    if (check_ID3(mp3_edit) == failure)
    {
        printf("Invalid mp3 ID format\n");
        return failure;
    }
    if (check_mp3version(mp3_edit) == failure)
    {
        printf("Invalid ID3 version\n");
        return failure;
    }

    // Copy the header to the new file
    if (copy_header(mp3_edit->fptr_out, mp3_edit->fptr_src) == failure)
    {
        printf("Error in copying header\n");
        return failure;
    }

    // Edit each tag as specified by the user
    int flag = 0;

    // Change Title
    if (do_change(mp3_edit, "TIT2", "-t", &flag) == failure || flag)
    {
        if (flag)
        {
            printf("TITLE: %s\n", mp3_edit->modify_data);
            fclose(mp3_edit->fptr_src);
            fclose(mp3_edit->fptr_out);
            file_copy(mp3_edit);
            printf("TITLE CHANGED SUCCESSFULLY\n");
            return success;
        }
        printf("Error\n");
        return failure;
    }

    // Change Artist
    flag = 0;
    if (do_change(mp3_edit, "TPE1", "-a", &flag) == failure || flag)
    {
        if (flag)
        {
            printf("ARTIST: %s\n", mp3_edit->modify_data);
            fclose(mp3_edit->fptr_src);
            fclose(mp3_edit->fptr_out);
            file_copy(mp3_edit);
            printf("ARTIST CHANGED SUCCESSFULLY\n");
            return success;
        }
        printf("Error\n");
        return failure;
    }

    // Change Album
    flag = 0;
    if (do_change(mp3_edit, "TALB", "-A", &flag) == failure || flag)
    {
        if (flag)
        {
            printf("ALBUM: %s\n", mp3_edit->modify_data);
            fclose(mp3_edit->fptr_src);
            fclose(mp3_edit->fptr_out);
            file_copy(mp3_edit);
            printf("ALBUM CHANGED SUCCESSFULLY\n");
            return success;
        }
        printf("Error\n");
        return failure;
    }

    // Change Year
    flag = 0;
    if (do_change(mp3_edit, "TYER", "-y", &flag) == failure || flag)
    {
        if (flag)
        {
            printf("YEAR: %s\n", mp3_edit->modify_data);
            fclose(mp3_edit->fptr_src);
            fclose(mp3_edit->fptr_out);
            file_copy(mp3_edit);
            printf("YEAR CHANGED SUCCESSFULLY\n");
            return success;
        }
        printf("Error\n");
        return failure;
    }

    // Change Content
    flag = 0;
    if (do_change(mp3_edit, "TCON", "-m", &flag) == failure || flag)
    {
        if (flag)
        {
            printf("CONTENT: %s\n", mp3_edit->modify_data);
            fclose(mp3_edit->fptr_src);
            fclose(mp3_edit->fptr_out);
            file_copy(mp3_edit);
            printf("CONTENT CHANGED SUCCESSFULLY\n");
            return success;
        }
        printf("Error\n");
        return failure;
    }

    // Change Comment
    flag = 0;
    if (do_change(mp3_edit, "COMM", "-c", &flag) == failure || flag)
    {
        if (flag)
        {
            printf("COMMENT: %s\n", mp3_edit->modify_data);
            fclose(mp3_edit->fptr_src);
            fclose(mp3_edit->fptr_out);
            file_copy(mp3_edit);
            printf("COMMENT CHANGED SUCCESSFULLY\n");
            return success;
        }
        printf("Error\n");
        return failure;
    }

    return success;
}

/**
 * Function: open_file
 * Description: Opens the source and output mp3 files for reading and writing.
 * Input: mp3_edit - pointer to the Edit_info struct.
 * Output: Returns success if both files are opened successfully; otherwise, returns failure.
 */
status open_file(Edit_info *mp3_edit)
{
    mp3_edit->fptr_src = fopen(mp3_edit->src_fname, "r");
    if (mp3_edit->fptr_src == NULL)
    {
        return failure;
    }
    mp3_edit->fptr_out = fopen(mp3_edit->out_fname, "w");
    if (mp3_edit->fptr_out == NULL)
    {
        fclose(mp3_edit->fptr_src);
        return failure;
    }
    return success;
}

/**
 * Function: check_ID3
 * Description: Verifies if the mp3 file contains a valid ID3 header.
 * Input: mp3_edit - pointer to the Edit_info struct.
 * Output: Returns success if the ID3 header is found; otherwise, returns failure.
 */
status check_ID3(Edit_info *mp3_edit)
{
    char head[4];
    if (fread(head, 1, 3, mp3_edit->fptr_src) != 3)
    {
        fprintf(stderr, "ERROR: Failed to read header.\n");
        return failure;
    }
    head[3] = '\0';
    if (strcmp(head, "ID3") == 0)
    {
        return success;
    }
    return failure;
}

/**
 * Function: check_mp3version
 * Description: Verifies the ID3 version of the mp3 file (checks if it is version 3.0).
 * Input: mp3_edit - pointer to the Edit_info struct.
 * Output: Returns success if the version is 3.0; otherwise, returns failure.
 */
status check_mp3version(Edit_info *mp3_edit)
{
    char version[2];
    if (fread(version, 1, 2, mp3_edit->fptr_src) != 2)
    {
        fprintf(stderr, "ERROR: Failed to read version.\n");
        return failure;
    }

    // Check if the version is 3.0
    if (version[0] == 0x03 && version[1] == 0x00)
    {
        return success;
    }

    return failure;
}

/**
 * Function: copy_header
 * Description: Copies the first 10 bytes (header) from the source file to the destination file.
 * Input: fptr_dest - the destination file pointer, fptr_src - the source file pointer.
 * Output: Returns success if the header is copied successfully; otherwise, returns failure.
 */
status copy_header(FILE *fptr_dest, FILE *fptr_src)
{
    rewind(fptr_dest);
    rewind(fptr_src);
    char header[10];
    if (fread(header, 10, 1, fptr_src) != 1)
        return failure;
    if (fwrite(header, 10, 1, fptr_dest) != 1)
        return failure;
    return success;
}

/**
 * Function: do_change
 * Description: Modifies the tag in the mp3 file. It checks if the frame matches and performs the modification.
 * Input: mp3_edit - pointer to the Edit_info struct, str - frame type to modify, frame - the argument from the command-line input, flag - a pointer to a flag to indicate success.
 * Output: Returns success if the tag is changed; otherwise, returns failure.
 */
status do_change(Edit_info *mp3_edit, char str[], char frame[], int *flag)
{
    if (check_frame(mp3_edit, str) == failure)
    {
        printf("Frame matching error\n");
        return failure;
    }
    if (strcmp(mp3_edit->frame, frame) == 0)
    {
        modify_data(mp3_edit);
        fseek(mp3_edit->fptr_src, mp3_edit->size - 1, SEEK_CUR);
        copy_remaining_data(mp3_edit->fptr_out, mp3_edit->fptr_src);
        *flag = 1;
        return success;
    }
    if (copy_same(mp3_edit) == failure)
    {
        printf("Error in copying title\n");
        return failure;
    }
    return success;
}

/**
 * Function: check_frame
 * Description: Checks if the current frame in the mp3 file matches the expected frame type.
 * Input: mp3_edit - pointer to the Edit_info struct, str - expected frame type.
 * Output: Returns success if the frame matches; otherwise, returns failure.
 */
status check_frame(Edit_info *mp3_edit, char str[])
{
    char frame[4];
    fread(frame, 4, 1, mp3_edit->fptr_src);
    if (strcmp(frame, str) != 0)
    {
        return failure;
    }
    fwrite(frame, 4, 1, mp3_edit->fptr_out);
    fread(&mp3_edit->size, 4, 1, mp3_edit->fptr_src);
    convert_endianess((char *)&mp3_edit->size, sizeof(int));
    return success;
}

/**
 * Function: copy_same
 * Description: Copies the frame that does not need editing from the source file to the destination file.
 * Input: mp3_edit - pointer to the Edit_info struct.
 * Output: Returns success if the frame is copied successfully; otherwise, returns failure.
 */
status copy_same(Edit_info *mp3_edit)
{
    int size = mp3_edit->size;
    convert_endianess((char *)&size, sizeof(int));
    fwrite(&size, 4, 1, mp3_edit->fptr_out);
    char flag[3];
    fread(flag, 3, 1, mp3_edit->fptr_src);
    fwrite(flag, 3, 1, mp3_edit->fptr_out);
    char data[mp3_edit->size];
    fread(data, mp3_edit->size - 1, 1, mp3_edit->fptr_src);
    fwrite(data, mp3_edit->size - 1, 1, mp3_edit->fptr_out);
    return success;
}

/**
 * Function: copy_remaining_data
 * Description: Copies the remaining data from the source file to the destination file after a modification.
 * Input: fptr_dest - the destination file pointer, fptr_src - the source file pointer.
 * Output: Returns success after copying the data.
 */
status copy_remaining_data(FILE *fptr_dest, FILE *fptr_src)
{
    char ch;
    while (fread(&ch, 1, 1, fptr_src) > 0)
    {
        fwrite(&ch, 1, 1, fptr_dest);
    }
    return success;
}

/**
 * Function: modify_data
 * Description: Writes the modified data (e.g., title, artist) into the frame of the mp3 file.
 * Input: mp3_edit - pointer to the Edit_info struct.
 * Output: Returns success after modifying the data.
 */
status modify_data(Edit_info *mp3_edit)
{
    convert_endianess((char *)&mp3_edit->data_length, sizeof(int));
    fwrite(&mp3_edit->data_length, 4, 1, mp3_edit->fptr_out);
    char flag[3];
    fread(flag, 3, 1, mp3_edit->fptr_src);
    fwrite(flag, 3, 1, mp3_edit->fptr_out);
    fwrite(mp3_edit->modify_data, strlen(mp3_edit->modify_data), 1, mp3_edit->fptr_out);
    return success;
}

/**
 * Function: convert_endianess
 * Description: Converts the byte order from little-endian to big-endian or vice versa.
 * Input: ptr - pointer to the data, size - size of the data.
 * Output: The data is converted in place.
 */
void convert_endianess(char *ptr, int size)
{
    for (int i = 0; i < size / 2; i++)
    {
        char temp = ptr[i];
        ptr[i] = ptr[size - i - 1];
        ptr[size - i - 1] = temp;
    }
}

/**
 * Function: file_copy
 * Description: Copies the content of the output file back into the source file to save changes.
 * Input: mp3_edit - pointer to the Edit_info struct.
 * Output: Returns success after copying the data.
 */
status file_copy(Edit_info *mp3_edit)
{
    mp3_edit->fptr_src = fopen(mp3_edit->src_fname, "w");
    mp3_edit->fptr_out = fopen(mp3_edit->out_fname, "r");
    rewind(mp3_edit->fptr_src);
    rewind(mp3_edit->fptr_out);
    char ch;
    while (fread(&ch, 1, 1, mp3_edit->fptr_out) > 0)
    {
        fwrite(&ch, 1, 1, mp3_edit->fptr_src);
    }
    return success;
}
