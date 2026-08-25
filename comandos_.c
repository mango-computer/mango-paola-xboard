/* 
	Mango AC Ajedrez 1.0
	Licencia: GPLv3
	Copyright (c) 2012-2013 - Mango Computer c.a

	El programa Mango AC Ajedrez está basado en la investigación de Jose Andres Morales Linares.

	Nadie debería verse restringido por el software que utiliza. Hay cuatro libertades que cada usuario debe tener:
	* La libertad de usar el programa para cualquier propósito.
	* La libertad de cambiar el software para satisfacer sus necesidades.
	* La libertad de compartir el software con amigos y vecinos.
	* La libertad de compartir los cambios que realice.

	Cuando un programa ofrece a los usuarios todas estas libertades, lo llamamos software libre.

	Winglet es un motor de ajedrez de código abierto basado en bitboards. El programa es software libre. Usted puede redistribuirlo y/o modificarlo bajo los términos de la Licencia Pública General de GNU publicada por la Free Software Foundation, ya sea la versión 3 de la Licencia o (a su elección) cualquier versión posterior. El programa se distribuye con la esperanza de que sea útil, pero SIN NINGUNA GARANTÍA, incluso sin la garantía implícita de COMERCIALIZACIÓN o IDONEIDAD PARA UN PROPÓSITO PARTICULAR. Consulte la Licencia Pública General de GNU para más detalles: http://www.gnu.org/licenses/

	Existen muchos avances en la forma de escribir motores de ajedrez. Estos dos sitios web
	fueron de gran ayuda durante el proceso de investigación y desarrollo:
	* http://chessprogramming.wikispaces.com
	* http://www.sluijten.com/winglet/

	Información de contacto:
	comprasmangocomputer@gmail.com

*/

#ifndef COMANDOS_C
#define COMANDOS_C

void leerComandos()
{
	int c=0;
	setbuf(stdout, NULL);

	printf("Blancas> ");

	while ( (c=getc(stdin)) != EOF  )
	{
		if (c=='\n')
		{
			BUFF_COMANDOS[CONT_BUFF_COMANDOS] = '\0';

			while (CONT_BUFF_COMANDOS)
			{
				if (!ejecutarComando(BUFF_COMANDOS)) return;
			}

			if (juego.colorTurno)
			{		
				printf("Negras> ");
			} else {
				printf("Blancas> ");
			}
			fflush(stdout);

		} else {
			if (CONT_BUFF_COMANDOS>=LARGO_BUFER_COMANDOS)
			{
				printf("El comando es demasiado largo\n");
				CONT_BUFF_COMANDOS = 0;
			}
			
			BUFF_COMANDOS[CONT_BUFF_COMANDOS++] = c;
		}

	}

}

