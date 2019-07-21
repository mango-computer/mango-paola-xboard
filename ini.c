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

#ifndef INI_C
#define INI_C


void iniMatrizMov()
{
	uint64 destinos=0;
	uint64 ocupados=0;

	int i=0;

	for (;i<64;i++)
	{
		ocupados = BITSET[i];
		destinos = ~ocupados;

		MATRIZ_MOV_ALFIL[i] = (DIAGA8H1_ATTACKS[i][((ocupados & DIAGA8H1MASK[i]) * DIAGA8H1MAGIC[(i)]) >> 57] & destinos) |
				      (DIAGA1H8_ATTACKS[i][((ocupados & DIAGA1H8MASK[i]) * DIAGA1H8MAGIC[(i)]) >> 57] & destinos);
	}

	for (i=0;i<64;i++)
	{
		ocupados = BITSET[i];
		destinos = ~ocupados;

		MATRIZ_MOV_TORRE[i] = (RANK_ATTACKS[i][((ocupados & RANKMASK[i]) >> RANKSHIFT[i])] & destinos) |
				      (FILE_ATTACKS[i][((ocupados & FILEMASK[i]) * FILEMAGIC[i]) >> 57] & destinos);
	}
}

void iniMatrizSeguridadRey()
{
	int seguridad, tropismo;

	for (seguridad = 0; seguridad < 16; seguridad++)
	{
		for (tropismo = 0; tropismo < 16; tropismo++) 
		{
			MATRIZ_SEGURIDAD_REY[seguridad][tropismo] = 
			180 * ((VECTOR_SEGURIDAD[seguridad] + 100) * (VECTOR_TROPISMO[tropismo] + 100) / 100 - 100) / 100;
		}
	}
}

void ini_is_outside()
{
int i,j;

  msb_8bit[0] = 8;
  lsb_8bit[0] = 8;
  pop_cnt_8bit[0] = 0;
  for (i = 1; i < 256; i++) {
    pop_cnt_8bit[i] = 0;
    for (j = 0; j < 8; j++)
      if (i & (1 << j))
        pop_cnt_8bit[i]++;
    lsb_8bit[i] = 8;
    for (j = 0; j < 8; j++) {
      if (i & (1 << j)) {
        msb_8bit[i] = j;
        if (lsb_8bit[i] == 8)
          lsb_8bit[i] = j;
      }
    }
  }



for (i = 0; i < 256; i++) {
    for (j = 0; j < 256; j++) {
      int ppsq1, ppsq2, psql, psqr;

      is_outside[i][j] = 0;
      ppsq1 = lsb_8bit[i];
      if (ppsq1 < 8) {
        psql = lsb_8bit[j];
        if (ppsq1 < psql - 1 || psql == 8)
          is_outside[i][j] += 1;
      }
      ppsq2 = msb_8bit[i];
      if (ppsq2 < 8) {
        psqr = msb_8bit[j];
        if (ppsq2 > psqr + 1 || psqr == 8)
          is_outside[i][j] += 1;
      }
      if (ppsq1 == ppsq2 && is_outside[i][j] > 0)
        is_outside[i][j] = 1;
    }
  }
}

void iniPeonCC()
{
	int i;
	memset(MASCARA_PEON_CC, 0, sizeof(uint64)*64);

  for (i = 8; i < 56; i++) {
    if (FILES[i] > 1 && FILES[i] < 8)
      MASCARA_PEON_CC[i] = BITSET[i - 1] | BITSET[i + 1] | BITSET[i - 9] | BITSET[i - 7] | BITSET[i + 7] | BITSET[i + 9];
    else if (FILES[i] == 1)
      MASCARA_PEON_CC[i] = BITSET[i + 1] | BITSET[i - 7] | BITSET[i + 9];
    else if (FILES[i] == 8)
      MASCARA_PEON_CC[i] = BITSET[i - 1] | BITSET[i - 9] | BITSET[i + 7];
  }
}

