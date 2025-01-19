#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "common.h" 

// Função para encontrar o código de barras EAN-8 na imagem
bool encontrarCodigoNaImagem(const imgPBM *imagem, char *codigoSaida) {
    int largura = imagem->largura; // Largura da imagem
    int altura = imagem->altura;  // Altura da imagem

    // Procuramos por "ccolunas" de pixels que formam a imagem preta e branca
    for (int largColuna = 1; largColuna <= 80; largColuna++) {
        int larguraDoCodigo = 67 * largColuna; // O código de barras(matriz) ocupa 67 "bits"
        if (larguraDoCodigo > largura) {
            break; // Se a largura a matriz formada pelas colunas código não couber na largura da imagem, interrompemos
        }
        // Verificamos linha por linha
        for (int linhaAtual = 0; linhaAtual < altura; linhaAtual++) {
            // Procurando o padrão de código
            // Testamos possíveis posições onde o código pode começar
            // Subtração para evitar ler fora da imagem e gerar erros
            for (int inicioX = 0; inicioX <= largura - larguraDoCodigo; inicioX++) {
                char bits[68]; 
                bits[67] = '\0'; // Termina a string de bits

                for (int bitIndex = 0; bitIndex < 67; bitIndex++) {
                    // Conta a quantidade de pixels pretos
                    int qtdPreto = 0;

                    // Quantos pixels pretos existem na coluna correspondente ao bit
                    for (int posicaoPixel_x = 0; posicaoPixel_x < largColuna; posicaoPixel_x++) {
                        // Calculando a posição do pixel na linha e coluna
                        int posicaoX = inicioX + bitIndex * largColuna + posicaoPixel_x;
                        int indice = linhaAtual * largura + posicaoX; // Cálculo do índice
                        if (imagem->pixels[indice] == 1) { // se encontrar 1
                            qtdPreto++;
                        }
                    }
                    // Se mais da metade dos pixels da coluna são pretos, consideramos que o bit é 1
                    if (qtdPreto > (largColuna / 2)) {
                        bits[bitIndex] = '1';
                    } else {
                        bits[bitIndex] = '0';
                    }
                }

                // Decodificar o código
                char codDecimal[9];
                // função definida em funcoes.h
                if (extraiCodigo(bits, codDecimal)) {
                    // Validar o código
                    if (codValido(codDecimal)) {
                        strcpy(codigoSaida, codDecimal);
                        return true; // Encontrou o código
                    }
                }
            }
        }
    }

    return false; // Não encontrou o código
}

 imgPBM *lerImagemPBM(const char *nomeArquivo) {
    // Abrir o arquivo PBM para leitura
    //r=reader
    FILE *arquivo = fopen(nomeArquivo, "r");
    if (!arquivo) {
        // verifica se consegue abrir o arquivo
        fprintf(stderr, "Erro ao abrir o arquivo PBM: %s\n", nomeArquivo);
        return NULL;
    }
    // Verifica o tipo de arquivo
    char tipo[3];
    // Lê o arquivo como entrada para leitura
    fscanf(arquivo, "%2s", tipo);
    if (strcmp(tipo, "P1") != 0) {
        // Verifica se é P1
        fprintf(stderr, "Erro: formato PBM invalido.\n");
        fclose(arquivo); // Fecha o arquivo
        return NULL;
    }
    // Lê altura e largura do arquivo
    int largura, altura;
    fscanf(arquivo, "%d %d", &largura, &altura);

    // Aloca memória para a estrutura que armazenará a imagem
    imgPBM *imagem = (imgPBM *)malloc(sizeof(imgPBM));
    if (!imagem) {
        fprintf(stderr, "ERRO: Falha ao alocar memoria para o arquivo.\n");
        fclose(arquivo); // Fecha o arquivo
        return NULL;
    }

    // Define a largura e altura da imagem
    imagem->largura = largura;
    imagem->altura = altura;

    // Aloca memória para armazenar os pixels
    imagem->pixels = (int *)malloc(largura * altura * sizeof(int));
    if (!imagem->pixels) {
        fprintf(stderr, "ERRO: Falha ao alocar memoria para o arquivo.\n");
        free(imagem); // Libera a memória da estrutura da imagem
        fclose(arquivo); // Fecha o arquivo
        return NULL;
    }

    // Ler os pixels
    for (int i = 0; i < altura; i++) { // Para cada linha da imagem
        for (int j = 0; j < largura; j++) { // Para cada coluna na linha
            int pixel;
            fscanf(arquivo, "%d", &pixel); // Lê o pixel (0 ou 1)
            imagem->pixels[i * largura + j] = pixel; // Armazena o valor do pixel
        }
    }
    fclose(arquivo);

    // Retorna o ponteiro para a imagem lida
    return imagem;
}

int main(int argc, char *argv[]) {
    // No terminal: ./extrair.exe imagem.pbm
    // ./extrai <arquivo PBM>

    // Verifica a quantidade de argumentos passados no terminal
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <arquivoPBM>\n", argv[0]);
        return 1;
    }

    char *arquivoEntrada = argv[1];

    // Tenta ler a imagem
    imgPBM *imagem = lerImagemPBM(arquivoEntrada);
    if (!imagem) {
        fprintf(stderr, "ERRO: Não foi possivel abrir o arquivo.\n");
        return 1;
    }

    // Tenta encontrar o código
    char codigo[9];
    if (!encontrarCodigoNaImagem(imagem, codigo)) {
        fprintf(stderr, "ERRO: codigo de barras não encontrado.\n");
        free(imagem); // Libera a memória da imagem
        return 1;
    }
    printf("Codigo de barras extraido: %s\n", codigo);

    free(imagem->pixels);
    free(imagem);
    return 0;
    
}
