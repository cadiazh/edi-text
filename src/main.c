#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "../include/buffer.h"
#include "../include/io.h"
#include "../include/crypto.h"

int main() {

    TextBuffer buffer;
    buffer_init(&buffer);

    unsigned char key[256];
    char filename[256];
    char option[10];
    char line[1024];
    char algorithm_option[10];

    uint32_t algorithm = ALGO_ZLIB;

    int editing_existing = 0;

    printf("EDI-TEXT v5.0\n");

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

    //PEDIR LLAVE
    printf("Llave de encriptacion: ");

    if (
        fgets(
            (char *)key,
            sizeof(key),
            stdin
        ) == NULL
    ) {

        buffer_free(&buffer);

        return 1;
    }

    key[strcspn((char *)key, "\n")] = '\0';

    // ABRIR ARCHIVO
    if (option[0] == '2') {

        editing_existing = 1;

        if (
            load_file(
                filename,
                &buffer,
                key,
                1
            ) == 0
        ) {

            printf("\n=== CONTENIDO CARGADO ===\n\n");

            printf("%s\n", buffer.data);

            // Detectar algoritmo leyendo header
            FILE *f = fopen(filename, "rb");

            if (f != NULL) {

                FileHeader header;

                fread(
                    &header,
                    sizeof(FileHeader),
                    1,
                    f
                );

                algorithm =
                    header.algorithm;

                fclose(f);
            }

        } else {

            printf(
                "\nError al abrir archivo.\n"
            );

            secure_zero(
                key,
                sizeof(key)
            );

            buffer_free(&buffer);

            return 1;
        }
    }

    // NUEVO ARCHIVO
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

            secure_zero(
                key,
                sizeof(key)
            );

            buffer_free(&buffer);

            return 1;
        }

        if (algorithm_option[0] == '1') {

            algorithm = ALGO_ZLIB;

        } else {

            algorithm = ALGO_RLE;
        }
    }

    printf(
        "\nEscribe ':wq' para guardar y salir\n\n"
    );

    while (1) {

        if (
            fgets(
                line,
                sizeof(line),
                stdin
            ) == NULL
        ) {

            break;
        }

        if (
            strcmp(
                line,
                ":wq\n"
            ) == 0
        ) {

            break;
        }

        buffer_append(
            &buffer,
            line
        );
    }

    // GUARDAR ARCHIVO
    if (
        save_file(
            filename,
            &buffer,
            algorithm,
            key,
            1
        ) == 0
    ) {

        printf(
            "\nArchivo guardado correctamente.\n"
        );

    } else {

        printf(
            "\nError al guardar archivo.\n"
        );
    }
    secure_zero(
        key,
        sizeof(key)
    );

    buffer_free(&buffer);

    return 0;
}