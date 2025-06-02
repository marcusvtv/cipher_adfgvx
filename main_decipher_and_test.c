#include <stdio.h>
#include <string.h>
#include <stdlib.h> // Para EXIT_SUCCESS, EXIT_FAILURE
#include <time.h>   // Para test_execution_time

#include "cipher_config.h"
#include "file_operations.h"
#include "adfgvx_core.h"     // Para cipher_adfgvx (usado em testes)
#include "adfgvx_decipher.h" // Para decipher_adfgvx

// --- Funções de Teste (adaptadas do código monolítico) ---

/**
 * @brief Testa a função de decifragem comparando com a mensagem original.
 */
static void test_decipher_internal(const char *test_name, char key[], char original_message[])
{
    printf("\n-> Teste de Decifragem: %s\n", test_name);
    int key_length = strlen(key);
    if (key_length == 0 || key_length >= MAX_KEY_LENGTH) {
        printf("\tErro: Chave de teste inválida '%s'.\n", key);
        return;
    }

    // VLA para a matriz de cifragem
    char encoded_symbol_matrix[key_length][MAX_MESSAGE_LENGTH];
    int symbols_per_column[MAX_KEY_LENGTH] = {0}; // Usa MAX_KEY_LENGTH para segurança

    // Cifrar a mensagem usando o módulo adfgvx_core
    cipher_adfgvx(key, key_length, original_message, encoded_symbol_matrix, symbols_per_column);

    // Linearizar mensagem cifrada para alimentar a decifragem
    char encrypted_linear[MAX_MESSAGE_LENGTH * 2 + 1]; // +1 para o nulo
    int pos = 0;
    for (int i = 0; i < key_length; i++)
    {
        for (int j = 0; j < symbols_per_column[i]; j++)
        {
            if (pos < MAX_MESSAGE_LENGTH * 2) {
                encrypted_linear[pos++] = encoded_symbol_matrix[i][j];
            } else {
                printf("\tErro: Buffer de encrypted_linear cheio durante a linearização.\n");
                encrypted_linear[pos] = '\0'; // Tenta terminar
                return;
            }
        }
    }
    encrypted_linear[pos] = '\0';

    // Decifrar usando o módulo adfgvx_decipher
    char decrypted_output[MAX_MESSAGE_LENGTH];
    decipher_adfgvx(encrypted_linear, key, key_length, decrypted_output);

    printf("\t\tMensagem Original:  \"%.50s%s\"\n", original_message, strlen(original_message) > 50 ? "..." : "");
    printf("\t\tChave:              \"%s\"\n", key);
    printf("\t\tTexto Cifrado:    \"%.50s%s\"\n", encrypted_linear, strlen(encrypted_linear) > 50 ? "..." : "");
    printf("\t\tMensagem Decifrada: \"%.50s%s\"\n", decrypted_output, strlen(decrypted_output) > 50 ? "..." : "");

    if (strcmp(original_message, decrypted_output) == 0)
    {
        printf("\tSUCESSO: Mensagem decifrada corresponde à original!\n");
    }
    else
    {
        printf("\tERRO: A decifragem falhou. Mensagens não correspondem.\n");
    }
}

/**
 * @brief Mede o tempo de execução máximo da cifragem de uma mensagem longa.
 */
static void test_execution_time_internal()
{
    printf("\n-> Teste: Tempo de Execução da Cifragem\n");
    char key[] = "CHAVE123"; // Chave de 8 caracteres (MAX_KEY_LENGTH - 1)
    int key_length = strlen(key);

    char long_message[MAX_MESSAGE_LENGTH];
    memset(long_message, 'A', MAX_MESSAGE_LENGTH - 1); // Preenche com 'A'
    long_message[MAX_MESSAGE_LENGTH - 1] = '\0';

    // Usa MAX_KEY_LENGTH para a primeira dimensão para segurança, embora key_length seja 8.
    char encoded_symbol_matrix[MAX_KEY_LENGTH -1][MAX_MESSAGE_LENGTH];
    int symbols_per_column[MAX_KEY_LENGTH] = {0};

    clock_t start_time = clock();
    cipher_adfgvx(key, key_length, long_message, encoded_symbol_matrix, symbols_per_column);
    clock_t end_time = clock();

    double elapsed_seconds = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    printf("\t\tMensagem: %d caracteres 'A'\n", (int)strlen(long_message));
    printf("\t\tChave: \"%s\"\n", key);
    printf("\t\tTempo de cifragem: %.6f segundos\n", elapsed_seconds);

    if (elapsed_seconds > 0.5) // Limite de 0.5 segundos
    {
        printf("\tAVISO: Tempo de execução excedeu 0.5 segundos!\n");
    }
    else
    {
        printf("\tSUCESSO: Tempo de execução dentro do limite de 0.5 segundos.\n");
    }
}

