# HISTOGRAMA - Guía Completa de Sustentación

> Todo lo que necesitás saber para entender y sustentar el proyecto

---

## 1. Estructura del Programa en Memoria

```
┌─────────────────────────────────────────────────────────────────┐
│                    MEMORIA (Data Segment)                       │
├─────────────────────────────────────────────────────────────────┤
│ DIRECCIÓN      │ CONTENIDO              │ DESCRIPCIÓN          │
├───────────────┼──────────────────────┼─────────────────────┤
│ 0x10000000   │ edades[0] = 12         │ Primer elemento    │
│ 0x10000004   │ edades[1] = 25         │ Segundo elemento │
│ 0x10000008   │ edades[2] = 70         │ Tercer elemento  │
│ 0x1000000C   │ edades[3] = 17         │                   │
│ 0x10000010   │ edades[4] = 45         │                   │
│ 0x10000014   │ edades[5] = 8         │                   │
│ 0x10000018   │ edades[6] = 66         │                   │
│ 0x1000001C   │ edades[7] = 30         │                   │
│ 0x10000020   │ edades[8] = 15         │                   │
│ 0x10000024   │ edades[9] = 99         │ Último elemento   │
├───────────────┼──────────────────────┼─────────────────────┤
│ 0x10000028   │ contador = 4            │ Menores (<18)     │
│ 0x1000002C   │ contador = 3            │ Adultos (18-65) │
│ 0x10000030   │ contador = 3            │ Mayores (>65)   │
└───────────────┴──────────────────────┴─────────────────────┘

NOTA: Cada .word = 4 bytes → las direcciones aumentan de 4 en 4
```

---

## 2. Directivas del Ensamblador (Qué son y qué hacen)

Las directivas NO son instrucciones del procesador. Son ÓRDENES para el ensamblador.

### 2.1 Directiva .data
```asm
.data
```
**Qué hace:** Le dice al ensamblador "todo lo que viene ahora es dato, guardalo en el segmento de datos".

**Para qué sirve:** Define el espacio en memoria donde vamos a guardar:
- El arreglo de edades
- Los contadores

---

### 2.2 Directiva .text
```asm
.text
```
**Qué hace:** Le dice al ensamblador "todo lo que viene ahora es código ejecutable".

**Para qué sirve:** Define las instrucciones que el procesador va a ejecutar.

---

### 2.3 Directiva .word
```asm
edades:
    .word 12, 25, 70, 17, 45, 8, 66, 30, 15, 99
```
**Qué hace:** Reserva espacio en memoria y guarda los valores dados como ENTEROS de 32 bits.

**Características:**
- Cada `.word` = 32 bits = 4 bytes
- Se guardan en orden secuencial
- La dirección de `edades[1]` = dirección de `edades[0]` + 4

**Analogía en C:** Es como declarar `int edades[10] = {...}` en C.

---

### 2.4 Directiva .globl
```asm
.globl main
```
**Qué hace:** Exporta el símbolo `main` para que sea visible desde afuera.

**Para qué sirve:** Le dice al ensamblador "este es el punto de entrada del programa".

---

## 3. Instrucciones RISC-V (Ejecución Real)

### 3.1 Carga de dirección - la (Load Address)

```asm
la s0, edades
```

**Qué hace:** Carga en el registro `s0` la dirección de memoria donde está `edades`.

**Desglose:**
- `la` es pseudoinstrucción (se convierte en dos instrucciones reales)
- `auipc + addi` o `lui + addi` dependiendo del caso
- Carga la dirección estática, NO el valor

**En términos de C:**
```c
int* s0 = &edades[0];  // s0 apunta al inicio del arreglo
```

**Para qué sirve:** Necesitamos la dirección base del arreglo para calcular dónde está cada elemento.

---

### 3.2 Cargar desde memoria - lw (Load Word)

```asm
lw t3, 0(t2)
```

**Qué hace:** Lee la palabra (4 bytes) que está en la dirección `t2 + 0` y la guarda en `t3`.

