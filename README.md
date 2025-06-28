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

**nota** Se dejara un csv finanzas_2026, por si requieren casos de prueba.

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

- Problemas con el buffer de entrada:

    -Al usar scanf y getchar juntos, a veces quedan caracteres en el buffer, lo que   puede hacer que se salte la espera de "Presiona Enter para continuar".

- No manejo de archivos corruptos o mal formateados:

    -Si el archivo CSV tiene un formato incorrecto, la carga puede fallar o cargar datos erróneos sin advertencia clara.

- No hay límite para montos o cantidades:

    -El usuario puede ingresar montos negativos o extremadamente altos sin advertencia.

- Dependencia de la consola y del sistema operativo:
    -El uso de funciones como limpiarConsola() y colores ANSI puede no funcionar igual en todas las plataformas.

- Error al validar entradas
    -Algunas lecturas de datos (scanf) pueden ocasionar un bucle infinito si se ingresa algo no esperado como respuesta.

**POSIBLES MEJORAS**

- Mejorar la validación de entradas del usuario para evitar errores por datos no numéricos o fuera de rango.

- Agregar límites y validaciones para montos negativos o excesivamente altos.

- Añadir soporte multiplataforma para limpiar la consola y mostrar colores de forma consistente.

- Añadir gráficos de tendencias y comparativas visuales.

- Implementar una interfaz gráfica (GUI) para facilitar el uso a usuarios no técnicos.


**EJEMPLO DE USO**

----------------------------------------------
- primero el menu principal : 
----------------------------------------------


╔════════════════════════════════════════════════════════════╗
║         SISTEMA DE GESTIÓN FINANCIERA PERSONAL (SGFP)      ║
╠════════════════════════════════════════════════════════════╣
║                                                            ║
║  1. Registrar Movimiento Financiero                        ║
║  2. Ver Resumen Mensual                                    ║
║  3. Gestionar Gastos                                       ║
║  4. Historial y Análisis                                   ║
║  5. Ver Excedente Mensual                                  ║
║  6. Acciones de Archivo CSV (Cargar/Crear/Guardar)         ║
║                                                            ║
║  0. Salir                                                  ║
╚════════════════════════════════════════════════════════════╝

Seleccione una opción (0-6): 6

Al seleccionar la opción 6 en el menú principal, se despliega un submenú como el siguiente:

╔════════════════════════════════════════════════════════════╗
║            ACCIONES DE ARCHIVO CSV DE FINANZAS             ║
╠════════════════════════════════════════════════════════════╣
║  1. Cargar archivo de finanzas existente                   ║
║  2. Crear nuevo archivo de finanzas para un año            ║
║  3. Guardar cambios en el archivo actual                   ║
║  0. Volver al menú principal                               ║
╚════════════════════════════════════════════════════════════╝

Seleccione una opción (0-3):

- Si eliges **1**, puedes cargar un archivo existente, por ejemplo:
    ```
    Ingrese el año del archivo de finanzas a cargar: 2024
    [✓] Archivo finanzas_2024.csv cargado correctamente.
    ```

- Si eliges **2**, puedes crear un nuevo archivo para un año específico:
    ```
    Ingrese el año para el nuevo archivo de finanzas: 2025
    [✓] Archivo [finanzas_2025.csv](http://_vscodecontentref_/0) creado exitosamente a partir de la plantilla.
    ```

- Si eliges **3**, puedes guardar los cambios realizados:
    ```
    [✓] Cambios guardados correctamente en finanzas_2024.csv
    ```

- Si eliges **0**, vuelves al menú principal.


----------------------------------------------
- Registrar movimiento financiero :
----------------------------------------------


╔════════════════════════════════════════════════════════════╗
║              REGISTRAR MOVIMIENTO FINANCIERO              ║
╠════════════════════════════════════════════════════════════╣
║  [INFO] Seleccione un mes para registrar sus ingresos y    ║
║  gastos. Una vez finalizado, el mes se marcará como        ║
║  'cerrado' y no podrá ser modificado. Si desea modificarlo ║
║  debe eliminarlo y registrarlo nuevamente.                 ║
╚════════════════════════════════════════════════════════════╝

  1. Enero
  2. Febrero
  3. Marzo
  ...
>> Ingrese el número del mes (1-12): 3

Ingrese el monto de ingresos para el mes Marzo: 500000
>> ¿Confirma el ingreso de 500000 para el mes Marzo? (1 = Sí, 0 = No): 1
[OK] Ingreso de 500000 registrado para el mes Marzo.


----------------------------------------------
- Registrar gasto :
----------------------------------------------


