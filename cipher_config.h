#ifndef CIPHER_CONFIG_H
#define CIPHER_CONFIG_H

// Define o comprimento máximo da mensagem a ser lida.
#define MAX_MESSAGE_LENGTH 2560

// Define o comprimento máximo da chave (8 caracteres + 1 para o terminador nulo '\0').
#define MAX_KEY_LENGTH 9

// Nomes de arquivo padrão.
#define DEFAULT_KEY_FILE "./key.txt"
#define DEFAULT_MESSAGE_FILE "./message.txt"
#define DEFAULT_ENCRYPTED_FILE "./encrypted.txt"
#define DEFAULT_DECRYPTED_FILE_FOR_TEST "./decrypted_test_output.txt" // Para o teste

#endif // CIPHER_CONFIG_H