**Desglose:**
- `lw` = Load Word (32 bits)
- El segundo operando (`0(t2)`) es: dirección = contenidos de t2 + offset
- Offset 0 = leer directamente de esa dirección

**En términos de C:**
```c
int t3 = *t2;  // cargar valor de memoria hacia registro
```

**Nota importante:** `lw` carga el VALOR, `la` carga la DIRECCIÓN.

---

### 3.3 Guardar en memoria - sw (Store Word)

```asm
sw t5, 0(s1)
```

**Qué hace:** Escribe el valor de `t5` en la dirección de memoria `s1 + 0`.

**En términos de C:**
```c
*contador_menores = t5;  // guardar valor de registro a memoria
```

---

### 3.4 Aritmética - addi (Add Immediate)

```asm
addi t5, t5, 1
```

**Qué hace:** Suma el valor inmediato (constante) `1` al registro `t5` y guarda en `t5`.

**Desglose:**
- `addi` = Add Immediate (sumar constante)
- Sintaxis: `addi rd, rs1, inmediato`
- Equivalente: `rd = rs1 + inmediato`

**En términos de C:**
```c
t5 = t5 + 1;  // incrementar contador
```

---

### 3.5 Multiplicar por 4 - slli (Shift Left Logical Immediate)

```asm
slli t2, t0, 2
```

**Qué hace:** Desplaza los bits del registro `t0` 2 posiciones a la izquierda y guarda en `t2`.

**¿Por qué multiplica por 4?**
- Mover un bit a la izquierda = multiplicar por 2
- Mover 2 bits = multiplicar por 2² = multiplicar por 4
- Cada `word` ocupa 4 bytes

**Tabla de equivalencias:**
| Desplazamiento | Multiplicación equivalentes |
|-----------------|------------------------------|
| `slli x, y, 1`  | `x = y × 2`                  |
| `slli x, y, 2`  | `x = y × 4`                  |
| `slli x, y, 3`  | `x = y × 8`                  |

**En términos de C:**
```c
int offset = i * 4;  // calcular offset en bytes para acceder arreglo
```

---

### 3.6 Suma de registros - add

```asm
add t2, s0, t2
```

**Qué hace:** Suma los valores de `s0` y `t2`, guarda en `t2`.

**Desglose:**
- `add` = suma de registros
- Sintaxis: `add rd, rs1, rs2`
- Equivalente: `rd = rs1 + rs2`

**En términos de C:**
```c
int direccion = base + offset;  // calcular dirección real
```

---

### 3.7 Comparaciones - bge (Branch Greater or Equal)

```asm
li t4, 18
bge t3, t4, elseif_1
```

**Qué hace:** Si `t3 >= t4`, salta a la etiqueta `elseif_1`.

**Desglose:**
- `bge` = Branch if Greater or Equal
- Sintaxis: `bge rs1, rs2, etiqueta`
- Significa: si `rs1 >= rs2`, ejecutar desde `etiqueta`

**Tabla de comparaciones:**
| Instr | Significado              | Condición para saltar |
|-------|--------------------------|----------------------|
| `beq` | Branch if Equal           | si `rs1 == rs2`       |
| `bne` | Branch if Not Equal     | si `rs1 != rs2`       |
| `blt` | Branch if Less Than     | si `rs1 < rs2`        |
| `bge` | Branch if Greater or Equal | si `rs1 >= rs2`   |
| `ble` | Branch if Less or Equal | si `rs1 <= rs2`      |
| `bgt` | Branch if Greater Than | si `rs1 > rs2`        |

---

### 3.8 Saltos - j (Jump)

```asm
j ciclo
```

**Qué hace:** Salta sin condición a la etiqueta `ciclo`.

**En términos de C:**
```c
goto ciclo;  // ir al inicio del ciclo
```

---

### 3.9 Cargar constantes - li (Load Immediate)

```asm
li t0, 0
li t1, 10
```

**Qué hace:** Carga un valor inmediato (constante) en el registro.

