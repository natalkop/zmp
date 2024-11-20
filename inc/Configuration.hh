#ifndef CONFIGURATION_HH
#define CONFIGURATION_HH

#include <string>
#include <vector>
#include <iostream>
#include "Vector3D.hh"

using namespace std;

struct Cube {
  string Name;
  Vector3D Shift;
  Vector3D Scale;
  Vector3D RotXYZ_deg;
  Vector3D Trans_m;
  Vector3D RGB;
};

class Configuration {
  private:
    vector<string> Libs;
    vector<Cube> Cubes;
  public:
    void setLib(const string& sLibName) {
      Libs.push_back(sLibName);
    }
    
    void setCube(const Cube& cube) {
      Cubes.push_back(cube);
    }
    
    const vector<string>& getLibs() const {
      return Libs;
    }
    
    const vector<Cube>& getCubes() const {
      return Cubes;
    }
};


#endif
