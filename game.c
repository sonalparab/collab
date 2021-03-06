#include "game.h"
#include "pipe_networking.h"
#include "sem.h"
#include "sharedmem.h"

char * blank_array(int length){
    char * array = calloc(length,sizeof(char));
    int i = 0;
    for (;i < length; i++){
        array[i] = '_';
    }
    return array;
}

/*void run_game_collab(char * word, int to_client, int from_client){
  while(1)
    run_turn(char * word, int to_client, int from_client);
    }*/


void run_game_collab(char* word, int to_client, int from_client){
  //GET FROM SHARED MEMORY, WILL NEED TO SEND THE SHMID instead of WORD
  //char * word;

    printf("WORD: %s\n\n",word);
  
    int test;
    int pid = getpid();
    char * message = (char *) calloc(BUFFER_SIZE, sizeof(char));
    char * buffer = (char *) calloc (BUFFER_SIZE, sizeof(char));
    char * hangman;
    
    //stuff to keep track of
    int wrong_guesses = 0;
    int len = strlen(word);
    //array for guessing the word, intially blank
    char * guessing_array = blank_array(len);
    //input (guess) with potentially multiple characters and newline
    char input[100];
    //letter guessed
    char letter;
    //array and counter for guessed letters
    char * guessed_letters = calloc(26,sizeof(char));
    //index of guessed_letters array
    int g = 0;
    printf("[subserver %d] running game...\n", pid);
    int won = 0;
    while(1){

      //semaphore and shared memory stuffs here
      //MAKE A SEMAPHORE TO KEEP TRACK OF THE BACK AND FORTH STUFF
      //GET THE WORD FROM SHARED MEMORY ON TOP

      //making the actual turn call
      won = run_turn(len,&wrong_guesses, guessing_array, guessed_letters, &g, word, to_client, from_client);
      //check if player lost
      if(wrong_guesses == 6){
	hangman = (char *) calloc(BUFFER_SIZE,sizeof(char));
	strcpy(hangman,generate_man(wrong_guesses));
	write(to_client, hangman, BUFFER_SIZE);
	printf("[subserver %d] Sent %s\n", pid, hangman);
	test = read(from_client, buffer, BUFFER_SIZE);
	if (test == -1 || strcmp(buffer, ACK)) {
	  printf("Error 5.5!");
	}
	buffer = zero_heap(hangman, BUFFER_SIZE);
	//man = generate_man(wrong_guesses);
	strcpy(message, "Sorry, you lose!");
	write(to_client, message, BUFFER_SIZE);
	printf("[subserver %d] Sent %s\n", pid, message);
	test = read(from_client, buffer, BUFFER_SIZE);
	if (test == -1 || strcmp(buffer, ACK)) {
	  printf("Error 6!");
	}
	buffer = zero_heap(buffer, BUFFER_SIZE);
	message = zero_heap(message, BUFFER_SIZE);
	return;
      }
      if(won == -2){
	strcpy(message,"You win!");
	write(to_client, message, BUFFER_SIZE);
	printf("[subserver %d] Sent %s\n", pid, message);
	test = read(from_client, buffer, BUFFER_SIZE);
	if (test == -1 || strcmp(buffer, ACK)) {
	  printf("Error 7!");
	}
	buffer = zero_heap(buffer, BUFFER_SIZE);
	message = zero_heap(message, BUFFER_SIZE);
	return;
      }
    }
    
}

