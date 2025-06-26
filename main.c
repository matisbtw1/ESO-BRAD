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
    habilitarColoresANSI(); // Habilitar colores ANSI en la consola
    SetConsoleOutputCP(CP_UTF8); // Configurar la consola para UTF-8

   int opcion;
   TreeMap *arbol = createTreeMap(lower_than_mes); // Crear el árbol para almacenar los movimientos financieros
   bool cargado = false; // Variable para verificar si se ha cargado un archivo CSV
   while (true){
    limpiarConsola(); // Limpiar la consola antes de mostrar el menú
    mostrarMenu(); // Mostrar el menú principal
   opcion = leerOpcionValida(0, 6); 
   switch (opcion) { 
       case 1:
           limpiarConsola(); // Limpiar la consola antes de registrar un movimiento financiero
           if (!cargado) 
           {
            printf("[!] Primero carga un archivo de finanzas\n");
            presionaEnter(); // Esperar a que el usuario presione Enter antes de continuar
            break;
           }
           printf("Registrar movimiento financiero\n");
           // Aquí iría la lógica para registrar un movimiento financiero
           SubMenuRegistrarMovimiento(arbol); // Llamar a la función para registrar un movimiento financiero
           presionaEnter(); // Esperar a que el usuario presione Enter antes de continuar
           mostrarMenu();  // Mostrar el menú después de registrar un movimiento
           break;
       case 2:
           limpiarConsola(); // Limpiar la consola antes de ver el resumen mensual
           // Aquí iría la lógica para ver el resumen mensual
              if (!cargado) 
              {
                printf("[!] Primero carga un archivo de finanzas\n");
                presionaEnter(); // Esperar a que el usuario presione Enter antes de continuar
                break;
              }
           subMenuMostrarMovimientos(arbol); // Mostrar los movimientos financieros por mes
           presionaEnter(); // Esperar a que el usuario presione Enter antes de continuar
              mostrarMenu();
           break;
       case 3:
            limpiarConsola(); // Limpiar la consola antes de modificar un gasto
            if (!cargado) 
            {
                printf("[!] Primero carga un archivo de finanzas\n");
                presionaEnter(); // Esperar a que el usuario presione Enter antes de continuar
                break;
            }
            subMenumodificarGasto(arbol); // Modificar un gasto existente
            presionaEnter(); // Esperar a que el usuario presione Enter antes de continuar
            mostrarMenu();
            break;
       case 4:
           limpiarConsola(); // Limpiar la consola antes de mostrar el submenú de análisis financiero
           if (!cargado) 
           {
               printf("[!] Primero carga un archivo de finanzas\n");
               presionaEnter(); // Esperar a que el usuario presione Enter antes de continuar
               break;
           }
           subMenuAnalisis(arbol); // Mostrar el submenú de análisis financiero
           presionaEnter(); // Esperar a que el usuario presione Enter antes de continuar
           mostrarMenu(); // Mostrar el menú después de realizar acciones de análisis
           break;
       case 5:
           limpiarConsola(); // Limpiar la consola antes de manejar el excedente mensual
           if (!cargado) 
           {
               printf("[!] Primero carga un archivo de finanzas\n");
               presionaEnter(); // Esperar a que el usuario presione Enter antes de continuar
               break;
           }
           submenuExcedenteMensual(arbol); // Mostrar el submenú para manejar el excedente mensual
           presionaEnter(); // Esperar a que el usuario presione Enter antes de continuar
           break;
       case 6:
           limpiarConsola(); // Limpiar la consola antes de mostrar el submenú de acciones con CSV
           accionesAlCSV(arbol, &cargado); // Cargar un archivo de finanzas
           presionaEnter(); // Esperar a que el usuario presione Enter antes de continuar
           mostrarMenu(); // Mostrar el menú después de realizar acciones con el CSV
           break;
       case 0:
           printf("Salir del programa\n");
           return 0; // Salir del programa
       default:
           printf("Opción no válida. Intente nuevamente.\n");
           presionaEnter(); // Esperar a que el usuario presione Enter antes de continuar
           limpiarConsola(); // Limpiar la consola antes de mostrar el menú nuevamente
      }
   }

   return 0;
}













































































































































































