#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "tdas/extra.h"
#include "tdas/list.h"
#include "funciones.h"

#ifdef _WIN32
#include <windows.h>
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif
#endif


//funcion para habilitar colores ANSI en la consola de Windows
void habilitarColoresANSI() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
}

//definición de los nombres de los meses y categorías de gastos
const char* nombresMeses[12] = {
    "Enero", "Febrero", "Marzo", "Abril", "Mayo", "Junio",
    "Julio", "Agosto", "Septiembre", "Octubre", "Noviembre", "Diciembre"
};

const char* categoriasGastos[12] = {
    "Agua", "Luz", "Gas", "Alimentacion", "Vivienda",
    "Transporte", "Conectividad", "Vestuario", "Salud", "Otros Gastos", NULL
};

void limpiarConsola() {
#ifdef _WIN32
    system("cls"); // Limpia la consola en Windows
#else
    system("clear");
#endif
}

void presionaEnter() {
    printf("\nPresiona Enter para continuar...");
    while (getchar() != '\n'); // limpia buffer
    getchar(); // espera enter
}

void mostrarMenu() {

    printf("\n"); //funcion para imprimir el menu en la consola
    printf(GREEN"╔════════════════════════════════════════════════════════════╗\n");
    printf("║         SISTEMA DE GESTIÓN FINANCIERA PERSONAL (SGFP)      ║\n");
    printf("╠════════════════════════════════════════════════════════════╣\n");
    printf("║                                                            ║\n");
    printf("║  1. Registrar Movimiento Financiero                        ║\n");
    printf("║  2. Ver Resumen Mensual                                    ║\n");
    printf("║  3. Gestionar Gastos                                       ║\n");
    printf("║  4. Historial y Análisis                                   ║\n");
    printf("║  5. Ver Excedente Mensual                                  ║\n");
    printf("║  6. Acciones de Archivo CSV (Cargar/Crear/Guardar)         ║\n");
    printf("║                                                            ║\n");
    printf("║  0. Salir                                                  ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n");
    
}

void copiarArchivoCSV(const char* origen, const char* destino) {
    FILE *fileOrigen = fopen(origen, "r"); //abrir el archivo origen en modo lectura
    if (fileOrigen == NULL) {
        printf("Error: no se pudo abrir el archivo base '%s'.\n", origen); //si no se puede abrir el archivo origen, imprimir un mensaje de error
        return;
    }

    FILE *fileDestino = fopen(destino, "w"); //abrir el archivo destino en modo escritura
    if (fileDestino == NULL) {
        printf("Error: no se pudo crear el archivo destino '%s'.\n", destino); //si no se puede crear el archivo destino, imprimir un mensaje de error
        fclose(fileOrigen); //cerrar el archivo origen
        return;
    }

    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), fileOrigen)) { //leer el archivo origen línea por línea
        fputs(buffer, fileDestino); //escribir cada línea en el archivo destino
    }
    // cerrar ambos archivos
    fclose(fileOrigen);
    fclose(fileDestino);
    printf("Archivo '%s' creado correctamente desde '%s'.\n", destino, origen);
}

void cargarMovimientosDesdeCSV(TreeMap *arbol, const char *nombreArchivo) {
    FILE *archivo = fopen(nombreArchivo, "r"); // Abrir el archivo CSV en modo lectura
    if (!archivo) {
        printf("No se pudo abrir el archivo.\n");
        return;
    }

    // Leer encabezado
    char **encabezadosTemp = leer_linea_csv(archivo, ',');
    if (!encabezadosTemp) return;

    // Contar cuántos encabezados hay
    int numCampos = 0;
    while (encabezadosTemp[numCampos] != NULL) numCampos++;

    // Reservar memoria para copia de encabezados
    char **encabezados = malloc((numCampos + 1) * sizeof(char *));
    for (int i = 0; i < numCampos; i++) {
      encabezados[i] = strdup(encabezadosTemp[i]);
    }
    encabezados[numCampos] = NULL; // último es NULL

    int idxMes = -1, idxIngreso = -1, idxAhorrado = -1, idxTotalGastos = -1, idxModificado = -1; // Índices de los campos
    int idxInicioGastos = -1; // Índice del primer gasto
    int idxFinGastos = -1; // Índice del último gasto

    int totalColumnas = 0;
    while (encabezados[totalColumnas] != NULL) totalColumnas++; // Contar total de columnas

    for (int i = 0; i < totalColumnas; i++) { // Iterar sobre los encabezados
        // si el encabezado es "Mes", "Ingreso", "Ahorrado", "Total Gastos" o "Modificado", guardar su índice
        if (strcmp(encabezados[i], "Mes") == 0) idxMes = i;
        else if (strcmp(encabezados[i], "Ingreso") == 0) idxIngreso = i;
        else if (strcmp(encabezados[i], "Ahorrado") == 0) idxAhorrado = i;
        else if (strcmp(encabezados[i], "Total Gastos") == 0) idxTotalGastos = i;
        else if (strcmp(encabezados[i], "Modificado") == 0) idxModificado = i;
        else if (idxInicioGastos == -1 && strcmp(encabezados[i], "Agua") == 0) idxInicioGastos = i;
    }

    // Leer línea por línea
    char **campos;
    while ((campos = leer_linea_csv(archivo, ',')) != NULL) {
        // Crear estructura del mes
        MesFinanciero *mes = malloc(sizeof(MesFinanciero));
        // Inicializar campos del mes
        strcpy(mes->nombreMes, campos[idxMes]);
        mes->ingresos = atoi(campos[idxIngreso]);
        mes->ahorrado = atoi(campos[idxAhorrado]);
        mes->totalGastos = atoi(campos[idxTotalGastos]);
        mes->modificado = strcmp(campos[idxModificado], "Si") == 0 ? 1 : 0;
        mes->listaGastos = list_create();

        // Leer gastos
        for (int i = idxInicioGastos; i < idxTotalGastos; i += 2) {
            if (campos[i] == NULL || campos[i + 1] == NULL) break;
            Gasto *g = malloc(sizeof(Gasto));
            strcpy(g->categoria, encabezados[i]); // Nombre viene del header
            g->monto = atoi(campos[i]); // Monto viene del CSV
            strcpy(g->estado, campos[i + 1]); // Estado viene del CSV
            g->modificado = false; // Inicializar modificado como false

            list_pushBack(mes->listaGastos, g);
        }

        // Insertar en árbol
        insertTreeMap(arbol, mes->nombreMes, mes);
        
        
    }
        for (int i = 0; encabezados[i] != NULL ; i++) {
            free(encabezados[i]); // Liberar memoria de los encabezados
        }
    fclose(archivo); // Cerrar el archivo CSV
}
void mostrarXmeses(TreeMap *arbol) {
    int x; // Variable para almacenar la cantidad de meses a mostrar
    printf(CYAN"\n¿Cuántos 'x' últimos meses desea ver? "RESET);
    scanf("%d", &x); // Leer la cantidad de meses a mostrar
    int indiceMesActual = -1;
    //el siguiente ciclo se encarga de buscar el último mes modificado o mes actual
    for (int i = 11; i >= 0; i--) { // Iterar desde el último mes (Diciembre) hasta el primero (Enero)
        Pair *par = searchTreeMap(arbol, (void*)nombresMeses[i]);
        if (par != NULL) {
            MesFinanciero *mes = (MesFinanciero*)par->value;
            if (mes->modificado == 1) { // Verificar si el mes ha sido modificado
                indiceMesActual = i; // Guardar el índice del mes actual
                break;
            }
        }
    }
    if (indiceMesActual == -1) { // Si no se encontró ningún mes modificado
        printf(YELLOW"[!] No hay meses modificados para mostrar.\n"RESET);
        return;
    }

    if (x > indiceMesActual + 1) x = indiceMesActual + 1; // Asegurarse de que no se soliciten más meses de los que existen
    if (x <= 0) {
        printf(YELLOW"[!] Cantidad inválida.\n"RESET);
        return;
    }

    int paginas = (x + 3) / 4; // Calcular el número de páginas necesarias, mostrando 4 meses por página
    int mostrados = 0;
    // el siguiente ciclo se encarga de mostrar los meses de 4 en 4, hasta llegar a la cantidad de meses solicitados
    for (int pagina = 1; pagina <= paginas; pagina++) {
        limpiarConsola(); // Limpia la consola antes de mostrar cada página

        int inicio = mostrados + 1;
        int fin = (mostrados + 4 < x) ? mostrados + 4 : x;
        printf(CYAN"\n--- Página %d de %d (meses %d-%d de %d) ---\n"RESET, pagina, paginas, inicio, fin, x);
        // Mostrar los meses correspondientes a la página actual
        for (int j = 0; j < 4 && mostrados < x; j++, mostrados++) {
            int i = indiceMesActual - (x-1) + mostrados;
            printf(CYAN"\n╔══════════════════════════════════════════════════════════╗\n"RESET);
            printf(BOLD"  MES: %s\n"RESET, nombresMeses[i]);
            Pair *par = searchTreeMap(arbol, (void*)nombresMeses[i]);
            if (par == NULL) { //validacion para verificar si el mes existe
                printf(YELLOW"  [!] No hay datos registrados para este mes.\n"RESET);
                printf(CYAN"╚══════════════════════════════════════════════════════════╝\n"RESET);
                continue;
            }
            MesFinanciero *mes = (MesFinanciero*)par->value;
            if (mes->modificado == 0) { //validacion para verificar si el mes ha sido modificado
                printf(YELLOW"  [!] El mes no ha sido modificado.\n"RESET);
                printf(CYAN"╚══════════════════════════════════════════════════════════╝\n"RESET);
                continue;
            }
            //para cada mes, se imprime la información del mes, ingresos, ahorro, total de gastos y las categorías registradas
            printf("  Ingresos:    "GREEN"%d\n"RESET, mes->ingresos);
            printf("  Ahorro:      "GREEN"%d\n"RESET, mes->ahorrado);
            printf("  Total Gastos:"GREEN"%d\n"RESET, mes->totalGastos);
            printf("  Categorías registradas:\n");

            int hayGastos = 0;
            Gasto *g = list_first(mes->listaGastos);
            //ciclo para recorrer la lista de gastos del mes y mostrar cada categoría, monto y estado
            while (g) {
                if (strcmp(g->estado, "Pendiente") == 0) {
                    printf("    - %-15s | Monto: " YELLOW "%-6d" RESET " | Estado: " RED "%s" RESET "\n", g->categoria, g->monto, g->estado);
                    hayGastos = 1;
                } else if (strcmp(g->estado, "Pagado") == 0) {
                    printf("    - %-15s | Monto: " YELLOW "%-6d" RESET " | Estado: " GREEN "%s" RESET "\n", g->categoria, g->monto, g->estado);
                    hayGastos = 1;
                } else if (strcmp(g->estado, "Recuperado") == 0) {
                    printf("    - %-15s | Monto: " YELLOW "%-6d" RESET " | Estado: \x1b[35m%s" RESET "\n", g->categoria, g->monto, g->estado);
                    hayGastos = 1;
                }
                g = list_next(mes->listaGastos);
            }
            if (!hayGastos) { //validacion para verificar si hay gastos registrados
                printf(YELLOW"    No hay gastos registrados para este mes.\n"RESET);
            }
            printf(CYAN"╚══════════════════════════════════════════════════════════╝\n"RESET);
        }
        printf(CYAN"\nMostrando meses %d-%d de %d. Página %d de %d.\n"RESET, inicio, fin, x, pagina, paginas);
        if (pagina < paginas) {
            presioneTeclaParaContinuar(); // Esperar a que el usuario presione Enter para continuar
        }
    }
}

