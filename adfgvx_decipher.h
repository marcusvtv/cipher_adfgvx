#ifndef ADFGVX_DECIPHER_H
#define ADFGVX_DECIPHER_H

#include "cipher_config.h" // Para MAX_MESSAGE_LENGTH, MAX_KEY_LENGTH

/**
 * @brief Fun��o principal para decodificar a cifra ADFGVX.
 *
 * Executa a sequ�ncia de etapas para decifrar o texto cifrado:
 * 1. Reverte a transposi��o com base na chave.
 * 2. Reagrupa os s�mbolos em sua ordem original.
 * 3. Decodifica os pares ADFGVX na matriz Polybius.
 *
 * @param encrypted_text Texto cifrado (string terminada em nulo).
 * @param key Chave de cifra (string terminada em nulo).
 * @param key_length Comprimento da chave.
 * @param output Buffer onde a mensagem decodificada ser� armazenada (deve ter tamanho MAX_MESSAGE_LENGTH).
 * A fun��o garante a termina��o nula do buffer de sa�da.
 */
void decipher_adfgvx(char *encrypted_text, char *key, int key_length, char *output);

#endif // ADFGVX_DECIPHER_H
