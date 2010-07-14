varying vec3 light;
varying vec3 eye;

attribute vec3 tangent;

void main()
{
    vec3 n = normalize(gl_NormalMatrix * gl_Normal);
    vec3 t = normalize(gl_NormalMatrix * tangent);
    vec3 b = cross(n, t);

    vec3 vVertex = vec3(gl_ModelViewMatrix * gl_Vertex);
    
    vec3 tmpVec;
    
    if(gl_LightSource[0].position.w == 1.0)
    tmpVec = gl_LightSource[0].position.xyz - vVertex;
    
    else
    tmpVec = gl_LightSource[0].position.xyz;

    light.x = dot(tmpVec, t);
    light.y = dot(tmpVec, b);
    light.z = dot(tmpVec, n);

    tmpVec = -vVertex;
    eye.x = dot(tmpVec, t);
    eye.y = dot(tmpVec, b);
    eye.z = dot(tmpVec, n);
    
    gl_FrontColor       = gl_Color;
    gl_TexCoord[0].xy   = gl_MultiTexCoord0.xy;
    gl_Position         = ftransform();
}
