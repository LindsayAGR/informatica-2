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
   // Variante binaria: pares (1 byte num, 1 byte char)
    if (len % 2 == 0) {
        int out = 0; bool ok = true;
        for (int i = 0; i < len; i += 2) {
            int num = entrada[i];
            unsigned char c = entrada[i+1];
            if (num == 0) { ok = false; break; }
            if (out + num > maxOut - 1) { ok = false; break; }
            for (int k = 0; k < num; k++) salida[out++] = (char)c;
        }
        if (ok) { salida[out] = '\0'; return out; }
    }

    // Variante binaria: triples (2 bytes num, 1 byte char)
    if (len % 3 == 0) {
        int out = 0; bool ok = true;
        for (int i = 0; i < len; i += 3) {
            int num = (entrada[i] << 8) | entrada[i+1];
            unsigned char c = entrada[i+2];
            if (num == 0) { ok = false; break; }
            if (out + num > maxOut - 1) { ok = false; break; }
            for (int k = 0; k < num; k++) salida[out++] = (char)c;
        }
        if (ok) { salida[out] = '\0'; return out; }
    }

    return -1; // ninguna funcionó
}
