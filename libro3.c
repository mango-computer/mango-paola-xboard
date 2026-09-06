/*
 * Lector de libros de aperturas Polyglot para Mango AC.
 *
 * El formato consta de entradas big-endian de 16 bytes ordenadas por clave:
 * key (64), move (16), weight (16), learn (32).
 */

#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "polyglot_random.h"

#define LIBRO3_ENTRY_SIZE 16u
#define LIBRO3_MAX_MOVES 256

typedef struct {
	uint64_t key;
	uint16_t move;
	uint16_t weight;
	uint32_t learn;
} LIBRO3_ENTRY;

typedef struct {
	unsigned char *data;
	uint64_t entries;
	size_t bytes;
} LIBRO3_BOOK;

static LIBRO3_BOOK libro3Book = {NULL, 0, 0};
static uint32_t libro3RandomState = 1;

static uint16_t libro3ReadBE16(const unsigned char *p)
{
	return (uint16_t)(((uint16_t)p[0] << 8) | p[1]);
}

static uint32_t libro3ReadBE32(const unsigned char *p)
{
	return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) |
	       ((uint32_t)p[2] << 8) | (uint32_t)p[3];
}

static uint64_t libro3ReadBE64(const unsigned char *p)
{
	return ((uint64_t)p[0] << 56) | ((uint64_t)p[1] << 48) |
	       ((uint64_t)p[2] << 40) | ((uint64_t)p[3] << 32) |
	       ((uint64_t)p[4] << 24) | ((uint64_t)p[5] << 16) |
	       ((uint64_t)p[6] << 8) | (uint64_t)p[7];
}

static int libro3ReadEntry(uint64_t index, LIBRO3_ENTRY *entry)
{
	const unsigned char *bytes;

	if (!libro3Book.data || !entry || index >= libro3Book.entries)
		return 0;
	bytes = libro3Book.data + (size_t)index * LIBRO3_ENTRY_SIZE;

	entry->key = libro3ReadBE64(bytes);
	entry->move = libro3ReadBE16(bytes + 8);
	entry->weight = libro3ReadBE16(bytes + 10);
	entry->learn = libro3ReadBE32(bytes + 12);
	return 1;
}

void cerrarLibro3(void)
{
	free(libro3Book.data);
	libro3Book.data = NULL;
	libro3Book.entries = 0;
	libro3Book.bytes = 0;
	esActivoLibro = FALSO;
}

int abrirLibro3Ruta(const char *ruta)
{
	FILE *file;
	long size;
	size_t maxBytes;
	unsigned char *data;
	uint64_t i;
	uint64_t previousKey, currentKey;

	cerrarLibro3();
	if (!ruta || !ruta[0])
		return 0;
	if (!maxLibroPolyglotMB)
		return 0;
	maxBytes = (size_t)maxLibroPolyglotMB * MEBIBYTE;

	file = fopen(ruta, "rb");
	if (!file)
		return 0;
	if (fseek(file, 0, SEEK_END) != 0 ||
	    (size = ftell(file)) <= 0 ||
	    (size_t)size > maxBytes ||
	    (size_t)size % LIBRO3_ENTRY_SIZE != 0 ||
	    fseek(file, 0, SEEK_SET) != 0) {
		fclose(file);
		return 0;
	}

	data = (unsigned char *)malloc((size_t)size);
	if (!data) {
		fclose(file);
		return 0;
	}
	if (fread(data, (size_t)size, 1, file) != 1) {
		free(data);
		fclose(file);
		return 0;
	}
	fclose(file);

	previousKey = libro3ReadBE64(data);
	for (i = 1; i < (uint64_t)((size_t)size / LIBRO3_ENTRY_SIZE); i++) {
		currentKey = libro3ReadBE64(data + (size_t)i * LIBRO3_ENTRY_SIZE);
		if (currentKey < previousKey) {
			free(data);
			return 0;
		}
		previousKey = currentKey;
	}

	libro3Book.data = data;
	libro3Book.bytes = (size_t)size;
	libro3Book.entries = (uint64_t)(libro3Book.bytes / LIBRO3_ENTRY_SIZE);
	esActivoLibro = VERDADERO;
	return 1;
}

int abrirLibro3(void)
{
	uint32_t seed = (uint32_t)time(NULL) ^ (uint32_t)clock();
	libro3RandomState = seed ? seed : 1;
	return abrirLibro3Ruta(rutaLibroPolyglot);
}

static int libro3PieceIndex(char piece)
{
	switch (piece) {
		case 'p': return 0;
		case 'P': return 1;
		case 'n': return 2;
		case 'N': return 3;
		case 'b': return 4;
		case 'B': return 5;
		case 'r': return 6;
		case 'R': return 7;
		case 'q': return 8;
		case 'Q': return 9;
		case 'k': return 10;
		case 'K': return 11;
		default: return -1;
	}
}

