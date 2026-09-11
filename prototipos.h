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

#ifndef PROTOTIPOS_H
#define PROTOTIPOS_H

#include <stddef.h>
#include <stdint.h>

int LoadEgbbLibrary(char* main_path, int egbb_cache_size);
void unload_bitbases(void);
int probe_bitbases(int *score);
void cargaBitbases();
void cerrarBitbases();

int abrirLibro3(void);
int abrirLibro3Ruta(const char *ruta);
void cerrarLibro3(void);
uint64_t libro3HashFen(const char *fen, int *ok);
uint64_t libro3HashJuego(int *ok);
int buscarMovimientoLibro3(const char *fen, char uci[6]);
int buscarMovimientoLibro3ConSemilla(const char *fen, char uci[6], uint32_t seed);
int buscarMovimientoLibro3DesdeEstado(char uci[6]);
int buscarMovimientoLibro3DesdeEstadoConSemilla(char uci[6], uint32_t seed);

int converTabler2FEN(char *string, size_t capacidad);
void depurarLibro(FILE *libro);
void limpiarAntesDeBusqueda();
void verMovimentosTodos();
BOOLEANO esMovimentoLegal(MOVIMIENTO m);
void ini_material();
void evalPeones(COLOR colorEval);
void evalCarreraPeonesPasados();
BOOLEANO evalOposicionReyes(COLOR colorEval);
BOOLEANO ReyPeonEscaque(int pawn, int king, int queen, int ptm);
void iniCarreraPeonesMask();
void evalParejaAlfiles();
void evalPiezasAtrapadas();
void iniPeonEscondido();
void iniPeonCC();
void evalFilaRey(COLOR colorEval);
void evalPeonesPasados(COLOR colorEval);
void ini_is_outside();
void evalEntorno(COLOR colorEval);
void evalRey(COLOR colorEval);
void iniMatrizSeguridadRey();
void iniMatrizMov();
void dobleAtaque7Ranks();
void evalParejaPiezas();
void evalEnroque();
void evalPosRey(COLOR colorEval);
void evalConteoPeones();
BOOLEANO esPosibilidadVictoria(COLOR colorEval);
void evaluarMate(COLOR colorEval);
int transformarSegunEmpate(int puedeGanar, int puntaje);
int ajustarReglaCincuenta(int puntaje);

void evalDesarrollo(COLOR colorEval);
void evalPeonesBloqueados(COLOR colorEval);
void evalPeonesProximoReyTactico(COLOR colorEval);
void evalPeonesPasadosCC(COLOR colorEval);
void evalTormentaEmpenio(COLOR colorEval);
void evalDamaPrematura(COLOR colorEval);

void evalCaballo(COLOR colorEval);
void evalAlfil(COLOR colorEval);
void evalTorre(COLOR colorEval);
void evalDama(COLOR colorEval);
void registrarAtaquesEval(COLOR colorEval, PIEZA tipo, uint64 ataques, int peso);
void registrarAtaquesPeonesEval(void);
void calcularMapasClavadas(void);
uint64 ataquesEfectivosEval(COLOR colorEval, int origen, uint64 ataques);
uint64 areaMovilidadUtil(COLOR colorEval, int origen, uint64 ataques);
int puntajeAmenazas(COLOR colorAtacante);
void evalInterCambioPiezas(COLOR colorEval);
void evalAfilMalo(COLOR colorEval);
void evalReyNegro();
void evalReyBlanco();
void evalRelacionMovilidad(COLOR colorEval);
int evalSeguridadRey(COLOR colorEval);

int bioskey();
BOOLEANO esAtacadoPorSR(uint64 bitmapDestino, const COLOR lado);
BOOLEANO esUnMovimiento(const char *l);
int obtEstadoJuego();
MOVIMIENTO parse_mov(char *s);
void xboard();
void uci();
void imprimirMovimiento(MOVIMIENTO mov);
void ponderarMovimientos(int capa, MOVIMIENTO *hash_mov, int profundidad);
void actualizarMovMatadores(MOVIMIENTO mov, int valor, int capa);
void mostrarMensajeFinalJuego();

void imprimirBitTablero(uint64 numero);
void imprimirBits(uint64 numero);
static inline int bitScanForwardBruijn(uint64 bb); 
static inline int convertir2lista(uint64 bb, uint8 *lista);
static inline uint32 cuentaBit(uint64 bitmap);
static inline uint32 bitScanLast(uint64 bitmap);

