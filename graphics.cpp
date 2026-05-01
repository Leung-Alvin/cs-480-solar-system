#include "graphics.h"

Graphics::Graphics()
{
	for(int i = 0; i < 150; i++) {
		randomizers[i] = glm::linearRand(-5, 5);
	}

}

Graphics::~Graphics()
{

}

bool Graphics::Initialize(int width, int height)
{
	// Used for the linux OS
#if !defined(__APPLE__) && !defined(MACOSX)
  // cout << glewGetString(GLEW_VERSION) << endl;
	glewExperimental = GL_TRUE;

	auto status = glewInit();

	// This is here to grab the error that comes from glew init.
	// This error is an GL_INVALID_ENUM that has no effects on the performance
	glGetError();

	//Check for error
	if (status != GLEW_OK)
	{
		std::cerr << "GLEW Error: " << glewGetErrorString(status) << "\n";
		return false;
	}
#endif



	// Init Camera
	m_camera = new Camera();
	if (!m_camera->Initialize(width, height))
	{
		printf("Camera Failed to Initialize\n");
		return false;
	}

	// Set up the shaders
	m_shader = new Shader();
	if (!m_shader->Initialize())
	{
		printf("Shader Failed to Initialize\n");
		return false;
	}

	// Add the vertex shader
	if (!m_shader->AddShader(GL_VERTEX_SHADER))
	{
		printf("Vertex Shader failed to Initialize\n");
		return false;
	}

	// Add the fragment shader
	if (!m_shader->AddShader(GL_FRAGMENT_SHADER))
	{
		printf("Fragment Shader failed to Initialize\n");
		return false;
	}

	// Connect the program
	if (!m_shader->Finalize())
	{
		printf("Program to Finalize\n");
		return false;
	}

	// Populate location bindings of the shader uniform/attribs
	if (!collectShPrLocs()) {
		printf("Some shader attribs not located!\n");
	}

	// Starship
	m_mesh = new Mesh(glm::vec3(2.0f, 3.0f, -5.0f), "assets\\SpaceShip-1.obj", "assets\\SpaceShip-1.png");

	// The Sun
	m_sun = new Sphere(64, "assets\\2k_sun.jpg");

	// Planets
	m_mercury = new Sphere(48, "assets\\Mercury.jpg");
	m_venus = new Sphere(48, "assets\\Venus.jpg");
	m_earth = new Sphere(48, "assets\\2k_earth_daymap.jpg");
	m_mars = new Sphere(48, "assets\\Mars.jpg");
	m_jupiter = new Sphere(48, "assets\\Jupiter.jpg");
	m_saturn = new Sphere(48, "assets\\Saturn.jpg");
	m_uranus = new Sphere(48, "assets\\Uranus.jpg");
	m_neptune = new Sphere(48, "assets\\Neptune.jpg");
	
	// Dwarf planets
	m_ceres = new Sphere(48, "assets\\Ceres.jpg");
	m_eris = new Sphere(48, "assets\\Eris.jpg");
	m_haumea = new Sphere(48, "assets\\Haumea.jpg");

	// The moon
	m_moon = new Sphere(48, "assets\\2k_moon.jpg");

	// Jupiter's moons
	m_moon1 = new Sphere(32, "assets\\2k_moon.jpg");
	m_moon2 = new Sphere(32, "assets\\2k_moon.jpg");
	
	// Saturn's moons
	m_moon3 = new Sphere(32, "assets\\2k_moon.jpg");
	m_moon4 = new Sphere(32, "assets\\2k_moon.jpg");
	
	// Independent moon orbiting the sun
	m_comet = new Sphere(40, "assets\\2k_moon.jpg");

	// Skybox - use a large cube with a space texture
	m_skybox = new Mesh(glm::vec3(0.0f, 0.0f, 0.0f), "assets\\cube.obj", "assets\\Galaxy-cubemap2.png");

	m_asteroidCount = 50;
	m_asteroids = new Asteroid("assets\\2k_moon.jpg");
	
	m_outerAsteroidCount = 150;
	m_outerAsteroids = new Asteroid("assets\\2k_moon.jpg");


	//enable depth testing
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	return true;
}

