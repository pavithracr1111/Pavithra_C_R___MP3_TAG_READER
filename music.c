#include <stdio.h>
#include "music.h"
#include <string.h>
#include <stdlib.h>

/* Function to check the operation type (edit, view, or help) */
Operation checkOperation(char **argv)
{
    if (strcmp(argv[1], "-e") == 0) // If first argument is "-e", return Edit operation
        return Edit;
    else if (strcmp(argv[1], "-v") == 0) // If first argument is "-v", return View operation
        return View;
    else // Otherwise, return Help operation
        return Help;
}

/* Function to print the help menu with usage instructions */
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

/* Function to validate if the provided file is an MP3 file */
Status readandvalidate(Music *music, char *arg)
{
    if (strcmp(strstr(arg, "."), ".mp3") == 0) // Check if the file has a .mp3 extension
    {
        music->Filename = arg; // Assign filename if valid
        return success;
    }
    else
    {
        printf("\n..................................\n\n");
        printf("ERROR: Mp3 File Type only\n"); // Error message if not a .mp3 file
        printf("\n..................................\n\n");
    }
}

/* Function to open the MP3 file */
Status openFiles(Music *music)
{
    music->fptr_fname = fopen(music->Filename, "rb+"); // Open file in binary read/write mode
    if (music->fptr_fname == NULL)
    {
        perror("fopen"); // Error if file opening fails
        fprintf(stderr, "ERROR: Unable to open file %s\n", music->Filename);
        return failure;
    }
    return success; // Return success if file opens successfully
}

/* Function to check if the MP3 file has a valid header and version */
Status checkheaderandversion(FILE *fname)
{
    char head[4];
    char ver[2];
    fread(head, 3, 1, fname);                                         // Read header (3 bytes)
    head[3] = '\0';                                                   // Null terminate the header string
    fread(ver, 2, 1, fname);                                          // Read version (2 bytes)
    if (strcmp(head, "ID3") == 0 && ver[0] == 0x03 && ver[1] == 0x00) // Check if header is "ID3" and version is 3.0
    {
        return success;
    }
    return failure; // Return failure if header or version doesn't match
}

/* Function to view the information of the MP3 file */
Status viewInfo(Music *music)
{
    if (openFiles(music) == success) // Check if file opened successfully
    {
        if (checkheaderandversion(music->fptr_fname) == success) // Validate header and version
        {
            if (readInfo(music) == success) // Read the MP3 tag information
            {
                printInfo(music); // Print the tag information
            }
        }
    }
}

