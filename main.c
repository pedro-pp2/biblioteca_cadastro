#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int id_registro;
    char titulo[100];
    char isbn[15];
    int ano_publicacao;
} Livro;

void menu();
void limpaBuffer();
void ler_string_segura(char *string, int max_size);
long int tamanho(FILE *arquivo);
void cadastrar(FILE *arquivo);
void consultar(FILE *arquivo);
void gerar_relatorio(FILE *arquivo);
void excluir(FILE *arquivo);

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

    fseek(arquivo, 0, SEEK_END);

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

    fseek(arquivo, indice * sizeof(Livro), SEEK_SET);

    if (fread(&livroConsultado, sizeof(Livro), 1, arquivo) == 1) {
        if (livroConsultado.id_registro > 0) {
            printf("\n--- DADOS DO LIVRO (Posição %ld) ---\n", indice + 1);
            printf("ID de Registro: %d\n", livroConsultado.id_registro);
            printf("Título: %s\n", livroConsultado.titulo);
            printf("ISBN: %s\n", livroConsultado.isbn);
            printf("Ano: %d\n", livroConsultado.ano_publicacao);
        } else {
            printf("\nLivro com ID %d encontrado, mas marcado como EXCLUÍDO.\n", id_busca);
        }
    } else {
        printf("\nErro ao ler o registro.\n");
    }
}

void gerar_relatorio(FILE *arquivo) {
    Livro livro;
    long int num_registros = tamanho(arquivo);
    int contador_validos = 0;
    
    FILE *arq_relatorio = fopen("relatorio_livros.txt", "w"); 

    if (arq_relatorio == NULL) {
        printf("\nERRO: Nao foi possivel criar o arquivo 'relatorio_livros.txt'.\n");
        return;
    }

    printf("\n--- GERANDO RELATÓRIO NO ARQUIVO 'relatorio_livros.txt' ---\n");
    
    char *cabecalho = "ID | TÍTULO | ISBN | ANO\n---|---|---|---\n";
    printf("%s", cabecalho);
    fprintf(arq_relatorio, "--- RELATÓRIO DE LIVROS (%ld registros) ---\n", num_registros);
    fprintf(arq_relatorio, "%s", cabecalho);

    fseek(arquivo, 0, SEEK_SET);

    for (long int i = 0; i < num_registros; i++) {
        if (fread(&livro, sizeof(Livro), 1, arquivo) == 1) {
            if (livro.id_registro > 0) {
                printf("%-2d | %-40s | %-12s | %d\n", 
                       livro.id_registro, 
                       livro.titulo, 
                       livro.isbn, 
                       livro.ano_publicacao);
                
                fprintf(arq_relatorio, "%d | %s | %s | %d\n", 
                         livro.id_registro, 
                         livro.titulo, 
                         livro.isbn, 
                         livro.ano_publicacao);
                contador_validos++;
            }
        }
    }
    
    printf("--- FIM DO RELATÓRIO (Total de Livros Válidos: %d) ---\n", contador_validos);
    fprintf(arq_relatorio, "--- FIM DO RELATÓRIO (Total de Livros Válidos: %d) ---\n", contador_validos);

    fclose(arq_relatorio);
    printf("Arquivo 'relatorio_livros.txt' gerado com sucesso!\n");
}

void excluir(FILE *arquivo) {
    int id_excluir;
    long int num_registros = tamanho(arquivo);
    Livro livroAtual;

    printf("\n--- EXCLUIR LIVRO ---\n");
    printf("Digite o ID do Livro a ser excluído (1 a %ld): ", num_registros);
    if (scanf("%d", &id_excluir) != 1) {
        printf("Entrada inválida.\n");
        limpaBuffer();
        return;
    }
    limpaBuffer();

    long int indice = id_excluir - 1;

    if (indice < 0 || indice >= num_registros) {
        printf("\nID inválido ou livro não encontrado.\n");
        return;
    }

    fseek(arquivo, indice * sizeof(Livro), SEEK_SET);
    if (fread(&livroAtual, sizeof(Livro), 1, arquivo) == 1) {
        if (livroAtual.id_registro == id_excluir && livroAtual.id_registro > 0) {
                
            livroAtual.id_registro = 0; 
            strcpy(livroAtual.titulo, "[EXCLUÍDO]");
            
            fseek(arquivo, indice * sizeof(Livro), SEEK_SET); 
            
            if (fwrite(&livroAtual, sizeof(Livro), 1, arquivo) == 1) {
                printf("\nLivro com ID %d excluído LÓGICAMENTE com sucesso.\n", id_excluir);
            } else {
                printf("\nErro ao tentar sobrescrever o registro para exclusão.\n");
            }
        } else if (livroAtual.id_registro == 0) {
            printf("\nO livro com ID %d já estava logicamente excluído.\n", id_excluir);
        }
    } else {
        printf("\nErro ao ler o registro para exclusão.\n");
    }
}

void menu() {
    printf("\n=== GERENCIADOR DE BIBLIOTECA ===\n");
    printf("1. Cadastrar Livro\n");
    printf("2. Consultar Livro\n");
    printf("3. Gerar Relatório em Arquivo Texto\n");
    printf("4. Excluir Livro\n");
    printf("0. Sair\n");
    printf("Escolha uma opção: ");
}

int main() {
    FILE *arquivo;
    int opcao;

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
                gerar_relatorio(arquivo);
                break;
            case 4:
                excluir(arquivo);
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
