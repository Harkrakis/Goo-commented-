#include "Scene.h"
#include "Ball.h"
#include "Camera.h"
#include "Background.h"
#include "../Utils/Timer.h"
int Scene_DoubleCapacity(Scene *scene);
Scene *Scene_New(Renderer *renderer)
{
    Scene *scene = NULL;
    int capacity = 1 << 10;
    int width  = Renderer_GetWidth(renderer);
    int height = Renderer_GetHeight(renderer);
    scene = (Scene *)calloc(1, sizeof(Scene));
    if (!scene) goto ERROR_LABEL;
    scene->m_textures = Textures_New(renderer);
    if (!scene->m_textures) goto ERROR_LABEL;
    scene->m_camera = Camera_New(width, height);
    if (!scene->m_camera) goto ERROR_LABEL;
    scene->m_input = Input_New();
    if (!scene->m_input) goto ERROR_LABEL;
    scene->m_balls = (Ball *)calloc(capacity, sizeof(Ball));
    if (!scene->m_balls) goto ERROR_LABEL;
    scene->m_renderer = renderer;
    scene->m_ballCount = 0;
    scene->m_ballCapacity = capacity;
    scene->m_timeStep = 1.0f / 100.f;
    
    // Création d'une scène minimale avec trois balles reliées
    Ball *ball1 = Scene_CreateBall(scene, Vec2_Set(-0.75f, 0.0f));
    Ball *ball2 = Scene_CreateBall(scene, Vec2_Set(+0.75f, 0.0f));
    Ball *ball3 = Scene_CreateBall(scene, Vec2_Set(0.0f, 1.299f));
    Ball_Connect(ball1, ball2, 1.5f);
    Ball_Connect(ball1, ball3, 1.5f);
    Ball_Connect(ball2, ball3, 1.5f);
    return scene;
ERROR_LABEL:
    printf("ERROR - Scene_New()\n");
    assert(false);
    Scene_Free(scene);
    return NULL;
}
void Scene_Free(Scene *scene)
{
    if (!scene) return;
    Camera_Free(scene->m_camera);
    Input_Free(scene->m_input);
    Textures_Free(scene->m_textures);
    if (scene->m_balls)
    {
        free(scene->m_balls);
    }
    memset(scene, 0, sizeof(Scene));
    free(scene);
}
Renderer *Scene_GetRenderer(Scene *scene)
{
    return scene->m_renderer;
}
Camera *Scene_GetCamera(Scene *scene)
{
    return scene->m_camera;
}
Input *Scene_GetInput(Scene *scene)
{
    return scene->m_input;
}
Vec2 Scene_GetMousePosition(Scene *scene)
{
    return scene->m_mousePos;
}
int Scene_DoubleCapacity(Scene *scene)
{
    Ball *newBalls = NULL;
    int newCapacity = scene->m_ballCapacity << 1;
    newBalls = (Ball *)realloc(scene->m_balls, newCapacity * sizeof(Ball));
    if (!newBalls) goto ERROR_LABEL;
    scene->m_balls = newBalls;
    scene->m_ballCapacity = newCapacity;
    return EXIT_SUCCESS;
ERROR_LABEL:
    printf("ERROR - Scene_DoubleCapacity()\n");
    return EXIT_FAILURE;
}
Ball *Scene_CreateBall(Scene *scene, Vec2 position)
{
    if (scene->m_ballCount >= scene->m_ballCapacity)
    {
        int exitStatus = Scene_DoubleCapacity(scene);
        if (exitStatus == EXIT_FAILURE) goto ERROR_LABEL;
    }
    Ball *ball = &scene->m_balls[scene->m_ballCount];
    scene->m_ballCount++;
    *ball = Ball_Set(position);
    return ball;
ERROR_LABEL:
    printf("ERROR - Scene_CreateBall()\n");
    return NULL;
}
void Scene_RemoveBall(Scene *scene, Ball *ball)
{
    int ballCount = Scene_GetBallCount(scene);
    Ball *balls = Scene_GetBalls(scene);
    int index = (int)(ball - balls);
    int springCount = 0;
    if (index < 0 || index >= ballCount)
        return;
    // Supprime les ressorts liés à la balle
    springCount = ball->springCount;
    for (int i = 0; i < springCount; ++i)
    {
        Ball_Deconnect(ball, ball->springs[i].other);
    }
    Ball *lastBall = &balls[ballCount - 1];
    if (ball != lastBall)
    {
        // Copie la dernière balle à la position de la balle à supprimer
        *ball = *lastBall;
        // Met à jour ses ressorts
        ball->springCount = 0;
        springCount = lastBall->springCount;
        for (int i = 0; i < springCount; ++i)
        {
            Ball *other = lastBall->springs[i].other;
            float length = lastBall->springs[i].length;
            Ball_Deconnect(lastBall, other);
            Ball_Connect(ball, other, length);
        }
    }
    // Supprime la dernière balle
    scene->m_ballCount--;
}
int Scene_GetBallCount(Scene *scene)
{
    return scene->m_ballCount;
}
Ball *Scene_GetBalls(Scene *scene)
{
    return scene->m_balls;
}
BallQuery Scene_GetNearestBall(Scene *scene, Vec2 position)
{
    int ballCount = Scene_GetBallCount(scene);
    Ball *balls = Scene_GetBalls(scene);
    BallQuery query = { 0 };
		if(ballCount==0)
			query.ball=NULL;
		else
		{
			query.distance = Vec2_Distance(position,balls[0].position);
			query.ball = &balls[0];
    for(int i=1;i<ballCount;i++)
    {
    	if(Vec2_Distance(position,balls[i].position)<query.distance)
    	{	
    		query.distance = Vec2_Distance(position,balls[i].position);
    		query.ball = &balls[i];
    	}
	  }
	  }
    return query;
}

