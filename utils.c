#include "cabeceras.h"

//Funci�n que borra espacios innecesarios en una cadena, para as� facilitar el split de este.
//Al usar esta funci�n todas las cadenas quedar�n con el formato: "palabra1 palabra2 ... palabraN"
void borrarEspacios(char * cadena){
	while(cadena[0] == ' '){
		correrString(0,cadena);
	}
	int i;
	for(i = 0; i<strlen(cadena); i++){
	    if(cadena[i] == ' ' && cadena[i+1] == ' '){
	        correrString(i,cadena);
	        i = i-1;
	    }
	}
	if(cadena[strlen(cadena)-1] == ' '){
	    cadena[strlen(cadena)-1] = '\0'; 
	}
}

void correrString(int inicio, char * cadena){
	int i;
	for(i = inicio; i<strlen(cadena)-1;i++){
		cadena[i] = cadena[i+1];
	}
	cadena[strlen(cadena)-1] = '\0';
}

//Funci�n que eleva el entero x al entero y con retorno entero
int intPow(int x, int y){
	int valor = 1;
	while(y > 0){
		valor = valor*x;
		y--;
	}
	return valor;
}

//Funci�n que transforma un string a numero entero. Se asume que cada caracter del string es efectivamente un n�mero.
int stringToInt(char * string){
	int valor = 0;
	int i = strlen(string)-1;
    int j = 0;
	for(i; i>=0;i--,j++){
		if(string[i]-'0' >= 0 && string[i] - '0' <= 9){
			valor = (string[i]-'0')*intPow(10,j)+valor;
		}
		else{
			return -1
		}
	}
	return valor;
}

//Funci�n que busca etiqueta o direcci�n
Nodo * encontraEtiqueta(Nodo * inicio, char * etiqueta, int direccion){
	if(etiqueta = NULL){
		while(inicio != NULL){
			if(strcmp(inicio->instruccion[0],etiqueta) == 0){
				return inicio;
			}
			inicio = inicio->siguiente;
		}
		return NULL;
	}
	else{
		while(inicio != NULL){
			if(inicio->numero == direccion){
				return inicio;
			}
			inicio = inicio->siguiente;
		}
		return NULL;
	}
}