void subMenuMostrarMovimientos(TreeMap *arbol){
// sub menu que hace llamado a las distinas funciones de visualizacion de los movimientos financieros
    int opcion;
    do {
        limpiarConsola(); // Limpiar la consola antes de mostrar el submenú
        printf("\n");
        printf(GREEN"╔════════════════════════════════════════════════════════════╗\n");
        printf("║                VISUALIZACIÓN DE MOVIMIENTOS                ║\n");
        printf("╠════════════════════════════════════════════════════════════╣\n");
        printf("║                                                            ║\n");
        printf("║  1. Mostrar movimientos por meses                          ║\n");
        printf("║  2. Mostrar mes actual                                     ║\n");
        printf("║                                                            ║\n");
        printf("║  0. Volver al menú principal                               ║\n");
        printf("║                                                            ║\n");
        printf("╚════════════════════════════════════════════════════════════╝\n"RESET);
        printf("\nIngrese su opción: ");
        scanf("%d", &opcion);

        switch (opcion) {
            case 1:
                limpiarConsola();
                mostrarXmeses(arbol); // Mostrar los últimos 'x' meses modificados
                presionaEnter(); // Esperar a que el usuario presione Enter antes de continuar
                break;
            case 2:
                limpiarConsola();
                mostrarMesActual(arbol); // Mostrar el mes actual (el último modificado)
                presionaEnter(); // Esperar a que el usuario presione Enter antes de continuar
                break;
            case 0:
                printf("Volviendo al menú principal...\n");
                break;
            default:
                printf("Opción no válida. Intente nuevamente.\n");
        }
    } while (opcion != 0);
}
void mostrarMesActual(TreeMap *arbol) {
    // Buscar el último mes modificado (el más reciente)
    MesFinanciero *mesActual = NULL;
    int indiceMesActual = -1;
    for (int i = 11; i >= 0; i--) { // iterar desde el último mes (Diciembre) hasta el primero (Enero)
        Pair *par = searchTreeMap(arbol, (void*)nombresMeses[i]); // buscar el mes en el árbol
        if (par != NULL) {
            MesFinanciero *mes = (MesFinanciero*)par->value;
            if (mes->modificado == 1) { // verificar si el mes ha sido modificado
                mesActual = mes; // asignar el mes actual
                indiceMesActual = i; // guardar el índice del mes actual
                break;
            }
        }
    }
    if (mesActual == NULL) { // si no se encontró ningún mes modificado
        printf(YELLOW"[!] No hay meses modificados para mostrar.\n"RESET);
        return;
    }
    printf(GREEN"\n╔══════════════════════════════════════════════════════════╗\n"RESET);
    printf(BOLD"  INFORMACIÓN DEL MES ACTUAL: %s\n"RESET, mesActual->nombreMes); //imprimir la info del mes actual
    printf(GREEN"╠══════════════════════════════════════════════════════════╣\n"RESET);
    printf("  Ingresos:    "GREEN"%d\n"RESET, mesActual->ingresos); // imprimir los ingresos del mes actual
    printf("  Ahorro:      "GREEN"%d\n"RESET, mesActual->ahorrado); // imprimir el ahorro del mes actual
    printf("  Total Gastos:"GREEN"%d\n"RESET, mesActual->totalGastos); // imprimir el total de gastos del mes actual
    printf("  Categorías registradas:\n");

    int hayGastos = 0;
    Gasto *g = list_first(mesActual->listaGastos);
    // recorrer la lista de gastos del mes actual y mostrar cada categoría, monto y estado
    while (g) {
        if (strcmp(g->estado, "No Registra") != 0) {
            if (strcmp(g->estado, "Pendiente") == 0) {
                printf("    - %-15s | Monto: "YELLOW"%-6d"RESET" | Estado: "RED"%s"RESET"\n", g->categoria, g->monto, g->estado);
            } else if (strcmp(g->estado, "Pagado") == 0) {
                printf("    - %-15s | Monto: "YELLOW"%-6d"RESET" | Estado: "GREEN"%s"RESET"\n", g->categoria, g->monto, g->estado);
            } else if (strcmp(g->estado, "Recuperado") == 0) {
                printf("    - %-15s | Monto: "YELLOW"%-6d"RESET" | Estado: "MAGENTA"%s"RESET"\n", g->categoria, g->monto, g->estado);
            } else {
                printf("    - %-15s | Monto: %-6d | Estado: %s\n", g->categoria, g->monto, g->estado);
            }
            hayGastos = 1;
        }
        g = list_next(mesActual->listaGastos); // avanzar al siguiente gasto
    }
    if (!hayGastos) { //validacion para verificar si hay gastos registrados
        printf(YELLOW"    No hay gastos registrados para este mes.\n"RESET);
    }
    printf(GREEN"╚══════════════════════════════════════════════════════════╝\n"RESET);
}
void mostrarMovimientosPorMes(TreeMap *arbol) {
    //funcion que se hizo para mostrar los movimientos financieros por mes y verificar si el mes ha sido modificado
    //actualmente se usan otras funciones para mostrar los movimientos financieros, pero esta funcion se dejo como referencia
    Pair *par = firstTreeMap(arbol); // Obtener el primer par del árbol
    while (par != NULL) {
    char *mes = (char *)par->key;
    MesFinanciero *mesFinanciero = (MesFinanciero *)par->value;
    printf("Mes: %s\n", mes);
    printf("Ingresos: %d\n", mesFinanciero->ingresos);
    printf("Ahorro: %d\n", mesFinanciero->ahorrado);
    printf("Total Gastos: %d\n", mesFinanciero->totalGastos);
    printf("Modificado: %s\n", mesFinanciero->modificado ? "Si" : "No");
    printf("-----------------------------------\n");
    List *lista = mesFinanciero->listaGastos;
    Gasto *mov = list_first(lista);
    while (mov != NULL) {
      printf("Categoría: %-15s | Valor: %6d | Estado: %s\n",
             mov->categoria, mov->monto, mov->estado);
      mov = list_next(lista);
    }
    printf("\n");
    par = nextTreeMap(arbol);
  }
}
int mes_a_numero(const char* mes) {
    // Función que convierte el nombre del mes a su número correspondiente
    const char* meses[] = {"enero","febrero","marzo","abril","mayo","junio","julio","agosto","septiembre","octubre","noviembre","diciembre"};
    for (int i = 0; i < 12; i++) {
        if (strcasecmp(mes, meses[i]) == 0) return i+1;
    }
    return 0; // No encontrado
}

