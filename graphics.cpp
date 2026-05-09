#include "graphics.h"

Graphics::Graphics()
{
	for (int i = 0; i < 150; i++) {
		randomizers[i] = glm::linearRand(-5, 5);
	}
}

Graphics::~Graphics()
{

}


glm::vec3 Graphics::getPlanetPosition(PlanetID planet) {
	switch (planet) {
	case SUN: return glm::vec3(m_sun->GetModel()[3]);
	case MERCURY: return glm::vec3(m_mercury->GetModel()[3]);
	case VENUS: return glm::vec3(m_venus->GetModel()[3]);
	case EARTH: return glm::vec3(m_earth->GetModel()[3]);
	case MOON: return glm::vec3(m_moon->GetModel()[3]);
	case MARS: return glm::vec3(m_mars->GetModel()[3]);
	case JUPITER: return glm::vec3(m_jupiter->GetModel()[3]);
	case JUPITER_MOON1: return glm::vec3(m_moon1->GetModel()[3]);
	case JUPITER_MOON2: return glm::vec3(m_moon2->GetModel()[3]);
	case SATURN: return glm::vec3(m_saturn->GetModel()[3]);
	case SATURN_MOON1: return glm::vec3(m_moon3->GetModel()[3]);
	case SATURN_MOON2: return glm::vec3(m_moon4->GetModel()[3]);
	case COMET: return glm::vec3(m_comet->GetModel()[3]);
	case URANUS: return glm::vec3(m_uranus->GetModel()[3]);
	case NEPTUNE: return glm::vec3(m_neptune->GetModel()[3]);
	case CERES: return glm::vec3(m_ceres->GetModel()[3]);
	case ERIS: return glm::vec3(m_eris->GetModel()[3]);
	case HAUMEA: return glm::vec3(m_haumea->GetModel()[3]);
	default: return glm::vec3(0.0f);
	}
}

PlanetID Graphics::findClosestPlanetID() {
	glm::vec3 shipPos = glm::vec3(m_mesh->GetModel()[3]);
	PlanetID bestID = SUN;
	float minDist = glm::distance(shipPos, getPlanetPosition(SUN));

	// Array of IDs to check
	PlanetID allPlanets[] = { MERCURY, VENUS, EARTH, MOON, MARS, 
		JUPITER, JUPITER_MOON1, JUPITER_MOON2, SATURN, SATURN_MOON1, SATURN_MOON2, COMET, 
		URANUS, NEPTUNE, CERES, ERIS, HAUMEA };

	for (PlanetID p : allPlanets) {
		float d = glm::distance(shipPos, getPlanetPosition(p));
		if (d < minDist) {
			minDist = d;
			bestID = p;
		}
	}
	return bestID;
}

