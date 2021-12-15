#pragma once
#include "figure.h"
#include "figure_model.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#define TO_STRING(x) #x

class lighting_Toon :
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
    // ID Transform struct
    GLint Unif_transform_model;
    GLint Unif_transform_viewProjection;
    GLint Unif_transform_normal;
    GLint Unif_transform_viewPosition;
    // ID Material struct
    GLint Unif_material_emission;
    GLint Unif_material_ambient;
    GLint Unif_material_diffuse;
    GLint Unif_material_specular;
    GLint Unif_material_shininess;
    // ID LigtPoint struct
    GLint Unif_light_ambient;
    GLint Unif_light_diffuse;
    GLint Unif_light_specular;
    GLint Unif_light_attenuation;
    GLint Unif_light_position;

    float xyz[3] = { 0.5f, 0.5f, 0.0f };

    // Вершина
    struct Vertex
    {
        GLfloat x;
        GLfloat y;
        GLfloat z;
    };

    figure_model fm = figure_model("figure.obj");



    struct Transform
    {
        float model[4][4] =
        {
            {1.0, 0.0, 0.0, 0.0},
            {0.0, 1.0, 0.0, 0.0},
            {0.0, 0.0, 1.0, 0.0},
            {0.0, 0.0, 0.0, 1.0},
        };
        float viewProjection[4][4] =
        {
            {1.0, 0.0, 0.0, 0.0},
            {0.0, 1.0, 0.0, 0.0},
            {0.0, 0.0, 1.0, 0.0},
            {0.0, 0.0, 0.0, 1.0},
        };
        float normal[3][3] =
        {
            {1.0, 0.0, 0.0},
            {0.0, 1.0, 0.0},
            {0.0, 0.0, 1.0},
        };
        float viewPosition[3] = { -1.0f, -1.0f, -1.0f };
    } transform;

    struct Material
    {
        float ambient[4] = { 0.1745f, 0.01175f, 0.01175f, 1.0f };
        float emission[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
        float diffuse[4] = { 0.61424f,0.04136f, 0.04136f, 1.0f };
        float specular[4] = { 0.727811f, 0.626959f, 0.626959f, 1.0f };
        float shininess = 0.128f;
    } material;

    struct PointLight
    {
        float ambient[4] = { 0.05f, 0.05f, 0.05f, 1.0f };
        float diffuse[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
        float specular[4] = { 0.7f, 0.7f, 0.7f, 0.7f };
        float attenuation[3] = { 1.0f, 0.0f, 0.0f };
        float position[3] = { -1.5f, -1.5f, -1.5f };
    } light;

    // Исходный код вершинного шейдера
    const char* VertexShaderSource = R"(
   #version 330 core

	uniform vec3 xyz;

    in vec3 position;
	in vec3 normal;

	uniform struct Transform {
		mat4 model;
		mat4 viewProjection;
		mat3 normal;
		vec3 viewPosition;
	} transform;

	uniform struct PointLight {
		vec3 position;
		vec4 ambient;
		vec4 diffuse;
		vec4 specular;
		vec3 attenuation;
	} light;

	out Vertex {
		//vec2 texcoord;
		vec3 normal;
		vec3 lightDir;
		vec3 viewDir;
		float distance;
	} Vert;

    void main() {
		float x_angle = xyz[0];
        float y_angle = xyz[1];
		float z_angle = xyz[2];
        
        mat3 rotate = mat3(
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
			0, 0, 1
		);

		vec3 vertex = position * rotate;
		vec3 lightDir = light.position - vertex;
		gl_Position = vec4(vertex, 1.0);
		//Vert.texcoord = texcoord;
		Vert.normal = normal * rotate;
		Vert.lightDir = lightDir;
		Vert.viewDir = transform.viewPosition - vertex;
		Vert.distance = length(lightDir);
    }
)";


    // Исходный код фрагментного шейдера
    const char* FragShaderSource = R"(
    #version 330 core

    in Vertex {
		//vec2 texcoord;
		vec3 normal;
		vec3 lightDir;
		vec3 viewDir;
		float distance;
	} Vert;

	uniform struct PointLight {
		vec3 position;
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

	out vec4 color;

    void main() {

       //Toon shading
       vec3 normal = normalize(Vert.normal);
	   vec3 lightDir = normalize(Vert.lightDir);
	   float diff = 0.2 + max(dot(normal, lightDir), 0.0);
       if(diff < 0.4)
        color = material.diffuse * 0.3;
       else if(diff < 0.7)
        color = material.diffuse;
       else
        color = material.diffuse * 1.3;

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
        glGenBuffers(1, &VBO);

        auto v = fm.get_vert_and_normals();
        glGenBuffers(1, &VBO);

        glEnableVertexAttribArray(Attrib_vertex);
        glEnableVertexAttribArray(attribNormals);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, v.size() * sizeof(GLfloat), v.data(), GL_STATIC_DRAW);


        glVertexAttribPointer(Attrib_vertex, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
        glVertexAttribPointer(attribNormals, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(0);

        glBindVertexArray(0);
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


        Unif_transform_viewPosition = glGetUniformLocation(Program, "transform.viewPosition");
        if (Unif_transform_viewPosition == -1)
        {
            std::cout << "could not bind uniform transform.viewPosition" << std::endl;
            return;
        }

        /*uniform struct Material {
            vec4 ambient;
            vec4 diffuse;
            vec4 specular;
            vec4 emission;
            float shininess;
        } material;*/

        Unif_material_ambient = glGetUniformLocation(Program, "material.ambient");
        if (Unif_material_ambient == -1)
        {
            std::cout << "could not bind uniform material.ambient" << std::endl;
            return;
        }

        Unif_material_diffuse = glGetUniformLocation(Program, "material.diffuse");
        if (Unif_material_diffuse == -1)
        {
            std::cout << "could not bind uniform material.diffuse" << std::endl;
            return;
        }

        Unif_material_emission = glGetUniformLocation(Program, "material.emission");
        if (Unif_material_emission == -1)
        {
            std::cout << "could not bind uniform material.emission" << std::endl;
            return;
        }

        Unif_material_specular = glGetUniformLocation(Program, "material.specular");
        if (Unif_material_specular == -1)
        {
            std::cout << "could not bind uniform material.specular" << std::endl;
            return;
        }

        Unif_material_shininess = glGetUniformLocation(Program, "material.shininess");
        if (Unif_material_specular == -1)
        {
            std::cout << "could not bind uniform material.shininess" << std::endl;
            return;
        }

        /* uniform struct PointLight {
             vec3 position;
             vec4 ambient;
             vec4 diffuse;
             vec4 specular;
             vec3 attenuation;
         } light;*/

        Unif_light_position = glGetUniformLocation(Program, "light.position");
        if (Unif_light_position == -1)
        {
            std::cout << "could not bind uniform light.position" << std::endl;
            return;
        }

        Unif_light_ambient = glGetUniformLocation(Program, "light.ambient");
        if (Unif_light_ambient == -1)
        {
            std::cout << "could not bind uniform light.ambient" << std::endl;
            return;
        }

        Unif_light_diffuse = glGetUniformLocation(Program, "light.diffuse");
        if (Unif_light_diffuse == -1)
        {
            std::cout << "could not bind uniform light.diffuse" << std::endl;
            return;
        }

        Unif_light_specular = glGetUniformLocation(Program, "light.specular");
        if (Unif_light_specular == -1)
        {
            std::cout << "could not bind uniform light.specular" << std::endl;
            return;
        }

        Unif_light_attenuation = glGetUniformLocation(Program, "light.attenuation");
        if (Unif_light_attenuation == -1)
        {
            std::cout << "could not bind uniform light.attenuation" << std::endl;
            return;
        }

        Unif_trans = glGetUniformLocation(Program, "xyz");
        if (Unif_trans == -1)
        {
            std::cout << "could not bind uniform xyz" << std::endl;
            return;
        }

        checkOpenGLerror();
    }

    void Init() {
        InitShader();
        InitVBO();
        // Включаем проверку глубины
        glEnable(GL_DEPTH_TEST);
    }



    void Draw() {
        // Устанавливаем шейдерную программу текущей
        glUseProgram(Program);


        glUniform3fv(Unif_transform_viewPosition, 1, transform.viewPosition);
        glUniform3fv(Unif_trans, 1, xyz);

        /*uniform struct Material {
           vec4 ambient;
           vec4 diffuse;
           vec4 specular;
           vec4 emission;
           float shininess;
       } material;*/
        glUniform4fv(Unif_material_ambient, 1, material.ambient);
        glUniform4fv(Unif_material_diffuse, 1, material.diffuse);
        glUniform4fv(Unif_material_specular, 1, material.specular);
        glUniform4fv(Unif_material_emission, 1, material.emission);
        glUniform1f(Unif_material_shininess, material.shininess);

        /* uniform struct PointLight {
            vec3 position;
            vec4 ambient;
            vec4 diffuse;
            vec4 specular;
            vec3 attenuation;
        } light;*/
        //light
        glUniform3fv(Unif_light_position, 1, light.position);
        glUniform4fv(Unif_light_ambient, 1, light.ambient);
        glUniform4fv(Unif_light_diffuse, 1, light.diffuse);
        glUniform4fv(Unif_light_specular, 1, light.specular);
        glUniform3fv(Unif_light_attenuation, 1, light.attenuation);

        glDrawArrays(GL_TRIANGLES, 0, fm.count() * 3);

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

