

/*
gcc -lm bitmma3.c -Wall -O2 -o mangoac
*/

/*
http://chessprogramming.wikispaces.com
http://chessprogramming.wikispaces.com/General+Setwise+Operations#TheLeastSignificantOneBitLS1B
http://chessprogramming.wikispaces.com/Classical+Approach
http://chessprogramming.wikispaces.com/Efficient+Generation+of+Sliding+Piece+Attacks
*/

/*
	Indicadores del movimiento (orden de los bits y su significado),
	según los estándares habituales de los motores de ajedrez.

	6: Escaque de origen del movimiento
	6: Escaque de destino del movimiento
	4: Pieza de origen
	4: Pieza capturada
	4: Pieza de coronación
	4: Código del movimiento

	Total: 28 bits en un uint32

	Tabla de códigos de movimiento

	cod	promoción	captura		especial 1	especial 0		tipo de movimiento
	0	0		0		0		0			Movimiento tranquilo
	1	0		0		0		1			Avance doble de peón
	2	0		0		1		0			Enroque corto (oo)
	3	0		0		1		1			Enroque largo (ooo)
	4	0		1		0		0			Captura
	5	0		1		0		1			Captura de peón al paso
	8	1		0		0		0			Promoción a caballo
	9	1		0		0		1			Promoción a alfil
	10	1		0		1		0 			Promoción a torre
	11	1		0		1		1			Promoción a dama
	12	1		1		0		0			Promoción/captura a caballo
	13	1		1		0		1			Promoción/captura a alfil
	14	1		1		1		0			Promoción/captura a torre
	15	1		1		1		1			Promoción/captura a dama

	Movimientos tranquilos:
	Son los que no alteran el material: no capturan ni promocionan.
	También se pueden excluir los que presentan amenazas inmediatas, como los jaques.


	Notas:

	printf("%#llxull\n",b);

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/timeb.h>
#include <ctype.h>
//#include <termios.h>
#if !defined(_WIN32) && !defined(_WIN64)
#include <unistd.h>
#endif
#include <time.h>

#if defined (_WIN32) || defined(_WIN64)
    #ifndef WINDOWS
        #define WINDOWS
    #endif
    #include <windows.h>	//bioskey()
#endif


#include "macros.h"
#include "tipoDatos.h"
#include "data.h"
#include "prototipos.h"
#include "variables.h"
#include "operacionesBit.c"
#include "busquedad.c"
#include "busquedadTranquilidad.c"
#include "debug.c"
#include "fen.c"
#include "ataque.c"
#include "ajedrez.c"
#include "ini.c"
#include "comandos.c"
#include "uci.c"
#include "tiempo.c"
#include "perft.c"
#include "fevaluacion.c"
#include "ordenar.c"
#include "util.c"
#include "hash.c"
#include "eet.c"
#include "libro2.c"

#ifdef COMPILAR_CON_EGBB
	#include "egbb.c"
#endif

int main(int np, char* param[])
{
	const char *R  = "\033[0m";
	const char *mx = "\033[38;5;83m";
	const char *mn = "\033[38;5;203m";
	const char *br = "\033[38;5;245m";
	const char *vy = "\033[48;5;220m";
	const char *vb = "\033[48;5;25m";
	const char *vr = "\033[48;5;160m";
	const char *sw = "\033[97m";
	const char *tc = "\033[48;5;81m";
	const char *tp = "\033[48;5;213m";
	const char *tw = "\033[48;5;231m";

	printf("#\n");
	printf("#  %s           +V            %s%s          %s  %s          %s\n", mx, R, vy, R, tc, R);
	printf("#  %s         /  |  \\         %s%s%s ******** %s  %s          %s\n", br, R, vb, sw, R, tp, R);
	printf("#  %s       -V  -V  -V        %s%s          %s  %s          %s\n", mn, R, vr, R, tw, R);
	printf("#  %s      / \\   |   / \\      %s            %s          %s\n", br, R, tp, R);
	printf("#  %s    +V  +V +V +V  +V     %s            %s          %s\n", mx, R, tc, R);
	printf("#\n");
	printf("#  +----------------------------------------------------+\n");
	printf("#  |          Mango Alexander & Camila Ajedrez          |\n");
	printf("#  +----------------------------------------------------+\n");
	printf("#  |  Autor      Jose Andres Morales Linares            |\n");
	printf("#  |  Version    %-38s |\n", VERSION_MANGO_AC);
	printf("#  |  Anio       2013-2026                              |\n");
	printf("#  |  Licencia   GPLv3                                  |\n");
	printf("#  |  Contacto   comprasmangocomputer@gmail.com         |\n");
	printf("#  |  LGBT+ friendly                                    |\n");
	printf("#  +----------------------------------------------------+\n");
	printf("#\n");
	printf("#  Arranque\n");



	LARGO_TABLA_HASH 	= LARGO_TABLA_HASH_LLAVE_22BIT;
	esUsoTablaHash		= VERDADERO;
	usarLibroAperturas 	= VERDADERO;


	FILE *mangoacINI = fopen("mangoac.ini", "r");

	if (mangoacINI)
	{
		// Leer variables de mangoac.ini
		char lineaINI[512];

		fseek(mangoacINI, 0, SEEK_SET);
		while (fgets(lineaINI, 512, mangoacINI)) 
		{
			if (lineaINI[0] != '#' && lineaINI[0] != ' ' && lineaINI[0] != 13 && lineaINI[0] != 10)
			{	

				if (!strncmp(lineaINI, "UsarTablaHash",13))
				{
					int u=0;
					sscanf(lineaINI, "UsarTablaHash %d", &u);
					esUsoTablaHash = ((u)?VERDADERO:FALSO);
					printf("#  Tabla hash            %s\n", esUsoTablaHash ? "activa" : "inactiva");

				} else if (!strncmp(lineaINI, "TamanioTablaHash",16)) {
					int t=0;
					sscanf(lineaINI, "TamanioTablaHash %d", &t);
		
					switch(t)
					{
						case 1: LARGO_TABLA_HASH = LARGO_TABLA_HASH_LLAVE_18BIT;break;
						case 2: LARGO_TABLA_HASH = LARGO_TABLA_HASH_LLAVE_19BIT;break;
						case 3: LARGO_TABLA_HASH = LARGO_TABLA_HASH_LLAVE_20BIT;break;
						case 4: LARGO_TABLA_HASH = LARGO_TABLA_HASH_LLAVE_21BIT;break;
						case 5: LARGO_TABLA_HASH = LARGO_TABLA_HASH_LLAVE_22BIT;break;
						case 6: LARGO_TABLA_HASH = LARGO_TABLA_HASH_LLAVE_23BIT;break;
						case 7: LARGO_TABLA_HASH = LARGO_TABLA_HASH_LLAVE_24BIT;break;
						case 8: LARGO_TABLA_HASH = LARGO_TABLA_HASH_LLAVE_25BIT;break;
						case 9: LARGO_TABLA_HASH = LARGO_TABLA_HASH_LLAVE_26BIT;break;
						default: LARGO_TABLA_HASH = LARGO_TABLA_HASH_LLAVE_22BIT;break;
					};

				} else if (!strncmp(lineaINI, "UsarLibroAperturas",18)) {
					int u=0;
					sscanf(lineaINI, "UsarLibroAperturas %d", &u);
					usarLibroAperturas = ((u) ? VERDADERO:FALSO);
					printf("#  Libro de aperturas    %s\n", usarLibroAperturas ? "activo" : "inactivo");

				} else if (!strncmp(lineaINI, "RutaRandom1",11)) {
					memset(rutaRandom1,'\0',512);
					sscanf(lineaINI, "RutaRandom1 %s", rutaRandom1);

				} else if (!strncmp(lineaINI, "RutaRandom2",11)) {
					memset(rutaRandom2,'\0',512);
					sscanf(lineaINI, "RutaRandom2 %s", rutaRandom2);

				} else if (!strncmp(lineaINI, "RutaMainBook",12)) {
					memset(rutaMainBook,'\0',512);
					sscanf(lineaINI, "RutaMainBook %s", rutaMainBook);

				} else if (!strncmp(lineaINI, "RutaTourBook",12)) {
					memset(rutaTourBook,'\0',512);
					sscanf(lineaINI, "RutaTourBook %s", rutaTourBook);
#ifdef COMPILAR_CON_EGBB
				} else if (!strncmp(lineaINI, "UsarTablaFinalesNalimov",23)) {
					int u=0;
					sscanf(lineaINI, "UsarTablaFinalesNalimov %d", &u);
					usar_egbb = ((u)?VERDADERO:FALSO);
					printf("#  Tablas de finales     %s\n", usar_egbb ? "activas" : "inactivas");

				} else if (!strncmp(lineaINI, "RutaEGBB",8)) {
					memset(rutaEGBB,'\0',512);
					sscanf(lineaINI, "RutaEGBB %s", rutaEGBB);

				} else if (!strncmp(lineaINI, "CacheEGBB",9)) {
					egbb_cache_size = 64;
					sscanf(lineaINI, "CacheEGBB %d", &egbb_cache_size);
					if (egbb_cache_size < 1 || egbb_cache_size > 512) egbb_cache_size = 64;
#endif
 				}
			}
		}

		fclose(mangoacINI);
	}

	iniMatrizMov();
	iniMatrizSeguridadRey();
	iniCarreraPeonesMask();
	iniPeonEscondido();
	iniPeonCC();
	ini_is_outside();
	inicializarTablero();
	inicializarVar();
	iniTablerosUtil();
	actualizarTablerosUtil(BLANCO);
	iniciarHash();
	iniHashTablero();
	juego.Buffer_MOV_INDEXCAPAS[1] = generarTodosMov(0);
	if (usarLibroAperturas)
		abrirLibro2();
#ifdef COMPILAR_CON_EGBB
	if (usar_egbb)
		cargaBitbases();
#endif
	printf("#\n");
	printf("#  Escriba \"ayuda\" o \"help\" y pulse <Enter>.\n");
	printf("#\n");
	leerComandos();

	exit(0);
}

