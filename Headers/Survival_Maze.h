#pragma once

#include <stdlib.h>
#include <ctime>
#include "components/simple_scene.h"
#include "lab_m1/Survival_Maze/Headers/lab_camera.h"
#include "lab_m1/Survival_Maze/Headers/Player.h"

#define PROJECTILE_DURATION 4.f
#define PROJECTILES_SPEED 40.f
#define PROJECTILES_FREQUENCY 0.8f
#define DIM 30
#define DIF 2
#define PLAYER_SPEED 7.f
#define GROSIME_CELULA 5.f
#define ENEMY_SPEED 7.f
#define LIFETIME 60.f
#define WIDTH_ENEMY 1.5f

namespace m1
{
    struct Enemy
    {
        float moved = (GROSIME_CELULA - WIDTH_ENEMY) / 2.f; // distanta parcursa
        glm::vec3 position;
        uint8_t direction; /* 0 - x++ 
                              1 - z++
                              2 - x--
                              3 - z--
                           */
        bool alive;
        float health;
        Enemy(glm::vec3 pos)
        {
            moved = 0;
            health = 3;
            position = pos;
            alive = true;
            direction = 0;
        }
        Enemy()
        {
            moved = 0;
            health = 3;
            position = glm::vec3(0);
            alive = true;
            direction = 0;
        }
    };

    struct Projectile
    {
        glm::vec3 position;
        glm::vec3 direction;
        float lifetime;
        uint8_t health;
        Projectile()
        {
            lifetime = PROJECTILE_DURATION;
        }
        Projectile(glm::vec3 pos, glm::vec3 dir): Projectile()
        {
            position = pos;
            direction = dir;
        }
    };

    class Survival_Maze : public gfxc::SimpleScene
    {
    public:
        Survival_Maze();
        ~Survival_Maze();

        void Init() override;

    private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void RenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color);

        void CreateMesh(const char* name, const std::vector<VertexFormat>& vertices, const std::vector<unsigned int>& indices);
        void RenderMeshEnemy(const glm::mat4& modelMatrix);

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;

        void Shoot();
        void Draw(float);
        void Generate_maze(int maze[DIM][DIM], int current_area[4], bool vertical);
        void MoveEnemies(float deltaTimeSeconds);
        void Collisions();
        bool Player_Walls_Collision(glm::vec3) const;
    protected:
        implementedSurvival::Camera* camera;
        glm::mat4 projectionMatrix;
        bool renderCameraTarget;

        float projectilesFrequency = PROJECTILES_FREQUENCY;
        survivalMaze::Player *player;
        std::vector<Projectile> projectiles;
        std::vector<Enemy> enemies;

        // TODO(student): If you need any other class variables, define them here.
        int  flag1 = 1;
        bool flag2;
        bool flag3;
        float fov;
        float a;
        float zNearPO;
        float zFarPO;

        //ortho
        float left;
        float right;
        float bottom;
        float top;
        float width;
        float height;

        bool firstPerson = false;
        int maze[DIM][DIM];

        float angleHUDOX = 0;


        float increasedNumber = 0;
        float lifetime = LIFETIME;
        float misc = 0;

        bool gameOver = false;
    };
}   // namespace m1
