#include "../include/juego.h"
#include "../include/dibujo.h"
#include <stdio.h>
#include <math.h>
#include <time.h>

// --- GLOBALES ALDEANOS ---
Aldeano aldeanos[MAX_ALDEANOS];
int totalAldeanos = 0;

void inicializarAldeanos() {
    totalAldeanos = 0;
    for(int i=0; i<MAX_ALDEANOS; i++) aldeanos[i].active = 0;
}

void comprarAldeano() {
    if (totalAldeanos >= MAX_ALDEANOS) return;
    if (jugOro >= 50) {
        jugOro -= 50;
        int idx = -1;
        for(int i=0; i<MAX_ALDEANOS; i++) { if(!aldeanos[i].active) { idx=i; break; } }
        if (idx != -1) {
            aldeanos[idx].active = 1;
            // Spawn cerca del cuartel (o jugador si no hay)
            int spawnX = jugX + 50; int spawnY = jugY + 50;
            // Buscar cuartel
            for(int k=0; k<totalObjetos; k++) {
                if(objTipo[k] == O_CUARTEL) { spawnX = objX[k]+50; spawnY = objY[k]+150; break; }
            }
            aldeanos[idx].x = (float)spawnX; aldeanos[idx].y = (float)spawnY;
            aldeanos[idx].estado = A_IDLE;
            aldeanos[idx].timer = 0;
            aldeanos[idx].direccion = 0; // Default frente
            aldeanos[idx].frame = 0;
            totalAldeanos++;
        }
    }
}

void actualizarAldeanos() {
    for(int i=0; i<MAX_ALDEANOS; i++) {
        if (!aldeanos[i].active) continue;

        Aldeano* a = &aldeanos[i];

        // --- ANIMACION ---
        if (a->estado == A_MOVIENDO || a->estado == A_RETORNANDO) {
            if ((clock() / 200) % 2 == 0) a->frame = 0; else a->frame = 1;
        } else {
            a->frame = 0;
        }

        // --- MAQUINA DE ESTADOS ---
        if (a->estado == A_IDLE) {
            // BUSCAR RECURSO CERCANO
            int mejorDist = 9999999;
            int mejorId = -1;
            
            for(int k=0; k<totalObjetos; k++) {
                if (objVisible[k] == 0) continue;
                int t = objTipo[k];
                // Es madera o comida?
                int esRecurso = 0;
                if (t==O_ARBOL || t==O_ARBOL2 || t==O_PINO || t==O_ARBUSTO || t==O_ARBOL10) esRecurso=1; // Madera
                // Falta aniadir animales si queremos, por ahora madera
                
                if (esRecurso) {
                     int dx = (int)a->x - objX[k]; int dy = (int)a->y - objY[k];
                     int d2 = dx*dx + dy*dy;
                     if (d2 < mejorDist) { mejorDist = d2; mejorId = k; }
                }
            }
            
            if (mejorId != -1) {
                a->targetId = mejorId;
                a->estado = A_MOVIENDO;
                // Definir tipo recurso para saber que sumar
                a->tipoRecurso = 0; // Madera default por ahora
            }
        }
        else if (a->estado == A_MOVIENDO) {
            // Verificar si el objetivo sigue existiendo
            if (objVisible[a->targetId] == 0) { a->estado = A_IDLE; continue; }
            
            int tx = objX[a->targetId] + 40; // Ir al centro aprox
            int ty = objY[a->targetId] + 100; // Base del arbol
            
            int dx = tx - (int)a->x;
            int dy = ty - (int)a->y;
            float dist = sqrtf((float)(dx*dx + dy*dy));
            
            if (dist < 5.0f) {
                // LLEGO
                a->estado = A_RECOLECTANDO;
                a->timer = 200; // Tiempo recolectando (simulado ciclos)
            } else {
                // MOVER
                float vx = (dx / dist) * 2.0f; // Vel 2 (Mas lento que 3)
                float vy = (dy / dist) * 2.0f;
                a->x += vx; a->y += vy;
                
                // Direccion
                if (fabsf((float)dx) > fabsf((float)dy)) {
                    if (dx > 0) a->direccion = 2; // Derecha
                    else a->direccion = 3; // Izquierda
                } else {
                    if (dy > 0) a->direccion = 0; // Frente
                    else a->direccion = 1; // Atras
                }
            }
        }
        else if (a->estado == A_RECOLECTANDO) {
            // Verificar si sigue visible (el jugador pudo haberlo talado)
            if (objVisible[a->targetId] == 0) { a->estado = A_IDLE; continue; }
            
            a->timer--;
            // Animacion "Taleo" basica (moverse izq der?)
            if (a->timer <= 0) {
                // TERMINO
                // Eliminar arbol
                objVisible[a->targetId] = 0;
                objTiempoRespawn[a->targetId] = time(NULL) + 30; // Respawn normal
                
                // Sumar Recurso (menos que el jugador)
                jugMadera += 1; 
                agregarEfectoVisual(EFECTO_MADERA, (int)a->x, (int)a->y - 50);
                
                a->estado = A_IDLE; // Buscar otro
            }
        }
    }
}

void dibujarAldeanos(HDC hdc, int camX, int camY, int zoom) {
    for(int i=0; i<MAX_ALDEANOS; i++) {
        if (!aldeanos[i].active) continue;
        
        // Culling basico
        int sx = ((int)aldeanos[i].x - camX) * zoom;
        int sy = ((int)aldeanos[i].y - camY) * zoom;
        
        // Sprite
        HBITMAP spr = hAldeanoFrente[aldeanos[i].frame];
        if (aldeanos[i].direccion == 1) spr = hAldeanoAtras[aldeanos[i].frame];
        else if (aldeanos[i].direccion == 2) spr = hAldeanoDerecha[aldeanos[i].frame];
        else if (aldeanos[i].direccion == 3) spr = hAldeanoLado[aldeanos[i].frame];
        
        dibujar(hdc, sx, sy, spr, 40*zoom, 60*zoom);
        
        // Barra Estado o Icono? (Opcional)
        if (aldeanos[i].estado == A_RECOLECTANDO) {
            // Dibujar hachita o algo?
        }
    }
}

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