float Graphics::getPlanetRadius(PlanetID planet) {
	glm::mat4 model;

	switch (planet) {
	case SUN:           model = m_sun->GetModel(); break;
	case MERCURY:       model = m_mercury->GetModel(); break;
	case VENUS:         model = m_venus->GetModel(); break;
	case EARTH:         model = m_earth->GetModel(); break;
	case MOON:          model = m_moon->GetModel(); break;
	case MARS:          model = m_mars->GetModel(); break;
	case JUPITER:       model = m_jupiter->GetModel(); break;
	case JUPITER_MOON1: model = m_moon1->GetModel(); break;
	case JUPITER_MOON2: model = m_moon2->GetModel(); break;
	case SATURN:        model = m_saturn->GetModel(); break;
	case SATURN_MOON1:  model = m_moon3->GetModel(); break;
	case SATURN_MOON2:  model = m_moon4->GetModel(); break;
	case URANUS:        model = m_uranus->GetModel(); break;
	case NEPTUNE:       model = m_neptune->GetModel(); break;
	case CERES:         model = m_ceres->GetModel(); break;
	case ERIS:          model = m_eris->GetModel(); break;
	case HAUMEA:        model = m_haumea->GetModel(); break;
	case COMET:         model = m_comet->GetModel(); break;
	default:            return 1.0f; // Fallback
	}

	// Extract the scale from the model matrix. 
	// Since we use uniform scaling, the length of the first column 
	// vector represents the scaling factor (and thus our radius).
	return glm::length(glm::vec3(model[0]));
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
	glm::vec4 gAmb(0.2f, 0.2f, 0.2f, 1.0f);
	glm::vec4 lAmb(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec4 lDiff(0.8f, 0.8f, 0.8f, 1.0f);
	glm::vec4 lSpec(0.9f, 0.9f, 0.9f, 0.9f);
	glm::vec3 lPos(0.0f, 0.0f, 0.0f);

	float matAmbient[4] = { 0.2f, 0.2f, 0.2f, 1.0f };
	float matDiffuse[4] = { 0.8f, 0.8f, 0.8f, 1.0f };
	float matSpecular[4] = { 0.9f, 0.9f, 0.9f, 0.9f };
	float matShininess = 32.0f;

	m_light = new Light(gAmb, lAmb, lDiff, lSpec, lPos, m_camera->GetView());
	m_material = new Material(matAmbient, matDiffuse, matSpecular,matShininess);
	// Populate location bindings of the shader uniform/attribs
	if (!collectShPrLocs()) {
		printf("Some shader attribs not located!\n");
	}

	// Starship
	//m_mesh = new Mesh(glm::vec3(0.0f, 10.0f, -16.0f), "assets\\SpaceShip-1.obj", "assets\\SpaceShip-1.png");
	m_mesh = new Mesh(glm::vec3(0.0f, 10.0f, -16.0f), "assets\\SpaceShip-1.obj", "assets\\SpaceShip-1.png", "assets\\SpaceShip-1-n.png");
	// position of the starship
	std::vector<float> speed, dist, rotSpeed, scale;
	glm::vec3 rotVector;
	glm::mat4 localTransform;
	modelStack.push(glm::translate(glm::mat4(1.f), glm::vec3(0.0f, 10.0f, -16.0f)));
	localTransform = modelStack.top();
	speed = { 3.0f, 3.0f, 3.0f };
	dist = { 0.0f, 1.25f, 1.25f };
	rotVector = glm::vec3(1.0f, 0.0f, 0.0f);
	rotSpeed = { 0.0f, 0.0f, 0.0f };
	scale = { 0.001f, 0.001f, 0.001f };

	localTransform = modelStack.top();
	localTransform *= glm::rotate(glm::mat4(1.f), rotSpeed[0] * (float)0, rotVector);
	glm::mat4 correction = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	localTransform *= correction;
	localTransform *= glm::scale(glm::vec3(scale[0], scale[1], scale[2]));

	if (m_mesh != NULL)
		m_mesh->Update(localTransform);
	modelStack.pop();

	// The Sun
	m_sun = new Sphere(64, "assets\\2k_sun.jpg");

	// Planets
	m_mercury = new Sphere(48, "assets\\Mercury.jpg", "assets\\Mercury-n.jpg");
	m_venus = new Sphere(48, "assets\\Venus.jpg", "assets\\Venus-n.jpg");
	m_earth = new Sphere(48, "assets\\2k_earth_daymap.jpg", "assets\\2k_earth_daymap-n.jpg");
	m_mars = new Sphere(48, "assets\\Mars.jpg", "assets\\Mars-n.jpg");
	m_jupiter = new Sphere(48, "assets\\Jupiter.jpg", "assets\\Jupiter-n.jpg");
	m_saturn = new Sphere(48, "assets\\Saturn.jpg");
	m_uranus = new Sphere(48, "assets\\Uranus.jpg", "assets\\Uranus-n.jpg");
	m_neptune = new Sphere(48, "assets\\Neptune.jpg", "assets\\Neptune-n.jpg");
	
	// Dwarf planets
	m_ceres = new Sphere(48, "assets\\Ceres.jpg", "assets\\Ceres-n.jpg");
	m_eris = new Sphere(48, "assets\\Eris.jpg", "assets\\Eris-n.jpg");
	m_haumea = new Sphere(48, "assets\\Haumea.jpg", "assets\\Haumea-n.jpg");

	// The moon
	m_moon = new Sphere(48, "assets\\2k_moon.jpg", "assets\\2k_moon-n.jpg");


	// Jupiter's moons
	m_moon1 = new Sphere(32, "assets\\2k_moon.jpg", "assets\\2k_moon-n.jpg");
	m_moon2 = new Sphere(32, "assets\\2k_moon.jpg", "assets\\2k_moon-n.jpg");

	// Saturn's moons
	m_moon3 = new Sphere(32, "assets\\2k_moon.jpg", "assets\\2k_moon-n.jpg");
	m_moon4 = new Sphere(32, "assets\\2k_moon.jpg", "assets\\2k_moon-n.jpg");
	// Independent moon orbiting the sun
	m_comet = new Sphere(40, "assets\\2k_moon.jpg", "assets\\2k_moon-n.jpg");

	// Skybox - use a large cube with a space texture
	m_skybox = new Mesh(glm::vec3(0.0f, 0.0f, 0.0f), "assets\\Galaxy-cubemap2.png");

	m_asteroidCount = 50;
	m_asteroids = new Asteroid("assets\\2k_moon.jpg");
	//m_asteroids = new Asteroid("assets\\2k_moon.jpg", "assets\\2k_moon-n.jpg");

	m_outerAsteroidCount = 150;
	m_outerAsteroids = new Asteroid("assets\\2k_moon.jpg");
	//m_outerAsteroids = new Asteroid("assets\\2k_moon.jpg", "assets\\2k_moon-n.jpg");

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

  if (m_light != NULL) {
	  m_light->updateViewSpacePosition(m_camera->GetView());
  }
  
  if (m_skybox != NULL) {
    glm::mat4 skyboxTransform = glm::scale(glm::mat4(1.0f), glm::vec3(200.0f, 200.0f, 200.0f));
    m_skybox->Update(skyboxTransform);
  }
  
  glm::vec3 shipPos = glm::vec3(m_mesh->GetModel()[3]);

  // position of the sun	
	modelStack.push(glm::translate(glm::mat4(1.f), glm::vec3(0, 0, 0)));
	localTransform = modelStack.top();
	localTransform *= glm::rotate(glm::mat4(1.0f), (float)dt * 0.5f, glm::vec3(0.f, 1.f, 0.f));
	localTransform *= glm::scale(glm::vec3(3.0f, 3.0f, 3.0f));
	glm::vec3 sunPos = glm::vec3(localTransform[3]);

	m_nearestPlanetPos = sunPos; // Initialize nearest planet position to the sun's position

	if (m_sun != NULL)
		m_sun->Update(localTransform);

	// Mercury
	speed = { 1.5f, 0.0f, 1.5f };
	dist = { spacingScale * 2.0f, spacingScale * 0.0f, spacingScale * 2.0f };
	rotVector = glm::vec3(0.0f, 1.0f, 0.0f);
	rotSpeed = { 3.0f, 3.0f, 3.0f };
	scale = { 0.2f, 0.2f, 0.2f };
	localTransform = modelStack.top();
	localTransform *= glm::translate(glm::mat4(1.f),
		glm::vec3(cos(speed[0] * dt) * dist[0], 0.0f, sin(speed[2] * dt) * dist[2]));
	localTransform *= glm::rotate(glm::mat4(1.f), rotSpeed[0] * (float)dt, rotVector);
	localTransform *= glm::scale(glm::vec3(scale[0], scale[1], scale[2]));
	glm::vec3 mercuryPos = glm::vec3(localTransform[3]);
	if (glm::distance(shipPos, mercuryPos) < glm::distance(shipPos, m_nearestPlanetPos)) {
		m_nearestPlanetPos = mercuryPos;
	}
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
	glm::vec3 venusPos = glm::vec3(localTransform[3]);
	if (glm::distance(shipPos, venusPos) < glm::distance(shipPos, m_nearestPlanetPos)) {
		m_nearestPlanetPos = venusPos;
	}
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

	glm::vec3 earthPos = glm::vec3(localTransform[3]);
	if (glm::distance(shipPos, earthPos) < glm::distance(shipPos, m_nearestPlanetPos)) {
		m_nearestPlanetPos = earthPos;
	}

	if (m_earth != NULL)
		m_earth->Update(localTransform);



	// position of moon
	speed = { 2.0f, 2.0f, 2.0f };
	dist = { spacingScale * 1.25f, spacingScale * 1.25f, spacingScale * 1.25f };
	rotVector = glm::vec3(1.0f, 1.0f, 0.0f);
	rotSpeed = { 0.125f, 0.125f, 0.125f };
	scale = { 0.27f, 0.27f, 0.27f };
	
	localTransform = modelStack.top();
	localTransform *= glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(0.0f, 0.0f, -1.0f));
	localTransform *= glm::translate(glm::mat4(1.f),
		glm::vec3(cos(speed[0] * dt) * dist[0], sin(speed[1] * dt) * dist[1], sin(speed[2] * dt) * dist[2]));
	modelStack.push(localTransform);
	localTransform *= glm::rotate(glm::mat4(1.f), rotSpeed[0] * (float)dt, rotVector);
	localTransform *= glm::scale(glm::vec3(scale[0], scale[1], scale[2]));

	glm::vec3 moonPos = glm::vec3(localTransform[3]);
	if (glm::distance(shipPos, moonPos) < glm::distance(shipPos, m_nearestPlanetPos)) {
		m_nearestPlanetPos = moonPos;
	}

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

	glm::vec3 marsPos = glm::vec3(localTransform[3]);
	if (glm::distance(shipPos, marsPos) < glm::distance(shipPos, m_nearestPlanetPos)) {
		m_nearestPlanetPos = marsPos;
	}

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
	modelStack.push(localTransform);
	localTransform *= glm::rotate(glm::mat4(1.f), rotSpeed[0] * (float)dt, rotVector);
	localTransform *= glm::scale(glm::vec3(scale[0], scale[1], scale[2]));


	glm::vec3 jupiterPos = glm::vec3(localTransform[3]);
	if (glm::distance(shipPos, jupiterPos) < glm::distance(shipPos, m_nearestPlanetPos)) {
		m_nearestPlanetPos = jupiterPos;
	}

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

	modelStack.push(localTransform);

	localTransform *= glm::rotate(glm::mat4(1.f), rotSpeed[0] * (float)dt, rotVector);
	localTransform *= glm::scale(glm::vec3(scale[0], scale[1], scale[2]));

	glm::vec3 saturnPos = glm::vec3(localTransform[3]);
	if (glm::distance(shipPos, saturnPos) < glm::distance(shipPos, m_nearestPlanetPos)) {
		m_nearestPlanetPos = saturnPos;
	}
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

	glm::vec3 uranusPos = glm::vec3(localTransform[3]);
	if (glm::distance(shipPos, uranusPos) < glm::distance(shipPos, m_nearestPlanetPos)) {
		m_nearestPlanetPos = uranusPos;
	}

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

	glm::vec3 neptunePos = glm::vec3(localTransform[3]);
	if (glm::distance(shipPos, neptunePos) < glm::distance(shipPos, m_nearestPlanetPos)) {
		m_nearestPlanetPos = neptunePos;
	}

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

	glm::vec3 ceresPos = glm::vec3(localTransform[3]);

	if (glm::distance(shipPos, ceresPos) < glm::distance(shipPos, m_nearestPlanetPos)) {
		m_nearestPlanetPos = ceresPos;
	}

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

	glm::vec3 erisPos = glm::vec3(localTransform[3]);
	if (glm::distance(shipPos, erisPos) < glm::distance(shipPos, m_nearestPlanetPos)) {
		m_nearestPlanetPos = erisPos;
	}

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

	glm::vec3 haumeaPos = glm::vec3(localTransform[3]);
	if (glm::distance(shipPos, haumeaPos) < glm::distance(shipPos, m_nearestPlanetPos)) {
		m_nearestPlanetPos = haumeaPos;
	}

	if (m_haumea != NULL)
		m_haumea->Update(localTransform);

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
	bool anyProblem = true;
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Start the correct program
	m_shader->Enable();

	glm::vec3 cameraPos = glm::vec3(m_camera->GetView()[3]);
	GLint viewPosLoc = m_shader->GetUniformLocation("viewPos");
	glUniform3fv(viewPosLoc, 1, glm::value_ptr(cameraPos));
	if (m_light != nullptr) {
		glUniform4fv(m_globalAmbLoc, 1, &m_light->m_globalAmbient[0]);
		glUniform4fv(m_lightAmbLoc, 1, &m_light->m_lightAmbient[0]);
		glUniform4fv(m_lightDiffLoc, 1, &m_light->m_lightDiffuse[0]);
		glUniform4fv(m_lightSpecLoc, 1, &m_light->m_lightSpecular[0]);
		glUniform3fv(m_lightPosLoc, 1, &m_light->m_lightPosition[0]);
	}

	if (m_material != nullptr) {
		glUniform4fv(mAmbLoc, 1, &m_material->matAmbient[0]);
		glUniform4fv(mDiffLoc, 1, &m_material->matDiffuse[0]);
		glUniform4fv(mSpecLoc, 1, &m_material->matSpecular[0]);
		glUniform1f(mShineLoc, m_material->matShininess);
	}




	// Send in the projection and view to the shader (stay the same while camera intrinsic(perspective) and extrinsic (view) parameters are the same
	glUniformMatrix4fv(m_projectionMatrix, 1, GL_FALSE, glm::value_ptr(m_camera->GetProjection()));
	glUniformMatrix4fv(m_viewMatrix, 1, GL_FALSE, glm::value_ptr(m_camera->GetView()));


	if (m_skybox != NULL) {
		glDepthMask(GL_FALSE); // Disable depth writing for 
		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_skybox->GetModel()));
		if (m_skybox->hasTex) {
			glUniform1i(m_hasTexture, 1);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_skybox->getTextureID());
			GLuint sampler = m_shader->GetUniformLocation("sp");
			if (sampler == INVALID_UNIFORM_LOCATION) {
				printf("Sampler Not found\n");
			}
			glUniform1i(sampler, 0);
			m_skybox->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture, m_normalAttrib);
		}
		glDepthMask(GL_TRUE); // Re-enable depth writing
	}

	// Render mesh (starship)
	//if (m_mesh != NULL && m_showShip) {
	//	glUniform1i(m_hasTexture, false);
	//	glUniformMatrix3fv(m_normalMatrix, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(glm::mat3(m_camera->GetView() * m_mesh->GetModel())))));
	//	glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_mesh->GetModel()));
	//	if (m_mesh->hasTex) {
	//		glActiveTexture(GL_TEXTURE0);
	//		glBindTexture(GL_TEXTURE_2D, m_mesh->getTextureID());
	//		GLuint sampler = m_shader->GetUniformLocation("sp");
	//		if (sampler != INVALID_UNIFORM_LOCATION) {
	//			glUniform1i(sampler, 0);
	//		}
	//		m_mesh->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture, m_normalAttrib);
	//	}
	//}
	if (m_mesh != NULL && m_showShip) {
		glUniform1i(m_hasTexture, false);
		glm::mat4 modelView = m_camera->GetView() * m_mesh->GetModel();
		glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelView)));

		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_mesh->GetModel()));

		GLuint hasN = m_shader->GetUniformLocation("hasNormalMap");
		if (m_mesh->getTextureID()) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_mesh->getTextureID());
			GLuint sampler = m_shader->GetUniformLocation("sp");
			if (sampler == INVALID_UNIFORM_LOCATION) {
				printf("Sampler Not found not found\n");
			}
			glUseProgram(m_shader->GetShaderProgram());
			glUniform1i(sampler, 0);
			glUniform1i(hasN, false);
		}
		if (m_mesh->getNormalID()) {
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, m_mesh->getNormalID());
			GLuint normalSampler = m_shader->GetUniformLocation("sp2");
			if (normalSampler == INVALID_UNIFORM_LOCATION) {
				printf("Normal Sampler Not found not found\n");
			}
			glUseProgram(m_shader->GetShaderProgram());
			glUniform1i(normalSampler, 1);
			glUniform1i(hasN, true);
		}
		m_mesh->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture, m_normalAttrib);
	}

	// Render Sun
	if (m_sun != NULL) {
		glUniform1i(m_isSunLoc, 1);
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
			m_sun->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture, m_normalAttrib);
		}
		glUniform3f(m_shader->GetUniformLocation("light.position"), 0.0f, 0.0f, 0.0f);
		glUniform1i(m_isSunLoc, 0);
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
			m_mercury->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture, m_normalAttrib);
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
			m_venus->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture, m_normalAttrib);
		}
	}

	// Render Earth
	if (m_earth != NULL) {
		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_earth->GetModel()));

		GLuint hasN = m_shader->GetUniformLocation("hasNormalMap");
		if (m_earth->getTextureID()) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_earth->getTextureID());
			GLuint sampler = m_shader->GetUniformLocation("sp");
			if (sampler == INVALID_UNIFORM_LOCATION) {
				printf("Sampler Not found not found\n");
			}
			glUseProgram(m_shader->GetShaderProgram());
			glUniform1i(sampler, 0);
			glUniform1i(hasN, false);
		}
		if (m_earth->getNormalID()) {
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, m_earth->getNormalID());
			GLuint normalSampler = m_shader->GetUniformLocation("sp2");
			if (normalSampler == INVALID_UNIFORM_LOCATION) {
				printf("Normal Sampler Not found not found\n");
			}
			glUseProgram(m_shader->GetShaderProgram());
			glUniform1i(normalSampler, 1);
			glUniform1i(hasN, true);
		}
		m_earth->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture, m_normalAttrib);
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
			m_moon->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture, m_normalAttrib);
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
			m_mars->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture, m_normalAttrib);
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
			m_jupiter->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture, m_normalAttrib);
		}
	}


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
			m_moon1->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture, m_normalAttrib);
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
			m_moon2->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture, m_normalAttrib);
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
			m_saturn->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture, m_normalAttrib);
		}
	}

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
			m_moon3->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture, m_normalAttrib);
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
			m_moon4->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture, m_normalAttrib);
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
			m_uranus->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture, m_normalAttrib);
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
			m_neptune->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture, m_normalAttrib);
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
			m_ceres->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture, m_normalAttrib);
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
			m_eris->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture, m_normalAttrib);
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
			m_haumea->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture, m_normalAttrib);
		}
	}

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
			m_comet->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture, m_normalAttrib);
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
		m_asteroids->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture, m_normalAttrib, 100);
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
		m_outerAsteroids->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture,m_normalAttrib, 150);
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

	m_useInstancing = m_shader->GetUniformLocation("useInstancing");
	if (m_useInstancing == INVALID_UNIFORM_LOCATION) {
		printf("m_useInstancing not found\n");
		anyProblem = false;
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

	m_colorAttrib = m_shader->GetAttribLocation("v_color");
	if (m_colorAttrib == -1)
	{
		printf("v_color attribute not found\n");
		anyProblem = false;
	}

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

	m_hasNorm = m_shader->GetUniformLocation("hasNormalMap");
	if (m_hasNorm == INVALID_UNIFORM_LOCATION) {
		printf("hasNormalMap uniform not found\n");
		anyProblem = false;
	}

	m_normalAttrib = m_shader->GetAttribLocation("v_normal");
	if (m_normalAttrib == -1) {
		printf("v_normal attribute not found! Check your vertex shader.\n");
		anyProblem = false;
	}

	m_isSunLoc = m_shader->GetUniformLocation("isSun");
	if (m_isSunLoc == INVALID_UNIFORM_LOCATION) {
		printf("isSun uniform not found! Check your fragment shader.\n");
		anyProblem = false;
	}

	GLuint globalAmbLoc = glGetUniformLocation(m_shader->GetShaderProgram(), "GlobalAmbient");
	if (globalAmbLoc == INVALID_UNIFORM_LOCATION) {
		printf("GlobalAmbient uniform not found! Check your fragment shader.\n");
		anyProblem = false;
	}
	glProgramUniform4fv(m_shader->GetShaderProgram(), globalAmbLoc, 1, glm::value_ptr(m_light->m_globalAmbient));

	m_globalAmbLoc = m_shader->GetUniformLocation("GlobalAmbient");
	if (m_globalAmbLoc == INVALID_UNIFORM_LOCATION) {
		printf("GlobalAmbient uniform not found! Check your fragment shader.\n");
		anyProblem = false;
	}
	m_lightAmbLoc = m_shader->GetUniformLocation("light.ambient");
	if (m_lightAmbLoc == INVALID_UNIFORM_LOCATION) {
		printf("LightAmbient uniform not found! Check your fragment shader.\n");
		anyProblem = false;
	}
	m_lightDiffLoc = m_shader->GetUniformLocation("light.diffuse");
	if (m_lightDiffLoc == INVALID_UNIFORM_LOCATION) {
		printf("LightDiffuse uniform not found! Check your fragment shader.\n");
		anyProblem = false;
	}
	m_lightSpecLoc = m_shader->GetUniformLocation("light.specular");
	if (m_lightSpecLoc == INVALID_UNIFORM_LOCATION) {
		printf("LightSpecular uniform not found! Check your fragment shader.\n");
		anyProblem = false;
	}
	m_lightPosLoc = m_shader->GetUniformLocation("light.position");
	if (m_lightPosLoc == INVALID_UNIFORM_LOCATION) {
		printf("LightPosition uniform not found! Check your fragment shader.\n");
		anyProblem = false;
	}


	mAmbLoc = m_shader->GetUniformLocation("material.ambient");
	mDiffLoc = m_shader->GetUniformLocation("material.diffuse");
	mSpecLoc = m_shader->GetUniformLocation("material.specular");
	mShineLoc = m_shader->GetUniformLocation("material.shininess");


	if (m_globalAmbLoc == INVALID_UNIFORM_LOCATION || m_lightPosLoc == INVALID_UNIFORM_LOCATION) {
		printf("Warning: Some lighting uniforms optimized out or missing.\n");
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

void Graphics::UpdateCamera(float x, float y, bool l, bool r, bool u, bool d) {
	m_camera->Update(x, y, l, r, u, d);
}