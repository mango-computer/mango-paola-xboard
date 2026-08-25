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

#ifndef DEBUG_C
#define DEBUG_C

void debugMov(char *llamado, MOVIMIENTO mov)
{

       int mat, i, j;
 
       // comprobar si ambos reyes están presentes
	if ((cuentaBit(juego.tablero[BLANCO][REY]) != 1) || (cuentaBit(juego.tablero[NEGRO][REY]) != 1))
	{
		printf("El rey ha sido capturado: %s\n", llamado);
		printf("Color del turno: %s\n",OBT_STR_COLOR_TURNO);
		verMov(mov);
		for (j = 0 ; j < juego.indiceHJuego ; j++)
		{
			printf("%d. ", j+1); 
			verMov(juego.historicoJuego[j].mov);
			printf("\n");
		}
		mostrarTablero();
		printf("El rey ha sido capturado: %s\n", llamado);
		exit(0);
	}
 
	for (i = 0 ; i < 64 ; i++)
	{
		if ((juego.tablero[NEGRO][DAMA] & BITSET[i]) && (ESCAQUES[i] != DAMA_NEGRO))
                {
			printf("Inconsistencia en la dama negra: %s\n", llamado);
			printf("Color del turno: %s\n",OBT_STR_COLOR_TURNO);
			verMov(mov);
			for (j = 0 ; j < juego.indiceHJuego ; j++)
			{
				printf("%d. ", j+1); 
				verMov(juego.historicoJuego[j].mov);
				printf("\n");
			}
			mostrarTablero();
			imprimirBitTablero(juego.tablero[NEGRO][DAMA]);
			printf("Inconsistencia en la dama negra: %s\n", llamado);
			exit(0);
                }

		if ((juego.tablero[NEGRO][TORRE] & BITSET[i]) && (ESCAQUES[i] != TORRE_NEGRO))
                {
			printf("Inconsistencia en la torre negra: %s\n", llamado);
			printf("Color del turno: %s\n",OBT_STR_COLOR_TURNO);
			verMov(mov);
			for (j = 0 ; j < juego.indiceHJuego ; j++)
			{
				printf("%d. ", j+1); 
				verMov(juego.historicoJuego[j].mov);
				printf("\n");
			}
			mostrarTablero();
			imprimirBitTablero(juego.tablero[NEGRO][TORRE]);
			printf("Inconsistencia en la torre negra: %s\n", llamado);
			exit(0);
                }

		if ((juego.tablero[NEGRO][ALFIL] & BITSET[i]) && (ESCAQUES[i] != ALFIL_NEGRO))
                {
			printf("Inconsistencia en el alfil negro: %s\n", llamado);
			printf("Color del turno: %s\n",OBT_STR_COLOR_TURNO);
			verMov(mov);
			for (j = 0 ; j < juego.indiceHJuego ; j++)
			{
				printf("%d. ", j+1); 
				verMov(juego.historicoJuego[j].mov);
				printf("\n");
			}
			mostrarTablero();
			imprimirBitTablero(juego.tablero[NEGRO][ALFIL]);
			printf("Inconsistencia en el alfil negro: %s\n", llamado);
			exit(0);
                }

		if ((juego.tablero[NEGRO][CABALLO] & BITSET[i]) && (ESCAQUES[i] != CABALLO_NEGRO))
                {
			printf("Inconsistencia en el caballo negro: %s\n", llamado);
			printf("Color del turno: %s\n",OBT_STR_COLOR_TURNO);
			verMov(mov);
			for (j = 0 ; j < juego.indiceHJuego ; j++)
			{
				printf("%d. ", j+1); 
				verMov(juego.historicoJuego[j].mov);
				printf("\n");
			}
			mostrarTablero();
			imprimirBitTablero(juego.tablero[NEGRO][CABALLO]);
			printf("Inconsistencia en el caballo negro: %s\n", llamado);
			exit(0);
                }

		if ((juego.tablero[NEGRO][REY] & BITSET[i]) && (ESCAQUES[i] != REY_NEGRO))
                {
			printf("Inconsistencia en el rey negro: %s\n", llamado);
			printf("Color del turno: %s\n",OBT_STR_COLOR_TURNO);
			verMov(mov);
			for (j = 0 ; j < juego.indiceHJuego ; j++)
			{
				printf("%d. ", j+1); 
				verMov(juego.historicoJuego[j].mov);
				printf("\n");
			}
			mostrarTablero();
			imprimirBitTablero(juego.tablero[NEGRO][REY]);
			printf("Inconsistencia en el rey negro: %s\n", llamado);
			exit(0);
                }

		if ((juego.tablero[NEGRO][PEON] & BITSET[i]) && (ESCAQUES[i] != PEON_NEGRO))
                {
			printf("Inconsistencia en el peón negro: %s\n", llamado);
			printf("Color del turno: %s\n",OBT_STR_COLOR_TURNO);
			verMov(mov);
			for (j = 0 ; j < juego.indiceHJuego ; j++)
			{
				printf("%d. ", j+1); 
				verMov(juego.historicoJuego[j].mov);
				printf("\n");
			}
			mostrarTablero();
			imprimirBitTablero(juego.tablero[NEGRO][PEON]);
			printf("Inconsistencia en el peón negro: %s\n", llamado);
			exit(0);
                }

// Las Blancas

		if ((juego.tablero[BLANCO][DAMA] & BITSET[i]) && (ESCAQUES[i] != DAMA_BLANCO))
                {
			printf("Inconsistencia en la dama blanca: %s\n", llamado);
			printf("Color del turno: %s\n",OBT_STR_COLOR_TURNO);
			verMov(mov);
			for (j = 0 ; j < juego.indiceHJuego ; j++)
			{
				printf("%d. ", j+1); 
				verMov(juego.historicoJuego[j].mov);
				printf("\n");
			}
			mostrarTablero();
			imprimirBitTablero(juego.tablero[BLANCO][DAMA]);
			printf("Inconsistencia en la dama blanca: %s\n", llamado);
			exit(0);
                }

		if ((juego.tablero[BLANCO][TORRE] & BITSET[i]) && (ESCAQUES[i] != TORRE_BLANCO))
                {
			printf("Inconsistencia en la torre blanca: %s\n", llamado);
			printf("Color del turno: %s\n",OBT_STR_COLOR_TURNO);
			verMov(mov);
			for (j = 0 ; j < juego.indiceHJuego ; j++)
			{
				printf("%d. ", j+1); 
				verMov(juego.historicoJuego[j].mov);
				printf("\n");
			}
			mostrarTablero();
			imprimirBitTablero(juego.tablero[BLANCO][TORRE]);
			printf("Inconsistencia en la torre blanca: %s\n", llamado);
			exit(0);
                }

		if ((juego.tablero[BLANCO][ALFIL] & BITSET[i]) && (ESCAQUES[i] != ALFIL_BLANCO))
                {
			printf("Inconsistencia en el alfil blanco: %s\n", llamado);
			printf("Color del turno: %s\n",OBT_STR_COLOR_TURNO);
			verMov(mov);
			for (j = 0 ; j < juego.indiceHJuego ; j++)
			{
				printf("%d. ", j+1); 
				verMov(juego.historicoJuego[j].mov);
				printf("\n");
			}
			mostrarTablero();
			imprimirBitTablero(juego.tablero[BLANCO][ALFIL]);
			printf("Inconsistencia en el alfil blanco: %s\n", llamado);
			exit(0);
                }

		if ((juego.tablero[BLANCO][CABALLO] & BITSET[i]) && (ESCAQUES[i] != CABALLO_BLANCO))
                {
			printf("Inconsistencia en el caballo blanco: %s\n", llamado);
			printf("Color del turno: %s\n",OBT_STR_COLOR_TURNO);
			verMov(mov);
			for (j = 0 ; j < juego.indiceHJuego ; j++)
			{
				printf("%d. ", j+1); 
				verMov(juego.historicoJuego[j].mov);
				printf("\n");
			}
			mostrarTablero();
			imprimirBitTablero(juego.tablero[BLANCO][CABALLO]);
			printf("Inconsistencia en el caballo blanco: %s\n", llamado);
			exit(0);
                }

		if ((juego.tablero[BLANCO][REY] & BITSET[i]) && (ESCAQUES[i] != REY_BLANCO))
                {
			printf("Inconsistencia en el rey blanco: %s\n", llamado);
			printf("Color del turno: %s\n",OBT_STR_COLOR_TURNO);
			verMov(mov);
			for (j = 0 ; j < juego.indiceHJuego ; j++)
			{
				printf("%d. ", j+1); 
				verMov(juego.historicoJuego[j].mov);
				printf("\n");
			}
			mostrarTablero();
			imprimirBitTablero(juego.tablero[BLANCO][REY]);
			printf("Inconsistencia en el rey blanco: %s\n", llamado);
			exit(0);
                }

		if ((juego.tablero[BLANCO][PEON] & BITSET[i]) && (ESCAQUES[i] != PEON_BLANCO))
                {
			printf("Inconsistencia en el peón blanco: %s\n", llamado);
			printf("Color del turno: %s\n",OBT_STR_COLOR_TURNO);
			verMov(mov);
			for (j = 0 ; j < juego.indiceHJuego ; j++)
			{
				printf("%d. ", j+1); 
				verMov(juego.historicoJuego[j].mov);
				printf("\n");
			}
			mostrarTablero();
			imprimirBitTablero(juego.tablero[BLANCO][PEON]);
			printf("Inconsistencia en el peón blanco: %s\n", llamado);
			exit(0);
                }
       }
 
	if (juego.blancos != (juego.tablero[BLANCO][REY] | juego.tablero[BLANCO][DAMA] | juego.tablero[BLANCO][TORRE] | 
			      juego.tablero[BLANCO][ALFIL] | juego.tablero[BLANCO][CABALLO] | juego.tablero[BLANCO][PEON] ))
	{
		printf("Inconsistencia en el mapa de bits blanco: %s\n", llamado);
		printf("Color del turno: %s\n",OBT_STR_COLOR_TURNO);
		verMov(mov);
		for (j = 0 ; j < juego.indiceHJuego ; j++)
		{
			printf("%d. ", j+1); 
			verMov(juego.historicoJuego[j].mov);
			printf("\n");
		}
		mostrarTablero();
		printf("Inconsistencia en el mapa de bits blanco: %s\n", llamado);
		exit(0);
       }
 
  	if (juego.negros != (juego.tablero[NEGRO][REY] | juego.tablero[NEGRO][DAMA] | juego.tablero[NEGRO][TORRE] | 
			      juego.tablero[NEGRO][ALFIL] | juego.tablero[NEGRO][CABALLO] | juego.tablero[NEGRO][PEON] ))
	{
		printf("Inconsistencia en el mapa de bits negro: %s\n", llamado);
		printf("Color del turno: %s\n",OBT_STR_COLOR_TURNO);
		verMov(mov);
		for (j = 0 ; j < juego.indiceHJuego ; j++)
		{
			printf("%d. ", j+1); 
			verMov(juego.historicoJuego[j].mov);
			printf("\n");
		}
		mostrarTablero();
		printf("Inconsistencia en el mapa de bits negro: %s\n", llamado);
		exit(0);
       }

  	if (juego.ocupados != (juego.blancos  | juego.negros))
	{
		printf("Inconsistencia en el mapa de bits de ocupados: %s\n", llamado);
		printf("Color del turno: %s\n",OBT_STR_COLOR_TURNO);
		verMov(mov);
		for (j = 0 ; j < juego.indiceHJuego ; j++)
		{
			printf("%d. ", j+1); 
			verMov(juego.historicoJuego[j].mov);
			printf("\n");
		}
		mostrarTablero();
		printf("Inconsistencia en el mapa de bits de ocupados: %s\n", llamado);
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
		printf("Inconsistencia en el balance de material: %s\n", llamado);
		printf("material_total=%d\nmat=%d\n",juego.material_total,mat);
		printf("Color del turno: %s\n",OBT_STR_COLOR_TURNO);
		verMov(mov);
		for (j = 0 ; j < juego.indiceHJuego ; j++)
		{
			printf("%d. ", j+1); 
			verMov(juego.historicoJuego[j].mov);
			printf("\n");
		}
		mostrarTablero();
		printf("Inconsistencia en el balance de material: %s\n", llamado);
		printf("material_total=%d\nmat=%d\n",juego.material_total,mat);
		printf("Color del turno: %s\n",OBT_STR_COLOR_TURNO);
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
		printf("Inconsistencia en la llave hash: %s\n", llamado);
		printf("Color del turno: %s\n",OBT_STR_COLOR_TURNO);
		verMov(mov);
		for (j = 0 ; j < juego.indiceHJuego ; j++)
		{
			printf("%d. ", j+1); 
			verMov(juego.historicoJuego[j].mov);
			printf("\n");
		}
		mostrarTablero();
		printf("Inconsistencia en la llave hash: %s\n", llamado);
		exit(0);
	}
	

}

#endif
