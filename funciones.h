//Implementacion para crear un csv de finanzas del año ingresado por el usuario 
/*
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
    } else {
        copiarArchivoCSV("plantilla.csv", nombreArchivo);
    }
*/