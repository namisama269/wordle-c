#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <ctype.h>

#define NUM_ANSWERS 2315
#define WORD_LEN 5
#define NUM_GUESSES 6
#define MAX_INPUT_LEN 256
#define ALPHABET_LEN 26

#define NOT_IN_WORD 0
#define INCORRECT_POS 1
#define CORRECT_POS 2

char* get_random_word(FILE* answers);
bool is_valid_guess(FILE* guesses, char* guess);
void process_guess(char* guess, char* word, int guess_num, char guessed_words[NUM_GUESSES][WORD_LEN+2], 
int letter_states[NUM_GUESSES][WORD_LEN+2], bool is_letter_guessed[WORD_LEN], int alphabet_state[ALPHABET_LEN]);
bool word_contains(char* word, char ch, bool is_letter_guessed[WORD_LEN]);
void print_game(int guess_num, char guessed_words[NUM_GUESSES][WORD_LEN+2], 
int letter_states[NUM_GUESSES][WORD_LEN+2], int alphabet_state[ALPHABET_LEN]);
void set_print_color(int letter_state);

void green();
void yellow();
void reset();

int main() {
    // actual randomisation
    srand(time(NULL));

    // open possible answer and allowed guess files
    FILE* answers = fopen("answers.txt", "r");
    FILE* guesses = fopen("guesses.txt", "r");

    // generate random word for the game
    char* word = "troll";
    //char* word = get_random_word(answers);
    printf("%s\n\n", word);
    
    // variables
    int guess_num = 0;
    char curr_input[MAX_INPUT_LEN];
    char guessed_words[NUM_GUESSES][WORD_LEN+2];
    int letter_states[NUM_GUESSES][WORD_LEN+2];
    bool is_letter_guessed[WORD_LEN] = {false};
    int alphabet_state[ALPHABET_LEN] = {NOT_IN_WORD} ;

    // process user commands
    while (guess_num < NUM_GUESSES && fgets(curr_input, sizeof(curr_input), stdin)) {
        // remove newline
        int curr_len = strlen(curr_input);
        curr_input[curr_len-1] = '\0';
        curr_len = strlen(curr_input);

        //printf("%s\n", curr_input);

        if (strcmp(curr_input, "q") == 0) {
            printf("You quit\n");
            break;
        //} else if (curr_len != WORD_LEN || !is_valid_guess(answers, curr_input)) {
        //    printf("not valid word\n");
        } else {
            //printf("go next\n");
            process_guess(curr_input, word, guess_num, guessed_words, letter_states, is_letter_guessed,
            alphabet_state);
            print_game(guess_num, guessed_words, letter_states, alphabet_state);
            ++guess_num;
        }
    }


    // free
    free(word);

    // close files

    return 0;
}


char* get_random_word(FILE* answers) {
    char* line = malloc((WORD_LEN+2) * sizeof(char));
    int i = 0;
    int idx = rand() % NUM_ANSWERS;
    while (fgets(line, sizeof(line), answers)) {
        if (i == idx) {
            line[WORD_LEN] = '\0';
            return line;
        }
        ++i;
    }
    return NULL;
}

bool is_valid_guess(FILE* guesses, char* guess) {
    rewind(guesses);
    char line[WORD_LEN+2];
    while (fgets(line, sizeof(line), guesses)) {
        line[WORD_LEN] = '\0';
        //printf("%s\n", guess);
        //printf("%s\n", line);
        if (strcmp(guess, line) == 0) {
            return true;
        }
    }
    return false;
}

void process_guess(char* guess, char* word, int guess_num, char guessed_words[NUM_GUESSES][WORD_LEN+2], 
int letter_states[NUM_GUESSES][WORD_LEN+2], bool is_letter_guessed[WORD_LEN], int alphabet_state[ALPHABET_LEN]) {
    for (int i = 0; i < WORD_LEN; ++i) {
        guessed_words[guess_num][i] = guess[i];

        // letter is in the word and in correct position
        if (guess[i] == word[i]) {
            letter_states[guess_num][i] = CORRECT_POS;
            is_letter_guessed[i] = true;
        // letter is in the word but not in correct position
        } else if (word_contains(word, guess[i], is_letter_guessed)) {
            letter_states[guess_num][i] = INCORRECT_POS;
            //printf("%c is in the word!\n", guess[i]);
        // letter is not in the word
        } else {
            letter_states[guess_num][i] = NOT_IN_WORD;
            //printf("%c is not in the word!\n", guess[i]);
        }

        if (letter_states[guess_num][i] > alphabet_state[guess[i]-'a']) {
            alphabet_state[guess[i]-'a'] = letter_states[guess_num][i];
        }
    }
}

bool word_contains(char* word, char ch, bool is_letter_guessed[WORD_LEN]) {
    for (int i = 0; i < WORD_LEN; ++i) {
        if (!is_letter_guessed[i] && word[i] == ch) {
            is_letter_guessed[i] = true;
            return true;
        }
    }
    return false;
}

void print_game(int guess_num, char guessed_words[NUM_GUESSES][WORD_LEN+2], 
int letter_states[NUM_GUESSES][WORD_LEN+2], int alphabet_state[ALPHABET_LEN]) {
    printf("\n");
    for (int i = 0; i < NUM_GUESSES; ++i) {
        if (i <= guess_num) {
            for (int j = 0; j < WORD_LEN; ++j) {
                set_print_color(letter_states[i][j]);
                printf("%c", toupper(guessed_words[i][j]));
                if (j == WORD_LEN-1) {
                    printf("\n");
                } else {
                    printf(" ");
                }
                reset();
            }
        } else {
            printf("_ _ _ _ _\n");
        }
    }
    printf("\n");

    // print alphabet
    for (int i = 0; i < ALPHABET_LEN; ++i) {
        set_print_color(alphabet_state[i]);
        printf("%c ", 'A'+i);
        reset();
    }
    printf("\n\n");
}

void set_print_color(int letter_state) {
    switch(letter_state) {
        case INCORRECT_POS:
            yellow(); break;
        case CORRECT_POS:
            green(); break;
        default:
            break;
    }
}

void green() {
    printf("\033[0;32m");
}
void yellow() {
    printf("\033[0;33m");
}
void reset() {
    printf("\033[0m");
}