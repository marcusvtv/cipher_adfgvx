#include "file_operations.h"
#include <stdio.h>
#include <string.h> // Para strcspn

int read_file(const char *filename, char *buffer, int max_length)
{
    FILE *file_ptr = fopen(filename, "r");
    if (file_ptr == NULL)
    {
        return 1;
    }

    if (fgets(buffer, max_length, file_ptr) == NULL)
    {
        fclose(file_ptr);
        return 2;
    }

    buffer[strcspn(buffer, "\r\n")] = '\0';

    fclose(file_ptr);
    return 0;
}

int write_encrypted_data_to_file(const char *filename,
                                 int key_length,
                                 char encoded_symbol_matrix[][MAX_MESSAGE_LENGTH],
                                 int symbols_per_column[])
{
    FILE *output_file_ptr = fopen(filename, "w");
    if (output_file_ptr == NULL)
    {
        perror("Erro ao abrir arquivo para escrita da saida cifrada");
        return 1;
    }

    for (int i = 0; i < key_length; i++)
    {
        for (int j = 0; j < symbols_per_column[i]; j++)
        {
            if (fputc(encoded_symbol_matrix[i][j], output_file_ptr) == EOF)
            {
                perror("Erro ao escrever no arquivo de saida cifrada");
                fclose(output_file_ptr);
                return 1;
            }
        }
    }

    fclose(output_file_ptr);
    return 0;
}

int write_plaintext_to_file(const char *filename, const char *plaintext_message)
{
    FILE *output_file_ptr = fopen(filename, "w");
    if (output_file_ptr == NULL)
    {
        perror("Erro ao abrir arquivo para escrita do texto plano");
        return 1; // Erro ao abrir
    }

    if (fputs(plaintext_message, output_file_ptr) == EOF)
    {
        perror("Erro ao escrever texto plano no arquivo");
        fclose(output_file_ptr);
        return 1; // Erro ao escrever
    }

    fclose(output_file_ptr);
    return 0; // Sucesso
}
