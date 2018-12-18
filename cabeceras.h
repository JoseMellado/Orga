typedef struct Nodo{
	char ** instruccion;
	Nodo * siguiente;
}Nodo;

typedef struct IF_ID{
	char ** instruccionCompleta;
	char tipoInst;//tipo puede ser {R, I, J, M (lw y sw)}
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
	int registroRd;
	char * instruccion;
	char tipoInst;
	int nInstruccion;
}EX_MEM;

typedef struct MEM_WB{
	int datoMemoria;
	int resultadoALU;
	int registroRd;
	char * instruccion;
	char tipoInst;
	int nInstruccion;
}MEM_WB;
