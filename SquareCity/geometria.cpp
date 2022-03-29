#include "Geometria.h"

#define  PI   3.14159265358979323846
int pval = 50;

void crea_cubo(Mesh* mesh, vec4 coloret, vec4 coloreb)
{
	mesh->vertici.push_back(vec3(-1.0, -1.0, 1.0));
	mesh->colori.push_back(coloret);
	mesh->vertici.push_back(vec3(1.0, -1.0, 1.0));
	mesh->colori.push_back(coloret);
	mesh->vertici.push_back(vec3(1.0, 1.0, 1.0));
	mesh->colori.push_back(coloret);
	mesh->vertici.push_back(vec3(-1.0, 1.0, 1.0));
	mesh->colori.push_back(coloret);
	// back
	mesh->vertici.push_back(vec3(-1.0, -1.0, -1.0));
	mesh->colori.push_back(coloreb);
	mesh->vertici.push_back(vec3(1.0, -1.0, -1.0));
	mesh->colori.push_back(coloreb);
	mesh->vertici.push_back(vec3(1.0, 1.0, -1.0));
	mesh->colori.push_back(coloreb);
	mesh->vertici.push_back(vec3(-1.0, 1.0, -1.0));
	mesh->colori.push_back(coloreb);

	mesh->indici.push_back(0); mesh->indici.push_back(1); mesh->indici.push_back(2);
	mesh->indici.push_back(2); mesh->indici.push_back(3); mesh->indici.push_back(0);
	mesh->indici.push_back(1); mesh->indici.push_back(5); mesh->indici.push_back(6);
	mesh->indici.push_back(6); mesh->indici.push_back(2); mesh->indici.push_back(1);
	mesh->indici.push_back(7); mesh->indici.push_back(6); mesh->indici.push_back(5);
	mesh->indici.push_back(5); mesh->indici.push_back(4); mesh->indici.push_back(7);
	mesh->indici.push_back(4); mesh->indici.push_back(0); mesh->indici.push_back(3);
	mesh->indici.push_back(3); mesh->indici.push_back(7); mesh->indici.push_back(4);
	mesh->indici.push_back(4); mesh->indici.push_back(5); mesh->indici.push_back(1);
	mesh->indici.push_back(1); mesh->indici.push_back(0); mesh->indici.push_back(4);
	mesh->indici.push_back(3); mesh->indici.push_back(2); mesh->indici.push_back(6);
	mesh->indici.push_back(6); mesh->indici.push_back(7); mesh->indici.push_back(3);

	mesh->normali.push_back(normalize(vec3(0, 0, 1)));
	mesh->normali.push_back(normalize(vec3(0, 0, 1)));
	mesh->normali.push_back(normalize(vec3(0, 0, 1)));
	mesh->normali.push_back(normalize(vec3(0, 0, 1)));
	mesh->normali.push_back(normalize(vec3(0, 0, -1)));
	mesh->normali.push_back(normalize(vec3(0, 0, -1)));
	mesh->normali.push_back(normalize(vec3(0, 0, -1)));
	mesh->normali.push_back(normalize(vec3(0, 0, -1)));
}
void crea_piano(Mesh* mesh, vec4 color)
{
	mesh->vertici.push_back(vec3(-1.0, 0.0, 1.0));
	mesh->vertici.push_back(vec3(1.0, 0.0, 1.0));
	mesh->vertici.push_back(vec3(1.0, 0.0, -1.0));
	mesh->vertici.push_back(vec3(-1.0, 0.0, -1.0));

	mesh->normali.push_back(vec3(0.0, 1.0, 0.0));
	mesh->normali.push_back(vec3(0.0, 1.0, 0.0));
	mesh->normali.push_back(vec3(0.0, 1.0, 0.0));
	mesh->normali.push_back(vec3(0.0, 1.0, 0.0));

	mesh->indici.push_back(0); mesh->indici.push_back(1); mesh->indici.push_back(2);
	mesh->indici.push_back(0); mesh->indici.push_back(2); mesh->indici.push_back(3);

	for (int i = 0; i < mesh->vertici.size(); i++)
		mesh->colori.push_back(color);

	mesh->texCoords.push_back(vec2(0.0, 0.0));
	mesh->texCoords.push_back(vec2(2.0, 0.0));
	mesh->texCoords.push_back(vec2(2.0, 2.0));
	mesh->texCoords.push_back(vec2(0.0, 2.0));
}
void crea_toro(Mesh* mesh, vec4 colore)
{
	int Stacks = 30;  //numero di suddivisioni sull'asse x
	int Slices = 30;  // numero di suddivisioni sull'asse y
	float R = 1, r = 0.5;
	float s, t;
	//Calc The Vertices
	for (int i = 0; i <= Stacks; ++i)
	{
		float V = i / (float)Stacks;
		float phi = V * glm::pi <float>() * 2;

		// Loop Through Slices
		for (int j = 0; j <= Slices; ++j)
		{
			float U = j / (float)Slices;
			float theta = U * (glm::pi <float>() * 2);

			// Calc The Vertex Positions
			float x = (R + r * cosf(phi)) * cosf(theta);
			float y = r * sinf(phi);
			float z = (R + r * cosf(phi)) * sinf(theta);
			// Push Back Vertex Data
			mesh->vertici.push_back(vec3(x, y, z));
			mesh->colori.push_back(colore);
			mesh->normali.push_back(vec3(normalize(vec3(sin(phi) * cos(theta), cos(phi), sin(theta) * sin(phi)))));

			s = U;
			t = V;
			mesh->texCoords.push_back(vec2(s, t));
		}
	}
	// Calc The Index Positions
	for (int i = 0; i < Slices * Stacks + Slices; ++i)
	{
		mesh->indici.push_back(i);
		mesh->indici.push_back(i + Slices + 1);
		mesh->indici.push_back(i + Slices);
		mesh->indici.push_back(i + Slices + 1);
		mesh->indici.push_back(i);
		mesh->indici.push_back(i + 1);
	}
}
void crea_sfera(Mesh* mesh, vec4 colore)
{
	vec3 centro = vec3(0.0, 0.0, 0.0);
	vec3 raggio = vec3(1.0, 1.0, 1.0);
	int Stacks = 50;  //numero di suddivisioni sull'asse y
	int Slices = 50;  // numero di suddivisioni sull'asse x
	float s, t;
	//Calc The Vertices
	//Stacks= suddivisioni lungo l'asse y
	for (int i = 0; i <= Stacks; ++i)
	{
		float V = i / (float)Stacks;
		float phi = V * pi <float>();
		// Loop Through Slices suddivisioni lungo l'asse x
		for (int j = 0; j <= Slices; ++j)
		{
			float U = j / (float)Slices;
			float theta = U * (pi <float>() * 2);
			// Calc The Vertex Positions
			float x = centro.x + raggio.x * (cosf(theta) * sinf(phi));
			float y = centro.y + raggio.y * cosf(phi);
			float z = centro.z + raggio.z * sinf(theta) * sinf(phi);

			mesh->vertici.push_back(vec3(x, y, z));
			mesh->colori.push_back(colore);
			mesh->normali.push_back(vec3(x, y, z));
			s = U;
			t = V;
			mesh->texCoords.push_back(vec2(s, t));
		}
	}
	// Calc The Index Positions
	for (int i = 0; i < Slices * Stacks + Slices; ++i)
	{
		mesh->indici.push_back(i);
		mesh->indici.push_back(i + Slices + 1);
		mesh->indici.push_back(i + Slices);
		mesh->indici.push_back(i + Slices + 1);
		mesh->indici.push_back(i);
		mesh->indici.push_back(i + 1);
	}
	mesh->vertici.push_back(vec3(0.0, 0.0, 0.0));
	mesh->colori.push_back(vec4(0.0, 1.0, 0.0, 1.0));
	int nv = mesh->vertici.size();
	mesh->indici.push_back(nv - 1);
}
void costruisci_fontana(vec4 color_top, vec4 color_bot, Figura* fontana)
{
	float* t;
	fontana->CP.push_back(vec3(0.0, 0.7, 0.0));
	fontana->CP.push_back(vec3(-0.3, 0.7, 0.0));
	fontana->CP.push_back(vec3(-0.3, 0.3, 0.0));
	fontana->CP.push_back(vec3(-0.5, 0.0, 0.0));
	fontana->CP.push_back(vec3(-2.0, 0.0, 0.0));
	fontana->CP.push_back(vec3(-0.5, -1.0, 0.0));
	fontana->CP.push_back(vec3(-1.5, -2.0, 0.0));
	t = new float[fontana->CP.size()];
	int i;
	float step = 1.0 / (float)(fontana->CP.size() - 1);
	for (i = 0; i < fontana->CP.size(); i++) {
		t[i] = i * step;
	}
	InterpolazioneHermite(t, fontana, color_top, color_bot);
	fontana->nv = fontana->vertici.size();
	fontana->Model = mat4(1.0);
	fontana->Model = translate(fontana->Model, vec3(0.0, 0.0, 0.0));
	fontana->Model = scale(fontana->Model, vec3(1.0, 1.0, 1.0));

}
void costruisci_formaBezier(vec4 color_top, vec4 color_bot, Figura* forma)
{
	/*float tg;
	int j;
	Poligonale.CP = Curva.CP;
	Poligonale.colCP = Curva.colCP;

	forma->vertici.clear();
	forma->colors.clear();
	vector<vec3> c;

	if (Poligonale.CP.size() > 1)
	{
		int i;
		float step = 1.0 / (float)(Curva.CP.size() - 1);
		float passotg = 1.0 / (float)(pval - 1);

		for (tg = 0; tg <= 1; tg += passotg)
		{
			c = Curva.CP;
			for (j = 1; j < Curva.CP.size(); j++)
				for (i = 0; i < Curva.CP.size() - j; i++)
					c[i] = vec3((1 - tg) * c[i].x + tg * c[i + 1].x, (1 - tg) * c[i].y + tg * c[i + 1].y, 0.0);

			forma->vertici.push_back(vec3(c[0].x, c[0].y, 0.0));
			forma->colors.push_back(vec4(1.0, 0.0, 0.0, 1.0));
		}
		forma->nv = forma->vertici.size();
	}*/
}
void rivoluzione(Figura figura, Mesh* superficie)
{
	int i, j, nvCerchio = 50;
	float stepA = (2 * PI) / nvCerchio;
	float t, x, y, z;
	for (i = 0; i < figura.vertici.size(); i++)
	{
		//Ogni vertice della curva profilo genera una circonferenza con centro sull'asse y e raggio uguale alla sua  coordinata x
		for (j = 0; j < nvCerchio; j++)
		{
			t = (float)j * stepA;
			y = figura.vertici[i].y;
			x = figura.vertici[i].x * cos(t);
			z = figura.vertici[i].x * sin(t);
			superficie->vertici.push_back(vec3(x, y, z));
			if (i % 2 == 0)
				superficie->colori.push_back(vec4(1.0, 0.0, 0.0, 1.0));
			else
				superficie->colori.push_back(vec4(1.0, 1.0, 0.0, 1.0));
		}
	}
	int n = figura.vertici.size() - 1;

	//Costruisce la triangolazione specificando gli indici che devono essere collegati a tre a tre per individuare i triangoli
	for (int i = 0; i < n * (nvCerchio - 1) - 2; ++i)
	{
		superficie->indici.push_back(i);
		superficie->indici.push_back(i + pval + 1);
		superficie->indici.push_back(i + pval);
		superficie->indici.push_back(i + pval + 1);
		superficie->indici.push_back(i);
		superficie->indici.push_back(i + 1);
	}
}