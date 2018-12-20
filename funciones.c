char* nombresRegistros[32] = {"$zero", "$at", "$v0", "$v1", "$a0", "$a1", "$a2", "$a3", "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", 
	"$t7", "$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7", "$t8", "$t9", "$k0", "$k1", "$gp", "$sp", "$fp", "$ra"};
	
int Registros[32] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
int Memoria[1024];
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
			return descomponer(particion, nodo->siguiente, numeroLinea+4);
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
	//INSTRUCCION I: instruccion, rt, rs, inmediato/etiqueta
	//INSTRUCCION J: instruccion, etiqueta
	if(strcmp(split[0],"sw") == 0 || strcmp(split[0],"lw") == 0){
		particion = strtok(split[2], "(");
		strcpy(split[3], particion);
		particion = strtok(NULL, ")");
		strcpy(split[2], particion);
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
		aux = descomponer(linea, inicio,0);
	}
	int numeroLinea = aux->numero;
	aux = aux->siguiente;
	while(!feof(fp)){
		fgets(linea,256,fp);
		aux = (Nodo *)malloc(sizeof(Nodo));
		aux = descomponer(linea, aux, numeroLinea);
		numeroLinea = aux->numero+4;
		aux = aux->siguiente;
	}
	fclose(fp);
	return inicio;
}

int ejecucion(Nodo * inicial){
	Nodo * aux = inicial;
	int i;
	for(i = 0; i<1024; i++){
		Memoria[i] = 0;
	}
	IF_ID ifId = (IF_ID *)malloc(sizeof(IF_ID));
	ID_EX idEx = (ID_EX *)malloc(sizeof(ID_EX));
	EX_MEM exMem = (EX_MEM *)malloc(sizeof(EX_MEM));
	exMem->writeData = 0;
	MEM_WB memWb = (MEM_WB *)malloc(sizeof(MEM_WB));
	memWb-> wirteData = 0;
	//variables que verifican que alguna etapa del pipeline se haya ejecutado
	int v1 = 0;
	int v2 = 0;
	int v3 = 0;
	int v4 = 0;
	int v5 = 0;
	do{
		//ETAPA 1
		v1 = IF(ifId, aux);
		if(ifId->tipoInst == 'E'){
			aux = aux->siguiente;
			v1 = IF(ifId,aux);
		}
		else if(ifId->tipoInst == 'N'){
			printf("ERROR: Existe un error de sintaxis en la linea %d\n",ifId->nInstruccion/4);
			break;
		}
		//Si hay más instrucciones, se "aumenta el program counter"
		if(ifId->tipoInst != 'V'){
			aux = aux->siguiente;
		}
		//ETAPA 2
			aux = ID(idEx, ifId, aux, inicio);
			//Si no hay instrucción en idEx, se marca v2 en 0, para marcar que no se ejecutó nada en dicha etapa
			if(idEx->tipoInst == 'V'){
				v2 = 0;
			}
			else{
				v2 = 1;
			}
			if((idEx->tipoInst == 'J' || idEx->tipoInst == 'j') && aux == NULL){
				printf("ERROR: La etiqueta o dirección del %s en la linea %d no existe\n",idEx->instruccion, idEx->nInstruccion)
				break;
			}
			//Si no, se pone un NOP en la instrucción anterior, para ello se guarda en el tipoInst una 'V' de vacío 
			else{
				ifId->tipoInst = 'V';
			}
		//ETAPA 3
		v3 = EX();
		v4 = MEM();
		v5 = WB();
		if(aux != NULL){
			aux = aux->siguiente;
		}
	}while(v1 == 1 || v2 == 1 || v3 == 1 || v4 == 1 || v5 == 1);//Si en ninguna etapa se ejecutó algo, se termina el programa.
	free(ifId);
	free(idEx);
	free(exMem);
	free(memWb);
	while(inicial != NULL){
		free(inicial->instruccion);
		aux = inicial;
		inicial = inicial->siguiente;
		free(aux);
	}
}

int identificarRegistro(char * registro){
	int i;
	for(i = 0; i<32; i++){
		if(strcmp(registro,Registros[i]) == 0){
			return i;
		}
	}
	return -1;
}

//Función que busca etiqueta o dirección
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

int IF(IF_ID ifId, Nodo * nodo){
	if(nodo == NULL){
		ifId->tipoInst = 'V';
		return 0;
	}
	ifId->nInstruccion = nodo->numero;
	char ** instruccion = nodo->instruccion;
	ifId->instruccionCompleta = instruccion;
	if(strcmp("add",instruccion[0])== 0 || strcmp("sub", instruccion[0]) == 0 || strcmp("mul",instruccion[0])==0 || strcmp("div",instruccion[0])==0)
	{
		ifId->tipoInst = 'R';
	} 
	else if(strcmp("addi",instruccion[0])==0 || strcmp("subi",instruccion[0])==0 || strcmp("addiu",instruccion[0]) == 0 || strcmp("bgt", instruccion[0])==0 || strcmp("beq",instruccion[0])==0 || strcmp("blt",instruccion[0])==0 ||
			strcmp("bne",instruccion[0])==0){
		ifId->tipoInst = 'I';		
	}
	else if(strcmp("j",instruccion[0])== 0 || strcmp("jal", instruccion[0]) == 0)
	{
		ifId->tipoInst = 'J';
	}
	else if(strcmp("jr",instruccion[0]) == 0){
		idId->tipoInst = 'j'
	}
	else if (instruccion[0][strlen(instruccion[0]-1)] == ':'){
		ifId->tipoInst = 'E';
	}
	else{
		ifId->tipoInst = 'N';
	}
	return 1;
}

