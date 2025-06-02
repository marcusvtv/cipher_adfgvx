#ifndef ADFGVX_DECIPHER_H
#define ADFGVX_DECIPHER_H

#include "cipher_config.h" // Para MAX_MESSAGE_LENGTH, MAX_KEY_LENGTH

/**
 * @brief Função principal para decodificar a cifra ADFGVX.
 *
 * Executa a sequência de etapas para decifrar o texto cifrado:
 * 1. Reverte a transposição com base na chave.
 * 2. Reagrupa os símbolos em sua ordem original.
 * 3. Decodifica os pares ADFGVX na matriz Polybius.
 *
 * @param encrypted_text Texto cifrado (string terminada em nulo).
 * @param key Chave de cifra (string terminada em nulo).
 * @param key_length Comprimento da chave.
 * @param output Buffer onde a mensagem decodificada será armazenada (deve ter tamanho MAX_MESSAGE_LENGTH).
 * A função garante a terminação nula do buffer de saída.
 */
void decipher_adfgvx(char *encrypted_text, char *key, int key_length, char *output);

#endif // ADFGVX_DECIPHER_H
