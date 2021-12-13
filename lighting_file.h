#pragma once
#include <glm/trigonometric.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "glm/gtx/transform.hpp"
#include "figure.h"
#include "figure_model.h"
#define TO_STRING(x) #x
class lighting_file :
    public figure
{
    // Переменные с индентификаторами ID
    // ID шейдерной программы
    GLuint Program;
    // ID атрибута
    GLint Attrib_vertex;
    GLuint attribTexture;
    GLuint attribNormals;
    // ID Vertex Buffer Object
    GLuint VBO;
    GLuint normalsVBO;
    //GLuint textureVBO;
    // ID юниформ переменной перемещения
    GLint Unif_trans;

    float xyz[3] = { 0.0f, 0.0f, 0.0f };

    figure_model fm = figure_model("figure.obj");

    // Вершина
    struct Vertex
    {
        GLfloat x;
        GLfloat y;
        GLfloat z;
    };

   
    struct Transform {
        glm::mat4 model;
        glm::mat4 viewProjection;
        glm::mat3 normal;
        glm::vec3 viewPosition;
    } transform;

    struct PointLight {
        glm::vec4 position;
        glm::vec4 ambient;
        glm::vec4 diffuse;
        glm::vec4 specular;
        glm::vec3 attenuation;
    } light;

    struct Material {
        glm::vec4 ambient;
        glm::vec4 diffuse;
        glm::vec4 specular;
        glm::vec4 emission;
        float shininess;
    } material;

    // Исходный код вершинного шейдера
    const char* VertexShaderSource = R"(
    #version 330 core

    in vec3 position;
    in vec3 normal;
    
    uniform vec3 xyz;

    out struct Vertex {
	//vec2 texcoord;
	vec3 normal;
	vec3 lightDir;
	vec3 viewDir;
	float distance;
    } Vert;

    uniform struct Transform {
	mat4 model;
	mat4 viewProjection;
	mat3 normal;
	vec3 viewPosition;
    } transform;

    uniform struct PointLight{
	vec4 position;
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	vec3 attenuation;
    } light;

    void main() {
        float x_angle = xyz[0];
        float y_angle = xyz[1];
		float z_angle = xyz[2];
        
        mat3 r = mat3(
            1, 0, 0,
            0, cos(x_angle), -sin(x_angle),
            0, sin(x_angle), cos(x_angle)
        ) * mat3(
            cos(y_angle), 0, sin(y_angle),
            0, 1, 0,
            -sin(y_angle), 0, cos(y_angle)
        ) * mat3(
			cos(z_angle), -sin(z_angle), 0,
			sin(z_angle), cos(z_angle), 0,
			0, 0, 1);
        vec3 vertex = position * r;
        gl_Position = vec4(vertex, 1.0);
        vec4 lightDir = light.position - vec4(vertex,1.0);
	    //Vert.texcoord = texcoord;
	    Vert.normal = r * normal;
	    Vert.lightDir = vec3(lightDir);
	    Vert.viewDir = transform.viewPosition - vertex;
	    Vert.distance = length(lightDir);

    }
)";

    // Исходный код фрагментного шейдера
    const char* FragShaderSource = R"(
    #version 330 core
    #define FRAG_OUTPUT0 0
    layout(location = FRAG_OUTPUT0) out vec4 color;
    
    uniform struct PointLight{
	vec4 position;
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	vec3 attenuation;
    } light;
    
    uniform struct Material {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    vec4 emission;
    float shininess;
    } material;

    in struct Vertex {
	//vec2 texcoord;
	vec3 normal;
	vec3 lightDir;
	vec3 viewDir;
	float distance;
    } Vert;

    void main() 
    {

        //Phong-Blinn
        vec3 normal = normalize(Vert.normal);
	    vec3 lightDir = normalize(Vert.lightDir);
	    vec3 viewDir = normalize(Vert.viewDir);
        float attenuation = 1.0/(light.attenuation[0] + light.attenuation[1] * Vert.distance + light.attenuation[2] * Vert.distance * Vert.distance); 	
        color = material.emission;
        color += material.ambient * light.ambient * attenuation;
        float Ndot = max(dot(normal,lightDir),0.0);
        color += material.diffuse * light.diffuse * Ndot* attenuation;
        float RdotVpow = max(pow(dot(reflect (-lightDir, normal), viewDir), material.shininess),0.0);
        color += material.specular * light.specular * RdotVpow * attenuation;

       //Toon shading
       /*vec3 normal = normalize(Vert.normal);
	   vec3 lightDir = normalize(Vert.lightDir);
	   float diff = 0.2 + max(dot(normal, lightDir), 0.0);
       if(diff < 0.4)
        color = material.diffuse * 0.3;
       else if(diff < 0.7)
        color = material.diffuse;
       else
        color = material.diffuse * 1.3;*/

   //Minnaert
    
  /*  const float k = 0.8;
    vec3  n2 = normalize (Vert.normal);
    vec3  l2 = normalize (Vert.lightDir);
    vec3  v2 = normalize (Vert.viewDir);
    float d1 = pow(max(dot(n2, l2), 0.0), 1.0 + k);
    float d2 = pow(1.0 - dot(n2, v2), 1.0 - k);
    color = material.diffuse * d1 * d2;*/
    }
)";


    // Проверка ошибок OpenGL, если есть то вывод в консоль тип ошибки
    void checkOpenGLerror() {
        GLenum errCode;
        // Коды ошибок можно смотреть тут
        // https://www.khronos.org/opengl/wiki/OpenGL_Error
        if ((errCode = glGetError()) != GL_NO_ERROR)
            std::cout << "OpenGl error!: " << errCode << std::endl;
    }

    // Функция печати лога шейдера
    void ShaderLog(unsigned int shader)
    {
        int infologLen = 0;
        int charsWritten = 0;
        char* infoLog;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infologLen);
        if (infologLen > 1)
        {
            infoLog = new char[infologLen];
            if (infoLog == NULL)
            {
                std::cout << "ERROR: Could not allocate InfoLog buffer" << std::endl;
                exit(1);
            }
            glGetShaderInfoLog(shader, infologLen, &charsWritten, infoLog);
            std::cout << "InfoLog: " << infoLog << "\n\n\n";
            delete[] infoLog;
        }
    }


    void InitVBO()
    {
        glGenBuffers(1, &normalsVBO);
        //glGenBuffers(1, &textureVBO);
        glGenBuffers(1, &VBO);
        // Вершины кубика
        /*Vertex cube[] = {
            {1.0,-1.0,-1.0},
            {1.0,-1.0,1.0},
            {-1.0,-1.0,1.0},
            {-1.0,-1.0,-1.0},
            {1.0, 1.0, -0.999999},
            {0.999999, 1.000000, 1.000001},
            {-1.000000, 1.000000, 1.000000},
            {-1.000000, 1.000000, - 1.000000},
            {-1.000000, 0.5, -1.000000},
        };*/
      
        auto cube = fm.get_vertices();
      /*  Vertex cube[] = {
            {-0.5,-0.5,0.5},{-0.5,0.5,0.5},{0.5,0.5,0.5},{0.5,-0.5,0.5},
            {-0.5,-0.5,0.5},{-0.5,0.5,0.5},{-0.5,0.5,-0.5},{-0.5,-0.5,-0.5},
            {-0.5,-0.5,-0.5},{-0.5,0.5,-0.5},{0.5,0.5,-0.5},{0.5,-0.5,-0.5},
            {0.5,-0.5,0.5},{0.5,0.5,0.5},{0.5,0.5,-0.5},{0.5,-0.5,-0.5},
            {-0.5,0.5,0.5},{0.5,0.5,0.5},{0.5,0.5,-0.5},{-0.5,0.5,-0.5},
            {-0.5,-0.5,0.5},{0.5,-0.5,0.5},{0.5,-0.5,-0.5},{-0.5,-0.5,-0.5}
        };*/
        // текстурные вершины
        /*float texture[][2] = { //texcoord
            {1.000000, 0.333333},
            {1.000000, 0.666667},
            {0.666667, 0.666667},
            {0.666667, 0.333333},
            {0.666667, 0.000000},
            {0.000000, 0.333333},
            {0.000000, 0.000000},
            {0.333333, 0.000000},
            {0.333333, 1.000000},
            {0.000000, 1.000000},
            {0.000000, 0.666667},
            {0.333333, 0.333333},
            {0.333333, 0.666667},
            {1.000000, 0.000000}
        };*/

        auto normals = fm.get_normals();

      /*  float normals[][3] = {
            {0.000000, -1.000000, 0.000000},
            {0.000000, 1.000000, 0.000000},
            {1.000000, 0.000000, 0.000000},
            {-0.000000, 0.000000, 1.000000},
            {-1.000000, -0.000000, -0.000000},
            {0.000000,0.000000, -1.000000},
            {1.000000, -1.000000, 1.000000},
            {-1.000000, -1.000000, -1.000000},
            {1.000000, 1.000000, 0.000000}
        };*/
        // Передаем вершины в буфер
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, std::get<1>(cube),std::get<0>( cube), GL_STATIC_DRAW);
       // glBindBuffer(GL_ARRAY_BUFFER, textureVBO);
      //  glBufferData(GL_ARRAY_BUFFER, sizeof(texture), texture, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, normalsVBO);
        glBufferData(GL_ARRAY_BUFFER, std::get<1>(normals), std::get<0>(normals), GL_STATIC_DRAW);
        checkOpenGLerror();
    }


    void InitShader() {
        // Создаем вершинный шейдер
        GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
        // Передаем исходный код
        glShaderSource(vShader, 1, &VertexShaderSource, NULL);
        // Компилируем шейдер
        glCompileShader(vShader);
        std::cout << "vertex shader \n";
        ShaderLog(vShader);

        // Создаем фрагментный шейдер
        GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
        // Передаем исходный код
        glShaderSource(fShader, 1, &FragShaderSource, NULL);
        // Компилируем шейдер
        glCompileShader(fShader);
        std::cout << "fragment shader \n";
        ShaderLog(fShader);

        // Создаем программу и прикрепляем шейдеры к ней
        Program = glCreateProgram();
        glAttachShader(Program, vShader);
        glAttachShader(Program, fShader);

        // Линкуем шейдерную программу
        glLinkProgram(Program);
        // Проверяем статус сборки
        int link_ok;
        glGetProgramiv(Program, GL_LINK_STATUS, &link_ok);
        if (!link_ok)
        {
            std::cout << "error attach shaders \n";
            return;
        }

        // Вытягиваем ID атрибута из собранной программы
        const char* attr_name = "position";
        Attrib_vertex = glGetAttribLocation(Program, attr_name);
        if (Attrib_vertex == -1)
        {
            std::cout << "could not bind attrib " << attr_name << std::endl;
            return;
        }

        attribNormals = glGetAttribLocation(Program, "normal");
        if (attribNormals == -1)
        {
            std::cout << "could not bind attrib normal" << std::endl;
            return;
        }
        
        Unif_trans = glGetUniformLocation(Program, "xyz");
        if (Unif_trans == -1)
        {
            std::cout << "could not bind uniform xyz" << std::endl;
            return;
        }

       /*attribTexture = glGetAttribLocation(Program, "texcoord");
        if (attribTexture == -1)
        {
            std::cout << "could not bind attrib texcoord" << std::endl;
            return;
        }*/

        

        checkOpenGLerror();
    }

    void Init() {
        InitShader();
        InitVBO();
        // Включаем проверку глубины
        glEnable(GL_DEPTH_TEST);
    }

    GLint getUniformLocation(const std::string& name) const {
        return glGetUniformLocation(Program, name.c_str());
    }

    void Draw() {
        // Устанавливаем шейдерную программу текущей
        glUseProgram(Program);

        // Включаем массив атрибутов
        glEnableVertexAttribArray(Attrib_vertex);
        glEnableVertexAttribArray(attribTexture);
        glEnableVertexAttribArray(attribNormals);
        // Подключаем VBO

        glBindBuffer(GL_ARRAY_BUFFER, normalsVBO);
        glVertexAttribPointer(attribNormals, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glVertexAttribPointer(Attrib_vertex, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glUniform3fv(Unif_trans, 1, xyz);

       // glBindBuffer(GL_ARRAY_BUFFER, textureVBO);
       // glVertexAttribPointer(attribTexture, 2, GL_FLOAT, GL_FALSE, 0, 0);
        /*
        * struct Transform {
        glm::mat4 model;
        glm::mat4 viewProjection;
        glm::mat3 normal;
        glm::vec3 viewPosition;
    } transform;
        */
        glUniformMatrix4fv(getUniformLocation("transform.model"), 1, GL_FALSE, &transform.model[0][0]);
        glUniformMatrix4fv(getUniformLocation("transform.viewProjection"), 1, GL_FALSE, &transform.viewProjection[0][0]);
        glUniformMatrix3fv(getUniformLocation("transform.normal"), 1, GL_FALSE, &transform.normal[0][0]);
        glUniform3fv(getUniformLocation("transform.viewPosition"), 1, &transform.viewPosition[0]);

        /*
        * struct PointLight {
        glm::vec4 position;
        glm::vec4 ambient;
        glm::vec4 diffuse;
        glm::vec4 specular;
        glm::vec3 attenuation;
    } light;
        */
        glUniform4fv(getUniformLocation("light.position"), 1, &light.position[0]);
        glUniform4fv(getUniformLocation("light.ambient"), 1, &light.ambient[0]);
        glUniform4fv(getUniformLocation("light.diffuse"), 1, &light.diffuse[0]);
        glUniform4fv(getUniformLocation("light.specular"), 1, &light.specular[0]);
        glUniform3fv(getUniformLocation("light.attenuation"), 1, &light.attenuation[0]);

        /*
        * struct Material {
        glm::vec4 ambient;
        glm::vec4 diffuse;
        glm::vec4 specular;
        glm::vec4 emission;
        float shininess;
    } material;
        */

        glUniform4fv(getUniformLocation("material.ambient"), 1, &material.ambient[0]);
        glUniform4fv(getUniformLocation("material.diffuse"), 1, &material.diffuse[0]);
        glUniform4fv(getUniformLocation("material.specular"), 1, &material.specular[0]);
        glUniform4fv(getUniformLocation("material.emission"), 1, &material.emission[0]);
        glUniform1f(getUniformLocation("material.shininess"), material.shininess);
        // Отключаем VBO
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // Передаем данные на видеокарту(рисуем)
        glDrawArrays(GL_TRIANGLES, 0, fm.count());
        // Отключаем массив атрибутов
        glDisableVertexAttribArray(Attrib_vertex);
        glDisableVertexAttribArray(attribTexture);
        glDisableVertexAttribArray(attribNormals);
        // Отключаем шейдерную программу
        glUseProgram(0);
        checkOpenGLerror();
    }


    // Освобождение шейдеров
    void ReleaseShader() {
        // Передавая ноль, мы отключаем шейдрную программу
        glUseProgram(0);
        // Удаляем шейдерную программу
        glDeleteProgram(Program);
    }

    // Освобождение буфера
    void ReleaseVBO()
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &normalsVBO);
        //glDeleteBuffers(1, &textureVBO);
    }

    void Release() {
        ReleaseShader();
        ReleaseVBO();
    }