Nodo * ID(ID_EX idEx, IF_ID ifId, Nodo * actual, Nodo * inicio){
	if(ifId->tipoInst == 'V'){
		idEx->tipoInst = ifId->tipoInst;
		return actual;
	}
	if(ifId->tipoInst == 'R'){
		idEx->registroRd = identificarRegistro(ifId->instruccionCompleta[1]);
		idEx->registroRs = identificarRegistro(ifId->instruccionCompleta[2]);
		idEx->registroRt = identificarRegistro(ifId->instruccionCompleta[3]);
		idEx->dato1 = Registros[idEx->registroRs];
		idEx->dato2 = Registros[idEx->registroRt];
	}
	else if(ifId->tipoInst == 'I'){
		idEx->registroRt = identificarRegistro(ifId->instruccionCompleta[1]);
		idEx->registroRs = identificarRegistro(ifId->instruccionCompleta[2]);
		idEx->inmediato = ifId->instruccionCompleta[3];
		idEx->dato1 = Registros[idEx->registroRs];
	}
	else if(ifId->tipoInst == 'J'){
		//hazard control falta jal
		if(stringToInt(ifId->instruccionCompleta[1]) != -1){
			actual = encontraEtiqueta(inicio,NULL, stringToInt(ifId->instruccionCompleta[1]));
		}
		else{
			actual = encontraEtiqueta(inicio, ifId->instruccionCompleta[1], 0);
		}
		//Si es instruccion jal, se guarda en el registroRd del buffer el registro 31 (ra) y en dato1 el valor del program counter+4
		if(strcmp("jal",ifId->instruccionCompleta[0]) == 0){
			idEx->registroRd = 31;
			idEx->dato1 = actual->numero;
		}
	}
	else if(strcmp(ifId->tipoInst == 'j')){
		//hazard control
		idEx->registroRs = identificarRegistro(ifId->instruccionCompleta[1]);
		actual = encontraEtiqueta(inicio, NULL, Registros[idEx->registroRs]);
	}
	idEx->nInstruccion = ifId->nInstruccion;
	idEx->instruccion = ifId->instruccionCompleta[0];
	idEx->tipoInst = ifId->tipoInst;
	return actual;
}



Nodo * EX(EX_MEM exMem, ID_EX idEx, Nodo * actual, Nodo * inicio){
	if(idEx->tipoInst == 'V'){
		exMem->tipoInst = idEx->tipoInst;
		return actual;
	}
	if(idEx->tipoInst == 'R'){
		//FORWARDING
		if(exMem->writeData != 0){
			if(exMem->writeData == idEx->registroRs){
				idEx->dato1 = exMem->resultadoALU;
			}
			else if(exMem->writeData == idEx->registroRt){
				idEx->dato2 = exMem->resultadoALU;
			}
		}
		else if(memWb->writeData != 0){
			if(memWb->writeData == idEx->registroRs){
				idEx->dato1 = exMem->resultadoALU;
			}
			else if(memWb->writeData == idEx->registroRt){
				idEx->dato2 = exMem->resultadoALU;
			}
		}
		//Ejecución de operación
		if(strcmp(idEx->instruccion,"add") == 0){
			exMem->resultadoALU = idEx->dato1+idEx->dato2;
		}
		else if(strcmp(idEx->instruccion,"sub") == 0){
			exMem->resultadoALU = idEx->dato1-idEx->dato2;
		}
		else if(strcmp(idEx->instruccion,"mul") == 0){
			exMem->resultadoALU = idEx->dato1*idEx->dato2;
		}
		else if(strcmp(idEx->instruccion,"div") == 0){
			exMem->resultadoALU = idEx->dato1/idEx->dato2;
		}
		exMem->writeData = idEx->registroRd;
	}
	else if(idEx->tipoInst == 'I'){
		//FORWARDING
		if(exMem->writeData != 0 && exMem->writeData == idEx->registroRs){
			idEx->dato1 = exMem->resultadoALU;
		}
		else if(memWb->writeData != 0 && memWb->writeData == idEx->registroRs){
			idEx->dato1 = memWb->resultadoALU;
		}
		//Ejecución de operación
		
	}
	exMem->tipoInst = idEx->tipoInst;
	exMem->instruccion = idEx->instruccion;
	exMem->nInstruccion = idEx->nInstruccion;
}

