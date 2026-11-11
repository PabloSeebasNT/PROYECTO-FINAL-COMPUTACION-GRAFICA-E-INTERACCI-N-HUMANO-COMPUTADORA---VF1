#pragma once
// Planeta Peanuts (Snoopy y sus amigos) - Yadira
#include <glm.hpp>
#include "PointLight.h"
#include "Escena.h"

const glm::vec3 CENTRO_PEANUTS(10000.0f, 0.0f, 0.0f);				// centro de su piso (lejos del universo, para no ver nada mas)
const glm::vec3 INICIO_AVATAR_PEANUTS(0.0f, 0.0f, 10.0f);			// donde aparece el avatar (en la calle del centro, relativo al centro)
const float RADIO_CAMINABLE_PEANUTS = 110.0f;						// el avatar no sale de este circulo

void CargarPeanuts();																	// modelos, texturas y mallas (una vez, antes del while)
void LucesPeanuts(float tiempo, bool deNoche, PointLight* puntuales, unsigned int& np);	// lamparas y focos de las casas
void AlternarLamparasPeanuts();															// tecla N: prende o apaga las lamparas
void DibujarPeanuts(const ContextoEscena& c, float t, bool deNoche);					// todo lo que se ve en el planeta
void DibujarSuperficiePeanuts(float t, bool deNoche);
glm::vec3 ResolverColisionPeanuts(glm::vec3 p, float radio);							// para que el avatar no atraviese nada