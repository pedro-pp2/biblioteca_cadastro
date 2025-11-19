#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct {
    int id_registro; 
    char titulo[100]; 
    char isbn[15]; 
    int ano_publicacao; 
} Livro;

// Protótipos de Funções
void menu();
void limpaBuffer(); 
void ler_string_segura(char *string, int max_size); 
long int tamanho(FILE *arquivo); 
void cadastrar(FILE *arquivo);
void consultar(FILE *arquivo);

// =========================================================================

void limpaBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}


void ler_string_segura(char *string, int max_size) {
    if (fgets(string, max_size, stdin) != NULL) {
        string[strcspn(string, "\n")] = '\0'; 
    } else {
        string[0] = '\0'; 
    }
}


long int tamanho(FILE *arquivo) {
    long int tam_bytes;
    fseek(arquivo, 0, SEEK_END); 
    tam_bytes = ftell(arquivo); 
    fseek(arquivo, 0, SEEK_SET); 

    return tam_bytes / sizeof(Livro); 
}


void cadastrar(FILE *arquivo) {
    Livro novoLivro;
    long int num_registros = tamanho(arquivo);

    printf("\n--- CADASTRAR LIVRO ---\n");
    novoLivro.id_registro = (int)(num_registros + 1); 
    printf("ID de Registro: %d\n", novoLivro.id_registro);

    printf("Título: ");
    ler_string_segura(novoLivro.titulo, 100); 

    printf("ISBN: ");
    ler_string_segura(novoLivro.isbn, 15); 

    printf("Ano de Publicação: ");
    if (scanf("%d", &novoLivro.ano_publicacao) != 1) {
        printf("Entrada inválida.\n");
        limpaBuffer();
        return;
    }
    limpaBuffer(); 

    // Posiciona o ponteiro no final do arquivo
    fseek(arquivo, 0, SEEK_END); 

    // Salva a struct
    if (fwrite(&novoLivro, sizeof(Livro), 1, arquivo) == 1) { 
        printf("\nLivro cadastrado com sucesso na posição %ld!\n", num_registros + 1);
    } else {
        printf("\nErro ao escrever no arquivo.\n");
    }
}


void consultar(FILE *arquivo) {
    int id_busca;
    long int num_registros = tamanho(arquivo);
    Livro livroConsultado;

    printf("\n--- CONSULTAR LIVRO ---\n");
    printf("Digite o ID do Livro (1 a %ld): ", num_registros);
    if (scanf("%d", &id_busca) != 1) {
        printf("Entrada inválida.\n");
        limpaBuffer();
        return;
    }
    limpaBuffer();

    long int indice = id_busca - 1; 

    if (indice < 0 || indice >= num_registros) {
        printf("\nID inválido ou livro não encontrado.\n");
        return;
    }

    // Posiciona o ponteiro no índice desejado
    fseek(arquivo, indice * sizeof(Livro), SEEK_SET); 

    // Lê a struct
    if (fread(&livroConsultado, sizeof(Livro), 1, arquivo) == 1) { 
        printf("\n--- DADOS DO LIVRO (Posição %ld) ---\n", indice + 1);
        printf("ID de Registro: %d\n", livroConsultado.id_registro);
        printf("Título: %s\n", livroConsultado.titulo);
        printf("ISBN: %s\n", livroConsultado.isbn);
        printf("Ano: %d\n", livroConsultado.ano_publicacao);
    } else {
        printf("\nErro ao ler o registro.\n");
    }
}

void menu() {
    printf("\n=== GERENCIADOR DE BIBLIOTECA ===\n");
    printf("1. Cadastrar Livro\n");
    printf("2. Consultar Livro (por ID)\n");
    printf("3. Mostrar Total de Livros\n");
    printf("0. Sair\n");
    printf("Escolha uma opção: ");
}

// Função principal
int main() {
    FILE *arquivo;
    int opcao;

    // Abrir/Criar um arquivo binário (r+b ou w+b)
    arquivo = fopen("biblioteca.bin", "r+b"); 

    if (arquivo == NULL) {
        arquivo = fopen("biblioteca.bin", "w+b");
        if (arquivo == NULL) {
            printf("Erro: Nao foi possivel criar ou abrir o arquivo!\n");
            return 1;
        }
        printf("Arquivo 'biblioteca.bin' criado com sucesso!\n");
    } else {
        printf("Arquivo 'biblioteca.bin' aberto com sucesso!\n");
    }

    do {
        menu();
        if (scanf("%d", &opcao) != 1) {
            opcao = -1; 
            limpaBuffer();
        } else {
            limpaBuffer();
        }

        switch (opcao) {
            case 1:
                cadastrar(arquivo);
                break;
            case 2:
                consultar(arquivo);
                break;
            case 3:
                printf("\nTotal de Livros: %ld\n", tamanho(arquivo));
                break;
            case 0:
                printf("\nSaindo do sistema...\n");
                break;
            default:
                printf("\nOpcao invalida. Tente novamente.\n");
        }
    } while (opcao != 0);

    fclose(arquivo); 
    return 0;
}