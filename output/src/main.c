#include <windows.h>
#include <time.h>
#include <math.h>
#include "../include/juego.h" 
#include "../include/dibujo.h"

// --- VARIABLES GLOBALES ---
int objTipo[MAX_OBJETOS];
int objX[MAX_OBJETOS];
int objY[MAX_OBJETOS];
int objValor[MAX_OBJETOS]; 
int totalObjetos = 0;

int objVisible[MAX_OBJETOS];      
time_t objTiempoRespawn[MAX_OBJETOS]; 

int menuBarcoActivo = 0; 
int menuCastilloActivo = 0;
int menuTiendaActivo = 0;
int menuCuartelActivo = 0; 
int menuInventarioActivo = 0; 

int menuSeleccionTropaActivo = 0;
int tropaBatallaSeleccionada = 0; 
char mensajeErrorTropa[64] = "";
int timerErrorTropa = 0;

int tropaSeleccionada = 0; 
int zoomNivel = 1;
int camaraX = 0; int camaraY = 0;
int pantallaAnchoPx = 1920; int pantallaAltoPx = 1080;
int anchoMapaReal = 0, altoMapaReal = 0;
int objetoBajoMouse = -1; 

int jugX = 400; int jugY = 400;
int jugVida = 100; int jugVidaMax = 100; int jugAtaque = 10;
int velocidad = 8; 
int direccionJugador = 0; 
int jugadorEnMovimiento = 0;

int jugOro = 500; int jugMadera = 500; int jugComida = 500;

int tropaGuerrero = 0; int tropaArquero = 0; int tropaLancero = 0; int tropaEscudero = 0;

// Variables Batalla
int enBatalla = 0;
int eneVida = 0, eneVidaMax = 0, eneAtaque = 0;
int turnoJugador = 0;
int tipoEnemigoBatalla = 0;
int islaActual = 1; 

// EFECO MADERA
int efectoMaderaX = 0;
int efectoMaderaY = 0;
int timerEfectoMadera = 0;

// EFECO COMIDA
int efectoComidaX = 0;
int efectoComidaY = 0;
int timerEfectoComida = 0;
HBITMAP hEfectoComida = NULL;
HBITMAP hEfectoMadera = NULL;

// Recursos Gráficos
HBITMAP hMapaIsla1, hMapaIsla2, hMapaIsla3;
HBITMAP hMapaIsla4, hMapaIsla5; // NUEVO
HBITMAP hMapaBatalla, hMapaBatalla2, hMapaBatalla3;

HBITMAP hBtnAtacar1, hBtnHuir1, hBtnCurar1;
HBITMAP hBtnAtacar2, hBtnHuir2, hBtnCurar2;
HBITMAP hBtnAtacar3, hBtnHuir3, hBtnCurar3;

HBITMAP hBtnComprar, hBtnCerrar;

// RETRATOS SELECCION TROPA
HBITMAP hRetratoHeroe;
HBITMAP hRetratoPrincipal = NULL; // NUEVO
HBITMAP hRetratoSoldado;
HBITMAP hRetratoArquero;

HBITMAP hEnemigoLobo, hEnemigoCiervo; 
HBITMAP hEnemigoArquero2, hEnemigoGuerrero2, hEnemigoSoldado2;
HBITMAP hEnemigoArquero3, hEnemigoGuerrero3, hEnemigoSoldado3;

HBITMAP hChispa1, hChispa2;

HBITMAP hObjArbol, hObjArbol2, hObjArbusto;
HBITMAP hObjRoca, hObjMontana, hObjCastillo, hObjBarco, hObjPino;
HBITMAP hObjComerciante, hObjCuartel, hObjIglu, hObjHoguera;
HBITMAP hObjItemOro = NULL; 
HBITMAP hMenuReclutamiento; 
HBITMAP hObjArbolNieve1, hObjArbolNieve2;
HBITMAP hObjPalmera, hObjPalmera2, hObjCarpa;
HBITMAP hObjChoza3 = NULL; // NUEVO
HBITMAP hObjArbol10 = NULL; // NUEVO

HBITMAP hMenuInventarioBG;
HBITMAP hIconoMochila;

HBITMAP hGuerreroFrente[2], hGuerreroAtras[2], hGuerreroLado[2], hGuerreroDerecha[2];

int contadorAnimacion = 0;
HANDLE hInput;

HBITMAP hMapaIsla1Mem = NULL; HDC hdcMapaMem1 = NULL;
HBITMAP hMapaIsla2Mem = NULL; HDC hdcMapaMem2 = NULL;
HBITMAP hMapaIsla3Mem = NULL; HDC hdcMapaMem3 = NULL;
HBITMAP hMapaIsla4Mem = NULL; HDC hdcMapaMem4 = NULL; // NUEVO
HBITMAP hMapaIsla5Mem = NULL; HDC hdcMapaMem5 = NULL; // NUEVO

static inline int clampInt(int v, int lo, int hi) {
    if (v < lo) return lo; if (v > hi) return hi; return v;
}

// --- SISTEMA DE EFECTOS VISUALES (Flying Icons) ---
typedef struct {
    float x, y;
    int tipo; // 0=Oro, 1=Madera, 2=Comida
    int activo;
} EfectoVisual;

#define MAX_EFECTOS 50
EfectoVisual efectos[MAX_EFECTOS];

void inicializarEfectos() {
    for(int i=0; i<MAX_EFECTOS; i++) efectos[i].activo = 0;
}

void agregarEfectoVisual(int tipo, int worldX, int worldY) {
    // Buscar slot
    int idx = -1;
    for(int i=0; i<MAX_EFECTOS; i++) {
        if (!efectos[i].activo) { idx = i; break; }
    }
    if (idx == -1) return; // Lleno

    // Convertir mundo -> pantalla
    int screenX = (worldX - camaraX) * zoomNivel;
    int screenY = (worldY - camaraY) * zoomNivel;
    
    // Ajustar para centrar un poco
    screenX += 20; 
    screenY += 20;

    efectos[idx].x = (float)screenX;
    efectos[idx].y = (float)screenY;
    efectos[idx].tipo = tipo;
    efectos[idx].activo = 1;
}

void actualizarEfectos() {
    // Destino: Mochila (aprox 20, 20 en pantalla)
    float destX = 40.0f;
    float destY = 40.0f;
    float velocidad = 15.0f; // Pixeles por frame (o lerp)

    for(int i=0; i<MAX_EFECTOS; i++) {
        if (!efectos[i].activo) continue;
        
        float dx = destX - efectos[i].x;
        float dy = destY - efectos[i].y;
        float dist = sqrtf(dx*dx + dy*dy);

        if (dist < 20.0f) {
            efectos[i].activo = 0; // Llego
        } else {
            // Moverse
            efectos[i].x += (dx / dist) * velocidad;
            efectos[i].y += (dy / dist) * velocidad;
            // Acelerar un poco visualmente
            velocidad += 1.0f; 
        }
    }
}

void dibujarEfectos(HDC hdc) {
    for(int i=0; i<MAX_EFECTOS; i++) {
        if (!efectos[i].activo) continue;
        HBITMAP spr = NULL;
        if (efectos[i].tipo == EFECTO_ORO) spr = hObjItemOro; // Usamos el sprite de moneda
        else if (efectos[i].tipo == EFECTO_MADERA) spr = hEfectoMadera;
        else if (efectos[i].tipo == EFECTO_COMIDA) spr = hEfectoComida;

        if (spr) {
            // Dibujar pequeñito (30x30)
            dibujar(hdc, (int)efectos[i].x, (int)efectos[i].y, spr, 40, 40);
        }
    }
}
// --------------------------------------------------

