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

#ifndef FEN_C
#define FEN_C

BOOLEANO readFen(char *nombreArchivo, int numero)
{
       int numerof;
       char s[180];
       char fenBlanco[80];
       char fenNegro[80];
       char fen[100];
       char fencolor[2];     
       char fenEnroque[5];
       char fenPeonPaso[3];
       char temp[80];
       int fenReglaCincuentaMov;
       int fenTotalMov;
       BOOLEANO valorRetorno;
       FILE * fp;
       int int_fscanf;
 
       valorRetorno = FALSO;
       if (numero <= 0) return valorRetorno;
 
    // Abrir el archivo y recorrer las posiciones hasta encontrar la número n:
       fp=fopen(nombreArchivo, "rt");
       if (fp != NULL)
       {
              numerof = 0;
              while (fscanf(fp, "%s", s) != EOF) 
              {
                     if (!strcmp(s, "[White"))
                     {
                           int_fscanf = fscanf(fp, "%s", fenBlanco);
                           // Quitar el primer (") y los dos últimos caracteres ("]) de fenBlanco:
                           strcpy(temp, "");
                           strncat(temp, fenBlanco, strlen(fenBlanco)-2);
                           strcpy(temp, temp+1);
                           strcpy(fenBlanco, temp);
                     }
                     if (!strcmp(s, "[Black"))
                     {
                           int_fscanf = fscanf(fp, "%s", fenNegro);
                           // Quitar el primer (") y los dos últimos caracteres ("]) de fenNegro:
                           strcpy(temp, "");
                           strncat(temp, fenNegro, strlen(fenNegro)-2);
                           strcpy(temp, temp+1);
                           strcpy(fenNegro, temp);
                     }
                     if (!strcmp(s, "[FEN"))
                     {
                           // Posición encontrada; se incrementa numerof.
                           // Ya tenemos fenBlanco y fenNegro.
                           numerof++;
                           if (numerof == numero)
                           {
                                  int_fscanf = fscanf(fp, "%s", fen);
                                  int_fscanf = fscanf(fp, "%s", fencolor);           // b or w
                                  int_fscanf = fscanf(fp, "%s", fenEnroque);        // -, or KQkq
                                  int_fscanf = fscanf(fp, "%s", fenPeonPaso);       // -, or e3, or b6, etc
                                  int_fscanf = fscanf(fp, "%d", &fenReglaCincuentaMov);  // Entero usado para la regla de los 50 movimientos 
                                  int_fscanf = fscanf(fp, "%d", &fenTotalMov); // Entero. Empieza en 1 y se incrementa tras el movimiento de las negras
 
				printf("Mango> fen # %d en %s:\n",numerof,nombreArchivo);
				printf("# Blanco: %s\n", fenBlanco);
				printf("# Negro : %s\n", fenNegro);
				printf(" %c\n", fen[1]);

                                  if (fencolor[0] == 'w')
                                  {
					printf("# Juegan las blancas\n");
                                  } else {
					printf("# Juegan las negras\n");
                                  }
				printf("# Enroque: %s\n", fenEnroque);
				printf("# Escaque de peón al paso: %s\n", fenPeonPaso);
				printf("# Regla de 50 movimientos: %d\n", fenReglaCincuentaMov);
				printf("# Total de movimientos: %d\n", fenTotalMov);
                           }
                     }
              }
 
              if (numerof < numero)
              {
                     printf("Mango> solo hay %d FEN en %s; no se encontró el FEN n.º %d\n",
                     numerof, nombreArchivo, numero);
                     valorRetorno = FALSO;
              } else {
                     setupFen(fen, fencolor, fenEnroque, fenPeonPaso, fenReglaCincuentaMov, fenTotalMov);
                     valorRetorno = VERDADERO;
              }
              fclose(fp);
       } else {
              printf("Mango> error al abrir: %s\n", nombreArchivo);
              valorRetorno = FALSO;
       }
       return valorRetorno;
}
 