/**
 * @brief Verifica se caracteres inválidos são ignorados durante a cifragem.
 */
static void test_invalid_character_handling_internal()
{
    printf("\n-> Teste: Tratamento de Caracteres Inválidos na Cifragem\n");
    char key[] = "UM";
    int key_length = strlen(key);
    char message_with_invalids[] = "L#UC%AS@!d"; // Esperado que apenas "LUCASd" seja cifrado

    // VLA para a matriz de cifragem
    char encoded_symbol_matrix[key_length][MAX_MESSAGE_LENGTH];
    int symbols_per_column[MAX_KEY_LENGTH] = {0};

    // Cifragem esperada para "LUCAS" com chave "UM":
    // L: VG (V na col 0, G na col 1)
    // U: DX (D na col 0, X na col 1)
    // C: AF (A na col 0, F na col 1)
    // A: AA (A na col 0, A na col 1)
    // S: DF (D na col 0, F na col 1)
    // Colunas antes da transposição:
    // Col 0 (U): V D A A D
    // Col 1 (M): G X F A F
    // Chave "UM" -> M U (ordem alfabética)
    // Colunas após transposição (ordem M, U):
    // Col M (originalmente col 1): G X F A F
    // Col U (originalmente col 0): V D A A D
    // Texto cifrado linear: GXFAFVDAAD
    // Se a mensagem for "LUCASd", 'd' é 'AX'.
    // L: VG
    // U: DX
    // C: AF
    // A: AA
    // S: DF
    // d: AX
    // Intermediário: V G D X A F A A D F A X
    // Col 0 (U): V D A A D A
    // Col 1 (M): G X F A F X
    // Transposto (M, U): G X F A F X V D A A D A
    const char expected_cipher_for_LUCASd[] = "GXFAFXVDAADA";


    cipher_adfgvx(key, key_length, message_with_invalids, encoded_symbol_matrix, symbols_per_column);

    char actual_cipher[MAX_MESSAGE_LENGTH * 2 + 1] = {0};
    int pos = 0;
    for (int i = 0; i < key_length; i++)
    {
        for (int j = 0; j < symbols_per_column[i]; j++)
        {
             if (pos < MAX_MESSAGE_LENGTH * 2) {
                actual_cipher[pos++] = encoded_symbol_matrix[i][j];
            }
        }
    }
    actual_cipher[pos] = '\0';

    printf("\t\tMensagem Original com Inválidos: \"%s\"\n", message_with_invalids);
    printf("\t\tChave: \"%s\"\n", key);
    printf("\t\tTexto Cifrado Obtido:          \"%s\"\n", actual_cipher);
    printf("\t\tTexto Cifrado Esperado (para LUCASd): \"%s\"\n", expected_cipher_for_LUCASd);

    if (strcmp(actual_cipher, expected_cipher_for_LUCASd) == 0)
    {
        printf("\tSUCESSO: Caracteres inválidos ignorados e cifragem correta.\n");
    }
    else
    {
        printf("\tERRO: A mensagem cifrada está incorreta ou os caracteres inválidos não foram tratados como esperado.\n");
    }
}


