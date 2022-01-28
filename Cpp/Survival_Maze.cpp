#include "lab_m1/Survival_Maze/Headers/Survival_Maze.h"

#include <vector>
#include <string>
#include <iostream>

using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */

Survival_Maze::Survival_Maze(): player(NULL)
{
    flag2 = true;
    flag3 = false;

    fov = RADIANS(60);
    a = window->props.aspectRatio;
    zNearPO = 0.01f;
    zFarPO = 200.0f;

    left = 14.9f;
    right = -10.54f;
    bottom = -12.8f;
    top = 16.7f;
    width = right - left;
    height = top - bottom;

    int current_area[4] = { 0, 0, DIM - 1, DIM - 1 };
    for (int i = 0; i < DIM; ++i)
        for (int j = 0; j < DIM; ++j)
        {
            if (i == 0 || i == DIM - 1 || j == 0 || j == DIM - 1)
                maze[i][j] = 1;
            else
                maze[i][j] = 0;
        }
    for (int i = 0; i < DIM / 10; ++i)
    {
        maze[rand() % DIM][0] = 0;
    }

    for (int i = 0; i < DIM / 10; ++i)
    {
        maze[rand() % DIM][DIM - 1] = 0;
    }

    for (int i = 0; i < DIM / 10; ++i)
    {
        maze[DIM - 1][rand() % DIM] = 0;
    }

    for (int i = 0; i < DIM / 10; ++i)
    {
        maze[DIM - 1][rand() % DIM] = 0;
    }
    while (player == NULL)
    {

        Generate_maze(maze, current_area, true);
        for (int i = 0; i < DIM; ++i)
        {
            for (int j = 0; j < DIM; ++j)
                cout << maze[i][j] << " ";
            cout << endl;
        }

        srand(static_cast<unsigned int>(std::time(nullptr)));
        int n = rand() % 10 + 1;
        for (int i = DIM / 5; i < DIM; ++i)
            for (int j = DIM / 5; j < DIM / 1.5f; ++j)
            {
                if (maze[i][j] == 0)
                    --n;
                if (n == 0)
                {
                    cout << "Maze generated" << endl;
                    player = new survivalMaze::Player(window, glm::vec3(i * GROSIME_CELULA, 0, j * GROSIME_CELULA));
                    i = DIM;
                    j = DIM;
                }

            }
    }

    cout << "\n\n-----------------------------------------------------------------------------------------------------\n\n" << endl;
    
    
}


Survival_Maze::~Survival_Maze()
{
}