/* Function to read the tag information from the MP3 file */
Status readInfo(Music *music)
{
    rewind(music->fptr_fname);              // Rewind the file pointer to the start
    fseek(music->fptr_fname, 10, SEEK_CUR); // Skip the 10-byte header
    for (int i = 0; i < 6; i++)             // Loop to read 6 tags (title, album, artist, etc.)
    {
        tags t = tagreader(music->fptr_fname); // Read tag
        switch (t)
        {
        case m_title:
            music->pos[m_title] = ftell(music->fptr_fname);                   // Store position of title tag
            tagsizereader(music->fptr_fname, &music->Title_size);             // Read tag size
            fseek(music->fptr_fname, 3, SEEK_CUR);                            // Skip 3 bytes (encoding type)
            readtaginfo(music->fptr_fname, &music->Title, music->Title_size); // Read the tag content
            break;
        case m_album:
            music->pos[m_album] = ftell(music->fptr_fname);                   // Store position of album tag
            tagsizereader(music->fptr_fname, &music->Album_size);             // Read tag size
            fseek(music->fptr_fname, 3, SEEK_CUR);                            // Skip 3 bytes
            readtaginfo(music->fptr_fname, &music->album, music->Album_size); // Read the tag content
            break;
        case m_artist:
            music->pos[m_artist] = ftell(music->fptr_fname);                    // Store position of artist tag
            tagsizereader(music->fptr_fname, &music->Artist_size);              // Read tag size
            fseek(music->fptr_fname, 3, SEEK_CUR);                              // Skip 3 bytes
            readtaginfo(music->fptr_fname, &music->Artist, music->Artist_size); // Read the tag content
            break;
        case m_year:
            music->pos[m_year] = ftell(music->fptr_fname);                  // Store position of year tag
            tagsizereader(music->fptr_fname, &music->Year_size);            // Read tag size
            fseek(music->fptr_fname, 3, SEEK_CUR);                          // Skip 3 bytes
            readtaginfo(music->fptr_fname, &music->Year, music->Year_size); // Read the tag content
            break;
        case m_genre:
            music->pos[m_genre] = ftell(music->fptr_fname);                   // Store position of genre tag
            tagsizereader(music->fptr_fname, &music->Genre_size);             // Read tag size
            fseek(music->fptr_fname, 3, SEEK_CUR);                            // Skip 3 bytes
            readtaginfo(music->fptr_fname, &music->Genre, music->Genre_size); // Read the tag content
            break;
        case m_comment:
            music->pos[m_comment] = ftell(music->fptr_fname);                 // Store position of comment tag
            tagsizereader(music->fptr_fname, &music->Com_size);               // Read tag size
            fseek(music->fptr_fname, 3, SEEK_CUR);                            // Skip 3 bytes
            readtaginfo(music->fptr_fname, &music->Comment, music->Com_size); // Read the tag content
            break;
        default:
            break;
        }
    }
    return success; // Return success after reading all tags
}

/* Function to print the information of the MP3 file */
Status printInfo(Music *music)
{
    printf("\n........................Selected View Details...................\n");
    printf("\n................................................................\n");
    printf("\n                MP3 TAG READER AND EDITOR FOR ID3v2          \n");
    printf("\n................................................................\n\n");
    printf("TITLE          : %s \n", music->Title);   // Print the title tag content
    printf("ARTIST         : %s \n", music->Artist);  // Print the artist tag content
    printf("ALBUM          : %s \n", music->album);   // Print the album tag content
    printf("YEAR           : %s \n", music->Year);    // Print the year tag content
    printf("MUSIC          : %s \n", music->Genre);   // Print the genre tag content
    printf("COMMENT        : %s \n", music->Comment); // Print the comment tag content
    printf("\n................................................................\n\n");
    return success;
}

/* Function to read the tag identifier and determine which tag it is */
tags tagreader(FILE *fname)
{
    char tag[4 + 1];
    fread(tag, 4, 1, fname); // Read the 4-byte tag identifier
    tag[4] = '\0';           // Null terminate the tag identifier
    if (strcmp(tag, "TIT2") == 0)
        return m_title;
    else if (strcmp(tag, "TALB") == 0)
        return m_album;
    else if (strcmp(tag, "TPE1") == 0)
        return m_artist;
    else if (strcmp(tag, "TYER") == 0)
        return m_year;
    else if (strcmp(tag, "TCON") == 0)
        return m_genre;
    else if (strcmp(tag, "COMM") == 0)
        return m_comment;
}

/* Function to read the size of the tag */
Status tagsizereader(FILE *fname, int *size)
{
    char ch;
    *size = 0;
    for (int i = 0; i < 4; i++) // Loop to read the 4-byte size field
    {
        fread(&ch, 1, 1, fname);
        *size = *size << i * 8 | ch; // Convert size to integer
    }
    *size = *size - 1; // Adjust size value
}

/* Function to read the tag content and store it */
Status readtaginfo(FILE *fname, char **name, int size)
{
    char *buffer = malloc(size + 1); // Allocate memory for the tag content
    fread(buffer, size, 1, fname);   // Read the tag content
    buffer[size] = '\0';             // Null terminate the content
    *name = malloc(size + 1);        // Allocate memory to store the content
    strcpy(*name, buffer);           // Copy the content into the destination variable
    free(buffer);                    // Free the temporary buffer
}