void inicializarTablero();
void inicializarVar();
unsigned int maxHashMBPermitido(void);
BOOLEANO calcularDimensionTablaHash(unsigned int mb, size_t *entradas, uint64 *mascara, size_t *bytes, unsigned int *mbReales);
BOOLEANO redimensionarTablaHashMB(unsigned int mb);
void limpiarTablasHash(void);
void aplicarCambioHashPendiente(void);
void actualizarTablerosUtil(COLOR color);
BOOLEANO esAtacadoPor(uint64 targetBitmap, const COLOR fromSide);
uint32 generarTodosMov(uint32 indice);
uint32 generarMovCapPro(uint32 indice);
BOOLEANO ejecutarComando(const char *buff);
void leerComandos();
void mostrarTablero();
BOOLEANO esValidoMovUsuario(const char *mov, MOVIMIENTO *m);
void iniTablerosUtil();
void verMovimentosLegales();
void verMovimentosJuego();
void hacerMovimiento(MOVIMIENTO mov);
void desHacerMovimiento(MOVIMIENTO mov);
void reconstruirEstadoEvaluacion(void);
void hacerCaptura(PIEZA captura, uint8 destino);
void desHacerCaptura(PIEZA captura, uint8 destino);
void hacerPromocion(PIEZA piezaPromocion, uint8 posPromocion);
void desHacerPromocion(PIEZA piezaPromocion, uint8 posPromocion);
void informacionVariables();
void prepararTiempo();
void iniciarTiempo();
void detenerTiempo();
void resetTiempo();
void verTiempo();
uint64 obt_msTiempo();
uint64 obt_sysmsTiempo();
uint64 perft(int ply, int depth);
void debugMov(char *llamado, MOVIMIENTO mov);
BOOLEANO readFen(char *nombreArchivo, int numero);
void setupFen(char *fen, char *fencolor, char *fenEnroque, char *fenPeonPaso, int fenReglaCincuentaMov, int fenTotalMov);
void iniciarTableroFEN();
void verMov(MOVIMIENTO mov);
int evaluacionTablero(int alfa, int beta);
MOVIMIENTO pensarRapido();
int alfabetaNegado(int capa, int profundidad, int alfa, int beta, BOOLEANO hacerNULL);
uint64 obtPiezasClavadasPor(uint8 escaque, const COLOR lado, uint64 mapaPosiblesPiezasClavadas, uint64 mapaPosiblesPiezasAtaX,uint64 mapaPosiblesPiezasAtaNoX);
void limpiarBufffer();
void seleccionarMovimiento(int capa, int desde);
void agregarMovTablaHash(int profundidad, int capa, int valor, int banderas, MOVIMIENTO mov);
int verificarTablaHash(int alfa, int beta, int capa, int profundidad, int *banderas, MOVIMIENTO *mov);
void cerrarTablas();
void seleccionarMov(int capa, int i, uint32 profundidad);
void iniciarHash();
void recalcularLlavePeones(void);
uint64 rand64();
void iniHashTablero();
int busquedadTranquilidad(int capa, int alfa, int beta);
void agregarMovCapPro(uint32 indexPrimero, uint32 *index);
int EET(MOVIMIENTO mov);
BOOLEANO EETSuperaUmbral(MOVIMIENTO mov, int umbral);
#ifdef PRUEBAS_HCE
int EETReferencia(MOVIMIENTO mov);
void reiniciarEstadisticasEET(void);
#endif
uint64 obtAtacantesDeEscaque(uint8 escaque);
uint64 obtProximosAtacantes(uint64 atacantes, uint64 noRemovidos, int destino, int direccion);
int EETPOS(uint8 origen, COLOR colorPrimerTurno);
BOOLEANO esEmpate(int alfa, int capa);
void nuevo_juego();
void test(char *entradaUsuario);
void verReloj_y_Teclado();
void verMov2(MOVIMIENTO mov);
int mov2alg(MOVIMIENTO mov, char *arraytmp);
char obtCharPieza(int pieza);
int obtTipoFinalJuego();
int contarMovLegales();

//**************** tabla hash *****************
void agregarEvalTablaHash(int valor);
BOOLEANO verificarEvalTablaHash(int *valor);
BOOLEANO cargarHashPeones(void);
void guardarHashPeones(const int *puntajeMAnterior, const int *puntajeFAnterior);

int obtMayorValorPiezaDeBando(COLOR color);
int obtSegundoValorPiezaDeBando(COLOR color);


#endif




