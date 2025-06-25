#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "tdas/extra.h"
#include "tdas/list.h"
#include "funciones.h"

const char* nombresMeses[12] = {
    "Enero", "Febrero", "Marzo", "Abril", "Mayo", "Junio",
    "Julio", "Agosto", "Septiembre", "Octubre", "Noviembre", "Diciembre"
};

const char* categoriasGastos[12] = {
    "Agua", "Luz", "Gas", "Alimentacion", "Vivienda",
    "Transporte", "Conectividad", "Vestuario", "Salud", "Otros Gastos", NULL
};

void mostrarMenu() {

    printf("\n");
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║         SISTEMA DE GESTIÓN FINANCIERA PERSONAL (SGFP)      ║\n");
    printf("╠════════════════════════════════════════════════════════════╣\n");
    printf("║                                                            ║\n");
    printf("║  1. Registrar Movimiento Financiero                        ║\n");
    printf("║  2. Ver Resumen Mensual                                    ║\n");
    printf("║  3. Gestionar Gastos                                       ║\n");
    printf("║  4. Historial y Análisis                                   ║\n");
    printf("║  5. Ver Excedente Mensual                                  ║\n");
    printf("║  6. Ver Porcentaje de Gastos por Categoría                 ║\n");
    printf("║  7. Acciones de Archivo CSV (Cargar/Crear/Guardar)         ║\n");
    printf("║                                                            ║\n");
    printf("║  0. Salir                                                  ║\n");
    printf("║                                                            ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n");
    
}

void imprimirMensaje(const char *mensaje) {
    printf("%s\n", mensaje);
}

void copiarArchivoCSV(const char* origen, const char* destino) {
    FILE *fileOrigen = fopen(origen, "r");
    if (fileOrigen == NULL) {
        printf("Error: no se pudo abrir el archivo base '%s'.\n", origen);
        return;
    }

    FILE *fileDestino = fopen(destino, "w");
    if (fileDestino == NULL) {
        printf("Error: no se pudo crear el archivo destino '%s'.\n", destino);
        fclose(fileOrigen);
        return;
    }

    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), fileOrigen)) {
        fputs(buffer, fileDestino);
    }

    fclose(fileOrigen);
    fclose(fileDestino);
    printf("Archivo '%s' creado correctamente desde '%s'.\n", destino, origen);
}
int lower_than_string(void *key1, void *key2) {
    char *str1 = (char *)key1;
    char *str2 = (char *)key2;
    if (strcmp(str1,str2) < 0) {
        return 1; // str1 es menor que str2
    }
    return 0;


}


