#ifndef CAMERA_H
#define CAMERA_H
#include "Strutture.h"

extern float cameraSpeed;
extern vec3 asse;
extern string Operazione;
extern string stringa_asse;
extern int idfg, idfi;
extern int selected_obj;


void moveCameraLeft();
void moveCameraRight();
void moveCameraForward();
void moveCameraBack();
void modifyModelMatrix(glm::vec3 translation_vector, glm::vec3 rotation_vector, GLfloat angle, GLfloat scale_factor);


#endif