#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 normal;
uniform sampler2D texture_diffuse1;
uniform vec3 lightPos; 
uniform vec3 viewPos;
//uniform vec3 lightColor;
in vec3 position;
void main()


{   // ambient
	vec3 objectcolor=texture(texture_diffuse1, TexCoords).rgb;
    float ambientStrength = 0.1;
    vec3 ambient =  vec3(0.2,0.2,0.2)*texture(texture_diffuse1, TexCoords).rgb;
  	
    // diffuse 
    
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos -position);
   // vec3 lightDir = normalize(lightPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse =  vec3(0.5,0.5,0.5)*diff *texture(texture_diffuse1, TexCoords).rgb;
            
            
     float specularStrength = 0.5;
     vec3 viewDir = normalize(viewPos - position);
     vec3 reflectDir = reflect(-lightDir, norm);   
     float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	 vec3 specular = specularStrength* spec *texture(texture_diffuse1, TexCoords).rgb;      
     
    vec3 result = (ambient + diffuse+specular) ;
  	FragColor = vec4(result, 1.0);
   //FragColor = vec4(normal, 0.0);
   // FragColor = texture(texture_diffuse1, TexCoords);
    //FragColor=vec4(0.0f,0.0f,1.0f,1.0f);
   
}