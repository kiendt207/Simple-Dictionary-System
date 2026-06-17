/*
    Module 1: Core Trie Engine
    Module 2: Spell Checker
    Module 3: Autocomplete
    Module 4: Persistence
    Module 5: Delete node
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define ALPHABET_SIZE 256
#define MAX_SUGGESTIONS 100
#define MAX_WORD_LEN 256

//Module 1: Basic prefix tree
typedef struct TrieNode{
    struct TrieNode *children[ALPHABET_SIZE];
    int endOfWord;
    int frequency;
}TrieNode;

TrieNode *createNode(){
    TrieNode *newNode = (TrieNode *)malloc(sizeof(TrieNode));

    if(newNode != NULL){
        newNode->endOfWord = 0;
        newNode->frequency = 0;

        for(int i = 0; i < ALPHABET_SIZE; i++)
            newNode->children[i] = NULL;
    }
    return newNode;
}

void insertWord(TrieNode *root, const char *word){
    TrieNode *current = root;
    int len = strlen(word);

    for(int i = 0; i < len; i++){
        unsigned char index = (unsigned char)tolower(word[i]);

        if(current->children[index] == NULL)
            current->children[index] = createNode();

        current = current->children[index];
    }

    current->endOfWord = 1;
    current->frequency++;
}

//Module 2: Spell Checker
int searchWord(TrieNode *root, const char *word){
    TrieNode *current = root;
    int len = strlen(word);

    for(int i = 0; i < len; i++){
        unsigned char index = (unsigned char)tolower(word[i]);

        if(current->children[index] == NULL)
            return 0;
        
        current = current->children[index];
    }

    return (current != NULL && current->endOfWord);
}

void loadDictionary(TrieNode *root, const char *filepath){
    FILE *file = fopen(filepath, "r");

    if(file == NULL) {
        printf("No file found\n");
        return;
    }

    char buffer[256];

    while(fgets(buffer, sizeof(buffer), file)){
        buffer[strcspn(buffer, "\r\n")] = '\0';

        if(strlen(buffer) > 0)
            insertWord(root, buffer);
    }

    fclose(file);
}

void spellChecker(TrieNode *root, const char *inputFile, const char *outputFile){
    FILE *in = fopen(inputFile, "r");
    FILE *out = fopen(outputFile, "w");

    if(!in || !out){
        printf("Error! Can not open file.\n");
        if(in) fclose(in);
        if(out) fclose(out);
        return;
    }

    printf("------ SPELL CHECKER ------\n");

    int c;
    char wordBuffer[256];
    int wIndex = 0;
    int errorCount = 0;

    while((c = fgetc(in)) != EOF){
        if(isalpha(c))
            wordBuffer[wIndex++] = (char)c;
        else{
            if(wIndex > 0){
                wordBuffer[wIndex] = '\0';

                if(!searchWord(root, wordBuffer)){
                    printf("Error: %s\n", wordBuffer);
                    fprintf(out, "%s\n", wordBuffer);
                    errorCount++;
                }
                wIndex = 0;
            }
        }
    }

    if(wIndex > 0){
        wordBuffer[wIndex] = '\0';
        if(!searchWord(root, wordBuffer)){
            printf("Error: %s", wordBuffer);
            fprintf(out, "%s\n", wordBuffer);
            errorCount++;
        }
    }

    fclose(in);
    fclose(out);

    printf("Error word count: %d\n", errorCount);
    printf("Save error word to: %s\n", outputFile);
}

//Module 3: Autocomplete
typedef struct suggestion{
    char word[MAX_WORD_LEN];
    int frequency;
}suggestion;

typedef struct queueItem{
    TrieNode *node;
    char prefix[MAX_WORD_LEN];
}queueItem;

int compareSuggestions(const void *a, const void *b){
    suggestion *s1 = (suggestion *)a;
    suggestion *s2 = (suggestion *)b;
    return (s2->frequency - s1->frequency);
}

void autocompleteBFS(TrieNode *root, const char *prefix){
    TrieNode *current = root;
    int len = strlen(prefix);

    for(int i = 0; i < len; i++){
        unsigned char index = (unsigned char)tolower(prefix[i]);

        if(current->children[index] == NULL){
            printf("No suggestion for %s.\n", prefix);
            return;
        }

        current = current->children[index];
    }

    suggestion results[MAX_SUGGESTIONS];
    int resultCount = 0;

    queueItem queue[100000];
    int front = 0, rear = 0;

    queue[rear].node = current;
    strcpy(queue[rear].prefix, prefix);
    rear++;

    while(front < rear && resultCount < MAX_SUGGESTIONS){
        queueItem currentItem = queue[front++];

        if(currentItem.node->endOfWord){
            strcpy(results[resultCount].word, currentItem.prefix);
            results[resultCount].frequency = currentItem.node->frequency;
            resultCount++;
        }

        for(int i = 0; i < ALPHABET_SIZE; i++){
            if(currentItem.node->children[i] != NULL){
                queue[rear].node = currentItem.node->children[i];

                strcpy(queue[rear].prefix, currentItem.prefix);
                int currLen = strlen(queue[rear].prefix);
                queue[rear].prefix[currLen] = (char)i;
                queue[rear].prefix[currLen + 1] = '\0';

                rear++;
            }
        }
    }
    
    if(resultCount > 0){
        qsort(results, resultCount, sizeof(suggestion), compareSuggestions);
        
        printf("Suggestion results: \n");
        for(int i = 0; i < resultCount; i++){
            printf("%d. %s\n", i + 1, results[i].word);
        }
    }
    else
        printf("No complete words found starting with '%s'.\n", prefix);
}

//Wildcard
void wildcardDFS(TrieNode *node, const char *pattern, int depth, char *wordBuffer){
    if(node == NULL) return;

    if(depth == strlen(pattern)){
        if(node->endOfWord){
            wordBuffer[depth] = '\0';
            printf("- %s\n", wordBuffer);
        }
        return;
    }

    char currentChar = pattern[depth];

    if(currentChar == '_'){
        for(int i = 0; i < ALPHABET_SIZE; i++){
            if(node->children[i] != NULL){
                wordBuffer[depth] = (char)i;
                wildcardDFS(node->children[i], pattern, depth + 1, wordBuffer);
            }
        }
    }

    else {
        unsigned char index = (unsigned char)tolower(currentChar);
        if(node->children[index] != NULL){
            wordBuffer[depth] = (char)index;
            wildcardDFS(node->children[index], pattern, depth + 1, wordBuffer);
        }
    }
}

void wildcardSearch (TrieNode *root, const char *pattern){
    char buffer[MAX_WORD_LEN];
    wildcardDFS(root, pattern, 0, buffer);
}

//Module 4: Persistence
void serializeDFS(TrieNode *node, char *buffer, int depth, FILE *file){
    if(node == NULL) return;

    if(node->endOfWord){
        buffer[depth] = '\0';
        fprintf(file, "%s, %d\n", buffer, node->frequency);
    }

    for(int i = 0; i < ALPHABET_SIZE; i++){
        if(node->children[i] != NULL){
            buffer[depth] = (char)i;
            serializeDFS(node->children[i], buffer, depth + 1, file);
        }
    }
}

void savePersonalDictionary(TrieNode *root, const char *filename){
    FILE *file = fopen(filename, "w");
    if(file == NULL){
        printf("No file found.\n");
        return;
    }

    char buffer[256];
    serializeDFS(root, buffer, 0, file);

    fclose(file);
    printf("Succesfully save personal dictionary.\n");
}

void insertWithFrequency (TrieNode *root, const char *word, int freq){
    TrieNode *current = root;
    int len = strlen(word);

    for(int i = 0; i < len; i++){
        unsigned char index = (unsigned char)tolower(word[i]);
        
        if(current->children[index] == NULL)
            current->children[index] = createNode();
        
        current = current->children[index];
    }

    current->endOfWord = 1;
    current->frequency = freq;
}

void loadPersonalDictionary(TrieNode *root, const char *filename){
    FILE *file = fopen(filename, "r");

    if(file == NULL){
        printf("No file found.\n");
        return;
    }

    char buffer[512];
    int count = 0;

    while(fgets(buffer, sizeof(buffer), file)){
        buffer[strcspn(buffer, "\r\n")] = '\0';

        char *wordPart = strtok(buffer, ",");
        char *freqPart = strtok(NULL, ",");

        if(wordPart != NULL && freqPart != NULL){
            int freq = atoi(freqPart);
            insertWithFrequency(root, wordPart, freq);
            count++;
        }
    }

    fclose(file);
    printf("Succesfully load personal dictionary. (%d words)\n", count);
}

void freeTrie(TrieNode *node){
    if(node == NULL) return;

    for(int i = 0; i < ALPHABET_SIZE; i++){
        if(node->children[i] != NULL){
            freeTrie(node->children[i]);
        }
    }

    free(node);
}


//Module 5: Delete word
int isEmpty(TrieNode *node){
    for(int i = 0; i < ALPHABET_SIZE; i++){
        if(node->children[i] != NULL){
            return 0;
        }
    }
    return 1;
}

TrieNode *removeDFS(TrieNode *node, const char *word, int depth){
    if(node == NULL)
        return NULL;
    
    if(depth == strlen(word)){
        if(node->endOfWord){
            node->endOfWord = 0;
            node->frequency = 0;
        }

        if(isEmpty(node)){
            free(node);
            return NULL;
        }

        return node;
    }

    unsigned char index = (unsigned char)tolower(word[depth]);
    node->children[index] = removeDFS(node->children[index], word, depth + 1);

    if(node->endOfWord == 0 && isEmpty(node)){
        free(node);
        return NULL;
    }

    return node;
}

void deleteWord(TrieNode *root, const char *word){
    if(searchWord(root, word) == 0){
        printf("'%s' is unavailable\n", word);
        return;
    }

    removeDFS(root, word, 0);
    printf("'%s' has been deleted", word);
}

int main() {
    printf("Loading...\n");

    TrieNode *root = createNode();

    loadDictionary(root, "words.txt");
    loadPersonalDictionary(root, "personal_dictionary.csv");

    printf("Set up complete.\n");

    int choice;
    char inputBuffer[256];

    do {
        printf("========================================\n");
        printf("           SIMPLE DICTIONARY            \n");
        printf("========================================\n");
        printf("1. Spell Checker\n");
        printf("2. Autocomplete / Wildcard Searching\n");
        printf("3. Add new words to your dictionary\n");
        printf("4. Save personal dictionary\n");
        printf("5. Delete word\n");
        printf("0. Exit program\n");
        printf("========================================\n");
        printf("Choose function (0-4): ");

        if(fgets(inputBuffer, sizeof(inputBuffer), stdin) != NULL)
            choice = atoi(inputBuffer);
        else 
            choice = -1;

        switch(choice){
            case 1:{
                printf("----- SPELL CHECKER -----\n");
                char inputFile[256], outputFile[256];

                printf("Enter input file name: ");
                if(fgets(inputFile, sizeof(inputFile), stdin))
                    inputFile[strcspn(inputFile, "\r\n")] = '\0';
                
                printf("Enter output file name: ");
                if(fgets(outputFile, sizeof(outputFile), stdin))
                    outputFile[strcspn(outputFile, "\r\n")] = '\0';
                
                spellChecker(root, inputFile, outputFile);
                break;
            }

            case 2:{
                printf("----- AUTOCOMPLETE & WILDCARD SEARCHING -----\n");
                printf("Enter a prefix or a wildcard (eg: app, c_r): ");

                char pattern[256];

                if(fgets(pattern, sizeof(pattern), stdin)){
                    pattern[strcspn(pattern, "\r\n")] = '\0';

                    if(strchr(pattern, '_') != NULL)
                        wildcardSearch(root, pattern);
                    else 
                        autocompleteBFS(root, pattern);
                }
                break;
            }

            case 3:{
                printf("------- ADD NEW WORDS -------\n");
                printf("Enter a word: ");

                char newWord[256];
                
                if(fgets(newWord, sizeof(newWord), stdin)){
                    newWord[strcspn(newWord, "\r\n")] = '\0';

                    insertWord(root, newWord);
                    
                    printf("Remember to save new words using function 4.\n");
                }
                break;
            }

            case 4:{
                printf("---- SAVE YOUR PERSONAL DICTIONARY ----\n");

                savePersonalDictionary(root, "personal_dictionary.csv");
                break;
            }

            case 5: {
                printf("------- DELETE WORD -------\n");
                printf("Enter delete word: ");

                char wordToDelete[256];
                
                if(fgets(wordToDelete, sizeof(wordToDelete), stdin)){
                    wordToDelete[strcspn(wordToDelete, "\r\n")] = '\0';

                    deleteWord(root, wordToDelete);

                    printf("Remember to save using function 4.\n");
                }
            }

            case 0:{
                printf("Cleaning memory...\n");
                freeTrie(root);
                printf("Clean succesfully.\n");
                printf("========================================\n");
                break;
            }

            default:{
                printf("Please choose function (0-4)\n");
                break;
            }
        }
    }while(choice != 0);

    return 0;
}