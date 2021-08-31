#include "transformable.hpp"
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

glm::mat4 transformable::transform() const
	{
		glm::quat orientation = glm::angleAxis(glm::radians(pitch), glm::vec3(1.f, 0.f, 0.f));
		orientation *= glm::angleAxis(glm::radians(yaw), glm::vec3(0.f, 0.f, 1.f));
		orientation *= glm::angleAxis(glm::radians(roll), glm::vec3(0.f, 1.f, 0.f));

		glm::mat4 transform(1.f);
		transform = glm::translate(transform, position);
		transform *= glm::toMat4(orientation);

		return transform;
	}