int main()
{
    char key_buffer[MAX_KEY_LENGTH];
    char original_message_for_test[MAX_MESSAGE_LENGTH];
    char encrypted_input_buffer[MAX_MESSAGE_LENGTH * 2 + 1];
    char decrypted_output_buffer[MAX_MESSAGE_LENGTH];
    int key_len_actual = 0;
    int status;

    printf("--- PROGRAMA DE TESTE DE DECIFRAGEM E OUTROS TESTES ADFGVX ---\n");

    // 1. Ler chave
    printf("\nEtapa 1: Lendo chave de '%s'...\n", DEFAULT_KEY_FILE);
    status = read_file(DEFAULT_KEY_FILE, key_buffer, MAX_KEY_LENGTH);
    if (status != 0) {
        fprintf(stderr, "Erro ao ler o arquivo da chave '%s'. Código: %d\n", DEFAULT_KEY_FILE, status);
        return EXIT_FAILURE;
    }
    key_len_actual = strlen(key_buffer);
    if (key_len_actual == 0 || key_len_actual >= MAX_KEY_LENGTH) {
        fprintf(stderr, "Erro: Comprimento da chave inválido (%d).\n", key_len_actual);
        return EXIT_FAILURE;
    }
    printf("Chave: \"%s\", Comprimento: %d\n", key_buffer, key_len_actual);

    // 2. Ler texto cifrado (assumindo que foi gerado por um programa de cifragem separado)
    printf("\nEtapa 2: Lendo texto cifrado de '%s'...\n", DEFAULT_ENCRYPTED_FILE);
    status = read_file(DEFAULT_ENCRYPTED_FILE, encrypted_input_buffer, sizeof(encrypted_input_buffer));
    if (status != 0) {
        fprintf(stderr, "Erro ao ler o arquivo cifrado '%s'. Código: %d\n", DEFAULT_ENCRYPTED_FILE, status);
        fprintf(stderr, "Certifique-se de que este arquivo existe e foi gerado previamente.\n");
        // Não sair, pois os testes auto-contidos abaixo ainda podem rodar.
    } else {
        printf("Texto Cifrado Lido (para decifragem principal): \"%.50s%s\"\n",
               encrypted_input_buffer, strlen(encrypted_input_buffer) > 50 ? "..." : "");

        // 3. Decifrar
        printf("\nEtapa 3: Decifrando o texto lido...\n");
        decipher_adfgvx(encrypted_input_buffer, key_buffer, key_len_actual, decrypted_output_buffer);
        printf("Texto Decifrado: \"%.50s%s\"\n",
               decrypted_output_buffer, strlen(decrypted_output_buffer) > 50 ? "..." : "");

        // 4. Salvar texto decifrado
        printf("\nEtapa 4: Salvando texto decifrado em '%s'...\n", DEFAULT_DECRYPTED_FILE_FOR_TEST);
        if (write_plaintext_to_file(DEFAULT_DECRYPTED_FILE_FOR_TEST, decrypted_output_buffer) != 0) {
            fprintf(stderr, "Falha ao salvar o texto decifrado.\n");
        } else {
            printf("Texto decifrado salvo com sucesso.\n");
        }

        // 5. Comparar com original (se possível)
        printf("\nEtapa 5: Lendo mensagem original de '%s' para comparação...\n", DEFAULT_MESSAGE_FILE);
        status = read_file(DEFAULT_MESSAGE_FILE, original_message_for_test, MAX_MESSAGE_LENGTH);
        if (status != 0) {
            fprintf(stderr, "Erro ao ler o arquivo da mensagem original '%s'. Código: %d. Comparação não será feita.\n", DEFAULT_MESSAGE_FILE, status);
        } else {
            if (strcmp(original_message_for_test, decrypted_output_buffer) == 0) {
                printf("VERIFICAÇÃO: SUCESSO! Texto decifrado corresponde ao original de '%s'.\n", DEFAULT_MESSAGE_FILE);
            } else {
                fprintf(stderr, "VERIFICAÇÃO: FALHA! Texto decifrado NÃO corresponde ao original de '%s'.\n", DEFAULT_MESSAGE_FILE);
                 // fprintf(stderr, "Original: \"%s\"\nDecifrado: \"%s\"\n", original_message_for_test, decrypted_output_buffer);
            }
        }
    }

    // --- Executar os outros testes ---
    printf("\n--- EXECUTANDO TESTES ADICIONAIS ---\n");

    test_decipher_internal("Teste com 'UM' e 'LUCAS'", "UM", "LUCAS");
    test_decipher_internal("Teste com 'SEMB2025' e msg longa", "SEMB2025", "LOREM IPSUM DOLOR SIT AMET, CONSECTETUR ADIPISCING ELIT. CURABITUR NISI EROS, MAXIMUS A FACILISIS ID, ACCUMSAN NEC TORTOR."); // Mensagem mais curta para o log

    test_execution_time_internal();
    test_invalid_character_handling_internal();

    printf("\n--- FIM DOS TESTES ---\n");
    return EXIT_SUCCESS;
}
