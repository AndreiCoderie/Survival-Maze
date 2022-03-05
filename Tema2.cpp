#include "lab_m1/Tema2/Tema2.h"

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <cmath>
#include <lab_m1/lab4/transform3D.h>
#include <lab_m1/Tema2/transform2D.h>
#include <lab_m1/Tema2/object2D.h>
#include <string>




using namespace std;
using namespace m1;
using std::ifstream;

/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */



Tema2::Tema2()
{
}


Tema2::~Tema2()
{
}


void Tema2::Init()
{
    angle = 0;
    condition4 = true;
    cond1 = true;
    deformare = 0;
    randomIndex = 0;
    timeX = 0;
    scaleX = 0;
    life = 100;
    timer = 0.5;


    mazeX = 0;
    mazeY = 0.2;
    mazeZ = 0;

    translateX = 0;
    translateY = 0;
    translateZ = 0;
    renderCameraTarget = false;
    playerAngle = 0;
    leftO = 10.0f;
    rightO = 0.5f ;
    bottom = 0.5f;
    top = 10.0f;
    anglePerspective = 60;
    checkPerspOrtho = 0;
    attack = false;

    camera = new implemented::Camera();

    {
        Mesh* mesh = new Mesh("box");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = new Mesh("plane50");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "plane50.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = new Mesh("sphere");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "sphere.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }


    {
        Shader* shader = new Shader("TemaShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    {
        Shader* shader= new Shader("EnemyShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "EnemyShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    {
        Shader* shader = new Shader("ShaderHud");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "VertexShaderHUD.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "FragmentShaderHUD.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    {
        Mesh* liveSquareFillTrue = object2D::CreateDrept("liveSquareFillTrue", glm::vec3(0, 0, -1), 5, 20, glm::vec3(1,0,1), true);
        AddMeshToList(liveSquareFillTrue);
    }

    // TODO(student): After you implement the changing of the projection
    // parameters, remove hardcodings of these parameters
    projectionMatrix = glm::perspective(RADIANS(60), window->props.aspectRatio, 0.01f, 200.0f);

    //initializez intai matricea cu 0
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            mazeMatrix[i][j] = 0;

        }
    }
    readFromTexts();

    camera->Set(glm::vec3(0, 2, 2), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));
    randomIndex = rand() % freeSpaces.size();
    camera->position = glm::vec3(freeSpaces[randomIndex].x, 1.75, freeSpaces[randomIndex].z + 1.8);
    

}



void Tema2::drawProjectiles(glm::vec3 positionToCamera) {

    Projectile proj(positionToCamera.x, positionToCamera.y, positionToCamera.z, camera->forward.x, camera->forward.y, camera->forward.z );
    projectiles.push_back(proj);

}

void Tema2::shootPrjectile(float deltaTimeSeconds, glm::vec3 translateToCamera) {
    if (projectiles.size() > 2) {
        projectiles.erase(projectiles.begin() + 0);
    }
    for (int i = 0; i < projectiles.size(); i++) {
        projectiles[i].translateX += projectiles[i].directionX * deltaTimeSeconds * projectiles[i].speed;
        projectiles[i].translateY += projectiles[i].directionY * deltaTimeSeconds * projectiles[i].speed;
        projectiles[i].translateZ += projectiles[i].directionZ * deltaTimeSeconds * projectiles[i].speed;

        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(projectiles[i].initialX + projectiles[i].translateX , projectiles[i].initialY + projectiles[i].translateY, 
            projectiles[i].initialZ + projectiles[i].translateZ));
   
        modelMatrix *= glm::scale(glm::mat4(1), glm::vec3(0.05, 0.05, 0.05));

        if (projectiles[i].translateX > 4 || projectiles[i].translateZ > 4 | projectiles[i].translateY > 4) {
            projectiles[i].isDead = true;
        }


        RenderSimpleMesh(meshes["sphere"], shaders["TemaShader"], modelMatrix, glm::vec3(1,1,0));

        checkProjectileEnemyCollision(projectiles[i]);



       
    }
}

