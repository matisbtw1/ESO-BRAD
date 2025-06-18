#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "tdas/extra.h"
#include "tdas/list.h"
#include "funciones.h"

void mostrarMenu() {
    printf("Menu de opciones:\n");
    printf("1. Registrar movimiento financiero\n");
    printf("2. Ver resumen mensual\n");
    printf("3. Presupuesto mensual\n");
    printf("4. Gastos planificados\n");
    printf("5. Historial y análisis\n");
    printf("6. Excedente mensual\n");
    printf("7. Ver cuenta de ahorro\n");
    printf("8. Crear nuevo csv finanzas anio\n");
    printf("0. Salir\n");
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
        mes->modificado = strcmp(campos[idxModificado], "Sí") == 0 ? 1 : 0;
        mes->listaGastos = list_create();

        // Leer gastos
        for (int i = idxInicioGastos; i < idxTotalGastos; i += 2) {
            if (campos[i] == NULL || campos[i + 1] == NULL) break;

            Gasto *g = malloc(sizeof(Gasto));
            strcpy(g->categoria, encabezados[i]); // Nombre viene del header
            g->monto = atoi(campos[i]);
            strcpy(g->estado, campos[i + 1]);

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

void mostrarMovimientosPorMes(TreeMap *arbol) {
  Pair *par = firstTreeMap(arbol);

  while (par != NULL) {
    char *mes = (char *)par->key;
    MesFinanciero *mesFinanciero = (MesFinanciero *)par->value;

    printf("Mes: %s\n", mes);
    printf("Ingresos: %d\n", mesFinanciero->ingresos);
    printf("Ahorro: %d\n", mesFinanciero->ahorrado);
    printf("Total Gastos: %d\n", mesFinanciero->totalGastos);
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