BOOLEANO ejecutarComando(const char *buff)
{
	BOOLEANO esJaque;
	uint64 tiempo_ms  = 0;  
	MOVIMIENTO m;
	int profundidadSugerida;

	if (!strcmp(buff, ""))
	{
		CONT_BUFF_COMANDOS = '\0';
		return VERDADERO;    
	}

	if ((!strcmp(buff, "help")) || (!strcmp(buff, "h")) || (!strcmp(buff, "?")) || (!strcmp(buff, "ayuda")))
	{

		printf("\n******* Ayuda *******\n");
		printf("ata n			: Muestra un mapa de bits de todos los atacantes del escaque indicado\n");
		printf("blanco			: Juegan las blancas\n");
		printf("cc			: Partida de computadora contra computadora\n");
		printf("configuracion		: Configura el tablero\n");
		printf("dale			: La computadora realiza su jugada\n");
		printf("d			: Muestra el tablero\n");
		printf("eval			: Muestra la evaluación estática de esta posición\n");
		printf("g			: Gira el tablero\n");
		printf("i			: Muestra el valor de las variables\n");
		printf("info			: Muestra el valor de las variables\n");
		printf("juego			: Muestra los movimientos de la partida\n");
		printf("leerfen nombreArchivo n	: Lee la línea n de un archivo FEN\n");
		printf("librono			: Desactiva el libro de aperturas\n");
		printf("librosi			: Activa el libro de aperturas\n");
		printf("movimientos		: Muestra todos los movimientos legales\n");
		printf("negro			: Juegan las negras\n");
		printf("nuevo			: Inicia una nueva partida\n");
		printf("regresar		: Deshace el último movimiento\n");
		printf("r			: Gira el tablero\n");
		printf("sd n			: Establece la profundidad de búsqueda en n*2\n");
		printf("t			: Muestra el tablero\n");
		printf("u			: Deshace el último movimiento\n");
		printf("ver blancos		: Muestra el mapa de bits de las piezas blancas\n");
		printf("ver destinos blancos	: Muestra el mapa de bits de los destinos blancos\n");
		printf("ver destinos negros	: Muestra el mapa de bits de los destinos negros\n");
		printf("ver			: Muestra la versión y el autor del motor de ajedrez\n");
		printf("ver negros		: Muestra el mapa de bits de las piezas negras\n");
		printf("ver ocupados		: Muestra el mapa de bits de las casillas ocupadas\n");
		printf("ver peones blancos	: Muestra el mapa de bits de los peones blancos\n");
		printf("ver peones negros	: Muestra el mapa de bits de los peones negros\n");
		printf("ver todos mov		: Muestra todos los movimientos generados, sin validar\n");

		printf("\n******* Help *******\n");
		printf("ata n			: Muestra un mapa de bits de todos los atacantes del escaque indicado\n");
		printf("black			: Juegan las negras\n");
		printf("cc			: Partida de computadora contra computadora\n");
		printf("d			: Muestra el tablero\n");
		printf("eval			: Muestra la evaluación estática de esta posición\n");
		printf("exit			: Sale de la partida\n");
		printf("game			: Muestra los movimientos de la partida\n");
		printf("g			: Gira el tablero\n");
		printf("go			: La computadora realiza su jugada\n");
		printf("i			: Muestra el valor de las variables\n");
		printf("info			: Muestra el valor de las variables\n");
		printf("move e2e4		: Introduce un movimiento (use este formato)\n");
		printf("moves			: Muestra todos los movimientos legales\n");
		printf("new			: Inicia una nueva partida\n");
		printf("perft n			: Calcula el número de nodos brutos a profundidad n\n");
		printf("perft			: Ejecuta una prueba de rendimiento de funciones clave\n");
		printf("quit			: Sale del programa\n");
		printf("readfen nombreArchivo n	: Lee la línea n de un archivo FEN\n");
		printf("r			: Gira el tablero\n");
		printf("sd n			: Establece la profundidad de búsqueda en n*2\n");
		printf("setup			: Configura el tablero\n");
		printf("t			: Muestra el tablero\n");
		printf("u			: Deshace el último movimiento\n");
		printf("undo			: Deshace el último movimiento\n");
		printf("white			: Juegan las blancas\n");


		CONT_BUFF_COMANDOS = '\0';
		return VERDADERO;    
       }

	if (buff[0] == 'p' && buff[1] == 'e' && buff[2] == 'r' && buff[3] == 'f' && buff[4] == 't' && isdigit(buff[6]))  
	{
		uint64 resulPerft = 0;
		tiempo_ms  = 0;  
		uint8  profundidad = buff[6] - 48;
		printf("Iniciando la prueba a profundidad %d\n", profundidad);
		printf("...\n");

		DataPerft.InvCaptura		= 0;
		DataPerft.InvPeonPaso		= 0;
		DataPerft.InvPromocion		= 0;	
		DataPerft.InvEnroqueOO		= 0;
		DataPerft.InvEnroqueOOO		= 0;
		DataPerft.InvJaqueContrario	= 0;

		prepararTiempo();
		iniciarTiempo();
		resulPerft = perft(0,profundidad);
		detenerTiempo();
		tiempo_ms  = obt_msTiempo();
		printf("Nodos %lld, en %lld ms\n",resulPerft,tiempo_ms);

		printf("\n");
		printf("Capturas	: %u\n",DataPerft.InvCaptura);
		printf("Peón al paso	: %u\n",DataPerft.InvPeonPaso);
		printf("Promoción	: %u\n",DataPerft.InvPromocion);
		printf("OO		: %u\n",DataPerft.InvEnroqueOO);
		printf("OOO		: %u\n",DataPerft.InvEnroqueOOO);
		printf("Total Enroque	: %u\n",(DataPerft.InvEnroqueOOO+DataPerft.InvEnroqueOO));
		printf("Jaque enemigo	: %u\n",DataPerft.InvJaqueContrario);
		printf("\n");

		if (tiempo_ms > 0)
		{
			printf("Relación: %lld knodos/s\n", resulPerft/tiempo_ms);
		}
		CONT_BUFF_COMANDOS = '\0';
		return VERDADERO;    
	}




	if (buff[0] == 'm' && buff[1] == 'o' && buff[2] == 'v' && buff[3] == 'e' && buff[4] == ' ')  
	{
		CONT_BUFF_COMANDOS = '\0';
		MOVIMIENTO mov;
		mov ^= mov;
		if (esValidoMovUsuario(buff, &mov))
		{
			hacerMovimiento(mov);

			if (juego.colorTurno) // Si le toca al negro, se comprueba que el rey blanco no haya quedado en jaque
			{
				esJaque = esAtacadoPor(juego.tablero[BLANCO][REY], NEGRO);
			} else {
				esJaque = esAtacadoPor(juego.tablero[NEGRO][REY], BLANCO);
			}
			
			if (!esJaque)
			{
				juego.Buffer_MOV_INDEXCAPAS[1] = generarTodosMov(0);
				mostrarTablero();
			} else {
				printf("Movimiento ilegal: el rey queda en jaque\n");
				desHacerMovimiento(mov);
			}
		} else {
			printf("Movimiento ilegal: escaque de origen o destino inválido\n");
		}

		return VERDADERO;    
	}

	if ((!strncmp(buff, "sd", 2)))
	{
		CONT_BUFF_COMANDOS = '\0';
		sscanf(buff,"sd %d", &profundidadSugerida);
		if (profundidadSugerida < 2) profundidadSugerida = 2;
		juego.profundidadBusquedad = profundidadSugerida;
		nivelPC = PC_NIVEL_PERSONALIZADO;
		return VERDADERO;    
	}

	if (buff[0] == 'l' && buff[1] == 'e' && buff[2] == 'e' && buff[3] == 'r' && buff[4] == 'f' && buff[5] == 'e' && buff[6] == 'n')
	{
		CONT_BUFF_COMANDOS = '\0';
		char nombreArchivo[80];
		int numero=0;
		sscanf(buff,"leerfen %s %d", nombreArchivo, &numero);
		readFen(nombreArchivo, numero);
		mostrarTablero();
		return VERDADERO;    
	}


	if (buff[0] == 'r' && buff[1] == 'e' && buff[2] == 'a' && buff[3] == 'd' && buff[4] == 'f' && buff[5] == 'e' && buff[6] == 'n')
	{
		CONT_BUFF_COMANDOS = '\0';
		char nombreArchivo[80];
		int numero=0;
		sscanf(buff,"readfen %s %d", nombreArchivo, &numero);
		readFen(nombreArchivo, numero);
		mostrarTablero();
		return VERDADERO;    
	}

	if ((!strcmp(buff, "undo")) || (!strcmp(buff, "regresar")) || (!strcmp(buff, "u")))
	{
		CONT_BUFF_COMANDOS = '\0';
		if (juego.indiceHJuego > 0)
		{
			desHacerMovimiento(juego.historicoJuego[juego.indiceHJuego-1].mov);
			juego.Buffer_MOV_INDEXCAPAS[1] = generarTodosMov(0);
			mostrarTablero();
		}
		return VERDADERO;    
	}

	if ((!strcmp(buff, "eval")))
	{
		CONT_BUFF_COMANDOS = '\0';
		uint32 resulEval = evaluacionTablero(-INFINITO,INFINITO);
		printf("Evaluación estática del tablero actual\nEval:	%d\n",resulEval);
		return VERDADERO;    
	}

	if ((!strcmp(buff, "facil")))
	{
		CONT_BUFF_COMANDOS = '\0';
		juego.maxTiempo 		= 3000;
		juego.profundidadBusquedad	= 98;
		nivelPC = PC_NIVEL_FACIL;
		return VERDADERO;    
	}

	if ((!strcmp(buff, "medio")))
	{
		CONT_BUFF_COMANDOS = '\0';
		juego.maxTiempo 		= 7000;
		juego.profundidadBusquedad	= 98;
		nivelPC = PC_NIVEL_MEDIO;
		return VERDADERO;    
	}

	if ((!strcmp(buff, "fuerte")))
	{
		CONT_BUFF_COMANDOS = '\0';
		juego.maxTiempo 		= 12000;
		juego.profundidadBusquedad	= 98;
		nivelPC = PC_NIVEL_FUERTE;
		return VERDADERO;    
	}


	if ((!strncmp(buff, "time", 4)))
	{
		CONT_BUFF_COMANDOS = '\0';
		int tiempoUser = juego.maxTiempo/1000;
		sscanf(buff,"time %d", &tiempoUser);
		if (tiempoUser < 1) tiempoUser = 1;
		juego.maxTiempo = tiempoUser * 1000;
		printf("juego.maxTiempo = %d\n",juego.maxTiempo);
		nivelPC = PC_NIVEL_PERSONALIZADO;
		return VERDADERO;    
	}

	if ((!strcmp(buff, "new")) || (!strcmp(buff, "nuevo")))
	{
		CONT_BUFF_COMANDOS = '\0';
		nuevo_juego();
		printf("Motor reiniciado\n");
		return VERDADERO;    
	}

	if ((!strcmp(buff, "white")) || (!strcmp(buff, "blanco")))
	{
		CONT_BUFF_COMANDOS = '\0';
		if (juego.colorTurno)
		{
			juego.colorTurno = BLANCO;
			juego.Buffer_MOV_INDEXCAPAS[1] = generarTodosMov(0);
		}
		return VERDADERO;    
	}

	if ((!strcmp(buff, "black")) || (!strcmp(buff, "negro")))
	{
		CONT_BUFF_COMANDOS = '\0';
		if (!juego.colorTurno)
		{
			juego.colorTurno = NEGRO;
			juego.Buffer_MOV_INDEXCAPAS[1] = generarTodosMov(0);
		}
		return VERDADERO;    
	}


	if ((!strcmp(buff, "info")) || (!strcmp(buff, "i")))
	{
		CONT_BUFF_COMANDOS = '\0';
		informacionVariables();
		return VERDADERO;    
	}

	if ((!strcmp(buff, "r")) || (!strcmp(buff, "g")))
	{
		CONT_BUFF_COMANDOS = '\0';
		juego.vista = !juego.vista;
		mostrarTablero();
		return VERDADERO;    
	}



	if ((!strcmp(buff, "d")) || (!strcmp(buff, "t")))
	{
		CONT_BUFF_COMANDOS = '\0';
		mostrarTablero();
		return VERDADERO;    
	}

	if ((!strcmp(buff, "game")) || (!strcmp(buff, "juego")))
	{
		CONT_BUFF_COMANDOS = '\0';
		verMovimentosJuego();
		return VERDADERO;    
	}


	if ((!strcmp(buff, "moves")) || (!strcmp(buff, "movimientos")))
	{
		CONT_BUFF_COMANDOS = '\0';
		verMovimentosLegales();
		return VERDADERO;    
	}

	if ((!strcmp(buff, "genmov")))
	{
		CONT_BUFF_COMANDOS = '\0';
		juego.Buffer_MOV_INDEXCAPAS[1] = generarTodosMov(0);
		verMovimentosLegales();
		return VERDADERO;    
	}

	if ((!strcmp(buff, "genmovcap")))
	{
		CONT_BUFF_COMANDOS = '\0';
		juego.Buffer_MOV_INDEXCAPAS[1] = generarMovCapPro(0);
		verMovimentosLegales();
		juego.Buffer_MOV_INDEXCAPAS[1] = generarTodosMov(0);
		return VERDADERO;    
	}

	if ((!strcmp(buff, "eetpos")))
	{
		CONT_BUFF_COMANDOS = '\0';
		int ponderacion=0,i=0;
		for(i=0;i<64;i++)
		{
			
			printf("eetpos pos=%d valor=%d\n",i, EETPOS(i, juego.colorTurno));
			ponderacion += EETPOS(i,  juego.colorTurno);
		}
		printf("ponderacion=%d\n",ponderacion);
		return VERDADERO;    
	}

	if ((!strcmp(buff, "genmovcapt")))
	{
		CONT_BUFF_COMANDOS = '\0';
		juego.Buffer_MOV_INDEXCAPAS[1] = generarMovCapPro(0);
		verMovimentosLegales();
		juego.Buffer_MOV_INDEXCAPAS[1] = generarTodosMov(0);
		return VERDADERO;    
	}

//*
	if ((!strncmp(buff, "test",4)))
	{
		CONT_BUFF_COMANDOS = '\0';
		char nombreArchivo[100];
		printf("Por favor, espere mientras se realiza la prueba...\n");
		FILE *fp = freopen("test.txt", "w",stdout);
		sscanf(buff,"test %s", nombreArchivo);

		printf("MODO PRUEBA en %s\n",nombreArchivo);

		int i=1;
		while(readFen(nombreArchivo, i))
		{
			printf("Iteración n.º %d\n",i);
			pensarRapido();
			i++;
		}	

		fclose(fp);
//		stdout = fdopen(2, "w");
		return VERDADERO;    
	}
//*/
	if (buff[0] == 'e' && buff[1] == 'e' && buff[2] == 't')  
	{
		CONT_BUFF_COMANDOS = '\0';
		int escaque=0;
		int ccolor=0;
		sscanf(buff,"eet %d %d", &escaque, &ccolor);
		if (escaque >= 0 && escaque < 64 )
		{
			printf("Escaque=%d EET=%d\n",escaque, EETPOS(escaque, ccolor));
		} else {
			printf("Escaque=%d fuera de rango (0-63)\n",escaque);
		}
		return VERDADERO;    
	}


	if (((!strcmp(buff, "go")) || (!strcmp(buff, "dale"))) && STATUS_FIN_JUEGO_NO_TERMINADO)
	{

		CONT_BUFF_COMANDOS = '\0';
		printf("Pensando... profundidad: %d\n", juego.profundidadBusquedad);
		esConsola = VERDADERO;
		m = pensarRapido();
		esConsola = FALSO;
		if (!m)
		{
			printf("\nNo se encontró ningún movimiento válido\n");
			return VERDADERO;    
		} 

		hacerMovimiento(m);

		juego.Buffer_MOV_INDEXCAPAS[1] = generarTodosMov(0);

		imprimirMovimiento(m);	
		mostrarTablero();
		statusFinJuego = obtEstadoJuego();
		mostrarMensajeFinalJuego();

		return VERDADERO;    
	}

	if (!strcmp(buff, "."))
	{

		CONT_BUFF_COMANDOS = '\0';
		printf("Pensando sin búsqueda de tranquilidad... profundidad: %d\n", juego.profundidadBusquedad);
		esVariable = VERDADERO;	
		m = pensarRapido();
		esVariable = FALSO;	

		if (!m)
		{
			printf("\nNo se encontró ningún movimiento válido\n");
			return VERDADERO;    
		} 

		hacerMovimiento(m);
		juego.Buffer_MOV_INDEXCAPAS[1] = generarTodosMov(0);
		imprimirMovimiento(m);	
		mostrarTablero();

		return VERDADERO;    
	}

	if ((!strcmp(buff, "xboard")))
	{
		CONT_BUFF_COMANDOS = '\0';
		xboard();
		return VERDADERO;    
	}


	if ((!strcmp(buff, "exit")) || (!strcmp(buff, "quit")) || (!strcmp(buff, "salir")))
	{
		CONT_BUFF_COMANDOS = '\0';
		cerrarLibro();
		cerrarTablas();
		return FALSO;    
	}

	if ((!strcmp(buff, "librosi")))
	{
		CONT_BUFF_COMANDOS = '\0';
		usarLibroAperturas = VERDADERO;
		return VERDADERO;    
	}

	if ((!strcmp(buff, "librono")))
	{
		CONT_BUFF_COMANDOS = '\0';
		usarLibroAperturas = FALSO;
		return VERDADERO;    
	}

	if ((!strcmp(buff, "ver")))
	{
		CONT_BUFF_COMANDOS = '\0';
		printf("%s\n",VERSION_MANGO_AJEDREZ);
		return VERDADERO;    
	}
	

	if ((!strcmp(buff, "cc")) && STATUS_FIN_JUEGO_NO_TERMINADO)
	{
		CONT_BUFF_COMANDOS = '\0';
		statusFinJuego = obtEstadoJuego();

		while (STATUS_FIN_JUEGO_NO_TERMINADO && (juego.indiceHJuego < MAX_BUFF_MOV))
		{
			tiempo_ms  = 0;  
			prepararTiempo();
			iniciarTiempo();
			m = pensarRapido();
			detenerTiempo();
			tiempo_ms  = obt_msTiempo();
			printf("Tiempo de proceso: %lld ms ",tiempo_ms);
			if (!m)
			{
				printf("\nNo se encontró ningún movimiento válido\n");
				break;
			} 
			hacerMovimiento(m);
			juego.Buffer_MOV_INDEXCAPAS[1] = generarTodosMov(0);
			imprimirMovimiento(m);	
			mostrarTablero();
			statusFinJuego = obtEstadoJuego();
		}

		mostrarMensajeFinalJuego();
		return VERDADERO;    
	}

	printf("Comando desconocido. Escriba \'ayuda\' para más información\n");
	CONT_BUFF_COMANDOS = '\0';
	return VERDADERO;    
}