void Tema2::drawCharacter2(glm::vec3 translateToCamera, float angle) {

    //tricou
    {

        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, translateToCamera);
        modelMatrix = glm::rotate(modelMatrix, angle, glm::vec3(0, 1, 0));
        modelMatrix *= glm::scale(glm::mat4(1), glm::vec3(0.4, 0.4, 0.2));



        RenderSimpleMesh(meshes["box"], shaders["TemaShader"], modelMatrix, BLUE);
    }

    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, translateToCamera);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.3, 0.1, 0));

        modelMatrix = glm::translate(modelMatrix, glm::vec3(0.3, -0.1, 0));
        modelMatrix = glm::rotate(modelMatrix, angle, glm::vec3(0, 1, 0));
        modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.3, 0.1, 0));

        modelMatrix *= glm::scale(glm::mat4(1), glm::vec3(0.2, 0.2, 0.2));


        RenderSimpleMesh(meshes["box"], shaders["TemaShader"], modelMatrix, BLUE);
    }

    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, translateToCamera);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0.3, 0.1, 0));

        modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.3, -0.1, 0));
        modelMatrix = glm::rotate(modelMatrix, angle, glm::vec3(0, 1, 0));
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0.3, 0.1, 0));

        modelMatrix *= glm::scale(glm::mat4(1), glm::vec3(0.2, 0.2, 0.2));


        RenderSimpleMesh(meshes["box"], shaders["TemaShader"], modelMatrix, BLUE);
    }

    //maini

    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, translateToCamera);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.3, -0.1, 0));

        modelMatrix = glm::translate(modelMatrix, glm::vec3(0.3, 0.1, 0));
        modelMatrix = glm::rotate(modelMatrix, angle, glm::vec3(0, 1, 0));
        modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.3, -0.1, 0));

        modelMatrix *= glm::scale(glm::mat4(1), glm::vec3(0.2, 0.2, 0.2));


        RenderSimpleMesh(meshes["box"], shaders["TemaShader"], modelMatrix, SKIN);
    }

    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, translateToCamera);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0.3, -0.1, 0));
       
        modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.3, 0.1, 0));
        modelMatrix = glm::rotate(modelMatrix, angle, glm::vec3(0, 1, 0));
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0.3, -0.1, 0));

        modelMatrix *= glm::scale(glm::mat4(1), glm::vec3(0.2, 0.2, 0.2));


        RenderSimpleMesh(meshes["box"], shaders["TemaShader"], modelMatrix, SKIN);
    }

    //cap
    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, translateToCamera);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0.3, 0));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f));
        modelMatrix = glm::rotate(modelMatrix, angle, glm::vec3(0, 1, 0));

        RenderSimpleMesh(meshes["box"], shaders["TemaShader"], modelMatrix, SKIN);
    }
    //pantaloni

    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, translateToCamera);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.1, -0.4, 0));

        modelMatrix = glm::translate(modelMatrix, glm::vec3(0.1, 0.4, 0));
        modelMatrix = glm::rotate(modelMatrix, angle, glm::vec3(0, 1, 0));
        modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.1, -0.4, 0));

        modelMatrix *= glm::scale(glm::mat4(1), glm::vec3(0.2, 0.4, 0.2));

        RenderSimpleMesh(meshes["box"], shaders["TemaShader"], modelMatrix, WHITE);

    }
    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, translateToCamera);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0.1, -0.4, 0));

        modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.1, 0.4, 0));
        modelMatrix = glm::rotate(modelMatrix, angle, glm::vec3(0, 1, 0));
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0.1, -0.4, 0));

        modelMatrix *= glm::scale(glm::mat4(1), glm::vec3(0.2, 0.4, 0.2));

        RenderSimpleMesh(meshes["box"], shaders["TemaShader"], modelMatrix, WHITE);
    }

    //pantofi

    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, translateToCamera);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.1, -0.7, 0));

        modelMatrix = glm::translate(modelMatrix, glm::vec3(0.1, 0.7, 0));
        modelMatrix = glm::rotate(modelMatrix, angle, glm::vec3(0, 1, 0));
        modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.1, -0.7, 0));

        modelMatrix *= glm::scale(glm::mat4(1), glm::vec3(0.2, 0.2, 0.2));


        RenderSimpleMesh(meshes["box"], shaders["TemaShader"], modelMatrix, BLACK);
    }

    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, translateToCamera);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0.1, -0.7, 0));
        
        modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.1, 0.7, 0));
        modelMatrix = glm::rotate(modelMatrix, angle, glm::vec3(0, 1, 0));
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0.1, -0.7, 0));

        modelMatrix *= glm::scale(glm::mat4(1), glm::vec3(0.2, 0.2, 0.2));


        RenderSimpleMesh(meshes["box"], shaders["TemaShader"], modelMatrix, BLACK);
    }

}



