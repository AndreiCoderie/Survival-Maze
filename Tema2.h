#pragma once

#include "components/simple_scene.h"
#include "lab_m1/Tema2/Tema2_camera.h"

#define GREEN glm::vec3(0.0, 0.5, 0.0)
#define BLACK glm::vec3(0, 0, 0)
#define ORANGE glm::vec3(1, 0.55f, 0.f)
#define BLUE glm::vec3(0, 0, 1)
#define SKIN glm::vec3(1, 1, 0.8)
#define WHITE glm::vec3(1, 1, 1)
#define RED glm::vec3(1,0,0)
#define YELLOW glm::vec3(1, 0.89, 0)


namespace m1
{

    class Wall {
    public:
        float wallX;
        float wallY;
        float wallZ;

        Wall(float wallX, float wallY, float wallZ) {
            this->wallX = wallX;
            this->wallY = wallY;
            this->wallZ = wallZ;


        }

        ~Wall() {}
    };

    class Enemy {
    public:
        float enemyInitialX;
        float enemyInitialY;
        float enemeyInitialZ;
        glm::vec3 currentPosition;
        bool isDead = false;
        int deformare = 0;
        int timer = 0;

        Enemy(float enemyInitialX, float enemyInitialY, float enemeyInitialZ) {
            this->enemyInitialX = enemyInitialX;
            this->enemyInitialY = enemyInitialY;
            this->enemeyInitialZ = enemeyInitialZ;
            currentPosition = glm::vec3(enemyInitialX, enemyInitialY, enemeyInitialZ);


        }

        ~Enemy() {}
    };

    class Projectile {
    public:
        float angle;
        float initialX;
        float initialY;
        float initialZ;
        float translateX;
        float translateY;
        float translateZ;
        float speed;
        bool isDead = false;
        float scale = 1;
        float directionX;
        float directionY;
        float directionZ;


        Projectile(float initialX,  float initialY,   float initialZ, float directionX,float directionY, float directionZ) {
            this->translateX = 0;
            this->translateY = 0;
            this->translateZ = 0;
            this->initialX = initialX;
            this->initialY = initialY;
            this->initialZ = initialZ;
            this->directionX = directionX;
            this->directionY = directionY;
            this->directionZ = directionZ;
            //50
            this->speed = 10;
        }

        ~Projectile() {}
    };

    class Tema2 : public gfxc::SimpleScene
    {
     public:
         Tema2();
        ~Tema2();

        void Init() override;

        void drawProjectiles(glm::vec3 positionToCamera);

        void computeProjectileAngle();

        void shootPrjectile(float deltaTimeSeconds, glm::vec3 translateToCamera);

        Mesh* CreateCircle(std::string name, float radius, glm::vec3 color, glm::vec3 centerColor);

     private:
        void FrameStart() override;
        void drawCrosshair();
        void drawCharacter(glm::vec3 translateToCamera);
        void drawCharacter2(glm::vec3 translateToCamera, float angle);
        void readFromTexts();
        bool checkCharacterMazeCollision();
        void generateMaze();
        void generateEnemies(float deltaTimeSeconds);
        void generatePlane();
        void checkProjectileEnemyCollision(Projectile& projectile);
        void playerEnemyCollision();
        void projectileMazeCollision();
        void deleteDeadProjectiles();
        void generateLifeBar();
        void generateTimeBar();
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void RenderMesh(Mesh *mesh, Shader *shader, const glm::mat4 &modelMatrix) override;

        void OnInputUpdate(float deltaTime, int mods) override;
        void RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color);
        void RenderSimpleEnemy(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color);
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;

     protected:
        implemented::Camera *camera;
        glm::mat4 projectionMatrix;
        bool renderCameraTarget;
        float translateX;
        float translateY;
        float translateZ;
        int mazeMatrix[10][10];

        std::vector<Wall> maze;
        std::vector<Enemy> enemies;
        std::vector<Projectile> projectiles;
        std::vector<glm::vec3> freeSpaces;
        std::vector<glm::vec3> allSpaces;
        float leftO, rightO, bottom, top, anglePerspective;
        bool checkPerspOrtho;
        float playerAngle;
        bool attack;
        float mazeX, mazeY, mazeZ;
        float timer;
        bool collisionF;
        bool collisionB;
        bool collisionR;
        bool collisionL;
        int life;
        float scaleX;
        float timeX;
        int randomIndex;
        int deformare;
        bool condition4;
        bool cond1;
        float angle;




        // TODO(student): If you need any other class variables, define them here.

    };
}   // namespace m1
