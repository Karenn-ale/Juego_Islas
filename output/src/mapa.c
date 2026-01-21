#include "../include/juego.h"
#include <stdio.h>
#include <stdlib.h> 
#include <math.h>   

// Función auxiliar
void agregarObjeto(int tipo, int x, int y) {
    if (totalObjetos >= MAX_OBJETOS) return;
    objTipo[totalObjetos] = tipo;
    objX[totalObjetos] = x;
    objY[totalObjetos] = y;
    totalObjetos++;
}

void crearMoneda(int x, int y, int valor) {
    int idx = -1;
    // Buscar slot vacio (reciclaje)
    for(int i=0; i<totalObjetos; i++) {
        if (objTipo[i] == O_NADA) {
            idx = i;
            break;
        }
    }
    // Si no hay vacio, nuevo slot
    if (idx == -1) {
        if (totalObjetos < MAX_OBJETOS) {
            idx = totalObjetos;
            totalObjetos++;
        } else {
            return; // Lleno
        }
    }
    objTipo[idx] = O_ITEM_ORO;
    objX[idx] = x;
    objY[idx] = y;
    objValor[idx] = valor;
    objVisible[idx] = 1;
}

// =========================================================
//   ISLA 1: BOSQUE
// =========================================================
void cargarIsla1() {
    agregarObjeto(O_CASTILLO, 1200, 200); 
    agregarObjeto(O_BARCO, 90, 200);     
    agregarObjeto(O_COMERCIANTE, 400, 600); 
    agregarObjeto(O_CUARTEL, 1390, 500); 
    agregarObjeto(O_HOGUERA, 300, 600); 

    // Árboles
    agregarObjeto(O_ARBOL, 780, 210); 
    agregarObjeto(O_ARBOL2, 700, 400); 
    agregarObjeto(O_ARBOL, 800, 600);
    agregarObjeto(O_ARBOL, 500, 300);
    agregarObjeto(O_ARBOL2, 1100, 400);
    agregarObjeto(O_ARBOL2, 500, 500); 
    agregarObjeto(O_ARBOL2, 1250, 550);
    agregarObjeto(O_ARBOL, 1000, 600);
    agregarObjeto(O_ARBOL2, 1050, 200);
    
    // Arbustos
    agregarObjeto(O_ARBUSTO, 850, 400); 
    agregarObjeto(O_ARBUSTO, 600, 480);
    agregarObjeto(O_ARBUSTO, 950, 450);
    agregarObjeto(O_ARBUSTO, 1000, 350);
    agregarObjeto(O_ARBUSTO, 1290, 450);
    agregarObjeto(O_ARBUSTO, 600, 700);
    agregarObjeto(O_ARBUSTO, 700, 300);
    agregarObjeto(O_ARBUSTO, 400, 300);
}

// =========================================================
//   ISLA 2: NIEVE (TU CONFIGURACIÓN ORIGINAL)
// =========================================================
void cargarIsla2() {
    agregarObjeto(O_BARCO, 150, 200); 
    agregarObjeto(O_MONTANA, 1500, 1500);
    
    // --- IGLÚS ---
    agregarObjeto(O_IGLU, 400, 300);
    agregarObjeto(O_IGLU, 600, 400);
    agregarObjeto(O_IGLU, 800, 350);

    // --- ÁRBOLES DE NIEVE (COORDENADAS ESPECÍFICAS) ---
    agregarObjeto(O_ARBOL_NIEVE1, 250, 500);
    agregarObjeto(O_ARBOL_NIEVE2, 550, 600);
    agregarObjeto(O_ARBOL_NIEVE1, 900, 550);
    agregarObjeto(O_ARBOL_NIEVE2, 1100, 450);
    agregarObjeto(O_ARBOL_NIEVE2, 300, 700);
    agregarObjeto(O_ARBOL_NIEVE2, 690, 690);
    agregarObjeto(O_ARBOL_NIEVE2, 1000, 300);
    agregarObjeto(O_ARBOL_NIEVE2, 1200, 200);
    agregarObjeto(O_ARBOL_NIEVE2, 900, 100);
    agregarObjeto(O_ARBOL_NIEVE2, 800, 100);
    agregarObjeto(O_ARBOL_NIEVE2, 1200, 700);
    agregarObjeto(O_ARBOL_NIEVE2, 1350, 500);
    agregarObjeto(O_ARBOL_NIEVE2, 1000, 700);
    agregarObjeto(O_ARBOL_NIEVE2, 500, 200);
    agregarObjeto(O_ARBOL_NIEVE2, 1300, 300);
}

