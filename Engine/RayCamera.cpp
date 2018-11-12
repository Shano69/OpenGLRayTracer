#include "RayCamera.h"

RayCamera::RayCamera(float aspect , float aperture , glm::vec3 startPosition, glm::vec3 startUp , GLfloat startYaw , GLfloat startPitch , GLfloat startMoveSpeed , GLfloat StartTurnSpeed )
{
	cameraMoved = false;

	position = startPosition;
	worldUp = startUp;
	yaw = startYaw;
	pitch = startPitch;
	front = glm::vec3(-1.0f, 1.0f, 1.0f);
	
	update();

	lens_radius = aperture / 2;
	float theta = 20 * M_PI / 180.0;
	half_height = tan(theta / 2);
	half_width = aspect * half_height;


	moveSpeed = startMoveSpeed;
	turnSpeed = StartTurnSpeed;

}

Ray RayCamera::get_ray(float u, float v)
{
	return(Ray(position, lower_left_corner + u * horizontal + v * vertical - position));
}

void RayCamera::keyControl(bool * keys, GLfloat deltaTime)
{
	GLfloat velocity = 0.5;

	if (keys[GLFW_KEY_S])
	{
		position += front * velocity;
	}

	if (keys[GLFW_KEY_W])
	{
		position -= front * velocity;
	}

	if (keys[GLFW_KEY_A])
	{
		position -= right * velocity;
	}

	if (keys[GLFW_KEY_D])
	{
		position += right * velocity;
	}
	if (keys[GLFW_KEY_SPACE])
	{
		position.x +=  velocity;
	}

	if (keys[GLFW_KEY_C])
	{
		position.x -= velocity;
	}
	update();
}

void RayCamera::mouseControl(GLfloat xChange, GLfloat yChange)
{
	xChange *= turnSpeed;
	yChange *= turnSpeed;

	yaw -= yChange;
	pitch += xChange;
	
	update();
}

void RayCamera::update()
{
	//Determine the front of the camera using the angles of rotation
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

	front = glm::normalize(front);

	right = glm::normalize(glm::cross(front, worldUp));
	up = glm::normalize(glm::cross(right, front));

	lower_left_corner = position - half_width * right - half_height * up - front;
	horizontal = 2 * half_width*right;
	vertical = 2 * half_height*up;

}

