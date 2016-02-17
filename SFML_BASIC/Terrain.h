#pragma once
#include "SFML/OpenGL.hpp"
#include "SFML/Graphics.hpp" 
class Terrain
{
	//size of the mesh forming the terrain
	int gridWidth,gridDepth;
	int numVerts;
	//size of the terrain in world_coords
	float terrWidth,terrDepth;

	typedef  GLfloat vector[3];
	//array of vertices for the grid(this will be a triangle list)
	//I know, very inefficient, but let's run before we walk
	vector *vertices;
	vector *colors;

	vector *textureCoordinates;
	vector *normals;

	sf::Image image;
	sf::Color color;

	float pixels[100][100];

	float getHeight(int x, int y);
	void setPoint(vector nrmlVec, float p1, float p2, float p3);
	void NormalVector(GLfloat p1[], GLfloat p2[], GLfloat p3[], GLfloat n[]);
	void setTexCoords(vector v, float x, float y);

	GLfloat normal[3];
	int currentNumTriangles;
	float tallestPoint;
	const float HEIGHTMULTI = 40;
	
public:
	Terrain(void);
	~Terrain(void);

	void Init();
	void Draw();
};

