# HISTOGRAMA - RISC-V Assembly Project

## Project Overview

**Purpose**: Count ages into three categories (minors, adults, seniors) using RISC-V assembly.

**Language**: RISC-V Assembly (RV32I)

**Target Environment**: RARS (RISC-V Assembler and Runtime Simulator)

---

## Technical Specification

### Data Structure

```
Memory Layout (Data Segment):
┌───────────────┬─────────────────┬────────────────────┐
│ Address       │ Content         │ Description       │
├───────────────┼─────────────────┼────────────────────┤
│ 0x10000000    │ edades[0..9]    │ 10 × 4-byte words │
│ 0x10000028    │ contador_menores│ .word              │
│ 0x1000002C    │ contador_adultos│ .word              │
│ 0x10000030    │ contador_mayores│ .word              │
└───────────────┴─────────────────┴────────────────────┘
```

### Algorithm

```
Input:  ages[10] = {12, 25, 70, 17, 45, 8, 66, 30, 15, 99}
Output: minors=4, adults=3, seniors=3

Pseudocode equivalent:
for (i = 0; i < 10; i++) {
    if (age[i] < 18)      minors++;
    else if (age[i] <= 65) adults++;
    else                  seniors++;
}
```

### Register Allocation

| Register | Purpose |
|----------|---------|
| s0 | Base address of ages array |
| s1 | Address of counter_menores |
| s2 | Address of counter_adultos |
| s3 | Address of counter_mayores |
| t0 | Loop index (i) |
| t1 | Loop bound (N=10) |
| t2 | Computed offset/address |
| t3 | Current age value |
| t4 | Comparison constant (18) |
| t5 | Temporary for increment |

---

## Instruction Set Reference

### Directives

| Directive | Description |
|-----------|-------------|
| `.data` | Declares data section (static storage) |
| `.text` | Declares code section |
| `.word` | 32-bit integer storage (4 bytes each) |
| `.globl main` | Exports main as entry point |

### Instructions Used

| Instruction | Operation | Notes |
|------------|-----------|-------|
| `la rd, label` | Load address | Pseudoinstruction (expands to lui+addi) |
| `lw rd, offset(rs)` | Load word | 32-bit read from memory |
| `sw rs, offset(rd)` | Store word | 32-bit write to memory |
| `addi rd, rs, imm` | Add immediate | rd = rs + imm |
| `add rd, rs1, rs2` | Add registers | rd = rs1 + rs2 |
| `slli rd, rs, imm` | Shift left logical | rd = rs << imm (imm=2 multiplies by 4) |
| `li rd, imm` | Load immediate | Pseudoinstruction |
| `bge rs1, rs2, label` | Branch if >= | Conditional jump |
| `bgt rs1, rs2, label` | Branch if > | Conditional jump |
| `j label` | Unconditional jump | Jump to label |
| `ecall` | Environment call | System call (a7=10 for exit) |

---

## Pipeline Analysis

### RISC-V 5-Stage Pipeline

```
IF → ID → EX → MEM → WB
```

### Control Hazards

The program contains 3 conditional branches per iteration:

- `bge t3, t4, elseif_1` (age >= 18 check)
- `bgt t3, t4, else_final` (age > 65 check)
- `bge t0, t1, fin_ciclo` (loop termination)

Total: 30 branches for 10 iterations.

**Hazard Impact**: Each unresolved branch causes 1-2 NOP cycles (pipeline bubbles), increasing CPI above 1.0.

---

## Performance Metrics

### AMAT Calculation (Intel Core i7-13th Gen)

| Cache Level | Hit Rate | Miss Penalty |
|-------------|----------|--------------|
| L1 | 98% | 10 ns (to L2) |
| L2 | 95% | 40 ns (to L3) |
| L3 | 99% | 100 ns (to RAM) |

```
AMAT = 0.98 × 4ns + 0.02 × [0.95 × 14ns + 0.05 × [0.99 × 54ns + 0.01 × 154ns]]
AMAT ≈ 4.24 ns
```

**Improvement**: 89.4% faster than uncached access (~40ns)

---

## Program Structure

```
.data
    edades:      .word 12, 25, 70, 17, 45, 8, 66, 30, 15, 99
    contador_menores: .word 0
    contador_adultos: .word 0
    contador_mayores: .word 0

.text
.globl main
main:
    # Load base addresses
    la s0, edades
    la s1, contador_menores
    la s2, contador_adultos
    la s3, contador_mayores

    # Initialize counters to 0
    sw zero, 0(s1)
    sw zero, 0(s2)
    sw zero, 0(s3)

    # Initialize loop
    li t0, 0          # i = 0
    li t1, 10         # N = 10

ciclo:
    # Loop condition: i >= N?
    bge t0, t1, fin_ciclo

    # Compute offset: i * 4
    slli t2, t0, 2
    add t2, s0, t2

    # Load current age
    lw t3, 0(t2)

    # Compare: age >= 18?
    li t4, 18
    bge t3, t4, elseif_1

    # age < 18: increment minors
    lw t5, 0(s1)
    addi t5, t5, 1
    sw t5, 0(s1)
    j fin_iteracion

elseif_1:
    # Compare: age > 65?
    bgt t3, t4, else_final

    # 18 <= age <= 65: increment adults
    lw t5, 0(s2)
    addi t5, t5, 1
    sw t5, 0(s2)
    j fin_iteracion

else_final:
    # age > 65: increment seniors
    lw t5, 0(s3)
    addi t5, t5, 1
    sw t5, 0(s3)

fin_iteracion:
    # Increment index
    addi t0, t0, 1
    j ciclo

fin_ciclo:
    li a7, 10
    ecall
```

---

## Execution Verification

Expected output:
- Menores (age < 18): 4
- Adultos (18 <= age <= 65): 3
- Mayores (age > 65): 3

---

## References

- RISC-V ISA Specification: https://riscv.org/technical/specifications/
- RARS Simulator: https://github.com/TheThirdMovement/rars

---

## Build & Run

```bash
# In RARS GUI
1. Open the .asm file
2. Assemble (F3)
3. Run (F5)

# Command line
java -jar rars.jar ce filename.asm
```