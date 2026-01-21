#include "../include/juego.h"
#include <stdio.h>
#include <stdlib.h> 

// Estados Generales
int faseBatalla = 0; // 0:TurnoJugador, 1:Espera, 2:TurnoEnemigo, 3:Espera, 4:Fin
int timerEspera = 0; 

// --- VARIABLES DE ANIMACIÓN (DASH + CHISPA) ---
static int animJugX = 0; 
static int animEneX = 0; 
// Fases de Animación: 
// 0: Nada, 1: Jugador Yendo, 2: Jugador Volviendo, 3: Enemigo Yendo, 4: Enemigo Volviendo
// 5: CHISPA JUGADOR, 6: CHISPA ENEMIGO
static int faseAnim = 0; 
static int timerAnimacionImpacto = 0; 

static int botonPresionado = 0; 
static int mouseFuePresionado = 0; 
char mensajeBatalla[100] = "¡Un Enemigo Salvaje aparecio!";

#define BTN_Y 820
#define BTN_W 300
#define BTN_H 220
#define BTN1_X 255 
#define BTN2_X 810 
#define BTN3_X 1365 

#define VELOCIDAD_DASH 150 
#define DISTANCIA_IMPACTO 850 
#define DURACION_CHISPA 15

void dibujarSpriteEnBatalla(HDC hdcDest, int x, int y, int w, int h, HBITMAP hBm) {
    if (!hBm) return; 
    HDC hdcSrc = CreateCompatibleDC(hdcDest);
    HBITMAP hOldSrc = (HBITMAP)SelectObject(hdcSrc, hBm);
    BITMAP bm; GetObject(hBm, sizeof(BITMAP), &bm);
    COLORREF colorFondo = GetPixel(hdcSrc, 0, 0);
    SetStretchBltMode(hdcDest, HALFTONE);
    TransparentBlt(hdcDest, x, y, w, h, hdcSrc, 0, 0, bm.bmWidth, bm.bmHeight, colorFondo);
    SelectObject(hdcSrc, hOldSrc); DeleteDC(hdcSrc);
}

void iniciarBatalla() {
    enBatalla = 1;
    faseBatalla = 0; 
    timerEspera = 0;
    botonPresionado = 0;
    
    animJugX = 0; animEneX = 0; faseAnim = 0; timerAnimacionImpacto = 0;

    // CONFIGURACIÓN DE ENEMIGOS
    if (tipoEnemigoBatalla == ENE_LOBO) {
        eneVidaMax = 80; eneAtaque = 12; sprintf(mensajeBatalla, "¡Un Lobo salvaje ataca!");
    } 
    else if (tipoEnemigoBatalla == ENE_CIERVO) {
        eneVidaMax = 50; eneAtaque = 4; sprintf(mensajeBatalla, "¡Un Ciervo aparece!");
    } 
    // ISLA 2
    else if (tipoEnemigoBatalla == ENE_ARQUERO2) {
        eneVidaMax = 120; eneAtaque = 25; sprintf(mensajeBatalla, "¡Un Arquero de Elite!");
    }
    else if (tipoEnemigoBatalla == ENE_GUERRERO2) {
        eneVidaMax = 180; eneAtaque = 35; sprintf(mensajeBatalla, "¡Un Guerrero Pesado!");
    }
    else if (tipoEnemigoBatalla == ENE_SOLDADO2) {
        eneVidaMax = 150; eneAtaque = 30; sprintf(mensajeBatalla, "¡Un Soldado Veterano!");
    }
    // ISLA 3 (HARDCORE)
    else if (tipoEnemigoBatalla == ENE_ARQUERO3) {
        eneVidaMax = 200; eneAtaque = 50; sprintf(mensajeBatalla, "¡ARQUERO INFERNAL!");
    }
    else if (tipoEnemigoBatalla == ENE_GUERRERO3) {
        eneVidaMax = 300; eneAtaque = 60; sprintf(mensajeBatalla, "¡GUERRERO DEL VOLCAN!");
    }
    else if (tipoEnemigoBatalla == ENE_SOLDADO3) {
        eneVidaMax = 250; eneAtaque = 55; sprintf(mensajeBatalla, "¡SOLDADO DE LAVA!");
    }
    else {
        eneVidaMax = 60; eneAtaque = 8; sprintf(mensajeBatalla, "¡Cuidado! Un Enemigo ataca.");
    }
    eneVida = eneVidaMax;
}