void mostrarMensajeFinalJuego()
{
			if (statusFinJuego == STATUS_FIN_JUEGO_MATE_GANAN_B){

				printf("result 1-0 {White mates}\n");

			} else if (statusFinJuego == STATUS_FIN_JUEGO_MATE_GANAN_N){

				printf("result 0-1 {Black mates}\n");

			} else if (statusFinJuego == STATUS_FIN_JUEGO_TABLAS_REPETICION) {

				printf("result 1/2-1/2 {Draw by repetition}\n");

			} else if (statusFinJuego == STATUS_FIN_JUEGO_TABLAS_INSUFI_MATERIAL) {

				printf("result 1/2-1/2 {Draw by insufficient material}\n");
			}

}


void mostrarTablero()
{
	int rank, file;
	printf("\n");

	if (!juego.vista)
        {
		for (rank = 8; rank >= 1; rank--)
                {
			printf("    +---+---+---+---+---+---+---+---+\n");
			printf("  %d |",rank);
                        for (file = 1; file <= 8; file++)
                        {
				printf(" %s|",NOMBREPIEZA[ESCAQUES[FILERANK2POS[file][rank]]]);
                        }
			printf("\n"); 	  
		}
		printf("    +---+---+---+---+---+---+---+---+\n");
		printf("      a   b   c   d   e   f   g   h\n");
	} else {
		printf("      h   g   f   e   d   c   b   a\n");
		for (rank = 1; rank <= 8; rank++)
		{
			printf("    +---+---+---+---+---+---+---+---+\n");
			printf("    |");
                        for (file = 8; file >= 1; file--)
                        {
				printf(" %s|",NOMBREPIEZA[ESCAQUES[FILERANK2POS[file][rank]]]);
                        }
			printf(" %d \n",rank);
                 }
		printf("    +---+---+---+---+---+---+---+---+\n");
	}

	return;

}



