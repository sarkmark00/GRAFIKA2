﻿#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include <array>
#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <math.h>

using namespace std;

// Konstansok és globális változók
#define SZAM_VAO 2          // Vertex Array Object-ek száma
#define SZAM_VBO 2          // Vertex Buffer Object-ek száma
constexpr float ÉRZÉKENYSÉG = 0.03f; // Egérkattintás érzékenysége

GLchar ablakCím[] = "Beadando 02."; // Ablak címe
GLuint ablakSzélesség = 600;        // Ablak szélessége
GLuint ablakMagasság = 600;         // Ablak magassága
GLFWwindow* ablak = nullptr;        // GLFW ablak pointere
GLfloat pontMéret = 7.0f;           // Kontrollpontok mérete

GLint mozgatottPont = -1;           // Az éppen mozgatott kontrollpont indexe

std::vector<glm::vec3> görbePontok; // A Bezier-görbe pontjainak tárolója
std::vector<glm::vec3> kontrollPontok; // Kontrollpontok tárolója

GLuint VAO[SZAM_VAO];               // Vertex Array Object tömb
GLuint VBO[SZAM_VBO];               // Vertex Buffer Object tömb

GLuint renderProgram;               // Shader program azonosítója

// OpenGL hibák ellenőrzése
bool checkOpenGLError() {
    bool hibaTalálva = false;
    int glHiba = glGetError();
    while (glHiba != GL_NO_ERROR) {
        cout << "OpenGL hiba: " << glHiba << endl;
        hibaTalálva = true;
        glHiba = glGetError();
    }
    return hibaTalálva;
}

// Shader napló kiírása
void printShaderLog(GLuint shader) {
    int hossz = 0;
    int írtKarakterek = 0;
    char* napló;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &hossz);
    if (hossz > 0) {
        napló = (char*)malloc(hossz);
        glGetShaderInfoLog(shader, hossz, &írtKarakterek, napló);
        cout << "Shader napló: " << napló << endl;
        free(napló);
    }
}

// Program napló kiírása
void printProgramLog(int program) {
    int hossz = 0;
    int írtKarakterek = 0;
    char* napló;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &hossz);
    if (hossz > 0) {
        napló = (char*)malloc(hossz);
        glGetProgramInfoLog(program, hossz, &írtKarakterek, napló);
        cout << "Program napló: " << napló << endl;
        free(napló);
    }
}

// Shader forráskód beolvasása fájlból
string readShaderSource(const char* fájlÚtvonal) {
    string tartalom;
    ifstream fájl(fájlÚtvonal, ios::in);
    string sor = "";
    while (!fájl.eof()) {
        getline(fájl, sor);
        tartalom.append(sor + "\n");
    }
    fájl.close();
    return tartalom;
}

// Shader program létrehozása
GLuint createShaderProgram() {
    GLint csúcsFordítva;
    GLint fragmentumFordítva;
    GLint összekapcsolva;

    // Shader forráskódok beolvasása
    string csúcsShaderStr = readShaderSource("vertexShader.glsl");
    string fragmentumShaderStr = readShaderSource("fragmentShader.glsl");

    // Shader objektumok létrehozása
    GLuint csúcsShaderObj = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentumShaderObj = glCreateShader(GL_FRAGMENT_SHADER);

    const char* csúcsShaderForrás = csúcsShaderStr.c_str();
    const char* fragmentumShaderForrás = fragmentumShaderStr.c_str();

    // Shader forráskódok betöltése
    glShaderSource(csúcsShaderObj, 1, &csúcsShaderForrás, NULL);
    glShaderSource(fragmentumShaderObj, 1, &fragmentumShaderForrás, NULL);

    // Csúcsshader fordítása
    glCompileShader(csúcsShaderObj);
    checkOpenGLError();
    glGetShaderiv(csúcsShaderObj, GL_COMPILE_STATUS, &csúcsFordítva);
    if (csúcsFordítva != 1) {
        cout << "Csúcsshader fordítási hiba" << endl;
        printShaderLog(csúcsShaderObj);
    }

    // Fragmentumshader fordítása
    glCompileShader(fragmentumShaderObj);
    checkOpenGLError();
    glGetShaderiv(fragmentumShaderObj, GL_COMPILE_STATUS, &fragmentumFordítva);
    if (fragmentumFordítva != 1) {
        cout << "Fragmentumshader fordítási hiba" << endl;
        printShaderLog(fragmentumShaderObj);
    }

    // Shader program létrehozása és összekapcsolása
    GLuint program = glCreateProgram();
    glAttachShader(program, csúcsShaderObj);
    glAttachShader(program, fragmentumShaderObj);
    glLinkProgram(program);

    checkOpenGLError();
    glGetProgramiv(program, GL_LINK_STATUS, &összekapcsolva);
    if (összekapcsolva != 1) {
        cout << "Program összekapcsolási hiba" << endl;
        printProgramLog(program);
    }

    // Shader objektumok törlése
    glDeleteShader(csúcsShaderObj);
    glDeleteShader(fragmentumShaderObj);

    return program;
}

