/*FILE: main.cpp
 *Author:Michael Huelsman
 *Abstract:
 *   Main program of the STGA
 */
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
using std::cout;
using std::endl;
using std::ofstream;
using std::srand;
using std::time;

#include "Object.hpp"
#include "Genetics.hpp"

const int NUM_GEN = 500;
const int GEN_SIZE = 100;
const int ELITISM = 2;

int main(int argc, char **argv){
  if(argc != 2)return 0;
  //setup
  unsigned int seed = time(NULL);
  srand(seed);
  Object generation[GEN_SIZE];
  voxel cloud[NUM_VOX];
  //setup file for record keeping
  ofstream fout;
  fout.open(argv[1]);
  if(!fout){
    cout << "Problem opening file!" <<endl;
    return 0;
  }
  //primordial soup generator
  for(int i = 0;i < GEN_SIZE;i++){
    int generator = rand();
    for(int j = 0;j < NUM_VOX;j++){ 
      int generator = rand();
      char *mod = (char *)(&(generator));
      cloud[j].x = mod[0];
      cloud[j].y = mod[1];
      cloud[j].z = mod[2];
      cloud[j].size = mod[3];
      cloud[j].size = (cloud[j].size<0)?(-cloud[j].size):(cloud[j].size);//sizes should not be negative
      cloud[j].size = (cloud[j].size==0)?1:cloud[j].size;//Minimal size is 1
    }
    generation[i] = Object(cloud);
  }

  //running the GA
  Object nextGen[GEN_SIZE];
  for(int i = 0;i < NUM_GEN;i++){
    cout << i <<endl;
    fout << i << endl;
    for(int j = 0;j < GEN_SIZE;j++){
      generation[j].calcQuality();
    }
    for(int j = 0;j < GEN_SIZE;j++){
      generation[j].calcFitness(generation, GEN_SIZE, j);
    }
    genSort(generation,GEN_SIZE);//sort according to dominance
    //record keeping
    for(int j = 0;j < GEN_SIZE;j++){
      generation[j].toCSV(fout);
    }
    //elitism
    for(int j = 0;j < ELITISM;j++){
      nextGen[j] = generation[j];
    }
    //reproduction
    Children product;
    for(int j = 0;j < ((GEN_SIZE-ELITISM)/2);j++){
      product = crossover(selection(generation,GEN_SIZE));
      nextGen[ELITISM+(2*j)] = product.first;
      nextGen[ELITISM+(2*j)] = product.second;
    }
    //copying generation over
    for(int j = 0;j < GEN_SIZE;j++){
      generation[j] = nextGen[j];
    }
  }
  //post-run records
  fout << NUM_GEN <<endl;
  for(int j = 0;j < GEN_SIZE;j++){
    generation[j].calcQuality();
  }
  for(int j = 0;j < GEN_SIZE;j++){
    generation[j].calcFitness(generation, GEN_SIZE, j);
  }
  genSort(generation,GEN_SIZE);//sort according to dominance
  //record keeping
  for(int j = 0;j < GEN_SIZE;j++){
    generation[j].toCSV(fout);
  }
  fout.close();
  return 0;
}
