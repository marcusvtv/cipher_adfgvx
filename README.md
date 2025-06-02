# Ferramenta de Cifragem e Decifragem ADFGVX em C

## Introdução: Um Programa para Cifragem ADFGVX

Este projeto apresenta uma implementação em C da cifra clássica ADFGVX, um algoritmo histórico de criptografia. O foco principal é fornecer uma ferramenta capaz de **cifrar** mensagens de texto, transformando-as em um formato codificado para proteger sua confidencialidade.

Embora a cifragem seja a funcionalidade central, o programa também oferece a capacidade de **decifrar** mensagens previamente codificadas (assumindo que a mesma chave e lógica de algoritmo sejam usadas) e realizar uma série de **testes** para validar a correção e o desempenho do algoritmo implementado.

## O Algoritmo ADFGVX

A cifra ADFGVX, utilizada durante a Primeira Guerra Mundial, é uma cifra de substituição fracionada combinada com uma transposição colunar. Seu funcionamento pode ser resumido nas seguintes etapas:

### Cifragem:

1.  **Matriz de Polybius Modificada**: Utiliza-se uma matriz quadrada (neste caso, 6x6, definida pelas constantes `square` e `symbols` no código) preenchida com caracteres (letras, números, símbolos). As linhas e colunas desta matriz são nomeadas com os símbolos 'A', 'D', 'F', 'G', 'V', 'X'.
2.  **Substituição**: Cada caractere da mensagem original é localizado na matriz Polybius. Ele é então substituído por um par de símbolos ADFGVX, onde o primeiro símbolo corresponde à linha e o segundo à coluna do caractere na matriz. Caracteres não presentes na matriz são geralmente ignorados (conforme implementado em `get_adfgvx_symbols`).
    * Exemplo: Se 'M' está na linha 'F' e coluna 'A' da matriz, ele é substituído por "FA".
3.  **Formação da Mensagem Intermediária**: Todos os pares de símbolos ADFGVX resultantes da substituição são concatenados para formar uma longa string de símbolos.
4.  **Transposição Colunar com Chave**:
    * Uma palavra-chave secreta é escolhida.
    * Os símbolos da mensagem intermediária são escritos linha por linha sob as letras da palavra-chave, formando colunas. (Função `polybius_encode_to_columns`).
    * As colunas são então reordenadas de acordo com a ordem alfabética das letras da palavra-chave. (Função `transpose_columns_by_key_order`).
    * O texto cifrado final é obtido lendo os símbolos de cada coluna reordenada, de cima para baixo, da esquerda para a direita. (Este processo de leitura para formar a string linear é feito ao salvar no arquivo ou ao preparar para a decifragem).

### Decifragem:

A decifragem é o processo inverso, utilizando a lógica implementada em `adfgvx_decipher.c`:

1.  **Reverter a Transposição Colunar (`reverse_transposition`)**:
    * O texto cifrado linearizado é recebido como entrada.
    * Calcula-se o número de linhas (`rows`) e símbolos extras (`extra`) com base no comprimento do texto cifrado e no `key_length`.
    * Um array `order` é criado e ordenado para refletir a ordem alfabética dos caracteres da chave. `order[i]` passa a conter o índice original da coluna que é a i-ésima na ordem alfabética da chave.
    * O número de símbolos em cada coluna *original* (`col_counts[orig_index]`) é determinado. A lógica que você forneceu (`col_counts[orig_index] = rows + (orig_index < extra ? 1 : 0);`) implica que as primeiras `extra` colunas *na ordem original da chave* são as que recebem um símbolo a mais.
    * A matriz `columns[][]` (representando as colunas na ordem original da chave) é preenchida. Para cada coluna na ordem alfabética da chave (identificada por `order[i]`), o número correspondente de símbolos (dado por `col_counts[order[i]]`) é lido do texto cifrado linearizado e colocado em `columns[order[i]]`.
2.  **Reconstruir a Sequência Intermediária (`reverse_polybius`)**:
    * Os símbolos da matriz `columns` (agora com as colunas na ordem original da chave e com os `col_counts` corretos) são lidos linha por linha, da esquerda para a direita, para reconstruir a string linear `rearranged_symbols` que existia antes da transposição na cifragem.
3.  **Reverter a Substituição de Polybius (`decode_symbols`)**:
    * A string `rearranged_symbols` é lida em pares.
    * Cada par de símbolos ADFGVX é convertido de volta para seus índices numéricos (usando `symbol_index`).
    * Esses índices (linha, coluna) são usados para localizar o caractere correspondente na matriz `square` original.
    * Esses caracteres formam a mensagem decifrada.

## Estrutura de Arquivos (Todos na Mesma Pasta)

Para este projeto, todos os arquivos fonte (`.c`) e de cabeçalho (`.h`) residem na mesma pasta raiz.

