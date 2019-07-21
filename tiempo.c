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

#ifndef TIEMPO_C
#define TIEMPO_C

void prepararTiempo()
{
	corriendoTiempo       	= FALSO;
	iniTiempo     		= 0;
	finTiempo      		= 0;
	detenerDeltaTiempo 	= 0;
}

void iniciarTiempo()
{

	if (!corriendoTiempo)
	{
		corriendoTiempo = VERDADERO;
		ftime(&iniBuffer);
		iniTiempo = iniBuffer.time * 1000 + iniBuffer.millitm + detenerDeltaTiempo;
	}
}

void detenerTiempo()
{
	if (corriendoTiempo)
	{
		corriendoTiempo = FALSO;
		ftime(&finBuffer);
		finTiempo 	   = finBuffer.time * 1000 + finBuffer.millitm;
		detenerDeltaTiempo = iniTiempo - finTiempo;
	}
}


void resetTiempo()
{
	if (corriendoTiempo)
	{
		ftime(&iniBuffer);
		iniTiempo = iniBuffer.time * 1000 + iniBuffer.millitm;
	} else {
		iniTiempo 	   = finTiempo;
		detenerDeltaTiempo = 0;
	}
}

void verTiempo()
{
	if (corriendoTiempo)
	{
		ftime(&actualBuffer);
		actualTiempo = actualBuffer.time * 1000 + actualBuffer.millitm;
		printf("%6.2f", (actualTiempo - iniTiempo)/1000.0);
	} else {
		printf("%6.2f", (finTiempo - iniTiempo)/1000.0);
	}
}

uint64 obt_msTiempo()
{
	if (corriendoTiempo)
	{
		ftime(&actualBuffer);
		actualTiempo = actualBuffer.time * 1000 + actualBuffer.millitm;
		return (actualTiempo - iniTiempo) ;
	} else {
		return (finTiempo - iniTiempo);
	}
}

uint64 obt_sysmsTiempo()
{
	ftime(&actualBuffer);
	return (actualBuffer.time * 1000 + actualBuffer.millitm);
}

#endif