// Két pont távolságának kiszámítása
GLfloat dist2(glm::vec3 pont1, glm::vec3 pont2) {
    GLfloat xKülönbség = pont1.x - pont2.x;
    GLfloat yKülönbség = pont1.y - pont2.y;
    return xKülönbség * xKülönbség + yKülönbség * yKülönbség;
}

// Aktív kontrollpont kiválasztása egérkattintás alapján
GLint getActivePoint(const std::vector<glm::vec3>& pontok, int méret, float érzékenység, float x, float y) {
    float xNormalizált = x / (ablakSzélesség / 2) - 1.0f;
    float yNormalizált = y / (ablakMagasság / 2) - 1.0f;
    glm::vec3 kattintásPozíció(xNormalizált, yNormalizált, 0.0f);
    float érzékenységNégyzet = érzékenység * érzékenység;

    for (int i = 0; i < méret; i++) {
        if (dist2(pontok[i], kattintásPozíció) < érzékenységNégyzet) {
            return i;
        }
    }
    return -1;
}

// Binomiális együttható kiszámítása
int binomiálisEgyüttható(int n, int r) {
    if (r > n - r) r = n - r; // Szimmetria kihasználása
    int eredmény = 1;
    for (int i = 0; i < r; ++i) {
        eredmény *= (n - i);
        eredmény /= (i + 1);
    }
    return eredmény;
}

// Bezier-görbe blending függvénye
double blending(int i, float t, int n) {
    return binomiálisEgyüttható(n, i) * pow(1 - t, n - i) * pow(t, i);
}

// Bezier-görbe pontjainak kiszámítása
void bezier(const std::vector<glm::vec3>& kontrollPontok) {
    görbePontok.clear();
    glm::vec3 következőPont;
    GLfloat t = 0.0f;
    GLfloat lépésköz = 1.0f / 100.0f;
    int n = kontrollPontok.size() - 1;

    while (t <= 1.0f) {
        következőPont = glm::vec3(0.0f);
        for (int i = 0; i <= n; ++i) {
            következőPont += (float)blending(i, t, n) * kontrollPontok[i];
        }
        görbePontok.push_back(következőPont);
        t += lépésköz;
    }
}

