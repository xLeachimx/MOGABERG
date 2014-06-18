/*FILE: Object.hpp
 *Author: Michael Huelsman
 *Abstract:
 *   A genetic object that expresses its genetic information in 3D voxels 
 *
 */

#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <ostream>
using std::ostream;

struct voxel{
  char x;
  char y;
  char z;
  char size;
};

const int NUM_VOX = 300;
const double CON_RATIO = 4.0; //Ratio for maximum connectivity
const int PHI = 1.618033988;

class Object{
public:
  //constructors
  Object();
  Object(voxel copy[NUM_VOX]);
  Object(const Object &copy);

  //accessors
  voxel *getVoxels();
  int getConnectivity();
  int getPhiRating();
  void toCSV(ostream &out);//outputs quality values
  void toScad(ostream &out);//outputs voxels to scad for viewing

  void calcQuality();
  void calcFitness(Object *gen, int size);//calculates fitness using amount of pareto dominances

  //conparative operators (they compare using pareto dominance over the connectivity and phiRating qualities)
  bool operator>(const Object &comp);
  bool operator>=(const Object &comp);
  bool operator<(const Object &comp);
  bool operator<=(const Object &comp);
  bool operator==(const Object &comp);
  bool operator!=(const Object &comp);

  //simple assignment operator
  Object &operator=(const Object &copy);
protected:
  void calcConnectivity();
  void calcPhiRating();
  bool pareToDominate(const Object &comp);//is this object pareto dominant over the comp object
  double distance(int one,int two);//finds the distance between two voxels
private:
  voxel voxels[NUM_VOX];
  double connectivity;
  double phiRating;
  int fitness;
};

#endif