void cargarRecursos() {
    hMapaIsla1 = cargarBitmap("assets/isla1.bmp");
    hMapaIsla2 = cargarBitmap("assets/isla2.bmp");
    hMapaIsla3 = cargarBitmap("assets/isla3.bmp");
    hMapaIsla4 = cargarBitmap("assets/isla4.bmp"); // NUEVO
    hMapaIsla5 = cargarBitmap("assets/isla5.bmp"); // NUEVO 
    
    hMapaBatalla  = cargarBitmap("assets/islabatalla.bmp");
    hMapaBatalla2 = cargarBitmap("assets/islabatalla2.bmp");
    hMapaBatalla3 = cargarBitmap("assets/islabatalla3.bmp");

    hBtnAtacar1 = cargarBitmap("assets/btn_atacar1.bmp"); hBtnHuir1 = cargarBitmap("assets/btn_huir1.bmp"); hBtnCurar1 = cargarBitmap("assets/btn_curarte1.bmp");
    hBtnAtacar2 = cargarBitmap("assets/btn_atacar2.bmp"); hBtnHuir2 = cargarBitmap("assets/btn_huir2.bmp"); hBtnCurar2 = cargarBitmap("assets/btn_curarte2.bmp");
    hBtnAtacar3 = cargarBitmap("assets/btn_atacar3.bmp"); hBtnHuir3 = cargarBitmap("assets/btn_huir3.bmp"); hBtnCurar3 = cargarBitmap("assets/btn_curarte3.bmp");

    hBtnComprar = cargarBitmap("assets/btn_comprar.bmp");
    hBtnCerrar  = cargarBitmap("assets/btn_cerrar.bmp");

    hRetratoHeroe   = cargarBitmap("assets/personaje.bmp");
    hRetratoPrincipal = cargarBitmap("assets/gf1.bmp"); // NUEVO
    hRetratoSoldado = cargarBitmap("assets/soldadoP2.bmp");
    hRetratoArquero = cargarBitmap("assets/arquero1.bmp");

    hEnemigoLobo = cargarBitmap("assets/lobo.bmp"); 
    hEnemigoCiervo = cargarBitmap("assets/ciervo.bmp");

    // CARGAR LOS NUEVOS GUERREROS
    hEnemigoArquero2 = cargarBitmap("assets/arquero2.bmp");
    hEnemigoGuerrero2 = cargarBitmap("assets/guerrero2.bmp");
    hEnemigoSoldado2 = cargarBitmap("assets/soldado2.bmp");

    // CARGAR ENEMIGOS ISLA 3 (VOLCÁNICA)
    hEnemigoArquero3 = cargarBitmap("assets/arquero3.bmp");
    hEnemigoGuerrero3 = cargarBitmap("assets/guerrero3.bmp");
    hEnemigoSoldado3 = cargarBitmap("assets/soldado3.bmp");

    
    hObjArbol = cargarBitmap("assets/arbolverde.bmp");
    hObjArbol2 = cargarBitmap("assets/arbolverde2.bmp");
    hObjArbusto = cargarBitmap("assets/arbusto.bmp");
    hObjRoca = cargarBitmap("assets/roca.bmp");
    hObjMontana = cargarBitmap("assets/montana2.bmp");
    hObjCastillo = cargarBitmap("assets/castillo.bmp");
    hObjBarco = cargarBitmap("assets/barco1.bmp");
    hObjPino = cargarBitmap("assets/pino.bmp"); 
    hObjComerciante = cargarBitmap("assets/comerciante.bmp"); 
    hObjCuartel = cargarBitmap("assets/cuartel.bmp"); 
    hObjIglu = cargarBitmap("assets/iglu.bmp"); 
    hObjHoguera = cargarBitmap("assets/hoguera.bmp"); 
    hObjItemOro = cargarBitmap("assets/oro.bmp");
    
    hMenuReclutamiento = cargarBitmap("assets/reclutamiento.bmp");

    hObjArbolNieve1 = cargarBitmap("assets/arbolnieve1.bmp");
    hObjArbolNieve2 = cargarBitmap("assets/arbolnieve2.bmp");

    hObjPalmera = cargarBitmap("assets/palmera.bmp");
    hObjPalmera2 = cargarBitmap("assets/palmera2.bmp");
    hObjCarpa = cargarBitmap("assets/carpa.bmp");
    
    hObjChoza3 = cargarBitmap("assets/choza3.bmp");   // NUEVO
    hObjArbol10 = cargarBitmap("assets/arbol10.bmp"); // NUEVO

    hMenuInventarioBG = cargarBitmap("assets/inventario.bmp");
    hIconoMochila     = cargarBitmap("assets/mochila.bmp"); 
    // --- CARGAR CHISPAS ---
    hChispa1 = cargarBitmap("assets/chispa1.bmp");
    hChispa2 = cargarBitmap("assets/chispa2.bmp");

    if (hMapaIsla1Mem) DeleteObject(hMapaIsla1Mem); hMapaIsla1Mem = cargarBitmap("assets/isla1.bmp");
    if (hdcMapaMem1) DeleteDC(hdcMapaMem1); hdcMapaMem1 = CreateCompatibleDC(NULL); SelectObject(hdcMapaMem1, hMapaIsla1Mem);
    if (hMapaIsla2Mem) DeleteObject(hMapaIsla2Mem); hMapaIsla2Mem = cargarBitmap("assets/isla2.bmp");
    if (hdcMapaMem2) DeleteDC(hdcMapaMem2); hdcMapaMem2 = CreateCompatibleDC(NULL); SelectObject(hdcMapaMem2, hMapaIsla2Mem);
    if (hMapaIsla3Mem) DeleteObject(hMapaIsla3Mem); hMapaIsla3Mem = cargarBitmap("assets/isla3.bmp");
    if (hdcMapaMem3) DeleteDC(hdcMapaMem3); hdcMapaMem3 = CreateCompatibleDC(NULL); SelectObject(hdcMapaMem3, hMapaIsla3Mem);
    // NUEVO: ISLA 4 y 5 MAPA MEMORIA
    if (hMapaIsla4Mem) DeleteObject(hMapaIsla4Mem); hMapaIsla4Mem = cargarBitmap("assets/isla4.bmp");
    if (hdcMapaMem4) DeleteDC(hdcMapaMem4); hdcMapaMem4 = CreateCompatibleDC(NULL); SelectObject(hdcMapaMem4, hMapaIsla4Mem);
    if (hMapaIsla5Mem) DeleteObject(hMapaIsla5Mem); hMapaIsla5Mem = cargarBitmap("assets/isla5.bmp");
    if (hdcMapaMem5) DeleteDC(hdcMapaMem5); hdcMapaMem5 = CreateCompatibleDC(NULL); SelectObject(hdcMapaMem5, hMapaIsla5Mem);
    
    hGuerreroFrente[0] = cargarBitmap("assets/gf1.bmp"); hGuerreroFrente[1] = cargarBitmap("assets/gf2.bmp");
    hGuerreroAtras[0] = cargarBitmap("assets/ga1.bmp"); hGuerreroAtras[1] = cargarBitmap("assets/ga2.bmp");
    hGuerreroLado[0] = cargarBitmap("assets/gl1.bmp"); hGuerreroLado[1] = cargarBitmap("assets/gl2.bmp");
    hGuerreroDerecha[0] = cargarBitmap("assets/gs1.bmp"); hGuerreroDerecha[1] = cargarBitmap("assets/gs2.bmp");
    
    hEfectoMadera = cargarBitmap("assets/madera.bmp");
    // NUEVO: Cargar imagen de comida
    hEfectoComida = cargarBitmap("assets/comida.bmp");
}



