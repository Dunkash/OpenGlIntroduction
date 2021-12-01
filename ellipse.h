#pragma once
#include "figure.h"
#define PI 3.14159265358979323846

class ellipse :
    public figure
{
    // Переменные с индентификаторами ID
// ID шейдерной программы
    GLuint Program;
    // ID атрибута
    GLint Attrib_vertex;

    GLuint VBO_color;



    GLint Attrib_color;

    GLint Unif_x;
    GLint Unif_y;

    // ID Vertex Buffer Object
    GLuint VBO;
    // Вершина
    struct Vertex
    {
        GLfloat x;
        GLfloat y;
    };

    float radX = 0.5f;
    float radY = 0.5f;

    void incRadX()
    {
        radX += 0.05;
        if (radX > 1)
            radX = 1;
    }

    void incRadY()
    {
        radY += 0.05;
        if (radY > 1)
            radY = 1;
    }

    void decRadX()
    {
        radX -= 0.05;
        if (radX < 0)
            radX = 0;
    }

    void decRadY()
    {
        radY -= 0.05;
        if (radY < 0)
            radY = 0;
    }

    // Исходный код вершинного шейдера
    const char* VertexShaderSource = R"(
    #version 330 core
    in vec2 coord;

    in vec4 color;
    out vec4 vert_color;
    

    uniform float radX;
    uniform float radY;
    void main() {
        gl_Position = vec4(coord[0]*radX,coord[1]*radY, 0.0, 1.0);
        vert_color = color;
    }
)";

    // Исходный код фрагментного шейдера
    const char* FragShaderSource = R"(
    #version 330 core
    in vec4 vert_color;

    out vec4 color;
    void main() {
        color = vert_color;
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

    float* HSVtoRGB(float h, float s, float v) {
        float r, g, b = 0;

        h = h / 360;

        int i = floor(h * 6);
        auto f = h * 6 - i;
        auto p = v * (1 - s);
        auto q = v * (1 - f * s);
        auto t = v * (1 - (1 - f) * s);

        switch (i%6)
        {
        case 0: r = v; g = t; b = p; break;
        case 1: r = q; g = v; b = p; break;
        case 2: r = p; g = v; b = t; break;
        case 3: r = p; g = q; b = v; break;
        case 4: r = t; g = p; b = v; break;
        case 5: r = v; g = p; b = q; break;
        default: break;
        }
        float res[4] = { r * 255,g * 255,b * 255, 1 };
        return res;
    }

    float* HSVtoRGB2(float fH, float fS, float fV) {
        float fR, fG, fB;
        float fC = fV * fS; // Chroma
        float fHPrime = fmod(fH / 60.0, 6);
        float fX = fC * (1 - fabs(fmod(fHPrime, 2) - 1));
        float fM = fV - fC;

        if (0 <= fHPrime && fHPrime < 1) {
            fR = fC;
            fG = fX;
            fB = 0;
        }
        else if (1 <= fHPrime && fHPrime < 2) {
            fR = fX;
            fG = fC;
            fB = 0;
        }
        else if (2 <= fHPrime && fHPrime < 3) {
            fR = 0;
            fG = fC;
            fB = fX;
        }
        else if (3 <= fHPrime && fHPrime < 4) {
            fR = 0;
            fG = fX;
            fB = fC;
        }
        else if (4 <= fHPrime && fHPrime < 5) {
            fR = fX;
            fG = 0;
            fB = fC;
        }
        else if (5 <= fHPrime && fHPrime < 6) {
            fR = fC;
            fG = 0;
            fB = fX;
        }
        else {
            fR = 0;
            fG = 0;
            fB = 0;
        }

        fR += fM;
        fG += fM;
        fB += fM;

        float res[4] = { fR,fG,fB,1 };
        return res;
    }


    void InitVBO()
    {
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &VBO_color);
        // Вершины нашего треугольника
        Vertex ellipse[362];
        ellipse[0] = { 0,0 };
        for (int i = 1; i < 362; ++i)
        {
            ellipse[i] = { (float)cos(i * PI / 180),(float)sin(i * PI / 180) };
        }

        float colors[362][4];
        {
            colors[0][0] = 1;
            colors[0][1] = 1;
            colors[0][2] = 1;
            colors[0][3] = 1;
        }

        for (int i = 0; i < 360; ++i)
        {
            auto j = HSVtoRGB2(i, 1, 1);
            colors[i+1][0] = j[0];
            colors[i+1][1] = j[1];
            colors[i+1][2] = j[2];
            colors[i+1][3] = 1;
        }
        
        {
            auto j = HSVtoRGB(0, 1, 1);
            colors[361][0] = j[0];
            colors[361][1] = j[1];
            colors[361][2] = j[2];
            colors[361][3] = 1;
        }
        

        // Передаем вершины в буфер
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(ellipse), ellipse, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_color);
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
        const char* attr_name = "coord";
        Attrib_vertex = glGetAttribLocation(Program, attr_name);
        if (Attrib_vertex == -1)
        {
            std::cout << "could not bind attrib " << attr_name << std::endl;
            return;
        }

        // Вытягиваем ID юниформ
        const char* unif_name = "radX";
        Unif_x = glGetUniformLocation(Program, unif_name);
        if (Unif_x == -1)
        {
            std::cout << "could not bind uniform " << unif_name << std::endl;
            return;
        }

        // Вытягиваем ID юниформ
        unif_name = "radY";
        Unif_y = glGetUniformLocation(Program, unif_name);
        if (Unif_y == -1)
        {
            std::cout << "could not bind uniform " << unif_name << std::endl;
            return;
        }

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
    }


    void Draw() {
        // Устанавливаем шейдерную программу текущей
        glUseProgram(Program);

        glUniform1f(Unif_x, radX);
        glUniform1f(Unif_y, radY);

        // Включаем массив атрибутов
        glEnableVertexAttribArray(Attrib_vertex);
        glEnableVertexAttribArray(Attrib_color);
        // Подключаем VBO
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        // Указывая pointer 0 при подключенном буфере, мы указываем что данные в VBO
        glVertexAttribPointer(Attrib_vertex, 2, GL_FLOAT, GL_FALSE, 0, 0);

        // Подключаем VBO_color
        glBindBuffer(GL_ARRAY_BUFFER, VBO_color);
        glVertexAttribPointer(Attrib_color, 4, GL_FLOAT, GL_FALSE, 0, 0);

        // Отключаем VBO
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // Передаем данные на видеокарту(рисуем)
        glDrawArrays(GL_TRIANGLE_FAN, 0, 362);
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
                // обработка нажатий клавиш
                else if (event.type == sf::Event::KeyPressed) {
                    switch (event.key.code) {
                    case (sf::Keyboard::Left): decRadX(); break;
                    case (sf::Keyboard::Right): incRadX(); break;
                    case (sf::Keyboard::Up): incRadY(); break;
                    case (sf::Keyboard::Down): decRadY(); break;
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

