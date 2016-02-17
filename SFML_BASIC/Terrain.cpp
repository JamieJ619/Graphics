#include "stdafx.h"
#include "SFML/OpenGL.hpp"
#include "Terrain.h"
#include <cmath>
#include <iostream>

Terrain::Terrain(void)
{
	gridWidth=100;
	gridDepth=100;

	terrWidth=50; //size of terrain in world units
	terrDepth=50;

	vertices=NULL;
	colors=NULL;	

	textureCoordinates = NULL;
	normals = NULL;
	
	//num squares in grid will be width*height, two triangles per square
	//3 verts per triangle
	 numVerts=gridDepth*gridWidth*2*3;

	 image.loadFromFile("../heightmap.jpg");
}

Terrain::~Terrain(void)
{
	delete[] normals;
	delete [] vertices;
	delete [] colors;
	delete[] textureCoordinates;
}

//interpolate between two values
float lerp(float start, float end, float t)
{
	return start+(end-start)*t;
}

void Terrain::setPoint(vector v,float x, float y, float z)
{
	v[0] = x;
	v[1] = y;
	v[2] = z;
}

void Terrain::setTexCoords(vector v, float x, float y)
{
	v[0] = x;
	v[1] = y;	
}

//helper function to calculate height of terrain at a given point in space
//you will need to modify this significantly to pull height from a map
float  Terrain::getHeight(int x, int y)
{
	//for the sample we will calculate height based on distance form origin
	float dist = pixels[y][x] * HEIGHTMULTI;
	return dist;
}

void Terrain::NormalVector(GLfloat p1[], GLfloat p2[], GLfloat p3[], GLfloat n[])
{
	GLfloat vec1[3];
	GLfloat vec2[3];

	for (int i = 0; i < 3; i++)
	{
		vec1[i] = p2[i] - p1[i];
		vec2[i] = p3[i] - p2[i];
	}

	// calculate cross product of two vectors ( n= v1 x v2)
	n[0] = vec1[1] * vec2[2] - vec2[1] * vec1[2];
	n[1] = vec1[2] * vec2[0] - vec2[2] * vec1[0];
	n[2] = vec1[0] * vec2[1] - vec2[0] * vec1[1];
}

void Terrain::Draw()
{
	static float ang = 0.0;
	ang += 0.1f;
	glRotatef(ang * 2, 0, 1, 0);//spin about y-axis

	glBegin(GL_TRIANGLES);
	int counter = 0;
	bool once = true;
	for (int i = 0; i < numVerts; i++)
	{
		if (i == currentNumTriangles)
		{
			NormalVector(vertices[i], vertices[i + 2], vertices[i + 1], normal);
			currentNumTriangles += 3;
			glNormal3fv(normal);
			counter++;

		}
		glColor3fv(colors[i]);					//set tex
		glTexCoord2fv(textureCoordinates[i]);	//set coor
		glVertex3fv(vertices[i]);				//set vertices


	}
	currentNumTriangles = 0;
	int i = counter;
	glEnd();

}

void Terrain::Init()
{
	delete[] normals;
	normals = new vector[numVerts];
	//delete[] vertices;//just in case we've called init before
	vertices = new vector[numVerts];
	delete[] colors;
	colors = new vector[numVerts];
	delete[] textureCoordinates;
	textureCoordinates = new vector[numVerts];

	for (int i = 0; i < gridWidth; i++)
	{ //iterate left to right
		for (int j = 0; j < gridDepth; j++)
		{//iterate front to back
			color = image.getPixel(j, i);
			pixels[j][i] = ((color.r / 255.0f));
		}
	}

	//interpolate along the edges to generate interior points
	for (int i = 0; i < gridWidth - 1; i++)
	{ //iterate left to right	
		for (int j = 0; j < gridDepth - 1; j++)
		{//iterate front to back
			int sqNum = (j + i*gridDepth);
			int vertexNum = sqNum * 3 * 2; //6 vertices per square (2 tris)
			int firstVertexNum = vertexNum;
			float front = lerp(-terrDepth / 2, terrDepth / 2, (float)j / gridDepth);
			float back = lerp(-terrDepth / 2, terrDepth / 2, (float)(j + 1) / gridDepth);
			float left = lerp(-terrWidth / 2, terrWidth / 2, (float)i / gridDepth);
			float right = lerp(-terrDepth / 2, terrDepth / 2, (float)(i + 1) / gridDepth);

			/*
			back   +-----+	looking from above, the grid is made up of regular squares
			|tri1/|	'left & 'right' are the x cooded of the edges of the square
			|   / |	'back' & 'front' are the y coords of the square
			|  /  |	each square is made of two trianlges (1 & 2)
			| /   |
			|/tri2|
			front  +-----+
			left   right
			*/


			//tri1
			setPoint(colors[vertexNum], 1.f, 1.f, 1.f);
			setTexCoords(textureCoordinates[vertexNum], (float)i / (gridWidth / 4 - 2), (float)j / (gridDepth / 4 - 2));
			setPoint(vertices[vertexNum++], left, getHeight(j, i), front);
			//setPoint(vertices[vertexNum++], left, getHeight(left, front), front);


			setPoint(colors[vertexNum], 0.f, 1.f, 0.f);
			setTexCoords(textureCoordinates[vertexNum], (float)(i + 1) / (gridWidth / 4 - 2), (float)j / (gridDepth / 4 - 2));
			setPoint(vertices[vertexNum++], right, getHeight(j, i + 1), front);
			//setPoint(vertices[vertexNum++], right, getHeight(right, front), front);//bottom right

			setPoint(colors[vertexNum], 0.f, 0.f, 1.f);
			setTexCoords(textureCoordinates[vertexNum], (float)(i + 1) / (gridWidth / 4 - 2), (float)(j + 1) / (gridDepth / 4 - 2));
			setPoint(vertices[vertexNum++], right, getHeight(j + 1, i + 1), back);
			//setPoint(vertices[vertexNum++], right, getHeight(right, back), back); //top right

			//tri2
			setPoint(colors[vertexNum], 1.f, 0.f, 0.f);
			setTexCoords(textureCoordinates[vertexNum], (float)(i + 1) / (gridWidth / 4 - 2), (float)(j + 1) / (gridDepth / 4 - 2));
			setPoint(vertices[vertexNum++], right, getHeight(j + 1, i + 1), back);
			//setPoint(vertices[vertexNum++], right, getHeight(right, back), back);

			setPoint(colors[vertexNum], 0.f, 0.f, 1.f);
			setTexCoords(textureCoordinates[vertexNum], (float)i / (gridWidth / 4 - 2), (float)(j + 1) / (gridDepth / 4 - 2));
			setPoint(vertices[vertexNum++], left, getHeight(j + 1, i), back);
			//setPoint(vertices[vertexNum++], left, getHeight(left, back), back); //top left

			setPoint(colors[vertexNum], 0.f, 1.f, 0.f);
			setTexCoords(textureCoordinates[vertexNum], (float)i / (gridWidth / 4 - 2), (float)j / (gridDepth / 4 - 2));
			setPoint(vertices[vertexNum++], left, getHeight(j, i), front);
			//setPoint(vertices[vertexNum++], left, getHeight(left, front), front); //bottom left 
		}
	}
}