void dibujarMenuSeleccionTropa(HDC hdc) {
    // Fondo semitransparente o cuadro
    // Dimensiones mas grandes: 1200x700 centrado
    int menuW = 1200;
    int menuH = 700;
    int mx = (pantallaAnchoPx - menuW) / 2;
    int my = (pantallaAltoPx - menuH) / 2;

    HBRUSH brushBG = CreateSolidBrush(RGB(20, 20, 40));
    RECT rectBG = {mx, my, mx + menuW, my + menuH};
    FillRect(hdc, &rectBG, brushBG);
    DeleteObject(brushBG);
    
    // Borde
    HBRUSH brushFrame = CreateSolidBrush(RGB(200, 200, 200));
    FrameRect(hdc, &rectBG, brushFrame);
    DeleteObject(brushFrame);

    // Titulo
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(255, 215, 0));
    
    // Fuente Titulo
    HFONT hFontTitle = CreateFont(40,0,0,0,FW_BOLD,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH,"Arial");
    HFONT hOldFont = (HFONT)SelectObject(hdc, hFontTitle);
    
    char* titulo = "ELIGE TU COMBATIENTE";
    SIZE testSize; GetTextExtentPoint32A(hdc, titulo, strlen(titulo), &testSize);
    TextOut(hdc, mx + (menuW - testSize.cx)/2, my + 40, titulo, strlen(titulo));

    // Fuente Opciones
    HFONT hFontOpc = CreateFont(20,0,0,0,FW_BOLD,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH,"Arial");
    SelectObject(hdc, hFontOpc); // Usar fuente chica

    // Slots
    int slotW = 200;
    int slotH = 300;
    int gap = 50;
    int startSlotsX = mx + (menuW - (4*slotW + 3*gap)) / 2;
    int slotsY = my + 150;

    // --- OPCION 1: DEFAULT HERO (SIEMPRE DISPONIBLE) ---
    // Usamos retrato heroe PRINCIPAL (gf1)
    dibujar(hdc, startSlotsX, slotsY, hRetratoPrincipal, slotW, slotW);
    SetTextColor(hdc, RGB(200, 200, 255));
    TextOut(hdc, startSlotsX + 20, slotsY + slotW + 10, "TU HEROE", 8);
    TextOut(hdc, startSlotsX + 20, slotsY + slotW + 40, "(Default)", 9);

    // --- OPCION 2: GUERRERO (imagen personaje.bmp, variable tropaGuerrero) ---
    int x2 = startSlotsX + slotW + gap;
    dibujar(hdc, x2, slotsY, hRetratoHeroe, slotW, slotW); 
    char buf[64];
    sprintf(buf, "GUERRERO x%d", tropaGuerrero);
    TextOut(hdc, x2 + 10, slotsY + slotW + 10, buf, strlen(buf));
    if (tropaGuerrero == 0) {
        SetTextColor(hdc, RGB(255, 50, 50));
        TextOut(hdc, x2 + 40, slotsY + slotW + 40, "BLOQUEADO", 9);
    } else {
        SetTextColor(hdc, RGB(0, 255, 0));
        TextOut(hdc, x2 + 40, slotsY + slotW + 40, "DISPONIBLE", 10);
    }

    // --- OPCION 3: ESCUDERO (imagen soldadop2.bmp, variable tropaEscudero) ---
    int x3 = x2 + slotW + gap;
    dibujar(hdc, x3, slotsY, hRetratoSoldado, slotW, slotW); 
    SetTextColor(hdc, RGB(200, 200, 255)); // Reset color
    sprintf(buf, "ESCUDERO x%d", tropaEscudero);
    TextOut(hdc, x3 + 10, slotsY + slotW + 10, buf, strlen(buf));
    if (tropaEscudero == 0) {
        SetTextColor(hdc, RGB(255, 50, 50));
        TextOut(hdc, x3 + 40, slotsY + slotW + 40, "BLOQUEADO", 9);
    } else {
        SetTextColor(hdc, RGB(0, 255, 0));
        TextOut(hdc, x3 + 40, slotsY + slotW + 40, "DISPONIBLE", 10);
    }

    // --- OPCION 4: ARQUERO (imagen arquero1.bmp, variable tropaArquero) ---
    int x4 = x3 + slotW + gap;
    dibujar(hdc, x4, slotsY, hRetratoArquero, slotW, slotW);
    SetTextColor(hdc, RGB(200, 200, 255)); // Reset color
    sprintf(buf, "ARQUERO x%d", tropaArquero);
    TextOut(hdc, x4 + 10, slotsY + slotW + 10, buf, strlen(buf));
    if (tropaArquero == 0) {
        SetTextColor(hdc, RGB(255, 50, 50));
        TextOut(hdc, x4 + 40, slotsY + slotW + 40, "BLOQUEADO", 9);
    } else {
        SetTextColor(hdc, RGB(0, 255, 0));
        TextOut(hdc, x4 + 40, slotsY + slotW + 40, "DISPONIBLE", 10);
    }

    // Mensaje Error
    if (timerErrorTropa > 0) {
        timerErrorTropa--;
        SetTextColor(hdc, RGB(255, 50, 50));
        // Centrado abajo
        char* err = mensajeErrorTropa;
        GetTextExtentPoint32A(hdc, err, strlen(err), &testSize);
        TextOut(hdc, mx + (menuW - testSize.cx)/2, my + menuH - 80, err, strlen(err));
    }

    SelectObject(hdc, hOldFont);
    DeleteObject(hFontTitle);
    DeleteObject(hFontOpc);
}
// ----------------------------------------------------------------------

