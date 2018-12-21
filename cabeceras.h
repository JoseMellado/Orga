#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct Nodo{
	char ** instruccion;
	int numero;
	Nodo * siguiente;
}Nodo;

typedef struct IF_ID{
	char ** instruccionCompleta;
	char tipoInst;//tipo puede ser {R, I, J, j(jr), E(etiqueta), N(no instruccion, V (vacío))}
	int nInstruccion;
}IF_ID;

typedef struct ID_EX{
	int dato1;
	int dato2;
	char * inmediato;
	int registroRs;
	int registroRt;
	int registroRd;
	char * instruccion;
	char tipoInst;
	int nInstruccion;
}ID_EX;

typedef struct EX_MEM{
	int resultadoALU;
	int dato2;
	int writeData;
	char * instruccion;
	char tipoInst;
	int nInstruccion;
	int salto;
	int hazardLw;
}EX_MEM;

typedef struct MEM_WB{
	int datoMemoria;
	int resultadoALU;
	int writeData;
	char * instruccion;
	char tipoInst;
	int nInstruccion;
}MEM_WB;


void borrarEspacios(char * cadena);
void correrString(int inicio, char * cadena);
int intPow(int x, int y);
int stringToInt(char * string);
Nodo * encontraEtiqueta(Nodo * inicio, char * etiqueta, int direccion);
Nodo * descomponer(char * cadena, Nodo * nodo, int numeroLinea);
Nodo * leerArchivo(char * direccion);
void ejecucion(Nodo * inicial);
int identificarRegistro(char * registro);
int IF(IF_ID ifId, Nodo * nodo);
Nodo * ID(ID_EX idEx, IF_ID ifId, Nodo * inicio);
Nodo * EX(EX_MEM exMem, ID_EX idEx, Nodo * inicio, MEM_WB memWb);
int MEM(MEM_WB memWb, EX_MEM exMem);
int WB(MEM_WB memWb);