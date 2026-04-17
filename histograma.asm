# ============================================================================
# HISTOGRAMA - RISC-V Assembly (RARS)
# ============================================================================
# Algoritmo: Clasificar edades en 3 categorías
#   Menores: < 18
#   Adultos: 18-65
#   Mayores: > 65
# ============================================================================

.data
edades:
    .word 12, 25, 70, 17, 45, 8, 66, 30, 15, 99

contador_menores:
    .word 0
contador_adultos:
    .word 0
contador_mayores:
    .word 0

.text
.globl main

main:
    # ----------------------------------------------------------------
    # INICIALIZACIÓN
    # ----------------------------------------------------------------
    la s0, edades           # s0 -> arreglo edades
    la s1, contador_menores # s1 -> contador menores
    la s2, contador_adultos  # s2 -> contador adultos
    la s3, contador_mayores # s3 -> contador mayores
    
    # Inicializar contadores = 0
    sw zero, 0(s1)
    sw zero, 0(s2)
    sw zero, 0(s3)
    
    # Índice i = 0
    li t0, 0
    li t1, 10              # N = 10
    
ciclo:
    # Verificar condición: if (i >= N) salir
    bge t0, t1, fin_ciclo
    
    # Cargar edad actual: edad = edades[i]
    slli t2, t0, 2          # offset = i * 4
    add t2, s0, t2          # dirección = base + offset
    lw t3, 0(t2)           # t3 = edad actual
    
    # ----------------------------------------------------------------
    # COMPARACIÓN 1: if (edad < 18) → menores++
    # ----------------------------------------------------------------
    li t4, 18
    bge t3, t4, elseif_1   # if (edad >= 18) ir a else if
    
    # edad < 18 → menores++
    lw t5, 0(s1)
    addi t5, t5, 1
    sw t5, 0(s1)
    j continuar
    
    # ----------------------------------------------------------------
    # COMPARACIÓN 2: else if (edad <= 65) → adultos++
    # ----------------------------------------------------------------
elseif_1:
    li t4, 65
    bgt t3, t4, else_final   # if (edad > 65) ir a else
    
    # 18 <= edad <= 65 → adultos++
    lw t5, 0(s2)
    addi t5, t5, 1
    sw t5, 0(s2)
    j continuar
    
    # ----------------------------------------------------------------
    # ELSE: else (edad > 65) → mayores++
    # ----------------------------------------------------------------
else_final:
    lw t5, 0(s3)
    addi t5, t5, 1
    sw t5, 0(s3)
    
continuar:
    addi t0, t0, 1          # i++
    j ciclo
    
fin_ciclo:
    # Fin del programa
    li a7, 10
    ecall