#pragma once
#include "figure.h"
class square :
    public figure
{
    // Переменные с индентификаторами ID
// ID шейдерной программы
    GLuint Program;
    // ID атрибута
    GLint Attrib_vertex;
    // ID Vertex Buffer Object
    GLuint VBO;
    // Вершина
    struct Vertex
    {
        GLfloat x;
        GLfloat y;
    };

    // Исходный код вершинного шейдера
    const char* VertexShaderSource = R"(
    #version 330 core
    in vec2 coord;
    out vec2 vPosition;
    void main() {
        vPosition = coord;
        gl_Position = vec4(coord, 0.0, 1.0);
    }
)";

    // Исходный код фрагментного шейдера
    const char* FragShaderSource = R"(
    #version 330 core
    in vec2 vPosition;
    out vec4 color;
    void main() {
        float k = 20;
        int sum = int((vPosition.x -1)* k);
        if(sum % 2 == 0)
            color = vec4(1, 1, 0, 1);
        else
            color = vec4(0, 0, 1, 1);
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
        // Вершины нашего пятиугольника
        Vertex pentagon[4] = {
            { 0.5, 0.5 },
            { -0.5, 0.5 },
            { -0.5, -0.5 },
            { 0.5, -0.5},  
        };

        // Передаем вершины в буфер
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(pentagon), pentagon, GL_STATIC_DRAW);
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
        const char* attr_name = "coord";
        Attrib_vertex = glGetAttribLocation(Program, attr_name);
        if (Attrib_vertex == -1)
        {
            std::cout << "could not bind attrib " << attr_name << std::endl;
            return;
        }

        checkOpenGLerror();
    }

    void Init() {
        InitShader();
        InitVBO();
    }


    void Draw() {
        // Устанавливаем шейдерную программу текущей
        glUseProgram(Program);
        // Включаем массив атрибутов
        glEnableVertexAttribArray(Attrib_vertex);
        // Подключаем VBO
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        // Указывая pointer 0 при подключенном буфере, мы указываем что данные в VBO
        glVertexAttribPointer(Attrib_vertex, 2, GL_FLOAT, GL_FALSE, 0, 0);
        // Отключаем VBO
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // Передаем данные на видеокарту(рисуем)
        glDrawArrays(GL_POLYGON, 0, 4);
        // Отключаем массив атрибутов
        glDisableVertexAttribArray(Attrib_vertex);
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
    }

    void Release() {
        ReleaseShader();
        ReleaseVBO();
    }

public:
    int Paint() override {
        sf::Window window(sf::VideoMode(600, 600), "Red pentagon", sf::Style::Default, sf::ContextSettings(24));
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

