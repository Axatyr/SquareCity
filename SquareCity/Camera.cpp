#include "Camera.h"

void moveCameraLeft() {
	glm::vec3 direzione_scorrimento = glm::cross(vec3(ViewSetup.direction), glm::vec3(ViewSetup.upVector));   //direzione perpendicolare al piano individuato da direction e upvector
	ViewSetup.position -= glm::vec4(direzione_scorrimento, .0) * cameraSpeed;
	ViewSetup.target = ViewSetup.position + ViewSetup.direction * cameraSpeed;;
}

void moveCameraRight() {
	glm::vec3 direzione_scorrimento = glm::cross(vec3(ViewSetup.direction), glm::vec3(ViewSetup.upVector));
	ViewSetup.position += glm::vec4(direzione_scorrimento, .0) * cameraSpeed;
	ViewSetup.target = ViewSetup.position + ViewSetup.direction;
}

void moveCameraForward() {

	ViewSetup.position += ViewSetup.direction * cameraSpeed;
	ViewSetup.target = ViewSetup.position + ViewSetup.direction;
}

void moveCameraBack() {
	ViewSetup.position -= ViewSetup.direction * cameraSpeed;
	ViewSetup.target = ViewSetup.position + ViewSetup.direction;
}

void modifyModelMatrix(glm::vec3 translation_vector, glm::vec3 rotation_vector, GLfloat angle, GLfloat scale_factor)
{
	//ricordare che mat4(1) costruisce una matrice identità di ordine 4
	mat4 traslation = glm::translate(glm::mat4(1), translation_vector);
	mat4 scale = glm::scale(glm::mat4(1), glm::vec3(scale_factor, scale_factor, scale_factor));
	mat4 rotation = glm::rotate(glm::mat4(1), angle, rotation_vector);

	//Modifica la matrice di Modellazione dell'oggetto della scena selezionato postmolitplicando per le matrici scale*rotation*traslation
	if (selected_obj > -1)
	{
		Scena[selected_obj].Model = Scena[selected_obj].Model * scale * rotation * traslation;
		centri[selected_obj] = centri[selected_obj] + translation_vector;
		raggi[selected_obj] = raggi[selected_obj] * scale_factor;
	}
}