#ifndef JUEGO_H
#define JUEGO_H

#include <windows.h>
#include <time.h> 

// --- CONFIGURACIÓN ---
#define ANCHO_MUNDO 3000
#define ALTO_MUNDO  3000

// --- IDENTIFICADORES DE OBJETOS ---
#define O_NADA        0
#define O_ARBOL       2
#define O_ROCA        3
#define O_MONTANA     4
#define O_CASTILLO    5
#define O_BARCO       6
#define O_PINO        7
#define O_ARBUSTO     8
#define O_ARBOL2      9
#define O_COMERCIANTE 10
#define O_CUARTEL     11 
#define O_IGLU        12 
#define O_HOGUERA     13
#define O_ARBOL_NIEVE1 14
#define O_ARBOL_NIEVE2 15
#define O_PALMERA      16
#define O_PALMERA2     17
#define O_CARPA        18
#define O_ITEM_ORO     19 // Nuevo objeto
#define O_CHOZA3       20 // Nuevo (Isla 4)
#define O_ARBOL10      21 // Nuevo (Isla 4)
#define O_ENTRADA_CUEVA 22 // Entrada a la Cueva (Isla 5)
#define O_ORO_GRANDE    23 // Tesoro Grande (Cueva)
#define O_CALAVERA      24 // Calavera Dragon (Cueva)

// --- IDENTIFICADORES DE ENEMIGOS ---
#define ENE_ESTANDAR 0
#define ENE_LOBO     1
#define ENE_CIERVO   2
// ISLA 2 (NIEVE)
#define ENE_ARQUERO2  3
#define ENE_GUERRERO2 4
#define ENE_SOLDADO2  5
// ISLA 3 (VOLCÁNICA - NUEVOS)
#define ENE_ARQUERO3  6
#define ENE_GUERRERO3 7
#define ENE_SOLDADO3  8

// --- VARIABLES COMPARTIDAS ---
#define MAX_OBJETOS 300

// --- NUEVAS VARIABLES COMPARTIDAS (EFECTO MADERA) ---
#define DURACION_EFECTO_MADERA 60
extern int efectoMaderaX;
extern int efectoMaderaY;
extern int timerEfectoMadera;
extern HBITMAP hEfectoMadera;

// --- EFECTOS VISUALES (RECURSOS VOLADORES) ---
#define EFECTO_ORO    0
#define EFECTO_MADERA 1
#define EFECTO_COMIDA 2
void agregarEfectoVisual(int tipo, int worldX, int worldY);

extern int menuSeleccionTropaActivo;
extern int tropaBatallaSeleccionada; // 0=Heroe, 1=Soldado, 2=Arquero

// RETRATOS
extern HBITMAP hRetratoHeroe;
extern HBITMAP hRetratoPrincipal; // NUEVO: Para el heroe default (gf1)
extern HBITMAP hRetratoSoldado;
extern HBITMAP hRetratoArquero;
extern HBITMAP hGuerreroFrente[2]; // Tambien este para el heroe default

// --- VARIABLES COMPARTIDAS JUGADOR ---
extern int jugX;
extern int jugY;

// --- EFECTO DE COMIDA (LOOT) ---
#define DURACION_EFECTO_COMIDA 60
extern int efectoComidaX;
extern int efectoComidaY;
extern int timerEfectoComida;
extern HBITMAP hEfectoComida;

extern int objTipo[MAX_OBJETOS];
extern int objX[MAX_OBJETOS];
extern int objY[MAX_OBJETOS];
extern int objValor[MAX_OBJETOS]; // Nuevo: Valor de monedas
extern int totalObjetos;

// Recursos
extern int objVisible[MAX_OBJETOS];      
extern time_t objTiempoRespawn[MAX_OBJETOS]; 

// Estados
extern int menuBarcoActivo; 
extern int menuCastilloActivo;
extern int menuTiendaActivo;
extern int menuCuartelActivo; 
extern int menuInventarioActivo;
extern int enBatalla;       
extern int islaActual;      

// Lógica Cuartel
extern int tropaSeleccionada; 

// Estadísticas
extern int jugVida, jugVidaMax, jugAtaque;
extern int jugOro;
extern int jugMadera;
extern int jugComida;

// PANTALLA
extern int pantallaAnchoPx;
extern int pantallaAltoPx;
extern HWND hwndGlobal;

// Tropas
extern int tropaGuerrero;
extern int tropaArquero;
extern int tropaLancero;
extern int tropaEscudero;