public:

    void init(const std::string& path)
    {
        fm = figure_model(path);
    }

    int Paint() override {
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 50.0f);
        float angle = 0.01f;
        glm::mat4 model = { glm::cos(angle), 0.0f, glm::sin(angle), 0.0f,
                       0.0f, 1, 0, 0.0f,
                       -glm::sin(angle),0, glm::cos(angle), 0.0f,
                       0.0f, 0.0f, 0.0f, 1.0f };

        glm::mat4 view = glm::lookAt(glm::vec3(5, 5, 5), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
        glm::mat4 viewProjection = projection * view;
        glm::mat3 normal = glm::transpose(glm::inverse(model));
        transform.model = model;
        transform.viewProjection = viewProjection;
        transform.normal = normal;
        transform.viewPosition = glm::vec3(5, 3, 3);

        light.ambient = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
        light.diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        light.specular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        light.attenuation = glm::vec3(0.5f, 0.0f, 0.02f);
        light.position = glm::vec4(5.0f, 5.0f, 3.0f,1.0f);

        material.ambient = glm::vec4(0.4f, 0.2f, 0.2f, 1.0f);
        material.diffuse = glm::vec4(0.3f, 0.5f, 1.0f, 1.0f);
        material.emission = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);
        material.specular = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        material.shininess = 5.0f;
        sf::Window window(sf::VideoMode(600, 600), "My OpenGL window", sf::Style::Default, sf::ContextSettings(24));
        window.setVerticalSyncEnabled(true);

        window.setActive(true);

        // Инициализация glew
        glewInit();

        Init();
      

        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                }
                else if (event.type == sf::Event::Resized) {
                    glViewport(0, 0, event.size.width, event.size.height);
                }
                else if (event.type == sf::Event::KeyPressed) {
                    switch (event.key.code) {
                    case (sf::Keyboard::Right): xyz[0] += 0.1; break;
                    case (sf::Keyboard::Up): xyz[1] += 0.1; break;
                    case (sf::Keyboard::Left): xyz[0] -= 0.1; break;
                    case (sf::Keyboard::Down): xyz[1] -= 0.1; break;
                    case (sf::Keyboard::W): xyz[2] += 0.1; break;
                    case (sf::Keyboard::S): xyz[2] -= 0.1; break;
                    default: break;
                    }
                }
            }

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            Draw();

            window.display();
        }

        Release();
        return 0;
    }
};

