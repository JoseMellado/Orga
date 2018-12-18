char* nombresRegistros[32] = {"$zero", "$at", "$v0", "$v1", "$a0", "$a1", "$a2", "$a3", "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", 
	"$t7", "$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7", "$t8", "$t9", "$k0", "$k1", "$gp", "$sp", "$fp", "$ra"};
	
int Registros[32] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
int Memoria[1000];
FILE * fp;

//Función que particiona una cadena de caracteres por espacios.
Nodo * descomponer(char * cadena, Nodo * nodo, int numeroLinea){
	char ** split = (char **)calloc(4,sizeof(char *));
	int i;
	for(i = 0; i<4; i++){
		split[i] = (char *)calloc(20, sizeof(char));
	}
	char * particion = strtok(cadena, " ");
	//Si la primera palabra contiene un ':' en su último caracter, se toma como etiqueta.
	if(particion[strlen(particion)-1] == ':'){
		strcpy(split[0],particion);
		nodo->instruccion = split;
		nodo->numero = numeroLinea;
		if(particion[strlen(particion)] == '\0'){
			return nodo;
		}
		else{
			nodo->siguiente = (Nodo *)malloc(sizeof(Nodo));
			particion = strtok(NULL; "\0");
			return descomponer(particion, nodo->siguiente, numeroLinea++);
		}
	}
	i = 0;
	while(particion != NULL){
		if(particion[strlen(particion) - 1] == ','){
            particion[strlen(particion)-1] = '\0';
        }
        strcpy(split[i], particion);
        particion = strtok(NULL, " ");
        i++;
	}
	//INSTRUCCION R: instruccion, rd, rs, rt
	//INSTRUCCION I: instruccion, rs, rt, inmediato/etiqueta
	//INSTRUCCION SW-LW: instruccion, rt, inmediato, rs
	//INSTRUCCION J: instruccion, etiqueta
	if(strcmp(split[0],"sw") == 0 || strcmp(split[0],"lw") == 0){
		particion = strtok(split[2], "(");
		strcpy(split[2], particion);
		particion = strtok(NULL, ")");
		strcpy(split[3], particion);
	}
	nodo->instruccion = split;
	nodo->numero = numeroLinea;
	return nodo;
}

//Función que lee el archivo que contiene las instrucciones MIPS, cada línea que representa una instrucción 
//se guarda en una lista
Nodo * leerArchivo(char * direccion){
	Nodo * inicio;
	Nodo * aux;
	char linea[256];
	fp = fopen(direccion,"r");
	fgets(linea, 256, fp);
	borrarEspacios(linea);
	if(linea != NULL){
		inicio = (Nodo *)malloc(sizeof(Nodo));
		aux = descomponer(linea, inicio,1);
	}
	int numeroLinea = aux->numero;
	aux = aux->siguiente;
	while(!feof(fp)){
		fgets(linea,256,fp);
		aux = (Nodo *)malloc(sizeof(Nodo));
		aux = descomponer(linea, aux, numeroLinea);
		numeroLinea = aux->numero++;
		aux = aux->siguiente;
	}
	fclose(fp);
	return inicio;
}

void ejecucion(Nodo * inicial){
	Nodo * aux = inicial;
	int i;
	for(i = 0; i<1000; i++){
		Memoria[i] = 0;
	}
	IF_ID buffer1 = (IF_ID *)malloc(sizeof(IF_ID));
	ID_EX buffer2 = (ID_EX *)malloc(sizeof(ID_EX));
	EX_MEM buffer3 = (EX_MEM *)malloc(sizeof(EX_MEM));
	MEM_WB buffer4 = (MEM_WB *)malloc(sizeof(MEM_WB));
	//variables que verifican que alguna etapa del pipeline se haya ejecutado
	int v1 = 0;
	int v2 = 0;
	int v3 = 0;
	int v4 = 0;
	int v5 = 0;
	do{
		v1 = IF(buffer1, aux);
		aux = aux->siguiente;
		v2 = ID();
		v3 = EX();
		v4 = MEM();
		v5 = WB();
		if(aux != NULL){
			aux = aux->siguiente;
		}
	}while(v1 == 1 || v2 == 1 || v3 == 1 || v4 == 1 || v5 == 1);
}

void ID(IF_ID buffer, Nodo * nodo){
	IF_ID->nInstruccion = nodo->numero;
	char ** instruccion = nodo->instruccion;
	IF_ID->instruccionCompleta = instruccion;
	if(strcmp("add",instruccion[0])== 0 || strcmp("sub", instruccion[0]) == 0 || strcmp("mul",instruccion[0])==0 || strcmp("div",instruccion[0])==0) || strcmp("jr",instruccion[0])==0)
	{
		IF_ID->tipoInst = 'R';
	} 
	else if(strcmp("addi",instruccion[0])==0 || strcmp("subi",instruccion[0])==0 || strcmp("addiu",instruccion[0]) == 0 || strcmp("bgt", instruccion[0])==0 || strcmp("beq",instruccion[0])==0 || strcmp("blt",instruccion[0])==0 ||
			strcmp("bne",instruccion[0])==0){
		IF_ID->tipoInst = 'I';		
	}
	else if(strcmp("lw",instruccion[0])==0 || strcmp("sw",instruccion[0])==0){
		IF_ID->tipoInst = 'M';
	}
	else if(strcmp("j",instruccion[0])== 0 || strcmp("jal", instruccion[0]) == 0)
	{
		IF_ID->tipoInst = 'J';
	}
}
