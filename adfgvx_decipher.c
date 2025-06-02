#include "adfgvx_decipher.h"
#include <stdio.h>
#include <string.h>

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
 * (Função auxiliar estática)
 */
static void reverse_transposition(char *input, char *key, int key_length, char columns[][MAX_MESSAGE_LENGTH], int col_counts[])
{
    if (key_length <= 0) return; // Evita divisão por zero ou comportamento indefinido

    int len = strlen(input);
    int rows = len / key_length;
    int extra = len % key_length;

    // 1) monta o vetor “order” com os índices 0,1,2,...,key_length-1
    int order[key_length];
    for (int i = 0; i < key_length; i++)
    {
        order[i] = i;
    }

    // 2) ordena esse vetor “order” segundo key[order[i]], para saber a ordem alfabética da chave
    //    e como as colunas do texto cifrado foram lidas.
    for (int i = 0; i < key_length - 1; i++)
    {
        for (int j = 0; j < key_length - i - 1; j++)
        {
            // Compara os caracteres da chave original nos índices apontados por order[j] e order[j+1]
            if (key[order[j]] > key[order[j + 1]])
            {
                int tmp = order[j];
                order[j] = order[j + 1];
                order[j + 1] = tmp;
            }
        }
    }
    // Agora, order[i] contém o índice original da coluna que é a i-ésima na ordem alfabética da chave.

    // 3) determina quantos símbolos cada coluna (no índice original) vai ter
    //    A coluna correspondente a order[i] (que é a i-ésima coluna lida do texto cifrado)
    //    terá 'rows + 1' símbolos se order[i] (o índice original da coluna) for menor que 'extra'.
    //    Esta lógica parece ser a forma como as colunas são preenchidas no texto cifrado.
    //    Isto é, as primeiras 'extra' colunas (na ordem da chave original) recebem um símbolo a mais.
    //    Então, precisamos mapear de volta: a i-ésima coluna que lemos do texto cifrado (que corresponde a key[order[i]])
    //    terá um certo comprimento.
    //    O código original calcula col_counts[orig_index], onde orig_index é o índice original.
    //    Isto está correto: as colunas são preenchidas no texto cifrado de acordo com a ordem alfabética da chave.
    //    As primeiras 'extra' colunas *na ordem alfabética da chave* recebem um símbolo a mais.

    // Ajuste: col_counts[i] deve ser o tamanho da i-ésima coluna na ordem alfabética.
    for (int i = 0; i < key_length; i++)
    {
         col_counts[i] = rows + (i < extra ? 1 : 0);
    }


    // 4) preenche cada “columns[order[i]]” lendo do texto cifrado linearizado
    //    A i-ésima coluna (na ordem alfabética) corresponde a columns[order[i]] na matriz de saída
    //    (que queremos que esteja na ordem original da chave).
    //    O texto cifrado é lido sequencialmente. A primeira porção preenche a primeira coluna
    //    alfabética (key[order[0]]), a segunda porção preenche a segunda coluna alfabética (key[order[1]]), etc.
    int pos = 0;
    for (int i = 0; i < key_length; i++) // Itera sobre as colunas na ordem alfabética da chave
    {
        int original_col_index = order[i]; // Coluna original que corresponde à i-ésima coluna alfabética
        int current_col_len = col_counts[i]; // Comprimento da i-ésima coluna alfabética

        for (int j = 0; j < current_col_len; j++)
        {
            if (pos < len) { // Segurança
                columns[original_col_index][j] = input[pos++];
            } else {
                break; // Evita ler além do input
            }
        }
    }
}


/**
 * @brief Reverte a organização em colunas, reconstruindo a sequência de símbolos linha a linha.
 * (Função auxiliar estática)
 */
