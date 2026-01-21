#include "../../include/dibujo.h"

// Definir variables globales si se usan aquí, o usar extern si están en main.c
// En este caso, main.c define anchoMapaReal/altoMapaReal, así que aquí solo las usamos (si fuera necesario) o las definimos aquí si queremos mover la propiedad.
// Para simplificar, asumiremos que quien llame a cargarBitmap gestiona las globales o las quitamos de aquí si son específicas de main.
// Revisando main.c original, cargarBitmap modificaba anchoMapaReal si era "isla1.bmp".

HBITMAP cargarBitmap(const char* ruta) {
    HBITMAP h = (HBITMAP)LoadImage(NULL, ruta, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
    if(!h) {
        h = (HBITMAP)LoadImage(NULL, ruta, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
        if(!h) { 
            char msg[256]; sprintf(msg, "ERROR CRITICO: No encuentro %s", ruta);
            MessageBox(NULL, msg, "Error Dibujo", MB_ICONERROR);
            exit(1); 
        }
    }
    // Lógica específica del juego original para detectar dimensiones del mapa 1
    // Si queremos desacoplar, esto debería estar fuera, pero para refactor rápido lo mantenemos o lo adaptamos.
    // Como anchoMapaReal es extern en el .h, necesitamos que main.c lo defina.
    if (strcmp(ruta, "assets/isla1.bmp") == 0 || strcmp(ruta, "isla1.bmp") == 0) {
        BITMAP bm; GetObject(h, sizeof(BITMAP), &bm);
        anchoMapaReal = bm.bmWidth; altoMapaReal = bm.bmHeight;
    }
    return h;
}

// Función unificada de dibujo (El "Nuevo Agente")
void dibujar(HDC hdcDest, int x, int y, HBITMAP hBitmap, int w, int h) {
    if (!hBitmap) return;
    
    // Bounds check (Optimización)
    if (x < -w || x > pantallaAnchoPx || y < -h || y > pantallaAltoPx) return;

    HDC hdcSrc = CreateCompatibleDC(hdcDest);
    HBITMAP hOldSrc = (HBITMAP)SelectObject(hdcSrc, hBitmap);
    BITMAP bm; GetObject(hBitmap, sizeof(BITMAP), &bm);
    
    COLORREF colorKey = GetPixel(hdcSrc, 0, 0); 
    SetStretchBltMode(hdcDest, HALFTONE);
    TransparentBlt(hdcDest, x, y, w, h, hdcSrc, 0, 0, bm.bmWidth, bm.bmHeight, colorKey);
    
    SelectObject(hdcSrc, hOldSrc); 
    DeleteDC(hdcSrc);
}
