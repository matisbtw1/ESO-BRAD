#include    <stdio.h>
#include    <stdlib.h>
#include    <string.h>
#include    <ctype.h>
#include    <stdbool.h>
#include    <unistd.h>

#include    "tdas/extra.h"
#include    "tdas/list.h"
#include    "funciones.h"

/* MENÚ PRINCIPAL
1. Registrar movimiento financiero
   1.1 Registrar ingreso ✅
   1.2 Registrar gasto ✅
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
   6.1 Revisar cuenta de ahorro

0. Salir
*/

// funciones.c hay una funcion que copia archivo csv, ahi esta la plantilla de finanzas, con esa funcion toma la plantilla y crea un archivo nuevo con el año que ingresa el usuario
// Implementacion para crear un csv de finanzas del año ingresado por el usuario, esta funcion

int main()
{
   int opcion;
   TreeMap *arbol = createTreeMap(lower_than_mes); // Crear el árbol para almacenar los movimientos financieros
   mostrarMenu();
   while (true){
   scanf("%d", &opcion);
   switch (opcion) {
       case 1:
           printf("Registrar movimiento financiero\n");
           // Aquí iría la lógica para registrar un movimiento financiero
           cargarMovimientosDesdeCSV(arbol, "finanzas_2025.csv"); // Cargar movimientos desde un archivo CSV
           registrarMovimientoFinanciero(arbol); // Registrar un nuevo movimiento financiero
           break;
       case 2:
           printf("Ver resumen mensual\n");
           // Aquí iría la lógica para ver el resumen mensual
              mostrarMovimientosPorMes(arbol); // Mostrar los movimientos por mes
              mostrarMenu();
           break;
       case 3:
           printf("Presupuesto mensual\n");
           // Aquí iría la lógica para manejar el presupuesto mensual
           break;
       case 4:
           printf("Gastos planificados\n");
           // Aquí iría la lógica para manejar los gastos planificados
           break;
       case 5:
           printf("Historial y análisis\n");
           // Aquí iría la lógica para el historial y análisis
           break;
       case 6:
           printf("Excedente mensual\n");
           // Aquí iría la lógica para manejar el excedente mensual
           break;
       case 7:
           printf("Ver cuenta de ahorro\n");
           // Aquí iría la lógica para ver la cuenta de ahorro
           break;
       case 8:
           printf("Crear nuevo csv finanzas anio\n");
           int anio;
           printf("Ingresa el año para crear archivo de finanzas: ");
           scanf("%d", &anio);
           
           char nombreArchivo[100];
           sprintf(nombreArchivo, "finanzas_%d.csv", anio);
           
           // Verificar si ya existe
           FILE *archivo = fopen(nombreArchivo, "r");
           if (archivo) {
            printf("El archivo ya existe, no se sobreescribirá.\n");
            fclose(archivo);
            }
            else {
               copiarArchivoCSV("plantilla.csv", nombreArchivo);
            }
           break;
       case 0:
           printf("Salir del programa\n");
           return 0; // Salir del programa
       default:
           printf("Opción no válida. Intente nuevamente.\n");
      }
   }

   return 0;
}













































































































































































