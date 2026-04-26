/**
 * HISTOGRAMA - Clasificador de Edades
 * ====================================
 * Clasifica edades en 3 categorías:
 *   Menores : < 18 años
 *   Adultos : 18–65 años
 *   Mayores : > 65 años
 *
 * ESCALADO A 100.000 ELEMENTOS
 * ─────────────────────────────
 * Por qué el arreglo original de 10 elementos NO puede simplemente
 * crecer a 100.000 como variable local dentro de main():
 *
 *   int edades[100000];  // ← PELIGRO: ~400 KB en el stack
 *
 * El stack de un proceso suele tener solo 1–8 MB.
 * Un arreglo de 100.000 ints ocupa 100.000 × 4 = 400.000 bytes (~391 KB).
 * Aunque entra en teoría, junto con otros datos del stack puede provocar
 * Segmentation fault (stack overflow) dependiendo del sistema/compilador.
 *
 * SOLUCIÓN SEGURA: asignación dinámica con malloc() en el heap,
 * que puede manejar gigabytes sin riesgo de desbordamiento de pila.
 */

#include <stdio.h>
#include <stdlib.h> /* malloc, free, rand, srand */
#include <time.h>   /* time – semilla aleatoria  */

#define N 100000     /* Número de elementos            */
#define EDAD_MAX 100 /* Rango de edades generadas: 0–99 */

    /* ── Imprime una barra del histograma en consola ───────────────────── */
    static void imprimir_barra(const char *etiqueta, int conteo, int total)
{
  double porcentaje = (conteo * 100.0) / total;
  int bloques = (int)(porcentaje / 2); /* escala: 1 bloque = 2 % */

  printf("  %-9s │", etiqueta);
  for (int i = 0; i < 50; i++)
    putchar(i < bloques ? '#' : ' ');
  printf("│ %6d  (%5.1f %%)\n", conteo, porcentaje);
}

/* ── Programa principal ─────────────────────────────────────────────── */
int main(void)
{
  /* 1. ASIGNACIÓN DINÁMICA en el heap – evita Segmentation fault */
  int *edades = (int *)malloc(N * sizeof(int));
  if (edades == NULL)
  {
    fprintf(stderr, "Error: no se pudo asignar memoria para %d elementos.\n", N);
    return EXIT_FAILURE;
  }

  /* 2. Generar edades aleatorias entre 0 y EDAD_MAX-1 */
  srand((unsigned int)time(NULL));
  for (int i = 0; i < N; i++)
    edades[i] = rand() % EDAD_MAX;

  /* 3. Clasificar */
  int contador_menores = 0;
  int contador_adultos = 0;
  int contador_mayores = 0;

  for (int i = 0; i < N; i++)
  {
    int edad = edades[i];
    if (edad < 18)
      contador_menores++;
    else if (edad <= 65)
      contador_adultos++;
    else
      contador_mayores++;
  }

  /* 4. Mostrar resultados */
  printf("\n╔══════════════════════════════════════════════════════════════╗\n");
  printf("║          HISTOGRAMA DE EDADES  (N = %6d elementos)       ║\n", N);
  printf("╠══════════════════════════════════════════════════════════════╣\n");
  printf("  Categoría │%-50s│  Count   (  %%  )\n", "  Distribución (cada # ≈ 2 %)         ");
  printf("  ──────────┼");
  for (int i = 0; i < 50; i++)
    putchar('─');
  printf("┼────────────────\n");

  imprimir_barra("Menores", contador_menores, N);
  imprimir_barra("Adultos", contador_adultos, N);
  imprimir_barra("Mayores", contador_mayores, N);

  printf("  ──────────┴");
  for (int i = 0; i < 50; i++)
    putchar('─');
  printf("┴────────────────\n");
  printf("  Total: %d  │  Menores: %d  │  Adultos: %d  │  Mayores: %d\n\n",
        contador_menores + contador_adultos + contador_mayores,
        contador_menores, contador_adultos, contador_mayores);

  /* 5. LIBERAR la memoria asignada con malloc */
  free(edades);
  return EXIT_SUCCESS;
}