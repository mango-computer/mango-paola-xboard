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
char			rutaLibroPolyglot[512]				= "polyglotbooks/mangoac-book.bin";
unsigned int		maxLibroPolyglotMB				= MAX_LIBRO_POLYGLOT_MB;

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
ThreadState		hilosBusqueda[MAX_HILOS];
ThreadState		*hiloActual					= &hilosBusqueda[0];
int			numHilosBusqueda				= 1;
#define			nPeones		(hiloActual->scratch.nPeones)
#define			nDama		(hiloActual->scratch.nDama)
#define			nCaballos	(hiloActual->scratch.nCaballos)
#define			nTorres		(hiloActual->scratch.nTorres)
#define			nAlfil		(hiloActual->scratch.nAlfil)
#define			nPiezasSP	(hiloActual->scratch.nPiezasSP)
#define			nPiezas		(hiloActual->scratch.nPiezas)
#define			totalPiezasSP	(hiloActual->scratch.totalPiezasSP)
#define			totalPeones	(hiloActual->scratch.totalPeones)
#define			totalPiezas	(hiloActual->scratch.totalPiezas)
#define			materialTotal	(hiloActual->scratch.materialTotal)
#define			valorPiezas	(hiloActual->scratch.valorPiezas)
#define			valorPeones	(hiloActual->scratch.valorPeones)
#define			mapaTodosPeones	(hiloActual->scratch.mapaTodosPeones)
uint64 			CARRERA_PEONES[2][2][64];
uint64 			PEON_ESCONDIDO_I[2][9];
uint64 			PEON_ESCONDIDO_D[2][9];
uint64			MASCARA_PEON_CC[64];
unsigned char 		is_outside[256][256];
#define			peonesMapaFila		(hiloActual->scratch.peonesMapaFila)
#define			peonesPasadosMapaFila	(hiloActual->scratch.peonesPasadosMapaFila)
int 			MATRIZ_SEGURIDAD_REY[16][16];
uint64	 		MATRIZ_MOV_ALFIL[64];
uint64 			MATRIZ_MOV_TORRE[64];
unsigned char 		msb_8bit[256];
unsigned char 		lsb_8bit[256];
unsigned char 		pop_cnt_8bit[256];
#define			FASE			(hiloActual->scratch.FASE)
#define			piezasBlancas		(hiloActual->scratch.piezasBlancas)
#define			piezasNegras		(hiloActual->scratch.piezasNegras)
#define			faseEval		(hiloActual->scratch.fase)
#define			peonDefectos		(hiloActual->scratch.peonDefectos)
#define			puntajeEval_m		(hiloActual->scratch.puntaje_m)
#define			puntajeEval_f		(hiloActual->scratch.puntaje_f)
#define			esPeligroso		(hiloActual->scratch.esPeligroso)
#define			PIEZAS_MAYORES		(hiloActual->scratch.PIEZAS_MAYORES)
#define			PIEZAS_MENORES		(hiloActual->scratch.PIEZAS_MENORES)
#define			mapaPosAtacadasXPza	(hiloActual->scratch.mapaPosAtacadasXPza)
#define			mapaPosAtacadas		(hiloActual->scratch.mapaPosAtacadas)
#define			mapaPosAtacadasPseudo	(hiloActual->scratch.mapaPosAtacadasPseudo)
#define			mapaPosAtacadasDoble	(hiloActual->scratch.mapaPosAtacadasDoble)
#define			mapaClavadas		(hiloActual->scratch.mapaClavadas)
#define			mapaClavadasRey		(hiloActual->scratch.mapaClavadasRey)
#define			mapaRayosClavada	(hiloActual->scratch.mapaRayosClavada)
#define			atacantesRey		(hiloActual->scratch.atacantesRey)
#define			pesoAtaqueRey		(hiloActual->scratch.pesoAtaqueRey)
#define			peonesDebiles		(hiloActual->scratch.peonesDebiles)
#define			peonesPasados		(hiloActual->scratch.peonesPasados)
#define			peonesCandidatos	(hiloActual->scratch.peonesCandidatos)
#define			escaqueReyEval		(hiloActual->scratch.escaqueRey)
#define			entornoRey		(hiloActual->scratch.entornoRey) 


struct 			timeval 					tv;

int			tipoDeBusqueda					= TIPO_BUSQUEDA_NORMAL;
BOOLEANO		esPermitirEvalPersoso				= FALSO;
BOOLEANO		permitirLazyEval				= VERDADERO;
BOOLEANO		esUsoTablaHash					= VERDADERO;					
uint64			LARGO_TABLA_HASH				= 0;
size_t			entradasTablaHash				= 0;
size_t			bytesTablaHash					= 0;
unsigned int		hashMBSolicitados				= HASH_MB_PREDETERMINADO;
unsigned int		hashMBReales					= 0;
unsigned int		hashMBPendientes				= HASH_MB_PREDETERMINADO;
BOOLEANO		hashResizePendiente				= FALSO;
uint16			generacionHash					= 1;
BOOLEANO 		esPost						= FALSO;
BOOLEANO		esUCI						= FALSO;
int			uciMultiPV					= 1;
MOVIMIENTO		uciAltMov[3];
int			uciAltScore[3];
int			uciAltCount					= 0;
BOOLEANO		primeraVez					= FALSO;
int			statusFinJuego					= STATUS_FIN_JUEGO_SIN_ESTATUS;
//BOOLEANO		esLibroTablaUsado				= FALSO;
BOOLEANO		esActivoLibro					= VERDADERO;
int			nFueraLibro					= 0;
BOOLEANO		esConsola					= FALSO;
TIPS 			listaTips[100];

int			evalInicioBusquedad[2]				= {INFINITO,INFINITO};

int 			nodoRaiz	 				= 0;
#define			juego			(hiloActual->posicion)
BOOLEANO		esActivoBusTranquilidad				= VERDADERO;
uint8 			PROMOCION[2]; 
uint32 			CONT_BUFF_COMANDOS 				= 0;
char 			BUFF_COMANDOS[LARGO_BUFER_COMANDOS];
int 			VALOR_EMPATE					= 0;
BOOLEANO 		tiempoVencido 					= FALSO;
int 			contadorDescendente;
int 			contadorNodos 					= 0;	
int 			QcontadorNodos					= 0;
#ifdef PRUEBAS_HCE
int			contadorPodasEET				= 0;
int			contadorExtensionesEET				= 0;
uint64			llamadasEval					= 0;
uint64			llamadasEvalEnJaque				= 0;
uint64			llamadasEETCompleto				= 0;
uint64			llamadasEETUmbral				= 0;
uint64			ramasEETCompleto				= 0;
uint64			ramasEETUmbral					= 0;
uint64			cortesEETUmbral				= 0;
uint64			llamadasEETPOS					= 0;
int			profundidadMaxEET				= 0;
#endif
int 			alfaGlobal 					= 0;
int 			betaGlobal 					= 0;
char 			pngmov[20];
uint64 			tiempoAculumado					= 0;
HASH_EVAL 		*hash_eval;
HASH_PEONES		*hash_peones;
uint64			aciertosHashPeones				= 0;
uint64			consultasHashPeones				= 0;
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