void verMovimentosLegales()
{
	int i=0;
	BOOLEANO esJaque;	

	if (juego.Buffer_MOV_INDEXCAPAS[0] == juego.Buffer_MOV_INDEXCAPAS[1])
	{
		printf("No hay movimientos válidos\n");
		return;
	}

	for (i=juego.Buffer_MOV_INDEXCAPAS[0];i<juego.Buffer_MOV_INDEXCAPAS[1];i++)
	{

		hacerMovimiento(juego.Buffer_MOV[i]);

		if (juego.colorTurno) // Si le toca al negro, se comprueba que el rey blanco no haya quedado en jaque
		{
			esJaque = esAtacadoPor(juego.tablero[BLANCO][REY], NEGRO);
		} else {
			esJaque = esAtacadoPor(juego.tablero[NEGRO][REY], BLANCO);
		}

		desHacerMovimiento(juego.Buffer_MOV[i]);
		
		if (!esJaque)
		{
			verMov(juego.Buffer_MOV[i]);
		}
	}
}



void verMovimentosJuego()
{
	int i=0;

	for (;i< juego.indiceHJuego;i++)
	{
		printf("%c ", ((i % 2) ? 'N':'B'));
		printf("%s ", juego.historicoJuego[i].movAlg);
		printf("ori=%d des=%d pieza=%d cap=%d\n",OBT_MOV_ORIGEN(juego.historicoJuego[i].mov),
							OBT_MOV_DESTINO(juego.historicoJuego[i].mov),
							OBT_MOV_PIEZA(juego.historicoJuego[i].mov),
							OBT_MOV_CAPTURA(juego.historicoJuego[i].mov));
	}
}