void dibujarMenuCuartel(HDC hdc) {
    BITMAP bm; if (!hMenuReclutamiento) return; 
    GetObject(hMenuReclutamiento, sizeof(BITMAP), &bm);
    
    int anchoImg = 900; int altoImg = 750;
    int anchoPanelTexto = 550;
    int totalAncho = anchoImg + anchoPanelTexto;
    int startX = (pantallaAnchoPx - totalAncho) / 2;
    int startY = (pantallaAltoPx - altoImg) / 2;
    int panelTextoX = startX + anchoImg;

    HDC hdcAlpha = CreateCompatibleDC(hdc);
    HBITMAP hbmAlpha = CreateCompatibleBitmap(hdc, anchoPanelTexto, altoImg);
    HBITMAP oldBmAlpha = (HBITMAP)SelectObject(hdcAlpha, hbmAlpha);
    HBRUSH hBrushBG = CreateSolidBrush(RGB(20, 20, 40)); 
    RECT rectBG = {0, 0, anchoPanelTexto, altoImg};
    FillRect(hdcAlpha, &rectBG, hBrushBG);
    DeleteObject(hBrushBG);
    BLENDFUNCTION bf; bf.BlendOp = AC_SRC_OVER; bf.BlendFlags = 0; bf.SourceConstantAlpha = 200; bf.AlphaFormat = 0;
    AlphaBlend(hdc, panelTextoX, startY, anchoPanelTexto, altoImg, hdcAlpha, 0, 0, anchoPanelTexto, altoImg, bf);
    SelectObject(hdcAlpha, oldBmAlpha); DeleteObject(hbmAlpha); DeleteDC(hdcAlpha);

    dibujar(hdc, startX, startY, hMenuReclutamiento, anchoImg, altoImg);

    HFONT hFontGrande = CreateFont(32, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Arial");
    HFONT hFontNormal = CreateFont(24, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Arial");
    
    HFONT hOldFont = (HFONT)SelectObject(hdc, hFontGrande);
    SetBkMode(hdc, TRANSPARENT);

    char buffer[100];
    sprintf(buffer, "RECURSOS DISPONIBLES");
    SetTextColor(hdc, RGB(255, 215, 0)); 
    TextOutA(hdc, panelTextoX + 30, startY + 40, buffer, strlen(buffer));

    SelectObject(hdc, hFontNormal);
    sprintf(buffer, "Oro: %d  |  Madera: %d  |  Comida: %d", jugOro, jugMadera, jugComida);
    SetTextColor(hdc, RGB(200, 200, 200)); 
    TextOutA(hdc, panelTextoX + 30, startY + 90, buffer, strlen(buffer));

    SelectObject(hdc, hFontGrande);
    SetTextColor(hdc, RGB(255, 215, 0));
    TextOutA(hdc, panelTextoX + 30, startY + 160, "RECLUTAR TROPAS", 15);

    SelectObject(hdc, hFontNormal);
    int yOpc = startY + 220;
    int salto = 50;
    
    if(tropaSeleccionada == 1) SetTextColor(hdc, RGB(0, 255, 0)); else SetTextColor(hdc, RGB(255, 255, 255));
    TextOutA(hdc, panelTextoX + 30, yOpc, "[1] Guerrero (50 Oro, 10 Comida)", 30);
    
    if(tropaSeleccionada == 2) SetTextColor(hdc, RGB(0, 255, 0)); else SetTextColor(hdc, RGB(255, 255, 255));
    TextOutA(hdc, panelTextoX + 30, yOpc + salto, "[2] Arquero (40 Oro, 30 Mad, 10 Com)", 36);

    if(tropaSeleccionada == 3) SetTextColor(hdc, RGB(0, 255, 0)); else SetTextColor(hdc, RGB(255, 255, 255));
    TextOutA(hdc, panelTextoX + 30, yOpc + salto*2, "[3] Lancero (30 Oro, 50 Mad, 20 Com)", 36);

    if(tropaSeleccionada == 4) SetTextColor(hdc, RGB(0, 255, 0)); else SetTextColor(hdc, RGB(255, 255, 255));
    TextOutA(hdc, panelTextoX + 30, yOpc + salto*3, "[4] Escudero (80 Oro, 40 Comida)", 30);

    SetTextColor(hdc, RGB(150, 150, 255)); 
    sprintf(buffer, "EJERCITO ACTUAL:");
    TextOutA(hdc, panelTextoX + 30, startY + altoImg - 180, buffer, strlen(buffer));
    sprintf(buffer, "Gue:%d | Arq:%d | Lan:%d | Esc:%d", tropaGuerrero, tropaArquero, tropaLancero, tropaEscudero);
    TextOutA(hdc, panelTextoX + 30, startY + altoImg - 140, buffer, strlen(buffer));

    int btnW = 180, btnH = 60;
    int btnComprarX = panelTextoX + 40;
    int btnComprarY = startY + altoImg - 80;
    int btnCerrarX  = panelTextoX + 280;
    int btnCerrarY  = startY + altoImg - 80;

    dibujar(hdc, btnComprarX, btnComprarY, hBtnComprar, btnW, btnH);
    dibujar(hdc, btnCerrarX, btnCerrarY, hBtnCerrar, btnW, btnH);

    SelectObject(hdc, hOldFont); DeleteObject(hFontGrande); DeleteObject(hFontNormal);
}

void dibujarMenuViaje(HDC hdc) {
    int anchoMenu = 500; int altoMenu = 350;
    int x1 = (pantallaAnchoPx / 2) - (anchoMenu / 2); int y1 = (pantallaAltoPx / 2) - (altoMenu / 2);
    int x2 = x1 + anchoMenu; int y2 = y1 + altoMenu;
    HBRUSH brushFondo = CreateSolidBrush(RGB(242, 226, 196)); HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brushFondo);
    Rectangle(hdc, x1, y1, x2, y2);
    HBRUSH brushBorde = CreateSolidBrush(RGB(101, 67, 33)); RECT r = {x1, y1, x2, y2}; FrameRect(hdc, &r, brushBorde);
    SetBkMode(hdc, TRANSPARENT); SetTextColor(hdc, RGB(50, 30, 10));
    TextOutA(hdc, x1 + 120, y1 + 30, "CAPITAN, ELIJA DESTINO:", 23);
    TextOutA(hdc, x1 + 50, y1 + 100, "[1] Isla Principal (Bosque)", 27);
    TextOutA(hdc, x1 + 50, y1 + 140, "[2] Isla Nevada (Pinos)", 23);
    TextOutA(hdc, x1 + 50, y1 + 180, "[3] Isla Volcanica", 18);
    TextOutA(hdc, x1 + 50, y1 + 220, "[4] Isla 4 (Nueva)", 14); // NUEVO
    TextOutA(hdc, x1 + 50, y1 + 260, "[5] Isla 5 (Nueva)", 14); // NUEVO
    TextOutA(hdc, x1 + 50, y1 + 310, "[ESC] Cerrar", 12);
    SelectObject(hdc, oldBrush); DeleteObject(brushFondo); DeleteObject(brushBorde);
}
void dibujarMenuTienda(HDC hdc) {
    int anchoMenu = 600; int altoMenu = 400;
    int x1 = (pantallaAnchoPx / 2) - (anchoMenu / 2); int y1 = (pantallaAltoPx / 2) - (altoMenu / 2);
    int x2 = x1 + anchoMenu; int y2 = y1 + altoMenu;
    HBRUSH brushFondo = CreateSolidBrush(RGB(30, 60, 30)); HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brushFondo);
    Rectangle(hdc, x1, y1, x2, y2);
    HBRUSH brushBorde = CreateSolidBrush(RGB(255, 215, 0)); RECT r = {x1, y1, x2, y2}; FrameRect(hdc, &r, brushBorde);
    SetBkMode(hdc, TRANSPARENT); SetTextColor(hdc, RGB(255, 255, 255));
    char buffer[100]; sprintf(buffer, "TIENDA DEL VIAJERO - ORO: %d", jugOro);
    TextOutA(hdc, x1 + 150, y1 + 30, buffer, strlen(buffer));
    SetTextColor(hdc, RGB(200, 200, 255));
    TextOutA(hdc, x1 + 50, y1 + 100, "[1] Pocion de Vida (+50 HP) - Precio: 30 Oro", 44);
    TextOutA(hdc, x1 + 50, y1 + 150, "[2] Afilar Espada (+5 Atk)  - Precio: 50 Oro", 44);
    SetTextColor(hdc, RGB(255, 100, 100)); TextOutA(hdc, x1 + 50, y1 + 320, "[ESC] Salir de la tienda", 24);
    SelectObject(hdc, oldBrush); DeleteObject(brushFondo); DeleteObject(brushBorde);
}

