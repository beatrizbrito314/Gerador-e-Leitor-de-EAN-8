#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h> // Usada para verificar a existência de arquivos
#include "common.h"   // Arquivo de cabeçalho de funções auxiliares

/* Valores de argumentos padrões que são utilizados quando o usuário 
não informa as especificações */
#define ESPACAMENTO_LATERAL_PADRAO 4
#define LARGURA_AREA_PADRAO 3
#define ALTURA_CODIGO_PADRAO 50
#define NOME_IMAGEM_PADRAO "codigo-de-barras.pbm"

/* Cria uma nova imagem do tipo imgPBM (estrutura definida em common.h).
Aloca a memória para a altura e largura que são passadas como argumento */
imgPBM criarImagem(int largura, int altura) {
    imgPBM imagem; // Declara uma variável do tipo imgPBM
    // Atribuindo valores aos campos de imagem
    imagem.largura = largura;
    imagem.altura = altura;
    // Alocação de memória para ponteiros int na quantidade da altura
    // Reserva memória para os ponteiros que representam as linhas da matriz que formam a imagem
    imagem.pixels = malloc(altura * sizeof(int *)); 
    if (imagem.pixels == NULL) {
        printf("Erro: falha ao alocar memória.\n");
        exit(1);
    }
    // Alocar memória para cada linha (altura)
    for (int i = 0; i < altura; i++) {  
        /* Para cada ponteiro alocado em imagem.pixels,
        aloca memória para a largura (que representa colunas da matriz).
        O calloc inicializa a matriz com 0. */
        imagem.pixels[i] = calloc(largura, sizeof(int)); 
        if (imagem.pixels[i] == NULL) {
            printf("Erro: falha ao alocar memória.\n");
            // Liberar memória alocada, pixel por pixel.
            for (int j = 0; j < i; j++) {
                free(imagem.pixels[j]);
            }
            free(imagem.pixels);
            exit(1);
        }
    }
    // Retorna a matriz de 0
    return imagem;
}

// Libera a memória de cada campo "pixel" da imagem
void liberarImagem(imgPBM *imagem) {
    for (int i = 0; i < imagem->altura; i++) {
        free(imagem->pixels[i]);
    }
    free(imagem->pixels);
    imagem->pixels = NULL;
}

// Insere 0 ou 1 na imagem de acordo com o código informado 
void preencherImagem(imgPBM *imagem, int largura, int altura, char *codificado, int area) {
    int comprimentoCodificado = strlen(codificado);
    int larguraPixel = area; // Cada barra vai ter a largura definida pelo parâmetro "area"
    int margemSuperior = altura / 4;
    int margemInferior = altura / 4;

    // Percorrendo as linhas da matriz
    for (int i = 0; i < altura; i++) {
        // Percorrendo as colunas da matriz
        for (int j = 0; j < largura; j++) {
            // Verifica se a linha atual está dentro das margens
            if (i < margemSuperior || i >= altura - margemInferior) {
                imagem->pixels[i][j] = 0;
            } else {
                // Define em qual barra estamos (utilizando a largura de cada barra)
                int indiceCodificado = j / larguraPixel; // Divide pela largura da barra para saber qual barra é
                if (indiceCodificado < comprimentoCodificado && codificado[indiceCodificado] == '1') {
                    imagem->pixels[i][j] = 1;  // Barra representada por 1
                } else {
                    imagem->pixels[i][j] = 0;  // Espaço representado por 0
                }
            }
        }
    }
}

// Função para salvar a imagem
void salvarImagem(imgPBM *imagem, const char *nomeArquivo) {
   // Escrever no arquivo
    FILE *arquivo = fopen(nomeArquivo, "w");
    if (arquivo == NULL) {
        printf("ERRO: Não foi possível abrir o arquivo.\n");
        return;
    }
    // Tipo do arquivo
    fprintf(arquivo, "P1\n");
    // Largura e altura do arquivo
    fprintf(arquivo, "%d %d\n", imagem->largura, imagem->altura);
    // Escrevendo a matriz de 0 e 1 na imagem (incluindo os 0 das margens)
    for (int i = 0; i < imagem->altura; i++) {
        for (int j = 0; j < imagem->largura; j++) {
            fprintf(arquivo, "%d", imagem->pixels[i][j]);
            if (j < imagem->largura - 1) {
                fprintf(arquivo, " ");
            }
        }
        fprintf(arquivo, "\n");
    }
    fclose(arquivo);
    printf("Imagem salva com sucesso em: %s\n", nomeArquivo);
}

