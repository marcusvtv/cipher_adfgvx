#include "file_operations.h"
#include <stdio.h>
#include <string.h> // Para strcspn

int read_file(const char *filename, char *buffer, int max_length)
{
    FILE *file_ptr = fopen(filename, "r"); // Renomeado de 'file' para 'file_ptr' para clareza
    if (file_ptr == NULL)
    {
        // perror("Erro ao abrir arquivo para leitura"); // Opcional: main pode tratar isso
        return 1; // Erro ao abrir
    }

    if (fgets(buffer, max_length, file_ptr) == NULL)
    {
        // fgets retorna NULL em erro ou se o fim do arquivo for atingido antes de ler caracteres.
        fclose(file_ptr);
        return 2; // Erro ao ler com fgets ou arquivo vazio/problema
    }

    // Remove o caractere de nova linha ('\n') ou carriage return + nova linha ('\r\n')
    // que fgets pode incluir no final da string lida.
    buffer[strcspn(buffer, "\r\n")] = '\0';

    fclose(file_ptr);
    return 0; // Sucesso
}

int write_encrypted_data_to_file(const char *filename,
                                 int key_length,
                                 char encoded_symbol_matrix[][MAX_MESSAGE_LENGTH],
                                 int symbols_per_column[])
{
    FILE *output_file_ptr = fopen(filename, "w"); // Renomeado de encrypted_file
    if (output_file_ptr == NULL)
    {
        perror("Erro ao abrir arquivo para escrita da saida cifrada");
        return 1; // Erro ao abrir
    }

    // Escreve as colunas ordenadas (conforme estao na matriz apos transposicao) no arquivo
    for (int i = 0; i < key_length; i++)
    {
        for (int j = 0; j < symbols_per_column[i]; j++)
        {
            if (fputc(encoded_symbol_matrix[i][j], output_file_ptr) == EOF)
            {
                perror("Erro ao escrever no arquivo de saida cifrada");
                fclose(output_file_ptr);
                return 1; // Erro ao escrever
            }
        }
    }

    fclose(output_file_ptr);
    return 0; // Sucesso
}
