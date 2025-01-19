#ifndef COMMON_H
#define COMMON_H
#include <stdbool.h>

typedef struct {
    int largura;
    int altura;
    char **pixels;
} imgPBM;


// Funções de verificação
bool tamCod(const char *cod_barras);
bool verificarNum(const char *cod_barras);
bool codValido(const char *cod_barras);

// Funções de cálculo
int digitoVerificador(const char *cod_barras);

// Funções de codificação e decodificação
void convercao(const char *codBarras, char *codificado);
bool extraiCodigo(const char *codificado, char *novoCod);

// Funções de decodificação
int decodeDigitL(const char *codificado);
int decodeDigitR(const char *codificado);

#endif