void cargarMovimientosDesdeCSV(TreeMap *arbol, const char *nombreArchivo) {
    FILE *archivo = fopen(nombreArchivo, "r");
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

    int idxMes = -1, idxIngreso = -1, idxAhorrado = -1, idxTotalGastos = -1, idxModificado = -1;
    int idxInicioGastos = -1;
    int idxFinGastos = -1;

    int totalColumnas = 0;
    while (encabezados[totalColumnas] != NULL) totalColumnas++;

    for (int i = 0; i < totalColumnas; i++) {
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
            g->monto = atoi(campos[i]);
            strcpy(g->estado, campos[i + 1]);
            g->modificado = false;

            list_pushBack(mes->listaGastos, g);
        }

        // Insertar en árbol
        insertTreeMap(arbol, mes->nombreMes, mes);
        
        
    }
        for (int i = 0; encabezados[i] != NULL ; i++) {
            free(encabezados[i]); // Liberar memoria de los encabezados
        }
    fclose(archivo);
}
void mostrarXmeses(TreeMap *arbol) {
    int x;
    printf("¿Cuántos meses desea ver a partir del mes actual? ");
    scanf("%d", &x);

    // Buscar el índice del mes actual (último modificado)
    int indiceMesActual = -1;
    MesFinanciero *mesActual = NULL;
    for (int i = 11; i >= 0; i--) {
        Pair *par = searchTreeMap(arbol, (void*)nombresMeses[i]);
        if (par != NULL) {
            MesFinanciero *mes = (MesFinanciero*)par->value;
            if (mes->modificado == 1) {
                mesActual = mes;
                indiceMesActual = i;
                break;
            }
        }
    }
    if (mesActual == NULL) {
        printf("No hay meses modificados para mostrar.\n");
        return;
    }

    printf("\n--- Mostrando los últimos %d meses desde %s ---\n", x, nombresMeses[indiceMesActual]);
    int mostrados = 0;
    for (int i = indiceMesActual; i >= 0 && mostrados < x; i--) {
        Pair *par = searchTreeMap(arbol, (void*)nombresMeses[i]);
        mostrados++;
        if (par == NULL) {
            printf("[%s] No hay datos registrados para este mes.\n", nombresMeses[i]);
            continue;
        }
        MesFinanciero *mes = (MesFinanciero*)par->value;
        if (mes->modificado == 0) {
            printf("[%s] El mes no ha sido modificado.\n", nombresMeses[i]);
            continue;
        }
        printf("\nMes: %s\n", mes->nombreMes);
        printf("Ingresos: %d\n", mes->ingresos);
        printf("Ahorro: %d\n", mes->ahorrado);
        printf("Total Gastos: %d\n", mes->totalGastos);
        printf("Categorías registradas:\n");
        int hayGastos = 0;
        Gasto *g = list_first(mes->listaGastos);
        while (g) {
            if (strcmp(g->estado, "Pendiente") == 0 || strcmp(g->estado, "Pagado") == 0) {
                printf(" - %s | Monto: %d | Estado: %s\n", g->categoria, g->monto, g->estado);
                hayGastos = 1;
            }
            g = list_next(mes->listaGastos);
        }
        if (!hayGastos) {
            printf("No hay gastos registrados para este mes.\n");
        }
        printf("--------------------------------------\n");
        
    }
}
void subMenuMostrarMovimientos(TreeMap *arbol){
// sub menu que hace llamado a las distinas funciones de visualizacion de los movimientos financieros
    int opcion;
    do {
        printf("Submenú de movimientos financieros:\n");
        printf("1. Mostrar ultimos X meses\n");
        printf("2. Mostrar mes actual\n");
        printf("0. Volver al menú principal\n");
        printf("Seleccione una opción: ");
        scanf("%d", &opcion);

        switch (opcion) {
            case 1:
                mostrarXmeses(arbol);
                break;
            case 2:
                mostrarMesActual(arbol);
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
    for (int i = 11; i >= 0; i--) {
        Pair *par = searchTreeMap(arbol, (void*)nombresMeses[i]);
        if (par != NULL) {
            MesFinanciero *mes = (MesFinanciero*)par->value;
            if (mes->modificado == 1) {
                mesActual = mes;
                indiceMesActual = i;
                break;
            }
        }
    }
    if (mesActual == NULL) {
        printf("No hay meses modificados para mostrar.\n");
        return;
    }

    printf("\n--- Información del mes actual: %s ---\n", mesActual->nombreMes);
    printf("Categorías registradas:\n");
    int hayGastos = 0;
    Gasto *g = list_first(mesActual->listaGastos);
    while (g) {
    if (strcmp(g->estado, "Pendiente") == 0 || strcmp(g->estado, "Pagado") == 0) {
        printf(" - %s | Monto: %d | Estado: %s\n", g->categoria, g->monto, g->estado);
        hayGastos = 1;
    }
    g = list_next(mesActual->listaGastos);
}
    if (!hayGastos) {
        printf("No hay gastos registrados para este mes.\n");
    }
    printf("Total gastado: %d\n", mesActual->totalGastos);
    printf("Monto ahorrado: %d\n", mesActual->ahorrado);
    printf("--------------------------------------\n");
}
void mostrarMovimientosPorMes(TreeMap *arbol) {
  Pair *par = firstTreeMap(arbol);

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
// ...existing code...

int mes_a_numero(const char* mes) {
    const char* meses[] = {"enero","febrero","marzo","abril","mayo","junio","julio","agosto","septiembre","octubre","noviembre","diciembre"};
    for (int i = 0; i < 12; i++) {
        if (strcasecmp(mes, meses[i]) == 0) return i+1;
    }
    return 0; // No encontrado
}

int lower_than_mes(void* key1, void* key2) {
    int n1 = mes_a_numero((const char*)key1);
    int n2 = mes_a_numero((const char*)key2);
    return n1 < n2;
}

// ...existing code...


void guardarCSV(TreeMap *arbol, const char *nombreArchivo) {
    FILE *archivo = fopen(nombreArchivo, "w");
    if (!archivo) {
        printf("Error al abrir el archivo para guardar.\n");
        return;
    }
    // Escribir encabezados
    fprintf(archivo, "Mes,Ingreso,Ahorrado");
    for (int i = 0; categoriasGastos[i] != NULL; i++) {
        fprintf(archivo, ",%s, Estado", categoriasGastos[i]);
    }
    fprintf(archivo, ",Total Gastos,Modificado\n");

    //recorrer el árbol y escribir los datos
    Pair *par = firstTreeMap(arbol);
    while (par != NULL) {
        MesFinanciero *mes = (MesFinanciero *)par->value;
        fprintf(archivo, "%s,%d,%d", mes->nombreMes, mes->ingresos, mes->ahorrado);

        // Escribir gastos
        Gasto *gasto = list_first(mes->listaGastos);
        while (gasto != NULL) {
            fprintf(archivo, ",%d,%s", gasto->monto, gasto->estado);
            gasto = list_next(mes->listaGastos);
        }

        fprintf(archivo, ",%d,%s\n", mes->totalGastos, mes->modificado ? "Si" : "No");
        par = nextTreeMap(arbol);
    }
    fclose(archivo);
    printf("Datos guardados en %s\n", nombreArchivo);
}
void reiniciarMes(TreeMap *arbol){
    int opcionMes;
    MesFinanciero *datosMes = NULL;
    do {
        printf("Ingrese 0 para volver al menú principal.\n");
        printf("Seleccione el mes a reiniciar:\n");
        for (int i = 0; i < 12; i++) {
            printf("%2d. %s\n", i + 1, nombresMeses[i]);
        }
        printf("Ingrese el número del mes (1-12): ");
        scanf("%d", &opcionMes);

        if (opcionMes == 0) {
            printf("Volviendo al menú principal...\n");
            return;
        }
        if (opcionMes < 1 || opcionMes > 12) {
            printf("Opción inválida. Intente nuevamente.\n");
            continue;
        }

        Pair *par = searchTreeMap(arbol, (void*)nombresMeses[opcionMes - 1]);
        if (!par) {
            printf("El mes seleccionado no existe. Intente nuevamente.\n");
            continue;
        }

        datosMes = (MesFinanciero*)par->value;
        if (datosMes->modificado == 0) {
            printf("El mes seleccionado no ha sido modificado. Seleccione otro mes.\n");
            datosMes = NULL;
        }
    } while (datosMes == NULL);

    // Reiniciar los datos del mes
    datosMes->ingresos = 0;
    datosMes->ahorrado = 0;
    datosMes->totalGastos = 0;
    datosMes->modificado = 0;

    Gasto *gasto = list_first(datosMes->listaGastos);
    while (gasto != NULL) {
        // Liberar memoria de cada gasto
        gasto->modificado = false; // Marcar como no modificado
        strcpy(gasto->estado, "No Registra"); 
        gasto->monto = 0; // Reiniciar el monto
        gasto = list_next(datosMes->listaGastos);

    }
    printf("El mes %s ha sido reiniciado exitosamente.\n", datosMes->nombreMes);
}
void SubMenuRegistrarMovimiento(TreeMap *arbol){
    int opcion;
    do {
        printf("\n");
        printf("╔════════════════════════════════════════════════════════════╗\n");
        printf("║             REGISTRO Y GESTIÓN DE MOVIMIENTOS              ║\n");
        printf("╠════════════════════════════════════════════════════════════╣\n");
        printf("║                                                            ║\n");
        printf("║  1. Registrar movimiento financiero                        ║\n");
        printf("║  2. Reiniciar todos los datos de un mes                    ║\n");
        printf("║                                                            ║\n");
        printf("║  0. Volver al menú principal                               ║\n");
        printf("║                                                            ║\n");
        printf("╚════════════════════════════════════════════════════════════╝\n");
        printf("\nIngrese su opción: ");
        scanf("%d", &opcion);

        switch (opcion) {
            case 1:
                registrarMovimientoFinanciero(arbol);
                break;
            case 2:
                reiniciarMes(arbol);
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
    
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║              REGISTRAR MOVIMIENTO FINANCIERO               ║\n");
    printf("╠════════════════════════════════════════════════════════════╣\n");
    printf("║  [INFO] Seleccione un mes para registrar sus ingresos y    ║\n");
    printf("║  gastos. Una vez finalizado, el mes se marcará como        ║\n");
    printf("║  como 'cerrado' y no podrá ser modificado. Si desea        ║\n");
    printf("║  modificarlo debe eliminarlo y registrarlo nuevamente      ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n\n");


    for (int i = 0; i < 12; i++) {
        printf("%2d. %s\n", i + 1, nombresMeses[i]);
    }

    int opcionMes = 0;
    do {
        printf("\n>> Ingrese el número del mes (1-12): ");
        scanf("%d", &opcionMes);
    } while (opcionMes < 1 || opcionMes > 12);

    const char* mes = nombresMeses[opcionMes - 1];
    Pair* par = searchTreeMap(arbol, (void*)mes);
    MesFinanciero* datosMes = (MesFinanciero*)par->value;
    if (datosMes -> modificado == 1) {
        printf("[!] El mes %s ya ha sido cerrado. Si desea modificar el mes deberá eliminarlo y registrarlo nuevamente.\n", mes);
        return;
    }

    while (true) {

        int ingresosAux = 0;
        printf("Ingrese el monto de ingresos para el mes %s: ", mes);
        scanf("%d", &ingresosAux);
        printf(">> ¿Confirma el ingreso de %d para el mes %s? (1 = Sí, 0 = No): ", ingresosAux, mes);
        int confirmarIngreso;
        scanf("%d", &confirmarIngreso);
        if (confirmarIngreso == 1) {
            datosMes->ingresos += ingresosAux;
            datosMes->ahorrado += ingresosAux; // Aumentar el ahorro con los ingresos
            printf("[OK] Ingreso de %d registrado para el mes %s.\n", ingresosAux, mes);
        } else {
            printf("[INFO] Ingreso no confirmado.\n");
        }
        if (confirmarIngreso == 1) break;
    }
    printf("Categorías actuales:\n");
    int idx = 1;
    Gasto* g = list_first(datosMes->listaGastos);
    while (g) {
        printf("%2d. %s | Monto: %d | Estado: %s\n", idx, g->categoria, g->monto, g->estado);
        g = list_next(datosMes->listaGastos);
        idx++;
    }
    int opcionGasto;
    while (true)
    {
        printf("Seleccione una categoría para modificar (0 para salir): ");
        scanf("%d", &opcionGasto);
        if (opcionGasto == 0) break;

        if (opcionGasto < 1 || opcionGasto >= idx) {
            printf("Opción inválida. Intente nuevamente.\n");
            continue;
        }

        g = list_first(datosMes->listaGastos);
        for (int i = 1; i < opcionGasto; i++) {
            g = list_next(datosMes->listaGastos);
        }
        if (g->modificado) {
            printf("El gasto ya ha sido modificado. No se puede cambiar.\n");
            continue;
        }
        while (true)
        {
            int nuevoMonto;
            printf("Ingrese el nuevo monto para la categoría %s: ", g->categoria);
            scanf("%d", &nuevoMonto);
            printf("Confirma el cambio de monto a %d para la categoría %s? (1=Sí, 0=No): ", nuevoMonto, g->categoria);
            int confirmarCambio;
            scanf("%d", &confirmarCambio);
            if (confirmarCambio == 1) {
                g->monto = nuevoMonto;
                
            }
            if (confirmarCambio == 1) break;
        }

         //opcion para cambiar el estado del gasto en numero
        int opcionEstado;
        printf("Seleccione el estado del gasto:\n");
        printf("1. Pendiente\n");
        printf("2. Pagado\n");

        do {
            printf("Ingrese su opción: ");
            scanf("%d", &opcionEstado);
        } while (opcionEstado < 1 || opcionEstado > 2);
        if (opcionEstado == 1) {
            strcpy(g->estado, "Pendiente");
        } else if (opcionEstado == 2) {
            if (datosMes->ahorrado < g->monto) {
                printf("No hay suficiente dinero para pagar este gasto.\n");
                strcpy(g->estado, "Pendiente");
                printf("El gasto se ha dejado como pendiente.\n");
                continue; // No se puede pagar, salir del bucle
            }
            strcpy(g->estado, "Pagado");
            datosMes->ahorrado -= g->monto; // Descontar del ahorrado
            datosMes->totalGastos += g->monto; // Aumentar el total de gastos pagados
            printf("Gasto marcado como pagado\n");
        }
    }
    datosMes->modificado = 1; // Marcar como modificado
    printf("¡Movimiento financiero actualizado para %s!\nEl mes ha sido cerrado\n", mes);
}

// el ingreso se ira a ahorrado y de ahi se descontara lo que tenga que pagarse para cada cuenta, el ahorrado tendra dinero que permitira pagar a futuro
// si marco wea pagado que se desucente del ahorrado, si no se paga se queda en pendiente y no se descuenta del ahorrado

//PENDIENTE HACER EN LA FUNCION DE ARRIBA

// Ingreso se puedan modificar
// Gastos se puedan modificar
// preguntar si quiere aceptar el ingreso del gasto o no, si no se acepta se deja como pendiente si no queda como tal (no se puede modificar el estado de un gasto pagado)


void marcarGastoComoPagado(TreeMap *arbol) {
   
    int opcionMes;
    MesFinanciero *datosMes = NULL;
    do {
        printf("Ingrese 0 para volver al menú principal.\n");
        printf("Seleccione el mes para marcar gasto como pagado:\n");
        for (int i = 0; i < 12; i++) {
            printf("%2d. %s\n", i + 1, nombresMeses[i]);
        }
        printf("Ingrese el número del mes (1-12): ");
        scanf("%d", &opcionMes);

        if (opcionMes == 0) {
            printf("Volviendo al menú principal...\n");
            return;
        }
        if (opcionMes < 1 || opcionMes > 12) {
            printf("Opción inválida. Intente nuevamente.\n");
            continue;
        }

        Pair *par = searchTreeMap(arbol, (void*)nombresMeses[opcionMes - 1]);
        if (!par) {
            printf("El mes seleccionado no existe. Intente nuevamente.\n");
            continue;
        }

        datosMes = (MesFinanciero*)par->value;
        if (datosMes->modificado == 0) {
            printf("El mes seleccionado no ha sido modificado. Seleccione otro mes.\n");
            datosMes = NULL;
        }
    } while (datosMes == NULL);

    // Mostrar solo gastos pendientes
    int idx = 1;
    int indicesPendientes[100]; // Asume máximo 100 gastos pendientes por mes
    Gasto* g = list_first(datosMes->listaGastos);
    printf("Gastos pendientes:\n");
    int hayPendientes = 0;
    int i = 1;
    while (g) {
        if (strcmp(g->estado, "Pendiente") == 0) {
            printf("%2d. %s | Monto: %d | Estado: %s\n", idx, g->categoria, g->monto, g->estado);
            indicesPendientes[idx] = i; // Guarda el índice real en la lista
            idx++;
            hayPendientes = 1;
        }
        g = list_next(datosMes->listaGastos);
        i++;
    }
    if (!hayPendientes) {
        printf("No hay gastos pendientes para el mes seleccionado.\n");
        return;
    }

    int opcionGasto;
    do {
        printf("Seleccione el gasto pendiente a marcar como pagado (0 para salir): ");
        scanf("%d", &opcionGasto);
        if (opcionGasto == 0) break;

        if (opcionGasto < 1 || opcionGasto >= idx) {
            printf("Opción inválida. Intente nuevamente.\n");
            continue;
        }

        // Buscar el gasto pendiente correspondiente
        g = list_first(datosMes->listaGastos);
        int realIdx = indicesPendientes[opcionGasto];
        for (int j = 1; j < realIdx; j++) {
            g = list_next(datosMes->listaGastos);
        }

        if (strcmp(g->estado, "Pagado") == 0) {
            printf("El gasto ya está marcado como pagado.\n");
            continue;
        }
        if (datosMes->ahorrado < g->monto) {
            printf("No hay suficiente dinero para pagar este gasto.\n");
            strcpy(g->estado, "Pendiente");
            printf("El gasto se ha dejado como pendiente.\n");
            continue;
        }
        strcpy(g->estado, "Pagado");
        printf("Gasto marcado como pagado: %s | Monto: %d\n", g->categoria, g->monto);
        datosMes->ahorrado -= g->monto; // Descontar del ahorrado
        //aumentar el total de gastos pagados
        datosMes->totalGastos += g->monto;
    } while (opcionGasto != 0);
}

void recuperarGastosPendientes(TreeMap *arbol) {
    MesFinanciero *mesActual = NULL;
    int indiceMesActual = -1;
    for (int i = 11; i >= 0; i--) {
        Pair *par = searchTreeMap(arbol, (void*)nombresMeses[i]);
        if (par != NULL) {
            MesFinanciero *mes = (MesFinanciero*)par->value;
            if (mes->modificado == 1) {
                mesActual = mes;
                indiceMesActual = i;
                break;
            }
        }
    }
    if (mesActual == NULL) {
        printf("No hay meses modificados para recuperar gastos pendientes.\n");
        return;
    }

    printf("Se usarán 'Otros Gastos' del mes %s como destino de recuperacion.\n", mesActual->nombreMes);

    Gasto *otrosActual = NULL;
    Gasto *gasto = list_first(mesActual->listaGastos);
    while (gasto != NULL) {
        if (strcmp(gasto->categoria, "Otros Gastos") == 0) {
            otrosActual = gasto;
            break;
        }
        gasto = list_next(mesActual->listaGastos);
    }

    for (int i = 0; i < indiceMesActual; i++) {
        Pair *par = searchTreeMap(arbol, (void*)nombresMeses[i]);
        if (par == NULL) continue;

        MesFinanciero *mes = (MesFinanciero*)par->value;
        if (mes->modificado == 0) {
            printf("El mes %s no ha sido modificado, no se recuperarán gastos pendientes.\n", mes->nombreMes);
            continue;
        }

        int totalGastosPendientes = 0;
        Gasto *gastoPendiente = list_first(mes->listaGastos);
        while (gastoPendiente != NULL) {
            if (strcmp(gastoPendiente->estado, "Pendiente") == 0) {
                totalGastosPendientes += gastoPendiente->monto;
            }
            gastoPendiente = list_next(mes->listaGastos); 
        }
        if (totalGastosPendientes == 0) continue;

        int respuesta;
        printf("\nEl mes %s tiene %d en gastos pendientes.\n", mes->nombreMes, totalGastosPendientes);
        printf("¿Deseas Trasladarlos al mes actual (%s) como 'Otros Gastos'? (1=Sí, 0=No): ", mesActual->nombreMes);
        scanf("%d", &respuesta);
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
        printf("Gastos pendientes del mes %s trasladados a 'Otros Gastos' del mes %s.\n", mes->nombreMes, mesActual->nombreMes);
    }
    printf("\nRecuperación de gastos pendientes completada. Total final en 'Otros Gastos' : %d\n", otrosActual->monto);
}

void modificarGasto(TreeMap *arbol){
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║                      GESTIONAR GASTOS                      ║\n");
    printf("╠════════════════════════════════════════════════════════════╣\n");
    printf("║                                                            ║\n");
    printf("║  1. Marcar un gasto pendiente como 'Pagado'                ║\n");
    printf("║  2. Recuperar gastos pendientes de meses anteriores        ║\n");
    printf("║                                                            ║\n");
    printf("║  0. Volver al menú principal                               ║\n");
    printf("║                                                            ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n");
    printf("\n  Ingrese su opción: ");



    int opcion;
    scanf("%d", &opcion);   

    switch (opcion) {
        case 1:
            marcarGastoComoPagado(arbol);
            break;
        case 2:
            recuperarGastosPendientes(arbol);
            break;
        case 0:
            printf("Volviendo al menú principal...\n");
            return;
        default:
            printf("Opción inválida. Volviendo al menú principal...\n");
            return;
    }

}

void mostrarPorcentajesPorCategorias(TreeMap *arbol)
{
    int seleccionado[12] = {0}; 
    int cantidad = 0;

    printf("Seleccione los meses para mostrar porcentajes por categorías (0 para terminar):\n");

    for (int i = 0; i < 12; i++) {
        printf("%2d. %s\n", i + 1, nombresMeses[i]);
    }

    while (1)
    {
        int opcion;
        printf("Ingrese el número del mes (1-12) o 0 para terminar: ");
        scanf("%d", &opcion);

        if (opcion == 0) break;

        if (opcion < 1 || opcion > 12) {
            printf("Opción inválida. Intente nuevamente.\n");
            continue;
        }

        bool yaSeleccionado = false;
        for (int j = 0; j < cantidad; j++) {
            if (seleccionado[j] == opcion) {
                yaSeleccionado = true;
                break;
            }
        }

        if (yaSeleccionado) {
            printf("El mes %s ya ha sido seleccionado.\n", nombresMeses[opcion - 1]);
        } else {
            seleccionado[cantidad++] = opcion;
            printf("Mes %s seleccionado.\n", nombresMeses[opcion - 1]);
        }
    }

    if (cantidad == 0) {
        printf("No se han seleccionado meses. Saliendo...\n");
        return;
    }

    printf("\n--- Porcentajes por Categorías ---\n");

    for (int i = 0; i < cantidad; i++) {
        int mesSeleccionado = seleccionado[i] - 1;
        Pair *par = searchTreeMap(arbol, (void*)nombresMeses[mesSeleccionado]);

        if (par == NULL) {
            printf("El mes %s no tiene datos registrados.\n", nombresMeses[mesSeleccionado]);
            continue;
        }

        MesFinanciero *mes = (MesFinanciero*)par->value;

        if (mes->totalGastos == 0) {
            printf("\n[%s] No hay gastos registrados.\n", mes->nombreMes);
            continue;
        }

        printf("\nResumen para %s:\n", mes->nombreMes);
        printf("Total Gastos: %d\n", mes->totalGastos);
        printf("=========================\n");

        Gasto *gasto = list_first(mes->listaGastos);
        while (gasto) {
            if (strcasecmp(gasto->estado, "Pagado") == 0) {
                float porcentaje = (float)gasto->monto / mes->totalGastos * 100;
                printf("Categoría: %-15s | Monto: %6d | Porcentaje: %.2f%%\n",
                       gasto->categoria, gasto->monto, porcentaje);
            }
            gasto = list_next(mes->listaGastos);
        }
    }
}

bool verificarArchivoExistente(int *anyo, char *nombreArchivo, int opcion) {
    if (opcion == 1) {
        printf("Ingrese el año del archivo a cargar: ");
    } else if (opcion == 2) {
        printf("Ingrese el año para crear un nuevo archivo: ");
    } else if (opcion == 3) {
        printf("Ingrese el año del archivo a guardar: ");
    } else {
        return false; // Opción inválida
    }
    scanf("%d", anyo);
    sprintf(nombreArchivo, "finanzas_%d.csv", *anyo);
    FILE *archivo = fopen(nombreArchivo, "r");
    if (archivo) {
        fclose(archivo);
        return true; // El archivo ya existe
    } else {
        return false; // El archivo no existe
    }

}

void accionesAlCSV (TreeMap *arbol, bool *cargado) {

    printf("\n");
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║                 ACCIONES DE ARCHIVO CSV                    ║\n");
    printf("╠════════════════════════════════════════════════════════════╣\n");
    printf("║                                                            ║\n");
    printf("║  1. Cargar archivo de finanzas existente                   ║\n");
    printf("║  2. Crear nuevo archivo de finanzas (desde plantilla)      ║\n");
    printf("║  3. Guardar cambios en el archivo actual                   ║\n");
    printf("║                                                            ║\n");
    printf("║  0. Volver al menú principal                               ║\n");
    printf("║                                                            ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n");
    printf("\nIngrese su opción: ");

    int opcion;
    scanf("%d", &opcion);

    int anyo;
    char nombreArchivo[100];
    switch (opcion) {
        case 1:
            if (*cargado) {
                printf("[!] Ya se ha cargado un archivo de finanzas.\n");
                return;
            }
            if (!verificarArchivoExistente(&anyo, nombreArchivo, opcion)) {
                printf("No se pudo verificar el archivo. Asegúrate de que el archivo exista.\n");
                return;
            }
            *cargado = true; // Marcar que se ha cargado un archivo CSV
            cargarMovimientosDesdeCSV(arbol, nombreArchivo);
            printf("[OK] Archivo de finanzas cargado correctamente.\n");
            break;
        case 2:
            if (verificarArchivoExistente(&anyo, nombreArchivo, opcion)) {
                printf("[!] El archivo ya existe, no se sobreescribirá.\n");
                return; 
            }
            copiarArchivoCSV("plantilla.csv", nombreArchivo);
            break;
        case 3:
            if (!*cargado) {
                printf(" [!] No se ha cargado ningún archivo de finanzas. Carga un archivo antes de guardar.\n");
                return;
            }
            if (!verificarArchivoExistente(&anyo, nombreArchivo, opcion)) {
                printf("No se pudo verificar el archivo. Asegúrate de que el archivo exista.\n");
                return;
            }
            guardarCSV(arbol, nombreArchivo);
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








//si se accede a un mes no modificiado se sale del bucle, hay que hacer que se reintente si no se ha modificado el mes (YA LO ARREGLÉ XD)
//podria solo mostrar cuentas que si se hayan registrado, o en su defecto no dejar marcar no registrados
// crear logica de que que si quedan pendientes en un mes "cerrado" se vaya a "otros gastos" del proximo mes (ya lo hice)

// despues de usar la funcion marcar te pregunta si deseas cerrar el mes pero no sirve de nada si el mes ya se c0erro al haber usado la
//funcion 1 registrar mov financiero. Creo que al menos en esta funcion no deberia de hacerse esa pregunta, solo preguntar si desea
//mover los p endites al siguiente mes, y si no hay pendientes no hacer nada, o si no hay gastos no hacer nada