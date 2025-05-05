#version 330 core

layout(location = 0) in vec3 position; // Bemeneti csúcspozíció
uniform int drawMode;                  // Rajzolási mód (0: bezier, 1: poligon, 2: kontrollpontok)
out vec3 vertexColor;                  // Kimeneti szín a fragment shader számára

void main(void) {
    gl_Position = vec4(position, 1.0); // Pozíció átadása homogén koordinátákban

    // Szín kiválasztása a rajzolási mód alapján
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
        default: // Hiba esetén
            vertexColor = vec3(0.5, 0.5, 0.5); // RGB(128, 128, 128)
            break;
    }
}