void iniPeonEscondido()
{

int i;
int pos;

    PEON_ESCONDIDO_I[NEGRO][0] 	= 0;
    PEON_ESCONDIDO_D[NEGRO][0] 	= 0;
    PEON_ESCONDIDO_I[BLANCO][0] = 0;
    PEON_ESCONDIDO_D[BLANCO][0] = 0;


for (i = 0; i < 8; i++) {
pos = i+1;
    PEON_ESCONDIDO_I[NEGRO][pos] 	= 0;
    PEON_ESCONDIDO_D[NEGRO][pos] 	= 0;
    PEON_ESCONDIDO_I[BLANCO][pos] 	= 0;
    PEON_ESCONDIDO_D[BLANCO][pos] 	= 0;
    if (i > 0) {
      PEON_ESCONDIDO_I[BLANCO][pos] |= BITSET[39 + i] | BITSET[47 + i];
      PEON_ESCONDIDO_I[NEGRO][pos] |= BITSET[15 + i] | BITSET[7 + i];
    }
    if (i > 1) {
      PEON_ESCONDIDO_I[BLANCO][pos] |= BITSET[46 + i] | BITSET[38 + i];
      PEON_ESCONDIDO_I[NEGRO][pos] |= BITSET[6 + i] | BITSET[14 + i];
    }
    if (i < 6) {
      PEON_ESCONDIDO_D[BLANCO][pos] |= BITSET[50 + i] | BITSET[42 + i];
      PEON_ESCONDIDO_D[NEGRO][pos] |= BITSET[10 + i] | BITSET[18 + i];
    }
    if (i < 7) {
      PEON_ESCONDIDO_D[BLANCO][pos] |= BITSET[41 + i] | BITSET[49 + i];
      PEON_ESCONDIDO_D[NEGRO][pos] |= BITSET[17 + i] | BITSET[9 + i];
    }
  }
}

void iniCarreraPeonesMask()
{

int i,j;

 for (i = 0; i < 64; i++) {
    CARRERA_PEONES[BLANCO][BLANCO][i] = 0;
    CARRERA_PEONES[BLANCO][NEGRO][i]  = 0;
    CARRERA_PEONES[NEGRO][BLANCO][i]  = 0;
    CARRERA_PEONES[NEGRO][NEGRO][i]   = 0;
  }
  for (j = 8; j < 56; j++) {
    for (i = 0; i < 64; i++) {
/* BLANCO pawn, wtm */
      if (j < 16) {
        if (ReyPeonEscaque(j + 8, i, FILES[j] + 55, 1))
          CARRERA_PEONES[BLANCO][BLANCO][j] |= BITSET[i];
      } else {
        if (ReyPeonEscaque(j, i, FILES[j] + 55, 1))
          CARRERA_PEONES[BLANCO][BLANCO][j] |= BITSET[i];
      }
/* BLANCO pawn, btm */
      if (j < 16) {
        if (ReyPeonEscaque(j + 8, i, FILES[j] + 55, 0))
          CARRERA_PEONES[BLANCO][NEGRO][j] |= BITSET[i];
      } else {
        if (ReyPeonEscaque(j, i, FILES[j] + 55, 0))
          CARRERA_PEONES[BLANCO][NEGRO][j] |= BITSET[i];
      }
/* NEGRO pawn, wtm */
      if (j > 47) {
        if (ReyPeonEscaque(j - 8, i, FILES[j]-1, 0))
          CARRERA_PEONES[NEGRO][BLANCO][j] |= BITSET[i];
      } else {
        if (ReyPeonEscaque(j, i, FILES[j]-1, 0))
          CARRERA_PEONES[NEGRO][BLANCO][j] |= BITSET[i];
      }
/* NEGRO pawn, btm */
      if (j > 47) {
        if (ReyPeonEscaque(j - 8, i, FILES[j]-1, 1))
          CARRERA_PEONES[NEGRO][NEGRO][j] |= BITSET[i];
      } else {
        if (ReyPeonEscaque(j, i, FILES[j]-1, 1))
          CARRERA_PEONES[NEGRO][NEGRO][j] |= BITSET[i];
      }
    }
  }
}