void Survival_Maze::Init()
{
    renderCameraTarget = false;

    camera = new implementedSurvival::Camera();
    //camera->Set(glm::vec3(0, 3, 6.f), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));

    projectionMatrix = glm::perspective(RADIANS(60), window->props.aspectRatio, 0.01f, 200.0f);
    float offsetX, offsetY;
    
    {
        Mesh* mesh = new Mesh("box");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = new Mesh("enemy");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "sphere.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }
    
    {
        glm::vec3 playerPos = player->getPosition();
        camera->Set(glm::vec3(0, 3, 6.f) + glm::vec3(playerPos.x, 0, playerPos.z),
            glm::vec3(0, 1, 0) + glm::vec3(playerPos.x, 0, playerPos.z),
            glm::vec3(0, 1, 0));
        camera->RotateThirdPerson_OY(player->getAngle()); 
    }

    // Create a shader for enemies
    {
        Shader* shader = new Shader("Enemy");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Survival_Maze", "shaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Survival_Maze", "shaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    {
        // Createa enemies
        int n = 0;
        for(int i=0; i<DIM; ++i)
            for (int j = 0; j < DIM; ++j)
            {
                if (maze[i][j] == 0)
                {
                    if (n % 15 == 0)
                    {
                        enemies.push_back(Enemy(glm::vec3(i * GROSIME_CELULA - GROSIME_CELULA/2.f + WIDTH_ENEMY/2, 2, j * GROSIME_CELULA - GROSIME_CELULA/2.f + WIDTH_ENEMY / 2)));
                        n = 3;
                    }
                    n++;
                }
                    
            }
    }
}


void Survival_Maze::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}

//  DE INFRUMUSETAT!!!
struct ViewportSpace
{
    ViewportSpace() : x(0), y(0), width(1), height(1) {}
    ViewportSpace(int x, int y, int width, int height)
        : x(x), y(y), width(width), height(height) {}
    int x;
    int y;
    int width;
    int height;
};

void SetViewportArea(const ViewportSpace& viewSpace, glm::vec3 colorColor, bool clear)
{
    glViewport(viewSpace.x, viewSpace.y, viewSpace.width, viewSpace.height);

    glEnable(GL_SCISSOR_TEST);
    glScissor(viewSpace.x, viewSpace.y, viewSpace.width, viewSpace.height);

    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(colorColor.r, colorColor.g, colorColor.b, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);

}
// SF de infrumuesetare

void Survival_Maze::Update(float deltaTimeSeconds)
{
    if (gameOver)
        return;
    if (player->getPosition().x > DIM * GROSIME_CELULA || player->getPosition().z > DIM * GROSIME_CELULA || player->getPosition().z < 0 || player->getPosition().x < 0)
    {
        cout << "\n\n\n\t\t---WIN---\n\n" << endl;
        gameOver = true;
        return;
    }
    if (lifetime < 0 || player->getHealth() == 0)
    {
        gameOver = true;
        cout << "\n\n\t\t---GAME OVER! YOU LOST!---\n\n" << endl;
        return;
    }

    lifetime -= deltaTimeSeconds;
    MoveEnemies(deltaTimeSeconds);
    Draw(deltaTimeSeconds);
    Collisions();
}


void Survival_Maze::FrameEnd()
{
    DrawCoordinateSystem(camera->GetViewMatrix(), projectionMatrix);
}

// pot sa scot parametrul de shader pentru ca il ia pe cel de Color
void Survival_Maze::RenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color)
{
    shader = shaders.at("Color");

    if (!mesh || !shader || !shader->program)
        return;

    // Render an object using the specified shader and the specified position
    shader->Use();
    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));
    glUniform3f(shader->GetUniformLocation("color"), color.r, color.g, color.b);
    
    mesh->Render();
}

