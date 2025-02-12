# Arikkei

This is collection of small code snippets that do not deserve to have own library

The parts written by me are in public domain.

Tinycthread is Copyright (c) 2012 Marcus Geelnard, please consult the actual file for distribution terms.

## Compilation

    cmake -S . -B build
    cmake --build build

Generates a static library

Alternately just copy needed headers/sources into your project and compile into the code

## Testing

In build directory invoke

    ctest

## Contents

### arikkei-strlib

- convenience stcpy methods - NULL is valid string, destination can be NULL
- methods to convert between utf8 and utf16
- integer <-> string conversion
- double <-> string conversion

### arikkei-iolib

- cross-platform memory mapping
- fopen with utf8 name/permissions (for Windows)
- epoch time as double

### arikkei-token

Implements a simple struct that can be used to handle texts from static storage (e.g. memory-mapped files)

### arikkei-dict

A simple hash table implementation, inspired from GLib

### arikkei-cache

A cache implemented on top of ArikkeiDict