int lower_than_mes(void* key1, void* key2) {
    int n1 = mes_a_numero((const char*)key1); // Convertir el primer mes a número
    int n2 = mes_a_numero((const char*)key2); // Convertir el segundo mes a número
    return n1 < n2;
}

void guardarCSV(TreeMap *arbol, const char *nombreArchivo) {
    FILE *archivo = fopen(nombreArchivo, "w"); // Abrir el archivo CSV en modo escritura
    if (!archivo) { // Validar si el archivo se abrió correctamente
        printf("Error al abrir el archivo para guardar.\n");
        return;
    }
    // Escribir encabezados
    fprintf(archivo, "Mes,Ingreso,Ahorrado");
    for (int i = 0; categoriasGastos[i] != NULL; i++) {
        fprintf(archivo, ",%s, Estado", categoriasGastos[i]);
    }
    fprintf(archivo, ",Total Gastos,Modificado\n"); // Escribir encabezados de gastos y estado

    //recorrer el árbol y escribir los datos
    Pair *par = firstTreeMap(arbol);
    while (par != NULL) {
        MesFinanciero *mes = (MesFinanciero *)par->value;
        fprintf(archivo, "%s,%d,%d", mes->nombreMes, mes->ingresos, mes->ahorrado);

        // Escribir gastos
        Gasto *gasto = list_first(mes->listaGastos);
        while (gasto != NULL) { // Recorrer la lista de gastos
            fprintf(archivo, ",%d,%s", gasto->monto, gasto->estado); // Escribir monto y estado del gasto
            gasto = list_next(mes->listaGastos); // Avanzar al siguiente gasto
        }

        fprintf(archivo, ",%d,%s\n", mes->totalGastos, mes->modificado ? "Si" : "No"); // Escribir total de gastos y si el mes ha sido modificado
        par = nextTreeMap(arbol); // Avanzar al siguiente par en el árbol
    } // Cerrar el archivo CSV
    fclose(archivo);
    printf("Datos guardados en %s\n", nombreArchivo);
}

void reiniciarMes(TreeMap *arbol){
    int opcionMes;
    MesFinanciero *datosMes = NULL; // Variable para almacenar los datos del mes seleccionado
    do {
        printf("Ingrese 0 para volver al menú principal.\n");
        printf("Seleccione el mes a reiniciar:\n");
        for (int i = 0; i < 12; i++) { // Mostrar los meses disponibles
            printf("%2d. %s\n", i + 1, nombresMeses[i]);
        }
        printf("Ingrese el número del mes (1-12): "); // Solicitar al usuario que ingrese el número del mes
        scanf("%d", &opcionMes);

        if (opcionMes == 0) {
            printf("Volviendo al menú principal...\n");
            return;
        }
        if (opcionMes < 1 || opcionMes > 12) { // Validar que la opción ingresada esté dentro del rango válido
            printf("Opción inválida. Intente nuevamente.\n");
            continue;
        }
        Pair *par = searchTreeMap(arbol, (void*)nombresMeses[opcionMes - 1]); // Buscar el mes seleccionado en el árbol
        if (!par) { // Validar si el mes existe en el árbol
            printf("El mes seleccionado no existe. Intente nuevamente.\n");
            continue;
        }
        datosMes = (MesFinanciero*)par->value; // Obtener los datos del mes seleccionado
        if (datosMes->modificado == 0) { // Validar si el mes ha sido modificado
            printf("El mes seleccionado no ha sido modificado. Seleccione otro mes.\n");
            datosMes = NULL;
            continue;
        }
        // Confirmación antes de reiniciar
        int confirmar = 0;
        limpiarConsola();
        printf(RED"\n¿Está seguro que desea reiniciar TODOS los datos del mes %s? (1=Sí, 0=No): "RESET, datosMes->nombreMes); // Solicitar confirmación al usuario
        scanf("%d", &confirmar); // Leer la respuesta del usuario
        if (confirmar != 1) { // Si el usuario no confirma, cancelar la operación
            printf("Operación cancelada. El mes no ha sido reiniciado.\n");
            datosMes = NULL;
            continue;
        }
    } while (datosMes == NULL); 

    // Reiniciar los datos del mes
    datosMes->ingresos = 0;
    datosMes->ahorrado = 0;
    datosMes->totalGastos = 0;
    datosMes->modificado = 0;

    Gasto *gasto = list_first(datosMes->listaGastos); // Obtener el primer gasto de la lista
    while (gasto != NULL) { // Recorrer la lista de gastos
        gasto->modificado = false; // Marcar como no modificado
        strcpy(gasto->estado, "No Registra");  // Cambiar el estado a "No Registra"
        gasto->monto = 0; // Reiniciar el monto
        gasto = list_next(datosMes->listaGastos); // Avanzar al siguiente gasto
    }
    printf(GREEN"El mes %s ha sido reiniciado exitosamente.\n"RESET, datosMes->nombreMes);
}

