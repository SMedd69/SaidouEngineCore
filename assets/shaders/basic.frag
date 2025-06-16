#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec4 u_Albedo;
uniform float u_Metallic;
uniform float u_Roughness;

uniform vec3 u_LightDirection;
uniform vec3 u_LightColor;
uniform float u_LightIntensity;

uniform vec3 u_CamPos;

// Simple éclairage de type PBR simplifié (lambert + speculaire simplifié)
void main()
{
    vec3 albedo = u_Albedo.rgb;
    float metallic = u_Metallic;
    float roughness = u_Roughness;

    // Normale normalisée
    vec3 N = normalize(Normal);

    // Lumière directionnelle simplifiée
    vec3 lightDir = normalize(u_LightDirection);
    vec3 lightColor = u_LightColor * u_LightIntensity;

    vec3 V = normalize(u_CamPos - FragPos);
    vec3 L = lightDir;
    vec3 H = normalize(V + L);

    float NdotL = max(dot(N, L), 0.0);
    float NdotV = max(dot(N, V), 0.0);
    float NdotH = max(dot(N, H), 0.0);

    // Diffuse lambertien
    vec3 diffuse = albedo / 3.14159265;

    // Spéculaire simplifié (Blinn-Phong approximation)
    float spec = pow(NdotH, (1.0 - roughness) * 128.0);

    // Mélange entre métal et non métal
    vec3 F0 = mix(vec3(0.04), albedo, metallic);
    vec3 specular = F0 * spec;

    vec3 color = (diffuse + specular) * NdotL * lightColor;

    FragColor = vec4(color, u_Albedo.a);
}