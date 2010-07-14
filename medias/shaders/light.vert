varying vec3 vertex;
varying vec3 normal;
varying vec3 eye;

void main()
{
    vertex  = vec3(gl_ModelViewMatrix * gl_Vertex);
    normal  = normalize(gl_NormalMatrix * gl_Normal);
    eye     = -normalize(vertex);

    gl_FrontColor       = gl_Color;
    gl_TexCoord[0].xy   = gl_MultiTexCoord0.xy;
    gl_Position         = ftransform();
    gl_ClipVertex       = gl_ModelViewMatrix*gl_Vertex;
}
