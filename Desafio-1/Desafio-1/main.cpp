
// main.cpp - Versión intermedia (sencilla y comentada)
// Cumple con el Desafío 1: detectar método (RLE o LZ78),
// desencriptar (rotación + XOR), y reconstruir el mensaje.

#include <cstdio>
#include <cstdlib>
#include <cstring>

// =============================
// Constantes
// =============================
#define MAX_FRAGMENTO 8192         // Tamaño máximo de pista (texto conocido)
#define MAX_SALIDA (6*1024*1024)   // 6 MB para salida descomprimida
#define PREVIEW_MAX 80             // cantidad de chars en preview
    const char BASE_DIR[] = "C:/Users/JAIDER RESTREPO/Documents/desafio1/";

// =============================
// Rotación y XOR
// =============================
unsigned char rotarDerecha(unsigned char b, int n) {
    return (unsigned char)((b >> n) | (b << (8 - n)));
}
unsigned char aplicarXOR(unsigned char b, unsigned char K) {
    return (unsigned char)(b ^ K);
}

// =============================
// Descompresión RLE
// =============================
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

// =============================
// Descompresión LZ78
// =============================
int descomprimirLZ78(const unsigned char* entrada, int len, char* salida, int maxOut) {
    if (!entrada || !salida) return -1;
    if (len % 3 != 0) return -1;

    int pares = len / 3;
    int* prefix = (int*) malloc(sizeof(int) * (pares + 2));
    unsigned char* ch = (unsigned char*) malloc(sizeof(unsigned char) * (pares + 2));
    unsigned char* temp = (unsigned char*) malloc(65536);
    if (!prefix || !ch || !temp) { free(prefix); free(ch); free(temp); return -1; }

    int dicSize = 0, out = 0;
    for (int p = 0; p < pares; p++) {
        int off = p * 3;
        int pref = (entrada[off] << 8) | entrada[off+1];
        unsigned char c = entrada[off+2];

        int tlen = 0, cur = pref;
        while (cur > 0) {
            if (cur > dicSize) { free(prefix); free(ch); free(temp); return -1; }
            temp[tlen++] = ch[cur - 1];
            cur = prefix[cur - 1];
        }
        for (int i = tlen - 1; i >= 0; i--) {
            if (out >= maxOut - 1) { free(prefix); free(ch); free(temp); return -1; }
            salida[out++] = (char) temp[i];
        }
        if (out >= maxOut - 1) { free(prefix); free(ch); free(temp); return -1; }
        salida[out++] = (char) c;

        prefix[dicSize] = pref;
        ch[dicSize] = c;
        dicSize++;
    }

    salida[out] = '\0';
    free(prefix); free(ch); free(temp);
    return out;
}

// =============================
// Buscar fragmento en salida
// =============================
int contieneFragmento(const char* texto, int len, const char* frag) {
    int fragLen = (int) strlen(frag);
    if (fragLen == 0) return 1;
    if (fragLen > len) return 0;

    for (int i = 0; i <= len - fragLen; i++) {
        int j = 0;
        while (j < fragLen && texto[i+j] == frag[j]) j++;
        if (j == fragLen) return 1;
    }
    return 0;
}

// =============================
// Leer archivo binario
// =============================
unsigned char* leerArchivoBinario(const char* ruta, int* outLen) {
    FILE* f = fopen(ruta, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END);
    long l = ftell(f);
    rewind(f);
    unsigned char* buf = (unsigned char*) malloc((size_t) l);
    if (!buf) { fclose(f); return NULL; }
    fread(buf, 1, (size_t) l, f);
    fclose(f);
    *outLen = (int) l;
    return buf;
}

// =============================
// Leer primera línea (pista)
// =============================
int leerPrimeraLinea(const char* ruta, char* dest, int size) {
    FILE* f = fopen(ruta, "r");
    if (!f) return -1;
    if (!fgets(dest, size, f)) { fclose(f); return -1; }
    int L = strlen(dest);
    while (L > 0 && (dest[L-1] == '\n' || dest[L-1] == '\r')) dest[--L] = '\0';
    fclose(f);
    return 0;
}

// =============================
// MAIN
// =============================
int main() {
    int casos;
    printf("Ingrese el numero de casos a evaluar: ");
    scanf("%d", &casos);

    for (int caso = 1; caso <= casos; caso++) {
        // Archivos
        char archEnc[256], archPista[256], archOut[256];
        sprintf(archEnc, "%sEncriptado%d.txt", BASE_DIR, caso);
        sprintf(archPista, "%spista%d.txt", BASE_DIR, caso);
        sprintf(archOut, "%sResultado%d.txt", BASE_DIR, caso);

        int lenEnc;
        unsigned char* enc = leerArchivoBinario(archEnc, &lenEnc);
        if (!enc) { printf("No se pudo abrir %s\n", archEnc); continue; }

        char fragmento[MAX_FRAGMENTO];
        if (leerPrimeraLinea(archPista, fragmento, sizeof(fragmento)) != 0) {
            printf("No se pudo abrir %s\n", archPista);
            free(enc); continue;
        }

        unsigned char* desen = (unsigned char*) malloc(lenEnc);
        char* salida = (char*) malloc(MAX_SALIDA);

        int encontrado = 0, nOK = -1, kOK = -1, outLen = 0;
        char metodo[8] = "";

        // Fuerza bruta de n y K
        for (int n = 1; n <= 7 && !encontrado; n++) {
            for (int K = 0; K <= 255 && !encontrado; K++) {
                for (int i = 0; i < lenEnc; i++) {
                    unsigned char x = aplicarXOR(enc[i], (unsigned char) K);
                    desen[i] = rotarDerecha(x, n);
                }
                int r = descomprimirRLE(desen, lenEnc, salida, MAX_SALIDA);
                if (r > 0 && contieneFragmento(salida, r, fragmento)) {
                    encontrado = 1; strcpy(metodo, "RLE");
                    nOK = n; kOK = K; outLen = r; break;
                }
                int l = descomprimirLZ78(desen, lenEnc, salida, MAX_SALIDA);
                if (l > 0 && contieneFragmento(salida, l, fragmento)) {
                    encontrado = 1; strcpy(metodo, "LZ78");
                    nOK = n; kOK = K; outLen = l; break;
                }
            }
        }

        printf("\n=== Caso %d ===\n", caso);
        printf("** Encriptado%d.txt **\n", caso);
        if (encontrado) {
            printf("Compresión: %s\n", metodo);
            printf("Rotación: %d\n", nOK);
            printf("Key= 0x%02X\n", kOK);
            printf("Pista usada: %s\n", fragmento);

            printf("Preview: ");
            int lim = (outLen < PREVIEW_MAX) ? outLen : PREVIEW_MAX;
            for (int i = 0; i < lim; i++) putchar(salida[i]);
            putchar('\n');

            FILE* fo = fopen(archOut, "wb");
            if (fo) {
                fwrite(salida, 1, outLen, fo);
                fclose(fo);
                printf("Resultado guardado en: %s\n", archOut);
            }
        } else {
            printf("No se pudo reconstruir el mensaje\n");
        }

        free(enc); free(desen); free(salida);
    }

    printf("\nProceso finalizado.\n");
    return 0;
}