void procesarBatalla() {
    // --- ANIMACIONES ---
    if (faseAnim > 0) {
        if (faseAnim == 1) { // Jugador Yendo
            animJugX += VELOCIDAD_DASH;
            if (animJugX >= DISTANCIA_IMPACTO) {
                animJugX = DISTANCIA_IMPACTO; 
                
                // BONUS POR TROPA
                int bonusAtk = 0;
                if (tropaBatallaSeleccionada == 1) bonusAtk = 10; // Guerrero
                else if (tropaBatallaSeleccionada == 2) bonusAtk = 5; // Escudero
                else if (tropaBatallaSeleccionada == 3) bonusAtk = 20; // Arquero

                int dano = (jugAtaque + bonusAtk) + (rand() % 5);
                eneVida -= dano; if (eneVida < 0) eneVida = 0;
                sprintf(mensajeBatalla, "¡GOLPE CRITICO! -%d HP.", dano);
                faseAnim = 5; timerAnimacionImpacto = DURACION_CHISPA;
            }
        }
        else if (faseAnim == 5) { // Chispa Jugador
            if (timerAnimacionImpacto > 0) timerAnimacionImpacto--; else faseAnim = 2; 
        }
        else if (faseAnim == 2) { // Jugador Volviendo
            animJugX -= VELOCIDAD_DASH;
            if (animJugX <= 0) {
                animJugX = 0; faseAnim = 0; 
                
                // --- COMPROBAR VICTORIA ---
                if (eneVida <= 0) { 
                    faseBatalla = 4; 
                    
                    // 1. Recompensas de ORO (DROP)
                    int oroGanado = 20;
                    if (tipoEnemigoBatalla >= ENE_ARQUERO3) oroGanado = 100;
                    else if (tipoEnemigoBatalla >= ENE_ARQUERO2) oroGanado = 50;

                    // Soltar moneda cerca del jugador (random offset)
                    int offX = (rand() % 100) - 50; // -50 a 50
                    int offY = (rand() % 100) - 50;
                    crearMoneda(jugX + offX, jugY + offY, oroGanado);

                    sprintf(mensajeBatalla, "¡VICTORIA! Solto %d Oro.", oroGanado);

                    // 2. NUEVO: Soltar COMIDA si es animal (Lobo/Ciervo)
                    if (tipoEnemigoBatalla == ENE_LOBO || tipoEnemigoBatalla == ENE_CIERVO) {
                        jugComida += 10; // Dar recurso
                        sprintf(mensajeBatalla, "¡VICTORIA! +10 Carne.");
                        
                        // ACTIVAR EFECTO VISUAL EN EL MAPA
                        // Solo volador
                        // efectoComidaX = jugX + 20; 
                        // efectoComidaY = jugY + 20;
                        // timerEfectoComida = DURACION_EFECTO_COMIDA; 
                        
                        // EFECTO VOLADOR (COMIDA)
                        agregarEfectoVisual(EFECTO_COMIDA, jugX, jugY);
                    }

                    timerEspera = 120; 
                } else { 
                    faseBatalla = 1; timerEspera = 30; 
                }
            }
        }
        else if (faseAnim == 3) { // Enemigo Yendo
            animEneX -= VELOCIDAD_DASH; 
            if (animEneX <= -DISTANCIA_IMPACTO) {
                animEneX = -DISTANCIA_IMPACTO;
                
                // BONUS DEFENSA
                int compensaDef = 0;
                if (tropaBatallaSeleccionada == 1) compensaDef = 2; // Guerrero
                else if (tropaBatallaSeleccionada == 2) compensaDef = 8; // Escudero (TANK)

                int dano = (eneAtaque + (rand() % 4)) - compensaDef;
                if (dano < 1) dano = 1; // Minimo 1 de daño

                jugVida -= dano; if (jugVida < 0) jugVida = 0;
                sprintf(mensajeBatalla, "¡AUCH! -%d HP.", dano);
                faseAnim = 6; timerAnimacionImpacto = DURACION_CHISPA;
            }
        }
        else if (faseAnim == 6) { // Chispa Enemigo
            if (timerAnimacionImpacto > 0) timerAnimacionImpacto--; else faseAnim = 4; 
        }
        else if (faseAnim == 4) { // Enemigo Volviendo
            animEneX += VELOCIDAD_DASH;
            if (animEneX >= 0) {
                animEneX = 0; faseAnim = 0; 
                if (jugVida <= 0) { 
                    faseBatalla = 4; 
                    sprintf(mensajeBatalla, "¡Has sido derrotado!..."); 
                    timerEspera = 100;

                    // --- PERMADEATH (MUERTE PERMANENTE) DE TROPA ---
                    if (tropaBatallaSeleccionada == 1) { // Guerrero
                        if (tropaGuerrero > 0) {
                            tropaGuerrero--;
                            sprintf(mensajeBatalla, "¡Tu Guerrero ha muerto!");
                        }
                    }
                    else if (tropaBatallaSeleccionada == 2) { // Escudero
                        if (tropaEscudero > 0) {
                            tropaEscudero--;
                            sprintf(mensajeBatalla, "¡Tu Escudero ha muerto!");
                        }
                    }
                    else if (tropaBatallaSeleccionada == 3) { // Arquero
                        if (tropaArquero > 0) {
                            tropaArquero--;
                            sprintf(mensajeBatalla, "¡Tu Arquero ha muerto!");
                        }
                    }
                } 
                else { faseBatalla = 3; timerEspera = 60; }
            }
        }
        return; 
    }

    // --- TURNOS ---
    if (timerEspera > 0) { timerEspera--; return; }
    if (timerEspera == 0) {
        if (faseBatalla == 1) { faseBatalla = 2; return; } 
        else if (faseBatalla == 3) { faseBatalla = 0; sprintf(mensajeBatalla, "¿Que haras ahora?"); return; } 
        else if (faseBatalla == 4) { if (jugVida <= 0) jugVida = 1; enBatalla = 0; return; } 
    }

    if (faseBatalla == 0) { 
        int clicIzquierdo = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
        POINT p; GetCursorPos(&p); ScreenToClient(hwndGlobal, &p); // USAR HWND GLOBAL
        RECT r; GetClientRect(hwndGlobal, &r);
        int clientW = r.right - r.left; int clientH = r.bottom - r.top;
        // Input mapeado a logico 1920x1080 para coincidir con hardcode
        // Pero mejor usar proporcional
        
        float sx = (float)clientW / 1920.0f;
        float sy = (float)clientH / 1080.0f;

        // Detectar botones escalados
        int botonBajoMouse = 0;
        // Botones Originales: Y=800, W=400, H=250. X=100, 700, 1380
        if (p.x >= BTN1_X*sx && p.x <= (BTN1_X+BTN_W)*sx && p.y >= BTN_Y*sy && p.y <= (BTN_Y+BTN_H)*sy) botonBajoMouse = 1;
        else if (p.x >= BTN2_X*sx && p.x <= (BTN2_X+BTN_W)*sx && p.y >= BTN_Y*sy && p.y <= (BTN_Y+BTN_H)*sy) botonBajoMouse = 2;
        else if (p.x >= BTN3_X*sx && p.x <= (BTN3_X+BTN_W)*sx && p.y >= BTN_Y*sy && p.y <= (BTN_Y+BTN_H)*sy) botonBajoMouse = 3;

        if (clicIzquierdo) {
            if (botonBajoMouse > 0) botonPresionado = botonBajoMouse; else botonPresionado = 0;
            mouseFuePresionado = 1;
        } else {
            if (mouseFuePresionado == 1) {
                if (botonPresionado == 1 && botonBajoMouse == 1) { faseAnim = 1; }
                else if (botonPresionado == 2 && botonBajoMouse == 2) { 
                    int cura = 25; jugVida += cura; if (jugVida > jugVidaMax) jugVida = jugVidaMax;
                    sprintf(mensajeBatalla, "Te curas +%d HP.", cura); faseBatalla = 1; timerEspera = 60;
                }
                else if (botonPresionado == 3 && botonBajoMouse == 3) { 
                    if (rand() % 100 < 50) { faseBatalla = 4; sprintf(mensajeBatalla, "¡Escapaste!"); timerEspera = 60; eneVida = 0; } 
                    else { faseBatalla = 1; sprintf(mensajeBatalla, "¡No pudiste escapar!"); timerEspera = 60; }
                }
                botonPresionado = 0; mouseFuePresionado = 0;
            }
        }
    } else if (faseBatalla == 2) { faseAnim = 3; }
}

