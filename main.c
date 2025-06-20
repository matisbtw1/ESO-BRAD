#include    <stdio.h>
#include    <stdlib.h>
#include    <string.h>
#include    <ctype.h>
#include    <stdbool.h>
#include    <unistd.h>
#include    <windows.h> 

#include    "tdas/extra.h"
#include    "tdas/list.h"
#include    "funciones.h"

/* MENÚ PRINCIPAL
francis
1. Registrar movimiento financiero
   1.1 Registrar ingreso 
   1.2 Registrar gasto 
alfonso
aca separar el ver por mes o los ultimos 3 meses etc
2. Ver resumen mensual
   2.1 Ver totales y saldo
   2.2 Ver porcentajes por categoría
   2.3 Ver excedente mensual
por eliminar y/o modificar
3. Presupuesto mensual 
   3.1 Establecer presupuesto
   3.2 Ver presupuesto vs gasto
matis
4. Gastos planificados
   4.1 Ver gastos pendientes
   4.2 Marcar gasto como pagado
jp
5. Historial y análisis
CUALKIER ANALISIS matematico Q SEA UTIL
   5.1 Ver meses anteriores
   5.2 Comparar meses
   5.3 Tendencias de gasto

entre todos
6. cuenta de ahorro
   6.1 Revisar cuenta de ahorro

interfaz y arreglar prints 




0. Salir
*/

// funciones.c hay una funcion que copia archivo csv, ahi esta la plantilla de finanzas, con esa funcion toma la plantilla y crea un archivo nuevo con el año que ingresa el usuario
// Implementacion para crear un csv de finanzas del año ingresado por el usuario, esta funcion

int main()
{
    SetConsoleOutputCP(CP_UTF8); // Configurar la consola para UTF-8
   int opcion;
   TreeMap *arbol = createTreeMap(lower_than_mes); // Crear el árbol para almacenar los movimientos financieros
   mostrarMenu();
   bool cargado = false; // Variable para verificar si se ha cargado un archivo CSV
   while (true){
   scanf("%d", &opcion);
   switch (opcion) { 
       case 1:
           if (!cargado) 
           {
            printf("primero carga un archivo de finanzas\n");
            break;
           }
           printf("Registrar movimiento financiero\n");
           // Aquí iría la lógica para registrar un movimiento financiero
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
            marcarGastoComoPagado(arbol); // Marcar un gasto como pagado o pendiente
            mostrarMenu();
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
           if (cargado) 
           {
            printf("Ya se ha cargado un archivo de finanzas.\n");
            break;
           }
           printf("Ingresa el año para cargar archivo de finanzas: ");
           int anyo;
           scanf("%d", &anyo);
           
           char leerArchivo[100];
           sprintf(leerArchivo, "finanzas_%d.csv", anyo);
           
           FILE *verificar = fopen(leerArchivo, "r");
           if (!verificar) {
               printf("No se pudo abrir el archivo %s. Asegúrate de que exista.\n", leerArchivo);
               break;
           }
           fclose(verificar);
           cargado = true; // Marcar que se ha cargado un archivo CSV
           cargarMovimientosDesdeCSV(arbol, leerArchivo); // Cargar movimientos desde un archivo CSV
           printf("Archivo de finanzas cargado correctamente.\n");
           break;
       case 9:
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
       case 10:
           printf("Guardar archivo de finanzas\n");
           guardarCSV(arbol, "finanzas_2025.csv"); // Guardar los movimientos financieros en un archivo CSV
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













































































































































































