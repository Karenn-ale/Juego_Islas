#include <windows.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "../include/menu.h"
#include "../include/registro.h"
#include "../include/juego.h"

// --- IDs DE BOTONES ---
#define BTN_JUGAR 101
#define BTN_NUEVA 102
#define BTN_CARGAR 103
#define BTN_OPC 104
#define BTN_CREDITOS 105
#define BTN_ATRAS 106
#define BTN_MASC 107
#define BTN_FEM 108
#define BTN_ISLA_AZUL 201
#define BTN_ISLA_VERDE 202
#define BTN_ISLA_AMARILLA 203
#define BTN_MUS_MENOS 401
#define BTN_MUS_MAS 402
#define BTN_EFX_MENOS 403
#define BTN_EFX_MAS 404
#define BTN_BRI_MENOS 405
#define BTN_BRI_MAS 406
#define BTN_CTRL_FLECHAS 407
#define BTN_CTRL_WASD 408
#define BTN_USR_NUEVO 501
#define BTN_USR_REG 502

// --- VARIABLES GLOBALES INTERNAS ---
int estadoMenuActual = ST_INICIO;
int botonPresionadoMenu = -1;
int nivelBrillo = 0; 
int generoSeleccionado = 0; 

// Variables de Captura de Nombre
char nombreJugadorGlobal[50] = "";
int indiceNombre = 0;
bool esUsuarioNuevo = true; 
bool errorNombreRepetido = false;
bool errorUsuarioNoEncontrado = false;
bool errorFormatoNombre = false;

// --- COORDINADAS (Originales) ---
RECT rcJugar = { 280, 480, 520, 560 };
RECT rcAtras = { 25, 25, 95, 95 };
RECT rcNueva    = { 200, 130, 600, 210 };    
RECT rcCargar   = { 150, 230, 650, 310 };
RECT rcOpc      = { 230, 330, 570, 410 };  
RECT rcCreditos = { 280, 430, 520, 510 };
RECT rcMasc = { 150, 170, 380, 560 };
RECT rcFem  = { 420, 170, 650, 560 };
RECT rcIslaAzul     = { 50, 130, 250, 410 };  
RECT rcIslaVerde    = { 275, 100, 525, 410 };
RECT rcIslaAmarilla = { 550, 130, 750, 410 };
RECT rcMusMeno = { 310, 170, 395, 215 }; RECT rcMusMas  = { 405, 170, 490, 215 };
RECT rcEfxMeno = { 310, 250, 395, 295 }; RECT rcEfxMas  = { 405, 250, 490, 295 };
RECT rcBriMeno = { 310, 335, 395, 380 }; RECT rcBriMas  = { 405, 335, 490, 380 };
RECT rcFlechas = { 240, 410, 390, 495 }; RECT rcWASD    = { 410, 410, 560, 495 };
RECT rcBtnNuevo = { 240, 180, 400, 220 };
RECT rcBtnReg   = { 410, 180, 570, 220 };

// --- RECURSOS ---
HBITMAP hF1, hF2, hFG, hFI, hFC, hFO;
HBITMAP hBtnJ, hBtnN, hBtnC, hBtnO, hBtnCr, hBtnA, hBtnMas, hBtnMen, hBtnFle, hBtnW, hAvM, hAvF, hI1, hI2, hI3;

