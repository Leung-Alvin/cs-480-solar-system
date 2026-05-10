#include "shader.h"

Shader::Shader()
{
  m_shaderProg = 0;
}

Shader::~Shader()
{
  for (std::vector<GLuint>::iterator it = m_shaderObjList.begin() ; it != m_shaderObjList.end() ; it++)
  {
    glDeleteShader(*it);
  }

  if (m_shaderProg != 0)
  {
    glDeleteProgram(m_shaderProg);
    m_shaderProg = 0;
  }
}

bool Shader::Initialize()
{
  m_shaderProg = glCreateProgram();

  if (m_shaderProg == 0) 
  {
    std::cerr << "Error creating shader program\n";
    return false;
  }

  return true;
}

// Use this method to add shaders to the program. When finished - call finalize()
bool Shader::AddShader(GLenum ShaderType)
{
  std::string s;
  // The vertex shader includes support for instancing, allowing multiple instances of the same geometry to be rendered with different transformations. 
  // It also calculates the normal vectors for lighting and passes necessary data to the fragment shader.
  if (ShaderType == GL_VERTEX_SHADER)
  {
      s = R"(
        #version 460
        layout (location = 0) in vec3 v_position;
        layout (location = 1) in vec3 v_normal;
        layout (location = 2) in vec2 v_tc;

        layout(location = 12) in vec4 instanceMatrix_col0;
        layout(location = 13) in vec4 instanceMatrix_col1;
        layout(location = 14) in vec4 instanceMatrix_col2;
        layout(location = 15) in vec4 instanceMatrix_col3;

        out vec3 color;
        out vec2 tc;
        out vec3 normal;
        out vec3 fragWorldPos;
        out vec3 localPos;

        uniform mat4 projectionMatrix;
        uniform mat4 viewMatrix;
        uniform mat4 modelMatrix;
        uniform int useInstancing;

        uniform sampler2D sp;

        void main(void)
        {
          localPos = v_position;
          vec4 worldPos = modelMatrix * vec4(v_position, 1.0);
          fragWorldPos = worldPos.xyz; 
          
          
          color = texture(sp, v_tc).rgb;
          tc = v_tc;

          mat4 finalModel = modelMatrix;
          if (useInstancing == 1) {
              finalModel = mat4(instanceMatrix_col0, instanceMatrix_col1, instanceMatrix_col2, instanceMatrix_col3) * modelMatrix;
          }
          else {
              finalModel = modelMatrix;
          }
          
          normal = mat3(transpose(inverse(finalModel))) * v_normal;
          gl_Position = (projectionMatrix * viewMatrix) * finalModel * vec4(v_position, 1.0);
        }

      )";
  }
  // The fragment shader includes support for normal mapping, positional lighting, and a special effect for ships based on their speed.
  else if (ShaderType == GL_FRAGMENT_SHADER)
  {
      s = R"(
        #version 460
        uniform sampler2D sp;
        uniform sampler2D sp2;
        uniform bool hasNormalMap;
        uniform bool isSun;

        struct PositionalLight {
            vec4 position;
            vec4 ambient;
            vec4 diffuse;
            vec4 specular;
        };

        uniform vec4 GlobalAmbient;
        uniform PositionalLight light;

        struct Material {
            vec4 ambient;
            vec4 diffuse;
            vec4 specular;
            float shininess;
        };

        uniform Material material;
        uniform vec3 viewPos;
        
        in vec3 color;
        in vec2 tc;
        in vec3 normal;
        in vec3 fragWorldPos;
        in vec3 localPos;

        uniform bool isShip;
        uniform float shipSpeedRatio;

        out vec4 frag_color;

        void main(void)
        {
            vec4 baseColor = vec4(color, 1.0);

            if (isSun) {
                frag_color = baseColor; 
            } else {
                vec3 norm = normalize(normal);

                if (hasNormalMap) {
                    vec3 normalMap = texture(sp2, tc).rgb;
                    normalMap = normalize(normalMap * 2.0 - 1.0);

                    vec3 Q1 = dFdx(fragWorldPos);
                    vec3 Q2 = dFdy(fragWorldPos);
                    vec2 st1 = dFdx(tc);
                    vec2 st2 = dFdy(tc);
                    vec3 N = normalize(normal);
                    vec3 T = normalize(Q1 * st2.t - Q2 * st1.t);
                    vec3 B = -normalize(cross(N, T));
                    mat3 TBN = mat3(T, B, N);
                    norm = normalize(TBN * normalMap);
                }
                                

                vec3 lightDir = normalize(light.position.xyz - fragWorldPos);
                vec3 viewDir = normalize(viewPos - fragWorldPos);

                vec4 ambient = (GlobalAmbient + light.ambient) * material.ambient;
                float diff = max(dot(norm, lightDir), 0.0);
                vec4 diffuse = light.diffuse * (diff * material.diffuse);

                vec3 reflectDir = reflect(-lightDir, norm);
                float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
                vec4 specular = light.specular * (spec * material.specular);

                vec3 emission = vec3(0.0);
                if (isShip) {
                    float engineMask = smoothstep(-0.2, -0.5, localPos.z);

                    
                    vec3 idleColor = vec3(0.8, 0.2, 0.0);
                    vec3 fastColor = vec3(0.2,0.8,1.0);
                    vec3 glowColor = mix(idleColor, fastColor, shipSpeedRatio);

                    emission = glowColor * (0.2 + shipSpeedRatio * 2.0) * engineMask;
                    //emission = vec3(0.0, 1.0, 1.0) * shipSpeedRatio;
                }

                //vec4 finalResult = (ambient + diffuse) * baseColor + specular;
                vec4 finalResult = (ambient + diffuse) * baseColor + specular + vec4(emission, 0.0);
                frag_color = vec4(finalResult.rgb, 1.0);
            }
        }
      )";
  }

  GLuint ShaderObj = glCreateShader(ShaderType);

  if (ShaderObj == 0) 
  {
    std::cerr << "Error creating shader type " << ShaderType << std::endl;
    return false;
  }

  // Save the shader object - will be deleted in the destructor
  m_shaderObjList.push_back(ShaderObj);

  const GLchar* p[1];
  p[0] = s.c_str();
  GLint Lengths[1] = { (GLint)s.size() };

  glShaderSource(ShaderObj, 1, p, Lengths);

  glCompileShader(ShaderObj);

  GLint success;
  glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);

  if (!success) 
  {
    GLchar InfoLog[1024];
    glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
    std::cerr << "Error compiling: " << InfoLog << std::endl;
    return false;
  }

  glAttachShader(m_shaderProg, ShaderObj);

  return true;
}


