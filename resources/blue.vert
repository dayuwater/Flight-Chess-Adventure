#version 120

varying float  intensity;


void main()
{
    vec3 normal=normalize(gl_NormalMatrix*gl_Normal);
    vec3 vertex_eye=vec3(gl_ModelViewMatrix*gl_Vertex);
    
    vec3 lightdir;
    intensity=0;
    
    for(int i=0; i<4; i++){
        lightdir=normalize(gl_LightSource[i].position.xyz-vertex_eye);
        intensity+=max(dot(normal,lightdir),0.0);
        
        
    }
    intensity/=4;
    
    /*vec3 lightDir = normalize(vec3(gl_LightSource[0].position.xyz - vertex_eye));
     intensity=0;
     intensity += max(dot(normal, lightDir), 0.0);*/
    
    
    
    gl_Position=ftransform();
}


