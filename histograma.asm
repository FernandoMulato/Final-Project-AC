# ============================================================================
# HISTOGRAMA - RISC-V Assembly (RARS)
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
    la s0, edades
    la s1, contador_menores
    la s2, contador_adultos
    la s3, contador_mayores
    
    sw zero, 0(s1)
    sw zero, 0(s2)
    sw zero, 0(s3)
    
    li t0, 0
    li t1, 10

ciclo:
    bge t0, t1, fin_ciclo
    
    slli t2, t0, 2
    add  t2, s0, t2
    lw   t3, 0(t2)
    
    # if (edad < 18) → menores++
    li t4, 18
    bge t3, t4, elseif_1
    
    lw t5, 0(s1)
    addi t5, t5, 1
    sw t5, 0(s1)
    j continuar

elseif_1:
    # if (edad >= 66) → mayores++   (equivalente a edad > 65)
    li t4, 66
    bge t3, t4, else_final
    
    # 18 <= edad <= 65 → adultos++
    lw t5, 0(s2)
    addi t5, t5, 1
    sw t5, 0(s2)
    j continuar

else_final:
    # edad > 65 → mayores++
    lw t5, 0(s3)
    addi t5, t5, 1
    sw t5, 0(s3)

continuar:
    addi t0, t0, 1
    j ciclo

fin_ciclo:
    li a7, 10
    ecall