void Tema2::readFromTexts() {
    ifstream indata;
    int num;
    int rows;
    int col;
    int choice = rand() % 5;
    string fileName;

    switch (choice) {
        case    0:
            fileName = "src/lab_m1/Tema2/labirint1.txt";
        break;
        case    1:
            fileName = "src/lab_m1/Tema2/labirint2.txt";
            break;
        case    2:
            fileName = "src/lab_m1/Tema2/labirint3.txt";
            break;
        case    3:
            fileName = "src/lab_m1/Tema2/labirint4.txt";
            break;
        case    4:
            fileName = "src/lab_m1/Tema2/labirint5.txt";
            break;

    }
    indata.open(fileName);
      if (!indata) {
        cerr << "Error: file could not be opened" << endl;
        exit(1);
    }

    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            indata >> num;
            mazeMatrix[i][j] = num;
            cout << mazeMatrix[i][j] << " ";
            if (num == 1){
                maze.push_back(Wall(mazeX, mazeY, mazeZ));
            }
            else if (num == 2) {
                enemies.push_back(Enemy(mazeX, mazeY, mazeZ));
            }
            else if (num == 0) {
                freeSpaces.push_back(glm::vec3(mazeX, mazeY, mazeZ));
            }
            mazeX += 2;
        }
        cout << endl;
        mazeX = 0;
        mazeZ += 2;
    }


    indata.close();



}

// https://developer.mozilla.org/en-US/docs/Games/Techniques/3D_collision_detection
//AABB VS AABB
bool Tema2::checkCharacterMazeCollision() {

    float charX = camera->GetTargetPosition().x ;
    float charZ = camera->GetTargetPosition().z ;

    float angle2 = angle;
    if (angle2 > 6.25) {
        angle2 = angle / 6.25;
    }

    for (int i = 0; i < maze.size(); i++) {
        if ((angle2 < 0.7 && angle2 > -0.7) || (angle2 < 3.6 && angle2 > 2.6)) {
            if ((charX - 0.4 <= maze[i].wallX + 1 && charX + 0.4 >= maze[i].wallX - 1) &&
                (charZ - 0.1 <= maze[i].wallZ + 1 && charZ + 0.1 >= maze[i].wallZ - 1) == true)
                return true;
        }
        else {
            if ((charX - 0.1 <= maze[i].wallX + 1 && charX + 0.1 >= maze[i].wallX - 1) &&
                (charZ - 0.4 <= maze[i].wallZ + 1 && charZ + 0.4 >= maze[i].wallZ - 1) == true)
                return true;
        }


    }
    return false;


}

void Tema2::checkProjectileEnemyCollision(Projectile& projectile) {
        for (int j = 0; j < enemies.size(); j++) {
            float distance = sqrt(((projectile.initialX + projectile.translateX)  - enemies[j].currentPosition.x) *
                ((projectile.initialX + projectile.translateX)  - enemies[j].currentPosition.x) +
                ((projectile.initialY + projectile.translateY )  - enemies[j].currentPosition.y - 0.75) *
                ((projectile.initialY + projectile.translateY)  - enemies[j].currentPosition.y - 0.75) +
                ((projectile.initialZ + projectile.translateZ)  - enemies[j].currentPosition.z) *
                ((projectile.initialZ + projectile.translateZ) - enemies[j].currentPosition.z));

            if (distance < (0.15f)) {
                projectile.isDead = true;
                enemies[j].isDead = true;
            }
    }

}