void Graphics::HierarchicalUpdate2(double dt) {

  // Update your animation for the solar system here.
  std::vector<float> speed, dist, rotSpeed, scale;
  glm::vec3 rotVector;
  glm::mat4 localTransform;
  
  if (m_skybox != NULL) {
    glm::mat4 skyboxTransform = glm::scale(glm::mat4(1.0f), glm::vec3(200.0f, 200.0f, 200.0f));
    m_skybox->Update(skyboxTransform);
  }
  
  // position of the sun	
  modelStack.push(glm::translate(glm::mat4(1.f), glm::vec3(0, 0, 0)));
	localTransform = modelStack.top();
	localTransform *= glm::rotate(glm::mat4(1.0f), (float)dt * 0.5f, glm::vec3(0.f, 1.f, 0.f));
	localTransform *= glm::scale(glm::vec3(3.0, 3.0, 3.0));
	if (m_sun != NULL)
		m_sun->Update(localTransform);

	// Mercury
	speed = { 1.5f, 0.0f, 1.5f };
	dist = { spacingScale*2.0f, spacingScale * 0.0f, spacingScale * 2.0f };
	rotVector = glm::vec3(0.0f, 1.0f, 0.0f);
	rotSpeed = { 3.0f, 3.0f, 3.0f };
	scale = { 0.2f, 0.2f, 0.2f };
	localTransform = modelStack.top();
	localTransform *= glm::translate(glm::mat4(1.f),
		glm::vec3(cos(speed[0] * dt) * dist[0], 0.0f, sin(speed[2] * dt) * dist[2]));
	localTransform *= glm::rotate(glm::mat4(1.f), rotSpeed[0] * (float)dt, rotVector);
	localTransform *= glm::scale(glm::vec3(scale[0], scale[1], scale[2]));
	if (m_mercury != NULL)
		m_mercury->Update(localTransform);

	// Venus
	speed = { 1.3f, 0.0f, 1.3f };
	dist = { spacingScale * 3.5f, spacingScale * 0.0f, spacingScale * 3.5f };
	rotVector = glm::vec3(0.0f, 1.0f, 0.0f);
	rotSpeed = { 2.5f, 2.5f, 2.5f };
	scale = { 0.35f, 0.35f, 0.35f };
	localTransform = modelStack.top();
	localTransform *= glm::translate(glm::mat4(1.f),
		glm::vec3(cos(speed[0] * dt) * dist[0], 0.0f, sin(speed[2] * dt) * dist[2]));
	localTransform *= glm::rotate(glm::mat4(1.f), rotSpeed[0] * (float)dt, rotVector);
	localTransform *= glm::scale(glm::vec3(scale[0], scale[1], scale[2]));
	if (m_venus != NULL)
		m_venus->Update(localTransform);

	// Earth
	speed = { 1.0f, 0.0f, 1.0f };
	dist = { spacingScale * 6.0f, spacingScale * 0.0f, spacingScale * 6.0f };
	rotVector = glm::vec3(0.0f, 1.0f, 0.0f);
	rotSpeed = { 2.0f, 2.0f, 2.0f };
	scale = { 0.5f, 0.5f, 0.5f };
	
	localTransform = modelStack.top();
	localTransform *= glm::translate(glm::mat4(1.f),
		glm::vec3(cos(speed[0] * dt) * dist[0], 0.0f, sin(speed[2] * dt) * dist[2]));
	modelStack.push(localTransform);
	localTransform *= glm::rotate(glm::mat4(1.f), rotSpeed[0] * (float)dt, rotVector); 
	localTransform *= glm::scale(glm::vec3(scale[0], scale[1], scale[2]));
	if (m_earth != NULL)
		m_earth->Update(localTransform);

	// position of moon
	speed = { 6.0f, 6.0f, 6.0f };
	dist = { spacingScale * 1.25f, spacingScale * 1.25f, spacingScale * 1.25f };
	rotVector = glm::vec3(1.0f, 1.0f, 0.0f);
	rotSpeed = { 0.25f, 0.25f, 0.25f };
	scale = { 0.27f, 0.27f, 0.27f };
	
	localTransform = modelStack.top();
	localTransform *= glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(0.0f, 0.0f, -1.0f));
	localTransform *= glm::translate(glm::mat4(1.f),
		glm::vec3(cos(speed[0] * dt) * dist[0], sin(speed[1] * dt) * dist[1], sin(speed[2] * dt) * dist[2]));
	modelStack.push(localTransform);
	localTransform *= glm::rotate(glm::mat4(1.f), rotSpeed[0] * (float)dt, rotVector);
	localTransform *= glm::scale(glm::vec3(scale[0], scale[1], scale[2]));

	if (m_moon != NULL)
		m_moon->Update(localTransform);

	modelStack.pop(); // Back to Earth
	modelStack.pop(); // Back to Sun

	// Mars
	speed = { 0.8f, 0.0f, 0.8f };
	dist = { spacingScale * 8.0f, spacingScale * 0.0f, spacingScale * 8.0f };
	rotVector = glm::vec3(0.0f, 1.0f, 0.0f);
	rotSpeed = { 1.8f, 1.8f, 1.8f };
	scale = { 0.4f, 0.4f, 0.4f };
	localTransform = modelStack.top();
	localTransform *= glm::translate(glm::mat4(1.f),
		glm::vec3(cos(speed[0] * dt) * dist[0], 0.0f, sin(speed[2] * dt) * dist[2]));
	localTransform *= glm::rotate(glm::mat4(1.f), rotSpeed[0] * (float)dt, rotVector);
	localTransform *= glm::scale(glm::vec3(scale[0], scale[1], scale[2]));
	if (m_mars != NULL)
		m_mars->Update(localTransform);

	// Jupiter
	speed = { 0.4f, 0.0f, 0.4f };
	dist = { spacingScale * 12.0f, spacingScale * 0.0f, spacingScale * 12.0f };
	rotVector = glm::vec3(0.0f, 1.0f, 0.0f);
	rotSpeed = { 3.0f, 3.0f, 3.0f };
	scale = { 0.8f, 0.8f, 0.8f };
	
	localTransform = modelStack.top();
	localTransform *= glm::translate(glm::mat4(1.f),
		glm::vec3(cos(speed[0] * dt) * dist[0], 0.0f, sin(speed[2] * dt) * dist[2]));
	modelStack.push(localTransform); // Save Jupiter's position for moons
	
	localTransform *= glm::rotate(glm::mat4(1.f), rotSpeed[0] * (float)dt, rotVector);
	localTransform *= glm::scale(glm::vec3(scale[0], scale[1], scale[2]));
	if (m_jupiter != NULL)
		m_jupiter->Update(localTransform);

	speed = { 5.0f, 5.0f, 5.0f };
	dist = { spacingScale * 1.3f, spacingScale * 1.3f, spacingScale * 1.3f };
	rotVector = glm::vec3(0.0f, 1.0f, 0.0f);
	rotSpeed = { 2.0f, 2.0f, 2.0f };
	scale = { 0.15f, 0.15f, 0.15f };
	
	localTransform = modelStack.top();
	localTransform *= glm::translate(glm::mat4(1.f),
		glm::vec3(cos(speed[0] * dt) * dist[0], sin(speed[1] * dt) * dist[1], sin(speed[2] * dt) * dist[2]));
	localTransform *= glm::rotate(glm::mat4(1.f), rotSpeed[0] * (float)dt, rotVector);
	localTransform *= glm::scale(glm::vec3(scale[0], scale[1], scale[2]));
	if (m_moon1 != NULL)
		m_moon1->Update(localTransform);

	speed = { 3.5f, 3.5f, 3.5f };
	dist = { spacingScale * 1.8f, spacingScale * 1.8f, spacingScale * 1.8f };
	rotVector = glm::vec3(0.0f, 0.0f, 1.0f);
	rotSpeed = { 1.5f, 1.5f, 1.5f };
	scale = { 0.13f, 0.13f, 0.13f };
	
	localTransform = modelStack.top();
	localTransform *= glm::rotate(glm::mat4(1.0f), glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	localTransform *= glm::translate(glm::mat4(1.f),
		glm::vec3(cos(speed[0] * dt) * dist[0], sin(speed[1] * dt) * dist[1], sin(speed[2] * dt) * dist[2]));
	localTransform *= glm::rotate(glm::mat4(1.f), rotSpeed[0] * (float)dt, rotVector);
	localTransform *= glm::scale(glm::vec3(scale[0], scale[1], scale[2]));
	if (m_moon2 != NULL)
		m_moon2->Update(localTransform);

	modelStack.pop(); // Back to Sun from Jupiter

	// Saturn
	speed = { 0.3f, 0.0f, 0.3f };
	dist = { spacingScale * 16.0f, spacingScale * 0.0f, spacingScale * 16.0f };
	rotVector = glm::vec3(0.0f, 1.0f, 0.0f);
	rotSpeed = { 2.5f, 2.5f, 2.5f };
	scale = { 0.7f, 0.7f, 0.7f };
	
	localTransform = modelStack.top();
	localTransform *= glm::translate(glm::mat4(1.f),
		glm::vec3(cos(speed[0] * dt) * dist[0], 0.0f, sin(speed[2] * dt) * dist[2]));
	modelStack.push(localTransform); // Save Saturn's position for moons
	
	localTransform *= glm::rotate(glm::mat4(1.f), rotSpeed[0] * (float)dt, rotVector);
	localTransform *= glm::scale(glm::vec3(scale[0], scale[1], scale[2]));
	if (m_saturn != NULL)
		m_saturn->Update(localTransform);

	speed = { 4.0f, 4.0f, 4.0f };
	dist = { spacingScale * 1.5f, spacingScale * 1.5f, spacingScale * 1.5f };
	rotVector = glm::vec3(0.0f, 1.0f, 0.0f);
	rotSpeed = { 1.8f, 1.8f, 1.8f };
	scale = { 0.2f, 0.2f, 0.2f };
	
	localTransform = modelStack.top();
	localTransform *= glm::translate(glm::mat4(1.f),
		glm::vec3(cos(speed[0] * dt) * dist[0], sin(speed[1] * dt) * dist[1], sin(speed[2] * dt) * dist[2]));
	localTransform *= glm::rotate(glm::mat4(1.f), rotSpeed[0] * (float)dt, rotVector);
	localTransform *= glm::scale(glm::vec3(scale[0], scale[1], scale[2]));
	if (m_moon3 != NULL)
		m_moon3->Update(localTransform);

	speed = { 5.5f, 5.5f, 5.5f };
	dist = { spacingScale * 1.1f, spacingScale * 1.1f, spacingScale * 1.1f };
	rotVector = glm::vec3(0.0f, 1.0f, 0.0f);
	rotSpeed = { 2.5f, 2.5f, 2.5f };
	scale = { 0.12f, 0.12f, 0.12f };
	
	localTransform = modelStack.top();
	localTransform *= glm::rotate(glm::mat4(1.0f), glm::radians(-45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	localTransform *= glm::translate(glm::mat4(1.f),
		glm::vec3(cos(speed[0] * dt) * dist[0], sin(speed[1] * dt) * dist[1], sin(speed[2] * dt) * dist[2]));
	localTransform *= glm::rotate(glm::mat4(1.f), rotSpeed[0] * (float)dt, rotVector);
	localTransform *= glm::scale(glm::vec3(scale[0], scale[1], scale[2]));
	if (m_moon4 != NULL)
		m_moon4->Update(localTransform);

	modelStack.pop(); // Back to Sun from Saturn

	// Uranus
	speed = { 0.2f, 0.0f, 0.2f };
	dist = { spacingScale * 20.0f, spacingScale * 0.0f, spacingScale * 20.0f };
	rotVector = glm::vec3(0.0f, 1.0f, 0.0f);
	rotSpeed = { 2.0f, 2.0f, 2.0f };
	scale = { 0.6f, 0.6f, 0.6f };
	localTransform = modelStack.top();
	localTransform *= glm::translate(glm::mat4(1.f),
		glm::vec3(cos(speed[0] * dt) * dist[0], 0.0f, sin(speed[2] * dt) * dist[2]));
	localTransform *= glm::rotate(glm::mat4(1.f), rotSpeed[0] * (float)dt, rotVector);
	localTransform *= glm::scale(glm::vec3(scale[0], scale[1], scale[2]));
	if (m_uranus != NULL)
		m_uranus->Update(localTransform);

	// Neptune
	speed = { 0.15f, 0.0f, 0.15f };
	dist = { spacingScale * 24.0f, spacingScale * 0.0f, spacingScale * 24.0f };
	rotVector = glm::vec3(0.0f, 1.0f, 0.0f);
	rotSpeed = { 1.8f, 1.8f, 1.8f };
	scale = { 0.55f, 0.55f, 0.55f };
	localTransform = modelStack.top();
	localTransform *= glm::translate(glm::mat4(1.f),
		glm::vec3(cos(speed[0] * dt) * dist[0], 0.0f, sin(speed[2] * dt) * dist[2]));
	localTransform *= glm::rotate(glm::mat4(1.f), rotSpeed[0] * (float)dt, rotVector);
	localTransform *= glm::scale(glm::vec3(scale[0], scale[1], scale[2]));
	if (m_neptune != NULL)
		m_neptune->Update(localTransform);

	// Ceres (dwarf planet in asteroid belt)
	speed = { 0.6f, 0.0f, 0.6f };
	dist = { spacingScale * 10.0f, spacingScale * 0.0f, spacingScale * 10.0f };
	rotVector = glm::vec3(0.0f, 1.0f, 0.0f);
	rotSpeed = { 2.2f, 2.2f, 2.2f };
	scale = { 0.15f, 0.15f, 0.15f };
	localTransform = modelStack.top();
	localTransform *= glm::translate(glm::mat4(1.f),
		glm::vec3(cos(speed[0] * dt) * dist[0], 0.0f, sin(speed[2] * dt) * dist[2]));
	localTransform *= glm::rotate(glm::mat4(1.f), rotSpeed[0] * (float)dt, rotVector);
	localTransform *= glm::scale(glm::vec3(scale[0], scale[1], scale[2]));
	if (m_ceres != NULL)
		m_ceres->Update(localTransform);

	// Eris (distant dwarf planet)
	speed = { 0.1f, 0.0f, 0.1f };
	dist = { spacingScale * 28.0f, spacingScale * 0.0f, spacingScale * 28.0f };
	rotVector = glm::vec3(0.0f, 1.0f, 0.0f);
	rotSpeed = { 1.5f, 1.5f, 1.5f };
	scale = { 0.25f, 0.25f, 0.25f };
	localTransform = modelStack.top();
	localTransform *= glm::translate(glm::mat4(1.f),
		glm::vec3(cos(speed[0] * dt) * dist[0], 0.0f, sin(speed[2] * dt) * dist[2]));
	localTransform *= glm::rotate(glm::mat4(1.f), rotSpeed[0] * (float)dt, rotVector);
	localTransform *= glm::scale(glm::vec3(scale[0], scale[1], scale[2]));
	if (m_eris != NULL)
		m_eris->Update(localTransform);

	// Haumea (distant dwarf planet)
	speed = { 0.12f, 0.0f, 0.12f };
	dist = { spacingScale * 26.0f, spacingScale * 0.0f, spacingScale * 26.0f };
	rotVector = glm::vec3(0.0f, 1.0f, 0.0f);
	rotSpeed = { 4.0f, 4.0f, 4.0f };
	scale = { 0.2f, 0.2f, 0.2f };
	localTransform = modelStack.top();
	localTransform *= glm::translate(glm::mat4(1.f),
		glm::vec3(cos(speed[0] * dt) * dist[0], 0.0f, sin(speed[2] * dt) * dist[2]));
	localTransform *= glm::rotate(glm::mat4(1.f), rotSpeed[0] * (float)dt, rotVector);
	localTransform *= glm::scale(glm::vec3(scale[0], scale[1], scale[2]));
	if (m_haumea != NULL)
		m_haumea->Update(localTransform);

	// Comet
	speed = { -0.1f, -0.1f, -0.1f };
	dist = { 18.0f, 0.0f, 18.0f };
	rotVector = glm::vec3(0.0f, 1.0f, 0.0f);
	rotSpeed = { .1f, 0.1f, 0.1f };
	scale = { 0.3f, 0.3f, 0.3f };
	glm::vec3 ellipse;
	ellipse = { 2.2f, 0.0f, 0.5f };
	
	localTransform = modelStack.top();  // Start from Sun
	localTransform *= glm::rotate(glm::mat4(1.0f), glm::radians(19.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	localTransform *= glm::translate(glm::mat4(1.f),
		glm::vec3(ellipse[0] * cos(speed[0] * dt) * dist[0] + 30, 0.0f, ellipse[2] * sin(speed[2] * dt) * dist[2]));
	localTransform *= glm::rotate(glm::mat4(1.f), rotSpeed[0] * (float)dt, rotVector);
	localTransform *= glm::scale(glm::vec3(scale[0], scale[1], scale[2]));
	if (m_comet != NULL)
		m_comet->Update(localTransform);

	// Asteroid Belt 
	m_asteroidTransforms.clear();
	float beltRadius = spacingScale * 10.5f; // Distance from sun
	float angleStep = (2.0f * 3.14159f) / m_asteroidCount;
	
	for (int i = 0; i < m_asteroidCount; i++) {
		float angle = angleStep * i + (float)dt * 0.1f + randomizers[i];
		float offset = (i % 3) * 0.5f - 0.5f; // Vary the radius slightly
		float currentRadius = beltRadius + offset;
		
		// Each asteroid has slightly different rotation
		float rotSpeed = 3.0f + (randomizers[i] * 0.03f);
		glm::vec3 rotAxis = glm::normalize(glm::vec3(0.3f + i * 0.1f, 1.0f, 0.2f + i * 0.05f));
		
		localTransform = modelStack.top();
		localTransform *= glm::translate(glm::mat4(1.f),
			glm::vec3(cos(angle) * currentRadius, (sin(i * 1.5f) * 0.3f), sin(angle) * currentRadius));
		localTransform *= glm::rotate(glm::mat4(1.f), rotSpeed * (float)dt, rotAxis);
		localTransform *= glm::scale(glm::vec3(0.025f + (i % 3) * 0.05f));
		
		m_asteroidTransforms.push_back(localTransform);
	}
	
	// Setup instancing with the computed transforms
	if (m_asteroids != NULL) {
		m_asteroids->Update(m_asteroidTransforms);
	}

	// Outer Asteroid Belt
	m_outerAsteroidTransforms.clear();
	float outerBeltRadius = spacingScale * 27.0f;
	float outerAngleStep = (2.0f * 3.14159f) / m_outerAsteroidCount;
	
	for (int i = 0; i < m_outerAsteroidCount; i++) {
		float orbitAngle = outerAngleStep * i + (float)dt * 0.03f + randomizers[i];
		float offset = (i % 4) * 0.8f - 1.2f;
		float currentRadius = outerBeltRadius + offset;
		
		float rotSpeed = 1.0f + (randomizers[i] * 0.05f);
		glm::vec3 rotAxis = glm::normalize(glm::vec3(0.5f + i * 0.08f, 1.0f, 0.3f + i * 0.06f));
		
		localTransform = glm::mat4(1.0f);
		
		// Position in orbit with more vertical spread
		localTransform *= glm::translate(glm::mat4(1.f), glm::vec3(cos(orbitAngle) * currentRadius, sin(i * 2.0f) * 0.5f, sin(orbitAngle) * currentRadius));
		localTransform *= glm::rotate(glm::mat4(1.f), rotSpeed * (float)dt, rotAxis);
		localTransform *= glm::scale(glm::vec3(0.15f + (i % 4) * 0.08f));
		
		m_outerAsteroidTransforms.push_back(localTransform);
	}
	
	// Setup instancing with the computed transforms
	if (m_outerAsteroids != NULL) {
		m_outerAsteroids->Update(m_outerAsteroidTransforms);
	}
	modelStack.pop();
}


void Graphics::ComputeTransforms(double dt, std::vector<float> speed, std::vector<float> dist,
	std::vector<float> rotSpeed, glm::vec3 rotVector, std::vector<float> scale, glm::mat4& tmat, glm::mat4& rmat, glm::mat4& smat) {
	tmat = glm::translate(glm::mat4(1.f),
		glm::vec3(cos(speed[0] * dt) * dist[0], sin(speed[1] * dt) * dist[1], sin(speed[2] * dt) * dist[2])
	);
	rmat = glm::rotate(glm::mat4(1.f), rotSpeed[0] * (float)dt, rotVector);
	smat = glm::scale(glm::vec3(scale[0], scale[1], scale[2]));
}

void Graphics::Render()
{
	//clear the screen
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Start the correct program
	m_shader->Enable();

	// Send in the projection and view to the shader (stay the same while camera intrinsic(perspective) and extrinsic (view) parameters are the same
	glUniformMatrix4fv(m_projectionMatrix, 1, GL_FALSE, glm::value_ptr(m_camera->GetProjection()));
	glUniformMatrix4fv(m_viewMatrix, 1, GL_FALSE, glm::value_ptr(m_camera->GetView()));

	if (m_skybox != NULL) {
		glDepthMask(GL_FALSE); // Disable depth writing for skybox
		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_skybox->GetModel()));
		if (m_skybox->hasTex) {
			glUniform1i(m_hasTexture, 1);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_skybox->getTextureID());
			GLuint sampler = m_shader->GetUniformLocation("sp");
			if (sampler == INVALID_UNIFORM_LOCATION)
			{
				printf("Sampler Not found\n");
			}
			glUniform1i(sampler, 0);
			m_skybox->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture);
		}
		glDepthMask(GL_TRUE); // Re-enable depth writing
	}

	// Render mesh (starship)
	/*if (m_mesh != NULL) {
		glUniform1i(m_hasTexture, false);
		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_mesh->GetModel()));
		if (m_mesh->hasTex) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_mesh->getTextureID());
			GLuint sampler = m_shader->GetUniformLocation("sp");
			if (sampler == INVALID_UNIFORM_LOCATION)
			{
				printf("Sampler Not found not found\n");
			}
			glUniform1i(sampler, 0);
			m_mesh->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture);
		}
	}*/

	// Render Sun
	if (m_sun != NULL) {
		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_sun->GetModel()));
		if (m_sun->hasTex) {
			glUniform1i(m_hasTexture, 1);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_sun->getTextureID());
			GLuint sampler = m_shader->GetUniformLocation("sp");
			if (sampler == INVALID_UNIFORM_LOCATION)
			{
				printf("Sampler Not found not found\n");
			}
			glUniform1i(sampler, 0);
			m_sun->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture);
		}
	}

	// Render Mercury
	if (m_mercury != NULL) {
		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_mercury->GetModel()));
		if (m_mercury->hasTex) {
			glUniform1i(m_hasTexture, 1);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_mercury->getTextureID());
			GLuint sampler = m_shader->GetUniformLocation("sp");
			if (sampler == INVALID_UNIFORM_LOCATION)
			{
				printf("Sampler Not found not found\n");
			}
			glUniform1i(sampler, 0);
			m_mercury->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture);
		}
	}

	// Render Venus
	if (m_venus != NULL) {
		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_venus->GetModel()));
		if (m_venus->hasTex) {
			glUniform1i(m_hasTexture, 1);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_venus->getTextureID());
			GLuint sampler = m_shader->GetUniformLocation("sp");
			if (sampler == INVALID_UNIFORM_LOCATION)
			{
				printf("Sampler Not found not found\n");
			}
			glUniform1i(sampler, 0);
			m_venus->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture);
		}
	}

	// Render Earth
	if (m_earth != NULL) {
		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_earth->GetModel()));
		if (m_earth->hasTex) {
			glUniform1i(m_hasTexture, 1);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_earth->getTextureID());
			GLuint sampler = m_shader->GetUniformLocation("sp");
			if (sampler == INVALID_UNIFORM_LOCATION)
			{
				printf("Sampler Not found not found\n");
			}
			glUniform1i(sampler, 0);
			m_earth->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture);
		}
	}

	// Render Moon
	if (m_moon != NULL) {
		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_moon->GetModel()));
		if (m_moon->hasTex) {
			glUniform1i(m_hasTexture, 1);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_moon->getTextureID());
			GLuint sampler = m_shader->GetUniformLocation("sp");
			if (sampler == INVALID_UNIFORM_LOCATION)
			{
				printf("Sampler Not found not found\n");
			}
			glUniform1i(sampler, 0);
			m_moon->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture);
		}
	}

	// Render Mars
	if (m_mars != NULL) {
		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_mars->GetModel()));
		if (m_mars->hasTex) {
			glUniform1i(m_hasTexture, 1);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_mars->getTextureID());
			GLuint sampler = m_shader->GetUniformLocation("sp");
			if (sampler == INVALID_UNIFORM_LOCATION)
			{
				printf("Sampler Not found not found\n");
			}
			glUniform1i(sampler, 0);
			m_mars->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture);
		}
	}

	// Render Jupiter
	if (m_jupiter != NULL) {
		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_jupiter->GetModel()));
		if (m_jupiter->hasTex) {
			glUniform1i(m_hasTexture, 1);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_jupiter->getTextureID());
			GLuint sampler = m_shader->GetUniformLocation("sp");
			if (sampler == INVALID_UNIFORM_LOCATION)
			{
				printf("Sampler Not found not found\n");
			}
			glUniform1i(sampler, 0);
			m_jupiter->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture);
		}
	}

	// Render moon
	if (m_moon1 != NULL) {
		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_moon1->GetModel()));
		if (m_moon1->hasTex) {
			glUniform1i(m_hasTexture, 1);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_moon1->getTextureID());
			GLuint sampler = m_shader->GetUniformLocation("sp");
			if (sampler == INVALID_UNIFORM_LOCATION)
			{
				printf("Sampler Not found not found\n");
			}
			glUniform1i(sampler, 0);
			m_moon1->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture);
		}
	}

	// Render Moon
	if (m_moon2 != NULL) {
		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_moon2->GetModel()));
		if (m_moon2->hasTex) {
			glUniform1i(m_hasTexture, 1);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_moon2->getTextureID());
			GLuint sampler = m_shader->GetUniformLocation("sp");
			if (sampler == INVALID_UNIFORM_LOCATION)
			{
				printf("Sampler Not found not found\n");
			}
			glUniform1i(sampler, 0);
			m_moon2->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture);
		}
	}

	// Render Saturn
	if (m_saturn != NULL) {
		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_saturn->GetModel()));
		if (m_saturn->hasTex) {
			glUniform1i(m_hasTexture, 1);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_saturn->getTextureID());
			GLuint sampler = m_shader->GetUniformLocation("sp");
			if (sampler == INVALID_UNIFORM_LOCATION)
			{
				printf("Sampler Not found not found\n");
			}
			glUniform1i(sampler, 0);
			m_saturn->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture);
		}
	}

	// Render moon
	if (m_moon3 != NULL) {
		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_moon3->GetModel()));
		if (m_moon3->hasTex) {
			glUniform1i(m_hasTexture, 1);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_moon3->getTextureID());
			GLuint sampler = m_shader->GetUniformLocation("sp");
			if (sampler == INVALID_UNIFORM_LOCATION)
			{
				printf("Sampler Not found not found\n");
			}
			glUniform1i(sampler, 0);
			m_moon3->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture);
		}
	}

	// Render moon
	if (m_moon4 != NULL) {
		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_moon4->GetModel()));
		if (m_moon4->hasTex) {
			glUniform1i(m_hasTexture, 1);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_moon4->getTextureID());
			GLuint sampler = m_shader->GetUniformLocation("sp");
			if (sampler == INVALID_UNIFORM_LOCATION)
			{
				printf("Sampler Not found not found\n");
			}
			glUniform1i(sampler, 0);
			m_moon4->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture);
		}
	}

	// Render Uranus
	if (m_uranus != NULL) {
		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_uranus->GetModel()));
		if (m_uranus->hasTex) {
			glUniform1i(m_hasTexture, 1);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_uranus->getTextureID());
			GLuint sampler = m_shader->GetUniformLocation("sp");
			if (sampler == INVALID_UNIFORM_LOCATION)
			{
				printf("Sampler Not found not found\n");
			}
			glUniform1i(sampler, 0);
			m_uranus->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture);
		}
	}

	// Render Neptune
	if (m_neptune != NULL) {
		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_neptune->GetModel()));
		if (m_neptune->hasTex) {
			glUniform1i(m_hasTexture, 1);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_neptune->getTextureID());
			GLuint sampler = m_shader->GetUniformLocation("sp");
			if (sampler == INVALID_UNIFORM_LOCATION)
			{
				printf("Sampler Not found not found\n");
			}
			glUniform1i(sampler, 0);
			m_neptune->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture);
		}
	}

	// Render Ceres
	if (m_ceres != NULL) {
		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_ceres->GetModel()));
		if (m_ceres->hasTex) {
			glUniform1i(m_hasTexture, 1);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_ceres->getTextureID());
			GLuint sampler = m_shader->GetUniformLocation("sp");
			if (sampler == INVALID_UNIFORM_LOCATION)
			{
				printf("Sampler Not found not found\n");
			}
			glUniform1i(sampler, 0);
			m_ceres->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture);
		}
	}

	// Render Eris
	if (m_eris != NULL) {
		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_eris->GetModel()));
		if (m_eris->hasTex) {
			glUniform1i(m_hasTexture, 1);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_eris->getTextureID());
			GLuint sampler = m_shader->GetUniformLocation("sp");
			if (sampler == INVALID_UNIFORM_LOCATION)
			{
				printf("Sampler Not found not found\n");
			}
			glUniform1i(sampler, 0);
			m_eris->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture);
		}
	}

	// Render Haumea
	if (m_haumea != NULL) {
		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_haumea->GetModel()));
		if (m_haumea->hasTex) {
			glUniform1i(m_hasTexture, 1);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_haumea->getTextureID());
			GLuint sampler = m_shader->GetUniformLocation("sp");
			if (sampler == INVALID_UNIFORM_LOCATION)
			{
				printf("Sampler Not found not found\n");
			}
			glUniform1i(sampler, 0);
			m_haumea->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture);
		}
	}

	// Render comet
	if (m_comet != NULL) {
		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_comet->GetModel()));
		if (m_comet->hasTex) {
			glUniform1i(m_hasTexture, 1);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_comet->getTextureID());
			GLuint sampler = m_shader->GetUniformLocation("sp");
			if (sampler == INVALID_UNIFORM_LOCATION)
			{
				printf("Sampler Not found\n");
			}
			glUniform1i(sampler, 0);
			m_comet->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture);
		}
	}

	// Render Inner Asteroid Belt
	if (m_asteroids != NULL && m_asteroidCount > 0) {
		glUniform1i(m_useInstancing, 1);
		if (m_asteroids->hasTex) {
			glUniform1i(m_hasTexture, 1);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_asteroids->getTextureID());
			GLuint sampler = m_shader->GetUniformLocation("sp");
			if (sampler == INVALID_UNIFORM_LOCATION)
			{
				printf("Sampler Not found not found\n");
			}
			glUniform1i(sampler, 0);
		}
		m_asteroids->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture, 100);
		if (m_useInstancing != INVALID_UNIFORM_LOCATION) {
			glUniform1i(m_useInstancing, 0);
		}
	}

	// Render Outer Asteroid Belt
	if (m_outerAsteroids != NULL && m_outerAsteroidCount > 0) {
		glUniform1i(m_useInstancing, 1);
		if (m_outerAsteroids->hasTex) {
			glUniform1i(m_hasTexture, 1);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_outerAsteroids->getTextureID());
			GLuint sampler = m_shader->GetUniformLocation("sp");
			if (sampler == INVALID_UNIFORM_LOCATION)
			{
				printf("Sampler Not found not found\n");
			}
			glUniform1i(sampler, 0);
		}
		m_outerAsteroids->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture, 150);
		if (m_useInstancing != INVALID_UNIFORM_LOCATION) {
			glUniform1i(m_useInstancing, 0);
		}
	}

	// Get any errors from OpenGL
	auto error = glGetError();
	if (error != GL_NO_ERROR)
	{
		string val = ErrorString(error);
		std::cout << "Error initializing OpenGL! " << error << ", " << val << std::endl;
	}
}


