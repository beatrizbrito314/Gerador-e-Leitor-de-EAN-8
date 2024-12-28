#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
//tabela L-code e R-code q serão usadas para codificar o cod de barras informado pelo user
//cada posicao corresponde ao numera decimal da convercao
const char *L_CODE[] = {"0001101", "0011001", "0010011", "0111101", "0100011", 
                        "0110001", "0101111", "0111011", "0110111", "0001011"};
const char *R_CODE[] = {"1110010", "1100110", "1101100", "1000010", "1011100", 
                        "1001110", "1010000", "1000100", "1001000", "1110100"};

//marcador inicial e final: 101. Marcador central: 01010
//converter de decimal para L-code e R-code
void convercao(char *codBarras, char *codificado){//codbarras é entrada e codificado a saida. São numeros inteiros, mas como char é mais facil de manipular
    //verificar se a quantidade de dígitos esta correta
    if(strlen(codigo) !=8){
        printf("Código de barras inválido!\n");
        return 0;
    }
    //copiando o marcado inicial para a string de saida
    strcpy(codificado, "101");
    //converter os 4 primeiros digitos para l-code
    for(int i = 0; i < 4; i++){
        //concatena cada um dos 4 primeiros digitos (em l-code) com o maracador inicial contido em codificado
        strcat(codificado, L_CODE[codigo[i] - '0']); //-0 para converter para int
    }
    //adicionar marcador central
    strcat(codificado, "01010");
    //converter os 4 ultimos digitos para r-code
    for(int i=4; i<8;i++){
         strcat(codificado, L_CODE[codigo[i] - '0']);}
    //marcador final
    strcat(codificado, "101");
    return 1;
} 
//----------------------------------------------------------------------------------------------------------------
// Estrutura para a imagem PBM
typedef struct {
    int largura;
    int altura;
    char **pixels; // Matriz de 0 (branco) e 1 (preto)
} imgPBM;

// Função para criar a imagem
imgPBM criarImagem(int largura, int altura) {
    imgPBM imagem;

    // Inicializa largura e altura
    imagem.largura = largura;
    imagem.altura = altura;

    // Aloca memória para as linhas da matriz
    imagem.pixels = malloc(altura * sizeof(char *));
    if (imagem.pixels == NULL) {
       printf ("Erro: falha ao alocar memória.\n");
       
    }

    // Aloca memória para as colunas de cada linha e inicializa com 0
    for (int i = 0; i < altura; i++) {
        imagem.pixels[i] = calloc(largura, sizeof(char));//calloc inicializa c 0
        if (imagem.pixels[i] == NULL) {
             printf ("Erro: falha ao alocar memória.\n");
            // Libera as linhas já alocadas em caso de erro
            for (int j = 0; j < i; j++) {
                free(imagem.pixels[j]);
            }
            free(imagem.pixels);
            
        }
    }
    return imagem; //retorna imagem em branco
}

//libera a memoria alocada 
void liberarImagem(imgPBM *imagem) {
    for (int i = 0; i < imagem->altura; i++) {
        free(imagem->pixels[i]);
    }
    free(imagem->pixels);
    imagem->pixels = NULL;
}
