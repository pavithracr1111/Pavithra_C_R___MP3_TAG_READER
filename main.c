/*
Name : Pavithra C R
Descritption : MP3 tag reader and editor
*/

#include <stdio.h>
#include "edit.h" 

// Main function
int main(int argc, char *argv[])
{
    Music music;  // Declare a variable to hold music file data
    if (argc > 1) // Check if there are enough arguments passed
    {
        if (checkOperation(argv) == Edit) // Check if the operation is "Edit"
        {
            if (argc >= 4) // Ensure there are enough arguments for editing
            {
                // Validate the input file and read its contents
                if (readandvalidate(&music, argv[4]) == success)
                {
                    // Perform the edit operation with the provided new information
                    editInfo(&music, argv[2], argv[3]);
                }
            }
            else
            {
                // If not enough arguments, prompt the user to enter correct arguments
                printf("\n...................................\n");
                printf("\n    please enter correct arguments \n");
                printf("\n......................................\n");
            }
        }
        else if (checkOperation(argv) == View) // Check if the operation is "View"
        {
            // Validate the file and display its contents
            if (readandvalidate(&music, argv[2]) == success)
            {
                viewInfo(&music); // Call function to view the music file's metadata
            }
        }
        else if (checkOperation(argv) == Help) // Check if the operation is "Help"
        {
            printHelp(); // Call function to display help/usage instructions
        }
    }
    else
    {
        // If no or incorrect arguments are passed, display an error message and usage guide
        printf("\n........................................................\n\n");
        printf("ERROR: ./a.out : INVALID ARGUMENTS\n");
        printf("USAGE : \n");
        printf("To view use : ./a.out -v mp3filename\n");
        printf("To edit use : ./a.out -e  -t/-a/-A/-m/-y/-c newname mp3filename\n");
        printf("To get help use : ./a.out --help\n");
        printf("\n........................................................\n\n");
    }
}
