#include <cstdio>
#include <cstdlib>
#include <cstring>

// Constantes
#define MAX_FRAGMENTO 8192         // Tamaño máximo de pista (texto conocido)
#define MAX_SALIDA (6*1024*1024)   // 6 MB para salida descomprimida
#define PREVIEW_MAX 80             // cantidad de chars en preview
const char BASE_DIR[] = "C:/Users/JAIDER RESTREPO/Documents/desafio1/";

// Rotación y XOR

unsigned char rotarDerecha(unsigned char b, int n) {
    return (unsigned char)((b >> n) | (b << (8 - n)));
}
unsigned char aplicarXOR(unsigned char b, unsigned char K) {
    return (unsigned char)(b ^ K);
}
// Descompresión RLE

int descomprimirRLE(const unsigned char* entrada, int len, char* salida, int maxOut) {
    if (!entrada || !salida || len <= 0) return -1;
