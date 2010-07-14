#ifndef LIGHT_COUNT
#define LIGHT_COUNT 1
#endif

varying vec3 vertex;
varying vec3 normal;
varying vec3 eye;

uniform int lightCount;

uniform sampler2D texture;

void compute(in int i, inout vec4 ambient, inout vec4 diffuse, inout vec4 specular)
{
	ambient += gl_LightSource[i].ambient * gl_FrontMaterial.ambient + gl_LightModel.ambient * gl_FrontMaterial.ambient;
	
	vec3 light;
	
	float att = 1.0;
	
	if(gl_LightSource[i].position.w == 1.0)
	{
		light = gl_LightSource[i].position.xyz - vertex;
		float distance = length(light);
		
		att = 1.0 / (gl_LightSource[i].constantAttenuation 
		+ gl_LightSource[i].linearAttenuation 
		* distance
		+ gl_LightSource[i].quadraticAttenuation 
		* distance
		* distance);
	}
	
	else
	{
		light = gl_LightSource[i].position.xyz;
	}
	
	vec3 lightDiri = normalize(light);
	
	float NdotL = max(0.0, dot(normal, lightDiri));
	
	if(NdotL > 0.0)
	{
		float NdotHV = dot(normal, normalize(lightDiri + eye));
		
		diffuse  += gl_FrontMaterial.diffuse * gl_LightSource[i].diffuse * NdotL * att;
		specular += gl_FrontMaterial.specular * gl_LightSource[i].specular * pow(NdotHV, gl_FrontMaterial.shininess) * att;
	}
}

void main()
{
    vec4 ambient = vec4(0);
    vec4 diffuse = vec4(0);
    vec4 specular = vec4(0);
    vec4 emission = gl_FrontMaterial.emission;
        
	//int lc = (lightCount <= 0) ? 1 : lightCount;
	
	for(int i=0 ;i<LIGHT_COUNT; i++)
		compute(i, ambient, diffuse, specular);

    vec4 finalColor = ambient  + diffuse + emission;
    finalColor *= gl_Color;
    finalColor *= texture2D(texture, gl_TexCoord[0].st);
    finalColor += specular;
    
    gl_FragColor = finalColor;
}
