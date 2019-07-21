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

#ifndef ORDENAR_C
#define ORDENAR_C

//BT: Busquedad Tranquilidad 
#define	HM	juego.historicoMovMatadores

void actualizarMovMatadores(MOVIMIENTO mov, int valor, int capa)
{
	if (mov != HM[MATADOR_MOV][0][capa] && !ES_MOV_CAPTURA(mov) && !ES_MOV_PROMOCION(mov))
	{
		if (HM[MATADOR_VALOR][0][capa] < valor)
		{
			HM[MATADOR_VALOR][1][capa] 	= HM[MATADOR_VALOR][0][capa];
			HM[MATADOR_MOV][1][capa] 	= HM[MATADOR_MOV][0][capa];
	
			HM[MATADOR_VALOR][0][capa] 	= valor;
			HM[MATADOR_MOV][0][capa] 	= mov;

		} else 	if (HM[MATADOR_VALOR][1][capa] < valor) {

			HM[MATADOR_VALOR][1][capa] 	= valor;
			HM[MATADOR_MOV][1][capa]	= mov;
		}
	}
}

void ponderarMovimientos(int capa, MOVIMIENTO *hash_mov, int profundidad)
{
	int i = 0;

	PIEZA piezaCaptura;
	int valorOrigen; 
	int valorDestino;
	BOOLEANO esPV = siguePV; 

	if (siguePV && (profundidad > 1)) siguePV = FALSO;


	for (i = juego.Buffer_MOV_INDEXCAPAS[capa]; i < juego.Buffer_MOV_INDEXCAPAS[capa+1]; i++)
	{
		juego.Buffer_MOV_PONDERACION[i] = 0;

		//Movimiento Hash 
		if (juego.Buffer_MOV[i] == *hash_mov)
		{
			juego.Buffer_MOV_PONDERACION[i] += 10000000; //1000000;
			continue;
		}


		//Es PV
		if ((juego.Buffer_MOV[i] == juego.triangularArray[0][capa]) && (esPV && profundidad > 1))
		{
			juego.Buffer_MOV_PONDERACION[i] += 1000000; //10000000;
			siguePV = VERDADERO;
			continue;
		}

		//Promociones
		if (ES_MOV_PROMOCION(juego.Buffer_MOV[i]))
		{
			juego.Buffer_MOV_PONDERACION[i] += ((VALORPIEZA[OBT_MOV_PROMOCION(juego.Buffer_MOV[i])]*10) + 500000);
			continue;
		}

		//Valor de la pieza origen
		valorOrigen = VALORPIEZA[OBT_MOV_PIEZA(juego.Buffer_MOV[i])];

		//Capturas
		if (ES_MOV_CAPTURA(juego.Buffer_MOV[i]))
		{
			piezaCaptura	= OBT_MOV_CAPTURA(juego.Buffer_MOV[i]);
			valorDestino 	= VALORPIEZA[piezaCaptura];
			juego.Buffer_MOV_PONDERACION[i] +=  (((valorDestino*10) - valorOrigen) + 100000);
			continue;
		}
	
		//Historial
		if (juego.colorTurno) 
		{
			juego.Buffer_MOV_PONDERACION[i] += (10000 +
				juego.heuristicaNegra[OBT_MOV_ORIGEN(juego.Buffer_MOV[i])][OBT_MOV_DESTINO(juego.Buffer_MOV[i])]);
		} else {

			juego.Buffer_MOV_PONDERACION[i] += (10000 +
				juego.heuristicaBlanca[OBT_MOV_ORIGEN(juego.Buffer_MOV[i])][OBT_MOV_DESTINO(juego.Buffer_MOV[i])]);
		}

		//Movimientos Matadores
		if (HM[MATADOR_MOV][0][capa] == juego.Buffer_MOV[i])
		{
			juego.Buffer_MOV_PONDERACION[i] += 1000;

		} else if (HM[MATADOR_MOV][1][capa] == juego.Buffer_MOV[i]) {

			juego.Buffer_MOV_PONDERACION[i] += 500;

		} else if ((HM[MATADOR_MOV][0][capa-2] == juego.Buffer_MOV[i]) && (capa>2)) {

			juego.Buffer_MOV_PONDERACION[i] += 250;

		} else if ((HM[MATADOR_MOV][1][capa-2] == juego.Buffer_MOV[i]) && (capa>2)) {

			juego.Buffer_MOV_PONDERACION[i] += 150;
		}		
	}
}

void seleccionarMovimiento(int capa, int desde)
{
	int i			= 0;
	int mejorValor		= -1;  
	int mejorIndex		= desde;  
	MOVIMIENTO mejorMov 	= juego.Buffer_MOV[i];
	MOVIMIENTO movSalvado 	= 0;
	int valorSalvado 	= 0;
	BOOLEANO existe		= FALSO;

	for (i = desde; i < juego.Buffer_MOV_INDEXCAPAS[capa+1]; i++)
	{
		if (juego.Buffer_MOV_PONDERACION[i] > mejorValor) 
		{
			mejorValor = juego.Buffer_MOV_PONDERACION[i];
			mejorMov   = juego.Buffer_MOV[i];
			mejorIndex = i;
			existe     = VERDADERO;
		}
	}

	if (existe)
	{
		if (mejorIndex != desde)
		{
			movSalvado 				 = juego.Buffer_MOV[desde];
			valorSalvado 				 = juego.Buffer_MOV_PONDERACION[desde];

			juego.Buffer_MOV[desde]			 = mejorMov;
			juego.Buffer_MOV_PONDERACION[desde]	 = mejorValor;

			juego.Buffer_MOV[mejorIndex]		 = movSalvado;
			juego.Buffer_MOV_PONDERACION[mejorIndex] = valorSalvado;
		}
	}
}

#endif