void SubMenuRegistrarMovimiento(TreeMap *arbol){
    int opcion;
    do {
        limpiarConsola(); // Limpiar la consola antes de mostrar el submenú
        printf("\n"); // imprimir el submenú de registro de movimientos financieros
        printf(GREEN"╔════════════════════════════════════════════════════════════╗\n");
        printf("║             REGISTRO Y GESTIÓN DE MOVIMIENTOS              ║\n");
        printf("╠════════════════════════════════════════════════════════════╣\n");
        printf("║                                                            ║\n");
        printf("║  1. Registrar movimiento financiero                        ║\n");
        printf("║  2. Reiniciar todos los datos de un mes                    ║\n");
        printf("║                                                            ║\n");
        printf("║  0. Volver al menú principal                               ║\n");
        printf("║                                                            ║\n");
        printf("╚════════════════════════════════════════════════════════════╝\n"RESET);
        printf("\nIngrese su opción: ");
        scanf("%d", &opcion);
        switch (opcion) {
            case 1:
                limpiarConsola();
                registrarMovimientoFinanciero(arbol); // Registrar un movimiento financiero
                presionaEnter(); // Esperar a que el usuario presione Enter antes de continuar
                break;
            case 2:
                limpiarConsola();
                reiniciarMes(arbol); // Reiniciar todos los datos de un mes
                presionaEnter(); // Esperar a que el usuario presione Enter antes de continuar
                break;
            case 0:
                printf("Volviendo al menú principal...\n");
                break;
            default:
                printf("Opción no válida. Intente nuevamente.\n");
        }
    } while (opcion != 0);
}
// Función para registrar movimiento financiero (sin presupuesto)
void registrarMovimientoFinanciero(TreeMap *arbol) {
    limpiarConsola(); // Limpiar la consola antes de mostrar el menú
    printf(GREEN"\n╔════════════════════════════════════════════════════════════╗\n"RESET);
    printf(BOLD"║              REGISTRAR MOVIMIENTO FINANCIERO               ║\n"RESET);
    printf(GREEN"╠════════════════════════════════════════════════════════════╣\n"RESET);
    printf("║  [INFO] Seleccione un mes para registrar sus ingresos y    ║\n");
    printf("║  gastos. Una vez finalizado, el mes se marcará como        ║\n");
    printf("║  'cerrado' y no podrá ser modificado. Si desea modificarlo ║\n");
    printf("║  debe eliminarlo y registrarlo nuevamente.                 ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n\n"RESET);

    for (int i = 0; i < 12; i++) { // Mostrar los meses disponibles
        printf("  %2d. %s\n", i + 1, nombresMeses[i]);
    }

    int opcionMes = 0;
    do {
        printf("\n>> Ingrese el número del mes (1-12): ");
        scanf("%d", &opcionMes);
    } while (opcionMes < 1 || opcionMes > 12);

    const char* mes = nombresMeses[opcionMes - 1]; // Obtener el nombre del mes seleccionado
    Pair* par = searchTreeMap(arbol, (void*)mes);
    MesFinanciero* datosMes = (MesFinanciero*)par->value; // Buscar los datos del mes en el árbol
    if (datosMes->modificado == 1) {
        printf(RED"[!] El mes %s ya ha sido cerrado. Si desea modificar el mes deberá eliminarlo y registrarlo nuevamente.\n"RESET, mes);
        return;
    }
    // Ingreso
    while (true) { // Bucle para ingresar ingresos
        limpiarConsola();
        int ingresosAux = 0;
        printf(CYAN"\nIngrese el monto de ingresos para el mes %s: "RESET, mes);
        scanf("%d", &ingresosAux); // Leer el monto de ingresos del usuario
        printf(">> ¿Confirma el ingreso de "YELLOW"%d"RESET" para el mes %s? ("GREEN"1 = Sí"RESET", "RED"0 = No"RESET"): ", ingresosAux, mes);
        int confirmarIngreso; // Variable para confirmar el ingreso
        scanf("%d", &confirmarIngreso);
        if (confirmarIngreso == 1) {
            datosMes->ingresos += ingresosAux; // Sumar los ingresos al mes
            datosMes->ahorrado += ingresosAux; // Sumar los ingresos al ahorro
            printf(GREEN"[OK] Ingreso de %d registrado para el mes %s.\n"RESET, ingresosAux, mes);
        } else {
            printf(YELLOW"[INFO] Ingreso no confirmado.\n"RESET);
        }
        if (confirmarIngreso == 1) break; // Salir del bucle si el ingreso fue confirmado
    }

    int opcionGasto;
    while (true) {
        // Mostrar menú actualizado de categorías
        printf(CYAN"\n╔══════════════════════════════════════════════════════════╗\n"RESET);
        printf(BOLD"  CATEGORÍAS DE GASTOS DEL MES %s\n"RESET, mes);
        printf(CYAN"╠══════════════════════════════════════════════════════════╣\n"RESET);

        int idx = 1;
        Gasto* gAux = list_first(datosMes->listaGastos);
        while (gAux) {
            printf("  %2d. %-15s | Monto: "YELLOW"%d"RESET" | Estado: %s\n", idx, gAux->categoria, gAux->monto, gAux->estado);
            gAux = list_next(datosMes->listaGastos);
            idx++;
        }
        printf(CYAN"╚══════════════════════════════════════════════════════════╝\n"RESET);

        printf("\nSeleccione una categoría para modificar ("YELLOW"0 para salir"RESET"): ");
        scanf("%d", &opcionGasto); // Leer la opción de gasto del usuario
        if (opcionGasto == 0) break;

        if (opcionGasto < 1 || opcionGasto >= idx) {
            printf(RED"Opción inválida. Intente nuevamente.\n"RESET);
            continue;
        }

        Gasto* g = list_first(datosMes->listaGastos); // Obtener el primer gasto de la lista
        for (int i = 1; i < opcionGasto; i++) {
            g = list_next(datosMes->listaGastos); // Avanzar al siguiente gasto hasta llegar al seleccionado
        }

        if (g->modificado) {
            printf(YELLOW"El gasto ya ha sido modificado. No se puede cambiar.\n"RESET); // Validar si el gasto ya ha sido modificado
            continue;
        }

        while (true) { // Bucle para modificar el monto del gasto
            int nuevoMonto; // Variable para almacenar el nuevo monto del gasto
            printf("Ingrese el nuevo monto para la categoría "BOLD"%s"RESET": ", g->categoria);
            scanf("%d", &nuevoMonto);
            printf("Confirma el cambio de monto a "YELLOW"%d"RESET" para la categoría %s? ("GREEN"1=Sí"RESET", "RED"0=No"RESET"): ", nuevoMonto, g->categoria);
            int confirmarCambio; // Variable para confirmar el cambio de monto
            scanf("%d", &confirmarCambio);
            if (confirmarCambio == 1) {
                g->monto = nuevoMonto;
            }
            if (confirmarCambio == 1) break;
        }

        // Cambiar estado del gasto
        int opcionEstado;
        printf("Seleccione el estado del gasto:\n");
        printf("  1. "RED"Pendiente"RESET"\n");
        printf("  2. "GREEN"Pagado"RESET"\n");

        do {
            printf("Ingrese su opción: "); // Solicitar opción de estado al usuario.
            scanf("%d", &opcionEstado); // Leer la opción de estado del usuario
        } while (opcionEstado < 1 || opcionEstado > 2);

        if (opcionEstado == 1) {
            strcpy(g->estado, "Pendiente"); // Cambiar el estado del gasto a "Pendiente"
        } else if (opcionEstado == 2) {
            if (datosMes->ahorrado < g->monto) {
                printf(RED"No hay suficiente dinero para pagar este gasto.\n"RESET);
                strcpy(g->estado, "Pendiente");
                printf(YELLOW"El gasto se ha dejado como pendiente.\n"RESET);
                continue;
            }
            strcpy(g->estado, "Pagado"); // Cambiar el estado del gasto a "Pagado"
            datosMes->ahorrado -= g->monto; // Restar el monto del gasto al ahorro del mes
            datosMes->totalGastos += g->monto; // Sumar el monto del gasto al total de gastos del mes
            printf(GREEN"Gasto marcado como pagado\n"RESET);
        }
        g->modificado = true; // Marcar el gasto como modificado
    }
    datosMes->modificado = 1; // Marcar el mes como modificado
    limpiarConsola();
    printf(GREEN"\n¡Movimiento financiero actualizado para %s!\nEl mes ha sido cerrado\n"RESET, mes);
}