**Desglose:**
- `li` = Load Immediate (pseudoinstrucción)
- Se expande a `addi` con inmediato

**En términos de C:**
```c
int i = 0;
int N = 10;  // En C se calcula automáticamente con sizeof(edades)/sizeof(edades[0])
```

---

### 3.10 Terminar programa - ecall

```asm
li a7, 10
ecall
```

**Qué hace:** Llama al sistema operativo para terminar el programa.

**Desglose:**
- `a7` = 10 indica "exit"
- `ecall` ejecuta la llamada al sistema

**En términos de C:**
```c
return 0;  // terminar programa
```

---

## 4. Registro de Convenciones (Qué registro usar para qué)

### 4.1 Registros temporales (t0-t6)

| Registro | Uso típico                     |
|----------|------------------------------|
| t0       | Índice del ciclo (i)          |
| t1       | Valor N (tamaño)              |
| t2       | Cálculo de offset/dirección  |
| t3       | Edad actual                  |
| t4       | Valor para comparaciones      |
| t5       | Valor temporal para incrementar|

**Características:**
- Se pueden sobrescribir libremente
- No preservan su valor entre llamadas a funciones

---

### 4.2 Registros salvados (s0-s11)

| Registro | Uso típico                          |
|----------|-----------------------------------|
| s0       | Dirección base del arreglo        |
| s1       | Dirección contador menores        |
| s2       | Dirección contador adultos       |
| s3       | Dirección contador mayores        |

**Características:**
- Preservan su valor entre llamadas a funciones
- Si los usás en una función,debés guardarlos primero

---

### 4.3 Registros de argumentos (a0-a7)

| Registro | Uso                                      |
|----------|----------------------------------------|
| a0       | Argumento para syscall / valor de retorno|
| a7       | Código de syscall                      |

---

## 5. Flujo de Ejecución Completo

### 5.1 Pseudocódigo equivalente

```c
// INICIALIZACIÓN
s0 = &edades[0];           // la s0, edades
s1 = &contador_menores;    // la s1, contador_menores
s2 = &contador_adultos;    // la s2, contador_adultos
s3 = &contador_mayores;    // la s3, contador_mayores

*s1 = 0;                  // sw zero, 0(s1)
*s2 = 0;
*s3 = 0;

i = 0;                    // li t0, 0
N = 10;                   // li t1, 10 (en C: sizeof(edades)/sizeof(edades[0]))

// CICLO PRINCIPAL
while (i < N) {
    // Cargar edad actual
    offset = i * 4;                     // slli t2, t0, 2
    direccion = s0 + offset;              // add t2, s0, t2
    edad = *direccion;                  // lw t3, 0(t2)
    
    // if (edad < 18)
    if (edad < 18) {                     // bge t3, t4, ...
        (*s1)++;                        // lw/addi/sw
    }
    // else if (edad <= 65)
    else if (edad <= 65) {              // bgt t3, t4, ...
        (*s2)++;
    }
    // else
    else {
        (*s3)++;
    }
    
    i++;                                // addi t0, t0, 1
}
```

---

### 5.2 Traza de ejecución (primera iteración)

| Paso | Instr            | Estado de registros / memoria            |
|------|-----------------|----------------------------------|
| 1    | `la s0, edades`  | s0 = 0x10000000                  |
| 2    | `la s1, ...`   | s1 = 0x10000028                |
| 3    | `sw zero, 0(s1)` | contador_menores = 0            |
| 4    | `li t0, 0`     | t0 = 0 (i = 0)                 |
| 5    | `li t1, 10`    | t1 = 10 (N = número de elementos, se calcula en C con sizeof)
| 6    | `bge t0, t1, fin`| 0 >= 10? NO → continuar          |
| 7    | `slli t2, t0, 2`| t2 = 0 × 4 = 0 (offset)         |
| 8    | `add t2, s0, t2`| t2 = 0x10000000 + 0             |
| 9    | `lw t3, 0(t2)` | t3 = 12 (primera edad)           |
| 10   | `li t4, 18`    | t4 = 18                        |
| 11   | `bge t3, t4, elseif_1` | 12 >= 18? NO → no saltar    |
| 12   | `lw t5, 0(s1)` | t5 = 0                         |
| 13   | `addi t5, t5, 1`| t5 = 1                         |
| 14   | `sw t5, 0(s1)` | contador_menores = 1               |
| 15   | `addi t0, t0, 1`| t0 = 1 (i++)                    |
| 16   | `j ciclo`      | volver al inicio del ciclo       |

