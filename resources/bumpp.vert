#version 120

varying vec4 diffuse, ambient, specular;
varying vec3 normal, eye_dir, light_dir;
attribute vec3 tan; // attribute from cpu side
void main()
{
    normal = normalize(gl_NormalMatrix * gl_Normal);
    vec3 tangent = normalize(gl_NormalMatrix * tan);
    vec3 binormal = normalize(cross(normal, tangent));
    tangent = normalize(cross(binormal , normal));
    
    // a matrix to transform light and eye vecs into  tangent space
    mat3 tanmat = transpose(mat3(tangent, binormal, normal));
    vec3 vertex_eye = (gl_ModelViewMatrix*gl_Vertex).xyz;
    light_dir = tanmat*normalize(gl_LightSource[0].position.xyz - vertex_eye);
    eye_dir = tanmat * normalize(vertex_eye);
    
    // Get the diffuse, specular and ambient colors
    diffuse = gl_FrontMaterial.diffuse * gl_LightSource[0].diffuse;
    specular = gl_FrontMaterial.specular * gl_LightSource[0].specular;
    ambient = gl_FrontMaterial.ambient* gl_LightSource[0].ambient;
    
    
    gl_TexCoord[0] = gl_MultiTexCoord0;
    gl_Position = ftransform();
}


