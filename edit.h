#include<stdio.h>
#include"music.h"  

/* Edit music metadata information */
Status editInfo(Music *music, char *ch, char *name);

/* Rename a specific tag in the MP3 metadata */
Status renametag(Music *music, char *ch, char *name, FILE *fptr_dest);

/* Copy the MP3 file header from the original file to the destination file */
Status copyheader(FILE *fname, FILE *fptr_dest);

/* Determine which tag the user wants to edit */
edittags checkedit(char *ch);

/* Copy content from the original file up to a specific position */
Status copyContent(FILE *fname, int pos, FILE *fptr_dest);

/* Write the new content (new tag value) into the destination file */
Status copynewcon(FILE *fname, char *name, FILE *fptr_dest);

/* Copy the remaining part of the MP3 file after the edited tag */
Status copyremaining(FILE *fname, int size, FILE *fptr_dest);

/* Copy all modified content back from the temporary file to the original MP3 file */
Status copytoriginal(FILE *fname, FILE *fptr_dest);
