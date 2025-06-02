#include "adfgvx_decipher.h"
#include <stdio.h>
#include <string.h>

// Constantes symbols e square s�o necess�rias para a decifragem
// e s�o encapsuladas neste m�dulo como static.
static const char symbols[6] = {'A', 'D', 'F', 'G', 'V', 'X'};
static const char square[6][6] = {
    {'A', 'B', 'C', 'D', 'E', 'F'},
    {'G', 'H', 'I', 'J', 'K', 'L'},
    {'M', 'N', 'O', 'P', 'Q', 'R'},
    {'S', 'T', 'U', 'V', 'W', 'X'},
    {'Y', 'Z', ' ', ',', '.', '1'},
    {'2', '3', '4', '5', '6', '7'}};

/**
 * @brief Retorna o �ndice de um s�mbolo ADFGVX dentro do vetor `symbols`.
 * (Fun��o auxiliar est�tica)
 */
static int symbol_index(char c)
{
    for (int i = 0; i < 6; i++)
    {
        if (symbols[i] == c)
            return i;
    }
    return -1; // S�mbolo n�o encontrado
}

/**
 * @brief Reconstr�i as colunas originais da cifra com base na chave de transposi��o.
 * (Fun��o auxiliar est�tica)
 */
static void reverse_transposition(char *input, char *key, int key_length, char columns[][MAX_MESSAGE_LENGTH], int col_counts[])
{
    if (key_length <= 0) return; // Evita divis�o por zero ou comportamento indefinido

    int len = strlen(input);
    int rows = len / key_length;
    int extra = len % key_length;

    // 1) monta o vetor �order� com os �ndices 0,1,2,...,key_length-1
    int order[key_length];
    for (int i = 0; i < key_length; i++)
    {
        order[i] = i;
    }

    // 2) ordena esse vetor �order� segundo key[order[i]], para saber a ordem alfab�tica da chave
    //    e como as colunas do texto cifrado foram lidas.
    for (int i = 0; i < key_length - 1; i++)
    {
        for (int j = 0; j < key_length - i - 1; j++)
        {
            // Compara os caracteres da chave original nos �ndices apontados por order[j] e order[j+1]
            if (key[order[j]] > key[order[j + 1]])
            {
                int tmp = order[j];
                order[j] = order[j + 1];
                order[j + 1] = tmp;
            }
        }
    }
    // Agora, order[i] cont�m o �ndice original da coluna que � a i-�sima na ordem alfab�tica da chave.

    // 3) determina quantos s�mbolos cada coluna (no �ndice original) vai ter
    //    A coluna correspondente a order[i] (que � a i-�sima coluna lida do texto cifrado)
    //    ter� 'rows + 1' s�mbolos se order[i] (o �ndice original da coluna) for menor que 'extra'.
    //    Esta l�gica parece ser a forma como as colunas s�o preenchidas no texto cifrado.
    //    Isto �, as primeiras 'extra' colunas (na ordem da chave original) recebem um s�mbolo a mais.
    //    Ent�o, precisamos mapear de volta: a i-�sima coluna que lemos do texto cifrado (que corresponde a key[order[i]])
    //    ter� um certo comprimento.
    //    O c�digo original calcula col_counts[orig_index], onde orig_index � o �ndice original.
    //    Isto est� correto: as colunas s�o preenchidas no texto cifrado de acordo com a ordem alfab�tica da chave.
    //    As primeiras 'extra' colunas *na ordem alfab�tica da chave* recebem um s�mbolo a mais.

    // Ajuste: col_counts[i] deve ser o tamanho da i-�sima coluna na ordem alfab�tica.
    for (int i = 0; i < key_length; i++)
    {
         col_counts[i] = rows + (i < extra ? 1 : 0);
    }


    // 4) preenche cada �columns[order[i]]� lendo do texto cifrado linearizado
    //    A i-�sima coluna (na ordem alfab�tica) corresponde a columns[order[i]] na matriz de sa�da
    //    (que queremos que esteja na ordem original da chave).
    //    O texto cifrado � lido sequencialmente. A primeira por��o preenche a primeira coluna
    //    alfab�tica (key[order[0]]), a segunda por��o preenche a segunda coluna alfab�tica (key[order[1]]), etc.
    int pos = 0;
    for (int i = 0; i < key_length; i++) // Itera sobre as colunas na ordem alfab�tica da chave
    {
        int original_col_index = order[i]; // Coluna original que corresponde � i-�sima coluna alfab�tica
        int current_col_len = col_counts[i]; // Comprimento da i-�sima coluna alfab�tica

        for (int j = 0; j < current_col_len; j++)
        {
            if (pos < len) { // Seguran�a
                columns[original_col_index][j] = input[pos++];
            } else {
                break; // Evita ler al�m do input
            }
        }
    }
}


/**
 * @brief Reverte a organiza��o em colunas, reconstruindo a sequ�ncia de s�mbolos linha a linha.
 * (Fun��o auxiliar est�tica)
 */
