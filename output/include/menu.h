#ifndef MENU_H
#define MENU_H

#include <windows.h>
#include <stdbool.h>

// Estados del Menu (para uso interno del menu y comunicacion basica)
#define ST_INICIO 0
#define ST_SUBMENU 1
#define ST_SELECCION_GENERO 2
#define ST_SELECCION_ISLA 3
#define ST_CREDITOS 4
#define ST_OPCIONES 5
#define ST_NOMBRE 6 
#define ST_JUEGO_INICIADO 99 

// Variables globales del menu que podrian necesitarse (o funciones de acceso)
extern int estadoMenuActual;
extern int generoSeleccionado; // 0=Masc, 1=Fem
extern char nombreJugadorGlobal[50];
extern bool esUsuarioNuevo;

// Inicializacion de recursos graficos del menu
void InicializarMenuRecursos();

// Procesa mensajes de ventana especificos del menu (Mouse, Teclado en inputs de texto)
// Retorna true si el mensaje fue procesado por el menu
bool ProcesarInputMenu(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// Renderiza el menu en el DC dado
void RenderizarMenu(HDC hdc, int width, int height);

// Limpia recursos al cerrar
void LimpiarMenuRecursos();

#endif
