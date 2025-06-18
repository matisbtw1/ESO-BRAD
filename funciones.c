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

  char **campos = leer_linea_csv(archivo, ','); // Leer encabezado
  if (!campos) return;

  // Guardar índices importantes
  int idxMes = -1;
  int categoriasInicio = -1;

  for (int i = 0; campos[i] != NULL; i++) {
    if (strcmp(campos[i], "Mes") == 0) idxMes = i;
    else if (categoriasInicio == -1 && strcmp(campos[i], "Agua") == 0) categoriasInicio = i;
  }

  while ((campos = leer_linea_csv(archivo, ',')) != NULL) {
    char *mes = strdup(campos[idxMes]);  // Clave del árbol

    // Revisar si ya existe una lista para ese mes
    Pair *par = searchTreeMap(arbol, mes);
    List *lista;

    if (par == NULL) {
      lista = list_create();
      insertTreeMap(arbol, mes, lista);
    } else {
      free(mes); // Ya existe, usamos la misma clave
      lista = (List *)par->value;
    }

    // Cargar los movimientos del mes actual
    for (int i = categoriasInicio; campos[i] != NULL; i += 2) {
      if (campos[i + 1] == NULL) break; // Evitar acceso fuera de rango

      Movimiento *mov = malloc(sizeof(Movimiento));
      strcpy(mov->categoria, campos[i]);
      mov->valor = atoi(campos[i]);
      strcpy(mov->estado, campos[i + 1]);

      list_pushBack(lista, mov);
    }
  }

  fclose(archivo);
}

void mostrarMovimientosPorMes(TreeMap *arbol) {
  Pair *par = firstTreeMap(arbol);

  while (par != NULL) {
    char *mes = (char *)par->key;
    List *lista = (List *)par->value;

    printf("Mes: %s\n", mes);
    printf("-----------------------------------\n");

    Movimiento *mov = list_first(lista);
    while (mov != NULL) {
      printf("Categoría: %-15s | Valor: %6d | Estado: %s\n",
             mov->categoria, mov->valor, mov->estado);
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