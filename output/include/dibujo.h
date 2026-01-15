#ifndef DIBUJO_H
#define DIBUJO_H

#include <windows.h>
#include <stdio.h>

HBITMAP cargarBitmap(const char* ruta);
// Función unificada (reemplaza a dibujarTransparente y dibujarSprite)
void dibujar(HDC hdcDest, int x, int y, HBITMAP hBitmap, int w, int h);

// Globals from main.c
extern int anchoMapaReal;
extern int altoMapaReal;
extern int pantallaAnchoPx;
extern int pantallaAltoPx;

#endif
