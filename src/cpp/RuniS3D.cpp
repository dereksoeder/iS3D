#include "iS3D.h"

int main(int argc, char *argv[])
{
  //create an instance of IS3D class
  iS3D::IS3D particlization;

  //run iS3D
  //if argument == 1, freeeout surface is read from file
  //otherwise freezeout surface is read from memory
  particlization.run_particlization(1);
}
