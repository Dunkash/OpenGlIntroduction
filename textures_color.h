#pragma once
#include "figure.h"
#define TO_STRING(x) #x
class textures_color :
    public figure
{

    // ���������� � ����������������� ID
    // ID ��������� ���������
    GLuint Program;
    // ID ��������
    GLint Attrib_vertex;
    // ID Vertex Buffer Object
    GLuint VBO;
    // ID ������ ���������� ���������
    GLuint textureVBO;
    GLuint VBO_position;
    // ID VBO �����
    GLuint VBO_color;
    // �������
    struct Vertex
    {
        GLfloat x;
        GLfloat y;
        GLfloat z;
    };

    // ID ��������
    GLint textureHandle;
    // SFML ��������
    sf::Texture textureData;
    // ID �������� ���������� ���������
    GLint attribTexture;
    // ID �������� ��������
    GLint unifTexture;

    GLuint colorsVBO;

    // ID �������� �����
    GLint Attrib_color;

    // �������� ��� ���������� �������
    const char* VertexShaderSource = R"(
    #version 330 core

    in vec3 vertexPosition;
    in vec3 textureCoord;
    in vec4 color;


    out vec4 vert_color;    
    out vec3 tCoord;

    void main() {
        tCoord = textureCoord;

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
        vert_color = color;
        gl_Position = vec4(position, 1.0);
    }
)";

        // �������� ��� ������������ �������
    const char* FragShaderSource = R"(
    #version 330 core
    
    uniform sampler2D textureData;

    in vec4 vert_color;
    in vec3 tCoord;

    out vec4 color;



    void main() 
    {
        vec4 tex = texture(textureData, tCoord.xy);
        color = vec4(vert_color.r*tex.r,vert_color.g*tex.g,vert_color.b*tex.b,vert_color.a*tex.a);
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
        glGenBuffers(1, &colorsVBO);
        glGenBuffers(1, &textureVBO);
        glGenBuffers(1, &VBO);
        // ������� ������
        Vertex triangle[] = {
            {-0.5,-0.5,0.5},{-0.5,0.5,0.5},{0.5,0.5,0.5},{0.5,-0.5,0.5},
            {-0.5,-0.5,0.5},{-0.5,0.5,0.5},{-0.5,0.5,-0.5},{-0.5,-0.5,-0.5},
            {-0.5,-0.5,-0.5},{-0.5,0.5,-0.5},{0.5,0.5,-0.5},{0.5,-0.5,-0.5},
            {0.5,-0.5,0.5},{0.5,0.5,0.5},{0.5,0.5,-0.5},{0.5,-0.5,-0.5},
            {-0.5,0.5,0.5},{0.5,0.5,0.5},{0.5,0.5,-0.5},{-0.5,0.5,-0.5},
            {-0.5,-0.5,0.5},{0.5,-0.5,0.5},{0.5,-0.5,-0.5},{-0.5,-0.5,-0.5}
        };

        Vertex texture[] = {
            {0,0,0},{0,1,0},{1,1,0},{1,0,0},
            {0,0,0},{0,1,0},{1,1,0},{1,0,0},
            {0,0,0},{0,1,0},{1,1,0},{1,0,0},
            {0,0,0},{0,1,0},{1,1,0},{1,0,0},
            {0,0,0},{0,1,0},{1,1,0},{1,0,0},
            {0,0,0},{0,1,0},{1,1,0},{1,0,0},
        };

        
        float colors[24][4] =
        {
            {1.0,0.0,0.0,1.0},{0.0,1.0,0.0,1.0},{0.0,0.0,1.0,1.0},{0.0,0.0,0.0,1.0},
            {1.0,0.0,0.0,1.0},{0.0,1.0,0.0,1.0},{1.0,1.0,0.0,1.0},{1.0,0.0,1.0,1.0},
            {1.0,0.0,1.0,1.0},{1.0,1.0,0.0,1.0},{0.0,1.0,1.0,1.0},{1.0,1.0,1.0,1.0},
            {0.0,0.0,0.0,1.0},{0.0,0.0,1.0,1.0},{0.0,1.0,1.0,1.0},{1.0,1.0,1.0,1.0},
            {0.0,1.0,0.0,1.0},{0.0,0.0,1.0,1.0},{0.0,1.0,1.0,1.0},{1.0,1.0,0.0,1.0},
            {1.0,0.0,0.0,1.0},{0.0,0.0,0.0,1.0},{1.0,1.0,1.0,1.0},{1.0,0.0,1.0,1.0}
        };
        /*
        float colors[24][4] =
        {
            {1.0,1.0,1.0,1.0},{1.0,1.0,1.0,1.0},{1.0,1.0,1.0,1.0},{1.0,1.0,1.0,1.0},
            {1.0,1.0,1.0,1.0},{1.0,1.0,1.0,1.0},{1.0,1.0,1.0,1.0},{1.0,1.0,1.0,1.0},
            {1.0,1.0,1.0,1.0},{1.0,1.0,1.0,1.0},{1.0,1.0,1.0,1.0},{1.0,1.0,1.0,1.0},
            {1.0,1.0,1.0,1.0},{1.0,1.0,1.0,1.0},{1.0,1.0,1.0,1.0},{1.0,1.0,1.0,1.0},
            {1.0,1.0,1.0,1.0},{1.0,1.0,1.0,1.0},{1.0,1.0,1.0,1.0},{1.0,1.0,1.0,1.0},
            {1.0,1.0,1.0,1.0},{1.0,1.0,1.0,1.0},{1.0,1.0,1.0,1.0},{1.0,1.0,1.0,1.0},
        };
        */
        // �������� ������� � �����
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, textureVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(texture), texture, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, colorsVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
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

        
        // ���������� ID �������� �� ��������� ���������
        const char* attr_name = "vertexPosition";
        Attrib_vertex = glGetAttribLocation(Program, attr_name);
        if (Attrib_vertex == -1)
        {
            std::cout << "could not bind attrib " << attr_name << std::endl;
            return;
        }


        attribTexture = glGetAttribLocation(Program, "textureCoord");
        if (attribTexture == -1)
        {
            std::cout << "could not bind attrib textureCoord" << std::endl;
            return;
        }

        unifTexture = glGetUniformLocation(Program, "textureData");
        if (unifTexture == -1)
        {
            std::cout << "could not bind uniform textureData" << std::endl;
            return;
        }

        // ���������� ID �������� �����
        Attrib_color = glGetAttribLocation(Program, "color");
        if (Attrib_color == -1)
        {
            std::cout << "could not bind attrib color" << std::endl;
            return;
        }

        checkOpenGLerror();
    }

    void Init() {
        InitShader();
        InitVBO();
        InitTexture();
        // �������� �������� �������
        glEnable(GL_DEPTH_TEST);
    }

    void InitTexture()
    {
        const char* filename = "image.jpg";
        // ��������� �������� �� �����
        if (!textureData.loadFromFile(filename))
        {
            // �� ����� ��������� ��������
            return;
        }
        // ������ �������� openGL ���������� ��������
        textureHandle = textureData.getNativeHandle();
    }

    void Draw() {
        // ������������� ��������� ��������� �������
        glUseProgram(Program);

        // ���������� ���������� ���� 0, ������ ����� �� �����������, �� ���������
        // � ��� ����������� GL_TEXTURE0, ��� ����� ��� ������������� ���������� �������
        glActiveTexture(GL_TEXTURE0);
        // ������ SFML �� opengl �������� glBindTexture
        sf::Texture::bind(&textureData);
        // � uniform ������� ���������� ������ ����������� ����� (��� GL_TEXTURE0 - 0, ��� GL_TEXTURE1 - 1 � ��)
        glUniform1i(unifTexture, 0);

        // �������� ������ ���������
        glEnableVertexAttribArray(Attrib_vertex);
        glEnableVertexAttribArray(Attrib_color);
        glEnableVertexAttribArray(attribTexture);
        // ���������� VBO

        glBindBuffer(GL_ARRAY_BUFFER, colorsVBO);
        glVertexAttribPointer(Attrib_color, 4, GL_FLOAT, GL_FALSE, 0, 0);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glVertexAttribPointer(Attrib_vertex, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glBindBuffer(GL_ARRAY_BUFFER, textureVBO);
        glVertexAttribPointer(attribTexture, 3, GL_FLOAT, GL_FALSE, 0, 0);

        // ��������� VBO
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // �������� ������ �� ����������(������)
        glDrawArrays(GL_QUADS, 0, 24);
        // ��������� ������ ���������
        glDisableVertexAttribArray(Attrib_vertex);
        glDisableVertexAttribArray(Attrib_color);
        // ��������� ��������� ���������
        glUseProgram(0);
        checkOpenGLerror();
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
        glDeleteBuffers(1, &VBO_color);
    }

    void Release() {
        ReleaseShader();
        ReleaseVBO();
    }

public:
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