int run_turn(int len,int *wrong_guessespointer, char* guessing_array, char* guessed_letters, int *index, char * word, int to_client, int from_client){
    int wrong_guesses = *wrong_guessespointer;
    int g = *index;
    //printf("LEN: %d\n\n",len);
    
    int pid = getpid();
    //input (guess) with potentially multiple characters and newline
    char input[100];
    //letter guessed
    char letter;
    // string containing hang man
    char * hangman;
    // to send number of wrong guesses
    char * man;
    // for messages to be sent to client
    char * message = (char *) calloc(BUFFER_SIZE, sizeof(char));
    char * buffer = (char *) calloc (BUFFER_SIZE, sizeof(char));
    int test;
    printf("[subserver %d] running turn...\n", pid);

    //print the man
    // sorta dangerous to write size below?
    hangman = (char *) calloc(BUFFER_SIZE,sizeof(char));
    strcpy(hangman,generate_man(wrong_guesses));
    write(to_client, hangman, BUFFER_SIZE);
    printf("[subserver %d] Sent %s\n", pid, hangman);
    test = read(from_client, buffer, BUFFER_SIZE);
    if (test == -1 || strcmp(buffer, ACK)) {
      printf("Error 0.5!");
    }
    buffer = zero_heap(hangman, BUFFER_SIZE);
	
    /*man = generate_man(wrong_guesses); */
    /* write(to_client, man, sizeof(char) * 100); */
    man = (char *)calloc(2,sizeof(char));
    sprintf(man, "%d", wrong_guesses);
    write(to_client, man, sizeof(man));
    printf("[subserver %d] Sent %s\n", pid, man);
    free(man);
    test = read(from_client, buffer, BUFFER_SIZE);

    if (test == -1 || strcmp(buffer, ACK)) {
      printf("Error 1!");
    }
    buffer = zero_heap(buffer, BUFFER_SIZE);

    //print the blank spaces for the word, with correct guesses filled in
    int i = 0;
    if(guessing_array[0] != 0){
      write(to_client, guessing_array, len);//sizeof(guessing_array));
      printf("[subserver %d] Sent %s\n", pid, guessing_array);
      test = read(from_client, buffer, BUFFER_SIZE);
      if (test == -1 || strcmp(buffer, ACK)) {
	printf("Error 2!");
      }
      buffer = zero_heap(buffer, BUFFER_SIZE);
    }

    //check for blank spaces in guessing_array
    // to see if the word was fully guessed already
    i = 0;
    //boolean for checking blank spaces
    int b = 0;
    for (;i < len; i++){
      if (guessing_array[i] == '_') {
	b = 1;
	break;
      }
    }

    //if b is 0, there were no blank spaces
    // word was already guessed, -1
    if (!b) {
      return -2;
    }

    int k = 1;
    while (k){

      //print the letters guessed already, if guesses were made
      i = 0;
      if (g){
	write(to_client, guessed_letters, sizeof(guessed_letters));
	printf("[subserver %d] Sent %s\n", pid, guessed_letters);
	test = read(from_client, buffer, BUFFER_SIZE);
	if (test == -1 || strcmp(buffer, ACK)) {
	  printf("Error 3!");
	}
	buffer = zero_heap(buffer, BUFFER_SIZE);
      }


      strcpy(message,"Pick a letter: ");
      write(to_client, message, BUFFER_SIZE);
      printf("[subserver %d] Sent %s\n", pid, message);
      message = zero_heap(message, BUFFER_SIZE);

      printf("[subserver %d] waiting for input\n", pid);
      //prompt input for a letter
      test = read(from_client, input, sizeof(input));
      printf("[subserver %d] received input {%s}\n", pid, input);

      //only first character inputed will be counted as letter guess
      letter = input[0];
      //update k because a guess was made
      k = 0;

      //if the character inputted was uppercase
      if(strchr("ABCDEFGHIJKLMNOPQRSTUVWXYZ",letter) != NULL){
	strcpy(message,"Please input a lowercase letter next time");
	write(to_client, message, BUFFER_SIZE);
	printf("[subserver %d] Sent %s\n", pid, message);
	test = read(from_client, buffer, BUFFER_SIZE);
	if (test == -1 || strcmp(buffer, ACK)) {
	  printf("Error 4!");
	}
	buffer = zero_heap(buffer, BUFFER_SIZE);
	message = zero_heap(message, BUFFER_SIZE);
	letter = tolower(letter);
      }

	    
      //if the guess was not a letter
      if (strchr("abcdefghijklmnopqrstuvwxyz",letter) == NULL){
	strcpy(message,"Not a valid letter");
	write(to_client, message, BUFFER_SIZE);
	printf("[subserver %d] Sent %s\n", pid, message);
	test = read(from_client, buffer, BUFFER_SIZE);
	if (test == -1 || strcmp(buffer, ACK)) {
	  printf("Error 5!");
	}
	buffer = zero_heap(buffer, BUFFER_SIZE);
	message = zero_heap(message, BUFFER_SIZE);
	k = 1;
      } 
      else{
	i = 0;
	for (;i < g;i++){
	  //if the letter was already guessed
	  // set k to 1 to prompt guess again
	  if (guessed_letters[i] == letter) {
	    k = 1;
	    strcpy(message,"Letter was previously guessed. Guess again.");
	    write(to_client, message, BUFFER_SIZE);
	    printf("[subserver %d] Sent %s\n", pid, message);
	    test = read(from_client, buffer, BUFFER_SIZE);
	    if (test == -1 || strcmp(buffer, ACK)) {
	      printf("Error lost count");
	    }
	    buffer = zero_heap(buffer, BUFFER_SIZE);
	    message = zero_heap(message, BUFFER_SIZE);
	  }
	}
      }
    }

    //update guessed_letters array with new guess

    guessed_letters[g] = letter;
    g++;


    //compare letter to each letter in word
    int j = 0;
    //boolean for if letter guessed was in word
    int t = 0;
    for(;j < len;j++){
      if(word[j] == letter){
	t = 1;
	guessing_array[j] = letter;
      }
    }

    //update wrong guess count if needed
    if(!t){
      wrong_guesses++;
    }

    *index = g;
    *wrong_guessespointer = wrong_guesses;

    return wrong_guesses;
}


