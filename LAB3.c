#include "caberas.h"

int main(){
	printf("Bienvenido al simulador de MIPS\n\n");
	printf("Ingrese el nombre del archivo MIPS a leer\n");
	char direccion[50];
	scanf("%s",direccion);
	Nodo * lista = leerArchivo(direccion);
	ejecucion(lista);
	return 0;
}