#include "common.h"
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Tabelas para codificar o código
const char *L_CODE[] = {"0001101", "0011001", "0010011", "0111101", "0100011", 
                        "0110001", "0101111", "0111011", "0110111", "0001011"};

const char *R_CODE[] = {"1110010", "1100110", "1101100", "1000010", "1011100", 
                        "1001110", "1010000", "1000100", "1001000", "1110100"};

// Função para verificar se o código tem o tamanho correto
bool tamCod(const char *cod_barras){
    int tam = strlen(cod_barras);
    if(tam != 8) {
        return false;
    }
    return true;
}

// Função para verificar se todos os caracteres são dígitos
bool verificarNum(const char *cod_barras) {
    for (int i = 0; cod_barras[i] != '\0'; i++) {
        if (!isdigit(cod_barras[i])) {
            return false;
        }
    }
    return true;
}

// Função para calcular o dígito verificador
int digitoVerificador(const char *cod_barras) {
    int soma = 0;
    for (int i = 0; i < 7; i++) {
        int digito = cod_barras[i] - '0';
        if ((i + 1) % 2 == 1) { 
            soma += digito * 3;
        } else {
            soma += digito * 1;
        }
    }
    int resto = soma % 10;
    return (resto == 0) ? 0 : 10 - resto;
}

// Função para validar o código de barras
bool codValido(const char *cod_barras) {
    if (!verificarNum(cod_barras)) {
        return false;
    }
    if (!tamCod(cod_barras)) {  
        return false;
    }
    int verificador = digitoVerificador(cod_barras);
    int ultimoD = cod_barras[7] - '0';
    return ultimoD == verificador;
}

// Função para codificar o código de barras
void convercao(const char *codBarras, char *codificado){
    strcpy(codificado, "101");
    for (int i = 0; i < 4; i++) {
        strcat(codificado, L_CODE[codBarras[i] - '0']);
    }
    strcat(codificado, "01010");
    for (int i = 4; i < 8; i++) {
        strcat(codificado, R_CODE[codBarras[i] - '0']);
    }
    strcat(codificado, "101");
}

// Função para decodificar o código à esquerda (L)
int decodeDigitL(const char *codificado) {
    for (int d = 0; d < 10; d++) {
        if (strncmp(codificado, L_CODE[d], 7) == 0) {
            return d;
        }
    }
    return -1; 
}

// Função para decodificar o código à direita (R)
int decodeDigitR(const char *codificado) {
    for (int d = 0; d < 10; d++) {
        if (strncmp(codificado, R_CODE[d], 7) == 0) {
            return d;
        }
    }
    return -1; 
}

// Função para extrair o código de barras a partir da versão codificada
bool extraiCodigo(const char *codificado, char *novoCod) {
    if (strlen(codificado) != 67) {
        return false;
    }

    if (strncmp(codificado, "101", 3) != 0) return false;
    if (strncmp(codificado + 31, "01010", 5) != 0) return false;
    if (strncmp(codificado + 64, "101", 3) != 0) return false;

    char temp[8];
    int comecoCod = 3; 

    for (int i = 0; i < 4; i++) {
        strncpy(temp, codificado + comecoCod, 7);
        temp[7] = '\0';
        int decode = decodeDigitL(temp);
        if (decode < 0) return false;
        novoCod[i] = (char)('0' + decode);
        comecoCod += 7;
    }

    comecoCod += 5; 

    for (int i = 4; i < 8; i++) {
        strncpy(temp, codificado + comecoCod, 7);
        temp[7] = '\0';
        int d = decodeDigitR(temp);
        if (d < 0) return false;
        novoCod[i] = (char)('0' + d);
        comecoCod += 7;
    }

    novoCod[8] = '\0';
    return true;
}
