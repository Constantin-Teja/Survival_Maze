#pragma once
#include <stdint.h>
#include "components/simple_scene.h"

namespace survivalMaze
{
	class Player
	{
		uint8_t health;
		glm::vec3 position;
		std::vector<glm::mat4> transformMatrices;
		float angle;
	public:
		Player();
		Player(WindowObject*, glm::vec3);
		~Player();
		void Move();
		void Die();
		void Draw();
		void Move(glm::vec3);
		void RotateOY(float);
		void Hurt();
		glm::vec3 getPosition() const;
		std::vector<glm::mat4> getTransformMatrices() const;
		float getAngle() const;
		uint8_t getHealth() const;

	};
}
