#pragma once
#pragma once
// Planeta HILEX: todo lo de adentro del planeta vive en Hilex.cpp
#include <glm.hpp>
#include "PointLight.h"
#include "Escena.h"

const glm::vec3 CENTRO_HILEX(20000.0f, 0.0f, 0.0f);				// lejos del universo y de Peanuts
const glm::vec3 INICIO_AVATAR_HILEX(0.0f, 0.0f, 10.0f);			// donde aparece el avatar (relativo al centro)
const float RADIO_CAMINABLE_HILEX = 110.0f;

void CargarHilex();
void LucesHilex(float tiempo, bool deNoche, PointLight* puntuales, unsigned int& np);
void DibujarHilex(const ContextoEscena& c, float t, bool deNoche);
glm::vec3 ResolverColisionHilex(glm::vec3 p, float radio);