// Helper local
HBITMAP CargarBMPMenu(const char* archivo) {
    HBITMAP h = (HBITMAP)LoadImage(NULL, archivo, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    if (!h) { char msg[128]; sprintf(msg, "Falta archivo: %s", archivo); MessageBox(NULL, msg, "Error Menu", MB_ICONERROR); }
    return h;
}

void InicializarMenuRecursos() {
    CreateDirectory("Partidas", NULL); 
    // RUTAS ACTUALIZADAS A assets/
    hF1 = CargarBMPMenu("assets/fondo.bmp"); hF2 = CargarBMPMenu("assets/fondo2.bmp");
    hFG = CargarBMPMenu("assets/fondo_personaje.bmp"); hFI = CargarBMPMenu("assets/fondo_islas.bmp");
    hFC = CargarBMPMenu("assets/fondo_creditos.bmp"); hFO = CargarBMPMenu("assets/fondo_opciones.bmp");
    hBtnJ = CargarBMPMenu("assets/btn_jugar.bmp"); hBtnN = CargarBMPMenu("assets/btn_nueva.bmp");
    hBtnC = CargarBMPMenu("assets/btn_continuar.bmp"); hBtnO = CargarBMPMenu("assets/btn_opciones.bmp");
    hBtnCr = CargarBMPMenu("assets/btn_creditos.bmp"); hBtnA = CargarBMPMenu("assets/btn_atras.bmp");
    hBtnMas = CargarBMPMenu("assets/btn_mas.bmp"); hBtnMen = CargarBMPMenu("assets/btn_menos.bmp");
    hBtnFle = CargarBMPMenu("assets/btn_flechas.bmp"); hBtnW = CargarBMPMenu("assets/btn_wasd.bmp");
    hAvM = CargarBMPMenu("assets/avatar_m.bmp"); hAvF = CargarBMPMenu("assets/avatar_f.bmp");
    // Renombrado para evitar conflicto con los mapas reales del juego
    hI1 = CargarBMPMenu("assets/btn_isla1.bmp"); 
    hI2 = CargarBMPMenu("assets/btn_isla2.bmp"); 
    hI3 = CargarBMPMenu("assets/btn_isla3.bmp");
}

bool PuntoEnRect(int x, int y, RECT rc) { return (x >= rc.left && x <= rc.right && y >= rc.top && y <= rc.bottom); }

// ExisteUsuario usa registro.c ahora (pero registro.c no tenia ExisteUsuario publico con ese nombre exacto, 
// registro.c tiene VerificarExistencia. Adaptamos aqui).
// Pero main.c original definia ExisteUsuario localmente. Podriamos usar VerificarExistencia de registro.h

void DibujarSpriteMenu(HDC hdc, HBITMAP hBmp, RECT rc, int id) {
    if (!hBmp) return;
    BITMAP bm; GetObject(hBmp, sizeof(bm), &bm);
    HDC hMem = CreateCompatibleDC(hdc); SelectObject(hMem, hBmp);
    int off = (botonPresionadoMenu == id) ? 4 : 0;
    TransparentBlt(hdc, rc.left + off, rc.top + off, (rc.right - rc.left) - (off * 2), (rc.bottom - rc.top) - (off * 2), hMem, 0, 0, bm.bmWidth, bm.bmHeight, RGB(255, 0, 255));
    DeleteDC(hMem);
}

bool ProcesarInputMenu(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (estadoMenuActual == ST_JUEGO_INICIADO) return false; 

    RECT rc; GetClientRect(hwnd, &rc);
    int clientW = rc.right - rc.left;
    int clientH = rc.bottom - rc.top; // Altura client area

    int mouseX = LOWORD(lParam); 
    int mouseY = HIWORD(lParam);

    // Mapear mouseX/Y (Screen) a 800x600 (Logico)
    // Evitar division por 0
    int x = (clientW > 0) ? (mouseX * 800) / clientW : mouseX;
    int y = (clientH > 0) ? (mouseY * 600) / clientH : mouseY;

    switch (uMsg) {
        case WM_CHAR: 
            if (estadoMenuActual == ST_NOMBRE) {
                if (wParam == VK_RETURN && indiceNombre > 0) {
                    nombreJugadorGlobal[indiceNombre] = '\0';
                    
                    if (isspace((unsigned char)nombreJugadorGlobal[0]) || isspace((unsigned char)nombreJugadorGlobal[indiceNombre - 1])) {
                        errorFormatoNombre = true; errorNombreRepetido = false; errorUsuarioNoEncontrado = false;
                    } else {
                        errorFormatoNombre = false;
                        bool existe = VerificarExistencia(nombreJugadorGlobal); // Usando registro.h
                        
                        if (esUsuarioNuevo && existe) { errorNombreRepetido = true; errorUsuarioNoEncontrado = false; }
                        else if (!esUsuarioNuevo && !existe) { errorUsuarioNoEncontrado = true; errorNombreRepetido = false; }
                        else {
                            if (esUsuarioNuevo) CrearRegistroBinario(nombreJugadorGlobal);
                            // Transicion al siguiente paso del menu
                            estadoMenuActual = ST_SUBMENU;
                        }
                    }
                }
                else if (wParam == VK_BACK && indiceNombre > 0) {
                    indiceNombre--; nombreJugadorGlobal[indiceNombre] = '\0';
                    errorNombreRepetido = false; errorUsuarioNoEncontrado = false; errorFormatoNombre = false;
                }
                else if (wParam >= 32 && indiceNombre < 49) {
                    nombreJugadorGlobal[indiceNombre] = (char)wParam; indiceNombre++; nombreJugadorGlobal[indiceNombre] = '\0';
                }
                InvalidateRect(hwnd, NULL, FALSE);
                return true;
            }
            break;

        case WM_LBUTTONDOWN:
            if (estadoMenuActual == ST_INICIO && PuntoEnRect(x, y, rcJugar)) botonPresionadoMenu = BTN_JUGAR;
            else if (estadoMenuActual == ST_NOMBRE) {
                if (PuntoEnRect(x, y, rcBtnNuevo)) { esUsuarioNuevo = true; botonPresionadoMenu = BTN_USR_NUEVO; }
                if (PuntoEnRect(x, y, rcBtnReg))   { esUsuarioNuevo = false; botonPresionadoMenu = BTN_USR_REG; }
                if (PuntoEnRect(x, y, rcAtras))    botonPresionadoMenu = BTN_ATRAS;
            }
            else if (estadoMenuActual == ST_SUBMENU) {
                if (PuntoEnRect(x, y, rcNueva)) botonPresionadoMenu = BTN_NUEVA;
                if (PuntoEnRect(x, y, rcCargar)) botonPresionadoMenu = BTN_CARGAR;
                if (PuntoEnRect(x, y, rcOpc)) botonPresionadoMenu = BTN_OPC;
                if (PuntoEnRect(x, y, rcCreditos)) botonPresionadoMenu = BTN_CREDITOS;
                if (PuntoEnRect(x, y, rcAtras)) botonPresionadoMenu = BTN_ATRAS;
            }
            else if (estadoMenuActual == ST_SELECCION_GENERO) {
                if (PuntoEnRect(x, y, rcMasc)) botonPresionadoMenu = BTN_MASC;
                if (PuntoEnRect(x, y, rcFem)) botonPresionadoMenu = BTN_FEM;
                if (PuntoEnRect(x, y, rcAtras)) botonPresionadoMenu = BTN_ATRAS;
            }
            else if (estadoMenuActual == ST_SELECCION_ISLA) {
                if (PuntoEnRect(x, y, rcIslaAzul)) botonPresionadoMenu = BTN_ISLA_AZUL;
                if (PuntoEnRect(x, y, rcIslaVerde)) botonPresionadoMenu = BTN_ISLA_VERDE;
                if (PuntoEnRect(x, y, rcIslaAmarilla)) botonPresionadoMenu = BTN_ISLA_AMARILLA;
                if (PuntoEnRect(x, y, rcAtras)) botonPresionadoMenu = BTN_ATRAS;
            }
            else if (estadoMenuActual == ST_OPCIONES) {
                if (PuntoEnRect(x, y, rcMusMas)) botonPresionadoMenu = BTN_MUS_MAS;
                if (PuntoEnRect(x, y, rcMusMeno)) botonPresionadoMenu = BTN_MUS_MENOS;
                if (PuntoEnRect(x, y, rcEfxMas)) botonPresionadoMenu = BTN_EFX_MAS;
                if (PuntoEnRect(x, y, rcEfxMeno)) botonPresionadoMenu = BTN_EFX_MENOS;
                if (PuntoEnRect(x, y, rcBriMas)) botonPresionadoMenu = BTN_BRI_MAS;
                if (PuntoEnRect(x, y, rcBriMeno)) botonPresionadoMenu = BTN_BRI_MENOS;
                if (PuntoEnRect(x, y, rcFlechas)) botonPresionadoMenu = BTN_CTRL_FLECHAS;
                if (PuntoEnRect(x, y, rcWASD)) botonPresionadoMenu = BTN_CTRL_WASD;
                if (PuntoEnRect(x, y, rcAtras)) botonPresionadoMenu = BTN_ATRAS;
            }
            else if (PuntoEnRect(x, y, rcAtras)) botonPresionadoMenu = BTN_ATRAS;
            
            if (botonPresionadoMenu != -1) {
                InvalidateRect(hwnd, NULL, FALSE);
                return true; 
            }
            break;

        case WM_LBUTTONUP:
            if (botonPresionadoMenu == BTN_JUGAR) estadoMenuActual = ST_NOMBRE;
            else if (botonPresionadoMenu == BTN_NUEVA) estadoMenuActual = ST_SELECCION_GENERO;
            else if (botonPresionadoMenu == BTN_CARGAR) {
                 // LOGICA PARA CONTINUAR/CARGAR:
                 EstadoJuego est;
                 if (CargarDatosBinarios(nombreJugadorGlobal, &est)) {
                     // Restaurar variables globales define en juego.h (deben ser extern aqui, necesitamos incluir juego.h)
                     // Pero menu.c no incluye juego.h todavia. Lo agregaremos arriba.
                     // Asumiendo que podemos acceder a las globales:
                     jugOro = est.oro;
                     jugMadera = est.madera; 
                     jugComida = est.comida;
                     islaActual = est.isla;
                     jugX = est.x; 
                     jugY = est.y;
                     generoSeleccionado = est.genero;
                     
                     // Cambiar isla visualmente
                     cambiarIsla(islaActual); 

                     estadoMenuActual = ST_JUEGO_INICIADO;
                 } else {
                     MessageBox(NULL, "No se pudo cargar la partida o archivo corrupto.", "Error Cargar", MB_OK|MB_ICONERROR);
                 }
            }
            else if (botonPresionadoMenu == BTN_OPC) estadoMenuActual = ST_OPCIONES;
            else if (botonPresionadoMenu == BTN_CREDITOS) estadoMenuActual = ST_CREDITOS;
            else if (botonPresionadoMenu == BTN_MASC) { generoSeleccionado = 0; estadoMenuActual = ST_SELECCION_ISLA; }
            else if (botonPresionadoMenu == BTN_FEM) { generoSeleccionado = 1; estadoMenuActual = ST_SELECCION_ISLA; }
            
            // SELECCION DE ISLA -> INICIO DEL JUEGO
            else if (botonPresionadoMenu == BTN_ISLA_AZUL || botonPresionadoMenu == BTN_ISLA_VERDE || botonPresionadoMenu == BTN_ISLA_AMARILLA) {
                // AQUI COMIENZA EL JUEGO REAL
                estadoMenuActual = ST_JUEGO_INICIADO;
                // Nota: Podriamos pasar el ID de la isla seleccionada a una variable global de juego.h
            }

            else if (botonPresionadoMenu == BTN_BRI_MAS) { if (nivelBrillo > 0) nivelBrillo -= 25; }
            else if (botonPresionadoMenu == BTN_BRI_MENOS) { if (nivelBrillo < 200) nivelBrillo += 25; }
            else if (botonPresionadoMenu == BTN_ATRAS) {
                if (estadoMenuActual == ST_NOMBRE) estadoMenuActual = ST_INICIO;
                else if (estadoMenuActual == ST_SUBMENU) estadoMenuActual = ST_INICIO;
                else if (estadoMenuActual == ST_SELECCION_GENERO) estadoMenuActual = ST_SUBMENU;
                else if (estadoMenuActual == ST_SELECCION_ISLA) estadoMenuActual = ST_SELECCION_GENERO;
                else estadoMenuActual = ST_SUBMENU;
            }
            botonPresionadoMenu = -1; InvalidateRect(hwnd, NULL, FALSE); 
            return true;
            break;
    }
    return false;
}

void RenderizarMenu(HDC hdc, int width, int height) {
    if (estadoMenuActual == ST_JUEGO_INICIADO) return;

    HDC hdcB = CreateCompatibleDC(hdc); 
    HBITMAP hbmB = CreateCompatibleBitmap(hdc, 800, 600); // Menu optimizado para 800x600 segun original
    SelectObject(hdcB, hbmB);
    
    HDC hMem = CreateCompatibleDC(hdcB);
    HBITMAP f = (estadoMenuActual==ST_INICIO)?hF1:((estadoMenuActual==ST_SUBMENU)||(estadoMenuActual==ST_NOMBRE))?hF2:(estadoMenuActual==ST_SELECCION_GENERO)?hFG:(estadoMenuActual==ST_SELECCION_ISLA)?hFI:(estadoMenuActual==ST_CREDITOS)?hFC:hFO;
    
    SelectObject(hMem, f); 
    // Escalar fondo al tamaño de buffer menu (800x600)
    SetStretchBltMode(hdcB, HALFTONE);
    StretchBlt(hdcB, 0, 0, 800, 600, hMem, 0, 0, 800, 600, SRCCOPY); // Asumimos bmps menu son 800x600 o similar

    if (estadoMenuActual == ST_INICIO) DibujarSpriteMenu(hdcB, hBtnJ, rcJugar, BTN_JUGAR);
    else if (estadoMenuActual == ST_NOMBRE) {
        SetBkMode(hdcB, TRANSPARENT);
        TextOut(hdcB, 260, 140, "SELECCIONA EL TIPO DE USUARIO:", 30);
        
        HBRUSH hbN = CreateSolidBrush(esUsuarioNuevo ? RGB(200, 255, 200) : RGB(230, 230, 230));
        HBRUSH hbR = CreateSolidBrush(!esUsuarioNuevo ? RGB(200, 255, 200) : RGB(230, 230, 230));
        FillRect(hdcB, &rcBtnNuevo, hbN); FrameRect(hdcB, &rcBtnNuevo, (HBRUSH)GetStockObject(BLACK_BRUSH));
        FillRect(hdcB, &rcBtnReg, hbR); FrameRect(hdcB, &rcBtnReg, (HBRUSH)GetStockObject(BLACK_BRUSH));
        TextOut(hdcB, 255, 190, "USUARIO NUEVO", 13);
        TextOut(hdcB, 420, 190, "REGISTRADO", 10);
        DeleteObject(hbN); DeleteObject(hbR);

        TextOut(hdcB, 300, 240, "INGRESA TU NOMBRE:", 18);
        RECT rcInput = { 240, 270, 560, 310 };
        FillRect(hdcB, &rcInput, (HBRUSH)GetStockObject(WHITE_BRUSH));
        FrameRect(hdcB, &rcInput, (HBRUSH)GetStockObject(BLACK_BRUSH));
        TextOut(hdcB, 250, 280, nombreJugadorGlobal, strlen(nombreJugadorGlobal));
        
        SetTextColor(hdcB, RGB(200, 0, 0));
        if (errorFormatoNombre) TextOut(hdcB, 240, 320, "¡SIN ESPACIOS AL INICIO O FINAL!", 32);
        else if (errorNombreRepetido) TextOut(hdcB, 240, 320, "¡NOMBRE REPETIDO! INTENTA CON OTRO.", 35);
        else if (errorUsuarioNoEncontrado) TextOut(hdcB, 240, 320, "¡USUARIO NO ENCONTRADO!", 24);
        SetTextColor(hdcB, RGB(0, 0, 0));

        TextOut(hdcB, 280, 360, "[PULSA ENTER PARA CONFIRMAR]", 28);
        DibujarSpriteMenu(hdcB, hBtnA, rcAtras, BTN_ATRAS);
    }
    else if (estadoMenuActual == ST_SUBMENU) {
        DibujarSpriteMenu(hdcB, hBtnN, rcNueva, BTN_NUEVA); DibujarSpriteMenu(hdcB, hBtnC, rcCargar, BTN_CARGAR);
        DibujarSpriteMenu(hdcB, hBtnO, rcOpc, BTN_OPC); DibujarSpriteMenu(hdcB, hBtnCr, rcCreditos, BTN_CREDITOS);
        DibujarSpriteMenu(hdcB, hBtnA, rcAtras, BTN_ATRAS);
    }
    else if (estadoMenuActual == ST_SELECCION_GENERO) { DibujarSpriteMenu(hdcB, hAvM, rcMasc, BTN_MASC); DibujarSpriteMenu(hdcB, hAvF, rcFem, BTN_FEM); DibujarSpriteMenu(hdcB, hBtnA, rcAtras, BTN_ATRAS); }
    else if (estadoMenuActual == ST_SELECCION_ISLA) { DibujarSpriteMenu(hdcB, hI1, rcIslaAzul, BTN_ISLA_AZUL); DibujarSpriteMenu(hdcB, hI2, rcIslaVerde, BTN_ISLA_VERDE); DibujarSpriteMenu(hdcB, hI3, rcIslaAmarilla, BTN_ISLA_AMARILLA); DibujarSpriteMenu(hdcB, hBtnA, rcAtras, BTN_ATRAS); }
    else if (estadoMenuActual == ST_OPCIONES) { DibujarSpriteMenu(hdcB, hBtnMen, rcMusMeno, BTN_MUS_MENOS); DibujarSpriteMenu(hdcB, hBtnMas, rcMusMas, BTN_MUS_MAS); DibujarSpriteMenu(hdcB, hBtnMen, rcEfxMeno, BTN_EFX_MENOS); DibujarSpriteMenu(hdcB, hBtnMas, rcEfxMas, BTN_EFX_MAS); DibujarSpriteMenu(hdcB, hBtnMen, rcBriMeno, BTN_BRI_MENOS); DibujarSpriteMenu(hdcB, hBtnMas, rcBriMas, BTN_BRI_MAS); DibujarSpriteMenu(hdcB, hBtnFle, rcFlechas, BTN_CTRL_FLECHAS); DibujarSpriteMenu(hdcB, hBtnW, rcWASD, BTN_CTRL_WASD); DibujarSpriteMenu(hdcB, hBtnA, rcAtras, BTN_ATRAS); }
    else if (estadoMenuActual == ST_CREDITOS) DibujarSpriteMenu(hdcB, hBtnA, rcAtras, BTN_ATRAS);

    // Brillo (Overlay oscuro)
    if (nivelBrillo > 0) {
        BLENDFUNCTION bf = { AC_SRC_OVER, 0, (BYTE)nivelBrillo, 0 };
        HDC hdcBlk = CreateCompatibleDC(hdcB); HBITMAP hbmBlk = CreateCompatibleBitmap(hdc, 800, 600); SelectObject(hdcBlk, hbmBlk);
        RECT r = {0, 0, 800, 600}; FillRect(hdcBlk, &r, (HBRUSH)GetStockObject(BLACK_BRUSH));
        AlphaBlend(hdcB, 0, 0, 800, 600, hdcBlk, 0, 0, 800, 600, bf);
        DeleteObject(hbmBlk); DeleteDC(hdcBlk);
    }
    
    // Volcar al HDC principal (Estiramos 800x600 a pantalla completa o ventana actual)
    StretchBlt(hdc, 0, 0, width, height, hdcB, 0, 0, 800, 600, SRCCOPY);

    DeleteDC(hMem); DeleteObject(hbmB); DeleteDC(hdcB);
}

void LimpiarMenuRecursos() {
    // DeleteObjects...
}
