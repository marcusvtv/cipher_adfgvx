# Cifra ADFGVX em C - Ferramenta Modularizada

## Descrição

Este projeto implementa a cifra clássica ADFGVX em linguagem C, com uma estrutura modularizada. Ele oferece funcionalidades para cifrar e decifrar mensagens usando uma chave e uma matriz de Polybius específica. O projeto também inclui um conjunto de testes para validar a funcionalidade da decifragem e outros aspectos do algoritmo.

A cifra ADFGVX é uma cifra de transposição fracionada que combina uma substituição (usando uma matriz de Polybius modificada com os símbolos A, D, F, G, V, X) com uma transposição de colunas regular baseada em uma palavra-chave.

## Funcionalidades

* Cifragem de mensagens a partir de um arquivo de texto.
* Decifragem de mensagens cifradas.
* Operações de leitura e escrita de arquivos para chave, mensagem, texto cifrado e texto decifrado.
* Testes automatizados para:
    * Validar o ciclo de cifragem-decifragem.
    * Verificar o tempo de execução da cifragem.
    * Analisar o tratamento de caracteres inválidos na mensagem.

## Estrutura de Arquivos e Módulos

O projeto está organizado nos seguintes módulos e arquivos:

* **`cipher_config.h`**:
    * Define constantes globais como `MAX_MESSAGE_LENGTH`, `MAX_KEY_LENGTH`.
    * Define os nomes padrão para os arquivos de entrada e saída (ex: `DEFAULT_KEY_FILE`, `DEFAULT_MESSAGE_FILE`, etc.).

* **`file_operations.h` / `file_operations.c`**:
    * Responsável por todas as operações de entrada e saída de arquivos.
    * `read_file()`: Lê o conteúdo de um arquivo para um buffer, removendo caracteres de nova linha.
    * `write_encrypted_data_to_file()`: Escreve a matriz de símbolos cifrados (saída da cifragem) em um arquivo.
    * `write_plaintext_to_file()`: Escreve uma string de texto plano (como a mensagem decifrada) em um arquivo.

* **`adfgvx_core.h` / `adfgvx_core.c`**:
    * Contém a lógica principal para o processo de **cifragem** ADFGVX.
    * A função pública `cipher_adfgvx()` orquestra a cifragem.
    * Funções auxiliares internas (estáticas) como `get_adfgvx_symbols`, `insert_symbol_to_column`, `polybius_encode_to_columns`, `transpose_columns_by_key_order` implementam os passos individuais da cifragem.
    * As constantes `symbols` e `square` (matriz de Polybius) são encapsuladas como `static const` dentro de `adfgvx_core.c`.

* **`adfgvx_decipher.h` / `adfgvx_decipher.c`**:
    * Contém a lógica principal para o processo de **decifragem** ADFGVX.
    * A função pública `decipher_adfgvx()` orquestra a decifragem.
    * Funções auxiliares internas (estáticas) como `symbol_index`, `reverse_transposition`, `reverse_polybius`, `decode_symbols` implementam os passos inversos da cifragem.
    * As constantes `symbols` e `square` também são encapsuladas como `static const` dentro de `adfgvx_decipher.c`.

* **`main.c`**: (Ferramenta Principal de Cifragem e Decifragem Integrada - Opcional)
    * Um programa principal que pode realizar a cifragem de uma mensagem, em seguida, a decifragem, e um teste de verificação. (Conforme a versão `main_c_v3` que você tinha).
    * Utiliza `adfgvx_core.c` para cifrar e `adfgvx_decipher.c` para decifrar.

* **`main_decipher_and_test.c`**: (Nova Ferramenta de Decifragem e Testes)
    * Um programa principal focado em:
        1.  Ler uma chave e um texto previamente cifrado de arquivos.
        2.  Decifrar o texto.
        3.  Salvar o texto decifrado.
        4.  Comparar o texto decifrado com a mensagem original (lida de arquivo) para verificação.
        5.  Executar um conjunto de testes de alto nível (`test_decipher_internal`, `test_execution_time_internal`, `test_invalid_character_handling_internal`).
    * Utiliza `adfgvx_decipher.c` para a decifragem principal e `adfgvx_core.c` para a cifragem necessária dentro das funções de teste.

## Como Compilar

É necessário um compilador C (como GCC) que suporte o padrão C99 (devido ao uso de VLAs - Variable Length Arrays - em algumas partes do código, como na declaração da matriz `encoded_symbol_matrix` no `main` baseada no `key_length`).

