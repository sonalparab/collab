#include "word_gen.h"

// Generates the list of words to be used in the hangman game
char ** wordlist() {
    // Open the original dict
    FILE *f = fopen(DICTLOCATION, "r");
    // If dict cannot be found, return with error
    if (f == NULL) {
        return NULL;
    }

    // Allocate memory for all the words
    char **wordlist = (char **) malloc(sizeof(char *) * MAXDICTLENGTH);
    int i = 0;
    for (i = 0; i < MAXDICTLENGTH; i++) {
        wordlist[i] = (char *) malloc(sizeof(char) * MAXWORDLENGTH);
    }

    // Read each word
    // Reject the word if len < 6 or has apostrophes
    i = -1;
    while (++i < MAXDICTLENGTH && fgets(wordlist[i], MAXWORDLENGTH * sizeof(char), f)) {
        char *newline = strchr(wordlist[i], '\n');
        if (newline) {
            *newline = 0;
        }
        if (strlen(wordlist[i]) < 6) {
            /* printf("word rejected: %s\n", wordlist[i]); */
            i--;
        } else {
            char *apostrophe = strchr(wordlist[i], '\'');
            if (apostrophe) {
                i--;
            }
        }
    }

    // Make sure the last item in list is null (for ease of use)
    *(wordlist[i]) = 0;
    printf("[INIT] Word list generated.\n");
    fclose(f);
    return wordlist;
}

char * word_pick(char ** list) {
    int len = wordlist_len(list);
    int index = rand() % len;
    char *rand_word = (char *) malloc(sizeof(char) * MAXWORDLENGTH);
    strcpy(rand_word, list[index]);
    lower(rand_word);

    // Remove the word chosen
    if (index != len - 1) {
        strcpy(list[index], list[len-1]);
    }
    *(list[len-1]) = 0;

    return rand_word;
}

int wordlist_len(char ** list) {
    int i = 0;
    for (i = 0; *(list[i]); i++);
    return i;
}

unsigned char * lower(char *str) {
    unsigned char *my_str = (unsigned char *) str;
    int i = 0;
    for (; my_str[i]; i++) {
        my_str[i] = tolower(my_str[i]);
    }
    return my_str;
}