void setupFen(char *fen, char *fencolor, char *fenEnroque, char *fenPeonPaso, int fenReglaCincuentaMov, int fenTotalMov)
{

	int i, file, rank, contador;
	PIEZA pieza;
	juego.llaveHash ^= juego.llaveHash;

 
       pieza = 0;
       for (i = 0; i < 64; i++)
       {
              ESCAQUES[i] = VACIO;
       }
 
       file = 1;
       rank = 8;
       i = 0;
       contador = 0;
       while ((contador < 64) && (fen[i] != '\0'))
       {
              // '1'  through '8':
              if (((int) fen[i] > 48) && ((int) fen[i] < 57))
              {
                     file+= (int) fen[i] - 48;
                     contador+= (int) fen[i] - 48;
              } else {
                     switch (fen[i])
                     {
                           case '/':
                                  rank--;
                                  file = 1;
                                  break;
 
                           case 'P':
                                  ESCAQUES[FILERANK2POS[file][rank]] = PEON_BLANCO;
				  juego.llaveHash ^= arrayHash.llaves[FILERANK2POS[file][rank]][PEON_BLANCO];
                                  file += 1;
                                  contador += 1;
                                  break;
 
                           case 'N':
                                  ESCAQUES[FILERANK2POS[file][rank]] = CABALLO_BLANCO;
				  juego.llaveHash ^= arrayHash.llaves[FILERANK2POS[file][rank]][CABALLO_BLANCO];
                                  file += 1;
                                  contador += 1;
                                  break;
 
                           case 'B':
                                  ESCAQUES[FILERANK2POS[file][rank]] = ALFIL_BLANCO;
				  juego.llaveHash ^= arrayHash.llaves[FILERANK2POS[file][rank]][ALFIL_BLANCO];
                                  file += 1;
                                  contador += 1;
                                  break;
 
                           case 'R':
                                  ESCAQUES[FILERANK2POS[file][rank]] = TORRE_BLANCO;
				  juego.llaveHash ^= arrayHash.llaves[FILERANK2POS[file][rank]][TORRE_BLANCO];
                                  file += 1;
                                  contador += 1;
                                  break;
 
                           case 'Q':
                                  ESCAQUES[FILERANK2POS[file][rank]] = DAMA_BLANCO;
				  juego.llaveHash ^= arrayHash.llaves[FILERANK2POS[file][rank]][DAMA_BLANCO];
                                  file += 1;
                                  contador += 1;
                                  break;
 
                           case 'K':
                                  ESCAQUES[FILERANK2POS[file][rank]] = REY_BLANCO;
				  juego.llaveHash ^= arrayHash.llaves[FILERANK2POS[file][rank]][REY_BLANCO];
                                  file += 1;
                                  contador += 1;
                                  break;
 
                           case 'p':
                                  ESCAQUES[FILERANK2POS[file][rank]] = PEON_NEGRO;
				  juego.llaveHash ^= arrayHash.llaves[FILERANK2POS[file][rank]][PEON_NEGRO];
                                  file += 1;
                                  contador += 1;
                                  break;
 
                           case 'n':
                                  ESCAQUES[FILERANK2POS[file][rank]] = CABALLO_NEGRO;
				  juego.llaveHash ^= arrayHash.llaves[FILERANK2POS[file][rank]][CABALLO_NEGRO];
                                  file += 1;
                                  contador += 1;
                                  break;
 
                           case 'b':
                                  ESCAQUES[FILERANK2POS[file][rank]] = ALFIL_NEGRO;
				  juego.llaveHash ^= arrayHash.llaves[FILERANK2POS[file][rank]][ALFIL_NEGRO];
                                  file += 1;
                                  contador += 1;
                                  break;
 
                           case 'r':
                                  ESCAQUES[FILERANK2POS[file][rank]] = TORRE_NEGRO;
				  juego.llaveHash ^= arrayHash.llaves[FILERANK2POS[file][rank]][TORRE_NEGRO];
                                  file += 1;
                                  contador += 1;
                                  break;
 
                           case 'q':
                                  ESCAQUES[FILERANK2POS[file][rank]] = DAMA_NEGRO;
				  juego.llaveHash ^= arrayHash.llaves[FILERANK2POS[file][rank]][DAMA_NEGRO];
                                  file += 1;
                                  contador += 1;
                                  break;
 
                           case 'k':
                                  ESCAQUES[FILERANK2POS[file][rank]] = REY_NEGRO;
				  juego.llaveHash ^= arrayHash.llaves[FILERANK2POS[file][rank]][REY_NEGRO];
                                  file += 1;
                                  contador += 1;
                                  break;
 
                           default:
                                  break;
                     }
              }
              i++;
       }

        if (fencolor[0] == 'b')
	{
		juego.colorTurno = NEGRO;
		juego.llaveHash ^= arrayHash.lado;
	} else {
		juego.colorTurno = BLANCO;
	} 
 
 	if (strstr(fenEnroque, "K")) 
	{ 
		juego.OOB = VERDADERO;
		juego.llaveHash ^= arrayHash.OOB;

	} else {
		juego.OOB = FALSO;
	}
        if (strstr(fenEnroque, "Q")) 
	{ 
		juego.OOOB = VERDADERO;
		juego.llaveHash ^= arrayHash.OOOB;
	} else {
		juego.OOOB = FALSO;
	}

        if (strstr(fenEnroque, "k"))
	{ 
		juego.OON = VERDADERO;
		juego.llaveHash ^= arrayHash.OON;
	} else {
		juego.OON = FALSO;
	}

        if (strstr(fenEnroque, "q")) 
	{ 
		juego.OOON = VERDADERO;
		juego.llaveHash ^= arrayHash.OOON;
	} else {
		juego.OOON = FALSO;
	}

       if (strstr(fenPeonPaso, "-"))
       {
              juego.posPeonPaso = SIN_POS_VALIDA;
       } else {
              juego.posPeonPaso = ((int) fenPeonPaso[0] - 96) + 8 * ((int) fenPeonPaso[1] - 48) - 9;
	      juego.llaveHash ^= arrayHash.ep[juego.posPeonPaso];
       }

	

	juego.reglaCincuentaMov = fenReglaCincuentaMov;
	juego.totalMov		= fenTotalMov;

	iniciarTableroFEN();
	recalcularLlavePeones();

	juego.Buffer_MOV_INDEXCAPAS[1] = generarTodosMov(0);

}