//Sphere vs AABB
void Tema2::playerEnemyCollision() {
    if (attack == false){
        for (int i = 0; i < enemies.size(); i++) {
            float x = fmax(camera->GetTargetPosition().x - 0.4, fmin(enemies[i].currentPosition.x,
                camera->GetTargetPosition().x + 0.4));
            float z = fmax(camera->GetTargetPosition().z - 0.1, fmin(enemies[i].currentPosition.z,
                camera->GetTargetPosition().z + 0.1));


            float dist = sqrt((x - enemies[i].currentPosition.x) * (x - enemies[i].currentPosition.x) +
                (z - enemies[i].currentPosition.z) * (z - enemies[i].currentPosition.z));

            if (dist <= 0.1f) {
                enemies[i].isDead = true;
                if (enemies[i].deformare == 0) {
                    life -= 20;
                    scaleX += 0.002;
                    cout << "Enemy player collision!" << endl;
                }
                if (life == 0)
                    scaleX = 0;
            }
        }
    }
}

void Tema2::projectileMazeCollision() {
    for (int j = 0; j < projectiles.size(); j++) {
        for (int i = 0; i < maze.size(); i++) {
            float x = fmax(maze[i].wallX - 1 , fmin(projectiles[j].translateX + projectiles[j].initialX,
                maze[i].wallX + 1));
            float z = fmax(maze[i].wallZ - 1, fmin(projectiles[j].translateZ + projectiles[j].initialZ,
                maze[i].wallZ + 1));


            float dist = sqrt((x - projectiles[j].translateX - projectiles[j].initialX) * (x - projectiles[j].translateX - projectiles[j].initialX) +
                (z - projectiles[j].translateZ - projectiles[j].initialZ) * (z - projectiles[j].translateZ - projectiles[j].initialZ));
            if (dist <= 0.1f) {
                cout << "Projectile wall collision!" << endl;
                projectiles[j].isDead = true;
            }
        }
    }
}


void Tema2::deleteDeadProjectiles() {
    for (int i = 0; i < projectiles.size(); i++) {
        if (projectiles[i].isDead == true) {
                projectiles.erase(projectiles.begin() + i);

        }
    }
}


void Tema2::generateLifeBar() {

    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0.725 + scaleX * 25, 0.95, 0));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.01 - scaleX, 0.0001, 0.001));

        meshes["plane50"]->SetDrawMode(GL_LINES);
        glLineWidth(15);
        RenderSimpleMesh(meshes["plane50"], shaders["ShaderHud"], modelMatrix, RED);
        meshes["plane50"]->SetDrawMode(GL_TRIANGLES);

        glLineWidth(1);

    }
}

void Tema2::generateTimeBar() {
    {
        //500 secunde ca sa termini labirintul
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0.725 + timeX / 50000 * 25  , 0.9, 0));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.01 - timeX /50000, 0.0001, 0.001));

        meshes["plane50"]->SetDrawMode(GL_LINES);
        glLineWidth(15);

        RenderSimpleMesh(meshes["plane50"], shaders["ShaderHud"], modelMatrix, YELLOW);

        meshes["plane50"]->SetDrawMode(GL_TRIANGLES);

        glLineWidth(1);

    }
}



void Tema2::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);

   

}

void Tema2::generateMaze() {
    for (int i = 0; i < maze.size(); i++) {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(maze[i].wallX, 1.5, maze[i].wallZ));
        modelMatrix *= glm::scale(glm::mat4(1), glm::vec3(2, 3, 2));

        RenderSimpleMesh(meshes["box"], shaders["TemaShader"], modelMatrix, ORANGE);
    }
}

