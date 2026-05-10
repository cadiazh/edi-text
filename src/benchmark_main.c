#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "../include/buffer.h"
#include "../include/io.h"

int main(int argc, char *argv[]) {

    if (argc != 4) {

        printf(
            "Uso:\n%s <input.txt> <output.edt> <algorithm>\n",
            argv[0]
        );

        return 1;
    }

    const char *input_file = argv[1];

    const char *output_file = argv[2];

    uint32_t algorithm = atoi(argv[3]);

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

    if (
        save_file(
            output_file,
            &buffer,
            algorithm
        ) != 0
    ) {

        printf("Error guardando archivo.\n");

        buffer_free(&buffer);

        return 1;
    }

    printf("Benchmark completado.\n");

    buffer_free(&buffer);

    return 0;
}