Existem dois "targets" principais (executáveis) que podem ser gerados:

1.  **Ferramenta de Cifragem (e Decifragem Integrada, se usando o `main.c` original completo):**
    Este executável pode ser usado para cifrar uma mensagem de `message.txt` usando `key.txt` e salvar em `encrypted.txt`. Se o `main.c` original (`main_c_v3`) for usado, ele também tentará decifrar e verificar.
    ```bash
    gcc -Wall -Wextra -pedantic -std=c99 main.c adfgvx_core.c adfgvx_decipher.c file_operations.c -o adfgvx_cipher_tool
    ```
    *(Se o seu `main.c` for apenas para cifrar, remova `adfgvx_decipher.c` do comando acima).*

2.  **Ferramenta de Decifragem e Testes (usando `main_decipher_and_test.c`):**
    Este executável é focado em decifrar um `encrypted.txt` existente e executar os testes de alto nível.
    ```bash
    gcc -Wall -Wextra -pedantic -std=c99 main_decipher_and_test.c adfgvx_core.c adfgvx_decipher.c file_operations.c -o adfgvx_decipher_tester
    ```

## Como Usar

1.  **Prepare os Arquivos de Entrada:**
    * **`key.txt`**: Crie este arquivo e coloque a chave de cifragem/decifragem em uma única linha (máximo de 8 caracteres, sem espaços extras ou quebras de linha desnecessárias).
        Exemplo de `key.txt`:
        ```
        SEGREDO
        ```
    * **`message.txt`**: Crie este arquivo e coloque a mensagem a ser cifrada. A mensagem deve estar em MAIÚSCULAS e pode conter espaços, vírgulas e pontos (conforme definido na matriz Polybius).
        Exemplo de `message.txt`:
        ```
        ATAQUE AO AMANHECER.
        ```

2.  **Para Cifrar (usando `adfgvx_cipher_tool` se compilado a partir de um `main.c` focado em cifragem):**
    * Compile o `adfgvx_cipher_tool` (se o seu `main.c` estiver configurado para apenas cifrar, o comando de compilação seria: `gcc -Wall ... main.c adfgvx_core.c file_operations.c -o adfgvx_cipher_tool`).
    * Execute: `./adfgvx_cipher_tool`
    * Isso lerá `key.txt` e `message.txt`, e criará (ou sobrescreverá) `encrypted.txt` com a mensagem cifrada.

3.  **Para Decifrar e Testar (usando `adfgvx_decipher_tester`):**
    * Certifique-se de que `key.txt` contém a chave correta e `encrypted.txt` contém o texto cifrado que você deseja decifrar. `message.txt` deve conter a mensagem original para o teste de verificação.
    * Compile o `adfgvx_decipher_tester` usando o comando fornecido na seção "Como Compilar".
    * Execute: `./adfgvx_decipher_tester`
    * O programa irá:
        * Ler `key.txt` e `encrypted.txt`.
        * Decifrar a mensagem.
        * Salvar a mensagem decifrada em `decrypted_test_output.txt` (ou o nome definido em `cipher_config.h`).
        * Comparar a mensagem decifrada com o conteúdo de `message.txt`.
        * Executar os testes adicionais e imprimir seus resultados.

## Descrição dos Principais Métodos (Funções Públicas)

* **`void cipher_adfgvx(char key[], int key_length, char message[], char encoded_symbol_matrix[][MAX_MESSAGE_LENGTH], int symbols_per_column[])`** (em `adfgvx_core.c`)
    * Orquestra o processo de cifragem ADFGVX.
    * Recebe a chave, seu comprimento, a mensagem original, uma matriz para armazenar os símbolos codificados por coluna, e um array para as contagens de símbolos por coluna.
    * Internamente, chama `polybius_encode_to_columns` e `transpose_columns_by_key_order`.

* **`void decipher_adfgvx(char *encrypted_text, char *key, int key_length, char *output)`** (em `adfgvx_decipher.c`)
    * Orquestra o processo de decifragem ADFGVX.
    * Recebe o texto cifrado linearizado, a chave, seu comprimento, e um buffer para armazenar a mensagem decifrada.
    * Internamente, chama `reverse_transposition`, `reverse_polybius`, e `decode_symbols`.

* **`int read_file(const char *filename, char *buffer, int max_length)`** (em `file_operations.c`)
    * Lê a primeira linha de um arquivo para um buffer, com um limite de tamanho. Remove caracteres de nova linha (`\n`, `\r\n`).
    * Retorna 0 em sucesso, ou um código de erro.

