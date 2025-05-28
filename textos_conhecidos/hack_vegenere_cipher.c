#include <stdio.h>
#include <stdlib.h>
#include "text_formatation.h"

int main()
{
    char *formatted_text = malloc(MAX_TEXT_SIZE * sizeof(char));
    FILE *known_file = fopen("textos_formatados/avesso_da_pele.txt", "r");
    read_file(known_file, formatted_text);
    printf("%.10s...\n", formatted_text);
    free(formatted_text);
    return 0;
}