void sortQueries(BallQuery *queries, int queryCount)
{
	int e;
	BallQuery tmp;
	e=0;
	
	//tri à bulles par ordre croissant
	while(e<queryCount-1)
	{
		for(int i=1;i<queryCount;i++)
		{
			if(queries[i-1].distance>queries[i].distance)//vérification selon la distance
			{
				tmp = queries[i];
				queries[i] = queries[i-1];
				queries[i-1] = tmp;
				//échange des variables 
				e=0;
			}
			else
				e++;
				//Lorsque e est égal à queryCount la liste est triée
		}
	}
}


int Scene_GetNearestBalls(Scene *scene, Vec2 position, BallQuery *queries, int queryCount)
{
    int ballCount = Scene_GetBallCount(scene);
    Ball *balls = Scene_GetBalls(scene);	
    	
	//initialisation de queries
	for(int i=0;i<queryCount;i++)
	{
		queries[i].ball=&balls[i];
		queries[i].distance=Vec2_Distance(position,balls[i].position);
	}
	
	//tri du queries
	sortQueries(queries,queryCount);	
	
	
	for(int i=queryCount;i<ballCount;i++)
	{
		if(Vec2_Distance(position,balls[i].position)<queries[queryCount-1].distance)//si une balle est à une distance plus petite que la plus grande valeur
			{
				queries[queryCount-1].ball=&balls[i];
				queries[queryCount-1].distance=Vec2_Distance(position,balls[i].position);
				sortQueries(queries,queryCount);
				//échange des valeurs puis tri
			}
	}
	
	return EXIT_SUCCESS;
}

void Scene_FixedUpdate(Scene *scene, float timeStep)
 
