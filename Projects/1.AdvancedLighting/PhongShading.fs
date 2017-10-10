#version 330 core

out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

uniform sampler2D groundTexture;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
    vec3 color = texture(groundTexture, fs_in.TexCoords).rgb;
    // Ambient
    float ambientStrength = 0.05;
    vec3 ambient = ambientStrength * color;
    
    // Diffuse
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * color;

    // Specular
    float specularStrength = 0.5f;
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflecDir = reflect(-lightDir, normal);
    // Phong shading
    float spec = pow(max(dot(viewDir, reflecDir), 0.0f), 8.0);
    
    vec3 specular = vec3(specularStrength) * spec;

    vec3 result = (ambient + diffuse + specular);
    FragColor = vec4(result, 1.0f);
}