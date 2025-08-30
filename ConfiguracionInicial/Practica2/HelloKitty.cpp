/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%     Practica 2. Dibujo de primitivas 2D      %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

Alumno: Pérez Ortiz Sofia
No. de cuenta: 319074806
Fecha de entrega: 22/08/2025

Título: Hello Kitty tipo “pixel-art” 
En está práctica se dibujo una figura tipo “pixel-art” de Hello Kitty,utilizando OpenGL y las librerias GLFW y GLEW

Dependencias:
Clase Shader (Shader.h) con método Use() y carga de core.vs/core.frag

drawGrid: si es true dibuja una rejilla guía (líneas GL_LINES).

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

#include <iostream>
#include <vector>
#include <array>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Shader.h"

//Funcion que contiene las dimensiones de la ventana y que se centre el dibujo.
void resize(GLFWwindow* window, int width, int height);
const GLint WIDTH = 800, HEIGHT = 600;

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% Paleta de colores %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% 
// 0 = fondo rosa claro
// 1 = rojo 
// 2 = negro 
// 3 = blanco 
// 4 = amarillo
static const std::array<std::array<float, 3>, 5> PALETTE = { {
    {1.00f, 0.75f, 0.79f}, // 0 rosa claro
    {1.00f, 0.00f, 0.00f}, // 1 rojo
    {0.00f, 0.00f, 0.00f}, // 2 negro
    {1.00f, 1.00f, 1.00f}, // 3 blanco
    {1.00f, 1.00f, 0.00f}  // 4 amarillo
} };
//Matriz de 24x24
static const int colum = 24;
static const int reng = 24;
static const int S[reng][colum] = {
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//1
    {0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,0,0,0,0,0,0,0,0,0},//2
    {0,0,0,0,0,2,2,2,0,0,0,0,2,1,1,2,0,2,2,2,0,0,0,0},//3
    {0,0,0,0,2,3,3,3,2,2,2,2,1,1,1,1,2,3,3,3,2,0,0,0},//4
    {0,0,0,0,2,3,3,3,3,3,3,2,1,1,1,2,2,2,3,3,2,0,0,0},//5
    {0,0,0,0,2,3,3,3,3,3,3,2,1,1,2,1,1,1,2,2,2,0,0,0},//6
    {0,0,0,0,0,2,3,3,3,3,3,3,2,2,2,1,1,1,2,1,1,2,0,0},//7
    {0,0,0,0,2,3,3,3,3,3,3,3,3,3,3,2,2,2,1,1,1,2,0,0},//8
    {0,0,0,0,2,3,3,3,3,3,3,3,3,3,3,3,3,2,1,1,2,0,0,0},//9
    {0,0,0,0,2,3,3,3,3,3,3,3,3,3,3,3,3,3,2,2,0,0,0,0},//10
    {0,0,0,2,3,3,3,3,2,3,3,3,3,3,3,2,3,3,3,3,2,0,0,0},//11
    {0,2,2,2,2,3,3,3,2,3,3,3,3,3,3,2,3,3,3,2,2,2,2,0},//12
    {0,0,0,0,2,2,3,3,3,3,3,4,4,3,3,3,3,3,2,2,0,0,0,0},//13
    {0,0,2,2,2,3,3,3,3,3,3,3,3,3,3,3,3,3,3,2,2,2,0,0},//14
    {0,0,0,0,0,2,3,3,3,3,3,3,3,3,3,3,3,3,2,0,0,0,0,0},//15
    {0,0,0,0,0,0,2,2,3,3,3,3,3,3,3,3,2,2,0,0,0,0,0,0},//16
    {0,0,0,0,0,0,0,0,2,2,2,2,2,2,2,2,0,0,0,0,0,0,0,0},//17
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//18
};


static void pushQuad(std::vector<float>& V, int cx, int cy, //Genera dos triángulos para un pixel
    float cellW, float cellH,        //Tamaño de celda
	const std::array<float, 3>& c)      //Color a usar -> c
{
    // Esquina superior-izquierda
    float x0 = -1.0f + cx * cellW;
    float y0 = 1.0f - cy * cellH;
    // Esquina inferior-derecha
    float x1 = x0 + cellW;
    float y1 = y0 - cellH;

    auto add = [&](float x, float y) {
        // Posición (x,y,z) y color (r,g,b)
        V.push_back(x); V.push_back(y); V.push_back(0.0f);
        V.push_back(c[0]); V.push_back(c[1]); V.push_back(c[2]);
        };

    // Se necesitan dos triangulos para generar el cuadrado
    add(x0, y0); add(x1, y0); add(x1, y1);
    add(x0, y0); add(x1, y1); add(x0, y1);
}


