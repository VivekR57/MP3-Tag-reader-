#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "view.h"
#include "type.h"

/**
 * Function: printHelp
 * Description: Prints the help menu for the user with detailed instructions on how to use the program.
 * Input: None.
 * Output: Prints the help menu to the standard output.
 */
void printHelp()
{
    printf("\n.............Help Menu.....................\n\n");
    printf("1. -v -> to view mp3 file contents\n");
    printf("2. -e -> to edit mp3 file contents\n");
    printf(" 2.1. -t -> to edit song title\n");
    printf(" 2.2. -A -> to edit artist name\n");
    printf(" 2.3. -a -> to edit album name\n");
    printf(" 2.4. -y -> to edit song year\n");
    printf(" 2.5. -m -> to edit song content\n");
    printf(" 2.6. -c -> to edit song comment\n");
    printf("\n............................................\n\n");
}

/**
 * Function: read_and_validate
 * Description: Validates the command-line arguments, ensuring that the mp3 filename is provided and is valid.
 * Input: argc - the number of command-line arguments, argv - the array of arguments, music - pointer to the Music struct.
 * Output: Returns success if the filename is valid, or failure if any validation check fails.
 */
status read_and_validate(int argc, char *argv[], Music *music)
{
    // Check if there are enough arguments for the filename
    if (argc < 3)
    {
        fprintf(stderr, "ERROR: Filename argument missing.\n");
        return failure;
    }

    // Check if the file extension is ".mp3"
    char *str = strstr(argv[2], ".mp3");
    if (str == NULL || strcmp(str, ".mp3") != 0)
    {
        fprintf(stderr, "ERROR: Mp3 File Type only\n");
        return failure;
    }

    // Set the filename in the Music struct
    music->Filename = argv[2];
    return success;
}

/**
 * Function: viewInfo
 * Description: Opens the mp3 file and displays the information about the mp3 file such as title, artist, album, year, genre, and comment.
 * Input: music - pointer to the Music struct containing the mp3 file information.
 * Output: Returns success if the information is retrieved successfully, or failure if any error occurs during the process.
 */
status viewInfo(Music *music)
{
    // Open the mp3 file
    if (openFiles(music) == failure)
    {
        return failure;
    }

    // Check the header and version of the mp3 file
    if (checkheaderandversion(music->fptr_fname) == failure)
    {
        return failure;
    }

    // Print and read each tag (title, artist, album, etc.)
    printf("TITLE    :   ");
    if (read_info(music, "TIT2") == failure)
    {
        printf("Error in getting title name\n");
    }
    printf("ARTIST   :   ");
    if (read_info(music, "TPE1") == failure)
    {
        printf("Error in getting artist name\n");
    }
    printf("ALBUM    :   ");
    if (read_info(music, "TALB") == failure)
    {
        printf("Error in getting album name\n");
    }
    printf("YEAR     :   ");
    if (read_info(music, "TYER") == failure)
    {
        printf("Error in getting year\n");
    }
    printf("MUSIC    :   ");
    if (read_info(music, "TCON") == failure)
    {
        printf("Error in getting genre\n");
    }
    printf("COMMENT  :   ");
    if (read_info(music, "COMM") == failure)
    {
        printf("Error in getting comments\n");
    }
    return success;
}

/**
 * Function: openFiles
 * Description: Opens the mp3 file for reading.
 * Input: music - pointer to the Music struct containing the filename.
 * Output: Returns success if the file is opened successfully, or failure if there is an error opening the file.
 */
status openFiles(Music *music)
{
    // Try to open the mp3 file in read mode
    music->fptr_fname = fopen(music->Filename, "r");
    if (music->fptr_fname == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", music->Filename);
        return failure;
    }
    return success;
}

/**
 * Function: checkheaderandversion
 * Description: Checks the ID3 header and version of the mp3 file to ensure it's a valid ID3v2 file.
 * Input: fname - the file pointer of the mp3 file.
 * Output: Returns success if the header and version are valid, or failure if there is an error.
 */
status checkheaderandversion(FILE *fname)
{
    char head[4];
    char ver[2];

    // Read the ID3 header (first 3 bytes)
    if (fread(head, 1, 3, fname) != 3)
    {
        fprintf(stderr, "ERROR: Failed to read header.\n");
        return failure;
    }
    head[3] = '\0';

    // Read the version (next 2 bytes)
    if (fread(ver, 1, 2, fname) != 2)
    {
        fprintf(stderr, "ERROR: Failed to read version.\n");
        return failure;
    }

    // Check if the header matches "ID3" and the version is 3.0
    if (strcmp(head, "ID3") == 0 && ver[0] == 0x03 && ver[1] == 0x00)
    {
        return success;
    }
    return failure;
}

/**
 * Function: read_info
 * Description: Reads and displays the content of a specific tag in the mp3 file.
 * Input: music - pointer to the Music struct, tag - the 4-character string representing the tag to read (e.g., "TIT2" for title).
 * Output: Returns success if the tag content is successfully read, or failure if any error occurs while reading.
 */
status read_info(Music *music, const char *tag)
{
    char frame_id[4];
    uint32_t size;

    // Move the file pointer to the start of the frames section
    fseek(music->fptr_fname, 10, SEEK_SET);

    // Loop through each frame in the mp3 file to find the requested tag
    while (fread(frame_id, 1, 4, music->fptr_fname) == 4)
    {
        // Read the size of the current frame
        if (fread(&size, 4, 1, music->fptr_fname) != 1)
        {
            fprintf(stderr, "ERROR: Failed to read frame size.\n");
            return failure;
        }
        little_to_big((char *)&size, sizeof(size)); // Convert to big-endian format

        // Skip the flags (2 bytes)
        fseek(music->fptr_fname, 2, SEEK_CUR);

        // If the current frame matches the requested tag, read and display its content
        if (strncmp(frame_id, tag, 4) == 0)
        {
            for (uint32_t i = 0; i < size; i++)
            {
                char ch;
                if (fread(&ch, 1, 1, music->fptr_fname) != 1)
                {
                    fprintf(stderr, "ERROR: Failed to read tag content.\n");
                    return failure;
                }
                if (ch != '\0') // Skip padding null characters
                {
                    putchar(ch);
                }
            }
            printf("\n");
            return success;
        }
        else
        {
            // Skip the content of the current frame if it doesn't match the requested tag
            fseek(music->fptr_fname, size, SEEK_CUR);
        }
    }

    // Print an error if the requested tag was not found
    fprintf(stderr, "ERROR: Tag %s not found.\n", tag);
    return failure;
}

/**
 * Function: little_to_big
 * Description: Converts the byte order of the data from little-endian to big-endian.
 * Input: ptr - pointer to the data to be reversed, size - size of the data.
 * Output: Reverses the byte order of the data in-place.
 */
void little_to_big(char *ptr, int size)
{
    int i;
    char temp;
    // Reverse the byte order (swap first with last, second with second last, etc.)
    for (i = 0; i < size / 2; i++)
    {
        temp = ptr[i];
        ptr[i] = ptr[size - i - 1];
        ptr[size - i - 1] = temp;
    }
}
