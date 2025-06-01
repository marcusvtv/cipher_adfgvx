#ifndef CIPHER_CONFIG_H
#define CIPHER_CONFIG_H

// Define o comprimento máximo da mensagem a ser lida.
#define MAX_MESSAGE_LENGTH 2560

// Define o comprimento máximo da chave (8 caracteres + 1 para o terminador nulo '\0').
#define MAX_KEY_LENGTH 9

// Nomes de arquivo padrão.
// Se seus arquivos estiverem em um subdiretório como "./src/", ajuste os caminhos.
// Exemplo: #define DEFAULT_KEY_FILE "./src/key.txt"
#define DEFAULT_KEY_FILE "./key.txt"
#define DEFAULT_MESSAGE_FILE "./message.txt"
#define DEFAULT_ENCRYPTED_FILE "./encrypted.txt"

#endif // CIPHER_CONFIG_H
