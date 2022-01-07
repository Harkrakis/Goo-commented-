#ifndef _BACKGROUND_H_
#define _BACKGROUND_H_

#include "../Settings.h"

typedef struct Scene_s Scene;

void Background_Render(Scene *scene);
void TileMap_Render(Scene *scene);
static const float Y = 660.0f;
static const float X_1 = 0.0f;
static const float X_2 = 75.0f;
static const float X_3 =150.0f;
static const float X_4 = 225.0f;
static const float X_5 = 300.0f;
static const float H = 50.0f;		///constantes  de position Y de hauteur et de largeur des bouttons
static const float W = 50.0f;

#endif
