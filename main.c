#include <windows.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h> // Necesario para isspace()

// --- ESTADOS DEL JUEGO ---
#define ST_INICIO 0
#define ST_SUBMENU 1
#define ST_SELECCION_GENERO 2
#define ST_SELECCION_ISLA 3
#define ST_CREDITOS 4
#define ST_OPCIONES 5
#define ST_NOMBRE 6 

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

// --- VARIABLES GLOBALES ---
int estadoActual = ST_INICIO;
int botonPresionado = -1;
int nivelBrillo = 0; 
int generoJugador = 0; 

// Variables de Captura de Nombre
char nombreUsuario[50] = "";
int indiceNombre = 0;
bool esUsuarioNuevo = true; 
bool errorNombreRepetido = false;
bool errorUsuarioNoEncontrado = false;
bool errorFormatoNombre = false; // Nueva variable para error de espacios/largo

// --- COORDINADAS ---
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

HBITMAP CargarBMP(const char* archivo) {
    HBITMAP h = (HBITMAP)LoadImage(NULL, archivo, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    if (!h) { char msg[128]; sprintf(msg, "Falta archivo: %s", archivo); MessageBox(NULL, msg, "Error", MB_ICONERROR); }
    return h;
}

void InicializarRecursos() {
    CreateDirectory("Partidas", NULL); 
    hF1 = CargarBMP("fondo.bmp"); hF2 = CargarBMP("fondo2.bmp");
    hFG = CargarBMP("fondo_personaje.bmp"); hFI = CargarBMP("fondo_islas.bmp");
    hFC = CargarBMP("fondo_creditos.bmp"); hFO = CargarBMP("fondo_opciones.bmp");
    hBtnJ = CargarBMP("btn_jugar.bmp"); hBtnN = CargarBMP("btn_nueva.bmp");
    hBtnC = CargarBMP("btn_continuar.bmp"); hBtnO = CargarBMP("btn_opciones.bmp");
    hBtnCr = CargarBMP("btn_creditos.bmp"); hBtnA = CargarBMP("btn_atras.bmp");
    hBtnMas = CargarBMP("btn_mas.bmp"); hBtnMen = CargarBMP("btn_menos.bmp");
    hBtnFle = CargarBMP("btn_flechas.bmp"); hBtnW = CargarBMP("btn_wasd.bmp");
    hAvM = CargarBMP("avatar_m.bmp"); hAvF = CargarBMP("avatar_f.bmp");
    hI1 = CargarBMP("isla1.bmp"); hI2 = CargarBMP("isla2.bmp"); hI3 = CargarBMP("isla3.bmp");
}

bool PuntoEnRect(int x, int y, RECT rc) { return (x >= rc.left && x <= rc.right && y >= rc.top && y <= rc.bottom); }

bool ExisteUsuario(char* nombre) {
    char ruta[100]; sprintf(ruta, "Partidas/%s.bin", nombre);
    FILE* f = fopen(ruta, "rb");
    if(f) { fclose(f); return true; }
    return false;
}

void DibujarSprite(HDC hdc, HBITMAP hBmp, RECT rc, int id) {
    if (!hBmp) return;
    BITMAP bm; GetObject(hBmp, sizeof(bm), &bm);
    HDC hMem = CreateCompatibleDC(hdc); SelectObject(hMem, hBmp);
    int off = (botonPresionado == id) ? 4 : 0;
    TransparentBlt(hdc, rc.left + off, rc.top + off, (rc.right - rc.left) - (off * 2), (rc.bottom - rc.top) - (off * 2), hMem, 0, 0, bm.bmWidth, bm.bmHeight, RGB(255, 0, 255));
    DeleteDC(hMem);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    int x = LOWORD(lParam), y = HIWORD(lParam);
    switch (uMsg) {
        case WM_CREATE: InicializarRecursos(); break;
        case WM_ERASEBKGND: return 1;
        
        case WM_CHAR: 
            if (estadoActual == ST_NOMBRE) {
                if (wParam == VK_RETURN && indiceNombre > 0) {
                    nombreUsuario[indiceNombre] = '\0';
                    
                    // --- VALIDACIÓN DE FORMATO (Parte 3) ---
                    if (isspace((unsigned char)nombreUsuario[0]) || isspace((unsigned char)nombreUsuario[indiceNombre - 1])) {
                        errorFormatoNombre = true;
                        errorNombreRepetido = false;
                        errorUsuarioNoEncontrado = false;
                    } else {
                        // Si el formato es correcto, procedemos con las reglas de negocio
                        errorFormatoNombre = false;
                        bool existe = ExisteUsuario(nombreUsuario);
                        
                        if (esUsuarioNuevo && existe) {
                            errorNombreRepetido = true;
                            errorUsuarioNoEncontrado = false;
                        } else if (!esUsuarioNuevo && !existe) {
                            errorUsuarioNoEncontrado = true;
                            errorNombreRepetido = false;
                        } else {
                            estadoActual = ST_SUBMENU;
                        }
                    }
                }
                else if (wParam == VK_BACK && indiceNombre > 0) {
                    indiceNombre--; nombreUsuario[indiceNombre] = '\0';
                    errorNombreRepetido = false; errorUsuarioNoEncontrado = false; errorFormatoNombre = false;
                }
                else if (wParam >= 32 && indiceNombre < 49) {
                    nombreUsuario[indiceNombre] = (char)wParam; indiceNombre++; nombreUsuario[indiceNombre] = '\0';
                }
                InvalidateRect(hwnd, NULL, FALSE);
            }
            break;

        case WM_LBUTTONDOWN:
            if (estadoActual == ST_INICIO && PuntoEnRect(x, y, rcJugar)) botonPresionado = BTN_JUGAR;
            else if (estadoActual == ST_NOMBRE) {
                if (PuntoEnRect(x, y, rcBtnNuevo)) { esUsuarioNuevo = true; botonPresionado = BTN_USR_NUEVO; }
                if (PuntoEnRect(x, y, rcBtnReg))   { esUsuarioNuevo = false; botonPresionado = BTN_USR_REG; }
                if (PuntoEnRect(x, y, rcAtras))    botonPresionado = BTN_ATRAS;
            }
            else if (estadoActual == ST_SUBMENU) {
                if (PuntoEnRect(x, y, rcNueva)) botonPresionado = BTN_NUEVA;
                if (PuntoEnRect(x, y, rcCargar)) botonPresionado = BTN_CARGAR;
                if (PuntoEnRect(x, y, rcOpc)) botonPresionado = BTN_OPC;
                if (PuntoEnRect(x, y, rcCreditos)) botonPresionado = BTN_CREDITOS;
                if (PuntoEnRect(x, y, rcAtras)) botonPresionado = BTN_ATRAS;
            }
            else if (estadoActual == ST_SELECCION_GENERO) {
                if (PuntoEnRect(x, y, rcMasc)) botonPresionado = BTN_MASC;
                if (PuntoEnRect(x, y, rcFem)) botonPresionado = BTN_FEM;
                if (PuntoEnRect(x, y, rcAtras)) botonPresionado = BTN_ATRAS;
            }
            else if (estadoActual == ST_SELECCION_ISLA) {
                if (PuntoEnRect(x, y, rcIslaAzul)) botonPresionado = BTN_ISLA_AZUL;
                if (PuntoEnRect(x, y, rcIslaVerde)) botonPresionado = BTN_ISLA_VERDE;
                if (PuntoEnRect(x, y, rcIslaAmarilla)) botonPresionado = BTN_ISLA_AMARILLA;
                if (PuntoEnRect(x, y, rcAtras)) botonPresionado = BTN_ATRAS;
            }
            else if (estadoActual == ST_OPCIONES) {
                if (PuntoEnRect(x, y, rcMusMas)) botonPresionado = BTN_MUS_MAS;
                if (PuntoEnRect(x, y, rcMusMeno)) botonPresionado = BTN_MUS_MENOS;
                if (PuntoEnRect(x, y, rcEfxMas)) botonPresionado = BTN_EFX_MAS;
                if (PuntoEnRect(x, y, rcEfxMeno)) botonPresionado = BTN_EFX_MENOS;
                if (PuntoEnRect(x, y, rcBriMas)) botonPresionado = BTN_BRI_MAS;
                if (PuntoEnRect(x, y, rcBriMeno)) botonPresionado = BTN_BRI_MENOS;
                if (PuntoEnRect(x, y, rcFlechas)) botonPresionado = BTN_CTRL_FLECHAS;
                if (PuntoEnRect(x, y, rcWASD)) botonPresionado = BTN_CTRL_WASD;
                if (PuntoEnRect(x, y, rcAtras)) botonPresionado = BTN_ATRAS;
            }
            else if (PuntoEnRect(x, y, rcAtras)) botonPresionado = BTN_ATRAS;
            InvalidateRect(hwnd, NULL, FALSE); break;

        case WM_LBUTTONUP:
            if (botonPresionado == BTN_JUGAR) estadoActual = ST_NOMBRE;
            else if (botonPresionado == BTN_NUEVA) estadoActual = ST_SELECCION_GENERO;
            else if (botonPresionado == BTN_OPC) estadoActual = ST_OPCIONES;
            else if (botonPresionado == BTN_CREDITOS) estadoActual = ST_CREDITOS;
            else if (botonPresionado == BTN_MASC) { generoJugador = 0; estadoActual = ST_SELECCION_ISLA; }
            else if (botonPresionado == BTN_FEM) { generoJugador = 1; estadoActual = ST_SELECCION_ISLA; }
            else if (botonPresionado == BTN_BRI_MAS) { if (nivelBrillo > 0) nivelBrillo -= 25; }
            else if (botonPresionado == BTN_BRI_MENOS) { if (nivelBrillo < 200) nivelBrillo += 25; }
            else if (botonPresionado == BTN_ATRAS) {
                if (estadoActual == ST_NOMBRE) estadoActual = ST_INICIO;
                else if (estadoActual == ST_SUBMENU) estadoActual = ST_INICIO;
                else if (estadoActual == ST_SELECCION_GENERO) estadoActual = ST_SUBMENU;
                else if (estadoActual == ST_SELECCION_ISLA) estadoActual = ST_SELECCION_GENERO;
                else estadoActual = ST_SUBMENU;
            }
            botonPresionado = -1; InvalidateRect(hwnd, NULL, FALSE); break;

        case WM_PAINT: {
            PAINTSTRUCT ps; HDC hdc = BeginPaint(hwnd, &ps);
            HDC hdcB = CreateCompatibleDC(hdc); HBITMAP hbmB = CreateCompatibleBitmap(hdc, 800, 600); SelectObject(hdcB, hbmB);
            HDC hMem = CreateCompatibleDC(hdcB);
            HBITMAP f = (estadoActual==ST_INICIO)?hF1:((estadoActual==ST_SUBMENU)||(estadoActual==ST_NOMBRE))?hF2:(estadoActual==ST_SELECCION_GENERO)?hFG:(estadoActual==ST_SELECCION_ISLA)?hFI:(estadoActual==ST_CREDITOS)?hFC:hFO;
            SelectObject(hMem, f); BitBlt(hdcB, 0, 0, 800, 600, hMem, 0, 0, SRCCOPY);

            if (estadoActual == ST_INICIO) DibujarSprite(hdcB, hBtnJ, rcJugar, BTN_JUGAR);
            else if (estadoActual == ST_NOMBRE) {
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
                TextOut(hdcB, 250, 280, nombreUsuario, strlen(nombreUsuario));
                
                // DIBUJO DE MENSAJES DE ERROR
                SetTextColor(hdcB, RGB(200, 0, 0));
                if (errorFormatoNombre) {
                    TextOut(hdcB, 240, 320, "¡SIN ESPACIOS AL INICIO O FINAL!", 32);
                } else if (errorNombreRepetido) {
                    TextOut(hdcB, 240, 320, "¡NOMBRE REPETIDO! INTENTA CON OTRO.", 35);
                } else if (errorUsuarioNoEncontrado) {
                    TextOut(hdcB, 240, 320, "¡USUARIO NO ENCONTRADO!", 24);
                }
                SetTextColor(hdcB, RGB(0, 0, 0));

                TextOut(hdcB, 280, 360, "[PULSA ENTER PARA CONFIRMAR]", 28);
                DibujarSprite(hdcB, hBtnA, rcAtras, BTN_ATRAS);
            }
            else if (estadoActual == ST_SUBMENU) {
                DibujarSprite(hdcB, hBtnN, rcNueva, BTN_NUEVA); DibujarSprite(hdcB, hBtnC, rcCargar, BTN_CARGAR);
                DibujarSprite(hdcB, hBtnO, rcOpc, BTN_OPC); DibujarSprite(hdcB, hBtnCr, rcCreditos, BTN_CREDITOS);
                DibujarSprite(hdcB, hBtnA, rcAtras, BTN_ATRAS);
            }
            else if (estadoActual == ST_SELECCION_GENERO) { DibujarSprite(hdcB, hAvM, rcMasc, BTN_MASC); DibujarSprite(hdcB, hAvF, rcFem, BTN_FEM); DibujarSprite(hdcB, hBtnA, rcAtras, BTN_ATRAS); }
            else if (estadoActual == ST_SELECCION_ISLA) { DibujarSprite(hdcB, hI1, rcIslaAzul, BTN_ISLA_AZUL); DibujarSprite(hdcB, hI2, rcIslaVerde, BTN_ISLA_VERDE); DibujarSprite(hdcB, hI3, rcIslaAmarilla, BTN_ISLA_AMARILLA); DibujarSprite(hdcB, hBtnA, rcAtras, BTN_ATRAS); }
            else if (estadoActual == ST_OPCIONES) { DibujarSprite(hdcB, hBtnMen, rcMusMeno, BTN_MUS_MENOS); DibujarSprite(hdcB, hBtnMas, rcMusMas, BTN_MUS_MAS); DibujarSprite(hdcB, hBtnMen, rcEfxMeno, BTN_EFX_MENOS); DibujarSprite(hdcB, hBtnMas, rcEfxMas, BTN_EFX_MAS); DibujarSprite(hdcB, hBtnMen, rcBriMeno, BTN_BRI_MENOS); DibujarSprite(hdcB, hBtnMas, rcBriMas, BTN_BRI_MAS); DibujarSprite(hdcB, hBtnFle, rcFlechas, BTN_CTRL_FLECHAS); DibujarSprite(hdcB, hBtnW, rcWASD, BTN_CTRL_WASD); DibujarSprite(hdcB, hBtnA, rcAtras, BTN_ATRAS); }
            else if (estadoActual == ST_CREDITOS) DibujarSprite(hdcB, hBtnA, rcAtras, BTN_ATRAS);

            if (nivelBrillo > 0) {
                BLENDFUNCTION bf = { AC_SRC_OVER, 0, (BYTE)nivelBrillo, 0 };
                HDC hdcBlk = CreateCompatibleDC(hdcB); HBITMAP hbmBlk = CreateCompatibleBitmap(hdc, 800, 600); SelectObject(hdcBlk, hbmBlk);
                RECT r = {0, 0, 800, 600}; FillRect(hdcBlk, &r, (HBRUSH)GetStockObject(BLACK_BRUSH));
                AlphaBlend(hdcB, 0, 0, 800, 600, hdcBlk, 0, 0, 800, 600, bf);
                DeleteObject(hbmBlk); DeleteDC(hdcBlk);
            }
            BitBlt(hdc, 0, 0, 800, 600, hdcB, 0, 0, SRCCOPY);
            DeleteDC(hMem); DeleteObject(hbmB); DeleteDC(hdcB); EndPaint(hwnd, &ps);
        } break;
        case WM_DESTROY: PostQuitMessage(0); break;
    } return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hI, HINSTANCE hP, LPSTR lp, int nS) {
    WNDCLASS wc = {0}; wc.lpfnWndProc = WindowProc; wc.hInstance = hI; wc.lpszClassName = "GuerraIslaUIFix";
    wc.hCursor = LoadCursor(NULL, IDC_HAND); RegisterClass(&wc);
    HWND hwnd = CreateWindow("GuerraIslaUIFix", "Islas en Guerra", WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_VISIBLE, 100, 100, 816, 639, NULL, NULL, hI, NULL);
    MSG m; while (GetMessage(&m, NULL, 0, 0)) { TranslateMessage(&m); DispatchMessage(&m); } return 0;
}