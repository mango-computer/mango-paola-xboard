/* 
	Mango Paola Ajedrez 1.0
	Licencia: GPLv3   
    	Copyright (c) 2012-2013 - Mango Computer c.a 

    	El Programa Mango Paola Ajedrez ésta basado en la investigación de Jose Andres Morales Linares.

	Nadie debería estar restringido por el software que utilizan. Hay cuatro libertades que cada usuario debe tener:
	* La libertad de usar el programa para cualquier propósito,
	* La libertad de cambiar el software para satisfacer sus necesidades,
	* La libertad de compartir el software con sus amigos y vecinos.
	* La libertad de compartir los cambios que realice.

	Cuando un programa se ofrece a los usuarios todas estas libertades, lo llamamos software libre.

	Winglet es un bitboard de código abierto motor de ajedrez. El programa es software libre. Usted puede redistribuirlo y/o 		modificarlo bajo los términos de la Licencia Pública General de GNU según es publicada por la Free Software Foundation, 	bien de 	la versión 3 de la Licencia, o (a su elección) cualquier versión posterior. El programa se distribuye 		con la esperanza de que 		sea útil, pero SIN NINGUNA GARANTÍA, incluso sin la garantía implícita de 		COMERCIALIZACIÓN o IDONEIDAD PARA UN PROPÓSITO 		PARTICULAR. Vea la Licencia Pública General de GNU para más 		detalles: http://www.gnu.org/licenses/

	Existen muchos avances en la forma de escribir motores de ajedrez, estos dos sitios web 
	fuerón de mucha ayuda durante el proceso de investigacion y desarrollo. 
	* http://chessprogramming.wikispaces.com
    	* http://www.sluijten.com/winglet/

    	Información de contacto:
	comprasmangocomputer@gmail.com

*/

#ifndef VARIABLES_H
#define VARIABLES_H

#ifdef COMPILAR_CON_EGBB
BOOLEANO		egbb_is_loaded;
BOOLEANO		usar_egbb;
int 			egbb_load_type 					= 1;
char			rutaEGBB[512];
int 			egbb_cache_size;
int 			nLimitePiezasBitBases 				= 4;
int 			profundidadEgbb;
#endif
int 			XError 						= 0;
char			rutaRandom1[512];
char			rutaRandom2[512];
char			rutaMainBook[512];
char			rutaTourBook[512];

int			maxHistorial[2]					= {0,0};		
BOOLEANO		siguePV						= FALSO;


COLOR			ladoJugando					= SIN_COLOR;
BOOLEANO		ABORTABLE					= FALSO;
int 			largoResetMatadores 				= sizeof(MOVIMIENTO)*2*2*MAX_CAPAS;

int			totalMovRaiz					= 0;
int			fraccionTotalMovRaiz				= 0;
MOVIMIENTO		mponder						= 0;
COLOR 			computadora 					= NEGRO;

int			numEstatico					= 0;
int 			nPeones[2]					= {0,0};
int 			nDama[2]					= {0,0};
int 			nCaballos[2]					= {0,0};
int 			nTorres[2]					= {0,0};
int 			nAlfil[2]					= {0,0};
int 			nPiezasSP[2]					= {0,0};
int 			nPiezas[2]					= {0,0};
int 			totalPiezasSP					= 0;
int 			totalPeones					= 0;
int 			totalPiezas					= 0;
int 			materialTotal					= 0;
int			valorPiezas[2]					= {0,0};
int			valorPeones[2]					= {0,0};
uint64			mapaTodosPeones					= 0;
uint64 			CARRERA_PEONES[2][2][64];
uint64 			PEON_ESCONDIDO_I[2][9];
uint64 			PEON_ESCONDIDO_D[2][9];
uint64			MASCARA_PEON_CC[64];
unsigned char 		is_outside[256][256];
unsigned char		peonesMapaFila[2];
unsigned char		peonesPasadosMapaFila[2];
int			tropismo[2];
int 			MATRIZ_SEGURIDAD_REY[16][16];
uint64	 		MATRIZ_MOV_ALFIL[64];
uint64 			MATRIZ_MOV_TORRE[64];
unsigned char 		msb_8bit[256];
unsigned char 		lsb_8bit[256];
unsigned char 		pop_cnt_8bit[256];
int 			FASE;

