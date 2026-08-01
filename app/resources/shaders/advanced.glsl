//#shader vertex
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;
out vec3 Normal;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;;
    TexCoords = aTexCoords;
    gl_Position = projection * view * vec4(FragPos, 1.0);
}


//#shader fragment
#version 330 core

in vec3 FragPos;
in vec2 TexCoords;
in vec3 Normal;

uniform sampler2D texture_diffuse1;

uniform vec3 camPos;

uniform vec3 spotLightPos;
uniform vec3 spotLightDir;

uniform float spotInnerCutOff;
uniform float spotOuterCutOff;

uniform bool spotLightEnabled;
uniform vec3 spotLightDiffuse;

out vec4 FragColor;

void main()
{
    vec3 ambientColor = vec3(0.2);
    vec3 specularColor = vec3(0.6);

    vec3 textureColor = texture(texture_diffuse1, TexCoords).rgb;

    vec3 ambient = ambientColor * textureColor;

    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);

    if (spotLightEnabled)
    {
        vec3 normal = normalize(Normal);
        vec3 lightDir = normalize(spotLightPos - FragPos);
        vec3 viewDir = normalize(camPos - FragPos);

        float theta = dot(lightDir, normalize(-spotLightDir));

        float epsilon = spotInnerCutOff - spotOuterCutOff;
        float spotIntensity = clamp((theta - spotOuterCutOff) / epsilon, 0.0, 1.0);

        float diffuseFactor = max(dot(normal, lightDir), 0.0);

        vec3 reflectedDir = reflect(-lightDir, normal);
        float specularFactor = pow(max(dot(viewDir, reflectedDir), 0.0), 32.0);

        diffuse = spotIntensity * diffuseFactor * spotLightDiffuse * textureColor;
        specular = spotIntensity * specularFactor * specularColor;
    }

    vec3 result = ambient + diffuse + specular;

    FragColor = vec4(result, 1.0);
}