void run_game(char * word, int to_client, int from_client){
    int pid = getpid();
    int wrong_guesses = 0;
    int len = strlen(word);
    //array for guessing the word, intially blank
    char * guessing_array = blank_array(len);
    //input (guess) with potentially multiple characters and newline
    char input[100];
    //letter guessed
    char letter;
    //array and counter for guessed letters
    char * guessed_letters = calloc(26,sizeof(char));
    //guessed_letters array index
    int g = 0;
    // string containing hang man
    char * hangman;
    // to send number of wrong guesses
    char * man;
    // for messages to be sent to client
    char * message = (char *) calloc(BUFFER_SIZE, sizeof(char));
    char * buffer = (char *) calloc (BUFFER_SIZE, sizeof(char));
    int test;
    printf("[subserver %d] running game...\n", pid);
    while (1){

        //print the man
        // sorta dangerous to write size below?
        hangman = (char *) calloc(BUFFER_SIZE,sizeof(char));
        strcpy(hangman,generate_man(wrong_guesses));
	write(to_client, hangman, BUFFER_SIZE);
	printf("[subserver %d] Sent %s\n", pid, hangman);
        test = read(from_client, buffer, BUFFER_SIZE);
        if (test == -1 || strcmp(buffer, ACK)) {
             printf("Error 0.5!");
        }
        buffer = zero_heap(hangman, BUFFER_SIZE);
	
        /*man = generate_man(wrong_guesses); */
	/* write(to_client, man, sizeof(char) * 100); */
        man = (char *)calloc(2,sizeof(char));
        sprintf(man, "%d", wrong_guesses);
        write(to_client, man, sizeof(man));
        printf("[subserver %d] Sent %s\n", pid, man);
        free(man);
        test = read(from_client, buffer, BUFFER_SIZE);

        //why is it -72?
        //the buffer is empty??
        //k it works now but im keeping this in just incase
        //printf("%s\n",buffer);

        if (test == -1 || strcmp(buffer, ACK)) {
            printf("Error 1!");
        }
        buffer = zero_heap(buffer, BUFFER_SIZE);

        //print the blank spaces for the word, with correct guesses filled in
        int i = 0;
        if(guessing_array[0] != 0){
            write(to_client, guessing_array, len);//sizeof(guessing_array));
            printf("[subserver %d] Sent %s\n", pid, guessing_array);
            test = read(from_client, buffer, BUFFER_SIZE);
            if (test == -1 || strcmp(buffer, ACK)) {
                printf("Error 2!");
            }
            buffer = zero_heap(buffer, BUFFER_SIZE);
        }

        //check for blank spaces in guessing_array
        // to see if the word was fully guessed already
        i = 0;
        //boolean for checking blank spaces
        int b = 0;
        for (;i < len; i++){
            if (guessing_array[i] == '_') {
                b = 1;
                break;
            }
        }

        //if b is 0, there were no blank spaces
        // word was already guessed, break
        if (!b) {
            break;
        }

        int k = 1;
        while (k){

            //print the letters guessed already, if guesses were made
            i = 0;
            if (g){
                write(to_client, guessed_letters, sizeof(guessed_letters));
                printf("[subserver %d] Sent %s\n", pid, guessed_letters);
                test = read(from_client, buffer, BUFFER_SIZE);
                if (test == -1 || strcmp(buffer, ACK)) {
                    printf("Error 3!");
                }
                buffer = zero_heap(buffer, BUFFER_SIZE);
            }


            strcpy(message,"Pick a letter: ");
            write(to_client, message, BUFFER_SIZE);
            printf("[subserver %d] Sent %s\n", pid, message);
            message = zero_heap(message, BUFFER_SIZE);

            printf("[subserver %d] waiting for input\n", pid);
            //prompt input for a letter
            test = read(from_client, input, sizeof(input));
            printf("[subserver %d] received input {%s}\n", pid, input);

            //only first character inputed will be counted as letter guess
            letter = input[0];
            //update k because a guess was made
            k = 0;

	    //if the character inputted was uppercase
	    if(strchr("ABCDEFGHIJKLMNOPQRSTUVWXYZ",letter) != NULL){
                strcpy(message,"Please input a lowercase letter next time");
                write(to_client, message, BUFFER_SIZE);
                printf("[subserver %d] Sent %s\n", pid, message);
                test = read(from_client, buffer, BUFFER_SIZE);
                if (test == -1 || strcmp(buffer, ACK)) {
                    printf("Error 4!");
                }
                buffer = zero_heap(buffer, BUFFER_SIZE);
                message = zero_heap(message, BUFFER_SIZE);
                letter = tolower(letter);
            }

	    
	    //if the guess was not a letter
            if (strchr("abcdefghijklmnopqrstuvwxyz",letter) == NULL){
                strcpy(message,"Not a valid letter");
                write(to_client, message, BUFFER_SIZE);
                printf("[subserver %d] Sent %s\n", pid, message);
                test = read(from_client, buffer, BUFFER_SIZE);
                if (test == -1 || strcmp(buffer, ACK)) {
                    printf("Error 5!");
                }
                buffer = zero_heap(buffer, BUFFER_SIZE);
                message = zero_heap(message, BUFFER_SIZE);
                k = 1;
            } 
	    else{
                 i = 0;
                 for (;i < g;i++){
		     //if the letter was already guessed
		     // set k to 1 to prompt guess again
		     if (guessed_letters[i] == letter) {
		        k = 1;
			strcpy(message,"Letter was previously guessed. Guess again.");
			write(to_client, message, BUFFER_SIZE);
			printf("[subserver %d] Sent %s\n", pid, message);
			test = read(from_client, buffer, BUFFER_SIZE);
			if (test == -1 || strcmp(buffer, ACK)) {
			  printf("Error lost count");
			}
			buffer = zero_heap(buffer, BUFFER_SIZE);
			message = zero_heap(message, BUFFER_SIZE);
                    }
                }
            }
        }

        //update guessed_letters array with new guess

        guessed_letters[g] = letter;
        g++;


        //compare letter to each letter in word
        int j = 0;
        //boolean for if letter guessed was in word
        int t = 0;
        for(;j < len;j++){
            if(word[j] == letter){
                t = 1;
                guessing_array[j] = letter;
            }
        }

        //update wrong guess count if needed
        if(!t){
            wrong_guesses++;
        }

        //check if player lost
        if(wrong_guesses == 6){
	    hangman = (char *) calloc(BUFFER_SIZE,sizeof(char));
            strcpy(hangman,generate_man(wrong_guesses));
	    write(to_client, hangman, BUFFER_SIZE);
	    printf("[subserver %d] Sent %s\n", pid, hangman);
	    test = read(from_client, buffer, BUFFER_SIZE);
	    if (test == -1 || strcmp(buffer, ACK)) {
	      printf("Error 5.5!");
	    }
	    buffer = zero_heap(hangman, BUFFER_SIZE);
            //man = generate_man(wrong_guesses);
            strcpy(message, "Sorry, you lose!");
            write(to_client, message, BUFFER_SIZE);
            printf("[subserver %d] Sent %s\n", pid, message);
            test = read(from_client, buffer, BUFFER_SIZE);
            if (test == -1 || strcmp(buffer, ACK)) {
                printf("Error 6!");
            }
            buffer = zero_heap(buffer, BUFFER_SIZE);
            message = zero_heap(message, BUFFER_SIZE);
            return;
        }
    }

    strcpy(message,"You win!");
    write(to_client, message, BUFFER_SIZE);
    printf("[sever %d] Sent %s\n", pid, message);
    test = read(from_client, buffer, BUFFER_SIZE);
    if (test == -1 || strcmp(buffer, ACK)) {
        printf("Error 7!");
    }
    buffer = zero_heap(buffer, BUFFER_SIZE);
    message = zero_heap(message, BUFFER_SIZE);
    return;
}

