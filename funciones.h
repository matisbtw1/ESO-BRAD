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
    char categoria[20];
    int monto;
    char estado[15]; // "pendiente", "pagado", etc.
    bool modificado; // Indica si el gasto ha sido modificado
} Gasto;

typedef struct {
    char nombreMes[12];
    int ingresos;
    int ahorrado;
    int totalGastos;
    int modificado;
    List *listaGastos;
} MesFinanciero;

int lower_than_string(void* key1, void* key2);
void copiarArchivoCSV(const char* origen, const char* destino);
void cargarMovimientosDesdeCSV(TreeMap *arbol, const char *nombreArchivo);
void mostrarMovimientosPorMes(TreeMap *arbol);
void mostrarMenu();
int mes_a_numero(const char* mes);
int lower_than_mes(void* key1, void* key2);
void registrarMovimientoFinanciero(TreeMap *arbol);
void marcarGastoComoPagado(TreeMap *arbol);
void mostrarPorcentajesPorCategorias(TreeMap *arbol);
void recuperarGastosPendientes(TreeMap *arbol);
void modificarGasto(TreeMap *arbol);
bool verificarArchivoExistente(int *anyo, char *nombreArchivo, int opcion);
void accionesAlCSV(TreeMap *arbol, bool *cargado);
void subMenuMostrarMovimientos(TreeMap *arbol);
void mostrarMesActual(TreeMap *arbol);


void tolowercase(char *str);
void guardarCSV(TreeMap *arbol, const char *nombreArchivo);


#endif