/* Convierte el estado interno en un FEN completo apto para hash Polyglot. */
int converTabler2FEN(char *string, size_t capacidad)
{
	char piezas[80];
	char enroques[5];
	const char *peonPaso;
	size_t usados = 0;
	int x, y, vacios, sq, nEnroques = 0;
	int escritos;

	if (!string || capacidad == 0)
		return 0;
	string[0] = '\0';

	for (y = 0; y < 8; y++) {
		vacios = 0;
		for (x = 0; x < 8; x++) {
			char pieza = '\0';
			sq = (y * 8) + x;

			switch (ESCAQUES[ESPEJO[sq]]) {
				case PEON_BLANCO: pieza = 'P'; break;
				case CABALLO_BLANCO: pieza = 'N'; break;
				case ALFIL_BLANCO: pieza = 'B'; break;
				case TORRE_BLANCO: pieza = 'R'; break;
				case DAMA_BLANCO: pieza = 'Q'; break;
				case REY_BLANCO: pieza = 'K'; break;
				case PEON_NEGRO: pieza = 'p'; break;
				case CABALLO_NEGRO: pieza = 'n'; break;
				case ALFIL_NEGRO: pieza = 'b'; break;
				case TORRE_NEGRO: pieza = 'r'; break;
				case DAMA_NEGRO: pieza = 'q'; break;
				case REY_NEGRO: pieza = 'k'; break;
				default: vacios++; break;
			}

			if (pieza) {
				if (vacios)
					piezas[usados++] = (char)('0' + vacios);
				piezas[usados++] = pieza;
				vacios = 0;
			}
		}
		if (vacios)
			piezas[usados++] = (char)('0' + vacios);
		if (y < 7)
			piezas[usados++] = '/';
	}
	piezas[usados] = '\0';

	if (juego.OOB) enroques[nEnroques++] = 'K';
	if (juego.OOOB) enroques[nEnroques++] = 'Q';
	if (juego.OON) enroques[nEnroques++] = 'k';
	if (juego.OOON) enroques[nEnroques++] = 'q';
	if (!nEnroques) enroques[nEnroques++] = '-';
	enroques[nEnroques] = '\0';

	peonPaso = juego.posPeonPaso < 64
		? NOMBRE_ESCAQUES[juego.posPeonPaso]
		: "-";
	escritos = snprintf(
		string,
		capacidad,
		"%s %c %s %s %u %u",
		piezas,
		juego.colorTurno == BLANCO ? 'w' : 'b',
		enroques,
		peonPaso,
		(unsigned int)juego.reglaCincuentaMov,
		(unsigned int)(juego.totalMov / 2 + 1)
	);
	if (escritos < 0 || (size_t)escritos >= capacidad) {
		string[0] = '\0';
		return 0;
	}
	return 1;
}

#endif
