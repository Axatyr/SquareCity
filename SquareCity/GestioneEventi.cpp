#include "GestioneEventi.h"

bool firstMouse = true;
float lastX = (float)width / 2;
float lastY = (float)height / 2;
float yaw_ = -90.0f;
float pitch_ = 0.0f;

enum {
	NAVIGATION,
	CAMERA_MOVING,
	TRASLATING,
	ROTATING,
	SCALING
} OperationMode;

enum {
	X,
	Y,
	Z
} WorkingAxis;

void keyboardPressedEvent(unsigned char key, int x, int y) {
	switch (key)
	{
	case 'a':
		moveCameraLeft();
		break;
	case 'd':
		moveCameraRight();
		break;

	case 'w':
		moveCameraForward();
		break;

	case 's':
		moveCameraBack();
		break;

	case 'g':  //Si entra in modalità di operazione traslazione
		OperationMode = TRASLATING;
		Operazione = "TRASLAZIONE";
		break;
	case 'r': //Si entra in modalità di operazione rotazione
		OperationMode = ROTATING;
		Operazione = "ROTAZIONE";
		break;
	case 'S':
		OperationMode = SCALING; //Si entra in modalità di operazione scalatura
		Operazione = "SCALATURA";
		break;
	case 27:
		glutLeaveMainLoop();
		break;
		// Selezione dell'asse
	case 'x':
		WorkingAxis = X;  //Seleziona l'asse X come asse lungo cui effettuare l'operazione selezionata (tra traslazione, rotazione, scalatura)
		stringa_asse = " Asse X";
		break;
	case 'y':
		WorkingAxis = Y;  //Seleziona l'asse X come asse lungo cui effettuare l'operazione selezionata (tra traslazione, rotazione, scalatura)
		stringa_asse = " Asse Y";
		break;
	case 'z':
		WorkingAxis = Z;
		stringa_asse = " Asse Z";  //Seleziona l'asse X come asse lungo cui effettuare l'operazione selezionata (tra traslazione, rotazione, scalatura)
		break;

	default:
		break;
	}

	// Selezione dell'asse per le trasformazioni
	switch (WorkingAxis) {
	case X:	asse = glm::vec3(1.0, 0.0, 0.0);

		break;
	case Y: asse = glm::vec3(0.0, 1.0, 0.0);

		break;
	case Z: asse = glm::vec3(0.0, 0.0, 1.0);

		break;
	default:
		break;
	}

	glutSetWindow(idfi);
	glutPostRedisplay();

	// I tasti + e -  aggiornano lo spostamento a destra o a sinistra, la rotazione in segno antiorario o in senso orario, la scalatura come amplificazione o diminuizione delle dimensioni

	float amount = .1;
	if (key == '+')
		amount *= 1;

	if (key == '-')
		amount *= -1;


	switch (OperationMode) {

		//la funzione modifyModelMatrix(glm::vec3 translation_vector, glm::vec3 rotation_vector, GLfloat angle, GLfloat scale_factor) 
		// definisce la matrice di modellazione che si vuole postmoltiplicare alla matrice di modellazione dell'oggetto selezionato, per poterlo traslare, ruotare scalare.

	case TRASLATING:
		// si passa angle 0 e scale factor =1, 
		modifyModelMatrix(asse * amount, asse, 0.0f, 1.0f);
		break;
	case ROTATING:
		// SI mette a zero il vettore di traslazione (vec3(0) e ad 1 il fattore di scale
		modifyModelMatrix(glm::vec3(0), asse, amount * 2.0f, 1.0f);
		break;
	case SCALING:
		// SI mette a zero il vettore di traslazione (vec3(0), angolo di rotazione a 0 e ad 1 il fattore di scala 1+amount.
		modifyModelMatrix(glm::vec3(0), asse, 0.0f, 1.0f + amount);
		break;

	}
	glutSetWindow(idfi);
	glutPostRedisplay();

	glutSetWindow(idfg);
	glutPostRedisplay();

}

