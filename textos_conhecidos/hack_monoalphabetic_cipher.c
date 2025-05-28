#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/stat.h>
#include <errno.h>
#include "text_formatation.h"

#define ALPHABET_SIZE 26
#define MAX_TEXT_SIZE 1000000 // 1MB
#define CIPHER_SIZE 120

struct monoalphabetic_transformation
{
    char origin;
    char destination;
};

struct monoalphabetic_key
{
    struct monoalphabetic_transformation *transformations;
    int size;
};

int *calculate_statistics(char *text, int initial_index, int final_index)
{
    int *statistics = calloc(ALPHABET_SIZE, sizeof(int));
    int i;
    for (i = initial_index; i < final_index; i++)
        statistics[text[i] - 'a']++;
    return statistics;
}

void swap(int *a, int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

void copy_statistics(const int *source, int *destination)
{
    int i;
    for (i = 0; i < ALPHABET_SIZE; i++)
        destination[i] = source[i];
}

int *sort_statistics(const int *statistics)
{
    int *sorted = malloc(ALPHABET_SIZE * sizeof(int));
    copy_statistics(statistics, sorted);
    int i, j;
    for (i = 0; i < ALPHABET_SIZE; i++)
        for (j = i + 1; j < ALPHABET_SIZE; j++)
            if (sorted[i] < sorted[j])
                swap(&sorted[i], &sorted[j]);

    return sorted;
}

void print_statistics(int *statistics, size_t size)
{
    for (int i = 0; i < ALPHABET_SIZE; i++)
        printf("[%c: %d]", 'a' + i, statistics[i]);
    printf("\n");
}

int compare_statistics(int *statistics1, int *statistics2)
{
    int i;
    for (i = 0; i < ALPHABET_SIZE; i++)
        if (statistics1[i] != statistics2[i])
            return 0;
    return 1;
}

char *extract_plaintext(char *text, int index)
{
    char *plaintext = malloc(CIPHER_SIZE + 1);
    strncpy(plaintext, text + index, CIPHER_SIZE);
    plaintext[CIPHER_SIZE] = '\0';
    return plaintext;
}

void print_key(struct monoalphabetic_key *key)
{
    for (int i = 0; i < key->size; i++)
        printf("%c -> %c\n", key->transformations[i].origin, key->transformations[i].destination);
}

char *decrypt_ciphertext(char *ciphertext, struct monoalphabetic_key *key)
{
    // Allocate extra space for color codes
    // Green for transformed characters -> "\032[32m" -> occupies 5 bytes
    // Red for untransformed characters -> "\032[31m" -> occupies 5 bytes
    // Reset color -> "\032[0m" -> occupies 4 bytes
    // Therefore, for each character displayed, we need to allocate 16 bytes
    // +1 for the null terminator
    char *decrypted_text = malloc(CIPHER_SIZE * 10 + 1);

    // Position in the output string
    int pos = 0;

    for (int i = 0; i < CIPHER_SIZE; i++)
    {
        int found = 0;
        for (int j = 0; j < key->size; j++)
        {
            if (key->transformations[j].destination == ciphertext[i])
            {
                pos += sprintf(decrypted_text + pos, "\033[32m%c\033[0m", key->transformations[j].origin);
                found = 1;
                break;
            }
        }
        if (!found)
        {
            pos += sprintf(decrypted_text + pos, "\033[31m%c\033[0m", ciphertext[i]);
        }
    }
    decrypted_text[pos] = '\0';
    return decrypted_text;
}

struct monoalphabetic_key *deduce_partial_key(int *current_plaintext_statistics, int *cipher_statistics)
{
    struct monoalphabetic_key *monoalphabetic_key = malloc(sizeof(struct monoalphabetic_key));
    monoalphabetic_key->transformations = malloc(ALPHABET_SIZE * sizeof(struct monoalphabetic_transformation));
    monoalphabetic_key->size = 0;

    int frequency_counts[CIPHER_SIZE] = {0};
    for (int i = 0; i < ALPHABET_SIZE; i++)
        frequency_counts[cipher_statistics[i]]++;

    for (int i = 0; i < ALPHABET_SIZE; i++)
    {
        int current_frequency = current_plaintext_statistics[i];
        if (frequency_counts[current_frequency] == 1)
        {
            for (int j = 0; j < ALPHABET_SIZE; j++)
            {
                if (cipher_statistics[j] == current_frequency)
                {
                    monoalphabetic_key->transformations[monoalphabetic_key->size].origin = 'a' + i;
                    monoalphabetic_key->transformations[monoalphabetic_key->size].destination = 'a' + j;
                    monoalphabetic_key->size++;
                    break;
                }
            }
        }
    }
    return monoalphabetic_key;
}

int main()
{
    char *formatted_text = malloc(MAX_TEXT_SIZE * sizeof(char));
    FILE *formatted_file = fopen("textos_formatados/avesso_da_pele.txt", "r");
    if (!formatted_file)
    {
        printf("Error opening known text file\n");
        free(formatted_text);
        return 1;
    }
    read_file(formatted_file, formatted_text);

    size_t known_text_length = strlen(formatted_text);
    int *cipher_statistics;
    int *sorted_cipher_statistics;
    char ciphertext[CIPHER_SIZE + 1];
    FILE *cipher_file = fopen("Cifrado/Mono/Grupo12_texto_cifrado.txt", "r");
    if (cipher_file)
    {
        fgets(ciphertext, CIPHER_SIZE + 1, cipher_file);
        ciphertext[CIPHER_SIZE] = '\0';
        cipher_statistics = calculate_statistics(ciphertext, 0, CIPHER_SIZE);
        sorted_cipher_statistics = sort_statistics(cipher_statistics);
        fclose(cipher_file);
    }
    int number_of_possible_plaintexts = known_text_length - CIPHER_SIZE + 1;
    int count_equal_statistics = 0;
    char *current_plaintext;
    for (int i = 0; i < number_of_possible_plaintexts; i++)
    {
        current_plaintext = extract_plaintext(formatted_text, i);
        int *current_plaintext_statistics = calculate_statistics(formatted_text, i, i + CIPHER_SIZE);
        int *sorted_current_plaintext_statistics = sort_statistics(current_plaintext_statistics);
        int is_equal = compare_statistics(sorted_cipher_statistics, sorted_current_plaintext_statistics);
        if (is_equal)
        {
            count_equal_statistics++;
            struct monoalphabetic_key *key = deduce_partial_key(current_plaintext_statistics, cipher_statistics);
            printf("--------------------------------\n");
            printf("Probable transformations: \n");
            print_key(key);

            char *decrypted_text = decrypt_ciphertext(ciphertext, key);
            printf("(Partially) Decrypted text with discovered transformations:\n %s\n", decrypted_text);
            printf("Plaintext: %s\n", current_plaintext);
            printf("PlainText Statistics: ");
            print_statistics(current_plaintext_statistics, CIPHER_SIZE);
            printf("Cipher statistics: ");
            print_statistics(cipher_statistics, CIPHER_SIZE);
            free(decrypted_text);
            free(key);
            printf("--------------------------------\n");
        }
        free(current_plaintext_statistics);
        free(sorted_current_plaintext_statistics);
    }
    if (count_equal_statistics == 1)
        printf("Exactly one possible plaintext found.\n%s\nNo need to try others transformations.\n", current_plaintext);
    else if (count_equal_statistics > 1)
        printf("Possible plaintexts found with the same statistics: %d\n", count_equal_statistics);
    else
        printf("No possible plaintext found\n");

    free(current_plaintext);
    free(formatted_text);
    return 0;
}