* **`int write_encrypted_data_to_file(const char *filename, int key_length, char encoded_symbol_matrix[][MAX_MESSAGE_LENGTH], int symbols_per_column[])`** (em `file_operations.c`)
    * Escreve a saída da cifragem (a matriz de símbolos já transposta) de forma linearizada em um arquivo.

* **`int write_plaintext_to_file(const char *filename, const char *plaintext_message)`** (em `file_operations.c`)
    * Escreve uma string de texto simples (como a mensagem decifrada) em um arquivo.

## Testes Implementados (em `main_decipher_and_test.c`)

O programa `adfgvx_decipher_tester` (compilado a partir de `main_decipher_and_test.c`) executa os seguintes testes:

1.  **Teste de Decifragem Principal:**
    * Lê a chave de `DEFAULT_KEY_FILE` e o texto cifrado de `DEFAULT_ENCRYPTED_FILE`.
    * Decifra o texto.
    * Salva o resultado em `DEFAULT_DECRYPTED_FILE_FOR_TEST`.
    * Lê a mensagem original de `DEFAULT_MESSAGE_FILE`.
    * Compara o texto decifrado com o original e reporta SUCESSO ou FALHA.

2.  **`test_decipher_internal(const char *test_name, char key[], char original_message[])`**:
    * Função de teste auxiliar que cifra uma `original_message` com uma `key` dada, e então decifra o resultado.
    * Compara a mensagem decifrada com a original.
    * **Exemplo de Chamada:** `test_decipher_internal("Teste com 'UM' e 'LUCAS'", "UM", "LUCAS");`
    * **Saída Esperada (para o exemplo acima):**
        ```
        -> Teste de Decifragem: Teste com 'UM' e 'LUCAS'
                Mensagem Original:  "LUCAS"
                Chave:              "UM"
                Texto Cifrado:    "GXFAFXVDAADA" (Nota: este é o cifrado para "LUCASd" do exemplo anterior, para "LUCAS" seria "XFFAADGAAG" se a lógica de transposição for igual à manual)
                                              Verifique a saída exata do seu cifrador para "LUCAS" com chave "UM".
                                              Se for "XFFAADGAAG":
                Texto Cifrado:    "XFFAADGAAG"
                Mensagem Decifrada: "LUCAS"
        SUCESSO: Mensagem decifrada corresponde à original!
        ```
    *(A lógica de cifragem/decifragem exata do seu `cipher_adfgvx` e `decipher_adfgvx` determinará o texto cifrado intermediário exato. O importante é que `original == decifrado`.)*

3.  **`test_execution_time_internal()`**:
    * Mede o tempo de execução da função `cipher_adfgvx` com uma mensagem longa (preenchida com 'A' até `MAX_MESSAGE_LENGTH`) e uma chave de 8 caracteres.
    * Verifica se o tempo de execução está dentro de um limite aceitável (ex: 0.5 segundos, como no seu código original).
    * **Saída Esperada:**
        ```
        -> Teste: Tempo de Execução da Cifragem
                Mensagem: 2559 caracteres 'A'
                Chave: "CHAVE123"
                Tempo de cifragem: X.XXXXXX segundos
        SUCESSO: Tempo de execução dentro do limite de 0.5 segundos.
        ```
        (ou AVISO se exceder)

4.  **`test_invalid_character_handling_internal()`**:
    * Testa como a função `cipher_adfgvx` lida com uma mensagem contendo caracteres que não estão na matriz Polybius. Espera-se que esses caracteres sejam ignorados.
    * Compara o texto cifrado resultante com um valor esperado.
    * **Exemplo de Chamada (usando a lógica do seu código original):**
        Mensagem de entrada: `"L#UC%AS@!d"`
        Chave: `"UM"`
        Texto cifrado esperado (se apenas "LUCASd" for processado): `GXFAFXVDAADA` (conforme cálculo no código de teste)
    * **Saída Esperada:**
        ```
        -> Teste: Tratamento de Caracteres Inválidos na Cifragem
                Mensagem Original com Inválidos: "L#UC%AS@!d"
                Chave: "UM"
                Texto Cifrado Obtido:          "GXFAFXVDAADA"
                Texto Cifrado Esperado (para LUCASd): "GXFAFXVDAADA"
        SUCESSO: Caracteres inválidos ignorados e cifragem correta.
        ```

## Autores

* Lucas Dantas
* Marcus Vinicius

## Licença

MIT License.