uint64_t libro3HashFen(const char *fen, int *ok)
{
	char board[64] = {0};
	char turn, castling[8], ep[8];
	const char *cursor = fen;
	uint64_t hash = 0;
	int rank = 7, file = 0, parsed, square;

	if (ok)
		*ok = 0;
	if (!fen)
		return 0;

	while (*cursor && *cursor != ' ') {
		if (*cursor == '/') {
			if (file != 8 || rank == 0)
				return 0;
			rank--;
			file = 0;
		} else if (isdigit((unsigned char)*cursor)) {
			file += *cursor - '0';
			if (file > 8)
				return 0;
		} else {
			int piece = libro3PieceIndex(*cursor);
			if (piece < 0 || file >= 8)
				return 0;
			board[rank * 8 + file++] = *cursor;
		}
		cursor++;
	}
	if (rank != 0 || file != 8)
		return 0;

	parsed = sscanf(cursor, " %c %7s %7s", &turn, castling, ep);
	if (parsed != 3 || (turn != 'w' && turn != 'b'))
		return 0;

	for (square = 0; square < 64; square++) {
		int piece = libro3PieceIndex(board[square]);
		if (piece >= 0)
			hash ^= POLYGLOT_RANDOM[64 * piece + square];
	}
	if (strchr(castling, 'K')) hash ^= POLYGLOT_RANDOM[768];
	if (strchr(castling, 'Q')) hash ^= POLYGLOT_RANDOM[769];
	if (strchr(castling, 'k')) hash ^= POLYGLOT_RANDOM[770];
	if (strchr(castling, 'q')) hash ^= POLYGLOT_RANDOM[771];

	if (ep[0] >= 'a' && ep[0] <= 'h' && ep[1] >= '1' && ep[1] <= '8') {
		int epFile = ep[0] - 'a';
		int epRank = ep[1] - '1';
		int pawnRank = turn == 'w' ? epRank - 1 : epRank + 1;
		char pawn = turn == 'w' ? 'P' : 'p';
		int canCapture = 0;

		if (pawnRank >= 0 && pawnRank < 8) {
			if (epFile > 0 && board[pawnRank * 8 + epFile - 1] == pawn)
				canCapture = 1;
			if (epFile < 7 && board[pawnRank * 8 + epFile + 1] == pawn)
				canCapture = 1;
		}
		if (canCapture)
			hash ^= POLYGLOT_RANDOM[772 + epFile];
	}
	if (turn == 'w')
		hash ^= POLYGLOT_RANDOM[780];

	if (ok)
		*ok = 1;
	return hash;
}

#ifndef LIBRO3_SIN_ESTADO_MOTOR
static int libro3PieceIndexInterno(PIEZA piece)
{
	switch (piece) {
		case PEON_NEGRO: return 0;
		case PEON_BLANCO: return 1;
		case CABALLO_NEGRO: return 2;
		case CABALLO_BLANCO: return 3;
		case ALFIL_NEGRO: return 4;
		case ALFIL_BLANCO: return 5;
		case TORRE_NEGRO: return 6;
		case TORRE_BLANCO: return 7;
		case DAMA_NEGRO: return 8;
		case DAMA_BLANCO: return 9;
		case REY_NEGRO: return 10;
		case REY_BLANCO: return 11;
		default: return -1;
	}
}

uint64_t libro3HashJuego(int *ok)
{
	uint64_t hash = 0;
	int square;

	if (ok)
		*ok = 0;
	if (juego.colorTurno != BLANCO && juego.colorTurno != NEGRO)
		return 0;

	for (square = 0; square < 64; square++) {
		int piece = libro3PieceIndexInterno(ESCAQUES[square]);
		if (piece >= 0)
			hash ^= POLYGLOT_RANDOM[64 * piece + square];
	}
	if (juego.OOB) hash ^= POLYGLOT_RANDOM[768];
	if (juego.OOOB) hash ^= POLYGLOT_RANDOM[769];
	if (juego.OON) hash ^= POLYGLOT_RANDOM[770];
	if (juego.OOON) hash ^= POLYGLOT_RANDOM[771];

	if (juego.posPeonPaso < 64) {
		int epFile = juego.posPeonPaso & 7;
		int epRank = juego.posPeonPaso >> 3;
		int pawnRank = juego.colorTurno == BLANCO ? epRank - 1 : epRank + 1;
		PIEZA pawn = juego.colorTurno == BLANCO ? PEON_BLANCO : PEON_NEGRO;
		int canCapture = 0;

		if (pawnRank >= 0 && pawnRank < 8) {
			if (epFile > 0 &&
			    ESCAQUES[pawnRank * 8 + epFile - 1] == pawn)
				canCapture = 1;
			if (epFile < 7 &&
			    ESCAQUES[pawnRank * 8 + epFile + 1] == pawn)
				canCapture = 1;
		}
		if (canCapture)
			hash ^= POLYGLOT_RANDOM[772 + epFile];
	}
	if (juego.colorTurno == BLANCO)
		hash ^= POLYGLOT_RANDOM[780];

	if (ok)
		*ok = 1;
	return hash;
}
#endif

