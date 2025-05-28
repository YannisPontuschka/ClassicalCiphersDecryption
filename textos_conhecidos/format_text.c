#include "text_formatation.h"
#include <stdio.h>
#include <stdlib.h>

// Format the text from the known file and save it to the formatted file
int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    char *raw_text = malloc(MAX_TEXT_SIZE * sizeof(char));
    char *formatted_text = malloc(MAX_TEXT_SIZE * sizeof(char));
    char *filepath = malloc(256 * sizeof(char));

    if (!raw_text || !formatted_text || !filepath)
    {
        printf("Memory allocation failed\n");
        free(raw_text);
        free(formatted_text);
        free(filepath);
        return 1;
    }

    snprintf(filepath, 256, "%s/%s", READ_DIRECTORY, argv[1]);
    FILE *known_file = fopen(filepath, "r");
    if (!known_file)
    {
        printf("Error opening known text file: %s\n", filepath);
        free(raw_text);
        free(formatted_text);
        free(filepath);
        return 1;
    }

    read_file(known_file, raw_text);
    format_text(raw_text, formatted_text);
    save_text(formatted_text, argv[1]);

    free(raw_text);
    free(formatted_text);
    free(filepath);
    return 0;
}