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
 
    // abrir el archivo para leer y escanear a través hasta encontrar la posición numero-ésima:
       fp=fopen(nombreArchivo, "rt");
       if (fp != NULL)
       {
              numerof = 0;
              while (fscanf(fp, "%s", s) != EOF) 
              {
                     if (!strcmp(s, "[White"))
                     {
                           int_fscanf = fscanf(fp, "%s", fenBlanco);
                           // quitar primero (") y los últimos dos caracteres ("]) de fenBlanco:
                           strcpy(temp, "");
                           strncat(temp, fenBlanco, strlen(fenBlanco)-2);
                           strcpy(temp, temp+1);
                           strcpy(fenBlanco, temp);
                     }
                     if (!strcmp(s, "[Black"))
                     {
                           int_fscanf = fscanf(fp, "%s", fenNegro);
                           // remove first (") and last two characters ("]) from fenNegro:
                           strcpy(temp, "");
                           strncat(temp, fenNegro, strlen(fenNegro)-2);
                           strcpy(temp, temp+1);
                           strcpy(fenNegro, temp);
                     }
                     if (!strcmp(s, "[FEN"))
                     {
                           // posición encontrada, por lo que incremento numerof.
                           // ya tenemos fenBlanco y fenNegro.
                           numerof++;
                           if (numerof == numero)
                           {
                                  int_fscanf = fscanf(fp, "%s", fen);
                                  int_fscanf = fscanf(fp, "%s", fencolor);           // b or w
                                  int_fscanf = fscanf(fp, "%s", fenEnroque);        // -, or KQkq
                                  int_fscanf = fscanf(fp, "%s", fenPeonPaso);       // -, or e3, or b6, etc
                                  int_fscanf = fscanf(fp, "%d", &fenReglaCincuentaMov);  // int, utilizado para la regla de mov50 empate 
                                  int_fscanf = fscanf(fp, "%d", &fenTotalMov); // int. comenzar con 1, se incrementa después del movimiento por Negro
 
				printf("Mango> fen # %d en %s:\n",numerof,nombreArchivo);
				printf("# Blanco: %s\n", fenBlanco);
				printf("# Negro : %s\n", fenNegro);
				printf(" %c\n", fen[1]);

                                  if (fencolor[0] == 'w')
                                  {
					printf("# blanco es siguiente movimiento\n");
                                  } else {
					printf("# negro es siguiente movimiento\n");
                                  }
				printf("# Enroque: %s\n", fenEnroque);
				printf("# Peon al Paso Escaque: %s\n", fenPeonPaso);
				printf("# Regla 50 mov: %d\n", fenReglaCincuentaMov);
				printf("# Total mov: %d\n", fenTotalMov);
                           }
                     }
              }
 
              if (numerof < numero)
              {
                     printf("Mango> sólo %d fens presentes en %s, fen #%d no encontrado\n",
                     numerof, nombreArchivo, numero);
                     valorRetorno = FALSO;
              } else {
                     setupFen(fen, fencolor, fenEnroque, fenPeonPaso, fenReglaCincuentaMov, fenTotalMov);
                     valorRetorno = VERDADERO;
              }
              fclose(fp);
       } else {
              printf("Mango> error abriendo: %s\n", nombreArchivo);
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

	juego.Buffer_MOV_INDEXCAPAS[1] = generarTodosMov(0);

}


/*convertimos una posición de tablero a formato FEN (epd) para poder consultar el libro de aperturas*/
void converTabler2FEN(char *string)
{
	int x, y, l=0, i=0, sq;
	char row[8];

	strcpy(string,"");

	for (y=0;y<8;y++) {
		i=l=0;
		strcpy(row,"");
		for (x=0;x<8;x++) {
			sq = (y*8)+x;
			if (ESCAQUES[ESPEJO[sq]] == VACIO) l++;
			else {
				if (l>0) 
				{
					row[i] = (char) (l+48);
					i++;
				}
				l=0;
				switch (ESCAQUES[ESPEJO[sq]]) 
				{
					case PEON_BLANCO 	: row[i]='P';break;
					case CABALLO_BLANCO 	: row[i]='N';break;
					case ALFIL_BLANCO	: row[i]='B';break;
					case TORRE_BLANCO	: row[i]='R';break;
					case DAMA_BLANCO	: row[i]='Q';break;
					case REY_BLANCO	 	: row[i]='K';break;

					case PEON_NEGRO	 	: row[i]='p';break;
					case CABALLO_NEGRO 	: row[i]='n';break;
					case ALFIL_NEGRO	: row[i]='b';break;
					case TORRE_NEGRO	: row[i]='r';break;
					case DAMA_NEGRO	 	: row[i]='q';break;
					case REY_NEGRO	 	: row[i]='k';break;
				}
				i++;
			}
		}
		if (l>0) {
			row[i] = (char) (l+48);
			i++;
		}
		strncat(string,row,i);
		if (y<7) strcat(string,"/");
	}

	if (!juego.colorTurno) strcat(string," w ");
	else strcat(string," b ");
}

#endif