static void reverse_polybius(char columns[][MAX_MESSAGE_LENGTH], int col_counts[], int key_length, char *output)
{
    if (key_length <= 0) {
        if (output) output[0] = '\0';
        return;
    }
    int max_rows = 0, pos = 0;

    // Encontra o número máximo de linhas em qualquer coluna (para saber até onde iterar)
    for (int i = 0; i < key_length; i++)
    {
        if (col_counts[i] > max_rows)
            max_rows = col_counts[i];
    }

    // Lê linha por linha, através das colunas (na ordem original da chave)
    for (int r = 0; r < max_rows; r++)
    {
        for (int c = 0; c < key_length; c++) // Itera sobre as colunas na ordem original da chave
        {
            if (r < col_counts[c]) // Se a coluna 'c' tiver um símbolo na linha 'r'
            {
                if (pos < MAX_MESSAGE_LENGTH * 2) { // Segurança para o buffer de saída
                    output[pos++] = columns[c][r];
                } else {
                    // Buffer de saída 'rearranged' está cheio
                    if (output) output[pos] = '\0'; // Tenta terminar a string
                    return;
                }
            }
        }
    }
    if (output) output[pos] = '\0'; // Termina a string de saída
}

/**
 * @brief Decodifica pares de símbolos ADFGVX em caracteres da matriz Polybius.
 * (Função auxiliar estática)
 */
static void decode_symbols(char *pairs, char *message)
{
    if (!pairs || !message) return;

    int len = strlen(pairs);
    int msg_index = 0;

    if (len % 2 != 0) { // Número ímpar de símbolos não pode ser decodificado
        message[0] = '\0';
        // Poderia retornar um código de erro ou logar
        return;
    }

    for (int i = 0; i < len; i += 2)
    {
        int row = symbol_index(pairs[i]);
        int col = symbol_index(pairs[i + 1]);
        if (row >= 0 && col >= 0) // Se ambos os símbolos do par são válidos
        {
            if (msg_index < MAX_MESSAGE_LENGTH -1) { // Segurança para o buffer de mensagem
                message[msg_index++] = square[row][col];
            } else {
                // Buffer de mensagem cheio
                break;
            }
        } else {
            // Par de símbolos inválido, pode indicar corrupção ou erro
            // O que fazer? Continuar, parar, inserir um placeholder?
            // Por enquanto, vamos parar para evitar dados incorretos.
            // Ou poderia inserir um '?' e continuar, dependendo do requisito.
            // message[msg_index++] = '?'; // Exemplo de placeholder
            break; // Parar se um par inválido for encontrado
        }
    }
    message[msg_index] = '\0'; // Termina a string da mensagem decifrada
}

// Implementação da função pública
void decipher_adfgvx(char *encrypted_text, char *key, int key_length, char *output)
{
    if (!encrypted_text || !key || !output || key_length <= 0 || key_length >= MAX_KEY_LENGTH) {
        if (output) output[0] = '\0'; // Garante terminação nula em caso de erro de parâmetro
        return; // Tratamento básico de erro de parâmetro
    }

    // VLAs - certifique-se de que key_length é válido
    char columns[key_length][MAX_MESSAGE_LENGTH];
    int col_counts[key_length]; // Não precisa inicializar com {0} aqui, reverse_transposition fará
                                // ou melhor, reverse_transposition deve receber col_counts já zerado ou zerá-lo.
                                // A lógica de reverse_transposition calcula col_counts.

    // Buffer para a sequência de símbolos após reverter a transposição.
    // O tamanho máximo é o mesmo do texto cifrado. +1 para o nulo.
    int encrypted_len = strlen(encrypted_text);
    if (encrypted_len == 0) {
        output[0] = '\0';
        return;
    }
    // O buffer 'rearranged' precisa ser grande o suficiente para encrypted_text + nulo.
    // MAX_MESSAGE_LENGTH * 2 é o tamanho máximo de símbolos ADFGVX.
    char rearranged[MAX_MESSAGE_LENGTH * 2 + 1];

    // Inicializa col_counts, embora reverse_transposition vá preenchê-lo.
    // É uma boa prática zerar arrays antes de passar para funções que os modificam,
    // a menos que a função garanta a inicialização.
    memset(col_counts, 0, key_length * sizeof(int));
    // Zera a matriz de colunas também pode ser uma boa prática
    for(int i=0; i<key_length; ++i) {
        memset(columns[i], 0, MAX_MESSAGE_LENGTH * sizeof(char));
    }


    reverse_transposition(encrypted_text, key, key_length, columns, col_counts);
    reverse_polybius(columns, col_counts, key_length, rearranged);
    decode_symbols(rearranged, output);
}
