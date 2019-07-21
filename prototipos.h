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

#ifndef PROTOTIPOS_H
#define PROTOTIPOS_H

int LoadEgbbLibrary(char* main_path, int egbb_cache_size);
void unload_bitbases(void);
int probe_bitbases(int *score);
void cargaBitbases();
void cerrarBitbases();

int INITIALIZE(const char *Toerfile,const char *Bookfile);
int FIND_OPENING(const char *EPD,char *From,char *To, char *From1,char *From2,char *To1,char *To2,int *Bookmvs);
void CLOSE_BOOK(void);
void BOEK_BEGIN();
void MAKE_HASHKEY();
void BOEKIN();
void abrirLibro2();

void converTabler2FEN(char *string);
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
uint64 rand64();
void iniHashTablero();
int busquedadTranquilidad(int capa, int alfa, int beta);
void agregarMovCapPro(uint32 indexPrimero, uint32 *index);
int EET(MOVIMIENTO mov);
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

//**************** Libro **********************
void abrirLibro();
void cerrarLibro();
int obtMovLibro(FILE *libro);
BOOLEANO esIgualLibro(char *s1, char *s2);
//**************** tabla hash *****************
void agregarEvalTablaHash(int valor);
BOOLEANO verificarEvalTablaHash(int *valor);

int obtMayorValorPiezaDeBando(COLOR color);
int obtSegundoValorPiezaDeBando(COLOR color);


#endif




