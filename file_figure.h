#pragma once
#pragma comment( lib, "OpenGL32.lib" )
#include "figure.h"
#include "figure_model.h"

class file_figure :
    public figure
{

    // ���������� � ����������������� ID
    // ID ��������� ���������
    GLuint Program;
    // ID ��������
    GLint Attrib_vertex;
    // ID Vertex Buffer Object
    GLuint VBO;

    GLuint VAO;
    // �������
    struct Vertex
    {
        GLfloat x;
        GLfloat y;
        GLfloat z;
    };

    figure_model fm = figure_model("model.obj");



    // �������� ��� ���������� �������
    const char* VertexShaderSource = R"(
    #version 330 core

    in vec3 vertexPosition;
    
    out vec3 vPosition;

    void main() {
        vPosition = vertexPosition;

        float x_angle = 1;
        float y_angle = 1;
        
        vec3 position = vertexPosition * mat3(
            1, 0, 0,
            0, cos(x_angle), -sin(x_angle),
            0, sin(x_angle), cos(x_angle)
        ) 
        * 
        mat3(
            cos(y_angle), 0, sin(y_angle),
            0, 1, 0,
            -sin(y_angle), 0, cos(y_angle)
        );

        gl_Position = vec4(position, 1.0);
    }
)";

    
    // �������� ��� ������������ �������
    const char* FragShaderSource = R"(
    #version 330 core

    in vec3 vPosition;

    out vec4 color;

    void main() {
        float k = 5;
        int sum = int(vPosition.x * k) + int(vPosition.y * k) + int(vPosition.z * k);
        if (sum - (sum / 2) * 2 != 0)
            color = vec4(0.2, 0, 0.2, 1);
        else
            color = vec4(0.1, 0.5, 0.7, 1);
    }
)";


    // �������� ������ OpenGL, ���� ���� �� ����� � ������� ��� ������
    void checkOpenGLerror() {
        GLenum errCode;
        // ���� ������ ����� �������� ���
        // https://www.khronos.org/opengl/wiki/OpenGL_Error
        if ((errCode = glGetError()) != GL_NO_ERROR)
            std::cout << "OpenGl error!: " << errCode << std::endl;
    }

    // ������� ������ ���� �������
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
        glGenVertexArrays(1, &VAO);
        // ������� ������
        auto figure_data = fm.get_vertices_struct();
        
      

        glBindVertexArray(VAO); //����������� VAO
// �������� ������ ������ � ����� ��� OpenGL
        glEnableVertexAttribArray(Attrib_vertex);
        // �������� ������� � �����
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, std::get<1>(figure_data), std::get<0>(figure_data), GL_STATIC_DRAW);
        //������������� ��������� �� ��������� ��������
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(0);
        //���������� VAO
        glBindVertexArray(0);
        checkOpenGLerror();
    }


    void InitShader() {
        // ������� ��������� ������
        GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
        // �������� �������� ���
        glShaderSource(vShader, 1, &VertexShaderSource, NULL);
        // ����������� ������
        glCompileShader(vShader);
        std::cout << "vertex shader \n";
        ShaderLog(vShader);

        // ������� ����������� ������
        GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
        // �������� �������� ���
        glShaderSource(fShader, 1, &FragShaderSource, NULL);
        // ����������� ������
        glCompileShader(fShader);
        std::cout << "fragment shader \n";
        ShaderLog(fShader);

        // ������� ��������� � ����������� ������� � ���
        Program = glCreateProgram();
        glAttachShader(Program, vShader);
        glAttachShader(Program, fShader);

        // ������� ��������� ���������
        glLinkProgram(Program);
        // ��������� ������ ������
        int link_ok;
        glGetProgramiv(Program, GL_LINK_STATUS, &link_ok);
        if (!link_ok)
        {
            std::cout << "error attach shaders \n";
            return;
        }

        /*
        // ���������� ID �������� �� ��������� ���������
        const char* attr_name = "vertexPosition";
        Attrib_vertex = glGetAttribLocation(Program, attr_name);
        if (Attrib_vertex == -1)
        {
            std::cout << "could not bind attrib " << attr_name << std::endl;
            return;
        }
        */

        checkOpenGLerror();
    }

    void Init() {
        InitShader();
        InitVBO();
        // �������� �������� �������
        glEnable(GL_DEPTH_TEST);
    }


    void Draw() {
        glUseProgram(Program);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, fm.count());
        glBindVertexArray(0);

    }


    // ������������ ��������
    void ReleaseShader() {
        // ��������� ����, �� ��������� �������� ���������
        glUseProgram(0);
        // ������� ��������� ���������
        glDeleteProgram(Program);
    }

    // ������������ ������
    void ReleaseVBO()
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDeleteBuffers(1, &VBO);
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

            // ������������� glew
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
                }

                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                Draw();

                window.display();
            }

            Release();
            return 0;
        }
};