void dibujarInventario(HDC hdc) {
    if (!hMenuInventarioBG) return;

    SetBkMode(hdc, TRANSPARENT);

    BITMAP bm; GetObject(hMenuInventarioBG, sizeof(BITMAP), &bm);
    int bgW = bm.bmWidth;
    int bgH = bm.bmHeight;
    if (bgW < 800) { bgW *= 1.5; bgH *= 1.5; }
    
    int startX = (pantallaAnchoPx - bgW) / 2;
    int startY = (pantallaAltoPx - bgH) / 2;

    dibujar(hdc, startX, startY, hMenuInventarioBG, bgW, bgH);

    HFONT fontNumeros = CreateFont(52, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Arial");
    HFONT oldFont = (HFONT)SelectObject(hdc, fontNumeros);
    SetTextColor(hdc, RGB(255, 255, 255)); 

    char buffer[50]; SIZE textSize;

    int off_OroX = (int)(bgW * 0.35); int off_OroY = (int)(bgH * 0.32); 
    int off_MaderaX = (int)(bgW * 0.85); int off_MaderaY = (int)(bgH * 0.32); 
    int off_ComidaX = (int)(bgW * 0.35); int off_ComidaY = (int)(bgH * 0.77); 
    int off_SoldadosX = (int)(bgW * 0.85); int off_SoldadosY = (int)(bgH * 0.77);

    sprintf(buffer, "%d", jugOro);
    GetTextExtentPoint32A(hdc, buffer, strlen(buffer), &textSize);
    TextOutA(hdc, startX + off_OroX - (textSize.cx / 2), startY + off_OroY - (textSize.cy / 2), buffer, strlen(buffer));

    sprintf(buffer, "%d", jugMadera);
    GetTextExtentPoint32A(hdc, buffer, strlen(buffer), &textSize);
    TextOutA(hdc, startX + off_MaderaX - (textSize.cx / 2), startY + off_MaderaY - (textSize.cy / 2), buffer, strlen(buffer));

    sprintf(buffer, "%d", jugComida);
    GetTextExtentPoint32A(hdc, buffer, strlen(buffer), &textSize);
    TextOutA(hdc, startX + off_ComidaX - (textSize.cx / 2), startY + off_ComidaY - (textSize.cy / 2), buffer, strlen(buffer));

    int totalTropas = tropaGuerrero + tropaArquero + tropaLancero + tropaEscudero;
    sprintf(buffer, "%d", totalTropas);
    GetTextExtentPoint32A(hdc, buffer, strlen(buffer), &textSize);
    TextOutA(hdc, startX + off_SoldadosX - (textSize.cx / 2), startY + off_SoldadosY - (textSize.cy / 2), buffer, strlen(buffer));

    HFONT fontSub = CreateFont(24, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Arial");
    SelectObject(hdc, fontSub);
    char* msgSalida = "[ESC] Cerrar Mochila";
    GetTextExtentPoint32A(hdc, msgSalida, strlen(msgSalida), &textSize);
    TextOutA(hdc, (pantallaAnchoPx - textSize.cx)/2, startY + bgH + 15, msgSalida, strlen(msgSalida));

    SelectObject(hdc, oldFont); 
    DeleteObject(fontNumeros); 
    DeleteObject(fontSub); 
}

int hayColisionConObjeto(int futuroX, int futuroY) {
    for(int i=0; i<totalObjetos; i++) {
        if (objVisible[i] == 0) continue; 
        int tipo = objTipo[i];
        if (tipo == O_NADA || tipo == O_ARBUSTO) continue; 
        
        if (tipo == O_COMERCIANTE || tipo == O_CUARTEL || tipo == O_IGLU || tipo == O_CARPA || tipo == O_CASTILLO || tipo == O_BARCO || tipo == O_HOGUERA || tipo == O_CHOZA3) {
             int cx = objX[i] + 40; int cy = objY[i] + 40;
             int radio = 30; 
             if (tipo == O_IGLU || tipo == O_CARPA || tipo == O_CHOZA3) radio = 50; 
             if (tipo == O_BARCO) radio = 80;
             if (tipo == O_HOGUERA) radio = 40; 
             if (tipo == O_CASTILLO) { cx += 80; cy += 80; radio = 100; }
             if (tipo == O_CUARTEL) { cx += 30; cy += 30; radio = 70; }

             if ((long long)(futuroX-cx)*(futuroX-cx) + (long long)(futuroY-cy)*(futuroY-cy) < (radio*radio)) return 1;
             continue;
        }

        if (tipo == O_ARBOL || tipo == O_ARBOL2 || tipo == O_PINO || tipo == O_ARBOL_NIEVE1 || tipo == O_ARBOL_NIEVE2 || tipo == O_PALMERA || tipo == O_PALMERA2 || tipo == O_ARBOL10) {
            int izq = objX[i] + 30; int der = objX[i] + 90;
            int arr = objY[i] + 100; int abj = objY[i] + 190; 
            if (futuroX >= izq && futuroX <= der && futuroY >= arr && futuroY <= abj) return 1;
            continue; 
        }

        if (tipo == O_ROCA) {
            int cx = objX[i] + 40; int cy = objY[i] + 40; if ((long long)(futuroX-cx)*(futuroX-cx) + (long long)(futuroY-cy)*(futuroY-cy) < 900) return 1;
        }
        if (tipo == O_MONTANA) {
            int cx = objX[i] + 125; int cy = objY[i] + 125; if ((long long)(futuroX-cx)*(futuroX-cx) + (long long)(futuroY-cy)*(futuroY-cy) < 6400) return 1;
        }
    }
    return 0;
}

int esTerrenoPermitido(int x, int y) {
    HDC hdcUsar = NULL;
    if (islaActual == 1) hdcUsar = hdcMapaMem1; 
    else if (islaActual == 2) hdcUsar = hdcMapaMem2; 
    else if (islaActual == 3) hdcUsar = hdcMapaMem3;
    else if (islaActual == 4) hdcUsar = hdcMapaMem4; // NUEVO
    else if (islaActual == 5) hdcUsar = hdcMapaMem5; // NUEVO

    if (!hdcUsar) return 1; 
    int maxX = anchoMapaReal > 0 ? anchoMapaReal : ANCHO_MUNDO; int maxY = altoMapaReal > 0 ? altoMapaReal : ALTO_MUNDO;
    if (x < 0 || y < 0 || x >= maxX || y >= maxY) return 0;
    COLORREF color = GetPixel(hdcUsar, x, y);
    int r = GetRValue(color); int g = GetGValue(color); int b = GetBValue(color);
    int esArena = (r >= 140 && g >= 100 && b <= 210);
    
    if (islaActual == 1) return ((g >= 60 && g > r && g > b) || (r <= 170 && g >= 80 && b <= 170)) || esArena;
    if (islaActual == 2) return (r > 130 && g > 130 && b > 130) || (b > 200 && r > 150 && g > 150) || esArena;
    if (islaActual == 3) return ((r < 140 && g < 140 && b < 140) || (r > 60 && r > b && g < r + 50) || esArena) && !(b > r + 30 && b > g + 30 && b > 80);
    
    // ISLA 4 y 5: Logica generica (Permitir casi todo menos agua azul pura)
    // Asumimos que agua es B > R+20 y B > G+20
    if (islaActual == 4 || islaActual == 5) {
        if (b > r + 30 && b > g + 30 && b > 100) return 0; // Agua
        return 1; // Resto camnable
    }
    return 0;
}

void actualizarMouse() {
    objetoBajoMouse = -1; 
    POINT p; HWND hwnd = GetConsoleWindow();
    if (GetCursorPos(&p)) {
        ScreenToClient(hwnd, &p);
        RECT rv; GetClientRect(hwnd, &rv);
        int ar = rv.right - rv.left; int al = rv.bottom - rv.top;
        if (ar<=0 || al<=0) return; 
        int mxScreen = (p.x * pantallaAnchoPx) / ar; 
        int myScreen = (p.y * pantallaAltoPx) / al;

        // CLIC EN MOCHILA GIGANTE
        if (mxScreen >= 20 && mxScreen <= 140 && myScreen >= 20 && myScreen <= 140) {
            if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) {
                if (menuInventarioActivo == 0) menuInventarioActivo = 1;
                else menuInventarioActivo = 0;
                Sleep(200); 
            }
            return; 
        }

        int mwx = camaraX + (mxScreen / zoomNivel); 
        int mwy = camaraY + (myScreen / zoomNivel);
        for(int i=0; i<totalObjetos; i++) {
            if (objVisible[i] == 0) continue; 
            int wa=0, ha=0;
            if(objTipo[i] == O_BARCO) { wa=200; ha=200; }
            else if(objTipo[i] == O_COMERCIANTE) { wa=80; ha=80; }
            else if(objTipo[i] == O_CUARTEL) { wa=150; ha=150; }
            if (wa > 0) {
                if (mwx >= objX[i] && mwx <= objX[i]+wa && mwy >= objY[i] && mwy <= objY[i]+ha) {
                    objetoBajoMouse = i; 
                    if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) {
                        if (objTipo[i] == O_BARCO) { menuBarcoActivo=1; Sleep(200); } 
                        else if (objTipo[i] == O_COMERCIANTE) { menuTiendaActivo=1; Sleep(200); }
                        else if (objTipo[i] == O_CUARTEL) { menuCuartelActivo=1; Sleep(200); }
                    }
                    return; 
                }
            }
        }
    }
}

