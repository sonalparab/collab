#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <ctype.h>

// For use in actual game
#define DICTLOCATION "/usr/share/dict/american-english"
#define MAXDICTLENGTH 100000

// for use in testing
/* #define DICTLOCATION "testdict" */
/* #define MAXDICTLENGTH 10 */

#define MAXWORDLENGTH 20

#ifndef WORDGEN_H
#define WORDGEN_H
char ** wordlist();

char * word_pick(char **);

int wordlist_len(char **);

unsigned char * lower(char *str);
#endif