// Verificar se um arquivo com o nome informado existe
bool arquivoExiste(char *nomeArquivo) {
    struct stat infoImg;
    // stat para verificar se o arquivo existe
    if (stat(nomeArquivo, &infoImg) == 0) {
        return true;
    } else {
        return false;
    }
}

// Recebe argumentos por linha de comando
int main(int argc, char *argv[]) {
    //No terminal: testeGerador.exe <codigo ean8>
    // Verifica se o usuário digitou o identificador
    if (argc < 2) {
        printf("Erro: identificador não informado. \n");
        printf("É necessário informar o identificador, os demais argumentos são opcionais.\n");
        fprintf(stderr, "Informe na seguinte ordem: %s [identificador EAN-8] [espacamento] [pixels_por_area] [altura] [arquivoSaida]\n", argv[0]);
        return 1;
    }
    // Recebe o valor do código EAN8 (primeiro argumento)
    char *codBarras = argv[1];
    if (!verificarNum(codBarras)) {
        fprintf(stderr, "ERRO: o identificador possui caracteres não numéricos.\n");
        return 1;
    }
    if (!tamCod(codBarras)) {
        fprintf(stderr, "ERRO: o identificador não possui 8 dígitos.\n");
        return 1;
    }
    if (!codValido(codBarras)) {
        fprintf(stderr, "ERRO: código inválido.\n");
        return 1;
    }
    // Inicializa as variáveis com valores padrão
    int espacamento = ESPACAMENTO_LATERAL_PADRAO;
    int area = LARGURA_AREA_PADRAO;
    int altura = ALTURA_CODIGO_PADRAO;
    char nome[256];
    strcpy(nome, NOME_IMAGEM_PADRAO);

    // Verifica se o número de argumentos informados sobrescrevem os valores atribuídos
    /* Se a quantidade de argumentos passada for suficiente, 
    o valor passado em arg[i] é atribuído à variável */
    if (argc >= 3) {
        espacamento = atoi(argv[2]);
    }
    if (argc >= 4) {
        area = atoi(argv[3]);
    }
    if (argc >= 5) {
        altura = atoi(argv[4]);
    }
    if (argc >= 6) {
        strncpy(nome, argv[5], 255);
        nome[255] = '\0';
    }

    if (arquivoExiste(nome)) {
        printf("O arquivo \"%s\" já existe. Deseja sobrescrevê-lo? (s/n) ", nome);
        char respostaUser[10];
        if (!fgets(respostaUser, sizeof(respostaUser), stdin)) {
            fprintf(stderr, "ERRO: não foi possível ler a resposta.\n");
            return 1;
        }
        if (respostaUser[0] != 's' && respostaUser[0] != 'S') {
            fprintf(stderr, "ERRO: Já existe um arquivo com o mesmo nome.\n");
            return 1;
        }
    }

    
    char codBin[68];
    codBin[0] = '\0';
    convercao(codBarras, codBin);

    
    int larguraCodBarras = strlen(codBin); // Número real de barras
    int novaLarguraMatriz = larguraCodBarras * area; // Multiplica pelo número de pixels por barra
    int larguraTotal = novaLarguraMatriz + (2 * espacamento); // Largura total com o espaçamento lateral
    int alturaTotal = altura + (2 * espacamento); // Altura total com as margens

    imgPBM imagem = criarImagem(larguraTotal, alturaTotal); // Cria a imagem com as novas dimensões
    preencherImagem(&imagem, larguraTotal, alturaTotal, codBin, area); // Preenche a imagem com o código binário
    salvarImagem(&imagem, nome); // Salva a imagem
    liberarImagem(&imagem); // Libera a memória da imagem

    system("pause");

    return 0;
}
