#version 330 core
in vec3 TexCoords;
out vec4 FragColor;

uniform samplerCube u_Skybox;
uniform vec3 u_SunDirection; // Ajout

void main() {
    vec3 dir = normalize(TexCoords);

    // Calcul de l'alignement avec la direction du soleil
    float sunAmount = max(dot(dir, normalize(u_SunDirection)), 0.0);

    // Paramètres du soleil
    float sunRadius = 0.02; // Ajuste la taille du soleil
    float sunIntensity = 10.0; // Ajuste la brillance

    // Disque du soleil (plus c'est proche de 1, plus c'est au centre du soleil)
    float sunDisk = smoothstep(sunRadius, sunRadius * 0.5, 1.0 - sunAmount);

    // Couleur du soleil (blanc très lumineux)
    vec3 sunColor = vec3(1.0, 0.95, 0.8) * sunIntensity * sunDisk;

    // Skybox de base
    vec3 sky = texture(u_Skybox, dir).rgb;

    FragColor = vec4(sky + sunColor, 1.0);
}