// Kurzor mozgásának kezelése (kontrollpont mozgatása)
void cursorPosCallback(GLFWwindow* ablak, double xPozíció, double yPozíció) {
    GLfloat xNormalizált = xPozíció / (ablakSzélesség / 2) - 1.0f;
    GLfloat yNormalizált = (ablakMagasság - yPozíció) / (ablakMagasság / 2) - 1.0f;

    if (mozgatottPont >= 0) {
        kontrollPontok.at(mozgatottPont).x = xNormalizált;
        kontrollPontok.at(mozgatottPont).y = yNormalizált;

        if (kontrollPontok.size() >= 3) {
            görbePontok.clear();
            bezier(kontrollPontok);
        }

        // Görbe pontjainak frissítése
        glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
        glBufferData(GL_ARRAY_BUFFER, görbePontok.size() * sizeof(glm::vec3), görbePontok.data(), GL_STATIC_DRAW);

        // Kontrollpontok frissítése
        glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
        glBufferData(GL_ARRAY_BUFFER, kontrollPontok.size() * sizeof(glm::vec3), kontrollPontok.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}

// Memória felszabadítása és program leállítása
void cleanUpScene() {
    glDeleteVertexArrays(SZAM_VAO, VAO);
    glDeleteBuffers(SZAM_VBO, VBO);
    glDeleteProgram(renderProgram);
    glfwDestroyWindow(ablak);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}

// Egérkattintás kezelése
void mouseButtonCallback(GLFWwindow* ablak, int gomb, int művelet, int módosítók) {
    double x, y;
    glfwGetCursorPos(ablak, &x, &y);

    // Bal egérgomb lenyomása: új pont vagy meglévő pont mozgatása
    if (gomb == GLFW_MOUSE_BUTTON_LEFT && művelet == GLFW_PRESS) {
        int i = getActivePoint(kontrollPontok, kontrollPontok.size(), ÉRZÉKENYSÉG, x, ablakMagasság - y);
        if (i != -1) {
            mozgatottPont = i;
        }
        else {
            kontrollPontok.push_back(glm::vec3(x / (ablakSzélesség / 2) - 1.0f,
                (ablakMagasság - y) / (ablakMagasság / 2) - 1.0f,
                0.0f));
            if (kontrollPontok.size() >= 3) {
                bezier(kontrollPontok);
            }
            else {
                görbePontok.clear();
            }

            // VBO-k frissítése
            glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
            glBufferData(GL_ARRAY_BUFFER, görbePontok.size() * sizeof(glm::vec3), görbePontok.data(), GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
            glBufferData(GL_ARRAY_BUFFER, kontrollPontok.size() * sizeof(glm::vec3), kontrollPontok.data(), GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
    }

    // Jobb egérgomb: pont törlése
    if (gomb == GLFW_MOUSE_BUTTON_RIGHT && művelet == GLFW_PRESS) {
        int idx = getActivePoint(kontrollPontok, kontrollPontok.size(), ÉRZÉKENYSÉG, x, ablakMagasság - y);
        if (idx != -1) {
            kontrollPontok.erase(kontrollPontok.begin() + idx);
        }
        if (kontrollPontok.size() >= 3) {
            bezier(kontrollPontok);
        }
        else {
            görbePontok.clear();
        }

        // VBO-k frissítése
        glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
        glBufferData(GL_ARRAY_BUFFER, görbePontok.size() * sizeof(glm::vec3), görbePontok.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
        glBufferData(GL_ARRAY_BUFFER, kontrollPontok.size() * sizeof(glm::vec3), kontrollPontok.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    // Bal egérgomb felengedése: mozgatás vége
    if (gomb == GLFW_MOUSE_BUTTON_LEFT && művelet == GLFW_RELEASE) {
        mozgatottPont = -1;
    }
}

// Inicializáció
void init(GLFWwindow* ablak) {
    renderProgram = createShaderProgram();

    // Antialiasing bekapcsolása
    glEnable(GL_POINT_SMOOTH);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

    // VBO és VAO létrehozása
    glGenBuffers(2, VBO);
    glGenVertexArrays(2, VAO);

    // Görbe VAO és VBO
    glBindVertexArray(VAO[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, görbePontok.size() * sizeof(glm::vec3), görbePontok.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    // Kontrollpontok és kontrollpoligon VAO és VBO
    glBindVertexArray(VAO[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, kontrollPontok.size() * sizeof(glm::vec3), kontrollPontok.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    // Kötések megszüntetése
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// Megjelenítés
void display(GLFWwindow* ablak, double aktuálisIdő) {
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    GLint rajzolásiMódHely = glGetUniformLocation(renderProgram, "drawMode");

    // Bezier-görbe rajzolása
    glUseProgram(renderProgram);
    glUniform1i(rajzolásiMódHely, 0);
    glBindVertexArray(VAO[0]);
    glLineWidth(5.0f);
    glDrawArrays(GL_LINE_STRIP, 0, görbePontok.size());

    // Kontrollpoligon rajzolása
    glUniform1i(rajzolásiMódHely, 1);
    glBindVertexArray(VAO[1]);
    glLineWidth(5.0f);
    glDrawArrays(GL_LINE_STRIP, 0, kontrollPontok.size());

    // Kontrollpontok rajzolása
    glUniform1i(rajzolásiMódHely, 2);
    glPointSize(pontMéret);
    glDrawArrays(GL_POINTS, 0, kontrollPontok.size());
}

// Főprogram
int main(void) {
    if (!glfwInit()) { exit(EXIT_FAILURE); }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    ablak = glfwCreateWindow(ablakSzélesség, ablakMagasság, ablakCím, NULL, NULL);
    glfwMakeContextCurrent(ablak);
    glViewport(0, 0, ablakSzélesség, ablakMagasság);

    // Eseménykezelők beállítása
    glfwSetCursorPosCallback(ablak, cursorPosCallback);
    glfwSetMouseButtonCallback(ablak, mouseButtonCallback);

    if (glewInit() != GLEW_OK) { exit(EXIT_FAILURE); }
    glfwSwapInterval(1);
    init(ablak);

    // Fő ciklus
    while (!glfwWindowShouldClose(ablak)) {
        display(ablak, glfwGetTime());
        glfwSwapBuffers(ablak);
        glfwPollEvents();
    }

    cleanUpScene();
}