void dibujarBatalla(HDC hdc, int ancho, int alto) {
    HDC hdcMem = CreateCompatibleDC(hdc);
    HBITMAP hFondoAUsar = hMapaBatalla; 
    if (islaActual == 2) hFondoAUsar = hMapaBatalla2; else if (islaActual == 3) hFondoAUsar = hMapaBatalla3;

    // DEFINIR FACTOR DE ESCALA
    float sx = (float)ancho / 1920.0f;
    float sy = (float)alto / 1080.0f;

    if (hFondoAUsar) {
        BITMAP bm; GetObject(hFondoAUsar, sizeof(BITMAP), &bm); HBITMAP oldBm = (HBITMAP)SelectObject(hdcMem, hFondoAUsar);
        SetStretchBltMode(hdc, HALFTONE); StretchBlt(hdc, 0, 0, ancho, alto, hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
        SelectObject(hdcMem, oldBm);
    } 
    DeleteDC(hdcMem); 

    HBITMAP sprJugador = hGuerreroFrente[0]; // Default
    if (tropaBatallaSeleccionada == 1) sprJugador = hRetratoHeroe;     // Guerrero (Personaje)
    else if (tropaBatallaSeleccionada == 2) sprJugador = hRetratoSoldado; // Escudero (SoldadoP2)
    else if (tropaBatallaSeleccionada == 3) sprJugador = hRetratoArquero; // Arquero

    dibujarSpriteEnBatalla(hdc, (200 + animJugX)*sx, 100*sy, 550*sx, 550*sy, sprJugador);

    HBITMAP hSpriteEnemigo = hObjRoca; 
    int eneX = 1190; int eneY = 60; int eneW = 240; int eneH = 240; 

    if (tipoEnemigoBatalla == ENE_LOBO) { hSpriteEnemigo = hEnemigoLobo; eneW = 600;  eneH = 600; eneX = 1300; eneY = 70; }
    else if (tipoEnemigoBatalla == ENE_CIERVO) { hSpriteEnemigo = hEnemigoCiervo; eneW = 530;  eneH = 530; eneX = 1300; eneY = 60; }
    else if (tipoEnemigoBatalla == ENE_ARQUERO2) { hSpriteEnemigo = hEnemigoArquero2; eneW = 500; eneH = 500; eneX = 1300; eneY = 100; }
    else if (tipoEnemigoBatalla == ENE_GUERRERO2) { hSpriteEnemigo = hEnemigoGuerrero2; eneW = 550; eneH = 550; eneX = 1250; eneY = 100; }
    else if (tipoEnemigoBatalla == ENE_SOLDADO2) { hSpriteEnemigo = hEnemigoSoldado2; eneW = 520; eneH = 520; eneX = 1280; eneY = 100; }
    // ISLA 3
    else if (tipoEnemigoBatalla == ENE_ARQUERO3) { hSpriteEnemigo = hEnemigoArquero3; eneW = 500; eneH = 500; eneX = 1300; eneY = 100; }
    else if (tipoEnemigoBatalla == ENE_GUERRERO3) { hSpriteEnemigo = hEnemigoGuerrero3; eneW = 550; eneH = 550; eneX = 1250; eneY = 100; }
    else if (tipoEnemigoBatalla == ENE_SOLDADO3) { hSpriteEnemigo = hEnemigoSoldado3; eneW = 520; eneH = 520; eneX = 1280; eneY = 100; }

    dibujarSpriteEnBatalla(hdc, (eneX + animEneX)*sx, eneY*sy, eneW*sx, eneH*sy, hSpriteEnemigo); 

    if (faseAnim == 5) dibujarSpriteEnBatalla(hdc, (eneX + (eneW/4))*sx, (eneY + (eneH/4))*sy, 300*sx, 300*sy, hChispa1);
    if (faseAnim == 6) dibujarSpriteEnBatalla(hdc, (200 + 100)*sx, (100 + 100)*sy, 300*sx, 300*sy, hChispa2);

    SetBkMode(hdc, TRANSPARENT);
    HFONT fontTexto = CreateFont(24, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Arial");
    HFONT fontTitulo = CreateFont(40, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Arial");
    
    // --- CORRECCIÓN: Declaración de oldFont ---
    HFONT oldFont = (HFONT)SelectObject(hdc, fontTexto);

    HBRUSH bGris = CreateSolidBrush(RGB(70, 70, 70)); 
    HBRUSH bVerde = CreateSolidBrush(RGB(0, 255, 0)); 
    HBRUSH bRojo = CreateSolidBrush(RGB(255, 50, 50)); 
    HBRUSH bFrame = CreateSolidBrush(RGB(255, 255, 255));

    int anchoBarraJug = 500; int xBarraJ = 250; int yBarraJ = 70;
    int anchoVerde = (jugVidaMax > 0) ? (jugVida * anchoBarraJug) / jugVidaMax : 0;
    
    // ESCALAR BARRA JUGADOR
    RECT rFondoJ = {xBarraJ*sx, yBarraJ*sy, (xBarraJ + anchoBarraJug)*sx, (yBarraJ + 25)*sy}; FillRect(hdc, &rFondoJ, bGris);
    RECT rVidaJ = {xBarraJ*sx, yBarraJ*sy, (xBarraJ + anchoVerde)*sx, (yBarraJ + 25)*sy}; FillRect(hdc, &rVidaJ, bVerde); FrameRect(hdc, &rFondoJ, bFrame);

    int anchoBarraEne = 400; int xBarraE = 1200; int yBarraE = 70; 
    int anchoRojo = (eneVidaMax > 0) ? (eneVida * anchoBarraEne) / eneVidaMax : 0;
    
    // ESCALAR BARRA ENEMIGO
    RECT rFondoE = {xBarraE*sx, yBarraE*sy, (xBarraE + anchoBarraEne)*sx, (yBarraE + 25)*sy}; FillRect(hdc, &rFondoE, bGris);
    RECT rVidaE = {xBarraE*sx, yBarraE*sy, (xBarraE + anchoRojo)*sx, (yBarraE + 25)*sy}; FillRect(hdc, &rVidaE, bRojo); FrameRect(hdc, &rFondoE, bFrame);

    char buffer[50]; SetTextColor(hdc, RGB(255, 255, 255));
    sprintf(buffer, "%d/%d", jugVida, jugVidaMax); TextOutA(hdc, (xBarraJ + 10)*sx, (yBarraJ + 2)*sy, buffer, strlen(buffer));
    sprintf(buffer, "%d/%d", eneVida, eneVidaMax); TextOutA(hdc, (xBarraE + 10)*sx, (yBarraE + 2)*sy, buffer, strlen(buffer)); 

    SelectObject(hdc, fontTitulo); TextOutA(hdc, 250*sx, 30*sy, "GUERRERO", 8);
    
    int yNombreEne = 30; 
    int xNombreEne = 1200;
    if (tipoEnemigoBatalla == ENE_LOBO) TextOutA(hdc, xNombreEne*sx, yNombreEne*sy, "LOBO", 4);
    else if (tipoEnemigoBatalla == ENE_CIERVO) TextOutA(hdc, xNombreEne*sx, yNombreEne*sy, "CIERVO", 6);
    else if (tipoEnemigoBatalla == ENE_ARQUERO2) TextOutA(hdc, xNombreEne*sx, yNombreEne*sy, "ARQUERO ELITE", 13);
    else if (tipoEnemigoBatalla == ENE_GUERRERO2) TextOutA(hdc, xNombreEne*sx, yNombreEne*sy, "GUERRERO ELITE", 14);
    else if (tipoEnemigoBatalla == ENE_SOLDADO2) TextOutA(hdc, xNombreEne*sx, yNombreEne*sy, "SOLDADO VET.", 12);
    // NOMBRES ISLA 3
    else if (tipoEnemigoBatalla == ENE_ARQUERO3) TextOutA(hdc, xNombreEne*sx, yNombreEne*sy, "ARQUERO VOLCAN", 14);
    else if (tipoEnemigoBatalla == ENE_GUERRERO3) TextOutA(hdc, xNombreEne*sx, yNombreEne*sy, "GUERRERO VOLCAN", 15);
    else if (tipoEnemigoBatalla == ENE_SOLDADO3) TextOutA(hdc, xNombreEne*sx, yNombreEne*sy, "SOLDADO VOLCAN", 14);
    else TextOutA(hdc, xNombreEne*sx, yNombreEne*sy, "ENEMIGO", 7);

    if (faseBatalla == 0 && faseAnim == 0) {
        HBITMAP b1, b2, b3;
        if (islaActual == 1) { b1=hBtnAtacar1; b2=hBtnCurar1; b3=hBtnHuir1; }
        else if (islaActual == 2) { b1=hBtnAtacar2; b2=hBtnCurar2; b3=hBtnHuir2; }
        else { b1=hBtnAtacar3; b2=hBtnCurar3; b3=hBtnHuir3; }

        int x1=BTN1_X, y1=BTN_Y, w1=BTN_W, h1=BTN_H; int x2=BTN2_X, y2=BTN_Y, w2=BTN_W, h2=BTN_H; int x3=BTN3_X, y3=BTN_Y, w3=BTN_W, h3=BTN_H;
        if (botonPresionado == 1) { x1 += 10; y1 += 10; w1 -= 20; h1 -= 20; }
        if (botonPresionado == 2) { x2 += 10; y2 += 10; w2 -= 20; h2 -= 20; }
        if (botonPresionado == 3) { x3 += 10; y3 += 10; w3 -= 20; h3 -= 20; }

        dibujarSpriteEnBatalla(hdc, x1*sx, y1*sy, w1*sx, h1*sy, b1); dibujarSpriteEnBatalla(hdc, x2*sx, y2*sy, w2*sx, h2*sy, b2); dibujarSpriteEnBatalla(hdc, x3*sx, y3*sy, w3*sx, h3*sy, b3);
    }

    SetTextColor(hdc, RGB(255, 215, 0)); TextOutA(hdc, 550*sx, 80*sy, mensajeBatalla, strlen(mensajeBatalla));
    DeleteObject(bGris); DeleteObject(bVerde); DeleteObject(bRojo); DeleteObject(bFrame); 
    
    SelectObject(hdc, oldFont); 
    DeleteObject(fontTexto); DeleteObject(fontTitulo);
}