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

#ifndef TIPO_DATOS_H
#define TIPO_DATOS_H

#ifndef MAX_HILOS
#define MAX_HILOS 4
#endif

typedef unsigned char 	uint8;
typedef signed   char 	sint8;
typedef unsigned short 	uint16;
typedef unsigned int 	uint32;
typedef char 		TIPS[256]; 


#ifdef _MSC_VER
  typedef unsigned __int64 		uint64;
#else
  typedef unsigned long long int 	uint64;
#endif

typedef uint8  BOOLEANO;
typedef uint8  COLOR;
typedef uint32 MOVIMIENTO;
typedef uint8  PIEZA;
typedef uint8  TABLERO[64];
typedef char   MOVALG[20];	

typedef struct 
{
	PIEZA	pieza;
	uint8	estado;
	uint8	numPosValidas;
	uint8	posValidas[28]; 
} VISTATABLERO;

typedef struct
{
	uint8 conteo[2][6];
	uint8 fase[2];
	uint8 escaqueRey[2];
	int pstMedio[2];
	int pstFinal[2];
} ESTADO_EVALUACION;

typedef struct
{
	BOOLEANO	OOB;
	BOOLEANO	OOOB;
	BOOLEANO	OON;
	BOOLEANO	OOON;
	BOOLEANO	ENROQUEB;
	BOOLEANO	ENROQUEN;
	uint8		posPeonPaso;
	uint16		reglaCincuentaMov;
	uint32		totalMov;
	MOVIMIENTO	mov;
	VISTATABLERO	vistaTablero[64];
	MOVALG		movAlg;
	uint64		llaveHash;
	uint64		llavePeones;
	ESTADO_EVALUACION estadoEvaluacion;
} DATAJUEGO;


typedef struct
{
	uint64  	tablero[2][6];
	uint64  	blancos;			//amigos;
	uint64  	negros;				//enemigos;
	uint64  	ocupados;
	uint64  	desOcupados;
	uint64  	destinos;
	COLOR		colorTurno;
	BOOLEANO	OOB;
	BOOLEANO	OOOB;
	BOOLEANO	OON;
	BOOLEANO	OOON;
	int		ENROQUEB;
	int		ENROQUEN;
	uint8		posPeonPaso;
	uint16		reglaCincuentaMov;
	uint32		totalMov;
	uint32 		Buffer_MOV[MAX_BUFF_MOV];
	int 		Buffer_MOV_PONDERACION[MAX_BUFF_MOV];
	uint32 		Buffer_MOV_INDEXCAPAS[MAX_CAPAS];
	COLOR 		vista;
	DATAJUEGO	historicoJuego[MAX_BUFF_MOV];	
	MOVIMIENTO	historicoMovMatadores[2][2][MAX_CAPAS];
	uint32 		indiceHJuego;
	int		material_total;
	uint32		material_peon_blanco;
	uint32		material_lado_blanco;
	uint32		material_peon_negro;
	uint32		material_lado_negro;
	uint64		llaveHash;
	uint64		llavePeones;
	ESTADO_EVALUACION estadoEvaluacion;
	uint32		maxTiempo;	

	// Variables para la búsqueda iterativa de profundidad
	int 		triangularLargo[MAX_CAPAS];
	MOVIMIENTO 	triangularArray[MAX_CAPAS][MAX_CAPAS];
	int 		profundidadBusquedad;
	int 		ultimoPVIndex;
	MOVIMIENTO	ultimoPV[MAX_CAPAS];
	uint32 		heuristicaBlanca[64][64];
	uint32 		heuristicaNegra[64][64];
} BITTABLERO;

typedef BITTABLERO Position;

typedef struct
{
	int nPeones[2];
	int nDama[2];
	int nCaballos[2];
	int nTorres[2];
	int nAlfil[2];
	int nPiezasSP[2];
	int nPiezas[2];
	int totalPiezasSP;
	int totalPeones;
	int totalPiezas;
	int materialTotal;
	int valorPiezas[2];
	int valorPeones[2];
	uint64 mapaTodosPeones;
	int FASE;
	uint64 piezasBlancas;
	uint64 piezasNegras;
	int fase[2];
	int peonDefectos[2][8];
	int puntaje_m[2];
	int puntaje_f[2];
	BOOLEANO esPeligroso[2];
	int PIEZAS_MAYORES[2];
	int PIEZAS_MENORES[2];
	uint64 mapaPosAtacadasXPza[2][6];
	uint64 mapaPosAtacadas[2];
	uint64 mapaPosAtacadasPseudo[2];
	uint64 mapaPosAtacadasDoble[2];
	uint64 mapaClavadas[2];
	uint64 mapaClavadasRey[2];
	uint64 mapaRayosClavada[64];
	int atacantesRey[2];
	int pesoAtaqueRey[2];
	uint64 peonesDebiles[2];
	uint64 peonesPasados[2];
	uint64 peonesCandidatos[2];
	uint8 escaqueRey[2];
	uint64 entornoRey[2];
	unsigned char peonesMapaFila[2];
	unsigned char peonesPasadosMapaFila[2];
} EvalScratch;

typedef struct
{
	Position posicion;
	EvalScratch scratch;
} ThreadState;

typedef struct
{
	uint32		InvCaptura;
	uint32		InvPeonPaso;
	uint32		InvPromocion;
	uint32		InvEnroqueOO;
	uint32		InvEnroqueOOO;
	uint32		InvJaqueContrario;
} DATA_PERFT;

typedef struct 
{
	uint64 llaves[64][16];  
	uint64 lado;          
	uint64 ep[64];        
	uint64 OOB;            
	uint64 OOOB;            
	uint64 OON;            
	uint64 OOON;           
} LLAVESHASH;

typedef struct 
{
	uint64	id;
	int 	valor;
	uint16	generacion;

} HASH_EVAL;

typedef struct
{
	uint64 peonesBlancos;
	uint64 peonesNegros;
	int puntaje_m[2];
	int puntaje_f[2];
	uint64 debiles[2];
	uint64 pasados[2];
	uint64 candidatos[2];
	uint8 filas[2];
	uint8 filasPasados[2];
	int defectosRey[2][8];
	uint16 generacion;
} HASH_PEONES;

typedef struct
{
	uint64		id;
	uint16		profundidad;
	uint16		generacion;
	uint16		reglaCincuentaMov;
	int		evalEstatico;
	int		puntaje;
	MOVIMIENTO	mov;
	uint8		banderas;
	uint8		enroqueBlanco;
	uint8		enroqueNegro;
} REGISTRO_TABLA_HASH;

#endif