void informacionVariables()
{
	printf("*** Información de las variables del juego ***\n\n");
	printf("Profundidad de búsqueda	(profundidadBusquedad)	: %d\n", juego.profundidadBusquedad);
	printf("Tiempo por movimiento		(juego.maxTiempo)	: %d ms\n", juego.maxTiempo);
	printf("Libro de Aperturas (SI/NO)	(usarLibroAperturas)	: %s\n", usarLibroAperturas?"Si":"No");
	printf("Nivel PC			(nivelPC)		: %s\n", NOMBRENIVELPC[nivelPC]);
	printf("Balance material		(material_total)	: %d\n", juego.material_total);
	printf("Material blanco			(material_lado_blanco)	: %d\n", juego.material_lado_blanco);
	printf("Material peones blanco		(material_peon_blanco)	: %d\n", juego.material_peon_blanco);
	printf("Material negro			(material_lado_negro)	: %d\n", juego.material_lado_negro);
	printf("Material peones negro		(material_peon_negro)	: %d\n", juego.material_peon_negro);
	printf("Bando que juega		(colorTurno)		: %s\n",(juego.colorTurno?"Negro":"Blanco"));
	printf("# Movimientos \"Regla 50 mov\"	(reglaCincuentaMov)	: %d\n",juego.reglaCincuentaMov);
	printf("# Total Movimientos 		(totalMov)		: %d\n",juego.totalMov);
	printf("Es posible OO  blanco		(OOB)			: %s\n",(juego.OOB?"Si":"No"));
	printf("Es posible OOO blanco		(OOOB)			: %s\n",(juego.OOOB?"Si":"No"));
	printf("Es posible OO  negro		(OON)			: %s\n",(juego.OON?"Si":"No"));
	printf("Es posible OOO negro		(OOON)			: %s\n",(juego.OOON?"Si":"No"));
	printf("Índice de la partida		(juego.indiceHJuego)	: %d\n",juego.indiceHJuego);
	if (juego.posPeonPaso==SIN_POS_VALIDA)
	{
		printf("Escaque \"peón al paso\"		(posPeonPaso)		: Sin una posición válida\n");
	} else {
		printf("Escaque \"peón al paso\"		(posPeonPaso)		: %d\n",juego.posPeonPaso);
	}
}


