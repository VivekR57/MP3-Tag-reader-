#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "type.h"
#include "view.h"
#include "edit.h"

int main(int argc, char *argv[])
{
    Music music;
    Edit_info mp3_edit;

    // Check if command-line arguments are provided
    if (argc > 1)
    {
        // Check the operation type based on the first argument
        if (check_operation_type(argv[1]) == edit)
        {
            // If operation is to edit, check if enough arguments are provided
            if (argc >= 5)
            {
                // Read and validate the edit information from command-line arguments
                if (read_and_validate_edit(argv, &mp3_edit) == success)
                {
                    // Edit the mp3 file's information based on user input
                    edit_info(&mp3_edit);
                }
            }
            else
            {
                // Print message if insufficient arguments for edit operation
                printf("Please enter the correct arguments.\n");
            }
        }
        else if (check_operation_type(argv[1]) == view)
        {
            // If operation is to view, validate the mp3 file input
            if (read_and_validate(argc, argv, &music) == failure)
            {
                return failure;
            }
            // View the mp3 file's information
            viewInfo(&music);
        }
        else if (check_operation_type(argv[1]) == help)
        {
            // Print the help message to guide the user on how to use the program
            printHelp();
        }
    }
    else
    {
        // Print error message and usage instructions if no arguments are provided
        printf("ERROR: Invalid arguments.\n");
        printf("USAGE:\n");
        printf("To view: ./a.out -v <mp3filename>\n");
        printf("To edit: ./a.out -e -t/-a/-A/-m/-y/-c <newname> <mp3filename>\n");
        printf("To get help: ./a.out --help\n");
    }

    return 0;
}

/**
 * Function: check_operation_type
 * Description: Determines the type of operation (view, edit, help) based on the command-line argument.
 * Input: argv - Command-line argument (string) that indicates the operation type.
 * Output: Returns the corresponding OperationType (view, edit, help), or failure if no match is found.
 */
OperationType check_operation_type(char *argv)
{
    if (strcmp(argv, "-v") == 0)
    {
        return view; // Operation to view the mp3 file info
    }
    else if (strcmp(argv, "-e") == 0)
    {
        return edit; // Operation to edit the mp3 file info
    }
    else if (strcmp(argv, "--help") == 0)
    {
        return help; // Operation to display help message
    }
    return failure; // Return failure if no recognized operation is found
}
