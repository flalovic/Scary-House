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
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoords = aTexCoords;
    gl_Position = projection * view * vec4(FragPos, 1.0);
}


//#shader fragment
#version 330 core

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

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

void main()
{
    vec3 normal = normalize(Normal);
    vec3 viewDir = normalize(camPos - FragPos);
    vec3 textureColor = texture(texture_diffuse1, TexCoords).rgb;

    vec3 ambientColor = vec3(0.2);
    vec3 ambient = ambientColor * textureColor;

    vec3 specularColor = vec3(0.6);

    vec3 pointLightPos = vec3(0.947171, 1.595212, 1.226468);
    vec3 pointLightColor = vec3(2.0, 1.7, 0.5);

    vec3 pointLightDir = normalize(pointLightPos - FragPos);
    float distance = length(pointLightPos - FragPos);

    float constant = 1.0;
    float linear = 0.09;
    float quadratic = 0.032;
    float attenuation = 1.0 / (constant + linear * distance + quadratic * (distance * distance));

    float pointDiffuseFactor = max(dot(normal, pointLightDir), 0.0);
    vec3 pointReflectDir = reflect(-pointLightDir, normal);
    float pointSpecularFactor = pow(max(dot(viewDir, pointReflectDir), 0.0), 32.0);

    vec3 pointDiffuse = pointDiffuseFactor * pointLightColor * textureColor * attenuation;
    vec3 pointSpecular = pointSpecularFactor * specularColor * attenuation;

    vec3 spotDiffuse = vec3(0.0);
    vec3 spotSpecular = vec3(0.0);

    if (spotLightEnabled)
    {
        vec3 spotLightDirNorm = normalize(spotLightPos - FragPos);
        float theta = dot(spotLightDirNorm, normalize(-spotLightDir));

        float epsilon = spotInnerCutOff - spotOuterCutOff;
        float spotIntensity = clamp((theta - spotOuterCutOff) / epsilon, 0.0, 1.0);

        float spotDiffuseFactor = max(dot(normal, spotLightDirNorm), 0.0);
        vec3 spotReflectDir = reflect(-spotLightDirNorm, normal);
        float spotSpecularFactor = pow(max(dot(viewDir, spotReflectDir), 0.0), 32.0);

        vec3 hdrSpotlightBoost = vec3(2.5);
        spotDiffuse = spotIntensity * spotDiffuseFactor * spotLightDiffuse * textureColor * hdrSpotlightBoost;
        spotSpecular = spotIntensity * spotSpecularFactor * specularColor * hdrSpotlightBoost;
    }

    vec3 result = ambient + pointDiffuse + pointSpecular + spotDiffuse + spotSpecular;

    FragColor = vec4(result, 1.0);

    float brightness = dot(result, vec3(0.2126, 0.7152, 0.0722));
//    if (brightness > 1.0)
//        BrightColor = vec4(result, 1.0);
//    else
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}