void marcarGastoComoPagado(TreeMap *arbol) { // Función para marcar un gasto como pagado
    int opcionMes; // Variable para almacenar la opción del mes seleccionado
    MesFinanciero *datosMes = NULL;
    do {
        limpiarConsola();
        printf(GREEN"\n╔══════════════════════════════════════════════════════════╗\n"RESET);
        printf(BOLD"  MARCAR GASTO COMO PAGADO\n"RESET);
        printf(GREEN"╠══════════════════════════════════════════════════════════╣\n"RESET);
        printf("  Ingrese 0 para volver al menú principal.\n");
        printf("  Seleccione el mes para marcar gasto como pagado:\n");
        for (int i = 0; i < 12; i++) {
            printf("    %2d. %s\n", i + 1, nombresMeses[i]); // Mostrar los meses disponibles
        }
        printf(GREEN"╚══════════════════════════════════════════════════════════╝\n"RESET);
        printf("\nIngrese el número del mes (1-12): ");
        scanf("%d", &opcionMes); // Leer la opción del mes seleccionado

        if (opcionMes == 0) {
            printf(YELLOW"Volviendo al menú principal...\n"RESET); // Si el usuario ingresa 0, salir del bucle
            return;
        }
        if (opcionMes < 1 || opcionMes > 12) {
            printf(RED"Opción inválida. Intente nuevamente.\n"RESET); // Validar que la opción ingresada esté dentro del rango válido
            continue;
        }

        Pair *par = searchTreeMap(arbol, (void*)nombresMeses[opcionMes - 1]);
        if (!par) {
            printf(RED"El mes seleccionado no existe. Intente nuevamente.\n"RESET); // Validar si el mes existe en el árbol
            continue;
        }

        datosMes = (MesFinanciero*)par->value; // Obtener los datos del mes seleccionado
        if (datosMes->modificado == 0) {
            printf(YELLOW"El mes seleccionado no ha sido modificado. Seleccione otro mes.\n"RESET); 
            datosMes = NULL;
        }
    } while (datosMes == NULL);

    // Mostrar solo gastos pendientes
    limpiarConsola();
    printf(CYAN"\n╔══════════════════════════════════════════════════════════╗\n"RESET);
    printf(BOLD"  GASTOS PENDIENTES EN %s\n"RESET, datosMes->nombreMes);
    printf(CYAN"╠══════════════════════════════════════════════════════════╣\n"RESET);

    int idx = 1;
    int indicesPendientes[100]; // Asume máximo 100 gastos pendientes por mes
    Gasto* g = list_first(datosMes->listaGastos); // Obtener el primer gasto de la lista
    int hayPendientes = 0; 
    int i = 1;
    while (g) {
        if (strcmp(g->estado, "Pendiente") == 0) {
            printf("    %2d. %-15s | Monto: "YELLOW"%-6d"RESET" | Estado: "RED"%s"RESET"\n", idx, g->categoria, g->monto, g->estado);
            indicesPendientes[idx] = i; // Guarda el índice real en la lista
            idx++;
            hayPendientes = 1; // Indica que hay al menos un gasto pendiente
        }
        g = list_next(datosMes->listaGastos); // Avanzar al siguiente gasto
        i++;
    }
    if (!hayPendientes) { // Validar si hay gastos pendientes
        printf(YELLOW"    No hay gastos pendientes para el mes seleccionado.\n"RESET);
        printf(CYAN"╚══════════════════════════════════════════════════════════╝\n"RESET);
        return;
    }
    printf(CYAN"╚══════════════════════════════════════════════════════════╝\n"RESET);

    int opcionGasto; // Variable para almacenar la opción del gasto a marcar como pagado
    do {
        printf("\nSeleccione el gasto pendiente a marcar como pagado ("YELLOW"0 para salir"RESET"): "); 
        scanf("%d", &opcionGasto); // Leer la opción del gasto seleccionado
        if (opcionGasto == 0) break;

        if (opcionGasto < 1 || opcionGasto >= idx) {
            printf(RED"Opción inválida. Intente nuevamente.\n"RESET);
            continue;
        }

        // Buscar el gasto pendiente correspondiente
        g = list_first(datosMes->listaGastos); 
        int realIdx = indicesPendientes[opcionGasto]; // Obtener el índice real del gasto pendiente
        for (int j = 1; j < realIdx; j++) {
            g = list_next(datosMes->listaGastos); // Avanzar al siguiente gasto hasta llegar al seleccionado
        }

        if (strcmp(g->estado, "Pagado") == 0) {
            printf(YELLOW"El gasto ya está marcado como pagado.\n"RESET); // Validar si el gasto ya está marcado como pagado
            continue;
        }
        if (datosMes->ahorrado < g->monto) {
            printf(RED"No hay suficiente dinero para pagar este gasto.\n"RESET); // Validar si hay suficiente dinero para pagar el gasto
            strcpy(g->estado, "Pendiente"); 
            printf(YELLOW"El gasto se ha dejado como pendiente.\n"RESET);
            continue;
        }
        strcpy(g->estado, "Pagado"); // Cambiar el estado del gasto a "Pagado"
        printf(GREEN"Gasto marcado como pagado: %s | Monto: "YELLOW"%d"RESET"\n"RESET, g->categoria, g->monto);
        datosMes->ahorrado -= g->monto; // Descontar del ahorrado
        datosMes->totalGastos += g->monto; // Sumar al total de gastos del mes
    } while (opcionGasto != 0);

    printf(GREEN"\nOperación finalizada. Volviendo al menú de gestión de gastos.\n"RESET);
}

void recuperarGastosPendientes(TreeMap *arbol) { // Función para recuperar gastos pendientes de meses anteriores
    MesFinanciero *mesSigActual = NULL;
    int indiceMesActual = -1; 
    for (int i = 11; i >= 0; i--) { // Iterar desde el último mes (Diciembre) hasta el primero (Enero)
        Pair *par = searchTreeMap(arbol, (void*)nombresMeses[i]);
        if (par != NULL) {
            MesFinanciero *mes = (MesFinanciero*)par->value;
            if (mes->modificado == 1) { // Verificar si el mes ha sido modificado
                if (i + 1 > 12) { // Validar si hay un mes siguiente
                    printf("No hay meses siguientes para recuperar gastos pendientes.\n");
                    return;
                }
                Pair *parSig = searchTreeMap(arbol, (void*)nombresMeses[i + 1]); // Buscar el mes siguiente
                MesFinanciero *mesSig = (MesFinanciero*)parSig->value; 
                mesSigActual = mesSig; // Asignar el mes siguiente como destino de recuperación
                indiceMesActual = i+1; 
                break;
            }
            
        }
    }
    if (mesSigActual == NULL) {
        printf("No hay meses modificados para recuperar gastos pendientes.\n");
        return;
    }

    printf("Se usarán 'Otros Gastos' del mes %s como destino de recuperacion.\n", mesSigActual->nombreMes);

    Gasto *otrosActual = NULL;
    Gasto *gasto = list_first(mesSigActual->listaGastos); // Obtener el primer gasto de la lista del mes siguiente
    // Buscar el gasto de "Otros Gastos" en la lista del mes siguiente
    while (gasto != NULL) {
        if (strcmp(gasto->categoria, "Otros Gastos") == 0) {
            otrosActual = gasto;
            break;
        }
        gasto = list_next(mesSigActual->listaGastos); // Avanzar al siguiente gasto
    }

    for (int i = 0; i < indiceMesActual; i++) {
        Pair *par = searchTreeMap(arbol, (void*)nombresMeses[i]); // Buscar el mes en el árbol
        if (par == NULL) continue;

        MesFinanciero *mes = (MesFinanciero*)par->value;
        if (mes->modificado == 0) {
            printf("El mes %s no ha sido modificado, no se recuperarán gastos pendientes.\n", mes->nombreMes);
            continue;
        }

        int totalGastosPendientes = 0; // Variable para almacenar el total de gastos pendientes del mes
        // Recorrer la lista de gastos del mes para calcular el total de gastos pendientes
        Gasto *gastoPendiente = list_first(mes->listaGastos);
        while (gastoPendiente != NULL) {
            if (strcmp(gastoPendiente->estado, "Pendiente") == 0) {
                totalGastosPendientes += gastoPendiente->monto; // Sumar el monto del gasto pendiente al total
            }
            gastoPendiente = list_next(mes->listaGastos); 
        }
        if (totalGastosPendientes == 0) continue;

        int respuesta;
        printf("\nEl mes %s tiene %d en gastos pendientes.\n", mes->nombreMes, totalGastosPendientes);
        printf("¿Deseas Trasladarlos al mes actual (%s) como 'Otros Gastos'? (1=Sí, 0=No): ", mesSigActual->nombreMes);
        scanf("%d", &respuesta); // Leer la respuesta del usuario
        if (respuesta == 0) {
            printf("No se trasladarán los gastos pendientes del mes %s.\n", mes->nombreMes); 
            continue;
        }

        gastoPendiente = list_first(mes->listaGastos); 
        while (gastoPendiente != NULL) {
            if (strcmp(gastoPendiente->estado, "Pendiente") == 0) {
                otrosActual->monto += gastoPendiente->monto; // Sumar al total de "Otros Gastos"
                strcpy(gastoPendiente->estado, "Recuperado"); // Cambiar estado a "Recuperado"
            }
            gastoPendiente = list_next(mes->listaGastos);
        }
        printf("Gastos pendientes del mes %s trasladados a 'Otros Gastos' del mes %s.\n", mes->nombreMes, mesSigActual->nombreMes);

    }
    strcpy(otrosActual->estado, "Pendiente"); // Cambiar estado de "Otros Gastos" a "Recuperado"
    printf("\nRecuperación de gastos pendientes completada. Total final en 'Otros Gastos' : %d\n", otrosActual->monto);
}