void Tema2::generateEnemies(float deltaTimeSeconds) {
    for (int i = 0; i < enemies.size(); i++) {
        glm::mat4 modelMatrix = glm::mat4(1);

        float enemytransX = enemies[i].currentPosition.x;
        float enemyTransY = enemies[i].currentPosition.y + 0.75;
        float enemytransZ = enemies[i].currentPosition.z;
        bool checkerX = false;

        modelMatrix = glm::translate(modelMatrix, glm::vec3(enemytransX, enemyTransY, enemytransZ));
        modelMatrix *= glm::scale(glm::mat4(1), glm::vec3(0.35, 0.35, 0.35));

        if (enemies[i].isDead == false && enemies[i].deformare == 0)
            RenderSimpleMesh(meshes["sphere"], shaders["EnemyShader"], modelMatrix, BLACK);
        else
            RenderSimpleEnemy(meshes["sphere"], shaders["EnemyShader"], modelMatrix, BLACK);

        if (enemies[i].deformare == 0) {
            if (cond1 == true) {
                if (enemytransX <= enemies[i].enemyInitialX + 0.8 && enemytransZ <= enemies[i].enemeyInitialZ + 0.8 && condition4 == true) {
                    enemies[i].currentPosition.x += deltaTimeSeconds / 5;
                    enemies[i].currentPosition.z += deltaTimeSeconds / 5;

                }
                else if (enemytransX >= enemies[i].enemyInitialX - 0.8 && enemytransZ >= enemies[i].enemeyInitialZ - 0.8) {
                    enemies[i].currentPosition.x += (-1) * deltaTimeSeconds / 5;
                    condition4 = false;


                }
                else if (enemytransX <= enemies[i].enemyInitialX - 0.8 && enemytransZ >= enemies[i].enemeyInitialZ - 0.8) {
                    enemies[i].currentPosition.z += (-1) * deltaTimeSeconds / 5;
                }
                else {
                    cond1 = false;
                    condition4 = true;
                }
            }
            else {
                if (enemytransX <= enemies[i].enemyInitialX + 0.8 && enemytransZ <= enemies[i].enemeyInitialZ + 0.8 && condition4 == true) {
                    enemies[i].currentPosition.x += deltaTimeSeconds / 5;

                }
                else if (enemytransX >= enemies[i].enemyInitialX + 0.8 && enemytransZ <= enemies[i].enemeyInitialZ + 0.8) {
                    enemies[i].currentPosition.z += deltaTimeSeconds / 5;


                }
                else if (enemytransX >= enemies[i].enemyInitialX - 0.8 && enemytransZ >= enemies[i].enemeyInitialZ - 0.8) {
                    enemies[i].currentPosition.x += (-1) * deltaTimeSeconds / 5;
                    enemies[i].currentPosition.z += (-1) * deltaTimeSeconds / 5;
                    condition4 = false;
                }
                else {
                    cond1 = true;
                    condition4 = true;
                }
            }
        }

        if (enemies[i].isDead == true) {
            enemies[i].deformare = 1;
            enemies[i].timer += 1;
            if (enemies[i].timer > 200)
                enemies.erase(enemies.begin() + i);
            //dupa 200 de frame uri se sterg
        }

    }
}

void Tema2::generatePlane() {
    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0.17, 0));
        RenderSimpleMesh(meshes["plane50"], shaders["TemaShader"], modelMatrix, GREEN);
    }
}



