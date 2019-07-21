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

#ifndef OPERACIONES_BIT_C
#define OPERACIONES_BIT_C

//				operaciones con bit
//*
void imprimirBitTablero(uint64 numero)
{

	int i=0,j=0, marca=56;
	uint64 mascara = (uint64)BITSET[marca];

	for (i=0;i<8;i++)
	{
		for (j=0;j<8;j++)
		{
			printf(" %d ", (numero & mascara)>0);
			mascara = mascara << 1;
		}
		marca -=8;
		mascara = (uint64)BITSET[marca];
		printf("\n");
	}
	printf("\n");
}


void imprimirBits(uint64 numero)
{

	int i=0;
	uint64 mascara = BITSET[63];

	for (i=0;i<64;i++)
	{
		printf("%d", (numero & mascara)>0);
		mascara = mascara >> 1;
	}
	printf("\n");
}
//*/

static inline int bitScanForwardBruijn(uint64 bb) 
{
	static const int index64[64] 	= {
					   63, 0,58, 1,59,47,53, 2,
					   60,39,48,27,54,33,42, 3,
					   61,51,37,40,49,18,28,20,
					   55,30,34,11,43,14,22, 4,
					   62,57,46,52,38,26,32,41,
					   50,36,17,19,29,10,13,21,
					   56,45,25,31,35,16, 9,12,
					   44,24,15, 8,23, 7, 6, 5};

	return index64[((bb & -bb) * debruijn64) >> 58];
}

static inline int convertir2lista(uint64 bb, uint8 *lista)
{
	int total=0;
	while ( bb ) {
	   *lista++  = bitScanForwardBruijn(bb); // determine bit index of least significant one bit
	   total++;	
	   bb &= bb-1; // reset LS1B
	}

	return total;
}

static inline uint32 cuentaBit(uint64 bitmap)
{
 
// MIT HAKMEM algorithm, see http://graphics.stanford.edu/~seander/bithacks.html
 
static const uint64  M1 = 0x5555555555555555ull;  // 1 zero,  1 one ...
static const uint64  M2 = 0x3333333333333333ull;  // 2 zeros,  2 ones ...
static const uint64  M4 = 0x0f0f0f0f0f0f0f0full;  // 4 zeros,  4 ones ...
static const uint64  M8 = 0x00ff00ff00ff00ffull;  // 8 zeros,  8 ones ...
static const uint64 M16 = 0x0000ffff0000ffffull;  // 16 zeros, 16 ones ...
static const uint64 M32 = 0x00000000ffffffffull;  // 32 zeros, 32 ones
 
    bitmap = (bitmap & M1 ) + ((bitmap >>  1) & M1 );   //put count of each  2 bits into those  2 bits
    bitmap = (bitmap & M2 ) + ((bitmap >>  2) & M2 );   //put count of each  4 bits into those  4 bits
    bitmap = (bitmap & M4 ) + ((bitmap >>  4) & M4 );   //put count of each  8 bits into those  8 bits
    bitmap = (bitmap & M8 ) + ((bitmap >>  8) & M8 );   //put count of each 16 bits into those 16 bits
    bitmap = (bitmap & M16) + ((bitmap >> 16) & M16);   //put count of each 32 bits into those 32 bits
    bitmap = (bitmap & M32) + ((bitmap >> 32) & M32);   //put count of each 64 bits into those 64 bits
    return (uint32)bitmap;
}

static inline uint32 bitScanLast(uint64 bitmap)
{
    // this is Eugene Nalimov's bitScanReverse
       // use firstOne if you can, it is faster than lastOne.
       // don't use this if bitmap = 0
 
       int result = 0;
       if (bitmap > 0xFFFFFFFF)
       {
              bitmap >>= 32;
              result = 32;
       }
       if (bitmap > 0xFFFF)
       {
              bitmap >>= 16;
              result += 16;
       }
       if (bitmap > 0xFF)
       {
              bitmap >>= 8;
              result += 8;
       }
       return result + TABLA_MSB1[bitmap];
}


//*********************************************************************************************************************************

#endif
