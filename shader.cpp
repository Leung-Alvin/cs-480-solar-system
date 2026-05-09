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
  //if (ShaderType == GL_VERTEX_SHADER)
  //{
  //    s = "#version 460\n \
  //        \
  //        layout (location = 0) in vec3 v_position; \
  //        layout (location = 1) in vec3 v_color; \
  //        layout (location = 2) in vec2 v_tc;  \
  //           \
  //        out vec3 color; \
  //        out vec2 tc;\
  //        \
  //        uniform mat4 projectionMatrix; \
  //        uniform mat4 viewMatrix; \
  //        uniform mat4 modelMatrix; \
  //        uniform bool hasTC;        \
  //        uniform sampler2D sp; \
  //        \
  //        void main(void) \
  //        { \
  //          vec4 v = vec4(v_position, 1.0); \
  //          gl_Position = (projectionMatrix * viewMatrix * modelMatrix) * v; \
  //          color = v_color; \
  //          tc = v_tc;\
  //        } \
  //        ";
  //}
  if (ShaderType == GL_VERTEX_SHADER)
  {
      s = R"(
        #version 460
        layout (location = 0) in vec3 v_position;
        layout (location = 1) in vec3 v_color;
        layout (location = 2) in vec2 v_tc;
        layout (location = 3) in vec3 v_normal;

        out vec3 color;
        out vec2 tc;
        out vec3 normal;
        out vec3 fragWorldPos; 

        uniform mat4 projectionMatrix;
        uniform mat4 viewMatrix;
        uniform mat4 modelMatrix;

        void main(void)
        {
          vec4 worldPos = modelMatrix * vec4(v_position, 1.0);
          fragWorldPos = worldPos.xyz; 
          
          gl_Position = (projectionMatrix * viewMatrix) * worldPos;
          
          color = v_color;
          tc = v_tc;
          normal = mat3(transpose(inverse(modelMatrix))) * v_normal;
        }
      )";
  }
  else if (ShaderType == GL_FRAGMENT_SHADER)
  {
      s = R"(
        #version 460
        uniform sampler2D sp;
        uniform bool hasTexture;
        uniform bool isSun;
        uniform vec4 GlobalAmbient;
        
        in vec3 color;
        in vec2 tc;
        in vec3 normal;
        in vec3 fragWorldPos; 

        out vec4 frag_color;

        void main(void)
        {
            vec4 baseColor = hasTexture ? texture(sp, tc) : vec4(color, 1.0);

            if (isSun) {
                frag_color = baseColor; 
            } else {
                vec3 lightPos = vec3(0.0, 0.0, 0.0);
                vec3 lightDir = normalize(lightPos - fragWorldPos);
                float diff = max(dot(normalize(normal), lightDir), 0.0);
                frag_color = baseColor * (GlobalAmbient + diff);
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
