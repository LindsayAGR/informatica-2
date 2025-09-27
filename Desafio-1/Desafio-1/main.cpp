#include <cstdio>
#include <cstdlib>
#include <cstring>
using namespace std;
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
  // Variante ASCII "12A3B..."
    {
        int i = 0, out = 0;
        bool ok = true;
        while (i < len) {
            if (!(entrada[i] >= '0' && entrada[i] <= '9')) { ok = false; break; }
            int num = 0;
            while (i < len && entrada[i] >= '0' && entrada[i] <= '9') {
                num = num * 10 + (entrada[i] - '0');
                i++;
            }
            if (i >= len) { ok = false; break; }
            unsigned char c = entrada[i++];
            if (out + num > maxOut - 1) { ok = false; break; }
            for (int k = 0; k < num; k++) salida[out++] = (char)c;
        }
        if (ok) { salida[out] = '\0'; return out; }
    }