---

## 6. Dónde están los PROBLEMAS (Control Hazards)

### 6.1 El problema arquitectónico

Cada `if/else` genera un salto condicional. En un procesador segmentado (pipeline), esto causa **control hazards**:

```
┌─────────────────────────────────────────────────────────┐
│           PROBLEMA: BRANCH HAZARDS                       │
├─────────────────────────────────────────────────────────┤
│                                                          │
│ Sin pipeline: cada instrucción toma 1 ciclo            │
│ Con pipeline: las instrucciones se solapan              │
│                                                          │
│ Cuando hay un BRANCH (bge, bgt):                        │
│ ┌────┬────┬────┬────┬────┐                            │
│ │ IF │ ID │ EX │MEM │ WB │  ← branch instruction       │
│ └────┴────┴────┴────┴────┘                            │
│      ┌────┬────┬────┬────┐                            │
│      │NOP │NOP │NOP │NOP │  ← burbuja insertada     │
│      └────┴────┴────┴────┘                            │
│                                                          │
│ CPI real > 1.0 debido a los stalls (burbujas)            │
│                                                          │
└─────────────────────────────────────────────────────────┘
```

### 6.2 Dónde están los branch en nuestro código

```asm
# Este genera branch hazard:
bge t3, t4, elseif_1   # if (edad >= 18) → ir a else if

# Este también:
bgt t3, t4, else_final  # else if (edad > 65) → ir a else

# Y este para el ciclo:
bge t0, t1, fin_ciclo   # while (i < N) → salir si i >= N
```

**Total:** 3 branches por cada iteración × 10 iteraciones = 30 branches

### 6.3 Por qué esto es un problema

El pipeline de 5 etapas:
```
IF → ID → EX → MEM → WB
```

1. **IF** (Instruction Fetch): traer la instr de memoria
2. **ID** (Instruction Decode): decodificar la instr
3. **EX** (Execute): ejecutar la operación
4. **MEM** (Memory): acceder a memoria
5. **WB** (Write Back): escribir el resultado

Cuando el procesador ve un **branch**, no sabe si tomar el salto hasta que la comparación termina en EX. Mientras tanto, no sabe qué instr traer después → debe insertar **NOPs** (burbujas).

---

## 7. Preguntas de Sustentación y Respuestas

### P1: ¿Por qué multiplicás por 4 con slli?

> "Porque cada elemento del arreglo es un `.word` que ocupa 4 bytes. Si el índice es i=3, la posición en memoria es base + (3×4) = base + 12. El shift left de 2 bits es equivalente a multiplicar por 4."

---

### P2: ¿Por qué hay tres comparaciones (bge, bgt, j)?

> "Porque el algoritmo tiene tres casos: if (edad < 18), else if (edad <= 65), y else. Cada uno necesita una comparación y un posible salto."

---

### P3: ¿Qué es ecall y para qué sirve?

> "ecall es una llamada al sistema operativo (syscall). En RARS se usa para terminar el programa (código 10)."

---

### P4: ¿Dónde están los contadores en memoria?

> "Están definidos en la sección .data como `.word 0`. En el Data Segment de RARS aparecen en las direcciones 0x10000028 (menores), 0x1000002C (adultos), y 0x10000030 (mayores)."

---

### P5: ¿Qué registros preservan su valor?

> "Los registros salvados s0-s11 preservan su valor entre llamadas. Por eso los usamos para guardar direcciones base. Los temporales t0-t6 se pueden sobrescribir."

---

### P6: ¿Qué diferencia hay entre lw y la?