void verMov(MOVIMIENTO mov)
{
	printf("Ori=%d, Des=%d, Pieza=%d, Captura=%d, Promocion=%d, cod=%d, EET=%d\n", 
		OBT_MOV_ORIGEN(mov),  OBT_MOV_DESTINO(mov),   OBT_MOV_PIEZA(mov),
		OBT_MOV_CAPTURA(mov), OBT_MOV_PROMOCION(mov), OBT_MOV_CODIGO(mov), EET(mov));
	 
}

void verMov2(MOVIMIENTO mov)
{
	printf("%d-%d\n",OBT_MOV_ORIGEN(mov),OBT_MOV_DESTINO(mov));
 
}

void nuevo_juego()
{
//		memcpy(ESCAQUES, ESCAQUES_INI, sizeof(ESCAQUES));
		inicializarTablero();
		inicializarVar();
		iniTablerosUtil();
		actualizarTablerosUtil(BLANCO);
//		iniciarHash();
		iniHashTablero();
		juego.Buffer_MOV_INDEXCAPAS[1] = generarTodosMov(0);
}
//*

void xboard()
{
	char minutos[4];	                        
	int entrada 		= 0;                                
	int incremento		= 0;	                                
	int inc 		= 0;
	int mps 		= 0;
	BOOLEANO esPonder 	= FALSO;
	COLOR computadora 	= NEGRO;
	esPost 			= VERDADERO;

	char linea[256], buffComandos[256];
	MOVIMIENTO m;
	BOOLEANO esJaque;	

	printf("\n");

	nuevo_juego();
	computadora = NEGRO;

	for (;;) 
	{
		fflush(stdout);
		if (juego.colorTurno == computadora) {

			tipoDeBusqueda = TIPO_BUSQUEDA_NORMAL;
printf(">>>>>>><<<<<<<<<<\n");
			m = pensarRapido();
			if (!m) continue; 
			hacerMovimiento(m);
			printf("move "); imprimirMovimiento(m);printf("\n");
			juego.Buffer_MOV_INDEXCAPAS[1] = generarTodosMov(0);
			statusFinJuego = obtEstadoJuego();
			mostrarMensajeFinalJuego();
			continue;
		} else if (computadora != SIN_COLOR){

			if (esPonder && (juego.maxTiempo > 1000))
			{
				tipoDeBusqueda = TIPO_BUSQUEDA_PONDER;
				m = pensarRapido();
			}
			continue;
		}


		if (!fgets(linea, 256, stdin))
			return;
		if (linea[0] == '\n')
			continue;
		sscanf(linea, "%s", buffComandos);
		if (!strcmp(buffComandos, "xboard"))
			continue;
		if (!strcmp(buffComandos, "new")) {
			nuevo_juego();
			computadora = NEGRO;
			continue;
		}
		if (!strcmp(buffComandos, "quit"))
			return;
		if (!strcmp(buffComandos, "force")) {
			computadora = NEGRO;
			continue;
		}
		if (!strcmp(buffComandos, "white")) {
			juego.colorTurno = BLANCO;
			juego.Buffer_MOV_INDEXCAPAS[1] = generarTodosMov(0);
			computadora = NEGRO;
			continue;
		}
		if (!strcmp(buffComandos, "black")) {
			juego.colorTurno = NEGRO;
			juego.Buffer_MOV_INDEXCAPAS[1] = generarTodosMov(0);
			computadora = BLANCO;
			continue;
		}
		if (!strcmp(buffComandos, "st")) {
			sscanf(linea, "st %d", &juego.maxTiempo);
			juego.maxTiempo *= 1000;
			juego.profundidadBusquedad = MAX_CAPAS;
			continue;
		}
		if (!strcmp(buffComandos, "sd")) {
			sscanf(buffComandos, "sd %d", &juego.profundidadBusquedad);
			juego.maxTiempo = 1 << 25;
			continue;
		}

		if (!strcmp(buffComandos, "level")) {
			sscanf(linea, "level %d %s %d", &entrada, minutos, &incremento);
			mps = entrada;	    
			inc = incremento * 1000;
			continue;
		}

		if (!strcmp(buffComandos, "time")) {
			sscanf(linea, "time %d", &juego.maxTiempo);
			juego.maxTiempo *= 10;
			juego.maxTiempo /= 30;
			continue;
		}

		if (!strcmp(buffComandos, "otim")) {
			continue;
		}
		if (!strcmp(buffComandos, "go")) {
			computadora = juego.colorTurno;
			continue;
		}
		if (!strcmp(buffComandos, "protover")) {
			printf("feature setboard=0 analyze=0 ping=1 draw=0 sigint=0 sigterm=0"
				   " variants=\"normal\" myname=\"%s %s\" done=1\n",NOMBRE_MANGO_AC,VERSION_MANGO_AC);
			fflush(stdout);
			continue;
		}
		if (!strcmp(buffComandos, "hint")) {
			tipoDeBusqueda = TIPO_BUSQUEDA_NORMAL;
			m = pensarRapido();
			if (!m) continue; 
			printf("Hint: ");
			imprimirMovimiento(m);printf("\n");
			continue;
		}
		if (!strcmp(buffComandos, "undo")) {
			if (juego.indiceHJuego > 0)
			{
				desHacerMovimiento(juego.historicoJuego[juego.indiceHJuego-1].mov);
				juego.Buffer_MOV_INDEXCAPAS[1] = generarTodosMov(0);
			}
			continue;
		}
		if (!strcmp(buffComandos, "remove")) {
			if (juego.indiceHJuego > 1)
			{
				desHacerMovimiento(juego.historicoJuego[juego.indiceHJuego-1].mov);
				desHacerMovimiento(juego.historicoJuego[juego.indiceHJuego-1].mov);
				juego.Buffer_MOV_INDEXCAPAS[1] = generarTodosMov(0);
			}
			continue;
		}
		if (!strcmp(buffComandos, "force")) {
			computadora = SIN_COLOR;
			continue;
		}
		if (!strcmp(buffComandos, "result")) {
			computadora = SIN_COLOR;
			continue;
		}
		if (!strcmp(buffComandos, "?")) {
			computadora = SIN_COLOR;
			continue;
		}
		if (!strcmp(buffComandos, "post")) {
			esPost = VERDADERO;
			continue;
		}
		if (!strcmp(buffComandos, "nopost")) {
			esPost = FALSO;
			continue;
		}
		if (!strcmp(buffComandos, "ping")) {
			int n;
			sscanf(linea, "ping %d", &n);
			printf("pong %d\n",n);
			continue;
		}
		if (!strcmp(buffComandos, "computer")) {
			continue;
		}
		if (!strcmp(buffComandos, "easy")) {
			esPonder = FALSO;
			continue;
		}
		if (!strcmp(buffComandos, "hard")) {
			esPonder = VERDADERO;
			continue;
		}
		if (!strcmp(buffComandos, "name")) {
			continue;
		}
		if (!strcmp(buffComandos, "bk")) {
			continue;
		}
		if (!strcmp(buffComandos, "edit")) {
			continue;
		}
		if (!strcmp(buffComandos, ".")) {
			continue;
		}

		if (!esUnMovimiento(buffComandos))
		{
			printf("Error (unknown command): %s\n", buffComandos);
			continue;
		}

		m = parse_mov(linea);
		if (m == -1)
			printf("Illegal move: %s\n", buffComandos);
		else {
			hacerMovimiento(m);

			if (juego.colorTurno) // Si le toca al negro, se comprueba que el rey blanco no haya quedado en jaque
			{
				esJaque = esAtacadoPor(juego.tablero[BLANCO][REY], NEGRO);
			} else {
				esJaque = esAtacadoPor(juego.tablero[NEGRO][REY], BLANCO);
			}
			
			if (!esJaque)
			{
				juego.Buffer_MOV_INDEXCAPAS[1] = generarTodosMov(0);
			} else {
				printf("Illegal move (in check): %s\n", buffComandos);
				desHacerMovimiento(m);
			}
		}
	}
}