void subMenumodificarGasto(TreeMap *arbol){
    int opcion;
    do {
        limpiarConsola(); // Limpiar la consola antes de mostrar el submenú
        printf("\n");
        printf(GREEN"╔════════════════════════════════════════════════════════════╗\n");
        printf("║                      GESTIONAR GASTOS                      ║\n");
        printf("╠════════════════════════════════════════════════════════════╣\n");
        printf("║                                                            ║\n");
        printf("║  1. Marcar un gasto pendiente como 'Pagado'                ║\n");
        printf("║  2. Recuperar gastos pendientes de meses anteriores        ║\n");
        printf("║                                                            ║\n");
        printf("║  0. Volver al menú principal                               ║\n");
        printf("║                                                            ║\n");
        printf("╚════════════════════════════════════════════════════════════╝\n"RESET);
        printf("\n  Ingrese su opción: ");
        scanf("%d", &opcion);   

        switch (opcion) {
            case 1:
                limpiarConsola();
                marcarGastoComoPagado(arbol); // Marcar un gasto pendiente como pagado
                presionaEnter(); // Esperar a que el usuario presione Enter antes de continuar
                break;
            case 2:
                limpiarConsola();
                recuperarGastosPendientes(arbol); // Recuperar gastos pendientes de meses anteriores
                presionaEnter(); // Esperar a que el usuario presione Enter antes de continuar
                break;
            case 0:
                printf("Volviendo al menú principal...\n");
                return;
            default:
                printf("Opción inválida. Intente nuevamente\n");
        }

    } while (opcion != 0);
    
}

void mostrarPorcentajesPorCategorias(TreeMap *arbol)
{
    int seleccionado[12] = {0}; 
    int cantidad = 0;

    printf(CYAN"\n╔══════════════════════════════════════════════════════════╗\n"RESET);
    printf(BOLD"  SELECCIONE LOS MESES PARA MOSTRAR PORCENTAJES POR CATEGORÍA\n"RESET);
    printf(CYAN"╠══════════════════════════════════════════════════════════╣\n"RESET);

    for (int i = 0; i < 12; i++) {
        printf("  %2d. %s\n", i + 1, nombresMeses[i]); // Mostrar los meses disponibles
    }
    printf(CYAN"╚══════════════════════════════════════════════════════════╝\n"RESET);

    while (1)
    {
        int opcion; // Variable para almacenar la opción del mes seleccionado
        printf("\nIngrese el número del mes ("YELLOW"1-12"RESET") o "GREEN"0 para confirmar"RESET": ");
        scanf("%d", &opcion);

        if (opcion == 0) break;

        if (opcion < 1 || opcion > 12) {
            printf(RED"Opción inválida. Intente nuevamente.\n"RESET);
            continue;
        }

        bool yaSeleccionado = false; // Validar si el mes ya ha sido seleccionado
        for (int j = 0; j < cantidad; j++) {
            if (seleccionado[j] == opcion) {
                yaSeleccionado = true; // El mes ya ha sido seleccionado
                break;
            }
        }

        if (yaSeleccionado) {
            printf(YELLOW"El mes %s ya ha sido seleccionado.\n"RESET, nombresMeses[opcion - 1]); // Informar al usuario que el mes ya ha sido seleccionado
        } else {
            seleccionado[cantidad++] = opcion; // Agregar el mes seleccionado al arreglo
            printf(GREEN"Mes %s seleccionado.\n"RESET, nombresMeses[opcion - 1]); 
        }
    }

    if (cantidad == 0) {
        printf(RED"No se han seleccionado meses. Saliendo...\n"RESET);
        return;
    }

    printf(CYAN"\n╔══════════════════════════════════════════════════════════╗\n"RESET);
    printf(BOLD"  PORCENTAJES POR CATEGORÍA DE GASTOS PAGADOS\n"RESET);
    printf(CYAN"╚══════════════════════════════════════════════════════════╝\n"RESET);

    for (int i = 0; i < cantidad; i++) {
        int mesSeleccionado = seleccionado[i] - 1; //Indice del mes seleccionado
        Pair *par = searchTreeMap(arbol, (void*)nombresMeses[mesSeleccionado]); // Buscar el mes en el árbol

        if (par == NULL) {
            printf(RED"\n[%s] No tiene datos registrados.\n"RESET, nombresMeses[mesSeleccionado]);
            continue;
        }

        MesFinanciero *mes = (MesFinanciero*)par->value; // Obtener los datos del mes seleccionado

        if (mes->totalGastos == 0) {
            printf(YELLOW"\n[%s] No hay gastos registrados.\n"RESET, mes->nombreMes);
            continue;
        }

        printf(GREEN"\nResumen para %s:\n"RESET, mes->nombreMes);
        printf("  "BOLD"Total Gastos: "YELLOW"%d"RESET"\n", mes->totalGastos);
        printf("  "CYAN"---------------------------------------------"RESET"\n");

        Gasto *gasto = list_first(mes->listaGastos); // Obtener el primer gasto de la lista
        int hayPagados = 0;
        while (gasto) {
            if (strcasecmp(gasto->estado, "Pagado") == 0) { // Validar si el gasto está pagado
                float porcentaje = (float)gasto->monto / mes->totalGastos * 100; // Calcular el porcentaje del gasto respecto al total de gastos
                // Imprimir el gasto con su categoría, monto y porcentaje
                printf("  Categoría: %-15s | Monto: "YELLOW"%6d"RESET" | Porcentaje: "GREEN"%.2f%%"RESET"\n",
                       gasto->categoria, gasto->monto, porcentaje); 
                hayPagados = 1;
            }
            gasto = list_next(mes->listaGastos); // Avanzar al siguiente gasto
        }
        if (!hayPagados) {
            printf(YELLOW"  No hay gastos pagados en este mes.\n"RESET);
        }
        printf(CYAN"---------------------------------------------\n"RESET);
    }
}

bool verificarArchivoExistente(int *anyo, char *nombreArchivo, int opcion) {
    if (opcion == 1) {
        printf("Ingrese el año del archivo a cargar: "); // Solicitar el año del archivo a cargar
    } else if (opcion == 2) {
        printf("Ingrese el año para crear un nuevo archivo: "); // Solicitar el año para crear un nuevo archivo
    } else if (opcion == 3) {
        printf("Ingrese el año del archivo a guardar: "); // Solicitar el año del archivo a guardar
    } else {
        return false; // Opción inválida
    }
    scanf("%d", anyo); // Leer el año ingresado por el usuario
    sprintf(nombreArchivo, "finanzas_%d.csv", *anyo); // Formatear el nombre del archivo con el año ingresado
    FILE *archivo = fopen(nombreArchivo, "r"); // Intentar abrir el archivo en modo lectura
    if (archivo) { // Si el archivo se abre correctamente, significa que ya existe
        fclose(archivo);
        return true; // El archivo ya existe
    } else {
        return false; // El archivo no existe
    }

}

