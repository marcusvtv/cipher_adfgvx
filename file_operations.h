#ifndef FILE_OPERATIONS_H
#define FILE_OPERATIONS_H

#include "cipher_config.h" // Para MAX_MESSAGE_LENGTH

/**
 * @brief Le o conteudo de um arquivo em um buffer.
 * Remove o caractere de nova linha ('\n' ou '\r\n') se presente no final.
 *
 * @param filename Caminho para o arquivo a ser lido.
 * @param buffer Buffer onde o conteudo sera armazenado.
 * @param max_length Tamanho maximo permitido do buffer (incluindo espaco para '\0').
 * @return int 0 em caso de sucesso, 1 se erro ao abrir o arquivo, 2 se fgets falhar ou ler nada.
 */
int read_file(const char *filename, char *buffer, int max_length);

/**
 * @brief Escreve a matriz de simbolos cifrados em um arquivo.
 *
 * @param filename Caminho para o arquivo onde a saida sera escrita.
 * @param key_length Comprimento da chave (que corresponde ao numero de colunas na matriz).
 * @param encoded_symbol_matrix Matriz [key_length][MAX_MESSAGE_LENGTH] contendo os simbolos cifrados.
 * @param symbols_per_column Vetor indicando quantos simbolos validos existem em cada coluna da matriz.
 * @return int 0 em caso de sucesso, 1 se erro ao abrir ou escrever no arquivo.
 */
int write_encrypted_data_to_file(const char *filename,
                                 int key_length,
                                 char encoded_symbol_matrix[][MAX_MESSAGE_LENGTH],
                                 int symbols_per_column[]);

#endif // FILE_OPERATIONS_H
