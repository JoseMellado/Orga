//Función que borra espacios innecesarios en una cadena, para así facilitar el split de este.
//Al usar esta función todas las cadenas quedarán con el formato: "palabra1 palabra2 ... palabraN"
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