void accionesAlCSV (TreeMap *arbol, bool *cargado) {

    printf("\n");
    printf(GREEN"╔════════════════════════════════════════════════════════════╗\n");
    printf("║                 ACCIONES DE ARCHIVO CSV                    ║\n");
    printf("╠════════════════════════════════════════════════════════════╣\n");
    printf("║                                                            ║\n");
    printf("║  1. Cargar archivo de finanzas existente                   ║\n");
    printf("║  2. Crear nuevo archivo de finanzas (desde plantilla)      ║\n");
    printf("║  3. Guardar cambios en el archivo actual                   ║\n");
    printf("║                                                            ║\n");
    printf("║  0. Volver al menú principal                               ║\n");
    printf("║                                                            ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n"RESET);
    printf("\nIngrese su opción: ");

    int opcion; // Variable para almacenar la opción seleccionada por el usuario
    scanf("%d", &opcion);

    int anyo;
    char nombreArchivo[100];
    switch (opcion) {
        case 1:
            if (*cargado) {
                printf("[!] Ya se ha cargado un archivo de finanzas.\n"); // Informar al usuario que ya se ha cargado un archivo
                return;
            }
            if (!verificarArchivoExistente(&anyo, nombreArchivo, opcion)) {
                printf("No se pudo verificar el archivo. Asegúrate de que el archivo exista.\n"); // Informar al usuario que no se pudo verificar el archivo
                return;
            }
            *cargado = true; // Marcar que se ha cargado un archivo CSV
            cargarMovimientosDesdeCSV(arbol, nombreArchivo); // Cargar los movimientos desde el archivo CSV
            printf("[OK] Archivo de finanzas cargado correctamente.\n");
            break;
        case 2:
            if (verificarArchivoExistente(&anyo, nombreArchivo, opcion)) {
                printf("[!] El archivo ya existe, no se sobreescribirá.\n"); // Informar al usuario que el archivo ya existe
                return; 
            }
            copiarArchivoCSV("plantilla.csv", nombreArchivo); // Copiar la plantilla a un nuevo archivo
            break;
        case 3:
            if (!*cargado) {
                printf(" [!] No se ha cargado ningún archivo de finanzas. Carga un archivo antes de guardar.\n");
                return;
            }
            if (!verificarArchivoExistente(&anyo, nombreArchivo, opcion)) {
                printf("No se pudo verificar el archivo. Asegúrate de que el archivo exista.\n"); // Informar al usuario que no se pudo verificar el archivo
                return;
            }
            guardarCSV(arbol, nombreArchivo); // Guardar los cambios en el archivo CSV actual
            break;
        case 0:
            printf("Volviendo al menú principal...\n");
            return;
        default:
            printf("Opción inválida. Volviendo al menú principal...\n");
            return;
    }
}


int leerOpcionValida(int min, int max)
{
    char buffer[100];
    int opcion;
    char *endptr;

    while (1) 
    {
        printf("Seleccione una opción (%d-%d): ", min, max);
        if (!fgets(buffer, sizeof(buffer), stdin)) {
            clearerr(stdin);
            continue;
        }

        // Eliminar salto de línea
        buffer[strcspn(buffer, "\n")] = 0;

        // Ignorar líneas vacías (solo Enter)
        if (strlen(buffer) == 0) continue;

        opcion = strtol(buffer, &endptr, 10);

        if (endptr == buffer || *endptr != '\0')
        {
            printf("[!] Entrada inválida. Por favor, ingrese un número válido.\n");
        }
        else if (opcion < min || opcion > max)
        {
            printf("[!] Opción fuera de rango. Por favor, ingrese un número entre %d y %d.\n", min, max);
        }
        else 
        {
            return opcion;
        }
    }
}


void compararGastosEntreMeses(TreeMap *arbol)
{
    limpiarConsola();
    printf(CYAN"\n╔══════════════════════════════════════════════════════════╗\n"RESET);
    printf(BOLD"  COMPARACIÓN DE GASTOS ENTRE DOS MESES\n"RESET);
    printf(CYAN"╠══════════════════════════════════════════════════════════╣\n"RESET);

    for (int i = 0; i < 12; i++) {
        printf("  %2d. %s\n", i + 1, nombresMeses[i]); // Mostrar los meses disponibles
    }
    printf(CYAN"╚══════════════════════════════════════════════════════════╝\n"RESET);

    int mes1, mes2; // Variables para almacenar los meses seleccionados por el usuario
    do {
        printf("\nIngrese el número del primer mes ("YELLOW"1-12"RESET"): ");
        scanf("%d", &mes1); // Leer el primer mes seleccionado por el usuario
    } while (mes1 < 1 || mes1 > 12);

    do {
        printf("Ingrese el número del segundo mes ("YELLOW"1-12"RESET"): ");
        scanf("%d", &mes2); // Leer el segundo mes seleccionado por el usuario
    } while (mes2 < 1 || mes2 > 12);

    if (mes1 == mes2) {
        printf(RED"[!] No se puede comparar el mismo mes. Seleccione meses diferentes.\n"RESET);
        return;
    }

    printf(CYAN"\n¿Respecto a qué mes desea comparar los gastos?\n"RESET);
    printf("  1. %s\n", nombresMeses[mes1 - 1]); 
    printf("  2. %s\n", nombresMeses[mes2 - 1]);
    printf("Seleccione una opción ("YELLOW"1 o 2"RESET"): "); // Leer la opción de comparación del usuario
    int opcion; 
    scanf("%d", &opcion);

    Pair *par1 = searchTreeMap(arbol, (void*)nombresMeses[mes1 - 1]); // Buscar el primer mes en el árbol
    Pair *par2 = searchTreeMap(arbol, (void*)nombresMeses[mes2 - 1]); // Buscar el segundo mes en el árbol

    if (!par1 || !par2) {
        printf(RED"[!] Uno o ambos meses no existen en los registros.\n"RESET);
        return;
    }

    MesFinanciero *mesFinanciero1 = (MesFinanciero*)par1->value; // Obtener los datos del primer mes seleccionado
    MesFinanciero *mesFinanciero2 = (MesFinanciero*)par2->value; // Obtener los datos del segundo mes seleccionado

    if(mesFinanciero1->modificado == 0 || mesFinanciero2->modificado == 0) {
        printf(RED"[!] Uno o ambos meses no han sido modificados. No se puede realizar la comparación.\n"RESET);
        return;
    }
    limpiarConsola();
    printf(GREEN"\n╔══════════════════════════════════════════════════════════╗\n"RESET);
    printf(BOLD"  Comparación de gastos entre %s y %s\n"RESET, mesFinanciero1->nombreMes, mesFinanciero2->nombreMes);
    printf(GREEN"╠══════════════════════════════════════════════════════════╣\n"RESET);

    Gasto *gasto1 = list_first(mesFinanciero1->listaGastos); // Obtener el primer gasto de la lista del primer mes
    Gasto *gasto2 = list_first(mesFinanciero2->listaGastos); // Obtener el primer gasto de la lista del segundo mes

    printf(BOLD"  %-15s | %-10s | %-10s | Variación\n"RESET, "Categoría", mesFinanciero1->nombreMes, mesFinanciero2->nombreMes);
    printf(CYAN"  --------------------------------------------------------\n"RESET);

    while (gasto1 != NULL || gasto2 != NULL) {
        if (strcmp(gasto1->estado, "No Registra") == 0 && strcmp(gasto2->estado, "No Registra") == 0) {
            gasto1 = list_next(mesFinanciero1->listaGastos);
            gasto2 = list_next(mesFinanciero2->listaGastos);
            continue; // Si ambos gastos son "No Registra", saltar a la siguiente iteración
        }
        float porcentaje = 0.0;
        if (gasto1->monto > 0 && gasto2->monto == 0) {
            porcentaje = (opcion == 1) ? 100.0 : -100.0; // Si el primer gasto es mayor que cero y el segundo es cero, asignar 100% o -100% según la opción
        }
        else if (gasto1->monto == 0 && gasto2->monto > 0) {
            porcentaje = (opcion == 1) ? -100.0 : 100.0; // Si el primer gasto es cero y el segundo es mayor que cero, asignar -100% o 100% según la opción
        }
        else if (gasto1->monto != 0 && gasto2->monto != 0) {
            if (opcion == 1)
                porcentaje = (gasto1->monto * 100.0 / (float)gasto2->monto) - 100.0; // Calcular el porcentaje de variación entre los dos gastos
            else
                porcentaje = (gasto2->monto * 100.0 / (float)gasto1->monto) - 100.0; // Calcular el porcentaje de variación entre los dos gastos
        }

        // Color para la variación
        char colorVar[10] = "";
        if (porcentaje > 0.0)
            strcpy(colorVar, GREEN);
        else if (porcentaje < 0.0)
            strcpy(colorVar, RED);
        else
            strcpy(colorVar, YELLOW);

        if (opcion == 1) {
            printf("  %-15s | "YELLOW"%10d"RESET" | "YELLOW"%10d"RESET" | %s%8.2f%%%s\n",
                gasto1->categoria, gasto1->monto, gasto2->monto, colorVar, porcentaje, RESET);
        } else if (opcion == 2) {
            printf("  %-15s | "YELLOW"%10d"RESET" | "YELLOW"%10d"RESET" | %s%8.2f%%%s\n",
                gasto2->categoria, gasto2->monto, gasto1->monto, colorVar, porcentaje, RESET);
        }

        gasto1 = list_next(mesFinanciero1->listaGastos); // Avanzar al siguiente gasto del primer mes
        gasto2 = list_next(mesFinanciero2->listaGastos); // Avanzar al siguiente gasto del segundo mes
    }
    printf(GREEN"╚══════════════════════════════════════════════════════════╝\n"RESET);
}