* **`cipher_config.h`**: Contém definições de macros globais (ex: `MAX_MESSAGE_LENGTH`, `MAX_KEY_LENGTH`) e nomes de arquivos padrão.
* **`file_operations.h` / `file_operations.c`**: Módulo responsável pelas operações de leitura e escrita de arquivos (`read_file`, `write_encrypted_data_to_file`, `write_plaintext_to_file`).
* **`adfgvx_core.h` / `adfgvx_core.c`**: Módulo contendo a lógica principal para o processo de **cifragem** ADFGVX. A função pública é `cipher_adfgvx()`.
* **`adfgvx_decipher.h` / `adfgvx_decipher.c`**: Módulo contendo a lógica principal para o processo de **decifragem** ADFGVX, utilizando a lógica que você considera melhor. A função pública é `decipher_adfgvx()`.
* **`main_decipher_and_test.c`**: Programa principal que foca na decifragem de um arquivo e na execução de testes de validação.
* **`main.c` (Opcional/Alternativo)**: Poderia ser um programa principal focado apenas na cifragem, ou um que realize tanto cifragem quanto decifragem de forma integrada.

## Principais Métodos (Funções Chave e sua Lógica)

### Em `adfgvx_core.c` (Cifragem):

* **`void cipher_adfgvx(...)`**:
    * Orquestra todo o processo de cifragem ADFGVX.
    * Chama internamente (funções `static`):
        * `get_adfgvx_symbols()`: Localiza um caractere na matriz Polybius (`square`) e retorna seus símbolos ADFGVX correspondentes (`symbols`) para linha e coluna.
        * `insert_symbol_to_column()`: Adiciona um símbolo ADFGVX à próxima posição disponível na coluna correta da `encoded_symbol_matrix`, baseando-se no `symbol_count` e `key_length`. Atualiza `symbols_per_column`.
        * `polybius_encode_to_columns()`: Itera sobre a mensagem original. Para cada caractere, obtém seus dois símbolos ADFGVX e os insere sequencialmente nas colunas da `encoded_symbol_matrix`.
        * `transpose_columns_by_key_order()`: Cria uma cópia da chave (`sorted_key`). Ordena `sorted_key` alfabeticamente. Sempre que dois caracteres em `sorted_key` são trocados durante a ordenação, as colunas correspondentes inteiras na `encoded_symbol_matrix` e seus contadores em `symbols_per_column` também são trocados.

### Em `adfgvx_decipher.c` (Decifragem):

* **`void decipher_adfgvx(char *encrypted_text, char *key, int key_length, char *output)`**:
    * Orquestra o processo de decifragem.
    * Chama internamente (funções `static`):
        * `symbol_index()`: Dado um caractere 'A', 'D', 'F', 'G', 'V', ou 'X', retorna seu índice numérico (0-5).
        * `reverse_transposition()`: Desfaz a transposição colunar. Primeiro, determina a ordem alfabética dos caracteres da chave e os índices originais correspondentes. Depois, calcula o comprimento de cada coluna original (`col_counts[]`) com base na sua posição original (`orig_index`) e no número de símbolos `extra`. Finalmente, preenche a matriz `columns[][]` (que representa as colunas na ordem original da chave) lendo sequencialmente do `encrypted_text` de acordo com os comprimentos calculados para as colunas na ordem em que foram escritas (alfabética da chave).
        * `reverse_polybius()`: Pega a matriz `columns[][]` (com as colunas já na ordem original da chave) e lê os símbolos linha por linha, da esquerda para a direita, para reconstruir a string linear `rearranged_symbols` que existia antes da transposição na cifragem.
        * `decode_symbols()`: Pega a string `rearranged_symbols`, lê os símbolos ADFGVX em pares, usa `symbol_index()` para encontrar suas posições na matriz `square`, e reconstrói os caracteres da mensagem original no buffer `output`.

### Em `file_operations.c`:

* **`int read_file(...)`**: Lê a primeira linha de um arquivo para um buffer, removendo o `\n` ou `\r\n`.
* **`int write_encrypted_data_to_file(...)`**: Escreve a `encoded_symbol_matrix` (saída da cifragem, após a transposição) de forma linearizada para um arquivo, lendo coluna por coluna na ordem em que estão na matriz.
* **`int write_plaintext_to_file(...)`**: Escreve uma string de texto simples (como a mensagem decifrada) para um arquivo.

## Como Compilar (Estrutura Plana)

Assumindo que todos os arquivos `.c` e `.h` estão na mesma pasta e você está compilando a partir dessa pasta:

**Requisitos:** Compilador GCC (ou compatível), padrão C99 ou superior (para VLAs).

1.  **Para compilar a Ferramenta de Decifragem e Testes (`adfgvx_decipher_tester`):**
    ```bash
    gcc main_decipher_and_test.c adfgvx_core.c adfgvx_decipher.c file_operations.c -o adfgvx_decipher_tester
    ```

