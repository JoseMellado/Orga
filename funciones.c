#include "cabeceras.h"

char* nombresRegistros[32] = {"$zero", "$at", "$v0", "$v1", "$a0", "$a1", "$a2", "$a3", "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", 
	"$t7", "$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7", "$t8", "$t9", "$k0", "$k1", "$gp", "$sp", "$fp", "$ra"};
	
int Registros[32] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
int Memoria[1024];
FILE * fp;
FILE * deteccion;
int ciclos = 1;
int numeroHazards = 1;

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
			return descomponer(particion, nodo->siguiente, numeroLinea);
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

void ejecucion(Nodo * inicial){
	Nodo * aux = inicial;
	Nodo * saltoBranch;
	Nodo * saltoJump;
	int ciclos = 1;
	int numeroHazards = 1;
	int i;
	for(i = 0; i<1024; i++){
		Memoria[i] = 0;
	}
	IF_ID ifId = (IF_ID *)malloc(sizeof(IF_ID));
	ID_EX idEx = (ID_EX *)malloc(sizeof(ID_EX));
	EX_MEM exMem = (EX_MEM *)malloc(sizeof(EX_MEM));
	exMem->writeData = 0;
	exMem->salto = 0;
	MEM_WB memWb = (MEM_WB *)malloc(sizeof(MEM_WB));
	memWb-> wirteData = 0;
	//variables que verifican que alguna etapa del pipeline se haya ejecutado
	int v1 = 0;
	int v2 = 0;
	int v3 = 0;
	int v4 = 0;
	int v5 = 0;
	fp = fopen("HAZARDS.txt","w");
	deteccion = fopen("DETECCION.txt","w");
	do{
		//ETAPA 5
		v5 = WB(memWb);
		//ETAPA 4
		v4 = MEM(memWb, exMem);
		//ETAPA 3
		saltoBranch = EX(exMem, idEx, actual, inicio, memWb);
		//Si no hay instrucción en exMem, se marca v3 en 0, para marcar que no se ejecutó nada en dicha etapa
		if(idEx->tipoInst == 'V'){
			v3 = 0;
		}
		else{
			v3 = 1;
		}
		if(!exMem->hazardLw){
			if(strcmp(exMem->instruccion,"beq")==0 || strcmp(exMem->instruccion,"bne")==0 || strcmp(exMem->instruccion,"blt")==0 || strcmp(exMem->instruccion,"bgt")==0){
				if(saltoBranch == NULL){
					printf("ERROR: La etiqueta o dirección del %s en la linea %d no existe\n",idEx->instruccion, idEx->nInstruccion)
					break;
				}
			}
			//ETAPA 2
				saltoJump = ID(idEx, ifId, inicio);
				//Si no hay instrucción en idEx, se marca v2 en 0, para marcar que no se ejecutó nada en dicha etapa
				if(idEx->tipoInst == 'V'){
					v2 = 0;
				}
				else{
					v2 = 1;
				}
				if(idEx->tipoInst == 'J' || idEx->tipoInst == 'j'){
					if(saltoJump == NULL){
						printf("ERROR: La etiqueta o dirección del %s en la linea %d no existe\n",idEx->instruccion, idEx->nInstruccion)
						break;
					}
				}
			//ETAPA 1
			v1 = IF(ifId, aux);
			while(ifId->tipoInst == 'E'){
				if(aux != NULL){
					aux = aux->siguiente;
				}
				v1 = IF(ifId,aux);
			}
			else if(ifId->tipoInst == 'N'){
				printf("ERROR: Existe un error de sintaxis en la linea %d\n",ifId->nInstruccion/4);
				break;
			}
			//Se decide cual será la instrucción para el siguiente ciclo, dando al salto realizado por la instrucción que entró antes
			if(saltoBranch != NULL){
				aux = saltoBranch;
				//En caso de existir un salto por branch se produce un hazard de control, y al implementar un branch never taken
				//se deben agregrar dos NOP a las siguientes dos instrucciones.
				//Para ello se pone un 'V' al tipoInst de los buffer idEx e ifId
				idEx->tipoInst = 'V';
				ifId->tipoInst = 'V';
				fprintf(fp,"%d- Hazard de control linea de instruccion %d, CC %d.\n",numeroHazards,(exMem->nInstruccion/4)+1,ciclos);
				fprintf(deteccion,"%d- Solucionable a través del uso de 2 NOP\n", numeroHazards);
				numeroHazards++;
			}
			else if(saltoJump != NULL){
				aux = saltoJump;
				//En caso de existir un salto por jump se produce un hazard de control
				//se debe agregrar un NOP a la siguiente instrucción al jump
				//Para ello se pone un 'V' al tipoInst del buffer ifId
				ifId->tipoInst = 'V';
				fprintf(fp,"%d- Hazard de control linea de instruccion %d, CC %d.\n",numeroHazards,(idEx->nInstruccion/4)+1,ciclos);
				fprintf(deteccion,"%d- Solucionable a través del uso de 1 NOP\n", numeroHazards);
				numeroHazards++;
			}
			else if(aux != NULL){
				aux = aux->siguiente;
			}
		}
		else{
			exMem->hazardLw = 2;
		}
		ciclos++;
	}while(v1 == 1 || v2 == 1 || v3 == 1 || v4 == 1 || v5 == 1);//Si en ninguna etapa se ejecutó algo, se termina el programa.
	fclose(fp);
	fclose(deteccion);
	fp = fopen("REGISTROS.txt","w");
	for(i = 0; i<32; i++){
		fprintf(fp,"%s = %d",nombresRegistros[i], Registros[i]);
	}
	fclose(fp);
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


//Función que simula la etapa IF
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
			strcmp("bne",instruccion[0])==0 || strcmp("sw",instruccion[0]) || strcmp("lw",instruccion[0])){
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

//Función que simula la etapa ID
Nodo * ID(ID_EX idEx, IF_ID ifId, Nodo * inicio){
	Nodo * salto;
	if(ifId->tipoInst == 'V'){
		idEx->tipoInst = ifId->tipoInst;
		return NULL;
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
		idEx->dato2 = Registros[idEx->registroRt];
	}
	else if(ifId->tipoInst == 'J'){
		if(stringToInt(ifId->instruccionCompleta[1]) != -1){
			salto = encontraEtiqueta(inicio,NULL, stringToInt(ifId->instruccionCompleta[1]));
		}
		else{
			salto = encontraEtiqueta(inicio, ifId->instruccionCompleta[1], 0);
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
		salto = encontraEtiqueta(inicio, NULL, Registros[idEx->registroRs]);
	}
	idEx->nInstruccion = ifId->nInstruccion;
	idEx->instruccion = ifId->instruccionCompleta[0];
	idEx->tipoInst = ifId->tipoInst;
	if(ifId->tipoInst == 'J' || ifId->tipoInst == 'j'){
		return salto;
	}
	else{
		return NULL;
	}
}


//Función que simula la etapa EX
Nodo * EX(EX_MEM exMem, ID_EX idEx, Nodo * inicio, MEM_WB memWb){
	exMem->salto = 0;
	Nodo * salto;
	if(idEx->tipoInst == 'V'){
		exMem->tipoInst = idEx->tipoInst;
		return NULL;
	}
	exMem->writeData = 0;
	if(idEx->tipoInst == 'R'){
		//FORWARDING
		if(exMem->writeData != 0 && exMem->tipoInst != 'V'){
			if(strcmp(exMem->instruccion,"lw") != 0){
				if(exMem->writeData == idEx->registroRs){
					idEx->dato1 = exMem->resultadoALU;
					fprintf(fp,"%d- Hazard de datos linea de instruccion %d, CC %d.\n",numeroHazards,(idEx->nInstruccion/4)+1,ciclos);
					fprintf(deteccion,"%d- Solucionable a través de Fowarding EX/MEM a ID/EX\n", numeroHazards);
					numeroHazards++;
				}
				else if(exMem->writeData == idEx->registroRt){
					idEx->dato2 = exMem->resultadoALU;
					fprintf(fp,"%d- Hazard de datos linea de instruccion %d, CC %d.\n",numeroHazards,(idEx->nInstruccion/4)+1,ciclos);
					fprintf(deteccion,"%d- Solucionable a través de Fowarding EX/MEM a ID/EX\n", numeroHazards);
					numeroHazards++;
				}
			}
			else{
				if(exMem->writeData == idEx->registroRs){
					exMem->tipoInst == 'V';
					exMem->hazardLw = 1;
					fprintf(fp,"%d- Hazard de datos linea de instruccion %d, CC %d.\n",numeroHazards,(idEx->nInstruccion/4)+1,ciclos);
					fprintf(deteccion,"%d- Solucionable a través de un NOP y Fowarding MEM/WB a ID/EX\n", numeroHazards);
					numeroHazards++;
					return NULL;
				}
				else if(exMem->writeData == idEx->registroRt){
					exMem->tipoInst == 'V';
					exMem->hazardLw = 1;
					fprintf(fp,"%d- Hazard de datos linea de instruccion %d, CC %d.\n",numeroHazards,(idEx->nInstruccion/4)+1,ciclos);
					fprintf(deteccion,"%d- Solucionable a través de un NOP y Fowarding MEM/WB a ID/EX\n", numeroHazards);
					numeroHazards++;
					return NULL;
				}
			}
		}
		else if(memWb->writeData != 0 && exMem->tipoInst != 'V'){
			if(memWb->writeData == idEx->registroRs && strcmp(exMem->instruccion,"lw") == 0){
				idEx->dato1 = memWb->datoMemoria;
				if(exMem->hazardLw != 2){
					fprintf(fp,"%d- Hazard de datos linea de instruccion %d, CC %d.\n",numeroHazards,(idEx->nInstruccion/4)+1,ciclos);
					fprintf(deteccion,"%d- Solucionable a través de Fowarding MEM/WB a ID/EX\n", numeroHazards);
					numeroHazards++;
				}
				else{
					exMem->hazardLw = 0;
				}
			}
			else if(memWb->writeData == idEx->registroRs){
				idEx->dato1 = memWb->resultadoALU;
				fprintf(fp,"%d- Hazard de datos linea de instruccion %d, CC %d.\n",numeroHazards,(idEx->nInstruccion/4)+1,ciclos);
				fprintf(deteccion,"%d- Solucionable a través de Fowarding MEM/WB a ID/EX\n", numeroHazards);
				numeroHazards++;
			}
			else if(memWb->writeData == idEx->registroRt && strcmp(exMem->instruccion,"lw") == 0){
				idEx->dato2 = memWb->datoMemoria;
				if(exMem->hazardLw != 2){
					fprintf(fp,"%d- Hazard de datos linea de instruccion %d, CC %d.\n",numeroHazards,(idEx->nInstruccion/4)+1,ciclos);
					fprintf(deteccion,"%d- Solucionable a través de Fowarding MEM/WB a ID/EX\n", numeroHazards);
					numeroHazards++;
				}
				else{
					exMem->hazardLw = 0;
				}
			}
			else if(memWb->writeData == idEx->registroRt){
				idEx->dato2 = memWb->resultadoALU;
				fprintf(fp,"%d- Hazard de datos linea de instruccion %d, CC %d.\n",numeroHazards,(idEx->nInstruccion/4)+1,ciclos);
				fprintf(deteccion,"%d- Solucionable a través de Fowarding MEM/WB a ID/EX\n", numeroHazards);
				numeroHazards++;
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
		if(exMem->writeData != 0 && exMem->tipoInst != 'V'){
			if(strcmp(exMem->instruccion,"lw") != 0){
				if(exMem->writeData == idEx->registroRs){
					idEx->dato1 = exMem->resultadoALU;
					fprintf(fp,"%d- Hazard de datos linea de instruccion %d, CC %d.\n",numeroHazards,(idEx->nInstruccion/4)+1,ciclos);
					fprintf(deteccion,"%d- Solucionable a través de Fowarding EX/MEM a ID/EX\n", numeroHazards);
					numeroHazards++;
				}
				else if(exMem->writeData == idEx->registroRt){
					idEx->dato2 = exMem->resultadoALU;
					fprintf(fp,"%d- Hazard de datos linea de instruccion %d, CC %d.\n",numeroHazards,(idEx->nInstruccion/4)+1,ciclos);
					fprintf(deteccion,"%d- Solucionable a través de Fowarding EX/MEM a ID/EX\n", numeroHazards);
					numeroHazards++;
				}
			}
			else{
				if(exMem->writeData == idEx->registroRs){
					exMem->tipoInst == 'V';
					exMem->hazardLw = 1;
					fprintf(fp,"%d- Hazard de datos linea de instruccion %d, CC %d.\n",numeroHazards,(idEx->nInstruccion/4)+1,ciclos);
					fprintf(deteccion,"%d- Solucionable a través de un NOP y Fowarding MEM/WB a ID/EX\n", numeroHazards);
					numeroHazards++;
					return NULL;
				}
				else if(exMem->writeData == idEx->registroRt){
					exMem->tipoInst == 'V';
					exMem->hazardLw = 1;
					fprintf(fp,"%d- Hazard de datos linea de instruccion %d, CC %d.\n",numeroHazards,(idEx->nInstruccion/4)+1,ciclos);
					fprintf(deteccion,"%d- Solucionable a través de un NOP y Fowarding MEM/WB a ID/EX\n", numeroHazards);
					numeroHazards++;
					return NULL;
				}
			}
		}
		else if(memWb->writeData != 0 && exMem->tipoInst != 'V'){
			if(memWb->writeData == idEx->registroRs && strcmp(exMem->instruccion,"lw") == 0){
				idEx->dato1 = memWb->datoMemoria;
				if(exMem->hazardLw != 2){
					fprintf(fp,"%d- Hazard de datos linea de instruccion %d, CC %d.\n",numeroHazards,(idEx->nInstruccion/4)+1,ciclos);
					fprintf(deteccion,"%d- Solucionable a través de Fowarding MEM/WB a ID/EX", numeroHazards);
					numeroHazards++;
				}
				else{
					exMem->hazardLw = 0;
				}
			}
			else if(memWb->writeData == idEx->registroRs){
				idEx->dato1 = memWb->resultadoALU;
				fprintf(fp,"%d- Hazard de datos linea de instruccion %d, CC %d.\n",numeroHazards,(idEx->nInstruccion/4)+1,ciclos);
				fprintf(deteccion,"%d- Solucionable a través de Fowarding MEM/WB a ID/EX", numeroHazards);
				numeroHazards++;
			}
			else if(memWb->writeData == idEx->registroRt && strcmp(exMem->instruccion,"lw") == 0){
				idEx->dato2 = memWb->datoMemoria;
				if(exMem->hazardLw != 2){
					fprintf(fp,"%d- Hazard de datos linea de instruccion %d, CC %d.\n",numeroHazards,(idEx->nInstruccion/4)+1,ciclos);
					fprintf(deteccion,"%d- Solucionable a través de Fowarding MEM/WB a ID/EX", numeroHazards);
					numeroHazards++;
				}
				else{
					exMem->hazardLw = 0;
				}
			}
			else if(memWb->writeData == idEx->registroRt){
				idEx->dato2 = memWb->resultadoALU;
				fprintf(fp,"%d- Hazard de datos linea de instruccion %d, CC %d.\n",numeroHazards,(idEx->nInstruccion/4)+1,ciclos);
				fprintf(deteccion,"%d- Solucionable a través de Fowarding MEM/WB a ID/EX", numeroHazards);
				numeroHazards++;
			}
		}
		//Ejecución de operación
		if(strcmp(idEx->instruccion,"sw") == 0){
			exMem->resultadoALU = idEx->dato1+stringToInt(exMem->inmediato);
			exMem->dato2 = idEx->dato2;
		}
		else if(strcmp(idEx->instruccion,"lw") == 0){
			exMem->resultadoALU = idEx->dato1+stringToInt(exMem->inmediato);
			exMem->writeData = idEx->registroRt;
		}
		else if(strcmp(idEx->instruccion,"addi")==0 || strcmp(idEx->instruccion,"addiu")==0){
			exMem->resultadoALU = idEx->dato1+stringToInt(exMem->inmediato);
			exMem->writeData = idEx->registroRt;
		}
		else if(strcmp(idEx->instruccion,"subi")==0){
			exMem->resultadoALU = idEx->dato1-stringToInt(exMem->inmediato);
			exMem->writeData = idEx->registroRt;
		}
		else if(strcmp(idEx->instruccion,"beq")==0){
			if(idEx->dato1 == idEx->dato2){
				exMem->salto = 1;
			}
		}
		else if(strcmp(idEx->instruccion,"bne")==0){
			if(idEx->dato1 != idEx->dato2){
				exMem->salto = 1;
			}
		}
		else if(strcmp(idEx->instruccion,"bgt")==0){
			if(idEx->dato1 > idEx->dato2){
				exMem->salto = 1;
			}
		}
		else if(strcmp(idEx->instruccion,"blt")==0){
			if(idEx->dato1 < idEx->dato2){
				exMem->salto = 1;
			}
		}
		if(exMem->salto){
			salto = encontraEtiqueta(inicio, idEx->inmediato, 0);
		}
	}
	else if(strcmp(idEx->instruccion,"jal")){
		exMem->writeData = idEx->registroRd;
		exMem->resultadoALU = idEx->dato1;
	}
	exMem->tipoInst = idEx->tipoInst;
	exMem->instruccion = idEx->instruccion;
	exMem->nInstruccion = idEx->nInstruccion;
	if(exMem->salto){
		return salto;
	}
	else{
		return NULL;
	}
}

int MEM(MEM_WB memWb, EX_MEM exMem){
	if(exMem->tipoInst == 'V'){
		memWb->tipoInst = exMem->tipoInst;
		return 0;
	}
	memWb->wirteData = 0;
	if(exMem->tipoInst == 'R' || strcmp(exMem->instruccion,"addi") == 0 || strcmp(exMem->instruccion,"subi") == 0 || strcmp(exMem->instruccion,"addiu") == 0
		|| strcmp(exMem->instruccion,"jal") == 0){
		memWb->resultadoALU = exMem->resultadoALU;
		memWb->writeData = exMem->writeData;
	}
	else if(strcmp(exMem->instruccion,"sw") == 0){
		Memoria[exMem->resultadoALU] = exMem->dato2;
	}
	else if(strcmp(exMem->instruccion,"lw") == 0){
		memWb->datoMemoria = Memoria[exMem->resultadoALU];
		memWb->writeData = exMem->writeData;
	}
	memWb->tipoInst = exMem->tipoInst;
	memWb->instruccion = exMem->instruccion;
	memWb->nInstruccion = exMem->nInstruccion;
	return 1;
}

int WB(MEM_WB memWb){
	if(memWb->tipoInst == 'V'){
		return 0;
	}
	if(memWb->tipoInst == 'R' || strcmp(memWb->instruccion,"addi") == 0 || strcmp(memWb->instruccion,"subi") == 0 || strcmp(memWb->instruccion,"addiu") == 0
		|| strcmp(memWb->instruccion,"jal") == 0){
		Registros[memWb->writeData] = memWb->resultadoALU;
		memWb->writeData = exMem->writeData;
	}
	else if(strcmp(memWb->instruccion,"lw") == 0){
		Registros[memWb->writeData] = memWb->datoMemoria;
	}
	return 1;
}
