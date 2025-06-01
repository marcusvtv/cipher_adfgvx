#include <stdio.h>
#include <string.h> // Para strlen
#include <stdlib.h> // Para EXIT_FAILURE, EXIT_SUCCESS (ou pode usar 0 e 1 diretamente)

// Inclui os novos arquivos de cabeçalho dos módulos
#include "cipher_config.h"
#include "file_operations.h"
#include "adfgvx_core.h"

/**
 * @brief Funcao principal do programa de cifragem ADFGVX.
 * (Mantendo a documentacao original da funcao main)
 */
int main()
{
    // Variaveis para armazenar a chave e a mensagem lidas dos arquivos.
    char cipher_key_buffer[MAX_KEY_LENGTH]; // Renomeado de cipher_key
    char message_buffer[MAX_MESSAGE_LENGTH]; // Renomeado de message

    // Contador de simbolos ADFGVX por coluna.
    // Inicializado com zeros. O tamanho MAX_KEY_LENGTH é usado aqui pois
    // o comprimento real da chave (actual_key_length) ainda nao é conhecido.
    // Apenas as posicoes ate actual_key_length-1 serao usadas.
    int symbols_per_column_count[MAX_KEY_LENGTH] = {0}; // Renomeado de symbols_per_column

    int actual_key_length = 0; // Renomeado de KEY_LENGTH para clareza e evitar conflito com macros
    int file_read_status;      // Renomeado de is_file_read

    // Le a chave de cifra do arquivo
    printf("Lendo chave de '%s'...\n", DEFAULT_KEY_FILE);
    file_read_status = read_file(DEFAULT_KEY_FILE, cipher_key_buffer, MAX_KEY_LENGTH);
    if (file_read_status != 0)
    {
        fprintf(stderr, "Erro lendo arquivo da chave '%s'. Codigo: %d\n", DEFAULT_KEY_FILE, file_read_status);
        return EXIT_FAILURE; // Ou return 1;
    }

    // Define o tamanho real da chave com base no conteudo lido.
    // A funcao read_file agora remove o '\n', entao strlen deve dar o comprimento correto.
    actual_key_length = strlen(cipher_key_buffer);

    // Validacao basica do comprimento da chave
    if (actual_key_length == 0 || actual_key_length >= MAX_KEY_LENGTH) {
        fprintf(stderr, "Erro: Comprimento da chave invalido (%d). Deve ser entre 1 e %d caracteres.\n",
                actual_key_length, MAX_KEY_LENGTH - 1);
        return EXIT_FAILURE;
    }
    printf("Chave lida: \"%s\" (Comprimento: %d)\n", cipher_key_buffer, actual_key_length);


    // Matriz para armazenar os simbolos ADFGVX organizados por coluna.
    // Usa VLA (Variable Length Array), uma funcionalidade do C99.
    // A primeira dimensao é o comprimento real da chave.
    // A segunda dimensao é MAX_MESSAGE_LENGTH, conforme esperado pelas funcoes do modulo de cifra.
    char intermediate_encoded_matrix[actual_key_length][MAX_MESSAGE_LENGTH]; // Renomeado de encoded_symbol_matrix

    // Ler a mensagem do arquivo
    printf("Lendo mensagem de '%s'...\n", DEFAULT_MESSAGE_FILE);
    file_read_status = read_file(DEFAULT_MESSAGE_FILE, message_buffer, MAX_MESSAGE_LENGTH);
    if (file_read_status != 0)
    {
        fprintf(stderr, "Erro lendo arquivo da mensagem '%s'. Codigo: %d\n", DEFAULT_MESSAGE_FILE, file_read_status);
        return EXIT_FAILURE;
    }
    // Para mensagens longas, imprimir apenas uma parte pode ser util
    printf("Mensagem lida (primeiros 50 chars, se houver): \"%.50s%s\"\n",
           message_buffer, strlen(message_buffer) > 50 ? "..." : "");


    // Realizar a cifra ADFGVX
    // symbols_per_column_count ja esta inicializado com zeros.
    printf("Cifrando a mensagem...\n");
    cipher_adfgvx(cipher_key_buffer, actual_key_length, message_buffer, intermediate_encoded_matrix, symbols_per_column_count);

    // Salvar a mensagem cifrada em 'encrypted.txt' usando a nova funcao dedicada
    printf("Salvando mensagem cifrada em '%s'...\n", DEFAULT_ENCRYPTED_FILE);
    if (write_encrypted_data_to_file(DEFAULT_ENCRYPTED_FILE, actual_key_length, intermediate_encoded_matrix, symbols_per_column_count) != 0)
    {
        // A funcao write_encrypted_data_to_file ja imprime um erro com perror.
        fprintf(stderr, "Falha ao salvar a mensagem cifrada.\n");
        return EXIT_FAILURE;
    }

    printf("Processo de cifragem concluido com sucesso!\n");
    return EXIT_SUCCESS; // Ou return 0;
}
