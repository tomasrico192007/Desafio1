#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <limits>
#include <iomanip>

using namespace std;

// --- Auxiliares ---
// Calcula la longitud de una cadena de caracteres terminada en null.
long longC(const char* s) {
    if (!s) return 0;
    long L = 0;
    while (s[L] != '\0') L++;
    return L;
}

// --- Leer archivo binario a buffer dinámico ---
// Lee el contenido de un archivo binario en un buffer y retorna su tamaño.
unsigned char* leerArchivo(const char* nombre, long& tamano) {
    tamano = 0;
    ifstream f(nombre, ios::binary | ios::ate);
    if (!f.is_open()) {
        cout << "Error: No se pudo abrir el archivo " << nombre << endl;
        return nullptr;
    }
    streampos sp = f.tellg();
    if (sp <= 0) {
        f.close();
        return nullptr;
    }
    tamano = (long)sp;
    f.seekg(0, ios::beg);

    unsigned char* buf = new unsigned char[tamano];
    if (!buf) {
        cout << "Error: Falla de asignación de memoria para leer el archivo." << endl;
        f.close();
        return nullptr;
    }
    f.read(reinterpret_cast<char*>(buf), tamano);
    f.close();
    return buf;
}

// --- Descifrar: XOR (inverso) PRIMERO, luego Rotación (inversa) ---
// Se asume que el cifrado fue: (Rotación izquierda por 'rot') y luego (XOR con 'clave').
// Para revertir: primero XOR y luego Rotación Derecha.
unsigned char* descifrar(const unsigned char* datos, long tamano, int rot, unsigned char clave) {
    if (!datos || tamano <= 0) return nullptr;
    unsigned char* out = new unsigned char[tamano];
    if (!out) {
        cout << "Error: Falla de asignación de memoria para descifrado." << endl;
        return nullptr;
    }

    for (long i = 0; i < tamano; ++i) {
        unsigned char b = datos[i];

        // 1. XOR inverso (XOR es su propia inversa)
        unsigned char x = b ^ clave;

        // 2. Rotación a la derecha por 'rot' (inversa de Rotación Izquierda)
        unsigned char r = (unsigned char)((x >> rot) | (x << (8 - rot)));

        out[i] = r;
    }
    return out;
}

// --- Descompresión RLE ---
// Descomprime datos codificados con RLE (Run-Length Encoding).
char* descomprimirRLE(const unsigned char* datos, long tamano, long& nuevoTam) {
    nuevoTam = 0;
    if (!datos || tamano <= 0) return nullptr;

    long capacidad = 1024;
    char* salida = new char[capacidad];
    if (!salida) return nullptr;

    long i = 0;
    while (i + 1 < tamano) {
        unsigned char ch = datos[i];
        unsigned char cnt = datos[i + 1];
        i += 2;

        long reps = (long)cnt;
        for (long k = 0; k < reps; ++k) {
            if (nuevoTam >= capacidad) {
                long nuevaCap = capacidad * 2;
                char* tmp = new char[nuevaCap];
                if (!tmp) { delete[] salida; return nullptr; }
                memcpy(tmp, salida, nuevoTam);
                delete[] salida;
                salida = tmp;
                capacidad = nuevaCap;
            }
            salida[nuevoTam++] = (char)ch;
        }
    }

    char* finalBuf = new char[nuevoTam + 1];
    if (!finalBuf) { delete[] salida; return nullptr; }
    if (nuevoTam > 0) memcpy(finalBuf, salida, nuevoTam);
    finalBuf[nuevoTam] = '\0';
    delete[] salida;
    return finalBuf;
}

// --- Descompresión LZ78 ---
// Descomprime datos codificados con LZ78.
char* descomprimirLZ78(const unsigned char* datos, long tamano, long& nuevoTam) {
    nuevoTam = 0;
    if (!datos || tamano <= 0) return nullptr;

    long capacidadSalida = 1024;
    char* salida = new char[capacidadSalida];
    if (!salida) return nullptr;

    const int MAX_ENTRADAS = 65536;
    char** dic = new char*[MAX_ENTRADAS];
    if (!dic) { delete[] salida; return nullptr; }

    long entradas = 0;

    // Entrada 0 = cadena vacía
    dic[0] = new char[1];
    dic[0][0] = '\0';
    entradas = 1;

    long i = 0;
    while (i + 2 < tamano) {
        // Leer índice 16-bit big-endian
        int hi = (int)datos[i];
        int lo = (int)datos[i+1];
        int indice = (hi << 8) | lo;
        i += 2;
        if (i >= tamano) break;
        char c = (char)datos[i++];

        if (indice < 0 || indice >= (int)entradas) {
            // Error, la clave o el orden de descifrado es incorrecto.
            cout << "Error LZ78: Indice fuera de rango. Indice=0x" << hex << indice << dec << ", TamanoDic=" << entradas << endl;
            break;
        }

        long baseLen = longC(dic[indice]);
        long newLen = baseLen + 1;
        char* nueva = new char[newLen + 1];

        if (!nueva) break;

        if (baseLen > 0) memcpy(nueva, dic[indice], baseLen);
        nueva[newLen - 1] = c;
        nueva[newLen] = '\0';

        // Añadir a salida
        if (nuevoTam + newLen >= capacidadSalida) {
            long nuevaCap = (nuevoTam + newLen) + 1024;
            char* tmp = new char[nuevaCap];
            if (!tmp) { delete[] nueva; break; }
            if (nuevoTam > 0) memcpy(tmp, salida, nuevoTam);
            delete[] salida;
            salida = tmp;
            capacidadSalida = nuevaCap;
        }
        memcpy(salida + nuevoTam, nueva, newLen);
        nuevoTam += newLen;

        // Guardar en diccionario si hay espacio
        if (entradas < MAX_ENTRADAS) {
            dic[entradas] = nueva;
            entradas++;
        } else {
            delete[] nueva;
        }
    }

    // Ajuste final
    char* finalBuf = new char[nuevoTam + 1];
    if (!finalBuf) {
        for (long k = 0; k < entradas; ++k) delete[] dic[k];
        delete[] dic;
        delete[] salida;
        return nullptr;
    }
    if (nuevoTam > 0) memcpy(finalBuf, salida, nuevoTam);
    finalBuf[nuevoTam] = '\0';
    delete[] salida;

    // liberar diccionario
    for (long k = 0; k < entradas; ++k) delete[] dic[k];
    delete[] dic;

    return finalBuf;
}

