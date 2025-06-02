#include "cipher_config.h"
#include "adfgvx_decipher.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Constantes symbols e square são necessárias para a decifragem
// e são encapsuladas neste módulo como static.
static const char symbols[6] = {'A', 'D', 'F', 'G', 'V', 'X'};
static const char square[6][6] = {
    {'A', 'B', 'C', 'D', 'E', 'F'},
    {'G', 'H', 'I', 'J', 'K', 'L'},
    {'M', 'N', 'O', 'P', 'Q', 'R'},
    {'S', 'T', 'U', 'V', 'W', 'X'},
    {'Y', 'Z', ' ', ',', '.', '1'},
    {'2', '3', '4', '5', '6', '7'}};

/**
 * @brief Retorna o índice de um símbolo ADFGVX dentro do vetor `symbols`.
 * (Função auxiliar estática)
 */
static int symbol_index(char c)
{
    for (int i = 0; i < 6; i++)
    {
        if (symbols[i] == c)
            return i;
    }
    return -1; // Símbolo não encontrado
}

/**
 * @brief Reconstrói as colunas originais da cifra com base na chave de transposição.
 * (Função auxiliar estática - lógica fornecida pelo utilizador)
 */
static void reverse_transposition(char *input, char *key, int key_length, char columns[][MAX_MESSAGE_LENGTH], int col_counts[])
{
    if (key_length <= 0) { // Proteção contra key_length inválido
        // Zera col_counts se key_length for inválido para evitar uso de dados não inicializados
        // No entanto, a função que chama deve garantir key_length > 0.
        // Se key_length é usado para dimensionar 'order', ele deve ser > 0.
        return;
    }

    int len = strlen(input);
    if (len == 0) { // Se a entrada estiver vazia, não há nada a fazer.
        for(int i=0; i<key_length; ++i) col_counts[i] = 0;
        return;
    }
    int rows = len / key_length;
    int extra = len % key_length;

    // 1) monta o vetor order[] = {0,1,2,...,key_length-1}
    int order[key_length]; // VLA
    for (int i = 0; i < key_length; i++) {
        order[i] = i;
    }

    // 2) ordena order[] de acordo com key[order[j]], para obter a ordem alfabética
    //    dos índices originais da chave.
    for (int i = 0; i < key_length - 1; i++) {
        for (int j = 0; j < key_length - i - 1; j++) {
            if (key[order[j]] > key[order[j + 1]]) {
                int tmp = order[j];
                order[j] = order[j + 1];
                order[j + 1] = tmp;
            }
        }
    }
    // Agora order[i] = índice original da i-ésima coluna EM ORDEM ALFABÉTICA da chave.

    // 3) determina quantos símbolos cada coluna (identificada pelo seu índice original) vai ter.
    //    A lógica fornecida: a coluna “orig_index” recebeu rows+1 símbolos se orig_index < extra.
    //    Isto significa que as primeiras 'extra' colunas *na ordem original da chave* são as mais longas.
    //    Isto é diferente da forma como a cifragem normalmente distribui (onde as primeiras 'extra'
    //    colunas *na ordem alfabética da chave* são as mais longas ao ler o texto cifrado linearmente).
    //    No entanto, vamos seguir a lógica fornecida.
    //    Para que esta lógica funcione, o texto cifrado (input) deve ter sido construído
    //    de forma que as colunas correspondentes aos primeiros 'extra' índices originais da chave
    //    foram preenchidas com 'rows+1' símbolos, e as restantes com 'rows' símbolos,
    //    E ENTÃO estas colunas foram concatenadas na ordem alfabética da chave.
    //    Se for assim, então, ao ler o 'input', precisamos saber o comprimento de cada
    //    coluna *na ordem alfabética*.
    //    A i-ésima coluna na ordem alfabética (que originalmente era a coluna order[i])
    //    terá rows+1 símbolos se ELA (a i-ésima coluna alfabética) for uma das 'extra' colunas.
    //    Portanto, o comprimento da coluna order[i] é rows + (i < extra ? 1 : 0).
    //    O código do utilizador faz: col_counts[orig_index] = rows + (orig_index < extra ? 1 : 0);
    //    Vou manter a lógica do utilizador como solicitada.
    for (int i = 0; i < key_length; i++)
    {
        int orig_index = order[i]; // orig_index é o índice da coluna original que é a i-ésima na ordem alfabética
        col_counts[orig_index] = rows + (orig_index < extra ? 1 : 0);
    }


    // 4) preenche cada “columns[orig_index]” lendo do texto cifrado linearizado
    //    O texto cifrado (input) é a concatenação das colunas na ordem alfabética da chave.
    //    A i-ésima coluna na ordem alfabética da chave é a coluna original order[i].
    //    O comprimento desta i-ésima coluna alfabética é o que precisamos para ler do input.
    //    Este comprimento NÃO é col_counts[order[i]] (que foi calculado acima com base em orig_index < extra).
    //    Precisamos do comprimento da i-ésima coluna *alfabética*.
    //    Seja `len_alpha[i]` o comprimento da i-ésima coluna alfabética.
    //    `len_alpha[i] = rows + (i < extra ? 1 : 0);`
    //    Então, lemos `len_alpha[i]` caracteres do `input` e colocamos em `columns[order[i]]`.
    //    A lógica do utilizador no passo 4 é:
    //    `int col_index = order[i];`
    //    `for (int j = 0; j < col_counts[col_index]; j++) { columns[col_index][j] = input[pos++]; }`
    //    Isto usa `col_counts[col_index]` (onde `col_index` é o índice original) como o número de
    //    caracteres a serem lidos do `input` para a coluna `columns[col_index]`.
    //    Isto implica que `col_counts[k]` deve ser o comprimento da coluna `k` quando ela é lida
    //    do `input` (ou seja, quando a coluna `k` aparece na ordem alfabética da chave).
    //    Portanto, o passo 3 do utilizador está correto se `col_counts[orig_index]` for interpretado
    //    como o comprimento da coluna `orig_index` *quando ela é lida do texto cifrado linear*.
    //    Mas o texto cifrado é lido coluna alfabética por coluna alfabética.
    //    A i-ésima coluna alfabética tem `rows + (i < extra ? 1 : 0)` caracteres.
    //    Esta i-ésima coluna alfabética corresponde à coluna original `order[i]`.
    //    Então, `columns[order[i]]` é preenchida com `rows + (i < extra ? 1 : 0)` caracteres.
    //    E `col_counts[order[i]]` deve ser igual a `rows + (i < extra ? 1 : 0)`.
    //    A lógica do utilizador no passo 3: `col_counts[orig_index] = rows + (orig_index < extra ? 1 : 0);`
    //    A lógica do utilizador no passo 4: usa `col_counts[col_index]` (onde `col_index = order[i]`).
    //    Vou implementar exatamente como o utilizador forneceu, assumindo que a sua cifragem
    //    produz um `input` que é decifrável por esta lógica.

    // Limpa col_counts antes de recalcular, para garantir que não há lixo.
    memset(col_counts, 0, key_length * sizeof(int));

    // Passo 3 do utilizador (calcula o tamanho de cada coluna original)
    for (int i = 0; i < key_length; i++) {
        int orig_index = order[i]; // order[i] é o índice original da i-ésima coluna alfabética
        // O utilizador atribui o comprimento à coluna original baseado no seu próprio índice original.
        col_counts[orig_index] = rows + (orig_index < extra ? 1 : 0);
    }

    // Passo 4 do utilizador (preenche as colunas)
    int pos = 0;
    for (int i = 0; i < key_length; i++) { // Itera sobre as colunas na ordem alfabética da chave
        int original_col_index_to_fill = order[i]; // Esta é a coluna original que estamos a preencher agora
        // O número de caracteres a ler do 'input' para esta coluna (original_col_index_to_fill)
        // é o seu tamanho, que foi calculado em col_counts[original_col_index_to_fill].
        int num_chars_for_this_col = col_counts[original_col_index_to_fill];

        for (int j = 0; j < num_chars_for_this_col; j++) {
            if (pos < len) {
                columns[original_col_index_to_fill][j] = input[pos++];
            } else {
                // Erro: input mais curto que o esperado pelos comprimentos das colunas.
                // Preencher o resto com nulos ou retornar erro?
                // Por agora, apenas para.
                return;
            }
        }
    }
}


