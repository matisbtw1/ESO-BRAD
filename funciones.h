#ifndef FUNCIONES_H
#define FUNCIONES_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tdas/list.h"

typedef struct {
    int dia;
    int mes;
    int anio;
} Fecha;

typedef struct {
    int id; // Identificador único del movimiento
    char tipo; // 'I' (ingreso), 'G' (gasto inmediato), 'P' (gasto planificado)
    char categoria[30];
    float monto; // Monto del movimiento
    bool pagado; // solo aplicable si es gasto planificado

} movimiento;

typedef struct Presupuesto {
    char categoria[30];
    float limite;
    float usado;
    struct Presupuesto* sig; // lista enlazada simple
} Presupuesto;


typedef struct MovimientoAhorro {
    Fecha fecha;
    float monto;
    char tipo[10]; // "DEPÓSITO" o "RETIRO"
    struct MovimientoAhorro* sig;
} MovimientoAhorro;

typedef struct Presupuesto {
    char categoria[30];
    float limite;
    float usado;
    struct Presupuesto* sig; // lista enlazada simple
} Presupuesto;


typedef struct {
    float saldo;
    MovimientoAhorro* historial; // lista enlazada
} CuentaAhorro;

void copiarArchivoCSV(const char* origen, const char* destino);

#endif // FUNCIONES_H