void Survival_Maze::RenderMeshEnemy(const glm::mat4& modelMatrix)
{

    if (!meshes["enemy"] || !shaders["Enemy"] || !shaders["Enemy"]->program)
        return;

    // Render an object using the specified shader and the specified position
    shaders["Enemy"]->Use();
    glUniformMatrix4fv(shaders["Enemy"]->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
    glUniformMatrix4fv(shaders["Enemy"]->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(shaders["Enemy"]->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    int vertexColorLocation = glGetUniformLocation(shaders["Enemy"]->program, "VertexColor");

    glUniform3fv(vertexColorLocation, 1, glm::value_ptr(glm::vec3(1, 0.2f, 0.2f)));
    meshes["enemy"]->Render();
}

/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Survival_Maze::OnInputUpdate(float deltaTime, int mods)
{
    // move the camera only if MOUSE_RIGHT button is pressed
    if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
    {
        float cameraSpeed = PLAYER_SPEED;
        if (window->KeyHold(GLFW_KEY_W)) {
            glm::vec3 dir = glm::normalize(glm::vec3(camera->forward.x, 0, camera->forward.z));
            if (!Player_Walls_Collision(cameraSpeed * deltaTime * dir))
            {
                camera->MoveForward(cameraSpeed * deltaTime);
                player->Move(cameraSpeed * deltaTime * dir);
            }
        }

        if (window->KeyHold(GLFW_KEY_A)) {
            glm::vec3 dir = glm::normalize(glm::vec3(camera->right.x, 0, camera->right.z));
            if (!Player_Walls_Collision(-cameraSpeed * deltaTime * dir))
            {
                camera->TranslateRight(-cameraSpeed * deltaTime);
                player->Move(-cameraSpeed * deltaTime * dir);
            }
        }

        if (window->KeyHold(GLFW_KEY_S)) {
            glm::vec3 dir = glm::normalize(glm::vec3(camera->forward.x, 0, camera->forward.z));
            if (!Player_Walls_Collision(-cameraSpeed * deltaTime * dir))
            {
                camera->MoveForward(-cameraSpeed * deltaTime);
                player->Move(-cameraSpeed * deltaTime * dir);
            }
        }

        if (window->KeyHold(GLFW_KEY_D)) {
            glm::vec3 dir = glm::normalize(glm::vec3(camera->right.x, 0, camera->right.z));
            if (!Player_Walls_Collision(cameraSpeed * deltaTime * dir))
            {
                camera->TranslateRight(cameraSpeed * deltaTime);
                player->Move(cameraSpeed * deltaTime * dir);
            }

        }

        if (window->KeyHold(GLFW_KEY_Q)) {
            camera->TranslateUpward(-cameraSpeed * deltaTime);
        }

        if (window->KeyHold(GLFW_KEY_E)) {
            camera->TranslateUpward(cameraSpeed * deltaTime);
        }

        if (flag2)
        {
            if (window->KeyHold(GLFW_KEY_1)) {

                fov += RADIANS(3);
                projectionMatrix = glm::perspective(fov, a, zNearPO, zFarPO);
            }

            if (window->KeyHold(GLFW_KEY_2)) {

                fov -= RADIANS(3);
                projectionMatrix = glm::perspective(fov, a, zNearPO, zFarPO);
            }
        }
        if (flag3)
        {

            if (window->KeyHold(GLFW_KEY_3)) {

                width++;
                right = width + left;
                projectionMatrix = glm::ortho(left, right, bottom, top, zNearPO, zFarPO);
            }
            if (window->KeyHold(GLFW_KEY_4)) {
                width--;
                right = width + left;
                projectionMatrix = glm::ortho(left, right, bottom, top, zNearPO, zFarPO);
            }
            if (window->KeyHold(GLFW_KEY_5)) {
                height++;
                top = height + bottom;
                projectionMatrix = glm::ortho(left, right, bottom, top, zNearPO, zFarPO);
            }
            if (window->KeyHold(GLFW_KEY_6)) {
                height--;
                top = height + bottom;
                projectionMatrix = glm::ortho(left, right, bottom, top, zNearPO, zFarPO);
            }
        }
    }

    // TODO(student): Change projection parameters. Declare any extra
    // variables you might need in the class header. Inspect this file
    // for any hardcoded projection arguments (can you find any?) and
    // replace them with those extra variables.

}


void Survival_Maze::OnKeyPress(int key, int mods)
{
    if (key == GLFW_KEY_T)
    {
        renderCameraTarget = !renderCameraTarget;
    }
    if (key == GLFW_KEY_P)
    {
        projectionMatrix = glm::perspective(fov, a, zNearPO, zFarPO);
        flag2 = true;
        flag3 = false;
    }
    if (key == GLFW_KEY_O)
    {
        projectionMatrix = glm::ortho(left, right, bottom, top, zNearPO, zFarPO);
        flag2 = false;
        flag3 = true;
    }
    if (window->KeyHold(GLFW_KEY_Z) && flag1 == 1)
    {
        camera->Set(glm::vec3(0, 2, 3.5f), glm::vec3(1, 0, 0), glm::vec3(0, 1, 0));
        camera->RotateFirstPerson_OY(0.3);
        camera->TranslateRight(-3);
        flag1 = 0;
    }
    else
        if (window->KeyHold(GLFW_KEY_Z) && flag1 == 0)
        {
            camera->Set(glm::vec3(0, 2, 3.5f), glm::vec3(0, 0, 1), glm::vec3(0, 1, 0));
            camera->RotateFirstPerson_OY(0.3);
            camera->TranslateRight(6);
            flag1 = 1;
        }
}


void Survival_Maze::OnKeyRelease(int key, int mods)
{
    if(key == GLFW_KEY_LEFT_CONTROL)
    {
        /*
        * This should be enough
        * camera->MoveForward(-6.f);
        */
        glm::vec3 playerPos = player->getPosition();
        camera->Set(glm::vec3(0, 3, 6.f) + glm::vec3(playerPos.x, 0, playerPos.z),
            glm::vec3(0, 1, 0) + glm::vec3(playerPos.x, 0, playerPos.z),
            glm::vec3(0, 1, 0));
        camera->RotateThirdPerson_OY(player->getAngle());
        firstPerson = false;

        angleHUDOX = 0;
    }
}


void Survival_Maze::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
    {
        float sensivityOX = 0.001f;
        float sensivityOY = 0.001f;

        if (window->GetSpecialKeyState() == 0) {
            renderCameraTarget = true;
            camera->RotateThirdPerson_OX(-deltaY * sensivityOY);
            camera->RotateThirdPerson_OY(-deltaX * sensivityOX);
            player->RotateOY(-deltaX * sensivityOX);
            /*renderCameraTarget = false;
            camera->RotateFirstPerson_OX(-deltaY * sensivityOY);
            camera->RotateFirstPerson_OY(-deltaX * sensivityOX);
            camera->RotateThirdPerson_OX(-deltaY * sensivityOY);
            camera->RotateThirdPerson_OY(-deltaX * sensivityOX);*/
            angleHUDOX += -deltaY * sensivityOY;
        }

        if (window->GetSpecialKeyState() & GLFW_MOD_CONTROL) {
            if (!firstPerson)
            {
                camera->TranslateForward(6.6f);
                camera->position += glm::vec3(0, 1.8f, 0);
                //camera->MoveForward(0.5f);
                firstPerson = true;
            }
            
            // TODO(student): Rotate the camera in third-person mode around
            // OX and OY using `deltaX` and `deltaY`. Use the sensitivity
            // variables for setting up the rotation speed.
            /*camera->RotateThirdPerson_OX(-deltaY * sensivityOY);
            camera->RotateThirdPerson_OY(-deltaX * sensivityOX);*/
            
            camera->TranslateForward(-0.6f);
            camera->RotateFirstPerson_OY(-deltaX * sensivityOX);
            camera->TranslateForward(0.6f);

            camera->TranslateForward(-0.6f);
            camera->RotateFirstPerson_OX(-deltaY * sensivityOY);
            camera->TranslateForward(0.6f);

            //camera->RotateFirstPerson_OX(-deltaY * sensivityOY);
            //camera->RotateFirstPerson_OY(-deltaX * sensivityOX);
            
            angleHUDOX += -deltaY * sensivityOY;
            player->RotateOY(-deltaX * sensivityOX);
            //camera->RotateThirdPerson_OX(-deltaY * sensivityOY);
            //camera->RotateThirdPerson_OY(-deltaX * sensivityOX);
        }
        player->Move(glm::vec3(0));
    }
}


void Survival_Maze::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    if ((button == 1) && firstPerson)
    {
        Shoot();
    }
}


void Survival_Maze::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Survival_Maze::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Survival_Maze::OnWindowResize(int width, int height)
{
}

void Survival_Maze::Shoot()
{
    if (projectilesFrequency <= 0.f)
    {
        projectilesFrequency = PROJECTILES_FREQUENCY;
        projectiles.push_back(Projectile(camera->position, glm::normalize(camera->forward)));
    }
}

void Survival_Maze::Draw(float deltaTime) // NU CRED CA ARE NEVOIE DE deltaTime
{
    {
        // Player
        if (!firstPerson)
        {
            std::vector<glm::mat4> modelMatrix = player->getTransformMatrices();
            RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix[0], glm::vec3(1, 0.9f, 0.65f));
            RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix[1], glm::vec3(0, 1, 0));
            RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix[2], glm::vec3(0, 1, 0));
            RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix[3], glm::vec3(0, 1, 0));
            RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix[4], glm::vec3(1, 0.9f, 0.65f));
            RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix[5], glm::vec3(1, 0.9f, 0.65f));
            RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix[6], glm::vec3(0, 0, 1));
            RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix[7], glm::vec3(0, 0, 1));
        }
    }

    {
        // Maze
        glm::mat4 modelMatrix;
        modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(DIM/2 * GROSIME_CELULA, 0.01f, DIM/2 * GROSIME_CELULA));
        modelMatrix = glm::scale(modelMatrix, glm::vec3((DIM+1) * GROSIME_CELULA, 0.01f, (DIM + 1) * GROSIME_CELULA));
        RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix, glm::vec3(1));
        for (int i = 0; i < DIM; ++i)
        {
            for (int j = 0; j < DIM; ++j)
            {
                if (maze[i][j] == 1)
                {
                    modelMatrix = glm::mat4(1);
                    modelMatrix = glm::translate(modelMatrix, glm::vec3(i * GROSIME_CELULA, 4, j * GROSIME_CELULA));
                    modelMatrix = glm::scale(modelMatrix, glm::vec3(GROSIME_CELULA, 8, GROSIME_CELULA));
                    RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix, glm::vec3(0, 0, 1));
                }
            }
        }
    }

    {
        // Projectiles
        for(auto& i: projectiles)
        {
            i.position += glm::vec3(deltaTime * PROJECTILES_SPEED * i.direction.x,
                deltaTime * PROJECTILES_SPEED * i.direction.y,
                deltaTime * PROJECTILES_SPEED * i.direction.z);
            glm::mat4 modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, i.position);
            modelMatrix = glm::rotate(modelMatrix, player->getAngle(), glm::vec3(0, 1, 0));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f, 0.2f, 0.2f));
            RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix, glm::vec3(1, 0, 0));
            i.lifetime -= deltaTime;
        }
        if(!projectiles.empty())
            if (projectiles[0].lifetime <= 0)
                projectiles.erase(projectiles.begin());
        if(projectilesFrequency > 0.f)
            projectilesFrequency -= deltaTime;
    }

    {
        // HUD
        {
            // Hud framework
            glm::mat4 modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, camera->position + camera->forward * 12.f + camera->up * -10.f + camera->right * 2.3f);
            modelMatrix = glm::rotate(modelMatrix, player->getAngle(), glm::vec3(0, 1, 0));
            modelMatrix = glm::rotate(modelMatrix, angleHUDOX, glm::vec3(1, 0, 0));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(20, 6, 0.01f));
            //modelMatrix = glm::translate(modelMatrix, glm::vec3(0)); // PROBABIL LINIE INUTILA

            projectionMatrix = glm::ortho(left, right, bottom, top, zNearPO, zFarPO);
            RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix, glm::vec3(0.3f, 1.f, 0.3f));
            projectionMatrix = glm::perspective(fov, a, zNearPO, zFarPO);
        }
        
        {
            // Health Bar
            glm::mat4 modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, camera->position + camera->forward * 8.f + camera->up * -9.f + camera->right * 2.3f);
            modelMatrix = glm::rotate(modelMatrix, player->getAngle(), glm::vec3(0, 1, 0));
            modelMatrix = glm::rotate(modelMatrix, angleHUDOX, glm::vec3(1, 0, 0));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(20 * player->getHealth() / 10.f, 1.5f, 0.01f));
            //modelMatrix = glm::translate(modelMatrix, glm::vec3(0)); // PROBABIL LINIE INUTILA

            projectionMatrix = glm::ortho(left, right, bottom, top, zNearPO, zFarPO);
            RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix, glm::vec3(1, 0, 0));
            projectionMatrix = glm::perspective(fov, a, zNearPO, zFarPO);
        }
        
        {
            //Time Bar
            glm::mat4 modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, camera->position + camera->forward * 8.f + camera->up * -11.f + camera->right * 2.3f);
            modelMatrix = glm::rotate(modelMatrix, player->getAngle(), glm::vec3(0, 1, 0));
            modelMatrix = glm::rotate(modelMatrix, angleHUDOX, glm::vec3(1, 0, 0));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(20*lifetime/60.f, 1.5f, 0.01f));
            //modelMatrix = glm::translate(modelMatrix, glm::vec3(0)); // PROBABIL LINIE INUTILA

            projectionMatrix = glm::ortho(left, right, bottom, top, zNearPO, zFarPO);
            RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix, glm::vec3(0, 0, 0));
            projectionMatrix = glm::perspective(fov, a, zNearPO, zFarPO);
        }

        {
            // Crosshair
            if (firstPerson)
            {
                glm::mat4 modelMatrix = glm::mat4(1);
                modelMatrix = glm::translate(modelMatrix, camera->position + camera->forward * 1.f + camera->up * 1.9f + camera->right * 2.17f);
                modelMatrix = glm::rotate(modelMatrix, player->getAngle(), glm::vec3(0, 1, 0));
                modelMatrix = glm::rotate(modelMatrix, angleHUDOX, glm::vec3(1, 0, 0));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f, 0.3f, 0.01f));
                modelMatrix = glm::translate(modelMatrix, glm::vec3(0)); // PROBABIL LINIE INUTILA

                projectionMatrix = glm::ortho(left, right, bottom, top, zNearPO, zFarPO);
                RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix, glm::vec3(0, 0, 0));
                projectionMatrix = glm::perspective(fov, a, zNearPO, zFarPO);
            }
        }
    }

    {
        // Enemies
        glm::mat4 modelMatrix;
        for (auto& i : enemies)
        {
            modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, i.position);
            modelMatrix = glm::scale(modelMatrix, glm::vec3(WIDTH_ENEMY, 4, WIDTH_ENEMY));
            modelMatrix = glm::translate(modelMatrix, glm::vec3(0)); // PROBABIL LINIE INUTILA

            if (i.alive)
            {
                glm::vec3 color;
                if (i.health == 3)
                    color = glm::vec3(0.5, 0.5, 0.5);
                if (i.health == 2)
                    color = glm::vec3(0.7, 0.8, 0.1);
                if (i.health == 1)
                    color = glm::vec3(1, 0, 0);
                RenderMesh(meshes["enemy"], shaders["VertexNormal"], modelMatrix, color);
            }
            else
                RenderMeshEnemy(modelMatrix);
        }
    }
}

