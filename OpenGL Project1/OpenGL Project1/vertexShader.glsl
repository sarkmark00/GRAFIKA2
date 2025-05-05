#version 330 core

layout(location = 0) in vec3 position; // Bemeneti cs�cspoz�ci�
uniform int drawMode;                  // Rajzol�si m�d (0: bezier, 1: poligon, 2: kontrollpontok)
out vec3 vertexColor;                  // Kimeneti sz�n a fragment shader sz�m�ra

void main(void) {
    gl_Position = vec4(position, 1.0); // Poz�ci� �tad�sa homog�n koordin�t�kban

    // Sz�n kiv�laszt�sa a rajzol�si m�d alapj�n
    switch (drawMode) {
        case 0: // Bezier
            vertexColor = vec3(0.8, 1.0, 0.0); // RGB(204, 255, 0)
            break;
        case 1: // Kontrollpoligon
            vertexColor = vec3(0.7529, 0.7529, 0.7529); // RGB(192, 192, 192)
            break;
        case 2: // Kontrollpontok
            vertexColor = vec3(0.698, 0.13, 0.13); // RGB(178, 34, 34)
            break;
        default: // Hiba eset�n
            vertexColor = vec3(0.5, 0.5, 0.5); // RGB(128, 128, 128)
            break;
    }
}