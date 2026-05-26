# EDI-TEXT v5.0

Editor de texto minimalista desarrollado en C para Linux utilizando llamadas POSIX directas, manejo dinámico de memoria, compresión en User Space y cifrado simétrico RC4.

El proyecto fue diseñado para analizar el impacto de la compresión y la seguridad criptográfica sobre el subsistema I/O de Linux, comparando rendimiento, uso de CPU, syscalls y tamaño final de los archivos.

---
# Video explicativo

-link del video: [Editor de Texto en C](https://youtu.be/frBT35r6HW0)

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

- Cifrado simétrico RC4

- Formato binario personalizado (`.edt`)

- Carga optimizada utilizando `mmap()`

- Eliminación segura de llaves desde RAM

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
│   ├── crypto.h
│   └── io.h
│
├── src/
│   ├── benchmark_main.c
│   ├── buffer.c
│   ├── compression.c
│   ├── crypto.c
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
Encriptación RC4
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
Desencriptación RC4
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

Las llaves criptográficas son eliminadas utilizando:

```c
secure_zero()
```

para evitar residuos sensibles en memoria RAM.

---

# Formato binario

Los archivos `.edt` utilizan un formato binario personalizado:

```text
[HEADER][PAYLOAD COMPRESSED + ENCRYPTED]
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

# Cifrado

## RC4

Stream cipher simétrico implementado en C.

### Ventajas

- Muy ligero
- Bajo overhead
- No requiere padding
- Mantiene el tamaño comprimido

### Seguridad implementada

- La llave se solicita por consola
- La llave no está hardcodeada
- La llave es eliminada de RAM usando `secure_zero()`

---

# Benchmarking

## Dataset repetitivo

Archivo original: `15 MB`

| Pipeline | Tamaño Final | real | user | sys |
|---|---|---|---|---|
| Plano | 15 MB | 0.049s | 0.012s | 0.025s |
| Compresión | 37 KB | 0.141s | 0.127s | 0.013s |
| Compresión + RC4 | 37 KB | 0.143s | 0.131s | 0.010s |

### Conclusión

- La compresión redujo drásticamente el tamaño físico.
- RC4 agregó un overhead mínimo.
- El tiempo kernel (`sys`) disminuyó significativamente.
- El sistema redujo presión sobre el bus I/O.

---

## Dataset natural

Archivo original: `58 MB`

| Pipeline | Tamaño Final | real | user | sys |
|---|---|---|---|---|
| Plano | 58 MB | 0.139s | 0.034s | 0.075s |
| Compresión | 200 KB | 0.510s | 0.485s | 0.024s |
| Compresión + RC4 | 200 KB | 0.510s | 0.481s | 0.024s |

### Conclusión

- zlib mantuvo una excelente compresión.
- RC4 no destruyó la compresión ya aplicada.
- El sistema redujo significativamente el tráfico I/O.
- El costo adicional de RC4 fue mínimo.

---

# Resultados y análisis

Los resultados demostraron que invertir ciclos de CPU en User Space mediante compresión y cifrado puede reducir considerablemente:

- tráfico I/O
- tamaño físico de los archivos
- presión sobre el kernel
- tiempo en modo kernel (`sys`)

También se comprobó que:

```text
compress → encrypt
```

es el orden correcto del pipeline.

Encriptar antes de comprimir genera alta entropía, haciendo imposible detectar patrones repetitivos y destruyendo la efectividad de la compresión.

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
strace -c ./build/benchmark natural.txt natural_secure.edt 2 1
```

---

# Ejemplo de uso

## Crear archivo nuevo

```text
1
archivo.edt
miclave
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
miclave
```

---

# Conceptos de Sistemas Operativos Aplicados

- User Space vs Kernel Space
- Optimización del bus I/O
- Syscalls POSIX
- Buffers de memoria
- Memoria dinámica
- Compresión de datos
- Entropía y cifrado
- Seguridad en RAM
- Context switches
- Profiling de procesos
- Cifrado simétrico

---

# Autor
Arturo