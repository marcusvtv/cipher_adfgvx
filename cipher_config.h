#ifndef CIPHER_CONFIG_H
#define CIPHER_CONFIG_H

// Define o comprimento m�ximo da mensagem a ser lida.
#define MAX_MESSAGE_LENGTH 2560

// Define o comprimento m�ximo da chave (8 caracteres + 1 para o terminador nulo '\0').
#define MAX_KEY_LENGTH 9

// Nomes de arquivo padr�o.
// Se seus arquivos estiverem em um subdiret�rio como "./src/", ajuste os caminhos.
// Exemplo: #define DEFAULT_KEY_FILE "./src/key.txt"
#define DEFAULT_KEY_FILE "./key.txt"
#define DEFAULT_MESSAGE_FILE "./message.txt"
#define DEFAULT_ENCRYPTED_FILE "./encrypted.txt"

#endif // CIPHER_CONFIG_H
