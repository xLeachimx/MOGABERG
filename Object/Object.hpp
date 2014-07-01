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
  double x;
  double y;
  double z;
};

struct vector{
  double x;
  double y;
  double z;
};

struct BoundingBox{
  int xMax;
  int xMin;
  int yMax;
  int yMin;
  int zMax;
  int zMin;
};

const int NUM_VOX = 100;
const int ENCODED_SIZE = (NUM_VOX-1)*4;
const int DISP_FACTOR = 0x80;
const int VOX_SIZE = 1;
const double CON_RATIO = 4.0; //Ratio for maximum connectivity
const int PHI = 1.618033988;

class Object{
public:
  //constructors
  Object();
  Object(char rep[], int size);
  Object(const Object &copy);

  //accessors
  char *getEncoding();
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
  void calcBoundingBox();
  void calcPhiRating();
  void calcSymmetry();
  void calcComplexity();
  bool pareToDominate(const Object &comp);//is this object pareto dominant over the comp object
  double distance(voxel &one,voxel &two);//finds the distance between two voxels

  //vector maths
  void makeUnit(vector &v);
  void scalarMult(vector &v, double scale);

  //Supplemental functions
  void generateVoxels();
private:
  voxel voxels[NUM_VOX];
  char encoding[ENCODED_SIZE];
  BoundingBox bBox;
  double phiRating;
  double symmetry;
  double complexity;
  int fitness;
};

#endif