// =========================================================
//   INPUT Y LÓGICA (ACTUALIZADO: RESTRICCIÓN TOTAL ISLA 2)
// =========================================================
void procesarEntradaContinua() {
    if (enBatalla) { procesarBatalla(); return; }
    
    actualizarRespawn();
    
    // MENÚ INVENTARIO
    if (menuInventarioActivo) {
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) { menuInventarioActivo = 0; Sleep(200); }
        if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) { }
        return; 
    }

    if (GetAsyncKeyState('E') & 0x8000) { intentarRecolectar(jugX, jugY); Sleep(200); }

    if (menuCuartelActivo) {
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) { menuCuartelActivo = 0; Sleep(200); }
        if (GetAsyncKeyState('1') & 0x8000) tropaSeleccionada = 1; 
        if (GetAsyncKeyState('2') & 0x8000) tropaSeleccionada = 2; 
        if (GetAsyncKeyState('3') & 0x8000) tropaSeleccionada = 3; 
        if (GetAsyncKeyState('4') & 0x8000) tropaSeleccionada = 4; 
        if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) {
            POINT p; GetCursorPos(&p); ScreenToClient(GetConsoleWindow(), &p);
            RECT r; GetClientRect(GetConsoleWindow(), &r);
            int clientW = r.right - r.left; int clientH = r.bottom - r.top;
            if (clientW>0 && clientH>0) {
                int mx = (p.x * pantallaAnchoPx) / clientW;
                int my = (p.y * pantallaAltoPx) / clientH;
                int anchoImg = 900; int altoImg = 750; int anchoPanelTexto = 550; int totalAncho = anchoImg + anchoPanelTexto;
                int startX = (pantallaAnchoPx - totalAncho) / 2; int startY = (pantallaAltoPx - altoImg) / 2; int panelTextoX = startX + anchoImg;
                if (mx >= panelTextoX && mx <= panelTextoX + anchoPanelTexto) {
                    int yOpc = startY + 220; int salto = 50;
                    if (my >= yOpc && my < yOpc + salto) tropaSeleccionada = 1;
                    if (my >= yOpc + salto && my < yOpc + salto*2) tropaSeleccionada = 2;
                    if (my >= yOpc + salto*2 && my < yOpc + salto*3) tropaSeleccionada = 3;
                    if (my >= yOpc + salto*3 && my < yOpc + salto*4) tropaSeleccionada = 4;
                }
                int btnW = 180, btnH = 60; int btnComprarX = panelTextoX + 40; int btnComprarY = startY + altoImg - 80; int btnCerrarX  = panelTextoX + 280;
                if (mx >= btnComprarX && mx <= btnComprarX+btnW && my >= btnComprarY && my <= btnComprarY+btnH) {
                    if (tropaSeleccionada == 1 && jugOro >= 50 && jugComida >= 10) { jugOro-=50; jugComida-=10; tropaGuerrero++; }
                    else if (tropaSeleccionada == 2 && jugOro >= 40 && jugMadera >= 30 && jugComida >= 10) { jugOro-=40; jugMadera-=30; jugComida-=10; tropaArquero++; }
                    else if (tropaSeleccionada == 3 && jugOro >= 30 && jugMadera >= 50 && jugComida >= 20) { jugOro-=30; jugMadera-=50; jugComida-=20; tropaLancero++; }
                    else if (tropaSeleccionada == 4 && jugOro >= 80 && jugComida >= 40) { jugOro-=80; jugComida-=40; tropaEscudero++; }
                    Sleep(200); 
                }
                if (mx >= btnCerrarX && mx <= btnCerrarX+btnW && my >= btnComprarY && my <= btnComprarY+btnH) { menuCuartelActivo = 0; Sleep(200); }
            }
        }
        return;
    }

    // --- LOGICA MENU SELECCION TROPA ---
    if (menuSeleccionTropaActivo) {
        if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) {
            POINT p; GetCursorPos(&p); ScreenToClient(GetConsoleWindow(), &p);
            RECT r; GetClientRect(GetConsoleWindow(), &r);
            int clientW = r.right - r.left; int clientH = r.bottom - r.top;
            if (clientW>0 && clientH>0) {
                int mx = (p.x * 1920) / clientW; 
                int my = (p.y * 1080) / clientH;

                // Recalcular posicion slots (IGUAL QUE EN DIBUJO)
                int menuW = 1200; int menuH = 700;
                int menuX = (1920 - menuW) / 2;
                int menuY = (1080 - menuH) / 2;
                int slotW = 200; int gap = 50;
                int startSlotsX = menuX + (menuW - (4*slotW + 3*gap)) / 2;
                int slotsY = menuY + 150;

                // 1. DEFAULT HERO
                if (mx >= startSlotsX && mx <= startSlotsX+slotW && my >= slotsY && my <= slotsY+slotW) {
                    tropaBatallaSeleccionada = 0; 
                    menuSeleccionTropaActivo = 0; iniciarBatalla(); Sleep(200);
                }

                // 2. GUERRERO (Variable tropaGuerrero)
                int x2 = startSlotsX + slotW + gap;
                if (mx >= x2 && mx <= x2+slotW && my >= slotsY && my <= slotsY+slotW) {
                    if (tropaGuerrero > 0) {
                        tropaBatallaSeleccionada = 1; // Guerrero
                        menuSeleccionTropaActivo = 0; iniciarBatalla(); Sleep(200);
                    } else { sprintf(mensajeErrorTropa, "¡No tienes Guerreros!"); timerErrorTropa=60; Sleep(200); }
                }

                // 3. ESCUDERO (Variable tropaEscudero)
                int x3 = x2 + slotW + gap;
                if (mx >= x3 && mx <= x3+slotW && my >= slotsY && my <= slotsY+slotW) {
                    if (tropaEscudero > 0) {
                        tropaBatallaSeleccionada = 2; // Escudero
                        menuSeleccionTropaActivo = 0; iniciarBatalla(); Sleep(200);
                    } else { sprintf(mensajeErrorTropa, "¡No tienes Escuderos!"); timerErrorTropa=60; Sleep(200); }
                }

                // 4. ARQUERO (Variable tropaArquero)
                int x4 = x3 + slotW + gap;
                if (mx >= x4 && mx <= x4+slotW && my >= slotsY && my <= slotsY+slotW) {
                    if (tropaArquero > 0) {
                        tropaBatallaSeleccionada = 3; // Arquero
                        menuSeleccionTropaActivo = 0; iniciarBatalla(); Sleep(200);
                    } else { sprintf(mensajeErrorTropa, "¡No tienes Arqueros!"); timerErrorTropa=60; Sleep(200); }
                }
            }
        }
        return; 
    }
    
    if (menuTiendaActivo) { if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) { menuTiendaActivo = 0; Sleep(200); } if (GetAsyncKeyState('1') & 0x8000) { if (jugOro >= 30) { jugOro -= 30; jugVida += 50; if (jugVida > jugVidaMax) jugVida = jugVidaMax; Sleep(200); } } if (GetAsyncKeyState('2') & 0x8000) { if (jugOro >= 50) { jugOro -= 50; jugAtaque += 5; Sleep(200); } } return; }
    if (menuBarcoActivo) { 
        if (GetAsyncKeyState('1') & 0x8000) { cambiarIsla(1); jugX=450; jugY=400; menuBarcoActivo=0; inicializarRecursos(); Sleep(200); } 
        if (GetAsyncKeyState('2') & 0x8000) { cambiarIsla(2); jugX=450; jugY=400; menuBarcoActivo=0; inicializarRecursos(); Sleep(200); } 
        if (GetAsyncKeyState('3') & 0x8000) { cambiarIsla(3); jugX=450; jugY=400; menuBarcoActivo=0; inicializarRecursos(); Sleep(200); } 
        if (GetAsyncKeyState('4') & 0x8000) { cambiarIsla(4); jugX=450; jugY=400; menuBarcoActivo=0; inicializarRecursos(); Sleep(200); } 
        if (GetAsyncKeyState('5') & 0x8000) { cambiarIsla(5); jugX=450; jugY=400; menuBarcoActivo=0; inicializarRecursos(); Sleep(200); } 
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) { menuBarcoActivo=0; Sleep(200); } 
        return; 
    }

    actualizarMouse(); 
    int paso = (zoomNivel == 1) ? 3 : (velocidad/zoomNivel); if(paso<1)paso=1;
    int dx=0, dy=0, mov=0;
    if (GetAsyncKeyState('W') & 0x8000) { dy-=paso; direccionJugador=1; mov=1; }
    if (GetAsyncKeyState('S') & 0x8000) { dy+=paso; direccionJugador=0; mov=1; }
    if (GetAsyncKeyState('A') & 0x8000) { dx-=paso; direccionJugador=3; mov=1; }
    if (GetAsyncKeyState('D') & 0x8000) { dx+=paso; direccionJugador=2; mov=1; } 
    jugadorEnMovimiento = mov;
    if (!mov) return;
    int nX = clampInt(jugX+dx, 0, anchoMapaReal-1); int nY = clampInt(jugY+dy, 0, altoMapaReal-1);
    if (esTerrenoPermitido(nX+20, nY+40)) { 
        if (!hayColisionConObjeto(nX+20, nY+20)) {
            jugX = nX; jugY = nY;
            for (int i=0; i<totalObjetos; i++) {
                
                // --- 1. ARBUSTOS ---
                if (objTipo[i] == O_ARBUSTO) { 
                    if (abs(jugX+20 - (objX[i]+40)) < 40 && abs(jugY+20 - (objY[i]+40)) < 40) { 
                        if (rand()%40 == 0) { 
                            // REGLA ESTRICTA ISLA 2 (Nieve)
                            if (islaActual == 2) {
                                int dado = rand() % 3;
                                if (dado == 0) tipoEnemigoBatalla = ENE_ARQUERO2;
                                else if (dado == 1) tipoEnemigoBatalla = ENE_GUERRERO2;
                                else tipoEnemigoBatalla = ENE_SOLDADO2;
                            } 
                            else if (islaActual == 3) {
                                int dado = rand() % 3;
                                if (dado == 0) tipoEnemigoBatalla = ENE_ARQUERO3;
                                else if (dado == 1) tipoEnemigoBatalla = ENE_GUERRERO3;
                                else tipoEnemigoBatalla = ENE_SOLDADO3;
                            }
                            // OTRAS ISLAS (1 y 3)
                            else {
                                int dado = rand() % 100; 
                                if (dado < 40) tipoEnemigoBatalla = ENE_LOBO; 
                                else if (dado < 70) tipoEnemigoBatalla = ENE_CIERVO; 
                                else tipoEnemigoBatalla = ENE_ESTANDAR; 
                            }
                            
                            // NUEVO: CHEQUEO DE TROPAS
                            // Si NO tienes tropas compradas (Guerrero, Arquero, Escudero) -> Vas directo con Heroe
                            if (tropaGuerrero == 0 && tropaArquero == 0 && tropaEscudero == 0) {
                                tropaBatallaSeleccionada = 0; // Default
                                sprintf(mensajeErrorTropa, ""); // Limpiar
                                iniciarBatalla();
                                jugadorEnMovimiento=0; 
                                return;
                            }
                            
                            // SI TIENES AL MENOS UNA, ABRE MENU
                            menuSeleccionTropaActivo = 1; 
                            timerErrorTropa = 0;
                            // iniciarBatalla(); // YA NO
                            
                            jugadorEnMovimiento=0; return; 
                        } 
                    } 
                }

                // --- 2. IGLÚS Y CARPAS ---
                if (objTipo[i] == O_IGLU || objTipo[i] == O_CARPA) { 
                    int cx = objX[i]+75; int cy = objY[i]+75; 
                    if (abs(jugX+20 - cx) < 80 && abs(jugY+20 - cy) < 80) { 
                        if (rand() % 40 == 0) { 
                            // REGLA ESTRICTA ISLA 2 (Nieve)
                            if (islaActual == 2) {
                                int dado = rand() % 3;
                                if (dado == 0) tipoEnemigoBatalla = ENE_ARQUERO2;
                                else if (dado == 1) tipoEnemigoBatalla = ENE_GUERRERO2;
                                else tipoEnemigoBatalla = ENE_SOLDADO2;
                            } 
                            // PEGAR ESTO JUSTO DESPUÉS DEL BLOQUE DE ISLA 2 (Nieve)
                            // Y ANTES DEL "else" FINAL

                            else if (islaActual == 3) {
                                int dado = rand() % 3;
                                if (dado == 0) tipoEnemigoBatalla = ENE_ARQUERO3;
                                else if (dado == 1) tipoEnemigoBatalla = ENE_GUERRERO3;
                                else tipoEnemigoBatalla = ENE_SOLDADO3;
                            }
                            // OTRAS ISLAS (1 y 3)
                            else {
                                tipoEnemigoBatalla = ENE_ESTANDAR; 
                            }
                            
                            if (tropaGuerrero == 0 && tropaArquero == 0 && tropaEscudero == 0) {
                                tropaBatallaSeleccionada = 0; // Default
                                sprintf(mensajeErrorTropa, ""); 
                                iniciarBatalla();
                                jugadorEnMovimiento=0; 
                                return;
                            }

                            // NUEVO: ABRIR MENU SELECCION
                            menuSeleccionTropaActivo = 1; 
                            timerErrorTropa = 0;
                            
                            jugadorEnMovimiento = 0; return; 
                        } 
                    } 
                }

                // --- 3. RECOGER MONEDAS (NUEVO) ---
                if (objTipo[i] == O_ITEM_ORO) {
                     if (objVisible[i] == 0) continue;
                     int cx = objX[i] + 30; // Centro aprox
                     int cy = objY[i] + 30;
                     if (abs(jugX+20 - cx) < 40 && abs(jugY+20 - cy) < 40) {
                         jugOro += objValor[i];
                         objVisible[i] = 0; // Ocultar
                         objTipo[i] = O_NADA; // ELIMINAR para que no respawnee
                         
                         // NUEVO: Efecto volar a mochila
                         agregarEfectoVisual(EFECTO_ORO, cx, cy);

                         continue;
                     }
                }
            }
        }
    }
}

