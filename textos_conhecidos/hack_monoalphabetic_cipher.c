#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/stat.h>
#include <errno.h>
#include "text_formatation.h"

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

struct encrypted_char *decrypt_ciphertext_partially(char *ciphertext, struct monoalphabetic_key *key)
{
    struct encrypted_char *partially_decrypted_text = malloc(CIPHER_SIZE * sizeof(struct encrypted_char));

    for (int i = 0; i < CIPHER_SIZE; i++)
    {
        int found = 0;
        for (int j = 0; j < key->size; j++)
        {
            if (key->transformations[j].destination == ciphertext[i])
            {
                partially_decrypted_text[i].character = key->transformations[j].origin;
                partially_decrypted_text[i].is_encrypted = 0;
                found = 1;
                break;
            }
        }
        if (!found)
        {
            partially_decrypted_text[i].character = ciphertext[i];
            partially_decrypted_text[i].is_encrypted = 1; // Still encrypted character
        }
    }
    return partially_decrypted_text;
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

int is_possible_plaintext_of_cipher(char *plain_text, struct encrypted_char *partially_decrypted_text)
{
    size_t length = strlen(plain_text);
    for (int i = 0; i < length; i++)
    {
        if (partially_decrypted_text[i].is_encrypted == 0 &&
            partially_decrypted_text[i].character != plain_text[i])
            return 0;
    }
    return 1;
}

void find_plaintext(char *open_text, char *cipher)
{
    size_t known_text_length = strlen(open_text);
    int *cipher_statistics = calculate_statistics(cipher, 0, CIPHER_SIZE);
    int *sorted_cipher_statistics = sort_statistics(cipher_statistics);
    int number_of_possible_plaintexts = known_text_length - CIPHER_SIZE + 1;
    int count_possible_plaintexts = 0;
    char *current_plaintext;
    for (int i = 0; i < number_of_possible_plaintexts; i++)
    {
        current_plaintext = extract_plaintext(open_text, i);
        int *current_plaintext_statistics = calculate_statistics(open_text, i, i + CIPHER_SIZE);
        int *sorted_current_plaintext_statistics = sort_statistics(current_plaintext_statistics);
        int is_equal = compare_statistics(sorted_cipher_statistics, sorted_current_plaintext_statistics);
        if (is_equal)
        {
            struct monoalphabetic_key *key = deduce_partial_key(current_plaintext_statistics, cipher_statistics);

            struct encrypted_char *partially_decrypted_text = decrypt_ciphertext_partially(cipher, key);

            if (is_possible_plaintext_of_cipher(current_plaintext, partially_decrypted_text))
            {
                count_possible_plaintexts++;
                printf("--------------------------------\n");
                printf("Possible Plaintext %d: %s\n", count_possible_plaintexts, current_plaintext);
                printf("Probable transformations: \n");
                print_key(key);
                printf("--------------------------------\n");
            }
            free(key);
            free(partially_decrypted_text);
        }
        free(current_plaintext_statistics);
        free(sorted_current_plaintext_statistics);
    }
    if (count_possible_plaintexts == 1)
        printf("Exactly one possible plaintext found.\n");
    else if (count_possible_plaintexts > 1)
        printf("Possible plaintexts found: %d\n", count_possible_plaintexts);
    else
        printf("No possible plaintext found\n");

    free(current_plaintext);
    free(open_text);
}

int main()
{
    char *open_text = load_text_file("textos_formatados/avesso_da_pele.txt");
    char *cipher = load_cipher_text("Cifrado/Mono/Grupo20_texto_cifrado.txt");
    find_plaintext(open_text, cipher);
    return 0;
}