> "`la` carga una dirección (puntero), `lw` carga el contenido de esa dirección. Es como (&edades) vs (*edades) en C."

---

### P7: ¿Cuántas instrucciones toma el ciclo?

> "El ciclo tiene aproximadamente 15-18 instrucciones por iteración. Con 10 elementos son ~170 instrucciones totales."

---

### P8: ¿Qué son los control hazards?

> "Son riesgos causados por los saltos condicionales. Cuando el procesador encuentra un branch (bge, bgt), no sabe si tomar el salto hasta que la comparación termina en la etapa EX. Mientras decide, debe insertar NOPs (burbujas) lo que aumenta el CPI."

---

### P9: ¿Qué hace la instrucción 'add'?

> "La instrucción `add` suma dos registros y guarda el resultado. A diferencia de `addi` que suma una constante, `add` suma el valor de dos registros: `add t2, s0, t2` significa t2 = s0 + t2."

---

## 8. Resumen Visual para la Sustentación

```
┌────────────────────────────────────────────────────────────┐
│                 ARQUITECTURA DEL PROGRAMA                  │
├────────────────────────────────────────────────────────────┤
│                                                            │
│  .data (DATOS)          .text (CÓDIGO)                    │
│  ┌─────────────┐        ┌──────────────────────┐           │
│  │ edades:    │        │ main:               │           │
│  │ [10 words]│        │   la s0, edades     │ ← cargar │
│  │          │        │   la s1, contr_men   │   dir    │
│  │ contr_:  │        │ ciclo:              │           │
│  │ 3 words  │        │   lw t3, 0(t2)     │ ← cargar │
│  │          │        │   bge t3, t4, ...   │ ← branch │
│  │          │        │   addi t5, t5, 1    │ ← aritm │
│  │          │        │   sw t5, 0(s1)       │ ← store  │
│  └─────────────┘        └──────────────────────┘           │
│       MEMORIA               PROCESADOR                       │
│                                                            │
└────────────────────────────────────────────────────────────┘

DATO CLAVE: El algoritmo tiene 3 branches por iteración
→ 10 iteraciones × 3 = 30 branch instructions
→ Cada branch mal predicho = 1-2 ciclos perdidos
→ CPI real > 1.0
```

---

## 9. Checklist de Sustentación

Marca cada uno cuando lo puedas explicar:

- [ ] Explicar qué hace `.data` y para qué sirve
- [ ] Explicar qué hace `.text` y para qué sirve  
- [ ] Explicar qué es `.word` y ejemplos de uso
- [ ] Explicar para qué sirve `la` (load address)
- [ ] Explicar para qué sirve `lw` (load word)
- [ ] Explicar para qué sirve `sw` (store word)
- [ ] Explicar por qué `slli` multiplica por 4
- [ ] Explicar las comparaciones: `bge`, `bgt`, `beq`, `bne`
- [ ] Explicar el flujo del ciclo completo
- [ ] Explicar dónde están los contadores en memoria
- [ ] Explicar qué son los control hazards
- [ ] Explicar por qué el CPI real > 1.0
- [ ] Explicar qué hace `ecall` (exit)

---

## 10. Glosario Rápido

| Término | Significado |
|---------|------------|
| Ensamblador | Programa que traduce código assembly a código máquina |
| Registro | Memoria muy rápida dentro del procesador |
| Pipeline | Técnica para ejecutar varias instrucciones simultaneamente |
| Hazard | Conflicto que impide ejecutar instrucciones en paralelo |
| Branch | Salto condicional (if/else) |
| Stall/Burbuja | Ciclo perdido esperando que algo termine |
| CPI | Ciclos Por Instruccón |
| Offset | Desplazamiento desde una dirección base |

---

## 11. Más Recursos

### Documentación oficial:
- RISC-Vspec: https://riscv.org/technical/specifications/
- RARS: https://github.com/TheThirdMovement/rars

