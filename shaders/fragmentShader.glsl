#version 330 core
struct Material {
	vec3 ambient;
	sampler2D diffuse;
	sampler2D specular;

	float shininess;
};

struct DirLight {
    vec3 direction;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;

    // For Spotlight
    vec3 direction;
    float cutOff;
    float outerCutOff;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform Material material;
uniform vec3 viewPos;

uniform DirLight dirLight;
#define NR_POINT_LIGHTS 4
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform PointLight spotLight;

vec3 calcDirectionalLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 calcSpotLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
   //FragColor = mix(texture(TexBox, TexCoord), texture(TexFace, TexCoord), 0.2);

   vec3 viewDir = normalize(viewPos - FragPos);
   vec3 result = vec3(0.0);

   result += calcDirectionalLight(dirLight, Normal, viewDir);
   
   for (int i=0; i < NR_POINT_LIGHTS; i++)
		result += calcPointLight(pointLights[i], Normal, FragPos, viewDir);

   result += calcSpotLight(spotLight, Normal, FragPos, viewDir);

   FragColor = vec4(result, 1.0);
}

vec3 calcDirectionalLight(DirLight light, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(-light.direction); // towards light source
	float diffuse_cos = max( dot(normal, lightDir), 0.0 ); // angle btw light and normal vector
	
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec_cos = max( dot(viewDir, reflectDir), 0.0 ); // angle btw viewDir & reflectDir
	float phong = pow(spec_cos, material.shininess);

	vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoord));
	vec3 diffuse = light.diffuse * diffuse_cos * vec3(texture(material.diffuse, TexCoord));
	vec3 specular = light.specular * phong * vec3(texture(material.specular, TexCoord));

	return (ambient + diffuse + specular);
}

vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diffuse_cos = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float phong = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
  			     light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, TexCoord));
    vec3 diffuse  = light.diffuse  * diffuse_cos * vec3(texture(material.diffuse, TexCoord));
    vec3 specular = light.specular * phong * vec3(texture(material.specular, TexCoord));
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

vec3 calcSpotLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    float theta = dot(lightDir, normalize(-light.direction));
    /* For Smoothing Edges */
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp( (theta - light.outerCutOff)/epsilon, 0.0, 1.0 );
    vec3 result;

    result = calcPointLight(light, normal, fragPos, viewDir);
    return intensity * result;
   
}