// Gestione selezione oggetti schermata secondaria
vec3 get_ray_from_mouse(float mouse_x, float mouse_y) {


	// mappiamo le coordinate di viewport del mouse [0,width], [height,0] in coordinate normalizzate [-1,1]  
	float x = (2.0f * mouse_x) / width - 1.0;
	float y = 1.0f - (2.0f * mouse_y) / height;
	float z = 1.0f;
	vec3  ray_nds = vec3(x, y, z);
	// clip space
	vec4 ray_clip = vec4(x, y, -1.0, 1.0);

	// eye space

	vec4 ray_eye = inverse(Projection) * ray_clip;


	ray_eye = vec4(ray_eye.x, ray_eye.y, -1.0, 0.0);

	// world space
	vec3 ray_wor = vec3(inverse(View) * ray_eye);

	ray_wor = normalize(ray_wor);

	return ray_wor;
}

/*controlla se un raggio interseca una sfera. In caso negativo, restituisce false. Rigetta
le intersezioni dietro l'origine del raggio, e pone  intersection_distance all'intersezione p iù vicina.
*/

bool ray_sphere(vec3 ray_origin_wor, vec3 ray_direction_wor, vec3 sphere_centre_wor, float sphere_radius, float* intersection_distance) {

	//Calcoliamo O-C
	vec3 dist_sfera = ray_origin_wor - sphere_centre_wor;
	float b = dot(dist_sfera, ray_direction_wor);
	float f = dot(dist_sfera, dist_sfera) - sphere_radius * sphere_radius;

	float delta = b * b - f;

	if (delta < 0)
		return false;

	if (delta > 0.0f) {
		//calcola le due intersezioni
		float t_a = -b + sqrt(delta);
		float t_b = -b - sqrt(delta);
		*intersection_distance = t_b;

		if (t_a < 0.0) {
			if (t_b < 0)
				return false;
		}

		return true;
	}

	if (delta == 0) {
		float t = -b + sqrt(delta);
		if (t < 0)
			return false;
		*intersection_distance = t;
		return true;
	}

	return false;
}

void mouse(int button, int state, int x, int y) {
	float xmouse = x;
	float ymouse = y;
	vec3 ray_wor = get_ray_from_mouse(xmouse, ymouse);

	selected_obj = -1;
	float closest_intersection = 0.0f;
	for (int i = 0; i < Scena.size(); i++)
	{
		float t_dist = 0.0f;

		if (ray_sphere(ViewSetup.position, ray_wor, centri[i], raggi[i], &t_dist))
		{
			if (selected_obj == -1 || t_dist < closest_intersection)
			{
				selected_obj = i;
				closest_intersection = t_dist;
			}
		}
	}

	printf("Oggetto selezionato %d \n", selected_obj);

	glutSetWindow(idfi);
	glutPostRedisplay();

	glutSetWindow(idfg);
	glutPostRedisplay();

}

void my_passive_mouse(int xpos, int ypos)
{

	float alfa = 0.05; //serve ridimensionare l'offset tra due posizioni successive del mosue
	ypos = height - ypos;
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = ypos - lastY;
	lastX = xpos;
	lastY = ypos;

	xoffset *= alfa;
	yoffset *= alfa;
	yaw_ += xoffset;  //aggiorno l'angolo yaw
	pitch_ += yoffset;  // aggiorno l'angolo Pitch

	// Facciamo si' che l'angolo di Picht vari tra -90 e 90.
	if (pitch_ > 89.0f)
		pitch_ = 89.0f;
	if (pitch_ < -89.0f)
		pitch_ = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
	front.y = sin(glm::radians(pitch_));
	front.z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));
	ViewSetup.direction = vec4(normalize(front), 0.0);
	ViewSetup.target = ViewSetup.position + ViewSetup.direction;
	glutSetWindow(idfg);
	glutPostRedisplay();
}

void update(int a)
{
	angoloUp += 0.05;


	if (angoloUp >= 0 && angoloUp < 25.0)
		angoloUp += 0.05;
	if (angoloUp > 25)
		angoloUp = 0;

	glutTimerFunc(10, update, 0);
	glutSetWindow(idfi);
	glutPostRedisplay();

	glutSetWindow(idfg);
	glutPostRedisplay();

}
