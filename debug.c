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

#ifndef DEBUG_C
#define DEBUG_C

void debugMov(char *llamado, MOVIMIENTO mov)
{

       int mat, i, j;
 
       // comprobar si ambos reyes están presentes
	if ((cuentaBit(juego.tablero[BLANCO][REY]) != 1) || (cuentaBit(juego.tablero[NEGRO][REY]) != 1))
	{
		printf("Rey ha sido capturado :%s\n", llamado);
		printf("Color Turno: %s\n",OBT_STR_COLOR_TURNO);
		verMov(mov);
		for (j = 0 ; j < juego.indiceHJuego ; j++)
		{
			printf("%d. ", j+1); 
			verMov(juego.historicoJuego[j].mov);
			printf("\n");
		}
		mostrarTablero();
		printf("Rey ha sido capturado :%s\n", llamado);
		exit(0);
	}
 
	for (i = 0 ; i < 64 ; i++)
	{
		if ((juego.tablero[NEGRO][DAMA] & BITSET[i]) && (ESCAQUES[i] != DAMA_NEGRO))
                {
			printf("Inconsistencia en Dama Negra :%s\n", llamado);
			printf("Color Turno: %s\n",OBT_STR_COLOR_TURNO);
			verMov(mov);
			for (j = 0 ; j < juego.indiceHJuego ; j++)
			{
				printf("%d. ", j+1); 
				verMov(juego.historicoJuego[j].mov);
				printf("\n");
			}
			mostrarTablero();
			imprimirBitTablero(juego.tablero[NEGRO][DAMA]);
			printf("Inconsistencia en Dama Negra :%s\n", llamado);
			exit(0);
                }

		if ((juego.tablero[NEGRO][TORRE] & BITSET[i]) && (ESCAQUES[i] != TORRE_NEGRO))
                {
			printf("Inconsistencia en Torre Negra :%s\n", llamado);
			printf("Color Turno: %s\n",OBT_STR_COLOR_TURNO);
			verMov(mov);
			for (j = 0 ; j < juego.indiceHJuego ; j++)
			{
				printf("%d. ", j+1); 
				verMov(juego.historicoJuego[j].mov);
				printf("\n");
			}
			mostrarTablero();
			imprimirBitTablero(juego.tablero[NEGRO][TORRE]);
			printf("Inconsistencia en Torre Negra :%s\n", llamado);
			exit(0);
                }

		if ((juego.tablero[NEGRO][ALFIL] & BITSET[i]) && (ESCAQUES[i] != ALFIL_NEGRO))
                {
			printf("Inconsistencia en Alfil Negro :%s\n", llamado);
			printf("Color Turno: %s\n",OBT_STR_COLOR_TURNO);
			verMov(mov);
			for (j = 0 ; j < juego.indiceHJuego ; j++)
			{
				printf("%d. ", j+1); 
				verMov(juego.historicoJuego[j].mov);
				printf("\n");
			}
			mostrarTablero();
			imprimirBitTablero(juego.tablero[NEGRO][ALFIL]);
			printf("Inconsistencia en Alfil Negro :%s\n", llamado);
			exit(0);
                }

		if ((juego.tablero[NEGRO][CABALLO] & BITSET[i]) && (ESCAQUES[i] != CABALLO_NEGRO))
                {
			printf("Inconsistencia en Caballo Negro :%s\n", llamado);
			printf("Color Turno: %s\n",OBT_STR_COLOR_TURNO);
			verMov(mov);
			for (j = 0 ; j < juego.indiceHJuego ; j++)
			{
				printf("%d. ", j+1); 
				verMov(juego.historicoJuego[j].mov);
				printf("\n");
			}
			mostrarTablero();
			imprimirBitTablero(juego.tablero[NEGRO][CABALLO]);
			printf("Inconsistencia en Caballo Negro :%s\n", llamado);
			exit(0);
                }

		if ((juego.tablero[NEGRO][REY] & BITSET[i]) && (ESCAQUES[i] != REY_NEGRO))
                {
			printf("Inconsistencia en Rey Negro :%s\n", llamado);
			printf("Color Turno: %s\n",OBT_STR_COLOR_TURNO);
			verMov(mov);
			for (j = 0 ; j < juego.indiceHJuego ; j++)
			{
				printf("%d. ", j+1); 
				verMov(juego.historicoJuego[j].mov);
				printf("\n");
			}
			mostrarTablero();
			imprimirBitTablero(juego.tablero[NEGRO][REY]);
			printf("Inconsistencia en Rey Negro :%s\n", llamado);
			exit(0);
                }

		if ((juego.tablero[NEGRO][PEON] & BITSET[i]) && (ESCAQUES[i] != PEON_NEGRO))
                {
			printf("Inconsistencia en Peon Negro :%s\n", llamado);
			printf("Color Turno: %s\n",OBT_STR_COLOR_TURNO);
			verMov(mov);
			for (j = 0 ; j < juego.indiceHJuego ; j++)
			{
				printf("%d. ", j+1); 
				verMov(juego.historicoJuego[j].mov);
				printf("\n");
			}
			mostrarTablero();
			imprimirBitTablero(juego.tablero[NEGRO][PEON]);
			printf("Inconsistencia en Peon Negro :%s\n", llamado);
			exit(0);
                }

// Las Blancas

		if ((juego.tablero[BLANCO][DAMA] & BITSET[i]) && (ESCAQUES[i] != DAMA_BLANCO))
                {
			printf("Inconsistencia en Dama Blanca :%s\n", llamado);
			printf("Color Turno: %s\n",OBT_STR_COLOR_TURNO);
			verMov(mov);
			for (j = 0 ; j < juego.indiceHJuego ; j++)
			{
				printf("%d. ", j+1); 
				verMov(juego.historicoJuego[j].mov);
				printf("\n");
			}
			mostrarTablero();
			imprimirBitTablero(juego.tablero[BLANCO][DAMA]);
			printf("Inconsistencia en Dama Blanca :%s\n", llamado);
			exit(0);
                }

		if ((juego.tablero[BLANCO][TORRE] & BITSET[i]) && (ESCAQUES[i] != TORRE_BLANCO))
                {
			printf("Inconsistencia en Torre Blanca :%s\n", llamado);
			printf("Color Turno: %s\n",OBT_STR_COLOR_TURNO);
			verMov(mov);
			for (j = 0 ; j < juego.indiceHJuego ; j++)
			{
				printf("%d. ", j+1); 
				verMov(juego.historicoJuego[j].mov);
				printf("\n");
			}
			mostrarTablero();
			imprimirBitTablero(juego.tablero[BLANCO][TORRE]);
			printf("Inconsistencia en Torre Blanca :%s\n", llamado);
			exit(0);
                }

		if ((juego.tablero[BLANCO][ALFIL] & BITSET[i]) && (ESCAQUES[i] != ALFIL_BLANCO))
                {
			printf("Inconsistencia en Alfil Blanco :%s\n", llamado);
			printf("Color Turno: %s\n",OBT_STR_COLOR_TURNO);
			verMov(mov);
			for (j = 0 ; j < juego.indiceHJuego ; j++)
			{
				printf("%d. ", j+1); 
				verMov(juego.historicoJuego[j].mov);
				printf("\n");
			}
			mostrarTablero();
			imprimirBitTablero(juego.tablero[BLANCO][ALFIL]);
			printf("Inconsistencia en Alfil Blanco :%s\n", llamado);
			exit(0);
                }

		if ((juego.tablero[BLANCO][CABALLO] & BITSET[i]) && (ESCAQUES[i] != CABALLO_BLANCO))
                {
			printf("Inconsistencia en Caballo Blanco :%s\n", llamado);
			printf("Color Turno: %s\n",OBT_STR_COLOR_TURNO);
			verMov(mov);
			for (j = 0 ; j < juego.indiceHJuego ; j++)
			{
				printf("%d. ", j+1); 
				verMov(juego.historicoJuego[j].mov);
				printf("\n");
			}
			mostrarTablero();
			imprimirBitTablero(juego.tablero[BLANCO][CABALLO]);
			printf("Inconsistencia en Caballo Blanco :%s\n", llamado);
			exit(0);
                }

		if ((juego.tablero[BLANCO][REY] & BITSET[i]) && (ESCAQUES[i] != REY_BLANCO))
                {
			printf("Inconsistencia en Rey Blanco :%s\n", llamado);
			printf("Color Turno: %s\n",OBT_STR_COLOR_TURNO);
			verMov(mov);
			for (j = 0 ; j < juego.indiceHJuego ; j++)
			{
				printf("%d. ", j+1); 
				verMov(juego.historicoJuego[j].mov);
				printf("\n");
			}
			mostrarTablero();
			imprimirBitTablero(juego.tablero[BLANCO][REY]);
			printf("Inconsistencia en Rey Blanco :%s\n", llamado);
			exit(0);
                }

		if ((juego.tablero[BLANCO][PEON] & BITSET[i]) && (ESCAQUES[i] != PEON_BLANCO))
                {
			printf("Inconsistencia en Peon Blanco :%s\n", llamado);
			printf("Color Turno: %s\n",OBT_STR_COLOR_TURNO);
			verMov(mov);
			for (j = 0 ; j < juego.indiceHJuego ; j++)
			{
				printf("%d. ", j+1); 
				verMov(juego.historicoJuego[j].mov);
				printf("\n");
			}
			mostrarTablero();
			imprimirBitTablero(juego.tablero[BLANCO][PEON]);
			printf("Inconsistencia en Peon Blanco :%s\n", llamado);
			exit(0);
                }
       }
 
	if (juego.blancos != (juego.tablero[BLANCO][REY] | juego.tablero[BLANCO][DAMA] | juego.tablero[BLANCO][TORRE] | 
			      juego.tablero[BLANCO][ALFIL] | juego.tablero[BLANCO][CABALLO] | juego.tablero[BLANCO][PEON] ))
	{
		printf("Inconsistencia en Bitmap Blanco :%s\n", llamado);
		printf("Color Turno: %s\n",OBT_STR_COLOR_TURNO);
		verMov(mov);
		for (j = 0 ; j < juego.indiceHJuego ; j++)
		{
			printf("%d. ", j+1); 
			verMov(juego.historicoJuego[j].mov);
			printf("\n");
		}
		mostrarTablero();
		printf("Inconsistencia en Bitmap Blanco :%s\n", llamado);
		exit(0);
       }
 
  	if (juego.negros != (juego.tablero[NEGRO][REY] | juego.tablero[NEGRO][DAMA] | juego.tablero[NEGRO][TORRE] | 
			      juego.tablero[NEGRO][ALFIL] | juego.tablero[NEGRO][CABALLO] | juego.tablero[NEGRO][PEON] ))
	{
		printf("Inconsistencia en Bitmap Negro :%s\n", llamado);
		printf("Color Turno: %s\n",OBT_STR_COLOR_TURNO);
		verMov(mov);
		for (j = 0 ; j < juego.indiceHJuego ; j++)
		{
			printf("%d. ", j+1); 
			verMov(juego.historicoJuego[j].mov);
			printf("\n");
		}
		mostrarTablero();
		printf("Inconsistencia en Bitmap Negro :%s\n", llamado);
		exit(0);
       }

  	if (juego.ocupados != (juego.blancos  | juego.negros))
	{
		printf("Inconsistencia en Bitmap Ocupados :%s\n", llamado);
		printf("Color Turno: %s\n",OBT_STR_COLOR_TURNO);
		verMov(mov);
		for (j = 0 ; j < juego.indiceHJuego ; j++)
		{
			printf("%d. ", j+1); 
			verMov(juego.historicoJuego[j].mov);
			printf("\n");
		}
		mostrarTablero();
		printf("Inconsistencia en Bitmap Ocupados :%s\n", llamado);
		exit(0);
       }

  
       mat = (cuentaBit(juego.tablero[BLANCO][PEON]) 	* VALOR_PEON) 		+
             (cuentaBit(juego.tablero[BLANCO][CABALLO]) * VALOR_CABALLO)	+
             (cuentaBit(juego.tablero[BLANCO][ALFIL]) 	* VALOR_ALFIL) 		+
             (cuentaBit(juego.tablero[BLANCO][TORRE]) 	* VALOR_TORRE) 		+
             (cuentaBit(juego.tablero[BLANCO][DAMA]) 	* VALOR_DAMA);
       mat-= (cuentaBit(juego.tablero[NEGRO][PEON]) 	* VALOR_PEON) 		+
             (cuentaBit(juego.tablero[NEGRO][CABALLO])  * VALOR_CABALLO)	+
             (cuentaBit(juego.tablero[NEGRO][ALFIL]) 	* VALOR_ALFIL) 		+
             (cuentaBit(juego.tablero[NEGRO][TORRE]) 	* VALOR_TORRE) 		+
             (cuentaBit(juego.tablero[NEGRO][DAMA]) 	* VALOR_DAMA);

	if (juego.material_total != mat)
	{
		printf("Inconsistencia en Balance Material :%s\n", llamado);
		printf("material_total=%d\nmat=%d\n",juego.material_total,mat);
		printf("Color Turno: %s\n",OBT_STR_COLOR_TURNO);
		verMov(mov);
		for (j = 0 ; j < juego.indiceHJuego ; j++)
		{
			printf("%d. ", j+1); 
			verMov(juego.historicoJuego[j].mov);
			printf("\n");
		}
		mostrarTablero();
		printf("Inconsistencia en Balance Material :%s\n", llamado);
		printf("material_total=%d\nmat=%d\n",juego.material_total,mat);
		printf("Color Turno: %s\n",OBT_STR_COLOR_TURNO);
		exit(0);
	}


	uint64 llave=0;
	int posX=0;
	
	for (;posX<64;posX++)
	{
		if (ESCAQUES[posX] != VACIO)
		{
			llave ^= arrayHash.llaves[posX][ESCAQUES[posX]];
		}
	}

	if (juego.OOB)					llave ^= arrayHash.OOB;
	if (juego.OOOB)					llave ^= arrayHash.OOOB;
	if (juego.OON)					llave ^= arrayHash.OON;
	if (juego.OOON)					llave ^= arrayHash.OOON;
	if (juego.colorTurno)				llave ^= arrayHash.lado;
	if (juego.posPeonPaso != SIN_POS_VALIDA) 	llave ^= arrayHash.ep[juego.posPeonPaso];
 
	if (juego.llaveHash != llave)
	{
		printf("Inconsistencia llave hash :%s\n", llamado);
		printf("Color Turno: %s\n",OBT_STR_COLOR_TURNO);
		verMov(mov);
		for (j = 0 ; j < juego.indiceHJuego ; j++)
		{
			printf("%d. ", j+1); 
			verMov(juego.historicoJuego[j].mov);
			printf("\n");
		}
		mostrarTablero();
		printf("Inconsistencia llave hash :%s\n", llamado);
		exit(0);
	}
	

}

#endif
