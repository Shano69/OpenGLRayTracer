#ifndef RAYCAMERA
#define RAYCAMERA

#define M_PI 3.14159265358979323846

#include "Ray.h"
#include <iostream>
#include "glm/ext.hpp"
class RayCamera
{
public:

	//constructor for the camera
	RayCamera(
		float aspect = float(180) / float(180),
		float aperture = 0.1,
		glm::vec3 startPosition = glm::vec3(0.0, 0.0f, -20.0),
		glm::vec3 startUp = glm::vec3(1.0f, 0.0f, 0.0f),
		GLfloat startYaw = -90.0f,
		GLfloat startPitch = 0.0f, 
		GLfloat startMoveSpeed = 6.0f,
		GLfloat StartTurnSpeed = 0.4f
	);

	//get a ray from the camera current possition
	Ray get_ray(float u, float v);

	//handle camera possition methods
	void keyControl(bool* keys, GLfloat deltaTime);
	void mouseControl(GLfloat xChange, GLfloat yChange);

	//methods for determining the clearing of the sampler
	inline bool getCameraMoved() { return cameraMoved; }
	inline void setMoved(bool move) { cameraMoved = move; }
private:

	bool cameraMoved;
	glm::vec3 lower_left_corner;
	glm::vec3 horizontal;
	glm::vec3 vertical;
	glm::vec3 u, v, w;
	float lens_radius;
	float half_height;
	float half_width;

	// new Camera
	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 worldUp;

	GLfloat yaw;
	GLfloat pitch;

	GLfloat moveSpeed;
	GLfloat turnSpeed;

	void update();

};

#endif // !RayCamera