// Batalla
extern int eneVida, eneVidaMax, eneAtaque;
extern int turnoJugador;
extern int tipoEnemigoBatalla;

// Recursos Gráficos
extern HBITMAP hGuerreroFrente[2];
extern HBITMAP hGuerreroAtras[2];
extern HBITMAP hGuerreroLado[2];    
extern HBITMAP hGuerreroDerecha[2]; 

extern HBITMAP hObjRoca; 
extern HBITMAP hObjArbusto; 
extern HBITMAP hObjArbol2; 
extern HBITMAP hObjComerciante;
extern HBITMAP hObjCuartel; 
extern HBITMAP hObjIglu;
extern HBITMAP hObjHoguera;
extern HBITMAP hObjArbolNieve1;
extern HBITMAP hObjArbolNieve2;

extern HBITMAP hObjPalmera;
extern HBITMAP hObjPalmera2;
extern HBITMAP hObjCarpa;

// INTERFAZ
extern HBITMAP hMenuInventarioBG;
extern HBITMAP hIconoMochila; 

extern HBITMAP hObjChoza3; 
extern HBITMAP hObjArbol10; 

extern HBITMAP hMapaBatalla;  
extern HBITMAP hMapaBatalla2; 
extern HBITMAP hMapaBatalla3; 

// CUEVA
extern HBITMAP hMapaCueva;
extern HBITMAP hMapaCuevaMem; 

// ENEMIGOS
extern HBITMAP hEnemigoLobo;
extern HBITMAP hEnemigoCiervo;
// ISLA 2
extern HBITMAP hEnemigoArquero2;
extern HBITMAP hEnemigoGuerrero2;
extern HBITMAP hEnemigoSoldado2;
// ISLA 3
extern HBITMAP hEnemigoArquero3;
extern HBITMAP hEnemigoGuerrero3;
extern HBITMAP hEnemigoSoldado3;

// EFECTOS
extern HBITMAP hChispa1; 
extern HBITMAP hChispa2; 

extern HBITMAP hObjItemOro; // Nuevo sprite 
extern HBITMAP hObjOroGrande; // Nuevo asset oro2
extern HBITMAP hObjCalavera;  // Nuevo asset calavera 

extern HBITMAP hBtnAtacar1, hBtnHuir1, hBtnCurar1;
extern HBITMAP hBtnAtacar2, hBtnHuir2, hBtnCurar2;
extern HBITMAP hBtnAtacar3, hBtnHuir3, hBtnCurar3;

extern HBITMAP hMenuReclutamiento;
extern HBITMAP hBtnComprar; 
extern HBITMAP hBtnCerrar;  

// --- FUNCIONES ---
void inicializarMundo();
void cambiarIsla(int numero);
void iniciarBatalla();
void procesarBatalla();
void dibujarBatalla(HDC hdc, int ancho, int alto);

void inicializarRecursos();
void intentarRecolectar(int jugadorX, int jugadorY);
void actualizarRespawn();
void crearMoneda(int x, int y, int valor); // Nueva funcion

// --- ALDEANOS ---
#define MAX_ALDEANOS 50
// Estados Aldeano
#define A_IDLE 0
#define A_MOVIENDO 1
#define A_RECOLECTANDO 2
#define A_RETORNANDO 3 // (Opcional si queremos que vuelvan al cuartel)

typedef struct {
    int active;
    float x, y; // Float para movimiento suave
    int estado;
    int targetId; // Indice del objeto recurso (arbol/roca/animal)
    int timer;    // Tiempo para recolectar
    int tipoRecurso; // 0=Madera, 1=Comida, 2=Oro
    int direccion; // 0=Abajo, 1=Arriba, 2=Derecha, 3=Izquierda
    int frame;     // 0 o 1 (Caminando)
} Aldeano;

extern Aldeano aldeanos[MAX_ALDEANOS];
extern int totalAldeanos;

// Recursos Graficos Aldeano
extern HBITMAP hAldeanoFrente[2];
extern HBITMAP hAldeanoAtras[2];
extern HBITMAP hAldeanoLado[2];    // Izq
extern HBITMAP hAldeanoDerecha[2]; // Der

// FUNCIONES ALDEANO
void inicializarAldeanos();
void actualizarAldeanos();
void dibujarAldeanos(HDC hdc, int camX, int camY, int zoom);
void comprarAldeano();

#endif