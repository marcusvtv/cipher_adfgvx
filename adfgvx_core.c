#include "adfgvx_core.h"
#include <string.h> // Necessário para strlen, se usado (embora key_length seja passado)
#include <stdio.h>  // Para debugging ou perror, se necessário (geralmente evitado em módulos core)

// Constantes da cifra ADFGVX, encapsuladas neste módulo.

static const char symbols[6] = {'A', 'D', 'F', 'G', 'V', 'X'};
static const char square[6][6] = {
    {'A', 'B', 'C', 'D', 'E', 'F'},
    {'G', 'H', 'I', 'J', 'K', 'L'},
    {'M', 'N', 'O', 'P', 'Q', 'R'},
    {'S', 'T', 'U', 'V', 'W', 'X'},
    {'Y', 'Z', ' ', ',', '.', '1'},
    {'2', '3', '4', '5', '6', '7'}};

/**
 * @brief Encontra os simbolos ADFGVX correspondentes a um caractere.
 * Função auxiliar estática, interna a este módulo.
 *
 * @param c Caractere a ser cifrado.
 * @param row Ponteiro para armazenar o simbolo da linha.
 * @param col Ponteiro para armazenar o simbolo da coluna.
 * @return int Retorna 1 se o caractere foi encontrado, 0 caso contrario.
 */
static int get_adfgvx_symbols(char c, char *row, char *col)
{
    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < 6; j++)
        {
            if (square[i][j] == c)
            {
                *row = symbols[i];
                *col = symbols[j];
                return 1;
            }
        }
    }
    return 0;
}

/**
 * @brief Insere um simbolo ADFGVX na matriz de colunas.
 * Função auxiliar estática, interna a este módulo.
 *
 * @param key_length Comprimento da chave.
 * @param symbol Simbolo a ser inserido (row ou col).
 * @param symbol_count Contador global de simbolos (sera incrementado).
 * @param encoded_symbol_matrix Matriz de saida contendo os simbolos organizados por coluna.
 * @param symbols_per_column Vetor com a quantidade de simbolos por coluna (sera atualizado).
 */
static void insert_symbol_to_column(int key_length, char symbol, int *symbol_count, char encoded_symbol_matrix[][MAX_MESSAGE_LENGTH], int symbols_per_column[])
{
    int col_index = (*symbol_count) % key_length;
    int write_pos = symbols_per_column[col_index];

    encoded_symbol_matrix[col_index][write_pos] = symbol;
    symbols_per_column[col_index]++;
    (*symbol_count)++;
}

/**
 * @brief Converte a mensagem em colunas de simbolos ADFGVX para cifra por transposicao.
 * Função auxiliar estática, interna a este módulo.
 *
 * @param key_length Comprimento da chave.
 * @param message Mensagem original a ser cifrada.
 * @param encoded_symbol_matrix Matriz onde os simbolos cifrados serao armazenados por coluna.
 * @param symbols_per_column Vetor que armazena o numero de elementos em cada coluna.
 * Este vetor deve ser zerado pelo chamador antes desta função.
 */
static void polybius_encode_to_columns(int key_length, char message[], char encoded_symbol_matrix[][MAX_MESSAGE_LENGTH], int symbols_per_column[])
{
    int i;
    int current_symbol_count = 0; // Renomeado de symbol_count para evitar shadowing se fosse global

    for (i = 0; message[i] != '\0'; i++)
    {
        char r_symbol, c_symbol; // Nomes de variáveis locais para clareza

        if (!get_adfgvx_symbols(message[i], &r_symbol, &c_symbol))
        {
            // No código original, caracteres não encontrados são simplesmente ignorados.
            // Poderia adicionar um log ou um contador de caracteres ignorados se necessário.
            continue;
        }

        insert_symbol_to_column(key_length, r_symbol, &current_symbol_count, encoded_symbol_matrix, symbols_per_column);
        insert_symbol_to_column(key_length, c_symbol, &current_symbol_count, encoded_symbol_matrix, symbols_per_column);
    }
}

/**
 * @brief Reorganiza as colunas da matriz com base na ordem alfabetica da chave.
 * Função auxiliar estática, interna a este módulo.
 *
 * @param key A chave usada na transposicao (array de caracteres).
 * @param key_length Comprimento da chave.
 * @param encoded_symbol_matrix Matriz com os dados cifrados por colunas.
 * @param symbols_per_column Vetor com o numero de elementos em cada coluna.
 */
static void transpose_columns_by_key_order(char key[], int key_length, char encoded_symbol_matrix[][MAX_MESSAGE_LENGTH], int symbols_per_column[])
{
    int i, j, k;
    // Usa VLA (Variable Length Array) para sorted_key, requer C99 ou posterior.
    // Se precisar de compatibilidade C89/90, use um buffer de tamanho MAX_KEY_LENGTH.
    char sorted_key_chars[key_length]; // Não precisa de +1 se não for usada como string com funções de string.h
    int temp_s_count; // Renomeado de temp_count

    // Copia a chave original para sorted_key_chars para ordenação
    for (i = 0; i < key_length; i++)
    {
        sorted_key_chars[i] = key[i];
    }

    // Ordenacao da chave (Bubble Sort) e reorganizacao simultanea das colunas da matriz
    // e dos contadores em symbols_per_column.
    for (i = 0; i < key_length - 1; i++)
    {
        for (j = 0; j < key_length - i - 1; j++)
        {
            if (sorted_key_chars[j] > sorted_key_chars[j + 1])
            {
                // Troca os caracteres na copia da chave que esta sendo ordenada
                char temp_char = sorted_key_chars[j];
                sorted_key_chars[j] = sorted_key_chars[j + 1];
                sorted_key_chars[j + 1] = temp_char;

                // Troca as colunas correspondentes na encoded_symbol_matrix.
                // A troca é feita para a coluna inteira (até MAX_MESSAGE_LENGTH).
                // A leitura posterior será limitada por symbols_per_column.
                for (k = 0; k < MAX_MESSAGE_LENGTH; k++)
                {
                    char temp_matrix_char = encoded_symbol_matrix[j][k];
                    encoded_symbol_matrix[j][k] = encoded_symbol_matrix[j + 1][k];
                    encoded_symbol_matrix[j + 1][k] = temp_matrix_char;
                }

                // Troca os contadores de simbolos para as colunas correspondentes
                temp_s_count = symbols_per_column[j];
                symbols_per_column[j] = symbols_per_column[j + 1];
                symbols_per_column[j + 1] = temp_s_count;
            }
        }
    }
}

// Implementação da função pública
void cipher_adfgvx(char key[], int key_length, char message[], char encoded_symbol_matrix[][MAX_MESSAGE_LENGTH], int symbols_per_column[])
{
    // É responsabilidade do chamador (main) garantir que symbols_per_column
    // esteja inicializado com zeros antes de chamar esta função.
    polybius_encode_to_columns(key_length, message, encoded_symbol_matrix, symbols_per_column);
    transpose_columns_by_key_order(key, key_length, encoded_symbol_matrix, symbols_per_column);
}