static void computeCamera(int jugXlocal, int jugYlocal, int* outCamX, int* outCamY) {
    int anchoVista = pantallaAnchoPx / zoomNivel; int altoVista = pantallaAltoPx / zoomNivel;
    int camX = jugXlocal - anchoVista / 2; int camY = jugYlocal - altoVista / 2;
    int maxX = (anchoMapaReal > 0 ? anchoMapaReal : ANCHO_MUNDO) - anchoVista;
    int maxY = (altoMapaReal > 0 ? altoMapaReal : ALTO_MUNDO) - altoVista;
    if (camX < 0) camX = 0; if (camY < 0) camY = 0;
    if (camX > maxX) camX = maxX > 0 ? maxX : 0;
    if (camY > maxY) camY = maxY > 0 ? maxY : 0;
    *outCamX = camX; *outCamY = camY;
}



void renderizar() {
    HWND hwnd = GetConsoleWindow(); HDC hdc = GetDC(hwnd); if (!hdc) return;
    HDC hdcMem = CreateCompatibleDC(hdc); HBITMAP hbmMem = CreateCompatibleBitmap(hdc, pantallaAnchoPx, pantallaAltoPx);
    HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, hbmMem);

    if (enBatalla) { dibujarBatalla(hdcMem, pantallaAnchoPx, pantallaAltoPx); } 
    else {
        computeCamera(jugX, jugY, &camaraX, &camaraY);
        HBITMAP hMapaActual = hMapaIsla1; 
        if (islaActual == 2) hMapaActual = hMapaIsla2; 
        else if (islaActual == 3) hMapaActual = hMapaIsla3; 
        else if (islaActual == 4) hMapaActual = hMapaIsla4; // NUEVO
        else if (islaActual == 5) hMapaActual = hMapaIsla5; // NUEVO 
        if (hMapaActual) { SetStretchBltMode(hdcMem, HALFTONE); HDC hdcMapa = CreateCompatibleDC(hdc); HBITMAP oldMapa = (HBITMAP)SelectObject(hdcMapa, hMapaActual); StretchBlt(hdcMem, 0, 0, pantallaAnchoPx, pantallaAltoPx, hdcMapa, camaraX, camaraY, pantallaAnchoPx/zoomNivel, pantallaAltoPx/zoomNivel, SRCCOPY); SelectObject(hdcMapa, oldMapa); DeleteDC(hdcMapa); }

        for(int i=0; i<totalObjetos; i++) {
            if (objVisible[i] == 0) continue; 
            int sx = (objX[i] - camaraX) * zoomNivel; int sy = (objY[i] - camaraY) * zoomNivel;
            HBITMAP spr = NULL; int t = 0; int tamRealW = 0; int tamRealH = 0;
            if (objTipo[i] == O_ARBOL) { spr = hObjArbol; tamRealW = 120 * zoomNivel; tamRealH = 200 * zoomNivel; }
            else if (objTipo[i] == O_ARBOL2) { spr = hObjArbol2; tamRealW = 120 * zoomNivel; tamRealH = 200 * zoomNivel; }
            else if (objTipo[i] == O_ROCA) { spr = hObjRoca; t = 80*zoomNivel; }
            else if (objTipo[i] == O_MONTANA) { spr = hObjMontana; t = 250*zoomNivel; }
            else if (objTipo[i] == O_CASTILLO) { spr = hObjCastillo; t = 250*zoomNivel; tamRealW=250*zoomNivel; tamRealH=250*zoomNivel; }
            else if (objTipo[i] == O_BARCO) { spr = hObjBarco; t = 200*zoomNivel; tamRealW=200*zoomNivel; tamRealH=200*zoomNivel; }
            else if (objTipo[i] == O_PINO) { spr = hObjPino; tamRealW=100*zoomNivel; tamRealH=140*zoomNivel; }
            else if (objTipo[i] == O_ARBUSTO) { spr = hObjArbusto; t = 80*zoomNivel; }
            else if (objTipo[i] == O_COMERCIANTE) { spr = hObjComerciante; t = 80*zoomNivel; }
            else if (objTipo[i] == O_CUARTEL) { spr = hObjCuartel; t = 150*zoomNivel; }
            else if (objTipo[i] == O_IGLU) { spr = hObjIglu; t = 150*zoomNivel; }
            else if (objTipo[i] == O_HOGUERA) { spr = hObjHoguera; t = 80*zoomNivel; }
            else if (objTipo[i] == O_ARBOL_NIEVE1) { spr = hObjArbolNieve1; tamRealW = 120 * zoomNivel; tamRealH = 200 * zoomNivel; }
            else if (objTipo[i] == O_ARBOL_NIEVE2) { spr = hObjArbolNieve2; tamRealW = 120 * zoomNivel; tamRealH = 200 * zoomNivel; }
            else if (objTipo[i] == O_PALMERA) { spr = hObjPalmera; tamRealW = 120 * zoomNivel; tamRealH = 200 * zoomNivel; }
            else if (objTipo[i] == O_PALMERA2) { spr = hObjPalmera2; tamRealW = 120 * zoomNivel; tamRealH = 200 * zoomNivel; }
            else if (objTipo[i] == O_CARPA) { spr = hObjCarpa; t = 150*zoomNivel; } 
            else if (objTipo[i] == O_ITEM_ORO) { spr = hObjItemOro; t = 50*zoomNivel; } // Nuevo
            else if (objTipo[i] == O_CHOZA3) { spr = hObjChoza3; t = 150*zoomNivel; } // Nuevo
            else if (objTipo[i] == O_ARBOL10) { spr = hObjArbol10; tamRealW = 120 * zoomNivel; tamRealH = 200 * zoomNivel; } // Nuevo

            if (spr) { if (tamRealW > 0) dibujar(hdcMem, sx, sy, spr, tamRealW, tamRealH); else dibujar(hdcMem, sx, sy, spr, t, t); }
            if (i == objetoBajoMouse) { int anchoBorde = (tamRealW > 0 ? tamRealW : t); int altoBorde = (tamRealH > 0 ? tamRealH : t); HBRUSH brushNulo = (HBRUSH)GetStockObject(NULL_BRUSH); HPEN penAmarillo = CreatePen(PS_SOLID, 4, RGB(255, 215, 0)); HGDIOBJ oldBrush = SelectObject(hdcMem, brushNulo); HGDIOBJ oldPen = SelectObject(hdcMem, penAmarillo); Rectangle(hdcMem, sx, sy, sx + anchoBorde, sy + altoBorde); SelectObject(hdcMem, oldBrush); SelectObject(hdcMem, oldPen); DeleteObject(penAmarillo); }
        }
// --- DIBUJAR EFECTO MADERA ---
        if (timerEfectoMadera > 0) {
            // Calculamos posición relativa a la cámara
            int sx = (efectoMaderaX - camaraX) * zoomNivel;
            int sy = (efectoMaderaY - camaraY) * zoomNivel;
            
            // Dibujamos (60x60 es un tamaño ejemplo, ajústalo si quieres)
            dibujar(hdcMem, sx, sy, hEfectoMadera, 60 * zoomNivel, 60 * zoomNivel);
            
            // Restamos tiempo
            timerEfectoMadera--;
        }
        // --- DIBUJAR EFECTO COMIDA ---
        if (timerEfectoComida > 0) {
            int sx = (efectoComidaX - camaraX) * zoomNivel;
            int sy = (efectoComidaY - camaraY) * zoomNivel;
            // Dibuja la comida (ajusta 50*zoomNivel si quieres otro tamaño)
            dibujar(hdcMem, sx, sy, hEfectoComida, 50 * zoomNivel, 50 * zoomNivel);
            timerEfectoComida--;
        }

        int jx = (jugX - camaraX) * zoomNivel; int jy = (jugY - camaraY) * zoomNivel;
        int frame = jugadorEnMovimiento ? ((clock() / 150) % 2) : 0;
        HBITMAP sprJ = hGuerreroFrente[0];
        if (direccionJugador == 0) sprJ = hGuerreroFrente[frame]; else if (direccionJugador == 1) sprJ = hGuerreroAtras[frame]; else if (direccionJugador == 3) sprJ = hGuerreroLado[frame]; else if (direccionJugador == 2) sprJ = hGuerreroDerecha[frame];
        dibujar(hdcMem, jx, jy, sprJ, 40*zoomNivel, 40*zoomNivel);

        // --- UI FIJA (MOCHILA GIGANTE 120x120) ---
        dibujar(hdcMem, 20, 20, hIconoMochila, 120, 120);

        // --- DIBUJAR EFECTOS VOLADORES ---
        actualizarEfectos(); // Actualizar logica aqui para suavidad visual
        dibujarEfectos(hdcMem);

        if (menuBarcoActivo) dibujarMenuViaje(hdcMem);
        else if (menuSeleccionTropaActivo) dibujarMenuSeleccionTropa(hdcMem); // NUEVO
        else if (menuCastilloActivo) { /* YA NO EXISTE */ } 
        else if (menuTiendaActivo) dibujarMenuTienda(hdcMem);
        else if (menuCuartelActivo) dibujarMenuCuartel(hdcMem);
        else if (menuInventarioActivo) dibujarInventario(hdcMem);
    }
    BitBlt(hdc, 0, 0, pantallaAnchoPx, pantallaAltoPx, hdcMem, 0, 0, SRCCOPY);
    SelectObject(hdcMem, hbmOld); DeleteObject(hbmMem); DeleteDC(hdcMem); ReleaseDC(hwnd, hdc);
}

