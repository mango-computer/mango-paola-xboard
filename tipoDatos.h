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

#ifndef TIPO_DATOS_H
#define TIPO_DATOS_H

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
	BOOLEANO	OOB;
	BOOLEANO	OOOB;
	BOOLEANO	OON;
	BOOLEANO	OOON;
	BOOLEANO	ENROQUEB;
	BOOLEANO	ENROQUEN;
	uint8		posPeonPaso;
	uint8		reglaCincuentaMov;
	uint32		totalMov;
	MOVIMIENTO	mov;
	VISTATABLERO	vistaTablero[64];
	MOVALG		movAlg;
	uint64		llaveHash;
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
	uint8		reglaCincuentaMov;
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
	uint32		maxTiempo;	

	// Variables para la busquedad interactiva de profundidad
	int 		triangularLargo[MAX_CAPAS];
	MOVIMIENTO 	triangularArray[MAX_CAPAS][MAX_CAPAS];
	int 		profundidadBusquedad;
	int 		ultimoPVIndex;
	MOVIMIENTO	ultimoPV[MAX_CAPAS];
	uint32 		heuristicaBlanca[64][64];
	uint32 		heuristicaNegra[64][64];
} BITTABLERO;

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

} HASH_EVAL;

typedef struct
{
	uint64		id;
	uint8		profundidad;
	int		puntaje;
	MOVIMIENTO	mov;
	uint8		banderas;
} REGISTRO_TABLA_HASH;

#endif














