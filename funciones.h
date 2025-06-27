#ifndef FUNCIONES_H
#define FUNCIONES_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tdas/list.h"
#include "tdas/treemap.h"
#include "tdas/extra.h"
#include <stdbool.h>
#include <windows.h> // Para SetConsoleOutputCP en Windows
#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"
#define RESET   "\x1b[0m"
#define BOLD    "\x1b[1m"


typedef struct {
    char categoria[20]; // "Alimentación", "Transporte", "Salud", etc.
    int monto;
    char estado[15]; // "Pendiente", "Pagado", "Recuperado".
    bool modificado; // Indica si el gasto ha sido modificado
} Gasto;

typedef struct {
    char nombreMes[12];
    int ingresos; // Ingresos del mes
    int ahorrado; 
    int totalGastos; 
    int modificado; // Indica si el mes ha sido modificado
    List *listaGastos; // Lista de gastos del mes
} MesFinanciero;

// Esta funcion se encarga de habilitar los colores ANSI en la consola de Windows
// para que se puedan mostrar los mensajes con colores.
void habilitarColoresANSI();

// Esta funcion sirve para copiar un archivo CSV de origen a destino.
// Se utiliza para copiar una plantilla de archivo CSV y crear un nuevo archivo
// para registrar los movimientos financieros.
void copiarArchivoCSV(const char* origen, const char* destino);

// Esta funcion se encarga de cargar los movimientos financieros desde un archivo CSV
// y almacenarlos en un árbol de búsqueda (TreeMap).
void cargarMovimientosDesdeCSV(TreeMap *arbol, const char *nombreArchivo);

// Esta funcion se encarga de mostrar los movimientos financieros almacenados en el árbol de búsqueda.
// Se utiliza para visualizar los gastos, ingresos y ahorros registrados.
// En el main ya no es llamada por lo que se usó solo para las primeras pruebas.
void mostrarMovimientosPorMes(TreeMap *arbol);

// Esta funcion muestra el menú principal de la aplicación.
void mostrarMenu();

// Esta funcion pasa un mes a su respectivo número.
// Por ejemplo, "Enero" se convierte en 1, "Febrero" en 2, etc.
// Se utiliza para facilitar la comparación y ordenación de meses en el árbol de búsqueda.
int mes_a_numero(const char* mes);

// Esta funcion se utiliza en el arbol de búsqueda para comparar dos claves (meses).
int lower_than_mes(void* key1, void* key2);

// Esta función se encarga de registrar un nuevo mes financiero en el árbol de búsqueda.
void registrarMovimientoFinanciero(TreeMap *arbol);

// Esta funcion se encarga de modificar el estado de un gasto de un mes financiero.
void marcarGastoComoPagado(TreeMap *arbol);

// Esta funcion muestra el porcentaje de gastos por categoría en un mes financiero.
void mostrarPorcentajesPorCategorias(TreeMap *arbol);

// Esta funcion se encarga de recueperar los gastos pendientes de un mes financiero.
// Permite mandar los gastos pendientes al mes siguiente del ultimo mes resgistrado/cerrado.
void recuperarGastosPendientes(TreeMap *arbol);

// Esta funcion muestra el submenu para la opcion de modificar/recuperar gastos.
void subMenumodificarGasto(TreeMap *arbol);

// Esta funcion verifica si un archivo ya existe en el sistema.
bool verificarArchivoExistente(int *anyo, char *nombreArchivo, int opcion);

// Esta funcion se encarga de mostrar un submenú donde muestras las acciones a realizar a un CSV.
void accionesAlCSV(TreeMap *arbol, bool *cargado);

// Esta funcion muestra un submenu con las distintas opciones para mostrar los movimientos financieros 
// registrados en el árbol de búsqueda.
void subMenuMostrarMovimientos(TreeMap *arbol);

// Esta funcion se encarga de mostrar el mes actual y sus movimientos financieros.
void mostrarMesActual(TreeMap *arbol);

// Esta funcion muestra los X ultimos meses registrados en el árbol de búsqueda.
void mostrarXmeses(TreeMap *arbol);

// Esta funcion muestra un submenu en el cual aparecen la opcion de registrar y eliminar un mes financiero.
void SubMenuRegistrarMovimiento(TreeMap *arbol);

// Esta funcion se encarga de eliminar un mes financiero del árbol de búsqueda. Iniciando sus valores por defecto.
void reiniciarMes(TreeMap *arbol);

// Esta funcion se encarga de leer una opción válida del usuario dentro de un rango específico.
// Se utiliza para asegurarse de que el usuario ingrese una opción válida en los menús
int leerOpcionValida(int min, int max);

// Esta funcion se encarga de mostrar el submenu de análisis financiero.
void subMenuAnalisis(TreeMap *arbol);

// Esta funcion compara los gastos entre meses, mostrando cuanto porcentaje más gastó respecto a un mes.
void compararGastosEntreMeses(TreeMap *arbol);

// Esta funcion muestra un submenu con las opciones de análisis financiero.
void submenuExcedenteMensual(TreeMap *arbol);

// Esta funcion permite ver al usuario el excedente mensual de un mes financiero.
void verAhorroMesAMes(TreeMap *arbol);

// Esta funcion permite al usuario ver el porcentaje de ahorro respecto a los ingresos por mes financiero.
void verPorcentajeAhorro(TreeMap *arbol);

// Funcion para limpiar la consola
void limpiarConsola();

// Funcion para pausar la consola y esperar a que el usuario presione Enter
void presionaEnter();

// Esta funcion se encarga de guardar los movimientos financieros en un archivo CSV.
// Se utiliza para persistir los datos y poder cargarlos en futuras ejecuciones del programa.
// El archivo sobre escribe el archivo existente correspondiente al año.
void guardarCSV(TreeMap *arbol, const char *nombreArchivo);


#endif