╔══════════════════════════════════════════════════════════╗
  CATEGORÍAS DE GASTOS DEL MES Marzo
╠══════════════════════════════════════════════════════════╣
  1. Agua            | Monto: 0 | Estado: No Registra
  2. Luz             | Monto: 0 | Estado: No Registra
  3. Alimentacion    | Monto: 0 | Estado: No Registra
  ...
╚══════════════════════════════════════════════════════════╝

Seleccione una categoría para modificar (0 para salir): 3
Ingrese el nuevo monto para la categoría Alimentacion: 120000
Confirma el cambio de monto a 120000 para la categoría Alimentacion? (1=Sí, 0=No): 1
Seleccione el estado del gasto:
  1. Pendiente
  2. Pagado
Ingrese su opción: 2
Gasto marcado como pagado


----------------------------------------------
- Ver resumen de meses (paginado) :
----------------------------------------------


    --- Página 1 de 1 (meses 1-3 de 3) ---

╔══════════════════════════════════════════════════════════╗
  MES: Marzo
  Ingresos:    500000
  Ahorro:      380000
  Total Gastos:120000
  Categorías registradas:
    - Alimentacion    | Monto: 120000 | Estado: Pagado
╚══════════════════════════════════════════════════════════╝

Mostrando meses 1-3 de 3. Página 1 de 1.
Presiona Enter para continuar...


----------------------------------------------
- Marcar gasto como pagado:
----------------------------------------------


╔══════════════════════════════════════════════════════════╗
  MARCAR GASTO COMO PAGADO
╠══════════════════════════════════════════════════════════╣
  Ingrese 0 para volver al menú principal.
  Seleccione el mes para marcar gasto como pagado:
    1. Enero
    2. Febrero
    3. Marzo
╚══════════════════════════════════════════════════════════╝

Ingrese el número del mes (1-12): 3

╔══════════════════════════════════════════════════════════╗
  GASTOS PENDIENTES EN Marzo
╠══════════════════════════════════════════════════════════╣
    1. Agua            | Monto: 15000 | Estado: Pendiente
    2. Luz             | Monto: 20000 | Estado: Pendiente
╚══════════════════════════════════════════════════════════╝

Seleccione el gasto pendiente a marcar como pagado (0 para salir): 1
Gasto marcado como pagado: Agua | Monto: 15000


----------------------------------------------
- Comparar gastos entre meses:
----------------------------------------------


╔══════════════════════════════════════════════════════════╗
  COMPARACIÓN DE GASTOS ENTRE DOS MESES
╠══════════════════════════════════════════════════════════╣
  1. Enero
  2. Febrero
  3. Marzo
  ...
Ingrese el número del primer mes (1-12): 2
Ingrese el número del segundo mes (1-12): 3

¿Respecto a qué mes desea comparar los gastos?
  1. Febrero
  2. Marzo
Seleccione una opción (1 o 2): 2

╔══════════════════════════════════════════════════════════╗
  Comparación de gastos entre Febrero y Marzo
╠══════════════════════════════════════════════════════════╣
  Categoría       | Febrero    | Marzo      | Variación
  --------------------------------------------------------
  Alimentacion    |     90000  |   120000   |   +33.33%
  Luz             |     20000  |    20000   |     0.00%
╚══════════════════════════════════════════════════════════╝


**CONTRIBUCIONES INTEGRANTES**

- **Francisco:**  
  Manejo CSV. Registrar movimientos. Lógica para guardar CSV
  
  Registro de movimientos financieros. Implementación de la lógica para registrar ingresos y gastos, validaciones y estructura de datos.

- **Alfonso:**  
  Registrar movimientos. Elaboración y cambios de interfaz. D
  
  
  Definición e implementación de TDAS.Resúmenes y visualización de meses. Desarrollo de funciones para mostrar resúmenes mensuales, paginación y filtrado de datos.

- **Matias:**  
  Registrar movimientos. Marcar gasto como pagados, gastos planificados
  
  
  Gastos planificados y gestión de pendientes. Programación de funciones para ver, marcar y mover gastos pendientes entre meses.

- **Juan Pablo:**  
  Registrar movimientos. Marcar gasto como pagados, gastos planificados
  
  Historial, análisis y excedente. Implementación de análisis comparativo, tendencias, excedente mensual y funciones de ahorro.

- **Todos:**  
  Interfaz, prints y documentación. Mejora de la experiencia de usuario, mensajes en consola y redacción de la documentación.

> **Nota:** Todos los integrantes estuvieron en la revisión, pruebas y mejoras generales del sistema.