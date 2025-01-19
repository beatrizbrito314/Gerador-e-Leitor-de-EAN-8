#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>
#include "common.h"

#define ESPACAMENTO_LATERAL_PADRAO 4
#define LARGURA_AREA_PADRAO 3
#define ALTURA_CODIGO_PADRAO 50
#define NOME_IMAGEM_PADRAO "codigo-de-barras"

imgPBM criarImagem(int largura, int altura) {
    imgPBM imagem;
    imagem.largura = largura;
    imagem.altura = altura;
    imagem.pixels = malloc(altura * sizeof(int *));  // Alocação para int **
    if (imagem.pixels == NULL) {
        printf("Erro: falha ao alocar memoria.\n");
        exit(1);
    }
    for (int i = 0; i < altura; i++) {
        imagem.pixels[i] = calloc(largura, sizeof(int));  // Alocação para int
        if (imagem.pixels[i] == NULL) {
            printf("Erro: falha ao alocar memoria.\n");
            for (int j = 0; j < i; j++) {
                free(imagem.pixels[j]);
            }
            free(imagem.pixels);
            exit(1);
        }
    }
    return imagem;
}

void liberarImagem(imgPBM *imagem) {
    for (int i = 0; i < imagem->altura; i++) {
        free(imagem->pixels[i]);
    }
    free(imagem->pixels);
    imagem->pixels = NULL;
}

void preencherImagem(imgPBM *imagem, int largura, int altura, const char *codificado) {
    int comprimentoCodificado = strlen(codificado);
    int larguraPixel = 1;
    int margemSuperior = altura / 4;
    int margemInferior = altura / 4;

    for (int i = 0; i < altura; i++) {
        for (int j = 0; j < largura; j++) {
            if (i < margemSuperior || i >= altura - margemInferior) {
                imagem->pixels[i][j] = 0;  // Usando int para representar o valor do pixel
            } else {
                int indiceCodificado = j / larguraPixel;
                if (indiceCodificado < comprimentoCodificado && codificado[indiceCodificado] == '1') {
                    imagem->pixels[i][j] = 1;  // Barra representada por 1
                } else {
                    imagem->pixels[i][j] = 0;  // Espaço representado por 0
                }
            }
        }
    }
}

void salvarImagem(imgPBM *imagem, const char *nomeArquivo) {
    FILE *arquivo = fopen(nomeArquivo, "w");
    if (arquivo == NULL) {
        printf("ERRO: Não foi possivel abrir o arquivo.\n");
        return;
    }
    fprintf(arquivo, "P1\n");
    fprintf(arquivo, "%d %d\n", imagem->largura, imagem->altura);
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

bool arquivoExiste(char *filename) {
    struct stat infoImg;
    if (stat(filename, &infoImg) == 0) {
        return true;
    } else {
        return false;
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Erro: identificador nao informado. \n");
        printf("É necessario informar o identificador, os demais argumentos são opcionais.\n")
        fprintf(stderr, "Informe na seguinte ordem: %s [identificador EAN-8] [espacamento] [pixels_por_area] [altura] [arquivoSaida]\n", argv[0]);
        return 1;
    }
    
    char *codBarras = argv[1];
    if (!verificarNum(codBarras)) {
        fprintf(stderr, "ERRO: o identificador possui caracteres nao numericos.\n");
        return 1;
    }
    if (!tamCod(codBarras)) {
        fprintf(stderr, "ERRO: o identificador nao possui 8 digitos.\n");
        return 1;
    }
    if (!codValido(codBarras)) {
        fprintf(stderr, "ERRO: codigo inválido.\n");
        return 1;
    }

    int espacamento = ESPACAMENTO_LATERAL_PADRAO;
    int area = LARGURA_AREA_PADRAO;
    int altura = ALTURA_CODIGO_PADRAO;
    char nome[256];
    strcpy(nome, NOME_IMAGEM_PADRAO);

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
        printf("O arquivo \"%s\" ja existe. Deseja sobrescreve-lo? (s/n) ", nome);
        char respostaUser[10];
        if (!fgets(respostaUser, sizeof(respostaUser), stdin)) {
            fprintf(stderr, "ERRO: nao foi possivel ler o arquivo.\n");
            return 1;
        }
        if (respostaUser[0] != 's' && respostaUser[0] != 'S') {
            fprintf(stderr, "ERRO: Ja existe um arquivo com o mesmo nome.\n");
            return 1;
        }
    }

    char codBin[68];
    codBin[0] = '\0';
    convercao(codBarras, codBin);

    int novaLarguraMatriz = 67 * area;
    int larguraTotal = novaLarguraMatriz + (2 * espacamento);
    int alturaTotal = altura + (2 * espacamento);

    imgPBM imagem = criarImagem(larguraTotal, alturaTotal);
    preencherImagem(&imagem, larguraTotal, alturaTotal, codBin);
    salvarImagem(&imagem, nome);
    liberarImagem(&imagem);

    system("pause");

    return 0;
}
