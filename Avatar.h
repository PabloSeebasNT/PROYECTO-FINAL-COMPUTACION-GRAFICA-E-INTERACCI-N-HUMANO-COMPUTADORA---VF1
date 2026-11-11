#pragma once
#pragma once
// AVATAR propio (Yadira): un explorador de nieve hecho con formas basicas y jerarquia.
// Se mueve con W A S D y el raton, y viaja de planeta en planeta cuando cambias de vista.
#include <glm.hpp>

struct Avatar
{
	glm::vec3 posicion;			// donde esta parado (los pies)
	float rumbo;				// hacia donde mira, en grados (180 = hacia -Z)
	float faseCaminata;			// avanza mientras camina (mueve piernas y brazos)
	float intensidadCaminata;	// 0 = quieto, 1 = caminando (cambia suave)
};
extern Avatar avatar;

void ColocarAvatar(glm::vec3 posicion, float rumbo);
//W/S avanzar y retroceder, A/D caminar de lado, el raton (xCambio) lo gira. dt en SEGUNDOS
//centro y radioCaminable: el avatar no sale de ese circulo
void ActualizarAvatar(bool* teclas, float xCambio, float dt, glm::vec3 centro, float radioCaminable);
//camara en tercera persona: devuelve donde esta el ojo y hacia donde mira
void CamaraAvatar(float dt, glm::vec3& ojo, glm::vec3& objetivo);
void DibujarAvatar(float t);