### Para practicar:
- Intentar dibujar el Data Segment completo
- Hacer la traza a mano de las primeras 2 iteraciones
- Explicar el código en voz alta como si fuera la sustentación

--- 

# Hazard de Control en un Pipeline de 5 Etapas (RISC-V)

## 📌 Descripción del problema

En un procesador RISC-V con pipeline de 5 etapas (IF, ID, EX, MEM, WB), se presenta un **hazard de control** cuando el flujo de ejecución del programa cambia debido a una instrucción de salto.

En este caso específico, la instrucción problemática es:

```
jal x0, 40
```

Esta instrucción realiza un **salto incondicional**, modificando el valor del PC (Program Counter) hacia una nueva dirección de memoria.

---

## ⚙️ Funcionamiento del pipeline

El pipeline tiene las siguientes etapas:

1. **IF (Instruction Fetch)**: Se obtiene la instrucción desde memoria.
2. **ID (Instruction Decode)**: Se decodifica la instrucción y se leen registros.
3. **EX (Execute)**: Se ejecuta la operación (ALU o cálculo de salto).
4. **MEM (Memory)**: Acceso a memoria (si aplica).
5. **WB (Write Back)**: Escritura en registros.

---

## 🚨 ¿Por qué ocurre el hazard?

El problema surge porque:

- El procesador **no sabe inmediatamente** que debe hacer un salto.
- Mientras el `jal` avanza en el pipeline, el procesador sigue trayendo instrucciones secuenciales.

### Secuencia real:

1. Se ejecuta una instrucción previa (por ejemplo `addi x5, x5, 1`).
2. El pipeline continúa trayendo instrucciones siguientes de forma secuencial.
3. La instrucción `jal` entra al pipeline.
4. El salto **se resuelve en la etapa EX**.
5. En ese momento, el procesador detecta que:
   - Las instrucciones que ya están en IF e ID **no son válidas**.

---

## 🔥 Consecuencia: instrucciones incorrectas en el pipeline

Antes de resolver el salto, el pipeline ya cargó instrucciones que:

- No deberían ejecutarse.
- Pertenecen a la ruta incorrecta del programa.

---

## 🧹 Solución: Flush del pipeline

Para corregir esto, el procesador realiza un **flush (limpieza del pipeline)**:

- Se eliminan las instrucciones incorrectas.
- Se reemplazan por instrucciones vacías (**NOP - No Operation**).

En el diagrama se observa:

```
nop (flush)
nop (flush)
```

---

## 📉 ¿Por qué aparecen 2 NOPs?

Esto depende de en qué etapa se resuelve el salto.

En este caso:

- El `jal` se resuelve en **EX**.
- Para ese momento:
  - Hay una instrucción en **IF**
  - Otra en **ID**

Ambas deben eliminarse.

Por eso:

```
Número de NOPs = número de etapas anteriores a EX = 2
```

---

## 📊 Impacto en el rendimiento

El flush introduce:

- **Burbujas en el pipeline**
- Pérdida de ciclos de reloj
- Disminución del rendimiento

Cada NOP representa un ciclo en el que el procesador no realiza trabajo útil.

---

## 🧠 Tipo de hazard

Este problema se clasifica como:

> **Control Hazard (hazard de control)**

Porque está relacionado con el cambio del flujo de ejecución del programa.

---

## 🚀 Técnicas para mitigar este problema

En arquitecturas más avanzadas, se utilizan:

- **Predicción de saltos (Branch Prediction)**  
  El procesador intenta adivinar si habrá salto.

- **Resolución temprana del salto**  
  Mover la lógica de decisión a etapas más tempranas.

- **Delay Slots**  
  Ejecutar instrucciones útiles mientras se resuelve el salto.

---

## 📌 Resumen final

- El hazard ocurre por una instrucción de salto (`jal`).
- El pipeline continúa ejecutando instrucciones incorrectas.
- Al resolverse el salto, estas instrucciones se eliminan (flush).
- Se insertan NOPs para limpiar el pipeline.
- En este caso aparecen 2 NOPs porque el salto se resuelve en EX.
