#ifndef OBJLOADER_H
#define OBJLOADER_H

#include "Strutture.h"
bool loadOBJ(string path, Mesh& mesh);
bool loadAssImp(const char* path, vector<unsigned short>& indices, vector<vec3>& vertices, vector<vec2>& uvs, vector<vec3>& normals);

#endif