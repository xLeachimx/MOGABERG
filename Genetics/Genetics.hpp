/*FILE: Genetics.hpp
 *Author: Michael Huelsman
 *Abstract:
 *   Genetic operators of selection, crossover, and mutation
 */

#ifndef GENETICS_HPP
#define GENETICS_HPP

#include "Object.hpp"

const int MUTATION_PER = 1;
const int SELECTION_SIZE = 10;


struct Children{
  Object first;
  Object second;
};

struct Parents{
  Object father;
  Object mother;
};


Children crossover(Parents p);
int mutate(char *&v, int size);
Parents selection(Object gen[], int size);
void genSort(Object *gen, int size);

#endif
