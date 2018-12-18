char* nombresRegistros[32] = {"$zero", "$at", "$v0", "$v1", "$a0", "$a1", "$a2", "$a3", "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", 
	"$t7", "$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7", "$t8", "$t9", "$k0", "$k1", "$gp", "$sp", "$fp", "$ra"};
	
int Registros[32] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

//Función que particiona una cadena de caracteres por espacios.
Nodo * descomponer(char * cadena, Nodo * nodo){
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
		if(particion[strlen(particion)] == '\0'){
			return nodo->siguiente;
		}
		else{
			nodo->siguiente = (Nodo *)malloc(sizeof(Nodo));
			particion = strtok(NULL; "\0");
			return descomponer(particion, nodo->siguiente);
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
	if(strcmp(split[0],"sw") == 0 || strcmp(split[0],"lw") == 0){
		particion = strtok(split[2], "(");
		strcpy(split[2], particion);
		particion = strtok(NULL, ")");
		strcpy(split[3], particion);
	}
	nodo->instruccion = split;
	return nodo->siguiente;
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
		aux = descomponer(linea, inicio);
	}
	while(!feof(fp)){
		fgets(linea,256,fp);
		aux = (Nodo *)malloc(sizeof(Nodo));
		aux = descomponer(linea, aux);
	}
	fclose(fp);
	return inicio;
}

void ejecucion(Nodo * inicial){
	Nodo * aux = inicial;
	IF_ID = (IF_ID *)malloc(sizeof(IF_ID));
	ID_EX = (ID_EX *)malloc(sizeof(ID_EX));
	EX_MEM = (EX_MEM *)malloc(sizeof(EX_MEM));
	MEM_WB = (MEM_WB *)malloc(sizeof(MEM_WB));
	//variables que verifican que alguna etapa del pipeline se haya ejecutado
	int v1 = 0;
	int v2 = 0;
	int v3 = 0;
	int v4 = 0;
	int v5 = 0;
	do{
		v1 = IF();
		v2 = ID();
		v3 = EX();
		v4 = MEM();
		v5 = WB();
		if(aux != NULL){
			aux = aux->siguiente;
		}
	}while(v1 == 1 || v2 == 1 || v3 == 1 || v4 == 1 || v5 == 1);
}
