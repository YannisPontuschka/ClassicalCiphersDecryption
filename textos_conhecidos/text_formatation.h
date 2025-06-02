#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/stat.h>
#include <errno.h>

#define ALPHABET_SIZE 26
#define MAX_TEXT_SIZE 1000000 // 1MB
#define CIPHER_SIZE 120
#define SAVE_DIRECTORY "textos_formatados"
#define READ_DIRECTORY "textos"

struct encrypted_char
{
    char character;
    int is_encrypted;
};

// Function to convert accented characters to non-accented ones
char normalize_char(unsigned char c)
{
    // á (225), à (224), ã (227), â (226), ä (228)
    if (c >= 224 && c <= 228)
        return 'a';

    // é (233), è (232), ê (234), ë (235)
    if (c >= 232 && c <= 235)
        return 'e';

    // í (237), ì (236), î (238), ï (239)
    if (c >= 236 && c <= 239)
        return 'i';

    // ó (243), ò (242), õ (245), ô (244), ö (246)
    if (c >= 242 && c <= 246)
        return 'o';

    // ú (250), ù (249), û (251), ü (252)
    if (c >= 249 && c <= 252)
        return 'u';

    // ý (253), ÿ (255)
    if (c == 253 || c == 255)
        return 'y';

    // ñ (241)
    if (c == 241)
        return 'n';

    // ç (231)
    if (c == 231)
        return 'c';

    // Uppercase versions
    // Á (193), À (192), Ã (195), Â (194), Ä (196)
    if (c >= 192 && c <= 196)
        return 'a';

    // É (201), È (200), Ê (202), Ë (203)
    if (c >= 200 && c <= 203)
        return 'e';

    // Í (205), Ì (204), Î (206), Ï (207)
    if (c >= 204 && c <= 207)
        return 'i';

    // Ó (211), Ò (210), Õ (213), Ô (212), Ö (214)
    if (c >= 210 && c <= 214)
        return 'o';

    // Ú (218), Ù (217), Û (219), Ü (220)
    if (c >= 217 && c <= 220)
        return 'u';

    // Ý (221)
    if (c == 221)
        return 'y';

    // Ñ (209)
    if (c == 209)
        return 'n';

    // Ç (199)
    if (c == 199)
        return 'c';

    return c;
}

void format_text(char *input, char *output)
{
    int j = 0;
    for (int i = 0; input[i] != '\0'; i++)
    {
        unsigned char c = (unsigned char)input[i];
        c = normalize_char(c);

        if (isalpha(c))
        {
            output[j] = tolower(c);
            j++;
        }
    }
    output[j] = '\0';
}

char *load_text_file(char *file_path)
{
    char *text = malloc(MAX_TEXT_SIZE * sizeof(char));
    FILE *file = fopen(file_path, "r");
    if (!file)
    {
        printf("Error opening known text file\n");
        free(text);
        return NULL;
    }
    size_t total_read = 0;
    size_t bytes_read;
    while (!feof(file) && total_read < MAX_TEXT_SIZE - 1)
    {
        bytes_read = fread(text + total_read, 1, 1024, file);
        total_read += bytes_read;
    }
    text[total_read] = '\0';
    fclose(file);
    return text;
}

char *load_cipher_text(char *file_path)
{
    char *cipher_text = malloc((CIPHER_SIZE + 1) * sizeof(char));
    FILE *cipher_file = fopen(file_path, "r");
    if (cipher_file)
    {
        fgets(cipher_text, CIPHER_SIZE + 1, cipher_file);
        cipher_text[CIPHER_SIZE] = '\0';
        fclose(cipher_file);
    }
    return cipher_text;
}

void ensure_directory_exists(const char *path)
{
    struct stat st = {0};
    if (stat(path, &st) == -1)
        mkdir(path, 0700);
}

void save_text(const char *text, const char *filename)
{
    ensure_directory_exists(SAVE_DIRECTORY);
    char filepath[256];
    snprintf(filepath, sizeof(filepath), "%s/%s", SAVE_DIRECTORY, filename);

    FILE *file = fopen(filepath, "w");
    if (!file)
    {
        printf("Error creating file %s: %s\n", filepath, strerror(errno));
        return;
    }

    fprintf(file, "%s", text);
    fclose(file);
    printf("Formatted text saved to %s\n", filepath);
}