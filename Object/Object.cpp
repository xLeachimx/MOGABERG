/*FILE: Object.cpp
 *Author: Michael Huelsman
 *Abstract:
 *   Implemention of the class defined in Object.hpp
 *
 */

#include "Object.hpp"
#include <cmath>
#include <iostream>
using std::sqrt;
using std::abs;
using std::endl;
using std::cout;

Object::Object(){
  phiRating = 0.0;
  complexity = 0.0;
  symmetry = 0.0;
  fitness = 0;
}

Object::Object(char rep[], int size){
  for(int i = 0 ;i < size;i++){
    encoding[i] = rep[i];
  }
  generateVoxels();
  phiRating = 0.0;
  complexity = 0.0;
  symmetry = 0.0;
  fitness = 0;
}

Object::Object(const Object &copy){
  for(int i = 0;i < NUM_VOX;i++){
    voxels[i] = copy.voxels[i];
  }
  for(int i = 0;i < ENCODED_SIZE;i++){
    encoding[i] = copy.encoding[i];
  }
  phiRating = copy.phiRating;
  complexity = copy.complexity;
  symmetry = copy.symmetry;
  fitness = copy.fitness;
}


char *Object::getEncoding(){
  return encoding;
}

void Object::toCSV(ostream &out){
  out << phiRating << "," << complexity << "," << symmetry << '\n';
}

void Object::toScad(ostream &out){
  out << "//Phi rating:" << phiRating << endl;
  out << "//Complexity:" << complexity <<endl;
  out << "//Symmetry:" << symmetry <<endl;
  out << "hull(){" << endl;
  for(int i = 0;i < NUM_VOX;i++){
	cout << i << endl;
    out << "\ttranslate([" << voxels[i].x << "," << voxels[i].y << "," << voxels[i].z << "])" <<endl;
    out << "\t\tsphere(r=" << VOX_SIZE << ");" << endl;
  }
  out << "}" <<endl;
}

void Object::calcQuality(){
  calcBoundingBox();
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
  for(int i = 0;i < ENCODED_SIZE;i++){
    encoding[i] = copy.encoding[i];
  }
  phiRating = copy.phiRating;
  complexity = copy.complexity;
  symmetry = copy.symmetry;
  fitness = copy.fitness;
  return *this;
}

//sets the bBox variable
void Object::calcBoundingBox(){
  bBox.xMax = voxels[0].x+VOX_SIZE;
  bBox.yMax = voxels[0].y+VOX_SIZE;
  bBox.zMax = voxels[0].z+VOX_SIZE;
  bBox.xMin = voxels[0].x-VOX_SIZE;
  bBox.yMin = voxels[0].y-VOX_SIZE;
  bBox.zMin = voxels[0].z-VOX_SIZE;

  for(int i = 1;i < NUM_VOX;i++){
    //reassign x
    if(bBox.xMax < voxels[i].x+VOX_SIZE){
      bBox.yMax = voxels[i].x+VOX_SIZE;
    }
    else if(bBox.yMin > voxels[i].x-VOX_SIZE){
      bBox.yMin = voxels[i].x-VOX_SIZE;
    }
    
    //reassign y
    if(bBox.xMax < voxels[i].y+VOX_SIZE){
      bBox.yMax = voxels[i].y+VOX_SIZE;
    }
    else if(bBox.yMin > voxels[i].y-VOX_SIZE){
      bBox.yMin = voxels[i].y-VOX_SIZE;
    }

    //reassign z
    if(bBox.zMax < voxels[i].z+VOX_SIZE){
      bBox.zMax = voxels[i].z+VOX_SIZE;
    }
    else if(bBox.zMin > voxels[i].z-VOX_SIZE){
      bBox.zMin = voxels[i].z-VOX_SIZE;
    }
  }
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
    symmetry += (xDist+yDist+zDist);
  }
}

void Object::calcComplexity(){
  int outer = 0.0; //number of voxels inside the internal bounding box
  double xInter = (bBox.xMax-bBox.xMin)/4.0; 
  double yInter = (bBox.yMax-bBox.yMin)/4.0;
  double zInter = (bBox.zMax-bBox.zMin)/4.0;
  BoundingBox internal;
  internal.xMin = bBox.xMin + xInter;
  internal.xMax = bBox.xMin + (3*xInter);
  internal.yMin = bBox.yMin + yInter;
  internal.yMax = bBox.yMin + (3*yInter);
  internal.zMin = bBox.zMin + zInter;
  internal.zMax = bBox.zMin + (3*zInter);
  outer = NUM_VOX;
  for(int i = 0;i < NUM_VOX;i++){
    if(voxels[i].x-VOX_SIZE >= internal.xMin && voxels[i].x+VOX_SIZE <= internal.xMax){
      if(voxels[i].y-VOX_SIZE >= internal.yMin && voxels[i].y+VOX_SIZE <= internal.yMax){
	if(voxels[i].z-VOX_SIZE >= internal.zMin && voxels[i].z+VOX_SIZE <= internal.zMax)outer--;
      }
    }
  }
  complexity = abs(5-(((double)NUM_VOX-outer)/((double)outer)));
}

bool Object::pareToDominate(const Object &comp){
  if(phiRating <= comp.phiRating && complexity <= comp.complexity && symmetry <= comp.symmetry){
    if(phiRating < comp.phiRating || complexity < comp.complexity || symmetry <= comp.symmetry)return true;
  }
  return false;
}

double Object::distance(voxel &one, voxel &two){
  double x = one.x-two.x;
  double y = one.y-two.y;
  double z = one.z-two.z;
  return sqrt((x*x) + (y*y) + (z*z));
}


void Object::makeUnit(vector &v){
  double mag = sqrt((v.x*v.x) + (v.y*v.y) + (v.z*v.z));
  v.x = v.x/mag;
  v.y = v.y/mag;
  v.z = v.z/mag;
}

void Object::scalarMult(vector &v, double scale){
  v.x = v.x*scale;
  v.y = v.y*scale;
  v.z = v.z*scale;
}

void Object::generateVoxels(){
  voxel temp;
  temp.x = 0.0;
  temp.y = 0.0;
  temp.z = 0.0;
  voxels[0] = temp;
  for(int i = 0;i < NUM_VOX-1;i++){
    int accessMod = 4*i;
    vector vec;
    vec.x = (double)encoding[accessMod];
    vec.y = (double)encoding[accessMod+1];
    vec.z = (double)encoding[accessMod+2];
    double disp = (double)encoding[accessMod+3];
    disp = disp/DISP_FACTOR;
    disp += 1;
    makeUnit(vec);
    scalarMult(vec, disp);
    temp.x = voxels[i].x+vec.x;
    temp.y = voxels[i].y+vec.y;
    temp.z = voxels[i].z+vec.z;
    voxels[i+1] = temp;
  }
}