void subMenuAnalisis(TreeMap *arbol)
{
    while (true) {
        limpiarConsola(); // Limpiar la consola antes de mostrar el submenú
        printf("\n");
        printf(GREEN"╔════════════════════════════════════════════════════════════╗\n"); 
        printf("║                      ANÁLISIS FINANCIERO                   ║\n");
        printf("╠════════════════════════════════════════════════════════════╣\n");
        printf("║                                                            ║\n");
        printf("║  1. Ver porcentaje de gastos por mes                       ║\n");
        printf("║  2. Comparacion de gastos entre meses                      ║\n");
        printf("║                                                            ║\n");
        printf("║  0. Volver al menú principal                               ║\n");
        printf("║                                                            ║\n");
        printf("╚════════════════════════════════════════════════════════════╝\n"RESET);
        printf("\nIngrese su opción: ");

        int opcion = leerOpcionValida(0, 2);
        switch (opcion) {
            case 1:
                limpiarConsola();
                mostrarPorcentajesPorCategorias(arbol); // Mostrar el porcentaje de gastos por mes
                presionaEnter(); // Esperar a que el usuario presione Enter antes de continuar
                break;
            case 2:
                limpiarConsola();
                compararGastosEntreMeses(arbol); // Comparar gastos entre dos meses
                presionaEnter(); // Esperar a que el usuario presione Enter antes de continuar
                break;
            case 0:
                printf("Volviendo al menú principal...\n");
                return;
            default:
                printf("Opción inválida. Intente Nuevamente\n");
                break;
        }
    }
    

}

void verAhorroMesAMes(TreeMap *arbol) { // Función para ver el ahorro mes a mes
    limpiarConsola();
    printf(CYAN"\n╔══════════════════════════════════════════════════════════╗\n"RESET);
    printf(BOLD"  AHORRADO MES A MES\n"RESET);
    printf(CYAN"╠══════════════════════════════════════════════════════════╣\n"RESET);

    int maximoAhorro = 0; // Variable para almacenar el máximo ahorro encontrado
    int minimoAhorro = 0; // Variable para almacenar el mínimo ahorro encontrado

    // Variables para almacenar los nombres de los meses con máximo y mínimo ahorro
    char mesConMaximo[20] = "";
    char mesConMinimo[20] = "";
    int hayDatos = 0;

    Pair *par = firstTreeMap(arbol); // Iterar sobre los meses en el árbol
    while (par != NULL) { // Iterar sobre los pares del árbol
        MesFinanciero *datosMes = (MesFinanciero*)par->value;

        if (datosMes->modificado == 1) { // Solo mostrar meses modificados
            printf("  "BOLD"%s"RESET" | Ingresos: "GREEN"%d"RESET" | Gastos Totales: "RED"%d"RESET" | Ahorro: "YELLOW"%d"RESET"\n",
                datosMes->nombreMes, datosMes->ingresos, datosMes->totalGastos, datosMes->ahorrado);

            if (!hayDatos) { // Si es el primer mes modificado
                // Inicializar los valores máximos y mínimos
                maximoAhorro = minimoAhorro = datosMes->ahorrado;
                strcpy(mesConMaximo, datosMes->nombreMes);
                strcpy(mesConMinimo, datosMes->nombreMes);
                hayDatos = 1;
            }
            if (datosMes->ahorrado > maximoAhorro) { // Actualizar máximo y mínimo
                // Si el ahorro del mes actual es mayor que el máximo, actualizar
                maximoAhorro = datosMes->ahorrado;
                strcpy(mesConMaximo, datosMes->nombreMes);
            }
            if (datosMes->ahorrado < minimoAhorro) { // Si el ahorro del mes actual es menor que el mínimo, actualizar
                // Si el ahorro del mes actual es menor que el mínimo, actualizar
                minimoAhorro = datosMes->ahorrado;
                strcpy(mesConMinimo, datosMes->nombreMes);
            }
        }
        par = nextTreeMap(arbol);
    }

    if (!hayDatos) { // Si no hay meses modificados, mostrar mensaje
        printf(RED"\nNo hay meses registrados o modificados.\n"RESET);
        printf(CYAN"╚══════════════════════════════════════════════════════════╝\n"RESET);
        return;
    }
    // Mostrar resumen de ahorro
    printf(CYAN"╠══════════════════════════════════════════════════════════╣\n"RESET);
    printf("  "BOLD"Ahorro Máximo:"RESET" "YELLOW"%d"RESET" en "GREEN"%s"RESET"\n", maximoAhorro, mesConMaximo);
    printf("  "BOLD"Ahorro Mínimo:"RESET" "YELLOW"%d"RESET" en "RED"%s"RESET"\n", minimoAhorro, mesConMinimo);
    printf(CYAN"╚══════════════════════════════════════════════════════════╝\n"RESET);
}

void verPorcentajeAhorro(TreeMap *arbol) { // Función para ver el porcentaje de ahorro respecto a ingresos por mes
    limpiarConsola(); 
    printf(CYAN"\n╔══════════════════════════════════════════════════════════╗\n"RESET);
    printf(BOLD"  PORCENTAJE DE AHORRO RESPECTO A INGRESOS POR MES\n"RESET);
    printf(CYAN"╠══════════════════════════════════════════════════════════╣\n"RESET);

    Pair *par = firstTreeMap(arbol); // Iterar sobre los meses en el árbol
    int hayDatos = 0;
    while (par != NULL) { // Iterar sobre los pares del árbol
        MesFinanciero *datosMes = (MesFinanciero*)par->value;
        if (datosMes->modificado == 1 && datosMes->ingresos > 0) { // Solo mostrar meses modificados con ingresos
            float porcentajeAhorro = ((float)datosMes->ahorrado / datosMes->ingresos) * 100.0;
            printf("  "BOLD"%s"RESET" | Ingresos: "GREEN"%d"RESET" | Ahorro: "YELLOW"%d"RESET" | "CYAN"Porcentaje: "GREEN"%.2f%%"RESET"\n",
                   datosMes->nombreMes, datosMes->ingresos, datosMes->ahorrado, porcentajeAhorro);
            hayDatos = 1;
        }
        par = nextTreeMap(arbol);
    }

    if (!hayDatos) {
        printf(RED"  No hay meses registrados o modificados con ingresos para mostrar.\n"RESET);
    }

    printf(CYAN"╚══════════════════════════════════════════════════════════╝\n"RESET);
}

void submenuExcedenteMensual(TreeMap *arbol) {
    int opcion; // Variable para almacenar la opción del submenú
    while (true) {
        limpiarConsola(); // Limpiar la consola antes de mostrar el submenú
        printf("\n");
        printf(GREEN"╔════════════════════════════════════════════════════════════╗\n");
        printf("║                 EXCEDENTE MENSUAL Y AHORROS                ║\n");
        printf("╠════════════════════════════════════════════════════════════╣\n");
        printf("║                                                            ║\n");
        printf("║  1. Ver ahorro mes a mes (Con total y extremos)            ║\n");
        printf("║  2. Ver porcentaje de ahorro respecto a ingresos           ║\n");
        printf("║                                                            ║\n");
        printf("║  0. Volver al menú principal                               ║\n");
        printf("║                                                            ║\n");
        printf("╚════════════════════════════════════════════════════════════╝\n"RESET);
        printf("\nIngrese su opción: ");
        scanf("%d", &opcion);

        switch (opcion) {
            case 1:
                limpiarConsola();
                verAhorroMesAMes(arbol); // Ver el ahorro mes a mes
                presionaEnter(); // Esperar a que el usuario presione Enter antes de continuar
                break;
            case 2:
                limpiarConsola();
                verPorcentajeAhorro(arbol); // Ver el porcentaje de ahorro respecto a ingresos por mes
                presionaEnter(); // Esperar a que el usuario presione Enter antes de continuar
                break;
            case 0:
                printf("Volviendo al menú principal...\n");
                return;
            default:
                printf("Opción inválida. Intente nuevamente.\n");
        }
    }
}
