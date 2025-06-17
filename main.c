#include    <stdio.h>
#include    <stdlib.h>
#include    <string.h>
#include    <ctype.h>
#include    <stdbool.h>
#include    <unistd.h>

#include    "tdas/extra.h"
#include    "tdas/list.h"
#include    "tdas/treeMap.c"

/* MENÚ PRINCIPAL
1. Registrar movimiento financiero
   1.1 Registrar ingreso
   1.2 Registrar gasto
2. Ver resumen mensual
   2.1 Ver totales y saldo
   2.2 Ver porcentajes por categoría
   2.3 Ver excedente mensual
3. Presupuesto mensual
   3.1 Establecer presupuesto
   3.2 Ver presupuesto vs gasto
4. Gastos planificados
   4.1 Ver gastos pendientes
   4.2 Marcar gasto como pagado
5. Historial y análisis
   5.1 Ver meses anteriores
   5.2 Comparar meses
   5.3 Tendencias de gasto
6. Excedente mensual
   6.1 Enviar a cuenta de ahorro
   6.2 Usar como fondo de ocio
   6.3 Ver cuenta de ahorro
7. Buscar movimiento
   7.1 Por fecha
   7.2 Por categoría
   7.3 Por descripción
0. Salir
*/


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



// funciones.c hay una funcion que copia archivo csv, ahi esta la plantilla de finanzas, con esa funcion toma la plantilla y crea un archivo nuevo con el año que ingresa el usuario
// Implementacion para crear un csv de finanzas del año ingresado por el usuario, esta funcion

void mostrarMenu() {
    printf("Menu de opciones:\n");
    printf("1. Registrar movimiento financiero\n");
    printf("2. Ver resumen mensual\n");
    printf("3. Presupuesto mensual\n");
    printf("4. Gastos planificados\n");
    printf("5. Historial y análisis\n");
    printf("6. Excedente mensual\n");
    printf("7. Buscar movimiento\n");
    printf("0. Salir\n");
}





int main()
{

    return 0;
}













































































































































