bool Graphics::collectShPrLocs() {
	bool anyProblem = true;
	// Locate the projection matrix in the shader
	m_projectionMatrix = m_shader->GetUniformLocation("projectionMatrix");
	if (m_projectionMatrix == INVALID_UNIFORM_LOCATION)
	{
		printf("m_projectionMatrix not found\n");
		anyProblem = false;
	}

	// Locate the view matrix in the shader
	m_viewMatrix = m_shader->GetUniformLocation("viewMatrix");
	if (m_viewMatrix == INVALID_UNIFORM_LOCATION)
	{
		printf("m_viewMatrix not found\n");
		anyProblem = false;
	}

	// Locate the model matrix in the shader
	m_modelMatrix = m_shader->GetUniformLocation("modelMatrix");
	if (m_modelMatrix == INVALID_UNIFORM_LOCATION)
	{
		printf("m_modelMatrix not found\n");
		anyProblem = false;
	}

	// Locate the position vertex attribute
	m_positionAttrib = m_shader->GetAttribLocation("v_position");
	if (m_positionAttrib == -1)
	{
		printf("v_position attribute not found\n");
		anyProblem = false;
	}

	// Locate the color vertex attribute
	m_colorAttrib = m_shader->GetAttribLocation("v_color");
	if (m_colorAttrib == -1)
	{
		printf("v_color attribute not found\n");
		anyProblem = false;
	}

	// Locate the color vertex attribute
	m_tcAttrib = m_shader->GetAttribLocation("v_tc");
	if (m_tcAttrib == -1)
	{
		printf("v_texcoord attribute not found\n");
		anyProblem = false;
	}

	m_hasTexture = m_shader->GetUniformLocation("hasTexture");
	if (m_hasTexture == INVALID_UNIFORM_LOCATION) {
		printf("hasTexture uniform not found\n");
		anyProblem = false;
	}

	m_useInstancing = m_shader->GetUniformLocation("useInstancing");
	if (m_useInstancing == INVALID_UNIFORM_LOCATION) {
		printf("m_useInstancing not found\n");
		anyProblem = false;
	}

	return anyProblem;
}

std::string Graphics::ErrorString(GLenum error)
{
	if (error == GL_INVALID_ENUM)
	{
		return "GL_INVALID_ENUM: An unacceptable value is specified for an enumerated argument.";
	}

	else if (error == GL_INVALID_VALUE)
	{
		return "GL_INVALID_VALUE: A numeric argument is out of range.";
	}

	else if (error == GL_INVALID_OPERATION)
	{
		return "GL_INVALID_OPERATION: The specified operation is not allowed in the current state.";
	}

	else if (error == GL_INVALID_FRAMEBUFFER_OPERATION)
	{
		return "GL_INVALID_FRAMEBUFFER_OPERATION: The framebuffer object is not complete.";
	}

	else if (error == GL_OUT_OF_MEMORY)
	{
		return "GL_OUT_OF_MEMORY: There is not enough memory left to execute the command.";
	}
	else
	{
		return "None";
	}
}