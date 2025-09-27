#include <iostream>
#include <fstream>
using namespace std;

//Funciones

unsigned char* leerarchivo(const char* nombre, int &tam) {
    ifstream f(nombre, ios::binary);
    if (!f.is_open()) {
        cout << "Error, el archivo nos e pudo abrir" << nombre << endl;
        tam = 0;
        return NULL;
    }

    f.seekg(0, ios::end);
    tam = f.tellg();
    f.seekg(0, ios::beg);

    unsigned char* buffer = new unsigned char[tam];
    f.read((char*)buffer, tam);
    f.close();
    return buffer;
}


unsigned char rotar(unsigned char b, int rot) {
    rot &= 7;
    return (b >> rot) | (b << (8 - rot));
}

void descifrar(unsigned char* datos, int tam, int rot, unsigned char clave) {
    for (int i = 0; i < tam; i++) {
        unsigned char b = datos[i];
        b = b ^ clave;
        b = rotar(b, rot);
        datos[i] = b;
    }
}

void mostrarbytes(const char* titulo, unsigned char* datos, int tam) {
    cout << "Los" << titulo << " (primeros 50 bytes):\n";
    for (int i = 0; i < tam && i < 50; i++) {
        printf("%02X ", datos[i]);
    }
    cout << "\n";

    cout << titulo << " como texto";
    for (int i = 0; i < tam && i < 50; i++) {
        char c = (datos[i] >= 32 && datos[i] <= 126) ? datos[i] : '.';
        cout << c;
    }
}


//Codigo main
int main() {
    const int n = 4;
    const char* archivos[n] = {
        "Encriptado1.txt",
        "Encriptado2.txt",
        "Encriptado3.txt",
        "Encriptado4.txt"
    };

    int rotaciones[n] = {3, 3, 3, 3};
    unsigned char claves[n] = {0x5A, 0x5A, 0x40, 0x40};

    cout << "Se procesaran los siguientes " << n << "archivos" << endl;

    for (int i = 0; i < n; i++) {
        cout << "Procesando los archivos" << archivos[i] <<endl;

        int tam = 0;
        unsigned char* datos = leerarchivo(archivos[i], tam);
        if (!datos || tam == 0) {
            cout << "Hay un archivo vacio o hubo un erroa a la hora de leerlo" << endl;
            continue;
        }

        mostrarbytes("El original", datos, tam);

        descifrar(datos, tam, rotaciones[i], claves[i]);

        mostrarbytes("El descifrado", datos, tam);

        delete[] datos;
    }

    cout << "Terminado";
    return 0;
}
