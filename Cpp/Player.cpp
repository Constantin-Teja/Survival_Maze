#include "../Headers/Player.h"
using namespace survivalMaze;

Player::Player()
{}

Player::Player(WindowObject* window, glm::vec3 pos): position(pos), angle(0.f), health(10)//position(glm::vec3(-4, 0, -4))
{
    for (uint8_t i = 0; i < 8; ++i)
        transformMatrices.push_back(glm::mat4(1));
    Move(glm::vec3(0));
}

/*
Player::Player(WindowObject* window): position(glm::vec3(0, 0, 0))
{
    Mesh* mesh = new Mesh("player");
    mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");
    meshes.push_back(mesh);
    for (uint8_t i = 0; i < 8; ++i)
        transformMatrices.push_back(glm::mat4(1));
    Move(glm::vec3(0));
}*/

Player::~Player()
{}

std::vector<glm::mat4> Player::getTransformMatrices() const
{
    return transformMatrices;
}

glm::vec3 Player::getPosition() const
{
    return position;
}

void Player::Move(glm::vec3 offset)
{
    position += offset;
    {
        // head
        transformMatrices[0] = glm::mat4(1);
        transformMatrices[0] = glm::translate(transformMatrices[0], position);
        transformMatrices[0] = glm::rotate(transformMatrices[0], angle, glm::vec3(0, 1, 0));
        transformMatrices[0] = glm::translate(transformMatrices[0], glm::vec3(0, 2.75f, 0));
        transformMatrices[0] = glm::scale(transformMatrices[0], glm::vec3(0.5f, 0.5f, 0.5f));
    }

    {
        // body
        transformMatrices[1] = glm::mat4(1);
        transformMatrices[1] = glm::translate(transformMatrices[1], position);
        transformMatrices[1] = glm::rotate(transformMatrices[1], angle, glm::vec3(0, 1, 0));
        transformMatrices[1] = glm::translate(transformMatrices[1], glm::vec3(0, 1.8f, 0));
        transformMatrices[1] = glm::scale(transformMatrices[1], glm::vec3(1.1f, 1.3f, 0.6f));
    }

    {
        // left arm
        transformMatrices[2] = glm::mat4(1);
        transformMatrices[2] = glm::translate(transformMatrices[2], position);
        transformMatrices[2] = glm::rotate(transformMatrices[2], angle, glm::vec3(0, 1, 0));
        transformMatrices[2] = glm::translate(transformMatrices[2], glm::vec3(-0.85f, 2.5f - 0.475f, 0));
        transformMatrices[2] = glm::scale(transformMatrices[2], glm::vec3(0.5f, 0.85f, 0.6f));
    }

    {
        // right arm
        transformMatrices[3] = glm::mat4(1);
        transformMatrices[3] = glm::translate(transformMatrices[3], position);
        transformMatrices[3] = glm::rotate(transformMatrices[3], angle, glm::vec3(0, 1, 0));

        transformMatrices[3] = glm::translate(transformMatrices[3], glm::vec3(+0.85f, 2.5f - 0.475f, 0));
        transformMatrices[3] = glm::scale(transformMatrices[3], glm::vec3(0.5f, 0.85f, 0.6f));
    }

    {
        // left hand
        transformMatrices[4] = glm::mat4(1);
        transformMatrices[4] = glm::translate(transformMatrices[4], position);
        transformMatrices[4] = glm::rotate(transformMatrices[4], angle, glm::vec3(0, 1, 0));
        transformMatrices[4] = glm::translate(transformMatrices[4], glm::vec3(-0.85f, 1.35f, 0));
        transformMatrices[4] = glm::scale(transformMatrices[4], glm::vec3(0.5f, 0.4f, 0.6f));
    }

    {
        // right hand
        transformMatrices[5] = glm::mat4(1);
        transformMatrices[5] = glm::translate(transformMatrices[5], position);
        transformMatrices[5] = glm::rotate(transformMatrices[5], angle, glm::vec3(0, 1, 0));
        transformMatrices[5] = glm::translate(transformMatrices[5], glm::vec3(+0.85f, 1.35f, 0));
        transformMatrices[5] = glm::scale(transformMatrices[5], glm::vec3(0.5f, 0.4f, 0.6f));
    }

    {
        // left leg
        transformMatrices[6] = glm::mat4(1);
        transformMatrices[6] = glm::translate(transformMatrices[6], position);
        transformMatrices[6] = glm::rotate(transformMatrices[6], angle, glm::vec3(0, 1, 0));
        transformMatrices[6] = glm::translate(transformMatrices[6], glm::vec3(-0.3f, 0.55f, 0));
        transformMatrices[6] = glm::scale(transformMatrices[6], glm::vec3(0.5f, 1.1f, 0.6f));
    }

    {
        // right leg
        transformMatrices[7] = glm::mat4(1);
        transformMatrices[7] = glm::translate(transformMatrices[7], position);
        transformMatrices[7] = glm::rotate(transformMatrices[7], angle, glm::vec3(0, 1, 0));
        transformMatrices[7] = glm::translate(transformMatrices[7], glm::vec3(0.3f, 0.55f, 0));
        transformMatrices[7] = glm::scale(transformMatrices[7], glm::vec3(0.5f, 1.1f, 0.6f));
    }
}

void Player::RotateOY(float angle)
{
    this->angle += angle;
}

float Player::getAngle() const
{
    return angle;
}

void Player::Hurt()
{
    --health;
}

uint8_t Player::getHealth() const
{
    return health;
}