// =========================================================
//   ISLA 3: VOLCÁN (RÉPLICA EXACTA DE ISLA 2 CON PALMERAS/CARPAS)
// =========================================================
// =========================================================
//   ISLA 3: VOLCÁN (RÉPLICA EXACTA DE ISLA 2 CON PALMERAS/CARPAS)
// =========================================================
void cargarIsla3() {
    agregarObjeto(O_BARCO, 150, 200); 
    agregarObjeto(O_MONTANA, 1500, 1500);
    
    // --- CARPAS (Exactamente igual a Isla 2) ---
    agregarObjeto(O_CARPA, 400, 300);
    agregarObjeto(O_CARPA, 600, 400);
    agregarObjeto(O_CARPA, 800, 350);

    // --- PALMERAS (Exactamente igual a Isla 2) ---
    agregarObjeto(O_PALMERA, 250, 500);
    agregarObjeto(O_PALMERA2, 550, 600);
    agregarObjeto(O_PALMERA, 900, 550);
    agregarObjeto(O_PALMERA2, 1100, 450);
    agregarObjeto(O_PALMERA2, 300, 700);
    agregarObjeto(O_PALMERA2, 690, 690);
    agregarObjeto(O_PALMERA2, 1000, 300);
    agregarObjeto(O_PALMERA2, 1200, 200);
    agregarObjeto(O_PALMERA2, 900, 100);
    agregarObjeto(O_PALMERA2, 800, 100);
    agregarObjeto(O_PALMERA2, 1200, 700);
    agregarObjeto(O_PALMERA2, 1350, 500);
    agregarObjeto(O_PALMERA2, 1000, 700);
    agregarObjeto(O_PALMERA2, 500, 200);
    agregarObjeto(O_PALMERA2, 1300, 300);
}

// =========================================================
//   ISLA 4 (COPIA DE ISLA 2 CON NUEVOS ASSETS)
// =========================================================
void cargarIsla4() {
    agregarObjeto(O_BARCO, 150, 200); 
    agregarObjeto(O_MONTANA, 1500, 1500);

    // --- CHOZAS (Exactamente igual a Isla 2) ---
    agregarObjeto(O_CHOZA3, 400, 300);
    agregarObjeto(O_CHOZA3, 600, 400);
    agregarObjeto(O_CHOZA3, 800, 350);

    // --- ÁRBOLES 10 (Exactamente igual a Isla 2) ---
    agregarObjeto(O_ARBOL10, 250, 500);
    agregarObjeto(O_ARBOL10, 550, 600);
    agregarObjeto(O_ARBOL10, 900, 550);
    agregarObjeto(O_ARBOL10, 1100, 450);
    agregarObjeto(O_ARBOL10, 300, 700);
    agregarObjeto(O_ARBOL10, 690, 690);
    agregarObjeto(O_ARBOL10, 1000, 300);
    agregarObjeto(O_ARBOL10, 1200, 200);
    agregarObjeto(O_ARBOL10, 900, 100);
    agregarObjeto(O_ARBOL10, 800, 100);
    agregarObjeto(O_ARBOL10, 1200, 700);
    agregarObjeto(O_ARBOL10, 1350, 500);
    agregarObjeto(O_ARBOL10, 1000, 700);
    agregarObjeto(O_ARBOL10, 500, 200);
    agregarObjeto(O_ARBOL10, 1300, 300);
}

// =========================================================
//   ISLA 5
// =========================================================
void cargarIsla5() {
    agregarObjeto(O_BARCO, 150, 200); 
    // AGREGAR ENTRADA A LA CUEVA
    agregarObjeto(O_ENTRADA_CUEVA, 1000, 500); 
}

// =========================================================
//   CUEVA (ISLA 6)
// =========================================================
void cargarCueva() {
    // --- ISLA 6: CUEVA DEL DRAGON ---
    // Colocamos la Calavera del Dragon protegiendo el oro
    // MAS PROFUNDO: Lejos de la entrada (Spawnea en 400, 800)
    // Movemos hacia la derecha y arriba.
    // Skull 600x600. Posicion X=1200, Y=50 (Arriba a la derecha)
    agregarObjeto(O_CALAVERA, 1200, 50); 

    // El Gran Tesoro (Debajo de la calavera)
    // Gold 200x200. X=1400 (Centrado bajo skull), Y=550
    agregarObjeto(O_ORO_GRANDE, 1400, 550);
    
    // Rocas bloqueando o decorando el camino
    agregarObjeto(O_ROCA, 800, 600); 
    agregarObjeto(O_ROCA, 1000, 300);
}

extern int generoSeleccionado;
extern char nombreJugadorGlobal[50];
extern int estadoGlobal;
extern int jugOro, jugMadera, jugComida, jugX, jugY; 
#include "../include/registro.h"
#include "../include/menu.h" // Para ST_JUEGO_INICIADO

void GuardarPartidaActual() {
    if (estadoGlobal != ST_JUEGO_INICIADO) return;
    if (strlen(nombreJugadorGlobal) == 0) return;

    EstadoJuego est;
    est.magic = 0x4947;
    strcpy(est.nombre, nombreJugadorGlobal);
    est.oro = jugOro;
    est.madera = jugMadera;
    est.comida = jugComida;
    est.isla = islaActual;
    est.x = jugX;
    est.y = jugY;
    est.genero = generoSeleccionado;

    GuardarJuego(nombreJugadorGlobal, &est);
}

void cambiarIsla(int numero) {
    // AUTO-SAVE al cambiar de isla
    if (islaActual != numero && estadoGlobal == ST_JUEGO_INICIADO) {
         GuardarPartidaActual();
    }

    totalObjetos = 0;
    islaActual = numero;
    
    if (numero == 1) cargarIsla1();
    else if (numero == 2) cargarIsla2();
    else if (numero == 3) cargarIsla3();
    else if (numero == 4) cargarIsla4();
    else if (numero == 5) cargarIsla5();
    else if (numero == 6) cargarCueva(); // CUEVA
}

void inicializarMundo() {
    cambiarIsla(1);
}