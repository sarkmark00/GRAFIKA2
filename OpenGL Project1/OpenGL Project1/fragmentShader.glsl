#version 330 core

in vec3 vertexColor; // Bemeneti szín a csúcs shadertol
uniform int drawMode; // Rajzolási mód (0: görbe, 1: poligon, 2: kontrollpontok)
out vec4 fragColor;   // Kimeneti szín

void main(void) {
    if (drawMode == 2) { // Kontrollpontok rajzolása
        vec2 coord = gl_PointCoord - vec2(0.5); // Középponttól való eltolás
        float dist = length(coord); // Euklideszi távolság a középponttól
        
        float radius = 0.5; // Teljes kör, 0.5 a maximális sugar
        float alpha = smoothstep(radius, radius - 0.1, dist); // Simább kör szél
        
        if (alpha < 0.01) discard; // Eldobja a nagyon átlátszó részeket

        fragColor = vec4(vertexColor, alpha);
    } else {
        fragColor = vec4(vertexColor, 1.0); // Normál színezés
    }
}