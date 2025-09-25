#include <iostream>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>

using namespace std;

vector<unsigned char> descifrar(vector<unsigned char> datos, unsigned char clave) {
    vector<unsigned char> resultado;
    for (size_t i = 0; i < datos.size(); i++) {   // cambiado a size_t
        resultado.push_back(datos[i] ^ clave);
    }
    return resultado;
}

string descomprimirRLE(vector<unsigned char> datos) {
    string resultado;
    for (size_t i = 0; i < datos.size(); i += 2) {  // cambiado a size_t
        unsigned char caracter = datos[i];
        int cantidad = datos[i + 1];
        for (int j = 0; j < cantidad; j++) {
            resultado += caracter;
        }
    }
    return resultado;
}

string descomprimirLZ78(vector<unsigned char> datos) {
    string resultado;
    vector<string> diccionario(1, "");
    for (size_t i = 0; i < datos.size(); i += 2) {  // cambiado a size_t
        int indice = datos[i];
        char c = datos[i + 1];
        string entrada = diccionario[indice] + c;
        diccionario.push_back(entrada);
        resultado += entrada;
    }
    return resultado;
}

int main() {
    vector<unsigned char> datos = {65, 3, 66, 2, 67, 1}; // Ejemplo RLE: AAA BB C
    vector<unsigned char> datosLZ = {0, 'A', 0, 'B', 0, 'C', 1, 'B'};

    unsigned char clave = 123;

    cout << "Descifrado: ";
    vector<unsigned char> datosDescifrados = descifrar(datos, clave);
    for (size_t i = 0; i < datosDescifrados.size(); i++) {   // cambiado a size_t
        cout << datosDescifrados[i] << " ";
    }
    cout << endl;

    cout << "RLE: " << descomprimirRLE(datos) << endl;
    cout << "LZ78: " << descomprimirLZ78(datosLZ) << endl;

    return 0;
}
