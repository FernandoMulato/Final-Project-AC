/**
 * HISTOGRAMA - Clasificador de Edades  (VERSIÓN LENTA)
 * =====================================================
 * Clasifica edades en 3 categorías:
 *   Menores : < 18 años
 *   Adultos : 18–65 años
 *   Mayores : > 65 años
 *
 * TÉCNICAS QUE DEGRADAN EL CACHÉ INTENCIONALMENTE
 * ─────────────────────────────────────────────────
 *
 *  1. ACCESO ALEATORIO (Fisher-Yates shuffle sobre índices)
 *     En vez de recorrer edades[0], edades[1], edades[2]...
 *     se accede en orden random: edades[71432], edades[3], edades[99001]
 *     → Cada acceso carga una línea de caché de 64 bytes (16 ints)
 *       pero solo usa 1 de esos 16 valores. Eficiencia: 6 %.
 *     → El prefetcher de la CPU no puede predecir el siguiente salto.
 *
 *  2. DOBLE LECTURA EN ZONA DE MEMORIA SEPARADA (edades_copia[])
 *     Para cada elemento se hace una segunda lectura en otro array
 *     con la misma posición aleatoria → fuerza otro cache miss en
 *     una dirección de heap distinta.
 *
 *  3. RAMAS ANIDADAS (if dentro de if)
 *     Confirma cada categoría con una segunda comparación sobre
 *     edades_copia[], forzando la segunda lectura aleatoria y
 *     generando más branch mispredictions que el flujo simple.
 */

#include <stdio.h>
#include <stdlib.h> /* malloc, free, rand, srand */
#include <time.h>   /* time – semilla aleatoria  */

#define N 100000     /* Número de elementos             */
#define EDAD_MAX 100 /* Rango de edades generadas: 0–99 */

/* ── Baraja el arreglo de índices con Fisher-Yates ─────────────────── */
static void mezclar_indices(int *arr, int n)
{
  for (int i = n - 1; i > 0; i--)
  {
    int j = rand() % (i + 1);
    int tmp = arr[i];
    arr[i] = arr[j];
    arr[j] = tmp;
  }
}

/* ── Imprime una barra del histograma en consola ───────────────────── */
static void imprimir_barra(const char *etiqueta, int conteo, int total)
{
  double porcentaje = (conteo * 100.0) / total;
  int bloques = (int)(porcentaje / 2); /* escala: 1 bloque = 2 % */

  printf("  %-9s |", etiqueta);
  for (int i = 0; i < 50; i++)
    putchar(i < bloques ? '#' : ' ');
  printf("| %6d  (%5.1f %%)\n", conteo, porcentaje);
}

/* ── Programa principal ─────────────────────────────────────────────── */
int main(void)
{
  /* 1. ASIGNACIÓN DINÁMICA – tres bloques en zonas distintas del heap */
  int *edades = (int *)malloc(N * sizeof(int));
  int *edades_copia = (int *)malloc(N * sizeof(int)); /* segunda zona lejana */
  int *indices = (int *)malloc(N * sizeof(int));      /* orden de acceso     */

  if (edades == NULL || edades_copia == NULL || indices == NULL)
  {
    fprintf(stderr, "Error: no se pudo asignar memoria.\n");
    return EXIT_FAILURE;
  }

  /* 2. Generar edades y construir índices 0..N-1 */
  srand(42);
  for (int i = 0; i < N; i++)
  {
    edades[i] = rand() % EDAD_MAX;
    edades_copia[i] = edades[i]; /* mismos datos, dirección distinta */
    indices[i] = i;
  }

  /* 3. Barajar índices → orden de acceso completamente aleatorio */
  mezclar_indices(indices, N);

  /* 4. Clasificar – acceso en orden ALEATORIO + doble lectura */
  int contador_menores = 0;
  int contador_adultos = 0;
  int contador_mayores = 0;

  for (int i = 0; i < N; i++)
  {
    int idx = indices[i];   /* salto impredecible             */
    int edad = edades[idx]; /* 1er cache miss probable        */

    /* Rama anidada: confirma con edades_copia en zona lejana del heap */
    if (edad < 18)
    {
      if (edades_copia[idx] < 18) /* 2do cache miss sobre otro bloque */
        contador_menores++;
    }
    else if (edad <= 65)
    {
      if (edades_copia[idx] <= 65)
        contador_adultos++;
    }
    else
    {
      if (edades_copia[idx] > 65)
        contador_mayores++;
    }
  }

  /* 5. Mostrar resultados */
  printf("\n HISTOGRAMA DE EDADES  (N = %d elementos)\n\n", N);

  imprimir_barra("Menores", contador_menores, N);
  imprimir_barra("Adultos", contador_adultos, N);
  imprimir_barra("Mayores", contador_mayores, N);

  printf("\n  Total: %d  |  Menores: %d  |  Adultos: %d  |  Mayores: %d\n\n",
        contador_menores + contador_adultos + contador_mayores,
        contador_menores, contador_adultos, contador_mayores);

  /* 6. LIBERAR los tres bloques */
  free(edades);
  free(edades_copia);
  free(indices);
  return EXIT_SUCCESS;
}