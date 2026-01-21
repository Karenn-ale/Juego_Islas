#include "../include/registro.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

// Valida: Máximo 50 caracteres, permite espacios intermedios, 
// pero PROHÍBE espacios al inicio o al final.
bool EsNombreValido(const char* nombre) {
    if (nombre == NULL) return false;
    
    size_t largo = strlen(nombre);

    // 1. Validar que no esté vacío y no supere los 50 caracteres
    if (largo == 0 || largo > 50) {
        return false;
    }

    // 2. Verificar que el primer carácter no sea un espacio
    if (isspace((unsigned char)nombre[0])) {
        return false;
    }

    // 3. Verificar que el último carácter no sea un espacio
    if (isspace((unsigned char)nombre[largo - 1])) {
        return false;
    }

    return true;
}

bool VerificarExistencia(const char* nombre) {
    if (!EsNombreValido(nombre)) return false;

    char ruta[100];
    sprintf(ruta, "saves/%s.bin", nombre);
    FILE* f = fopen(ruta, "rb");
    if (f) {
        fclose(f);
        return true;
    }
    return false;
}

bool CrearRegistroBinario(const char* nombre) {
    if (!EsNombreValido(nombre)) return false;

    char ruta[100];
    sprintf(ruta, "saves/%s.bin", nombre);
    
    EstadoJuego nuevo = {0};
    nuevo.magic = 0x4947; 
    strncpy(nuevo.nombre, nombre, 50);
    nuevo.nombre[50] = '\0';
    
    nuevo.oro = 0;      
    nuevo.madera = 0;
    nuevo.comida = 0;
    nuevo.isla = 1; 
    nuevo.x = 400; 
    nuevo.y = 400;
    // genero se asignara despues al elegir, o podemos poner 0 por defecto
    nuevo.genero = 0;

    FILE* f = fopen(ruta, "wb");
    if (!f) return false;
    
    fwrite(&nuevo, sizeof(EstadoJuego), 1, f);
    fclose(f);
    return true;
}

bool GuardarJuego(const char* nombre, const EstadoJuego* datos) {
    if (!EsNombreValido(nombre)) return false;
    char ruta[100];
    sprintf(ruta, "saves/%s.bin", nombre);
    
    FILE* f = fopen(ruta, "wb");
    if (!f) return false;
    
    fwrite(datos, sizeof(EstadoJuego), 1, f);
    fclose(f);
    return true;
}

bool CargarDatosBinarios(const char* nombre, EstadoJuego* datos) {
    if (!EsNombreValido(nombre)) return false;

    char ruta[100];
    sprintf(ruta, "saves/%s.bin", nombre);
    
    FILE* f = fopen(ruta, "rb");
    if (!f) return false;

    fread(datos, sizeof(EstadoJuego), 1, f);
    fclose(f);
    
    return (datos->magic == 0x4947);
}

void GuardarEnHistorialTexto(const char* nombre, const char* evento) {
    if (!EsNombreValido(nombre)) return;

    FILE* f = fopen("saves/historial.txt", "a");
    if (f) {
        fprintf(f, "JUGADOR: %s | EVENTO: %s\n", nombre, evento);
        fclose(f);
    }
}