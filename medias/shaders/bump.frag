varying vec3 light;
varying vec3 eye;

uniform sampler2D colorMap;
uniform sampler2D normalMap;

void main()
{
    vec4 ambient = gl_LightSource[0].ambient * gl_FrontMaterial.ambient + gl_LightModel.ambient * gl_FrontMaterial.ambient;
    vec4 emission = gl_FrontMaterial.emission;
    vec4 diffuse;
    vec4 specular;
    
    float att = 1.0;
    
    if(gl_LightSource[0].position.w == 1.0)
    {
        float distance = length(light);
        
        att = 1.0 / (gl_LightSource[0].constantAttenuation 
        + gl_LightSource[0].linearAttenuation 
        * distance
        + gl_LightSource[0].quadraticAttenuation 
        * distance
        * distance);
    }
    
    vec3 lightDiri = normalize(light);
    vec3 bumpNormal = normalize( texture2D(normalMap, gl_TexCoord[0].xy).xyz * 2.0 - 1.0);

    float NdotL = max(0.0, dot(bumpNormal, lightDiri));
    
    if(NdotL > 0.0)
    {
        float NdotHV = dot(bumpNormal, normalize(lightDiri + eye));
        
        diffuse  = gl_FrontMaterial.diffuse * gl_LightSource[0].diffuse * NdotL * att;
        specular = gl_FrontMaterial.specular * gl_LightSource[0].specular * pow(NdotHV, gl_FrontMaterial.shininess) * att;
    }
    
    gl_FragColor = ambient + emission + diffuse;
    gl_FragColor *= gl_Color;
    gl_FragColor *= texture2D(colorMap, gl_TexCoord[0].xy);
    gl_FragColor += specular;
}