{
    int ballCount = Scene_GetBallCount(scene);
    Ball *balls = Scene_GetBalls(scene);
    for (int i = 0; i < ballCount; i++)
    {
        Ball_UpdateVelocity(&balls[i], timeStep);
    }
    for (int i = 0; i < ballCount; i++)
    {
        Ball_UpdatePosition(&balls[i], timeStep);
    }
}
void Scene_UpdateGame(Scene *scene)
{
	int ballCount = Scene_GetBallCount(scene);
    Input *input = Scene_GetInput(scene);
    Camera *camera = Scene_GetCamera(scene);
    // Initialise les requêtes
    scene->m_validCount = 0;
    memset(scene->m_queries,0,4*sizeof(BallQuery));
    // Calcule la position de la souris et son déplacement
    Vec2 mousePos = Vec2_Set(0.0f, 0.0f);
    Vec2 mouseDelta = Vec2_Set(0.0f, 0.0f);
    Camera_ViewToWorld(camera, (float)input->mouseX, (float)input->mouseY, &mousePos);
    Camera_ViewToWorld(
        camera,
        (float)(input->mouseX + input->mouseDeltaX),
        (float)(input->mouseY + input->mouseDeltaY),
        &mouseDelta
    );
    mouseDelta = Vec2_Sub(mouseDelta, mousePos);
    scene->m_mousePos = mousePos;


    Scene_GetNearestBalls(scene,Scene_GetMousePosition(scene),scene->m_queries,3);
    BallQuery *queries=scene->m_queries;
    for(int i=0;i<3;i++)
    {
    if((queries[i].distance<2.0f)&&(queries[i].distance>0.2f))
			scene->m_validCount ++;
    }


  
    // Déplacement de la caméra
    if (input->mouseRDown)
    {
        Camera_Move(camera, Vec2_Scale(mouseDelta, -1.f));
        return;
    }
	
	if(input->mouseLPressed==true)																									
	{

		Input *input = Scene_GetInput(scene);
		float xs = (float)input->mouseX;  //Position de la souris en pixel sur axe x
		float ys = (float)input->mouseY;  //Position de la souris en pixel sur axe y
		
		if(((xs>=X_1)&&(xs<=X_1+W))&&((ys>=Y)&&(ys<=ys+H))) // Si dans le carré de l'image de ces coordonnées
		{
											//change au mode Balles lourdes
				modemod(4);
		}
		else if(((xs>=X_2)&&(xs<=X_2+W))&&((ys>=Y)&&(ys<=ys+H)))
		{									//change au mode grav neg
				modemod(5);
		}
		else if(((xs>=X_3)&&(xs<=X_3+W))&&((ys>=Y)&&(ys<=ys+H)))
		{
											//change au mode lune
				modemod(2);
		}
		else if(((xs>=X_4)&&(xs<=X_4+W))&&((ys>=Y)&&(ys<=ys+H)))
		{
											//change au mode nograv
				modemod(3);
		}
		else if(((xs>=X_5)&&(xs<=X_5+W))&&((ys>=Y)&&(ys<=ys+H)))
		{
											//change au mode nograv
				modemod(1);
		}
		
		else   // si souris dans aucune des images
		{
		
		
		
			BallQuery Nearest_Ball=Scene_GetNearestBall(scene,Scene_GetMousePosition(scene));    		 
			
			if((Nearest_Ball.distance<0.2f)&&(ballCount!=0))
				{
					Scene_RemoveBall(scene,Nearest_Ball.ball);
				}
			else
			{
				Ball *ball4 = Scene_CreateBall(scene, Scene_GetMousePosition(scene));
				
				if(ballCount!=0)
				{
					for(int i=0;i<3;i++)
					{
						if((scene->m_queries[i].ball!=NULL)&&(scene->m_queries[i].distance<2.0f))
						Ball_Connect(scene->m_queries[i].ball, ball4, 1.5f);
					}
				
				}
			
			 	}
			}
		}
	
		if(input-> KeyDDown==true)
	{
		BallQuery Nearest_Ball=Scene_GetNearestBall(scene,Scene_GetMousePosition(scene));
		if(Nearest_Ball.distance<0.5)
		{
			Nearest_Ball.ball->velocity= Vec2_Set(0.0f,0.0f);
			Nearest_Ball.ball->position=Scene_GetMousePosition(scene);
		}
	}
	
}
void Scene_Update(Scene *scene)
{
    float timeStep = scene->m_timeStep;
    // Met à jour les entrées de l'utilisateur
    Input_Update(scene->m_input);
    // Met à jour le moteur physique (pas de temps fixe)
    scene->m_accu += Timer_GetDelta(g_time);
    while (scene->m_accu >= timeStep)
    {
        Scene_FixedUpdate(scene, timeStep);
        scene->m_accu -= timeStep;
    }
    // Met à jour la caméra (déplacement)
    Camera_Update(scene->m_camera);
    // Met à jour le jeu
    Scene_UpdateGame(scene);
}
void Scene_RenderBalls(Scene *scene)
{
    int ballCount = Scene_GetBallCount(scene);
    Ball *balls = Scene_GetBalls(scene);
    for (int i = 0; i < ballCount; i++)
    {
        Ball *ball = &balls[i];
        int springCount = ball->springCount;
        for (int j = 0; j < springCount; j++)
        {
            // Supprime le flag
            ball->springs[j].flags &= ~SPRING_RENDERED;
        }
    }
    for (int i = 0; i < ballCount; i++)
    {
        Ball *ball = &balls[i];
        Vec2 start = Ball_GetPosition(ball);
        int springCount = ball->springCount;
        for (int j = 0; j < springCount; j++)
        {
            Spring *spring = ball->springs + j;
            if ((spring->flags & SPRING_RENDERED) != 0)
            {
                continue;
            }
            // Ajoute le flag
            spring->flags |= SPRING_RENDERED;
            // Affiche le ressort
            Vec2 end = Ball_GetPosition(spring->other);
            Ball_RenderSpring(start, end, scene, true);
        }
    }
    for (int i = 0; i < ballCount; i++)
    {
        // Affiche la balle
        Ball_Render(&balls[i], scene);
    }
}
void Scene_Render(Scene *scene)
{
    // Dessine le fond (avec parallax)
    Background_Render(scene);
    // Dessine le sol
    TileMap_Render(scene);
    
    
    if (scene->m_input->mouseRDown == false)
    {
        // Dessine les ressorts inactifs
        int validCount = scene->m_validCount;
        BallQuery *queries = scene->m_queries;
        for (int i = 0; i < validCount; ++i)
        {
            Vec2 start = Scene_GetMousePosition(scene);
            Vec2 end = Ball_GetPosition(queries[i].ball);
            Ball_RenderSpring(start, end, scene, false);
        }
    }
    // Dessine les balles (avec les ressorts actifs)
    Scene_RenderBalls(scene);
    
    //Dessine les bouttons pour choisir les modes
    ModeButton_Render(scene);
}

