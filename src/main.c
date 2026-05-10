#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "../include/buffer.h"
#include "../include/io.h"

int main() {

    TextBuffer buffer;

    buffer_init(&buffer);

    char filename[256];

    char option[10];

    char line[1024];

    char algorithm_option[10];

    uint32_t algorithm = ALGO_ZLIB;

    int editing_existing = 0;

    printf("EDI-TEXT v4.1\n");

    printf("\n1. Nuevo archivo\n");
    printf("2. Abrir archivo\n");

    printf("\nSeleccion: ");

    if (fgets(option, sizeof(option), stdin) == NULL) {

        buffer_free(&buffer);

        return 1;
    }

    printf("Nombre del archivo: ");

    if (fgets(filename, sizeof(filename), stdin) == NULL) {

        buffer_free(&buffer);

        return 1;
    }

    filename[strcspn(filename, "\n")] = '\0';

    if (option[0] == '2') {

        editing_existing = 1;

        if (load_file(filename, &buffer) == 0) {

            printf("\n=== CONTENIDO CARGADO ===\n\n");

            printf("%s\n", buffer.data);

            /*
             * Detectar algoritmo leyendo header
             */

            FILE *f = fopen(filename, "rb");

            if (f != NULL) {

                FileHeader header;

                fread(&header, sizeof(FileHeader), 1, f);

                algorithm = header.algorithm;

                fclose(f);
            }

        } else {

            printf("\nError al abrir archivo.\n");

            buffer_free(&buffer);

            return 1;
        }
    }

    if (!editing_existing) {

        printf("\nAlgoritmo:\n");
        printf("1. zlib\n");
        printf("2. RLE\n");

        printf("\nSeleccion: ");

        if (
            fgets(
                algorithm_option,
                sizeof(algorithm_option),
                stdin
            ) == NULL
        ) {

            buffer_free(&buffer);

            return 1;
        }

        if (algorithm_option[0] == '1') {

            algorithm = ALGO_ZLIB;

        } else {

            algorithm = ALGO_RLE;
        }
    }

    printf("\nEscribe ':wq' para guardar y salir\n\n");

    while (1) {

        if (fgets(line, sizeof(line), stdin) == NULL) {
            break;
        }

        if (strcmp(line, ":wq\n") == 0) {
            break;
        }

        buffer_append(&buffer, line);
    }

    if (
        save_file(
            filename,
            &buffer,
            algorithm
        ) == 0
    ) {

        printf("\nArchivo guardado correctamente.\n");

    } else {

        printf("\nError al guardar archivo.\n");
    }

    buffer_free(&buffer);

    return 0;
}