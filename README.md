## SISTEMA DE GESTIÓN FINANCIERA PERSONAL (SGFP)

**DESCRIPCION**

SGFP es una herramienta de consola para la gestión de finanzas personales. Permite a los usuarios registrar sus ingresos y gastos mensuales, llevar un control de sus ahorros y analizar sus hábitos de consumo a lo largo del tiempo. Todos los datos se almacenan y se gestionan a través de archivos CSV, permitiendo persistencia y portabilidad de la información financiera de un año completo.

**COMO COMPILAR Y EJECUTAR**

Este proyecto esta desarrollada en lenguaje C. Puede compilarse y ejecutarse facilmente desde Visual Studio Code. A continuacion se detallan los pasos para poder compilar.

Requisitos Previos: 
- Tener instalado Visual Studio Code.
- Instalar la extension C/C++ (Microsoft).
- Tener instalado un compilador de C como gcc.
- Instalar MINGW

1. Descarga y descomprime el archivo .zip 
2. Abre la carpeta del proyecto en tu editor de codigo a terminal.
3. Para compilar, abre una terminal (como PowerShell o CMD) y ejecuta el siguiente comando. Este comando une el main.c, funciones.c y los archivos de las TDAs necesarios para crear el ejecutable.
    **gcc -g main.c funciones.c tdas/extra.c tdas/list.c tdas/treemap.c -o finanzas**
4. Una vez compilado sin errores, ejecuta el programa con:
    **./finanzas**

**FUNCIONES IMPLEMENTADAS**

- Gestion de archivos CSV

    - Carga datos financieros desde un archivo finanzas_AAAA.csv existente.

    - Crea un nuevo archivo de finanzas para un año específico, utilizando plantilla.csv como base.

    - Guarda todos los cambios realizados (ingresos, gastos, estados) en el archivo CSV cargado.

- Registro Financiero Mensual:

    - Registra ingresos y gastos detallados por categoría para cualquier mes del año.

    - Una vez que se registran los datos de un mes, este se marca como "cerrado" para prevenir modificaciones accidentales. 

    - Permite reiniciar completamente los datos de un mes si se cometió un error.

- Visualización y Resumen:

    - Muestra un resumen detallado del mes actual (último mes modificado), incluyendo ingresos, ahorros, gastos totales y el estado de cada categoría.

    - Permite ver el historial de los últimos 'X' meses con un sistema de paginación para una fácil lectura.

- Gestión de Gastos:

    - Permite marcar gastos "Pendientes" como "Pagados", descontando automáticamente el monto del ahorro disponible.

    - Recupera automáticamente los gastos pendientes de meses anteriores y los agrupa en la categoría "Otros Gastos" del mes actual para su gestión.

- Análisis Financiero:

    - Calcula y muestra el porcentaje que representa cada categoría de gasto sobre el total de gastos del mes.

    - Compara los gastos categoría por categoría entre dos meses seleccionados, mostrando la variación porcentual.

    - Muestra un resumen del ahorro mes a mes, destacando los meses con el ahorro máximo y mínimo.

**ERRORES CONOCIDOS**



**POSIBLES MEJORAS**


**EJEMPLO DE USO**


**CONTRIBUCIONES INTEGRANTES**