// After all the shaders have been added to the program call this function
// to link and validate the program.
bool Shader::Finalize()
{
  GLint Success = 0;
  GLchar ErrorLog[1024] = { 0 };

  glLinkProgram(m_shaderProg);

  glGetProgramiv(m_shaderProg, GL_LINK_STATUS, &Success);
  if (Success == 0)
  {
    glGetProgramInfoLog(m_shaderProg, sizeof(ErrorLog), NULL, ErrorLog);
    std::cerr << "Error linking shader program: " << ErrorLog << std::endl;
    return false;
  }

  glValidateProgram(m_shaderProg);
  glGetProgramiv(m_shaderProg, GL_VALIDATE_STATUS, &Success);
  if (!Success)
  {
    glGetProgramInfoLog(m_shaderProg, sizeof(ErrorLog), NULL, ErrorLog);
    std::cerr << "Invalid shader program: " << ErrorLog << std::endl;
    return false;
  }

  // Delete the intermediate shader objects that have been added to the program
  for (std::vector<GLuint>::iterator it = m_shaderObjList.begin(); it != m_shaderObjList.end(); it++)
  {
    glDeleteShader(*it);
  }

  m_shaderObjList.clear();

  return true;
}


void Shader::Enable()
{
    glUseProgram(m_shaderProg);
}


GLint Shader::GetUniformLocation(const char* pUniformName)
{
    GLuint Location = glGetUniformLocation(m_shaderProg, pUniformName);

    if (Location == INVALID_UNIFORM_LOCATION) {
        fprintf(stderr, "Warning! Unable to get the location of uniform '%s'\n", pUniformName);
    }

    return Location;
}

GLint Shader::GetAttribLocation(const char* pAttribName)
{
    GLuint Location = glGetAttribLocation(m_shaderProg, pAttribName);

    if (Location == -1) {
        fprintf(stderr, "Warning! Unable to get the location of attribute '%s'\n", pAttribName);
    }

    return Location;
}