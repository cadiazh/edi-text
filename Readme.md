````md id="9qysf9"
# EDI-TEXT

Editor de texto minimalista desarrollado en C para Linux utilizando llamadas POSIX directas, manejo dinámico de memoria y compresión en User Space.

El proyecto fue diseñado para analizar el impacto de distintos algoritmos de compresión sobre el subsistema I/O de Linux, comparando rendimiento, cantidad de syscalls y tamaño final de los archivos.

---

# Características

- Manejo dinámico de memoria con:
  - `malloc`
  - `realloc`
  - `free`

- Persistencia usando syscalls POSIX:
  - `open`
  - `read`
  - `write`
  - `close`

- Escritura optimizada en bloques de 4KB

- Soporte para múltiples algoritmos de compresión:
  - zlib
  - RLE (Run-Length Encoding)

- Formato binario personalizado (`.edt`)

- Carga optimizada utilizando `mmap()`

- Benchmarking y profiling usando:
  - `strace`
  - `time`
  - `valgrind`

---

# Estructura del proyecto

```text
edi-text/
│
├── build/
├── docs/
├── include/
│   ├── buffer.h
│   ├── compression.h
│   └── io.h
│
├── src/
│   ├── benchmark_main.c
│   ├── buffer.c
│   ├── compression.c
│   ├── io.c
│   └── main.c
│
├── tests/
├── Makefile
└── README.md
```

---

# Compilación

Compilar el editor:

```bash
make
```

---

# Ejecutar editor

```bash
make run
```

---

# Ejecutar benchmark

```bash
make benchmark
```

---

# Funcionamiento del sistema

## Pipeline de guardado

```text
Texto usuario
↓
TextBuffer (RAM)
↓
Compresión
↓
Header binario
↓
write() POSIX
↓
Disco
```

## Pipeline de apertura

```text
Archivo .edt
↓
read() / mmap()
↓
Lectura de Header
↓
Descompresión
↓
TextBuffer
↓
Editor
```

---

# Gestión de memoria

El editor utiliza un buffer dinámico para almacenar el contenido del archivo en memoria RAM.

```c
typedef struct {

    char *data;

    size_t length;

    size_t capacity;

} TextBuffer;
```

El buffer crece dinámicamente utilizando `realloc()` conforme el usuario escribe texto.

Toda la memoria es liberada correctamente al finalizar la ejecución.

---

# Formato binario

Los archivos `.edt` utilizan un formato binario personalizado:

```text
[HEADER][PAYLOAD COMPRESSED]
```

Header utilizado:

```c
typedef struct __attribute__((packed)) {

    char magic[4];

    uint32_t algorithm;

    uint32_t original_size;

    uint32_t compressed_size;

} FileHeader;
```

## Campos del header

| Campo | Descripción |
|---|---|
| magic | Identificador del archivo (`EDTX`) |
| algorithm | Algoritmo utilizado |
| original_size | Tamaño original |
| compressed_size | Tamaño comprimido |

---

# Algoritmos implementados

## zlib

Compresión basada en LZ77 + Huffman.

### Ventajas

- Excelente ratio de compresión
- Muy eficiente para texto natural
- Reduce drásticamente tráfico I/O

### Desventajas

- Mayor uso de CPU

---

## RLE (Run-Length Encoding)

Algoritmo implementado manualmente en C.

### Ventajas

- Muy rápido
- Bajo consumo CPU
- Excelente en datos repetitivos

### Desventajas

- Dependiente del patrón de datos
- Puede aumentar el tamaño de archivos naturales

---

# Benchmarking

## Dataset repetitivo

Archivo original: `15 MB`

| Métrica | zlib | RLE |
|---|---|---|
| Archivo final | 37 KB | 2 MB |
| real | 0.100s | 0.066s |
| user | 0.090s | 0.052s |
| sys | 0.010s | 0.014s |
| write() | 12 | 491 |

### Conclusión

- RLE fue más rápido.
- zlib obtuvo mucha mejor compresión.
- zlib redujo drásticamente las syscalls `write()`.

---

## Dataset natural

Archivo original: `58 MB`

| Métrica | zlib | RLE |
|---|---|---|
| Archivo final | 200 KB | 115 MB |
| real | 0.350s | 0.596s |
| user | 0.332s | 0.356s |
| sys | 0.017s | 0.237s |

### Conclusión

- zlib mantuvo una excelente compresión.
- RLE empeoró significativamente el tamaño final.
- RLE incrementó el tiempo en modo kernel (`sys`).

---

# Resultados y análisis

Los resultados demostraron que invertir ciclos de CPU en User Space mediante compresión puede reducir considerablemente:

- tráfico I/O
- tamaño físico de los archivos
- presión sobre el kernel
- cantidad de syscalls `write()`

También se observó que la eficiencia de un algoritmo depende directamente del tipo de datos procesados.

---

# Herramientas utilizadas

- GCC
- Linux POSIX API
- zlib
- mmap()
- strace
- valgrind
- time

---

# Validación de memoria

```bash
valgrind --leak-check=full ./build/editor
```

Resultado obtenido:

```text
All heap blocks were freed -- no leaks are possible
```

---

# Profiling

Ejemplo de profiling:

```bash
strace -c ./build/benchmark repetitive.txt repetitive_zlib.edt 1
```

---

# Ejemplo de uso

## Crear archivo nuevo

```text
1
archivo.edt
```

Seleccionar algoritmo:

```text
1 -> zlib
2 -> RLE
```

Guardar:

```text
:wq
```

---

## Abrir archivo existente

```text
2
archivo.edt
```

---

# Autor

Arturo
````
