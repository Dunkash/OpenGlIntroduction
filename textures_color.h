#pragma once
#include "figure.h"
#define TO_STRING(x) #x
class textures_color :
    public figure
{

    // Переменные с индентификаторами ID
    // ID шейдерной программы
    GLuint Program;
    // ID атрибута
    GLint Attrib_vertex;
    // ID Vertex Buffer Object
    GLuint VBO;
    // ID буфера текстурных координат
    GLuint textureVBO;
    GLuint VBO_position;
    // ID VBO цвета
    GLuint VBO_color;
    // Вершина
    struct Vertex
    {
        GLfloat x;
        GLfloat y;
        GLfloat z;
    };

    // ID текстуры
    GLint textureHandle;
    // SFML текстура
    sf::Texture textureData;
    // ID атрибута текстурных координат
    GLint attribTexture;
    // ID юниформа текстуры
    GLint unifTexture;

    GLuint colorsVBO;

    // ID атрибута цвета
    GLint Attrib_color;

    // Исходный код вершинного шейдера
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

        // Исходный код фрагментного шейдера
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
        glGenBuffers(1, &colorsVBO);
        glGenBuffers(1, &textureVBO);
        glGenBuffers(1, &VBO);
        // Вершины кубика
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
        // Передаем вершины в буфер
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, textureVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(texture), texture, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, colorsVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
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

        // Вытягиваем ID атрибута цвета
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
        // Включаем проверку глубины
        glEnable(GL_DEPTH_TEST);
    }

    void InitTexture()
    {
        const char* filename = "image.jpg";
        // Загружаем текстуру из файла
        if (!textureData.loadFromFile(filename))
        {
            // Не вышло загрузить картинку
            return;
        }
        // Теперь получаем openGL дескриптор текстуры
        textureHandle = textureData.getNativeHandle();
    }

    void Draw() {
        // Устанавливаем шейдерную программу текущей
        glUseProgram(Program);

        // Активируем текстурный блок 0, делать этого не обязательно, по умолчанию
        // и так активирован GL_TEXTURE0, это нужно для использования нескольких текстур
        glActiveTexture(GL_TEXTURE0);
        // Обёртка SFML на opengl функцией glBindTexture
        sf::Texture::bind(&textureData);
        // В uniform кладётся текстурный индекс текстурного блока (для GL_TEXTURE0 - 0, для GL_TEXTURE1 - 1 и тд)
        glUniform1i(unifTexture, 0);

        // Включаем массив атрибутов
        glEnableVertexAttribArray(Attrib_vertex);
        glEnableVertexAttribArray(Attrib_color);
        glEnableVertexAttribArray(attribTexture);
        // Подключаем VBO

        glBindBuffer(GL_ARRAY_BUFFER, colorsVBO);
        glVertexAttribPointer(Attrib_color, 4, GL_FLOAT, GL_FALSE, 0, 0);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glVertexAttribPointer(Attrib_vertex, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glBindBuffer(GL_ARRAY_BUFFER, textureVBO);
        glVertexAttribPointer(attribTexture, 3, GL_FLOAT, GL_FALSE, 0, 0);

        // Отключаем VBO
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // Передаем данные на видеокарту(рисуем)
        glDrawArrays(GL_QUADS, 0, 24);
        // Отключаем массив атрибутов
        glDisableVertexAttribArray(Attrib_vertex);
        glDisableVertexAttribArray(Attrib_color);
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
            }

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            Draw();

            window.display();
        }

        Release();
        return 0;
    }
};