void Survival_Maze::Generate_maze(int maze[DIM][DIM], int current_area[4], bool vertical)
{
    if (((current_area[2] - current_area[0]) <= 2 * DIF) && ((current_area[3] - current_area[1]) <= 2 * DIF))
        return;
    if (((current_area[2] - current_area[0]) <= 2 * DIF) && !vertical)
    {
        Generate_maze(maze, current_area, true);
        return;
    }
    if (((current_area[3] - current_area[1]) <= 2 * DIF) && vertical)
    {
        Generate_maze(maze, current_area, !vertical);
        return;
    }

    int half_areas[2][4];
    srand(static_cast<unsigned int>(std::time(nullptr)));
    if (vertical)
    {
        int column = rand() % (current_area[3] - current_area[1] - (2 * DIF)) + current_area[1] + DIF;
        for (int i = current_area[0]; i <= current_area[2]; ++i)
        {
            maze[i][column] = 1;
        }
        maze[rand() % (current_area[2] - current_area[0] - 1) + current_area[0] + 1][column] = 0;
        half_areas[0][0] = current_area[0];
        half_areas[0][1] = current_area[1];
        half_areas[0][2] = current_area[2];
        half_areas[0][3] = column;

        half_areas[1][0] = current_area[0];
        half_areas[1][1] = column;
        half_areas[1][2] = current_area[2];
        half_areas[1][3] = current_area[3];
        maze[rand() % (current_area[2] - current_area[0] - 1) + current_area[0] + 1][column] = 0;
    }
    else
    {
        int row = rand() % (current_area[2] - current_area[0] - (2 * DIF)) + current_area[0] + DIF;
        for (int i = current_area[1]; i <= current_area[3]; ++i)
        {
            maze[row][i] = 1;
        }
        maze[row][rand() % (current_area[3] - current_area[1] - 1) + current_area[1] + 1] = 0;
        half_areas[0][0] = current_area[0];
        half_areas[0][1] = current_area[1];
        half_areas[0][2] = row;
        half_areas[0][3] = current_area[3];

        half_areas[1][0] = row;
        half_areas[1][1] = current_area[1];
        half_areas[1][2] = current_area[2];
        half_areas[1][3] = current_area[3];
        maze[row][rand() % (current_area[3] - current_area[1] - 1) + current_area[1] + 1] = 0;
    }
    Generate_maze(maze, half_areas[0], !vertical);
    Generate_maze(maze, half_areas[1], !vertical);
}

