#ifndef ADFGVX_CORE_H
#define ADFGVX_CORE_H

#include "cipher_config.h" // Para MAX_MESSAGE_LENGTH

/**
 * @brief Aplica a cifra ADFGVX: codifica os simbolos e faz a transposicao das colunas.
 * Esta é a função pública principal do módulo de cifra.
 *
 * @param key A chave usada na transposicao (array de caracteres, string terminada em nulo).
 * @param key_length Comprimento real da chave (numero de caracteres na chave).
 * @param message Mensagem de entrada (string terminada em nulo).
 * @param encoded_symbol_matrix Matriz onde os simbolos codificados serao armazenados.
 * A primeira dimensao DEVE corresponder a key_length.
 * A segunda dimensao é MAX_MESSAGE_LENGTH.
 * @param symbols_per_column Vetor (com tamanho baseado em MAX_KEY_LENGTH ou key_length)
 * para armazenar a contagem de elementos em cada coluna.
 * Deve ser inicializado com zeros pelo chamador.
 */
void cipher_adfgvx(char key[],
                   int key_length,
                   char message[],
                   char encoded_symbol_matrix[][MAX_MESSAGE_LENGTH],
                   int symbols_per_column[]);

#endif // ADFGVX_CORE_H