int main()
{
    glfwInit(); //Iniciamos GLFWY y la creación de la ventana
    GLFWwindow* window = glfwCreateWindow(
        WIDTH, HEIGHT,
        "Dibujo Hello Kitty Perez Ortiz Sofia",     //Nombre de la ventana
        nullptr, nullptr
    );
    glfwSetFramebufferSizeCallback(window, resize);

    if (!window) {
        std::cout << "Failed to create GLFW window\n";
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cout << "Failed to initialise GLEW\n";
        return EXIT_FAILURE;
    }

    // Imprimimos información de OpenGL del sistema
    std::cout << "> Version:  " << glGetString(GL_VERSION) << '\n';
    std::cout << "> Vendor:   " << glGetString(GL_VENDOR) << '\n';
    std::cout << "> Renderer: " << glGetString(GL_RENDERER) << '\n';
    std::cout << "> SL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

    // Ajuste inicial de viewport 
    int fbw, fbh; glfwGetFramebufferSize(window, &fbw, &fbh);
    resize(window, fbw, fbh);

    Shader shader("Shader/core.vs", "Shader/core.frag"); //Carga del progrma de shaders

    //Generamos los vertices
    std::vector<float> vertices;
    vertices.reserve(reng * colum * 6 /*verts*/ * 6 /*attr*/); //Estimación máxima

	float cellW = 2.0f / colum; //ancho de celda
	float cellH = 2.0f / reng;  //alto de celda

    // Recorre la matriz y genera quads
    for (int r = 0; r < reng; ++r) {
        for (int c = 0; c < colum; ++c) {
            int code = S[r][c];
            if (code == 0) continue;              // se ignora el fondo
            pushQuad(vertices, c, r, cellW, cellH, PALETTE[code]);
        }
    }

    // Se crea el VAO/VBO 
    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER,
        vertices.size() * sizeof(float),
        vertices.data(),
        GL_STATIC_DRAW);

    // Atributo 0 -> posición
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Atributo 1 -> color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

	bool drawGrid = false;              //Creación de la rejilla, si se desea activar es con true
    GLuint gridVAO = 0, gridVBO = 0;
    std::vector<float> grid;
    
    if (drawGrid) {
        // Líneas verticales
        for (int c = 0; c <= colum; ++c) {
            float x = -1.0f + c * cellW;
            grid.insert(grid.end(), { x,  1.0f, 0.0f, 0.8f,0.8f,0.8f });
            grid.insert(grid.end(), { x, -1.0f, 0.0f, 0.8f,0.8f,0.8f });
        }
        // Líneas horizontales
        for (int r = 0; r <= reng; ++r) {
            float y = 1.0f - r * cellH;
            grid.insert(grid.end(), { -1.0f, y, 0.0f, 0.8f,0.8f,0.8f });
            grid.insert(grid.end(), { 1.0f, y, 0.0f, 0.8f,0.8f,0.8f });
        }
        
        glGenVertexArrays(1, &gridVAO);
        glGenBuffers(1, &gridVBO);
        glBindVertexArray(gridVAO);
        glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
        glBufferData(GL_ARRAY_BUFFER, grid.size() * sizeof(float), grid.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glBindVertexArray(0);
    } 

    // Bucle principal
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // Limpieza de pantalla con color de fondo
        glClearColor(PALETTE[0][0], PALETTE[0][1], PALETTE[0][2], 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader.Use(); // activa el shader

        // Dibuja rejilla si está habilitada
        if (drawGrid) {
            glBindVertexArray(gridVAO);
            glLineWidth(1.0f);
            glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(grid.size() / 6));
        }
        // Dibuja la figura completa
        glBindVertexArray(VAO);
        GLsizei count = static_cast<GLsizei>(vertices.size() / 6); 
        glDrawArrays(GL_TRIANGLES, 0, count);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
    }
    glfwTerminate(); //Limpia
    return EXIT_SUCCESS;
}

/* Resize asegura un viewport cuadrado centrado y mantiene la proporción de los pixeles*/
void resize(GLFWwindow* window, int width, int height)
{
    int size = (width < height) ? width : height; 
    int x = (width - size) / 2;
    int y = (height - size) / 2;
    glViewport(x, y, size, size);
}