// --- Imprimir con seguridad ---
void imprimirSeguro(const char* buf, long tam, long maxLen = -1) {
    if (!buf || tam <= 0) return;
    long toPrint = tam;
    if (maxLen > 0 && maxLen < toPrint) toPrint = maxLen;
    cout.write(buf, toPrint);
    if (toPrint < tam) cout << " (continúa...)" << endl;
    else cout << endl;
}

// --- Funcion de Debug: Imprimir bytes en hexadecimal ---
void debugPrintBytes(const unsigned char* buf, long tam, const char* label) {
    if (!buf || tam <= 0) return;
    long toPrint = (tam < 10) ? tam : 10;
    cout << "  DEBUG - " << label << " (" << toPrint << " bytes): ";
    cout << hex << uppercase << setfill('0');
    for (long i = 0; i < toPrint; ++i) {
        cout << setw(2) << (int)buf[i] << " ";
    }
    cout << dec << nouppercase << setfill(' ') << endl;
}

// --- Programa principal ---
int main() {
    // Configuracion de prueba
    const char* nombres[4] = {"Encriptado1.txt", "Encriptado2.txt", "Encriptado3.txt", "Encriptado4.txt"};

    // Compresión (RLE, LZ78, RLE, LZ78)
    const char* comp_test[4] = {"RLE", "LZ78", "RLE", "LZ78"};

    // Hipótesis Claves:
    // 1 (RLE): 0x5A (Confirmada)
    // 2 (LZ78): 0x5A (PRUEBA FINAL CLAVE)
    // 3 (RLE): 0x40 (Confirmada)
    // 4 (LZ78): 0x40 (PRUEBA FINAL CLAVE)
    const unsigned char key_test[4] = {0x5A, 0x5A, 0x40, 0x40};

    // Rotación (3 para todos)
    const int rot[4] = {3, 3, 3, 3};

    for (int idx = 0; idx < 4; ++idx) {
        cout << "-----------------------------" << endl;
        cout << "Procesando: " << nombres[idx] << endl;
        cout << "  Compresion: " << comp_test[idx] << ", Rotacion: " << rot[idx]
             << ", Key=0x" << hex << (int)key_test[idx] << dec << endl;

        long tamEnc = 0;
        unsigned char* en = leerArchivo(nombres[idx], tamEnc);
        if (!en) continue;

        // Descifrar (Orden Revertido: XOR -> Rotacion Derecha)
        unsigned char* desc = descifrar(en, tamEnc, rot[idx], key_test[idx]);
        delete[] en;

        if (!desc) continue;

        // Debug para LZ78: ¿El inicio del archivo descifrado luce correcto? (Debería empezar con 0x00 00 si la clave es correcta)
        if (strcmp(comp_test[idx], "LZ78") == 0) {
            debugPrintBytes(desc, tamEnc, "Bytes descifrados LZ78");
        }

        // Descomprimir según método
        char* original = nullptr;
        long tamOrig = 0;
        if (strcmp(comp_test[idx], "RLE") == 0) {
            original = descomprimirRLE(desc, tamEnc, tamOrig);
        } else { // LZ78
            original = descomprimirLZ78(desc, tamEnc, tamOrig);
        }

        delete[] desc;

        if (!original) {
            cout << "  Error en descompresion. Clave/Orden incorrecto/a." << endl;
            continue;
        }

        cout << "  Tamano mensaje final: " << tamOrig << " caracteres." << endl;
        cout << "  Mensaje (preview 1000 chars):" << endl;
        imprimirSeguro(original, tamOrig, 1000);

        delete[] original;
    }

    cout << "-----------------------------" << endl;
    cout << "Fin de la prueba." << endl;
    return 0;
}