inline float distance(glm::vec3 pos1, glm::vec3 pos2)
{
    return sqrt(((pos1.x - pos2.x) * (pos1.x - pos2.x)) + ((pos1.z - pos2.z) * (pos1.z - pos2.z)));
}

bool Survival_Maze::Player_Walls_Collision(glm::vec3 offset) const
{
    for (int i = 0; i < DIM; ++i)
    {
        for (int j = 0; j < DIM; ++j)
        {
            if(maze[i][j] == 1)
                if (distance(player->getPosition() + offset, glm::vec3(i* GROSIME_CELULA, 0, j* GROSIME_CELULA)) < 8*GROSIME_CELULA/10)
                    return true;
        }
    }

    return false;
}

// Posibil sa nu fie nevoie
void Survival_Maze::CreateMesh(const char* name, const std::vector<VertexFormat>& vertices, const std::vector<unsigned int>& indices)
{
    unsigned int VAO = 0;
    // Create the VAO and bind it
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Create the VBO and bind it
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Send vertices data into the VBO buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

    // Create the IBO and bind it
    unsigned int IBO;
    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

    // Send indices data into the IBO buffer
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);

    // ========================================================================
    // This section demonstrates how the GPU vertex shader program
    // receives data.

    // TODO(student): If you look closely in the `Init()` and `Update()`
    // functions, you will see that we have three objects which we load
    // and use in three different ways:
    // - LoadMesh   + LabShader (this lab's shader)
    // - CreateMesh + VertexNormal (this shader is already implemented)
    // - CreateMesh + LabShader (this lab's shader)
    // To get an idea about how they're different from one another, do the
    // following experiments. What happens if you switch the color pipe and
    // normal pipe in this function (but not in the shader)? Now, what happens
    // if you do the same thing in the shader (but not in this function)?
    // Finally, what happens if you do the same thing in both places? Why?

    // Set vertex position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), 0);

    // Set vertex normal attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(sizeof(glm::vec3)));

    // Set texture coordinate attribute
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(2 * sizeof(glm::vec3)));

    // Set vertex color attribute
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(2 * sizeof(glm::vec3) + sizeof(glm::vec2)));
    // ========================================================================

    // Unbind the VAO
    glBindVertexArray(0);

    // Check for OpenGL errors
    CheckOpenGLError();

    // Mesh information is saved into a Mesh object
    meshes[name] = new Mesh(name);
    meshes[name]->InitFromBuffer(VAO, static_cast<unsigned int>(indices.size()));
    meshes[name]->vertices = vertices;
    meshes[name]->indices = indices;
}

