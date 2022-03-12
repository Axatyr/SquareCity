#version 330 core
in vec4 ourColor;
out vec4 FragColor;  

in vec3 N, L,R,V;
  uniform float time;
 uniform vec2 resolution;
uniform int sceltaVs;

struct PointLight{
	vec3 position;
	vec3 color;
	float power;
 };

 //definizione di una varialie uniform che hala struttura PointLight
uniform PointLight light;
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 
uniform Material material;

 #define pi 3.14159235659
 uniform sampler2D tex0;

 in vec2 frag_coord_st;
void main()
{
    FragColor = vec4(ourColor);
       FragColor=mix(ourColor,texture(tex0,vec2(frag_coord_st.x,1-frag_coord_st.y)),0.5);

       if (sceltaVs==2)
	 {
            vec3 ambient = light.power * material.ambient;

            //diffuse
            float coseno_angolo_theta= max(dot(L,N), 0);

            vec3 diffuse = light.power * light.color * coseno_angolo_theta * material.diffuse;

            //speculare
            float coseno_angolo_alfa =  pow(max(dot(V,R),0),material.shininess);

            vec3 specular =  light.power * light.color * coseno_angolo_alfa * material.specular;

            FragColor = vec4(ambient + diffuse + specular, 1.0);      
    }   
}