#include <iostream>

using namespace std;

//Funciones

//funcion de rotar byte a la izquierda n posiciones
unsigned char rotaralaizquierda(unsigned char byte, int n) {
    n = n % 8;
    return (byte << n) | (byte >> (8 - n));
}
//funcion para rotar elñ byte a la derecha n posiciones
unsigned char rotarDerecha(unsigned char byte, int n) {
    n = n % 8;
    return (byte >> n) | (byte << (8 - n));
}

int main()
{

}