2.  **Para compilar uma Ferramenta de Cifragem (ex: se você criar um `main_cipher_only.c`):**
    ```bash
    gcc main_cipher_only.c adfgvx_core.c file_operations.c -o adfgvx_cipher_tool
    ```

## Como Usar

1.  **Prepare os Arquivos de Entrada:**
    * **`key.txt`**: Contém a chave de cifragem/decifragem em uma única linha (ex: `SEGREDO`). Máximo de 8 caracteres.
    * **`message.txt`**: Contém a mensagem original a ser cifrada (ex: `ATAQUE AO AMANHECER.`).
    * **`encrypted.txt`**: (Para decifrar) Deve conter o texto cifrado gerado anteriormente.

2.  **Executando (Exemplo com `adfgvx_decipher_tester`):**
    * Primeiro, gere um `encrypted.txt` usando uma ferramenta de cifragem (como a `adfgvx_cipher_tool` compilada a partir de um `main` focado em cifragem, ou o `main.c` original (`main_c_v3`) se ele estiver configurado para cifrar).
    * Execute a ferramenta de decifragem e testes:
        ```bash
        ./adfgvx_decipher_tester
        ```
    * O programa tentará decifrar `encrypted.txt` usando `key.txt`, salvará o resultado em `decrypted_test_output.txt` (ou o nome em `cipher_config.h`), comparará com `message.txt`, e executará testes internos.

## Testes para Validação (em `main_decipher_and_test.c`)

A parte de teste no `main_decipher_and_test.c` é crucial para **validar a correção e a robustez** da nossa implementação da cifra ADFGVX, tanto na cifragem quanto na decifragem. Eles não são parte do processo de cifragem/decifragem para o usuário final, mas sim ferramentas de desenvolvimento para garantir que o algoritmo funciona como esperado.

* **Teste de Decifragem Principal (Fluxo do `main` em `main_decipher_and_test.c`)**:
    * **O que faz**: Simula um cenário de uso real. Lê uma chave e um texto cifrado de arquivos (que você teria gerado previamente), decifra-o, salva o resultado e, mais importante, compara o texto decifrado com a mensagem original (também lida de arquivo).
    * **Validação**: Confirma que o processo de decifragem reverte corretamente a cifragem para dados "do mundo real" lidos de arquivos.

* **`test_decipher_internal(const char *test_name, char key[], char original_message[])`**:
    * **O que faz**: Esta é uma função de teste autocontida. Ela pega uma `original_message` e uma `key` conhecidas (definidas diretamente no código do teste), cifra a mensagem internamente usando `cipher_adfgvx`, e então decifra o resultado usando `decipher_adfgvx`.
    * **Validação**: Verifica a integridade do ciclo completo de cifragem-decifragem para entradas específicas e controladas. É excelente para depurar e confirmar que a lógica central de ambos os processos está funcionando em conjunto corretamente.
        * Exemplo: `test_decipher_internal("Teste com 'UM' e 'LUCAS'", "UM", "LUCAS");`
        * A saída esperada é que a "Mensagem Decifrada" seja idêntica à "Mensagem Original" ("LUCAS").

* **`test_execution_time()`**:
    * **O que faz**: Cifra uma mensagem muito longa (preenchida com 'A's até o `MAX_MESSAGE_LENGTH`) usando uma chave de 8 caracteres (o máximo permitido). Mede o tempo que a função `cipher_adfgvx` leva para completar essa tarefa.
    * **Validação**: Verifica o desempenho da cifragem. Para aplicações como sistemas embarcados ou cenários onde a velocidade é crítica, é importante garantir que o algoritmo não seja excessivamente lento. O teste compara o tempo decorrido com um limite (ex: 0.5 segundos).

* **`test_invalid_character()`**:
    * **O que faz**: Fornece à função `cipher_adfgvx` uma mensagem que contém caracteres que não fazem parte da matriz Polybius definida (ex: `#`, `%`, `@`). A lógica de `get_adfgvx_symbols` deve ignorar esses caracteres inválidos.
    * **Validação**: Compara o texto cifrado resultante com um valor esperado (que seria a cifragem da mensagem contendo apenas os caracteres válidos). Isso confirma que o tratamento de caracteres inválidos está funcionando como projetado, evitando erros ou comportamento inesperado.
        * Exemplo: Para a entrada "L#UC%AS@!d" com chave "UM", o teste espera que o resultado cifrado seja o mesmo que para "LUCASd".

Estes testes, em conjunto, fornecem uma boa cobertura para garantir que a implementação da cifra ADFGVX é correta, funcional e se comporta de maneira previsível sob diferentes condições.

## Autores

* Lucas Dantas
* Marcus Vinicius

## Licença

MIT License.
