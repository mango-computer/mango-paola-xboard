#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FALSO 0
#define VERDADERO 1
#define MEBIBYTE 1048576u
#define LIBRO3_SIN_ESTADO_MOTOR

int esActivoLibro = FALSO;
char rutaLibroPolyglot[512] = "";
unsigned int maxLibroPolyglotMB = 64;

#include "../libro3.c"

static int usage(void)
{
	fprintf(
		stderr,
		"uso: polyglot-harness hash FEN | probe BOOK SEED FEN | "
		"move RAW | stats BOOK MAX_MB\n"
	);
	return 2;
}

int main(int argc, char **argv)
{
	if (argc == 3 && !strcmp(argv[1], "hash")) {
		int ok = 0;
		uint64_t key = libro3HashFen(argv[2], &ok);
		if (!ok)
			return 1;
		printf("%016llx\n", (unsigned long long)key);
		return 0;
	}

	if (argc == 5 && !strcmp(argv[1], "probe")) {
		char move[6];
		uint32_t seed = (uint32_t)strtoul(argv[3], NULL, 10);
		int found;

		if (!abrirLibro3Ruta(argv[2]))
			return 1;
		found = buscarMovimientoLibro3ConSemilla(argv[4], move, seed);
		cerrarLibro3();
		if (found < 0)
			return 1;
		if (!found) {
			puts("-");
			return 0;
		}
		puts(move);
		return 0;
	}

	if (argc == 3 && !strcmp(argv[1], "move")) {
		char move[6];
		libro3MoveToUci((uint16_t)strtoul(argv[2], NULL, 0), move);
		puts(move);
		return 0;
	}

	if (argc == 4 && !strcmp(argv[1], "stats")) {
		maxLibroPolyglotMB = (unsigned int)strtoul(argv[3], NULL, 10);
		if (!abrirLibro3Ruta(argv[2]))
			return 1;
		printf(
			"%llu %zu\n",
			(unsigned long long)libro3Book.entries,
			libro3Book.bytes
		);
		cerrarLibro3();
		return 0;
	}

	return usage();
}
