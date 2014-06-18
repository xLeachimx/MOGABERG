/*FILE: Genetics.cpp
 *Author: Michael Huelsman
 *Abstract:
 *   Implementation of algortihms in Genetics.hpp 
 */
#include "Genetics.hpp"
#include <cstdlib>
using std::rand;

//needed subfuntions
void randomize(Object gen[], int size, int cycles);
void swap(Object &one, Object &two);

Children crossover(Parents p){
  int crossed = 0;
  bool father = true;
  voxel first[NUM_VOX];
  voxel second[NUM_VOX];
  for(int i = 0;i < NUM_VOX;i++){
    if(crossed < CROSS_POINTS && rand()%100 < CROSS_PER){
      father = !father;
      crossed++;
    }
    first[i] = father?p.father.getVoxels()[i]:p.mother.getVoxels()[i];
    second[i] = father?p.mother.getVoxels()[i]:p.father.getVoxels()[i];
  }
  mutate(first,NUM_VOX);
  mutate(second,NUM_VOX);
  Children result;
  result.first = Object(first);
  result.second = Object(second);
  return result;
}

void mutate(voxel v[], int size){
  int *mut = new int;
  for(int i = 0;i < size;i++){
    if(rand()%1000 < MUTATION_PER){
      *mut = rand();
      char *mod = ((char *)mut);
      v[i].x = mod[0];
      v[i].y = mod[1];
      v[i].z = mod[2];
      v[i].size = mod[3];
      v[i].size = (v[i].size<0)?(-v[i].size):(v[i].size);//sizes should not be negative
      v[i].size = (v[i].size==0)?1:v[i].size;//Minimal size is 1
    }
  }
  delete mut;
}

Parents selection(Object gen[], int size){
  randomize(gen,size,size*2);//jumble the generation up
  genSort(gen,SELECTION_SIZE);//sort out a SELECTION_SIZE group
  Parents result;
  result.father = gen[0];//take best members of random grouping
  result.mother = gen[1];
  return result;
}

//generic quicksort implementation sort to DSC
void genSort(Object *gen, int size){
  if(size <= 1)return;
  if(size == 2){
    if(gen[0] < gen[1]){
      swap(gen[0],gen[1]);
    }
    return;
  }
  Object *pivot = &gen[0];
  int left = 1;
  int right = size - 1;
  while(left <= right){
    if(gen[left] < *pivot){
      if(gen[right] >= *pivot){
	swap(gen[left++],gen[right]);
      }
      right--;
    }
    else{
      left++;
    }
  }
  swap(gen[0],gen[right]);
  genSort(gen,right);
  genSort(&gen[left],size-left);
}

void randomize(Object gen[], int size, int cycles){
  for(int i = 0;i < cycles;i++){
    swap(gen[rand()%size],gen[rand()%size]);
  }
}


void swap(Object &one, Object &two){
  Object temp = one;
  one = two;
  two = temp;
}
