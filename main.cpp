#include <iostream>

using namespace std;

//Funciones
unsigned char rotaralaizquierda(unsigned char byte, int n) {
    n = n % 8;
    return (byte << n) | (byte >> (8 - n));
}

unsigned char rotaraladerecha(unsigned char byte, int n) {
    n = n % 8;
    return (byte >> n) | (byte << (8 - n));
}

void aplicarXOR(unsigned char* datos, int tam, unsigned char clave) {
    for (int i = 0; i < tam; i++) {
        datos[i] = datos[i] ^ clave;
    }
}

int main() {
    unsigned char mensaje[4] = {65, 66, 67, 68}; // 'A','B','C','D'

    cout << "Original: ";
    for (int i = 0; i < 4; i++) cout << mensaje[i] << " ";
    cout << endl;

    aplicarXOR(mensaje, 4, 42);

    cout << "Tras XOR con 42: ";
    for (int i = 0; i < 4; i++) cout << (int)mensaje[i] << " ";
    cout << endl;

    unsigned char rotado = rotaraladerecha(mensaje[0], 3);
    cout << "Primer byte rotado der 3: " << (int)rotado << endl;

    return 0;
}
