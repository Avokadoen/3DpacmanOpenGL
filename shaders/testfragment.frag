#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;

void main()
{
  float ambientStrength = 0.95;
  vec3 lightColor = vec3(1, 1, 1);
  vec3 ambient = ambientStrength * lightColor;

  //vec3 result = ambient * objectColor;
  FragColor = vec4(ambient, 1.0) * texture(texture_diffuse1, TexCoords);
}