void inicializarTablero()
{

	memcpy(ESCAQUES, ESCAQUES_INI, sizeof(ESCAQUES));

	juego.tablero[BLANCO][PEON] 	= 0xff00ull;
	juego.tablero[BLANCO][TORRE] 	= 0x81ull;
	juego.tablero[BLANCO][CABALLO] 	= 0x42ull;
	juego.tablero[BLANCO][ALFIL] 	= 0x24ull;
	juego.tablero[BLANCO][DAMA]  	= 0x8ull;
	juego.tablero[BLANCO][REY]  	= 0x10ull;

	juego.tablero[NEGRO][PEON] 	= 0xff000000000000ull;
	juego.tablero[NEGRO][TORRE] 	= 0x8100000000000000ull;
	juego.tablero[NEGRO][CABALLO] 	= 0x4200000000000000ull;
	juego.tablero[NEGRO][ALFIL] 	= 0x2400000000000000ull;
	juego.tablero[NEGRO][DAMA]  	= 0x800000000000000ull;
	juego.tablero[NEGRO][REY]  	= 0x1000000000000000ull;

}

void iniHashTablero()
{

	juego.llaveHash ^= juego.llaveHash;
	//Blancas
	juego.llaveHash ^= arrayHash.llaves[0][TORRE_BLANCO];
	juego.llaveHash ^= arrayHash.llaves[1][CABALLO_BLANCO];
	juego.llaveHash ^= arrayHash.llaves[2][ALFIL_BLANCO];
	juego.llaveHash ^= arrayHash.llaves[3][DAMA_BLANCO];
	juego.llaveHash ^= arrayHash.llaves[4][REY_BLANCO];
	juego.llaveHash ^= arrayHash.llaves[5][ALFIL_BLANCO];
	juego.llaveHash ^= arrayHash.llaves[6][CABALLO_BLANCO];
	juego.llaveHash ^= arrayHash.llaves[7][TORRE_BLANCO];
	juego.llaveHash ^= arrayHash.llaves[8][PEON_BLANCO];
	juego.llaveHash ^= arrayHash.llaves[9][PEON_BLANCO];
	juego.llaveHash ^= arrayHash.llaves[10][PEON_BLANCO];
	juego.llaveHash ^= arrayHash.llaves[11][PEON_BLANCO];
	juego.llaveHash ^= arrayHash.llaves[12][PEON_BLANCO];
	juego.llaveHash ^= arrayHash.llaves[13][PEON_BLANCO];
	juego.llaveHash ^= arrayHash.llaves[14][PEON_BLANCO];
	juego.llaveHash ^= arrayHash.llaves[15][PEON_BLANCO];
	//Negras
	juego.llaveHash ^= arrayHash.llaves[56][TORRE_NEGRO];
	juego.llaveHash ^= arrayHash.llaves[57][CABALLO_NEGRO];
	juego.llaveHash ^= arrayHash.llaves[58][ALFIL_NEGRO];
	juego.llaveHash ^= arrayHash.llaves[59][DAMA_NEGRO];
	juego.llaveHash ^= arrayHash.llaves[60][REY_NEGRO];
	juego.llaveHash ^= arrayHash.llaves[61][ALFIL_NEGRO];
	juego.llaveHash ^= arrayHash.llaves[62][CABALLO_NEGRO];
	juego.llaveHash ^= arrayHash.llaves[63][TORRE_NEGRO];
	juego.llaveHash ^= arrayHash.llaves[48][PEON_NEGRO];
	juego.llaveHash ^= arrayHash.llaves[49][PEON_NEGRO];
	juego.llaveHash ^= arrayHash.llaves[50][PEON_NEGRO];
	juego.llaveHash ^= arrayHash.llaves[51][PEON_NEGRO];
	juego.llaveHash ^= arrayHash.llaves[52][PEON_NEGRO];
	juego.llaveHash ^= arrayHash.llaves[53][PEON_NEGRO];
	juego.llaveHash ^= arrayHash.llaves[54][PEON_NEGRO];
	juego.llaveHash ^= arrayHash.llaves[55][PEON_NEGRO];

	juego.llaveHash ^= arrayHash.OOB;
	juego.llaveHash ^= arrayHash.OOOB;
	juego.llaveHash ^= arrayHash.OON;
	juego.llaveHash ^= arrayHash.OOON;
}

