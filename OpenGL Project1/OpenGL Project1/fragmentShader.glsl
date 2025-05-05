#version 330 core

in vec3 vertexColor; // Bemeneti sz�n a cs�cs shadertol
uniform int drawMode; // Rajzol�si m�d (0: g�rbe, 1: poligon, 2: kontrollpontok)
out vec4 fragColor;   // Kimeneti sz�n

void main(void) {
    if (drawMode == 2) { // Kontrollpontok rajzol�sa
        vec2 coord = gl_PointCoord - vec2(0.5); // K�z�ppontt�l val� eltol�s
        float dist = length(coord); // Euklideszi t�vols�g a k�z�ppontt�l
        
        float radius = 0.5; // Teljes k�r, 0.5 a maxim�lis sugar
        float alpha = smoothstep(radius, radius - 0.1, dist); // Sim�bb k�r sz�l
        
        if (alpha < 0.01) discard; // Eldobja a nagyon �tl�tsz� r�szeket

        fragColor = vec4(vertexColor, alpha);
    } else {
        fragColor = vec4(vertexColor, 1.0); // Norm�l sz�nez�s
    }
}