void Tema2::Update(float deltaTimeSeconds)
{
    generatePlane();
    generateMaze();

    generateEnemies(deltaTimeSeconds);

    checkCharacterMazeCollision();
    playerEnemyCollision();
    generateLifeBar();
    generateTimeBar();

    shootPrjectile(deltaTimeSeconds, camera->GetTargetPosition());
    projectileMazeCollision();
    deleteDeadProjectiles();


    if (attack == false)
        drawCharacter2(camera->GetTargetPosition(), angle);


    timeX += deltaTimeSeconds;
    timer += deltaTimeSeconds;

    //daca timpul ajunge la 500 sau nu mai ai vieti programul se incheie
    if (timeX > 500 || life == 0) {
        glClearColor(0.2, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        maze.clear();
        enemies.clear();
        projectiles.clear();
    }

}


void Tema2::FrameEnd()
{

}




void Tema2::RenderMesh(Mesh * mesh, Shader * shader, const glm::mat4 & modelMatrix)
{
    if (!mesh || !shader || !shader->program)
        return;

    shader->Use();
    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    mesh->Render();
}


void Tema2::OnInputUpdate(float deltaTime, int mods)
{
    {
        bool collision = checkCharacterMazeCollision();
        float cameraSpeed = 1.0f;

        if (window->KeyHold(GLFW_KEY_W)) {
            if ((collision == false || collisionB == true) && attack == false) {
                camera->MoveForward(cameraSpeed * deltaTime);
                collisionB = false;
                collisionR = false;
                collisionL = false;
                collisionF = false;
            }
            else {
                collisionF = true;
            }
        }
        if (window->KeyHold(GLFW_KEY_S) ) {
            if ((collision == false || collisionF == true) && attack == false) {
                camera->MoveForward(-cameraSpeed * deltaTime);
                collisionB = false;
                collisionR = false;
                collisionL = false;
                collisionF = false;
            }
            else {
                collisionB = true;
            }
                    
        }
        if (window->KeyHold(GLFW_KEY_A) && collisionL == false) {
            if ((collision == false || collisionR == true) && attack == false) {
                camera->TranslateRight(-cameraSpeed * deltaTime);
                collisionB = false;
                collisionR = false;
                collisionL = false;
                collisionF = false;
            }
            else {
                collisionL = true;
            }

        }
        if (window->KeyHold(GLFW_KEY_D) && collisionR == false) {
            if ((collision == false || collisionL == true) && attack == false) {
                camera->TranslateRight(cameraSpeed * deltaTime);
                collisionB = false;
                collisionR = false;
                collisionL = false;
                collisionF = false;
            }
            else {
                collisionR = true;
            }

        }
        
    }

}
void Tema2::RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color)
{
    if (!mesh || !shader || !shader->GetProgramID())
        return;

    glUseProgram(shader->program);
    glUseProgram(shader->program);

    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    int colorLoc = glGetUniformLocation(shader->program, "object_color");
    glUniform3f(colorLoc, color.r, color.g, color.b);

    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}

void Tema2::RenderSimpleEnemy(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color) {
    if (!mesh || !shader || !shader->GetProgramID())
        return;

    glUseProgram(shader->program);

    int object_color = glGetUniformLocation(shader->program, "object_color");
    glUniform3f(object_color, color.r, color.g, color.b);

    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    int deformare2 = glGetUniformLocation(shader->program, "deformare");
    glUniform1i(deformare2, deformare);

    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_SHORT, 0);

}



void Tema2::OnKeyPress(int key, int mods)
{
    if (key == GLFW_KEY_LEFT_CONTROL)
    {
        if (attack == false) {
            attack = true;
            camera->TranslateForward(2);

        }
        else
            attack = false;
            camera->TranslateForward(-1);

    }



}


void Tema2::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Tema2::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
    {
        float sensivityOX = 0.005f;
        float sensivityOY = 0.005f;

        float sensivityOXfp = 0.001f;
        float sensivityOYfp = 0.001f;
        if (attack == true) {          
            camera->RotateFirstPerson_OX(-deltaY * sensivityOXfp);
            camera->RotateFirstPerson_OY(-deltaX * sensivityOYfp);
            angle += -deltaX * sensivityOYfp;
       }


        if (attack == false) {


            camera->RotateThirdPerson_OX(-deltaY * sensivityOX);
            camera->RotateThirdPerson_OY(-deltaX * sensivityOY);
            angle += -deltaX * sensivityOY;


        }
   

    }
}


void Tema2::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event

    if (IS_BIT_SET(button, GLFW_MOUSE_BUTTON_LEFT) && attack == true) {
        if (timer > 0.5) {
            drawProjectiles(camera->GetTargetPosition());
            timer = 0;
        }

    }
}


void Tema2::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Tema2::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Tema2::OnWindowResize(int width, int height)
{
}