char * generate_man(int n){
    printf("[subserver] generating man...\n");
    char * man = (char *)calloc(100, sizeof(char));
    size_t size = 100 * sizeof(char);
    size_t line_len = strlen("       \n");
    strcpy(man, "\n  ____ \n");
    strncat(man, " |    |\n", size -= line_len + 1);
    //    printf(" O    |\n");
    //    printf("\|/   |\n");
    //    printf(" |    |\n");
    //    printf("/ \   |\n");
    //    printf("      |\n");
    //    printf("______|_\n");
    if (n == 0) {
        strncat(man, "      |\n", size-= line_len);
        strncat(man, "      |\n", size-= line_len);
        strncat(man, "      |\n", size-= line_len);
        strncat(man, "      |\n", size-= line_len);
    }
    if (n == 1) {
        strncat(man, " O    |\n", size-= line_len);
        strncat(man, "      |\n", size-= line_len);
        strncat(man, "      |\n", size-= line_len);
        strncat(man, "      |\n", size-= line_len);
    }
    if (n == 2) {
        strncat(man, " O    |\n", size-= line_len);
        strncat(man, " |    |\n", size-= line_len);
        strncat(man, " |    |\n", size-= line_len);
        strncat(man, "      |\n", size-= line_len);
    }
    if (n == 3) {
        strncat(man, " O    |\n", size-= line_len);
        strncat(man, "\\|    |\n", size-= line_len);
        strncat(man, " |    |\n", size-= line_len);
        strncat(man, "      |\n", size-= line_len);
    }
    if (n == 4) {
        strncat(man, " O    |\n", size-= line_len);
        strncat(man, "\\|/   |\n", size-= line_len);
        strncat(man, " |    |\n", size-= line_len);
        strncat(man, "      |\n", size-= line_len);
    }
    if (n == 5) {
        strncat(man, " O    |\n", size-= line_len);
        strncat(man, "\\|/   |\n", size-= line_len);
        strncat(man, " |    |\n", size-= line_len);
        strncat(man, "/     |\n", size-= line_len);
    }
    if (n == 6) {
        strncat(man, " O    |\n", size-= line_len);
        strncat(man, "\\|/   |\n", size-= line_len);
        strncat(man, " |    |\n", size-= line_len);
        strncat(man, "/ \\   |\n", size-= line_len);
    }
    strncat(man, "      |\n", size-= line_len);
    strncat(man, "______|_\n", size-= line_len);
    /* printf("%s", man); */
    return man;
}


/*int main(){
/*int main (int argc, char *argv[]){

char letter;
printf("Guess a letter: ");
scanf("%c", letter);

print_man(0);
printf("\n");
print_man(1);
printf("\n");
print_man(2);
printf("\n");
print_man(3);
printf("\n");
print_man(4);
printf("\n");
print_man(5);
printf("\n");
print_man(6);
printf("\n");*/

//run_game("fabulous");

//}*/
