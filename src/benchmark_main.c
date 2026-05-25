#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "../include/buffer.h"
#include "../include/io.h"
#include "../include/crypto.h"

int main(int argc, char *argv[]) {

    if (argc != 5) {

        printf(
            "Uso:\n%s <input> <output> <mode> <algorithm>\n",
            argv[0]
        );

        return 1;
    }

    const char *input_file = argv[1];

    const char *output_file = argv[2];

    int mode = atoi(argv[3]);

    uint32_t algorithm = atoi(argv[4]);

    unsigned char key[] = "benchmarkkey";

    TextBuffer buffer;

    buffer_init(&buffer);

    if (
        load_plain_text_mmap(
            input_file,
            &buffer
        ) != 0
    ) {

        printf("Error cargando archivo.\n");

        buffer_free(&buffer);

        return 1;
    }

    if (mode == 0) {

        FILE *f = fopen(output_file, "w");

        if (!f) {

            buffer_free(&buffer);

            return 1;
        }

        fwrite(
            buffer.data,
            1,
            buffer.length,
            f
        );

        fclose(f);
    }

    else if (mode == 1) {

        if (
            save_file(
                output_file,
                &buffer,
                algorithm,
                key,
                0
            ) != 0
        ) {

            printf("Error guardando.\n");

            buffer_free(&buffer);

            return 1;
        }
    }

    else if (mode == 2) {

        if (
            save_file(
                output_file,
                &buffer,
                algorithm,
                key,
                1
            ) != 0
        ) {

            printf("Error guardando.\n");

            buffer_free(&buffer);

            return 1;
        }
    }

    secure_zero(
        key,
        sizeof(key)
    );

    buffer_free(&buffer);

    printf("Benchmark completado.\n");

    return 0;
}