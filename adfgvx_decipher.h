#ifndef ADFGVX_DECIPHER_H
#define ADFGVX_DECIPHER_H

// cipher_config.h é incluído por adfgvx_decipher.c ou main,
// mas não é estritamente necessário aqui se MAX_MESSAGE_LENGTH não estiver no protótipo.
// No entanto, a função decipher_adfgvx implicitamente depende de MAX_MESSAGE_LENGTH
// para o tamanho do buffer de saída esperado.

/**
 * @brief Função principal para decodificar a cifra ADFGVX.
 *
 * Executa a sequência de etapas para decifrar o texto cifrado.
 * A lógica desta função e suas auxiliares é baseada no código fornecido pelo utilizador.
 *
 * @param encrypted_text Texto cifrado (string terminada em nulo).
 * @param key Chave de cifra (string terminada em nulo).
 * @param key_length Comprimento da chave.
 * @param output Buffer onde a mensagem decodificada será armazenada (deve ser grande o suficiente,
 * tipicamente MAX_MESSAGE_LENGTH conforme definido em cipher_config.h).
 * A função garante a terminação nula do buffer de saída.
 */
void decipher_adfgvx(char *encrypted_text, char *key, int key_length, char *output);

#endif // ADFGVX_DECIPHER_H
