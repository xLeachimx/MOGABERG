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
  char *first = new char[p.mother.getEncodingSize()];
  char *second = new char[p.father.getEncodingSize()];
  int minSize = (p.father.getEncodingSize() < p.mother.getEncodingSize())?p.father.getEncodingSize():p.mother.getEncodingSize();
  int cross = rand()%(minSize/4);
  cross = cross*4;
  for(int i = 0;i < cross;i++){
    first[i] = p.father.getEncoding()[i];
    second[i] = p.mother.getEncoding()[i];
  }
  for(int i = cross;i < p.mother.getEncodingSize();i++){
    first[i] = p.mother.getEncoding()[i];
  }
  for(int i = cross;i < p.father.getEncodingSize();i++){
    second[i] = p.father.getEncoding()[i];
  }
  int firstSize = mutate(first,p.mother.getEncodingSize());
  int secondSize = mutate(second,p.father.getEncodingSize());
  Children result;
  result.first = Object(first,firstSize);
  result.second = Object(second,secondSize);
  delete[] first;
  delete[] second;
  return result;
}

int mutate(char *&v, int size){
  for(int i = 0;i < size;i++){
    if(rand()%100 < MUTATION_PER){
      v[i] = (char)rand();
    }
  }
  if(rand()%200 < MUTATION_PER){
    if(rand()%2 == 0){
      if(size > 40){
	//delete a voxel
	char *temp = new char[size-4];
	int deletion = rand()%(size-4);
	int insert = 0;
	for(int i = 0;i < size;i++){
	  if(i>=deletion && i < deletion+4)continue;
	  temp[insert++] = v[i];
	}
	char *sub = v;
	delete[] sub;
	v = temp;
	size -= 4;
      }
    }
    else{
      if(size < 400){
	//add a voxel
	char *temp = new char[size+4];
	for(int i = 0;i < size;i++){
	  temp[i] = v[i];
	}
	for(int i = 0;i < 4;i++){
	  temp[size+i] = (char)rand();
	}
	char *sub = v;
	delete[] sub;
	v = temp;
	size += 4;
      }
    }
  }
  return size;
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
