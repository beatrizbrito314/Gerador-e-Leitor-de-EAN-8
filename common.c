//definições de funções para mandar p biblioteca
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "commom.h"

//estrutura para a imagem
typedef struct {
    int largura;
    int altura;
    char **pixels; //a imagem é uma matriz de 0(branco) e 1(preto)
}imgPBM;

//função do tipo imgPBM que é responsável por cirar a imagem
imgPBM criarImagem(int largura, int altura) {
//alocação da memória
imgPBM *imagem= malloc(sizeof(imgPBM)); //reservando um espaço na memoria para a imagem
if(imagem==NULL){
    return NULL;//caso a alocação falhe
}
//inicialização dos argumentos altura e largura
imagem.largura=largura;
imagem.altura=altura;
//alocação dos pixels na memoria
//inicializa tudo com 0 (Branco)
image.pixels = malloc(altura * sizeof(char *));//"linhas" da matriz
for (int i = 0; i < altura; i++){
    image.pixels[i] = calloc(largura, sizeof(char));//"colunas" da
}
return imagem;
//retorna uma imagem branca (formada por 0)
}
//-------------------------------------------------------------
//funções para implementar
//função para liberar a memória alocada
//função para o dígito verificador
//desenhar a imagem do codigo de barras





