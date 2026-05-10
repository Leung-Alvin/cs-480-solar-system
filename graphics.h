#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <iostream>
#include <stack>
#include <vector>
using namespace std;

#include "graphics_headers.h"
#include "camera.h"
#include "shader.h"
#include "object.h"
#include "sphere.h"
#include "mesh.h"
#include "Light.h"
#include "Asteroid.h"
#include "Material.h"

#define numVBOs 2;
#define numIBs 2;

enum PlanetID {
    SUN, MERCURY, VENUS, EARTH, MOON, MARS, JUPITER, JUPITER_MOON1, JUPITER_MOON2, SATURN,
    SATURN_MOON1, SATURN_MOON2, COMET, URANUS, NEPTUNE,
    CERES, ERIS, HAUMEA, NONE
};
class Graphics
{
  public:
    Graphics();
    ~Graphics();
    bool Initialize(int width, int height);
    void HierarchicalUpdate2(double dt);
    void Render();
    void Update(glm::mat4 transform);
    void UpdateCamera(float x, float y, bool l, bool r, bool u, bool d);
    Camera* getCamera() { return m_camera; }
	Mesh* getMesh() { return m_mesh; }
	glm::vec3 getNearestPlanetPos() { return m_nearestPlanetPos; }
	glm::vec3 getPlanetPosition(PlanetID id);
	PlanetID findClosestPlanetID();
	float getPlanetRadius(PlanetID planet);

    void SendMaterialToShader(Material* mat);
    
    bool m_showShip = true;

    float m_shipSpeedRatio = 0.0f;






  private:
    std::string ErrorString(GLenum error);

    bool collectShPrLocs();
    void ComputeTransforms (double dt, std::vector<float> speed, std::vector<float> dist,
        std::vector<float> rotSpeed, glm::vec3 rotVector, std::vector<float> scale, 
        glm::mat4& tmat, glm::mat4& rmat, glm::mat4& smat);

    stack<glm::mat4> modelStack;

    Camera *m_camera;
    Shader *m_shader;

    GLint m_projectionMatrix;
    GLint m_viewMatrix;
    GLint m_modelMatrix;
	GLint m_normalMatrix;
    GLint m_positionAttrib;
    GLint m_colorAttrib;
    GLint m_tcAttrib;
    GLint m_hasTexture;
    GLint m_normalAttrib;
    GLint m_useInstancing;
    GLint m_hasNorm;

    Sphere* m_sun;
    Sphere* m_mercury;
    Sphere* m_venus;
    Sphere* m_earth;
    Sphere* m_mars;
    Sphere* m_jupiter;
    Sphere* m_saturn;
    Sphere* m_uranus;
    Sphere* m_neptune;
    Sphere* m_ceres;
    Sphere* m_eris;
    Sphere* m_haumea;
    Sphere* m_moon;
    Sphere* m_moon1;
    Sphere* m_moon2;
    Sphere* m_moon3;
    Sphere* m_moon4;
    Sphere* m_comet;

    Mesh* m_mesh;
    Mesh* m_skybox;

    int randomizers[150];
    float spacingScale = 3.0f;

    // Inner asteroid belt
    Asteroid* m_asteroids;
    std::vector<glm::mat4> m_asteroidTransforms;
    int m_asteroidCount;

    // Outer asteroid belt
    Asteroid* m_outerAsteroids;
    std::vector<glm::mat4> m_outerAsteroidTransforms;
    int m_outerAsteroidCount;

	Light* m_light;

	Material* m_material;
	Material* m_rockyMaterial;
	Material* m_gasMaterial;
	Material* m_sunMaterial;
	Material* m_dwarfMaterial;
    Material* m_shipMaterial;

    GLint m_globalAmbLoc;
    GLint m_lightAmbLoc;
    GLint m_lightDiffLoc;
    GLint m_lightSpecLoc;
    GLint m_lightPosLoc;
	GLint m_isSunLoc;

    GLint mAmbLoc;
	GLint mDiffLoc;
	GLint mSpecLoc;
	GLint mShineLoc;

    glm::vec3 m_nearestPlanetPos;

	GLint m_isShipLoc;
    GLint m_shipSpeedRatioLoc;
	GLint m_isSkyboxLoc;

};

#endif /* GRAPHICS_H */
