/*FILE: main.cpp
 *Author:Michael Huelsman
 *Abstract:
 *   Main program of the STGA
 *Notes:
 *  The commandline needs 2 arguements:
 *    1)Filename for the csv output of the program
 *    2)Filename for the scad output of the program
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

const int NUM_GEN = 1000;
const int GEN_SIZE = 100;
const int ELITISM = 2;
const int RUNS = 1000;

void primSoup(Object generation[], int size);//generates a random first generation

int main(int argc, char **argv){
  if(argc != 3)return 0; //Make sure that there are the required number of commandline arguments

  //setup
  unsigned int seed = time(NULL);
  srand(seed);
  Object generation[GEN_SIZE];
  Object nextGen[GEN_SIZE];

  //setup file for record keeping
  ofstream fout;
  fout.open(argv[1]);
  if(!fout){
    cout << "Problem opening record file!" << endl;
    return 0;
  }

  for(int run = 0; run < RUNS;run++){
    cout << "Run:" << run << " of " << RUNS <<endl;
    //setup
    unsigned int seed = time(NULL);
    srand(seed);
    Object generation[GEN_SIZE];
    Object nextGen[GEN_SIZE];
    
    //record basic info
    fout << "Number of generations," << NUM_GEN << endl;
    fout << "Generation Size," << GEN_SIZE << endl;
    fout << "Elitism," << ELITISM << endl;
    fout << "Number of Voxels," << NUM_VOX << endl;
    fout << "Seed," << seed <<endl;
    
    //primordial soup generator
    primSoup(generation, GEN_SIZE);
    
    fout << "Generation,Top Connectivity,Top Phi Rating" << endl;
    
    //running the GA
    for(int i = 0;i < NUM_GEN;i++){
      
      //calculate  the fitness of all objects
      for(int j = 0;j < GEN_SIZE;j++){
	generation[j].calcQuality();
      }
      for(int j = 0;j < GEN_SIZE;j++){
	generation[j].calcFitness(generation, GEN_SIZE);
      }
      genSort(generation,GEN_SIZE);//sort according to dominance
      
      //record keeping
      fout << i << ',';
      generation[0].toCSV(fout);
      
      //elitism
      for(int j = 0;j < ELITISM;j++){
	nextGen[j] = generation[j];
      }
      
      //reproduction
      Children product;
      for(int j = 0;j < ((GEN_SIZE-ELITISM)/2);j++){
	product = crossover(selection(generation,GEN_SIZE));
	nextGen[ELITISM+(2*j)] = product.first;
	nextGen[ELITISM+(2*j)+1] = product.second;
      }
      
      //copying generation over to the next
      for(int j = 0;j < GEN_SIZE;j++){
	generation[j] = nextGen[j];
      }
    }

    //post-run records
    for(int j = 0;j < GEN_SIZE;j++){
      generation[j].calcQuality();
    }
    for(int j = 0;j < GEN_SIZE;j++){
      generation[j].calcFitness(generation, GEN_SIZE);
    }
    genSort(generation,GEN_SIZE);//sort according to dominance
    
    //record keeping
    fout << NUM_GEN << ',';
    generation[0].toCSV(fout);

    if(run == RUNS-1){
      //export example to scad file
      ofstream out;
      out.open(argv[2]);
      if(!out){
	cout << "Problem opening scad file!" << endl;
	return 0;
      }
      generation[0].toScad(out);
    }
  }
  fout.close();
  return 0;
}

void primSoup(Object generation[], int size){
  voxel cloud[NUM_VOX];
  int generator;
  for(int i = 0;i < size;i++){
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
}
