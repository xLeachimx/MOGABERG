/*FILE: Object.cpp
 *Author: Michael Huelsman
 *Abstract:
 *   Implemention of the class defined in Object.hpp
 *
 */

#include "Object.hpp"
#include <cmath>
using std::sqrt;
using std::abs;
using std::endl;

Object::Object(){
  connectivity = 0.0;
  phiRating = 0.0;
  complexity = 0.0;
  symmetry = 0.0;
  fitness = 0;
}

Object::Object(voxel copy[NUM_VOX]){
  for(int i = 0 ;i < NUM_VOX;i++){
    voxels[i] = copy[i];
  }
  connectivity = 0.0;
  phiRating = 0.0;
  complexity = 0.0;
  symmetry = 0.0;
  fitness = 0;
}

Object::Object(const Object &copy){
  for(int i = 0;i < NUM_VOX;i++){
    voxels[i] = copy.voxels[i];
  }
  connectivity = copy.connectivity;
  phiRating = copy.phiRating;
  complexity = copy.complexity;
  symmetry = copy.symmetry;
  fitness = copy.fitness;
}


voxel *Object::getVoxels(){
  return voxels;
}

int Object::getConnectivity(){
  return connectivity;
}

int Object::getPhiRating(){
  return phiRating;
}

void Object::toCSV(ostream &out){
  out << connectivity << "," << phiRating << "," << complexity << "," << symmetry << '\n';
}

void Object::toScad(ostream &out){
  out << "//Connectivity:" << connectivity << endl;
  out << "//Phi rating:" << phiRating << endl;
  out << "//Complexity:" << complexity <<endl;
  out << "//Symmetry:" << symmetry <<endl;
  out << "hull(){" << endl;
  for(int i = 0;i < NUM_VOX;i++){
    out << "\ttranslate([" << (int)voxels[i].x << "," << (int)voxels[i].y << "," << (int)voxels[i].z << "])" <<endl;
    out << "\t\tsphere(r=" << (int)voxels[i].size << ");" << endl;
  }
  out << "}" <<endl;
}

void Object::calcQuality(){
  calcBoundingBox();
  calcConnectivity();
  calcComplexity();
  calcPhiRating();
  calcSymmetry();
}

void Object::calcFitness(Object *gen, int size){
  fitness = 0;
  for(int i = 0;i < size;i++){
    if(pareToDominate(gen[i]))fitness++;
  }
}

bool Object::operator>(const Object &comp){
  return fitness > comp.fitness;
}

bool Object::operator>=(const Object &comp){
  return fitness >= comp.fitness;
}

bool Object::operator<(const Object &comp){
  return fitness < comp.fitness;
}

bool Object::operator<=(const Object &comp){
  return fitness <= comp.fitness;
}

bool Object::operator==(const Object &comp){
  return fitness == comp.fitness;
}

bool Object::operator!=(const Object &comp){
  return fitness != comp.fitness;
}

Object &Object::operator=(const Object &copy){
  for(int i = 0 ;i < NUM_VOX;i++){
    voxels[i] = copy.voxels[i];
  }
  connectivity = copy.connectivity;
  phiRating = copy.phiRating;
  complexity = copy.complexity;
  symmetry = copy.symmetry;
  fitness = copy.fitness;
  return *this;
}

//sets the bBox variable
void Object::calcBoundingBox(){
  bBox.xMax = voxels[0].x+voxels[0].size;
  bBox.yMax = voxels[0].y+voxels[0].size;
  bBox.zMax = voxels[0].z+voxels[0].size;
  bBox.xMin = voxels[0].x-voxels[0].size;
  bBox.yMin = voxels[0].y-voxels[0].size;
  bBox.zMin = voxels[0].z-voxels[0].size;

  for(int i = 1;i < NUM_VOX;i++){
    //reassign x
    if(bBox.xMax < voxels[i].x+voxels[i].size){
      bBox.yMax = voxels[i].x+voxels[i].size;
    }
    else if(bBox.yMin > voxels[i].x-voxels[i].size){
      bBox.yMin = voxels[i].x-voxels[i].size;
    }
    
    //reassign y
    if(bBox.xMax < voxels[i].y+voxels[i].size){
      bBox.yMax = voxels[i].y+voxels[i].size;
    }
    else if(bBox.yMin > voxels[i].y-voxels[i].size){
      bBox.yMin = voxels[i].y-voxels[i].size;
    }

    //reassign z
    if(bBox.zMax < voxels[i].z+voxels[i].size){
      bBox.zMax = voxels[i].z+voxels[i].size;
    }
    else if(bBox.zMin > voxels[i].z-voxels[i].size){
      bBox.zMin = voxels[i].z-voxels[i].size;
    }
  }
}

void Object::calcConnectivity(){
  double connections = 0.0;
  for(int i = 0;i < NUM_VOX;i++){
    for(int j = 0;j < NUM_VOX;j++){
      if(i != j){
	int comparedSize = voxels[i].size + voxels[j].size;
	double distanceBetween = distance(voxels[i],voxels[j]);
	if(distanceBetween < comparedSize)connections++;
      }
    }
  }
  connectivity = abs((NUM_VOX/CON_RATIO)-(connections/NUM_VOX));
}

void Object::calcPhiRating(){
  double width = bBox.xMax-bBox.xMin;
  double height = bBox.zMax-bBox.zMin;
  double depth = bBox.yMax-bBox.yMin;
  phiRating = abs((PHI - (width/height)))+abs((PHI - (depth/width))); //actually calculate the use of golden rectangles in the bounding box
}

//tests for symmetry accross the planes: x=0, y=0, and z=0
void Object::calcSymmetry(){
  symmetry = 0.0;
  for(int i = 0;i < NUM_VOX;i++){
    voxel antiX, antiY, antiZ;
    antiX = antiY = antiZ = voxels[i];
    antiX.x = -antiX.x;
    antiY.y = -antiY.y;
    antiZ.z = -antiZ.z;
    voxel xClosest = voxels[0];
    double xDist = distance(xClosest,antiX);
    voxel yClosest = voxels[0];
    double yDist = distance(xClosest,antiY);
    voxel zClosest = voxels[0];
    double zDist = distance(xClosest,antiZ);
    for(int j = 1; j < NUM_VOX;j++){
      //closest object to hypothetical reflection on xAxis
      if(xDist > distance(antiX,voxels[j])){
	xClosest = voxels[j];
	xDist = distance(antiX,voxels[j]);
      }

      if(yDist > distance(antiY,voxels[j])){
	yClosest = voxels[j];
	yDist = distance(antiY,voxels[j]);
      }

      if(zDist > distance(antiZ,voxels[j])){
	zClosest = voxels[j];
	zDist = distance(antiZ,voxels[j]);
      }
    }
    //add to the symmetry count
    double sizeComp = abs(xClosest.size-antiX.size) + abs(yClosest.size-antiY.size) + abs(zClosest.size-antiZ.size);
    symmetry += (xDist+yDist+zDist)+sizeComp;
  }
}

bool Object::pareToDominate(const Object &comp){
  if(connectivity <= comp.connectivity && phiRating <= comp.phiRating){
    if(connectivity < comp.connectivity || phiRating < comp.phiRating)return true;
  }
  return false;
}

double Object::distance(int one, int two){
  double x = (double)((int)voxels[one].x-(int)voxels[two].x);
  double y = (double)((int)voxels[one].y-(int)voxels[two].y);
  double z = (double)((int)voxels[one].z-(int)voxels[two].z);
  return sqrt((x*x) + (y*y) + (z*z));
}