void Survival_Maze::MoveEnemies(float deltaTimeSeconds)
{
    for (std::vector<Enemy>::iterator it = enemies.begin();
        it != enemies.end();)
    {
        it->moved += ENEMY_SPEED * deltaTimeSeconds;
        if (it->alive)
        {
            switch (it->direction)
            {
            case 0:
                it->position.x += ENEMY_SPEED * deltaTimeSeconds;
                break;
            case 1:
                it->position.z += ENEMY_SPEED * deltaTimeSeconds;
                break;
            case 2:
                it->position.x -= ENEMY_SPEED * deltaTimeSeconds;
                break;
            case 3:
                it->position.z -= ENEMY_SPEED * deltaTimeSeconds;
                break;
            }
            it->direction += (it->moved > GROSIME_CELULA - WIDTH_ENEMY) ? (it->moved = 0, 1) : 0;
            it->direction = it->direction % 4;
        }

        bool flag = true;
        if (it->alive == false)
        {
            it->health += deltaTimeSeconds;
            if (it->health >= 1) {
                it = enemies.erase(it);
                flag = false;
            }
        }
        if (flag)
            ++it;
    }
}

void Survival_Maze::Collisions()
{
    // Bullets - Enemies
    for (std::vector<Projectile>::iterator it = projectiles.begin(); it != projectiles.end();)
    {
        bool flag = true;
        for (std::vector<Enemy>::iterator itEnemy = enemies.begin(); itEnemy != enemies.end(); ++itEnemy)
        {
            if (distance(it->position, itEnemy->position) < 1.3f)
            {
                it = projectiles.erase(it);
                if(itEnemy->alive)
                itEnemy->health--;
                if(itEnemy->health == 0)
                    itEnemy->alive = false;
                flag = false;
                break;
            }
        }
        if (flag)
            ++it;
    }
    
    // Player - Enemies
    for (std::vector<Enemy>::iterator itEnemy = enemies.begin(); itEnemy != enemies.end();)
    {
        if (distance(player->getPosition(), itEnemy->position) < 2.f)
        {
            itEnemy->alive = false;
            itEnemy = enemies.erase(itEnemy);
            player->Hurt();
        }
        else
            ++itEnemy;
    }
}
