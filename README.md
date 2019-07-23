# mango-paola-xboard

Código del Motor de ajedrez mango paola versión competencia entre motores o para interfaces que implementen el protocolo xboard

Blog oficial http://mangocomputerca.blogspot.com/
Ranking Mundial del Motor https://ccrl.chessdom.com/ccrl/4040/cgi/engine_details.cgi?print=Details&each_game=1&eng=Mango%20Paola%20Ajedrez%204.1#Mango_Paola_Ajedrez_4_1

**************************************************************************************************

Metodologías y tecnologías más importantes empleadas en el motor

* Búsqueda Alfa-Beta Negamax: Variación de búsqueda principal (algoritmo de búsqueda)
* Tabla de transposición (Técnica para acelerar la búsqueda)
* Poda por movimiento nulo (Podas de ramas menos prometedoras)
* Poda por movimiento tardío (Podas de ramas menos prometedoras)
* Poda por ventana mínima  (Podas de ramas menos prometedoras)
* Validación regla 50 movimientos (Podas por reglas propias del ajedrez)
* Tabla hash de movimientos variable (Técnica para acelerar la búsqueda)
* Estructura de datos del tablero por medio de tabla de bits (bitboard) 64 bits (Técnica de representación del tablero )
* Generación de movimientos legales por medio de aritmética de bits (Técnica para acelerar la búsqueda)