uint64 			piezasBlancas; 		
uint64 			piezasNegras;  		
int			fase[2]						={0,0};
int			peonDefectos[2][8];


int 			puntaje_m[2]					= {0,0};
int 			puntaje_f[2]					= {0,0};
BOOLEANO		esPeligroso[2]					= {FALSO, FALSO};
int			PIEZAS_MAYORES[2]				= {0,0};
int			PIEZAS_MENORES[2]				= {0,0};

uint64 			mapaPosAtacadasXPza[2][6]			= {{0,0,0,0,0,0},{0,0,0,0,0,0}};
uint64 			mapaPosAtacadas[2]				= {0,0};
uint64 			peonesDebiles[2]				= {0,0};
uint64 			peonesPasados[2] 				= {0,0};
uint64 			peonesCandidatos[2] 				= {0,0};

uint8 			escaqueRey[2];
uint64 			entornoRey[2]; 


struct 			timeval 					tv;

int			tipoDeBusqueda					= TIPO_BUSQUEDA_NORMAL;
BOOLEANO		esPermitirEvalPersoso				= FALSO;					
BOOLEANO		esUsoTablaHash					= VERDADERO;					
unsigned int		LARGO_TABLA_HASH				= 0;
BOOLEANO 		esPost						= FALSO;
BOOLEANO		primeraVez					= FALSO;
int			statusFinJuego					= STATUS_FIN_JUEGO_SIN_ESTATUS;
//BOOLEANO		esLibroTablaUsado				= FALSO;
BOOLEANO		esActivoLibro					= VERDADERO;
int			nFueraLibro					= 0;
BOOLEANO		esConsola					= FALSO;
TIPS 			listaTips[100];

int			evalInicioBusquedad[2]				= {INFINITO,INFINITO};

int 			nodoRaiz	 				= 0;
BITTABLERO 		juego;
BOOLEANO		esActivoBusTranquilidad				= VERDADERO;
uint8 			PROMOCION[2]; 
uint32 			CONT_BUFF_COMANDOS 				= 0;
char 			BUFF_COMANDOS[LARGO_BUFER_COMANDOS];
int 			VALOR_EMPATE					= 0;
BOOLEANO 		tiempoVencido 					= FALSO;
int 			contadorDescendente;
int 			contadorNodos 					= 0;	
int 			QcontadorNodos					= 0;
int 			alfaGlobal 					= 0;
int 			betaGlobal 					= 0;
char 			pngmov[20];
FILE 			*libroBlancas;
FILE 			*libroNegras;
//FILE 			*libroTablas;
uint64 			tiempoAculumado					= 0;
HASH_EVAL 		*hash_eval;
REGISTRO_TABLA_HASH	*tabla_hash;
BOOLEANO 		usarLibroAperturas 				= VERDADERO;
uint8 			nivelPC 					= PC_NIVEL_FACIL;
int			xc						= 0;

// Timer
uint64   		iniTiempo;   
uint64   		finTiempo;    
uint64   		actualTiempo;
uint64   		detenerDeltaTiempo;
struct timeb 		iniBuffer;   
struct timeb 		finBuffer;   
struct timeb 		actualBuffer;
BOOLEANO 		corriendoTiempo; 

DATA_PERFT 		DataPerft; 

MOVIMIENTO 		movResul;
MOVIMIENTO 		movAterior;

int			movResulPuntaje;

int 			Buffer_TMP_ORD[MAX_BUFF_MOV][2];
int			indexTMP_ORD;

LLAVESHASH 		arrayHash;
// Fin Timer

BOOLEANO 		esVariable					= FALSO;
int 			cantPeonesInicioArbol 				= 0;	
int 			cantPiezasInicioArbol 				= 0;
int 			profundidadTotal	  			= 0;
int 			profundidadActual				= 0;

#endif
