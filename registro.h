#ifndef REGISTRO_H
#define REGISTRO_H

#include <stdbool.h>

typedef struct {
    unsigned short magic;
    char nombre[51]; // 50 caracteres + nulo
    int oro;
    int madera;
} EstadoJuego;

// Prototipos
bool EsNombreValido(const char* nombre);
bool VerificarExistencia(const char* nombre);
bool CrearRegistroBinario(const char* nombre);
bool CargarDatosBinarios(const char* nombre, EstadoJuego* datos);
void GuardarEnHistorialTexto(const char* nombre, const char* evento);

#endif