BOOLEANO esUnMovimiento(const char *l)
{
	if (l[0] > 96 && l[0] < 105)
	{
		if (l[1] > 48 && l[1] < 57)
		{
			if (l[2] > 96 && l[2] < 105)
			{
				if (l[3] > 48 && l[3] < 57)
				{
					return VERDADERO;
				}			
			}
		}	
	}

	return FALSO;
}

MOVIMIENTO parse_mov(char *s)
{
	uint8 origen, destino;
	int i = 0;
	PIEZA pCoronada = VACIO;

	origen  = s[0]   - 97;
	origen += ((s[1] - 49) * 8);

	destino  = s[2]   - 97;
	destino += ((s[3] - 49) * 8);

	for (i=juego.Buffer_MOV_INDEXCAPAS[0];i<juego.Buffer_MOV_INDEXCAPAS[1];i++)
	{
		if ( ((OBT_MOV_ORIGEN(juego.Buffer_MOV[i])) == origen) &&  ((OBT_MOV_DESTINO(juego.Buffer_MOV[i]))==destino))
		{
			if (ES_MOV_PROMOCION(juego.Buffer_MOV[i]))
			{
				switch (s[4]) 
				{
					case 'n':
					case 'N':
						pCoronada = OBT_MOV_PROMOCION(juego.Buffer_MOV[i]);
						if (pCoronada == CABALLO_BLANCO || pCoronada == CABALLO_NEGRO)
							return juego.Buffer_MOV[i];
						break;
					case 'b':
					case 'B':
						pCoronada = OBT_MOV_PROMOCION(juego.Buffer_MOV[i]);
						if (pCoronada == ALFIL_BLANCO || pCoronada == ALFIL_NEGRO)
							return juego.Buffer_MOV[i];
						break;
					case 'r':
					case 'R':
						pCoronada = OBT_MOV_PROMOCION(juego.Buffer_MOV[i]);
						if (pCoronada == TORRE_BLANCO || pCoronada == TORRE_NEGRO)
							return juego.Buffer_MOV[i];
						break;
					default:  
						return juego.Buffer_MOV[i];
				}

			} else {

				return juego.Buffer_MOV[i];
			}
		}
	}

	return -1;
}

//*/



#endif



