#include "edit.h"
#include <string.h>

// Function to edit music metadata information and print only the edited content
Status editInfo(Music *music, char *ch, char *name)
{
    // Open the music file for reading
    if (openFiles(music) == success)
    {
        // Open a temporary file for editing
        FILE *fptr_dest = fopen("sample.mp3", "wb+");
        if (fptr_dest == NULL)
        {
            perror("fopen"); // Handle file open error
            fprintf(stderr, "ERROR: Unable to open file %s\n", "sample.mp3");

            return failure; // Return failure if unable to open file
        }
        // Check the MP3 header and version compatibility
        if (checkheaderandversion(music->fptr_fname) == success)
        {
            // Read existing metadata from the file
            if (readInfo(music) == success)
            {
                // Modify the tag based on user input and get the edited tag type
               edittags tag = renametag(music, ch, name, fptr_dest);
                
                // Copy the modified content back to the original file
                copytoriginal(music->fptr_fname, fptr_dest);

                // Print only the edited content
                switch (tag)
                {
                    case t:
                        printf("\nEdited Title: %s\n\n", name);
                        break;
                    case a:
                        printf("\nEdited Album: %s\n\n", name);
                        break;
                    case A:
                        printf("\nEdited Artist: %s\n\n", name);
                        break;
                    case y:
                        printf("\nEdited Year: %s\n\n", name);
                        break;
                    case m:
                        printf("\nEdited Genre: %s\n\n", name);
                        break;
                    case c:
                        printf("\nEdited Comment: %s\n\n", name);
                        break;
                    default:
                        printf("\nUnknown tag edited.\n\n");
                        break;
                }
            }
        }
        if (fptr_dest != NULL)
        {
            fclose(fptr_dest);  // Ensure the file is closed
            remove("sample.mp3");  // Remove the temporary file only after closing
        }
    }

    return success;
}



edittags renametag(Music *music, char *ch, char *name, FILE *fptr_dest)
{
    edittags tag = checkedit(ch);  // Get the tag type from the user input
    if (copyheader(music->fptr_fname, fptr_dest) == success)
    {
        // Determine which tag to edit based on user input
        switch (tag)
        {
            case t: // Modify title tag
                copyContent(music->fptr_fname, music->pos[m_title], fptr_dest);
                copynewcon(music->fptr_fname, name, fptr_dest);
                copyremaining(music->fptr_fname, music->Title_size, fptr_dest);
                break;
            case a: // Modify album tag
                copyContent(music->fptr_fname, music->pos[m_album], fptr_dest);
                copynewcon(music->fptr_fname, name, fptr_dest);
                copyremaining(music->fptr_fname, music->Album_size, fptr_dest);
                break;
            case A: // Modify artist tag
                copyContent(music->fptr_fname, music->pos[m_artist], fptr_dest);
                copynewcon(music->fptr_fname, name, fptr_dest);
                copyremaining(music->fptr_fname, music->Artist_size, fptr_dest);
                break;
            case y: // Modify year tag
                copyContent(music->fptr_fname, music->pos[m_year], fptr_dest);
                copynewcon(music->fptr_fname, name, fptr_dest);
                copyremaining(music->fptr_fname, music->Year_size, fptr_dest);
                break;
            case m: // Modify genre tag
                copyContent(music->fptr_fname, music->pos[m_genre], fptr_dest);
                copynewcon(music->fptr_fname, name, fptr_dest);
                copyremaining(music->fptr_fname, music->Genre_size, fptr_dest);
                break;
            case c: // Modify comment tag
                copyContent(music->fptr_fname, music->pos[m_comment], fptr_dest);
                copynewcon(music->fptr_fname, name, fptr_dest);
                copyremaining(music->fptr_fname, music->Com_size, fptr_dest);
                break;
            default:
                break;
        }
    }
    return tag;  // Return the tag that was modified
}


// Function to copy content up to a specific position in the file
Status copyContent(FILE *fname, int pos, FILE *fptr_dest)
{
    char ch;
    // Copy the content up to the specified position
    for (int i = 10; i < pos; i++)
    {
        fread(&ch, 1, 1, fname);
        fwrite(&ch, 1, 1, fptr_dest);
    }
}

// Function to copy the new content (e.g., new tag name) into the file
Status copynewcon(FILE *fname, char *name, FILE *fptr_dest)
{
    int len = strlen(name) + 1; // Get the length of the new tag name
    char bytes[4];              // Store length in bytes
    for (int i = 0; i < 4; i++)
    {
        bytes[3 - i] = ((len >> 8 * i) & 0xFF); // Break down length into bytes
    }
    fwrite(bytes, 4, 1, fptr_dest); // Write the length to the destination file
    fseek(fname, 4, SEEK_CUR);      // Skip 4 bytes in the original file

    char buffer[3]; // Buffer to store next 3 bytes of content
    fread(buffer, 3, 1, fname);
    fwrite(buffer, 3, 1, fptr_dest);

    // Write the new name (tag) to the destination file
    fwrite(name, len - 1, 1, fptr_dest);
    return success;
}

// Function to skip and copy remaining content after editing the tag
Status copyremaining(FILE *fname, int size, FILE *fptr_dest)
{
    fseek(fname, size, SEEK_CUR); // Skip the remaining tag data
    char ch;
    // Copy all remaining content to the destination file
    while (fread(&ch, 1, 1, fname))
    {
        fwrite(&ch, 1, 1, fptr_dest);
    }
    return success;
}

// Function to copy the header (first 10 bytes) of the MP3 file
Status copyheader(FILE *fname, FILE *fptr_dest)
{
    rewind(fname);                    // Go to the start of the file
    char buffer[10];                  // Buffer to store the header
    fread(buffer, 10, 1, fname);      // Read the header from the original file
    fwrite(buffer, 10, 1, fptr_dest); // Write the header to the destination file
    return success;
}

Status copytoriginal(FILE *fname, FILE *fptr_dest)
{
    if (fname == NULL || fptr_dest == NULL)
    {
        return failure;  // Ensure pointers are valid
    }

    rewind(fname);     // Go to the start of the original file
    rewind(fptr_dest); // Go to the start of the destination file
    char ch;

    // Copy content from the temporary file to the original file
    while (fread(&ch, 1, 1, fptr_dest) > 0)
    {
        fwrite(&ch, 1, 1, fname);
    }

    return success;
}

// Function to determine the tag to be edited based on user input
edittags checkedit(char *ch)
{
    if (!strcmp(ch, "-t"))
        return t; // Edit title
    else if (!strcmp(ch, "-a"))
        return a; // Edit album
    else if (!strcmp(ch, "-A"))
        return A; // Edit artist
    else if (!strcmp(ch, "-y"))
        return y; // Edit year
    else if (!strcmp(ch, "-m"))
        return m; // Edit genre
    else if (!strcmp(ch, "-c"))
        return c; // Edit comment
}
