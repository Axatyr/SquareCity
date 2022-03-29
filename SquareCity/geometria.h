#pragma once
#ifndef GEOMETRIA_H
#define GEOMETRIA_H

#include "Strutture.h"
#include "Hermitte.h"

void crea_cubo(Mesh* mesh, vec4 colortop, vec4 colorbot);
//Per il piano base
void crea_piano(Mesh* mesh, vec4 color);
//Non so se lo useremo
void crea_toro(Mesh* mesh, vec4 color);
//Per il sole e la luna
void crea_sfera(Mesh* mesh, vec4 color);
//Per quello che dobbiamo creare con Hermitte
void costruisci_fontana(vec4 color_top, vec4 color_bot, Figura* forma);
//Per quello che dobbiamo creare con Bezier
void costruisci_formaBezier(vec4 color_top, vec4 color_bot, Figura* forma);
//Per passare da Hermitte/Bezier 2D a 3D
void rivoluzione(Figura Curva, Mesh* Superficie);
#endif GEOMETRIA_H