static int libro3Probe(uint64_t key, LIBRO3_ENTRY *entries, int capacity)
{
	uint64_t low = 0, high = libro3Book.entries;
	int count = 0;
	LIBRO3_ENTRY entry;

	if (!libro3Book.data || !entries || capacity <= 0)
		return 0;
	while (low < high) {
		uint64_t middle = low + (high - low) / 2;
		if (!libro3ReadEntry(middle, &entry))
			return -1;
		if (entry.key < key)
			low = middle + 1;
		else
			high = middle;
	}
	while (low < libro3Book.entries && count < capacity) {
		if (!libro3ReadEntry(low++, &entry))
			return -1;
		if (entry.key != key)
			break;
		entries[count++] = entry;
	}
	return count;
}

static int libro3Pick(const LIBRO3_ENTRY *entries, int count, uint32_t seed)
{
	uint64_t total = 0, selected;
	int i;

	if (!entries || count <= 0)
		return -1;
	for (i = 0; i < count; i++)
		total += entries[i].weight;
	if (!total)
		return (int)(seed % (uint32_t)count);

	selected = seed % total;
	for (i = 0; i < count; i++) {
		if (selected < entries[i].weight)
			return i;
		selected -= entries[i].weight;
	}
	return count - 1;
}

static void libro3MoveToUci(uint16_t move, char uci[6])
{
	static const char promotions[] = " nbrq";
	int to = move & 63;
	int from = (move >> 6) & 63;
	int promotion = (move >> 12) & 7;

	if (from == 4 && to == 7) to = 6;
	else if (from == 4 && to == 0) to = 2;
	else if (from == 60 && to == 63) to = 62;
	else if (from == 60 && to == 56) to = 58;

	uci[0] = (char)('a' + (from & 7));
	uci[1] = (char)('1' + (from >> 3));
	uci[2] = (char)('a' + (to & 7));
	uci[3] = (char)('1' + (to >> 3));
	if (promotion >= 1 && promotion <= 4) {
		uci[4] = promotions[promotion];
		uci[5] = '\0';
	} else {
		uci[4] = '\0';
	}
}

static int libro3BuscarClaveConSemilla(uint64_t key, char uci[6], uint32_t seed)
{
	LIBRO3_ENTRY entries[LIBRO3_MAX_MOVES];
	int count, selected;

	if (uci)
		uci[0] = '\0';
	if (!libro3Book.data || !uci)
		return 0;

	count = libro3Probe(key, entries, LIBRO3_MAX_MOVES);
	if (count <= 0)
		return count;
	selected = libro3Pick(entries, count, seed);
	if (selected < 0)
		return 0;

	libro3MoveToUci(entries[selected].move, uci);
	return 1;
}

int buscarMovimientoLibro3ConSemilla(const char *fen, char uci[6], uint32_t seed)
{
	uint64_t key;
	int ok;

	key = libro3HashFen(fen, &ok);
	if (!ok)
		return 0;
	return libro3BuscarClaveConSemilla(key, uci, seed);
}

#ifndef LIBRO3_SIN_ESTADO_MOTOR
int buscarMovimientoLibro3DesdeEstadoConSemilla(char uci[6], uint32_t seed)
{
	uint64_t key;
	int ok;

	key = libro3HashJuego(&ok);
	if (!ok)
		return 0;
	return libro3BuscarClaveConSemilla(key, uci, seed);
}
#endif

static uint32_t libro3SiguienteAleatorio(void)
{
	libro3RandomState ^= libro3RandomState << 13;
	libro3RandomState ^= libro3RandomState >> 17;
	libro3RandomState ^= libro3RandomState << 5;
	return libro3RandomState;
}

int buscarMovimientoLibro3(const char *fen, char uci[6])
{
	int result;

	result = buscarMovimientoLibro3ConSemilla(
		fen,
		uci,
		libro3SiguienteAleatorio()
	);
	if (result < 0)
		cerrarLibro3();
	return result;
}

#ifndef LIBRO3_SIN_ESTADO_MOTOR
int buscarMovimientoLibro3DesdeEstado(char uci[6])
{
	int result;

	result = buscarMovimientoLibro3DesdeEstadoConSemilla(
		uci,
		libro3SiguienteAleatorio()
	);
	if (result < 0)
		cerrarLibro3();
	return result;
}
#endif
