#ifndef FUNCIONES_H
#define FUNCIONES_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tdas/list.h"
#include "tdas/treemap.h"
#include "tdas/extra.h"
#include <stdbool.h>


typedef struct {
  char categoria[64];
  int valor;
  char estado[64];
} Movimiento;

typedef struct{
    char categoria[30];
    float limite;
    float usado;
} Presupuesto;


int lower_than_string(void* key1, void* key2);
void copiarArchivoCSV(const char* origen, const char* destino);
void cargarMovimientosDesdeCSV(TreeMap *arbol, const char *nombreArchivo);
void mostrarMovimientosPorMes(TreeMap *arbol);
void mostrarMenu();
int mes_a_numero(const char* mes);
int lower_than_mes(void* key1, void* key2);

#endif // FUNCIONES_H