void iniTablerosUtil()
{
	juego.blancos		= juego.tablero[BLANCO][PEON]    | juego.tablero[BLANCO][TORRE] | 
				  juego.tablero[BLANCO][CABALLO] | juego.tablero[BLANCO][ALFIL] | 
				  juego.tablero[BLANCO][DAMA]    | juego.tablero[BLANCO][REY];

	juego.negros		= juego.tablero[NEGRO][PEON]     | juego.tablero[NEGRO][TORRE] | 
				  juego.tablero[NEGRO][CABALLO]  | juego.tablero[NEGRO][ALFIL] | 
				  juego.tablero[NEGRO][DAMA]   	 | juego.tablero[NEGRO][REY];

	juego.ocupados		= juego.blancos  | juego.negros;

}

void actualizarTablerosUtil(COLOR color)
{
	juego.desOcupados	= ~juego.ocupados;

	if (color) //Negro
	{
		juego.destinos	= juego.desOcupados | juego.blancos;
	
	} else {
		juego.destinos	= juego.desOcupados | juego.negros;
	}
}
void inicializarVar()
{
	PROMOCION[BLANCO] 		= DAMA_BLANCO;
	PROMOCION[NEGRO]  		= DAMA_NEGRO;

	juego.OOB 			= VERDADERO;
	juego.OOOB 			= VERDADERO;
	juego.OON 			= VERDADERO;
	juego.OOON 			= VERDADERO;
	juego.ENROQUEB 			= SIN_ENROQUE;
	juego.ENROQUEN 			= SIN_ENROQUE;
	juego.posPeonPaso		= SIN_POS_VALIDA;
	juego.reglaCincuentaMov		= 0;
	juego.totalMov			= 0;
	juego.indiceHJuego		= 0;
	juego.vista			= BLANCO;
	juego.colorTurno		= BLANCO;

	juego.material_total		= 0;
	juego.material_lado_blanco	= (VALOR_TORRE*2) + (VALOR_CABALLO*2) + (VALOR_ALFIL*2) + VALOR_DAMA;
	juego.material_lado_negro	= juego.material_lado_blanco; 
	juego.material_peon_blanco	= (VALOR_PEON*8);	
	juego.material_peon_negro	= juego.material_peon_blanco;

	juego.profundidadBusquedad	= 98;
	juego.maxTiempo			= 2000;

	statusFinJuego			= STATUS_FIN_JUEGO_SIN_ESTATUS;
	nFueraLibro			= 0;
	
	limpiarBufffer();
	memset(juego.historicoJuego, 0, sizeof(DATAJUEGO)*MAX_BUFF_MOV);

	if (hash_eval==NULL)
	{
		hash_eval = (HASH_EVAL*)malloc((LARGO_HASH_EVAL+1) * sizeof(HASH_EVAL));
	}

	if (hash_eval == NULL)
	{
		printf("#Tabla Hash EVAL no ha podido ser creada\n");
	} else {
		memset(hash_eval, 0, (LARGO_HASH_EVAL+1)*sizeof(HASH_EVAL));
		if (!primeraVez)
		{
			printf("#Tabla hash EVAL        [OK] - Tamanio: %d MB - Largo: %d\n",
					((LARGO_HASH_EVAL+1)*sizeof(HASH_EVAL))/1048576,LARGO_HASH_EVAL);
		}
	}


	if (esUsoTablaHash)
	{
		if (tabla_hash == NULL)
		{
			tabla_hash = (REGISTRO_TABLA_HASH*)malloc((LARGO_TABLA_HASH+1) * sizeof(REGISTRO_TABLA_HASH));
		}

		if (tabla_hash == NULL)
		{
			printf("#Tabla Hash no ha podido ser creada\n");
		} else {
			memset(tabla_hash, 0, (LARGO_TABLA_HASH+1) * sizeof(REGISTRO_TABLA_HASH));
			if (!primeraVez)
			{
				printf("#Tabla hash MOVIMIENTOS [OK] - Tamanio: %d MB - Largo: %d\n#\n",
						((LARGO_TABLA_HASH+1)*sizeof(REGISTRO_TABLA_HASH))/1048576,LARGO_TABLA_HASH);
			}
		}
	}

	primeraVez = VERDADERO;
}



void cerrarTablas()
{
	if (hash_eval)
	{
		free(hash_eval);
	}

	if (tabla_hash)
	{
		free(tabla_hash);
	}

}

#endif