void configurarVentana() { /* ... IGUAL ... */ 
    HWND hwnd = GetConsoleWindow(); pantallaAnchoPx = 1920; pantallaAltoPx = 1080;
    SetWindowLong(hwnd, GWL_STYLE, WS_POPUP | WS_VISIBLE); SetWindowPos(hwnd, HWND_TOP, 0, 0, pantallaAnchoPx, pantallaAltoPx, SWP_FRAMECHANGED | SWP_SHOWWINDOW); ShowWindow(hwnd, SW_MAXIMIZE);
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE); COORD bufferSize = { (SHORT)(pantallaAnchoPx/8), (SHORT)(pantallaAltoPx/16) }; SetConsoleScreenBufferSize(hOut, bufferSize);
    hInput = GetStdHandle(STD_INPUT_HANDLE); DWORD mode; GetConsoleMode(hInput, &mode); SetConsoleMode(hInput, mode | ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS);
}

int main() {
    srand(time(NULL)); configurarVentana(); cargarRecursos(); inicializarMundo(); inicializarRecursos();
    int run = 1; INPUT_RECORD ir[128]; DWORD nRead;
    while(run) {
        procesarEntradaContinua(); renderizar(); Sleep(16);
        DWORD nEvents = 0; GetNumberOfConsoleInputEvents(hInput, &nEvents);
        if (nEvents > 0) { ReadConsoleInput(hInput, ir, 128, &nRead);
            for (DWORD i = 0; i < nRead; i++) {
                if (ir[i].EventType == KEY_EVENT && ir[i].Event.KeyEvent.bKeyDown) { char k = ir[i].Event.KeyEvent.uChar.AsciiChar; if (k == 'x' || k == 'X') run = 0; if (k == '+') { if(zoomNivel < 5) zoomNivel++; } if (k == '-') { if(zoomNivel > 1) zoomNivel--; } }
                else if (ir[i].EventType == MOUSE_EVENT) { if (ir[i].Event.MouseEvent.dwEventFlags & MOUSE_WHEELED) { if ((int)ir[i].Event.MouseEvent.dwButtonState > 0) { if(zoomNivel < 5) zoomNivel++; } else { if(zoomNivel > 1) zoomNivel--; } } }
            }
        }
    }
    return 0;
}