/**
 * @brief Reverte a organizacao em colunas, reconstruindo a sequencia de simbolos linha a linha.
 * (Funcao auxiliar estatica - logica do utilizador)
 */
static void reverse_polybius(char columns[][MAX_MESSAGE_LENGTH], int col_counts[], int key_length, char *output)
{
    if (key_length <= 0 || !output) {
        if (output) output[0] = '\0';
        return;
    }

    int max_rows = 0, pos = 0;

    for (int i = 0; i < key_length; i++)
    {
        if (col_counts[i] > max_rows)
            max_rows = col_counts[i];
    }

    for (int r = 0; r < max_rows; r++)
    {
        for (int c = 0; c < key_length; c++) // Itera sobre as colunas na ordem original da chave
        {
            if (r < col_counts[c]) // Se a coluna 'c' tiver um simbolo na linha 'r'
            {
                if (pos < MAX_MESSAGE_LENGTH * 2) { // Protege o buffer de saida 'output' (rearranged)
                    output[pos++] = columns[c][r];
                } else {
                    if (output) output[pos] = '\0'; // Tenta terminar a string
                    return; // Buffer de saida cheio
                }
            }
        }
    }
    if (output) output[pos] = '\0'; // Termina a string de saida
}

/**
 * @brief Decodifica pares de simbolos ADFGVX em caracteres da matriz Polybius.
 * (Funcao auxiliar estatica - logica do utilizador)
 */