static void reverse_polybius(char columns[][MAX_MESSAGE_LENGTH], int col_counts[], int key_length, char *output)
{
    if (key_length <= 0) {
        if (output) output[0] = '\0';
        return;
    }
    int max_rows = 0, pos = 0;

    // Encontra o n�mero m�ximo de linhas em qualquer coluna (para saber at� onde iterar)
    for (int i = 0; i < key_length; i++)
    {
        if (col_counts[i] > max_rows)
            max_rows = col_counts[i];
    }

    // L� linha por linha, atrav�s das colunas (na ordem original da chave)
    for (int r = 0; r < max_rows; r++)
    {
        for (int c = 0; c < key_length; c++) // Itera sobre as colunas na ordem original da chave
        {
            if (r < col_counts[c]) // Se a coluna 'c' tiver um s�mbolo na linha 'r'
            {
                if (pos < MAX_MESSAGE_LENGTH * 2) { // Seguran�a para o buffer de sa�da
                    output[pos++] = columns[c][r];
                } else {
                    // Buffer de sa�da 'rearranged' est� cheio
                    if (output) output[pos] = '\0'; // Tenta terminar a string
                    return;
                }
            }
        }
    }
    if (output) output[pos] = '\0'; // Termina a string de sa�da
}

/**
 * @brief Decodifica pares de s�mbolos ADFGVX em caracteres da matriz Polybius.
 * (Fun��o auxiliar est�tica)
 */
static void decode_symbols(char *pairs, char *message)
{
    if (!pairs || !message) return;

    int len = strlen(pairs);
    int msg_index = 0;

    if (len % 2 != 0) { // N�mero �mpar de s�mbolos n�o pode ser decodificado
        message[0] = '\0';
        // Poderia retornar um c�digo de erro ou logar
        return;
    }

    for (int i = 0; i < len; i += 2)
    {
        int row = symbol_index(pairs[i]);
        int col = symbol_index(pairs[i + 1]);
        if (row >= 0 && col >= 0) // Se ambos os s�mbolos do par s�o v�lidos
        {
            if (msg_index < MAX_MESSAGE_LENGTH -1) { // Seguran�a para o buffer de mensagem
                message[msg_index++] = square[row][col];
            } else {
                // Buffer de mensagem cheio
                break;
            }
        } else {
            // Par de s�mbolos inv�lido, pode indicar corrup��o ou erro
            // O que fazer? Continuar, parar, inserir um placeholder?
            // Por enquanto, vamos parar para evitar dados incorretos.
            // Ou poderia inserir um '?' e continuar, dependendo do requisito.
            // message[msg_index++] = '?'; // Exemplo de placeholder
            break; // Parar se um par inv�lido for encontrado
        }
    }
    message[msg_index] = '\0'; // Termina a string da mensagem decifrada
}

// Implementa��o da fun��o p�blica
void decipher_adfgvx(char *encrypted_text, char *key, int key_length, char *output)
{
    if (!encrypted_text || !key || !output || key_length <= 0 || key_length >= MAX_KEY_LENGTH) {
        if (output) output[0] = '\0'; // Garante termina��o nula em caso de erro de par�metro
        return; // Tratamento b�sico de erro de par�metro
    }

    // VLAs - certifique-se de que key_length � v�lido
    char columns[key_length][MAX_MESSAGE_LENGTH];
    int col_counts[key_length]; // N�o precisa inicializar com {0} aqui, reverse_transposition far�
                                // ou melhor, reverse_transposition deve receber col_counts j� zerado ou zer�-lo.
                                // A l�gica de reverse_transposition calcula col_counts.

    // Buffer para a sequ�ncia de s�mbolos ap�s reverter a transposi��o.
    // O tamanho m�ximo � o mesmo do texto cifrado. +1 para o nulo.
    int encrypted_len = strlen(encrypted_text);
    if (encrypted_len == 0) {
        output[0] = '\0';
        return;
    }
    // O buffer 'rearranged' precisa ser grande o suficiente para encrypted_text + nulo.
    // MAX_MESSAGE_LENGTH * 2 � o tamanho m�ximo de s�mbolos ADFGVX.
    char rearranged[MAX_MESSAGE_LENGTH * 2 + 1];

    // Inicializa col_counts, embora reverse_transposition v� preench�-lo.
    // � uma boa pr�tica zerar arrays antes de passar para fun��es que os modificam,
    // a menos que a fun��o garanta a inicializa��o.
    memset(col_counts, 0, key_length * sizeof(int));
    // Zera a matriz de colunas tamb�m pode ser uma boa pr�tica
    for(int i=0; i<key_length; ++i) {
        memset(columns[i], 0, MAX_MESSAGE_LENGTH * sizeof(char));
    }


    reverse_transposition(encrypted_text, key, key_length, columns, col_counts);
    reverse_polybius(columns, col_counts, key_length, rearranged);
    decode_symbols(rearranged, output);
}
