/**
 * HISTOGRAMA - Clasificador de Edades
 * ==================================
 * Clasifica idades en 3 categorías:
 * - Menores: < 18 años
 * - Adultos: 18-65 años
 * - Mayores: > 65 años
 */

#include <stdio.h>

// Arreglo de edades
int edades[] = {12, 25, 70, 17, 45, 8, 66, 30, 15, 99};
#define N (sizeof(edades) / sizeof(edades[0]))

int main()
{
  // Contadores para cada categoría
  int contador_menores = 0;
  int contador_adultos = 0;
  int contador_mayores = 0;

  // Procesar cada edad
  for (int i = 0; i < N; i++)
  {
    int edad = edades[i];

    if (edad < 18)
    {
      contador_menores++;
    }
    else if (edad <= 65)
    {
      contador_adultos++;
    }
    else
    {
      contador_mayores++;
    }
  }

  // Imprimir resultados
  printf("=== RESULTADOS ===\n");
  printf("Menores (< 18): %d\n", contador_menores);
  printf("Adultos (18-65): %d\n", contador_adultos);
  printf("Mayores (> 65): %d\n", contador_mayores);
  printf("Total: %d\n", contador_menores + contador_adultos + contador_mayores);

  return 0;
}