static void decode_symbols(char *pairs, char *message)
{
    if (!pairs || !message) return;

    int len = strlen(pairs);
    int msg_index = 0;

    if (len % 2 != 0) {
        message[0] = '\0'; // Nao pode decodificar numero impar de simbolos
        return;
    }

    for (int i = 0; i < len; i += 2)
    {
        int row = symbol_index(pairs[i]);
        int col = symbol_index(pairs[i + 1]);
        if (row >= 0 && col >= 0) // Se o par de simbolos e valido
        {
            if (msg_index < MAX_MESSAGE_LENGTH - 1) { // Protege o buffer de saida 'message'
                message[msg_index++] = square[row][col];
            } else {
                break; // Buffer de mensagem cheio
            }
        } else {
            // Par de simbolos invalido.
            // O comportamento original era parar. Mantendo isso.
            break;
        }
    }
    message[msg_index] = '\0'; // Termina a string da mensagem decifrada
}

// Implementacao da funcao publica
void decipher_adfgvx(char *encrypted_text, char *key, int key_length, char *output)
{
    // Validacao basica de parametros
    if (!encrypted_text || !key || !output || key_length <= 0 || key_length >= MAX_KEY_LENGTH) {
        if (output) output[0] = '\0';
        return;
    }
    if (strlen(encrypted_text) == 0) {
        output[0] = '\0';
        return;
    }

    // VLAs para 'columns' e 'col_counts'.
    char columns[key_length][MAX_MESSAGE_LENGTH];
    int col_counts[key_length];
    // É crucial zerar col_counts e columns antes de usá-los,
    // especialmente porque reverse_transposition pode não preencher todas as partes se len for 0.
    memset(col_counts, 0, key_length * sizeof(int));
    for(int i=0; i<key_length; ++i) {
        memset(columns[i], 0, MAX_MESSAGE_LENGTH * sizeof(char));
    }

    // Buffer para a sequencia de simbolos apos reverter a transposicao.
    // O tamanho máximo é o mesmo do texto cifrado (que pode ser até MAX_MESSAGE_LENGTH * 2).
    char rearranged_symbols[MAX_MESSAGE_LENGTH * 2 + 1]; // +1 para o nulo

    reverse_transposition(encrypted_text, key, key_length, columns, col_counts);
    reverse_polybius(columns, col_counts, key_length, rearranged_symbols);
    decode_symbols(rearranged_symbols, output);
}
