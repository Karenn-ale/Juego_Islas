#include "../include/juego.h"
#include <stdio.h>
#include <math.h>
#include <time.h>

void inicializarRecursos() {
    for(int i = 0; i < MAX_OBJETOS; i++) {
        // Solo reiniciamos si es necesario, cuidado con no resetear visibilidad si no quieres
        objTiempoRespawn[i] = 0;
    }
}

void intentarRecolectar(int jugadorX, int jugadorY) {
    int radioPicar = 70;
    int segundosEspera = 30; // Bajé a 30s para pruebas (tu tenías 180)

    int jx = jugadorX + 20; 
    int jy = jugadorY + 20;

    for (int i = 0; i < totalObjetos; i++) {
        if (objVisible[i] == 0) continue;

        int tipo = objTipo[i];
        
        // Mantenemos la lista básica de árboles
        int esArbol = (tipo == O_ARBOL || tipo == O_ARBOL2 || tipo == O_PINO || 
                       tipo == O_ARBOL_NIEVE1 || tipo == O_ARBOL_NIEVE2 || 
                       tipo == O_PALMERA || tipo == O_PALMERA2 || 
                       tipo == O_ARBOL10); // Nuevo

        if (esArbol) {
            int ox = objX[i] + 40; 
            int oy = objY[i] + 100; 
            long long dist = (long long)(jx - ox)*(jx - ox) + (long long)(jy - oy)*(jy - oy);

            if (dist < (radioPicar * radioPicar)) {
                // 1. Ocultar árbol y dar recurso
                objVisible[i] = 0; 
                jugMadera += 2;    
                objTiempoRespawn[i] = time(NULL) + segundosEspera; 

                // 2. --- ACTIVAR EFECTO VISUAL (NUEVO) ---
                // Solo efecto volador, quitamos el estatico
                // efectoMaderaX = objX[i] + 40;  
                // efectoMaderaY = objY[i] + 120; 
                // timerEfectoMadera = DURACION_EFECTO_MADERA; 
                
                // EFECTO VOLADOR
                agregarEfectoVisual(EFECTO_MADERA, objX[i]+40, objY[i]+100);
                // ----------------------------------------

                return; 
            }
        }
        // Agregué la roca también por si acaso la tenías
        else if (tipo == O_ROCA) {
             int ox = objX[i] + 40; 
             int oy = objY[i] + 40; 
             long long dist = (long long)(jx - ox)*(jx - ox) + (long long)(jy - oy)*(jy - oy);
             if (dist < (radioPicar * radioPicar)) {
                 objVisible[i] = 0;
                 jugOro += 10;
                 // EFECTO VOLADOR (ORO)
                 agregarEfectoVisual(EFECTO_ORO, objX[i]+40, objY[i]+40);

                 objTiempoRespawn[i] = time(NULL) + segundosEspera;
                 return;
             }
        }
    }
}

void actualizarRespawn() {
    time_t ahora = time(NULL);
    for(int i = 0; i < totalObjetos; i++) {
        if (objVisible[i] == 0) { 
            if (ahora >= objTiempoRespawn[i]) {
                objVisible[i] = 1; 
            }
        }
    }
}