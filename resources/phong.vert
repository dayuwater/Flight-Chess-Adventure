#version 120


varying vec4 diffuse, ambient;
varying vec3 tnorm, ec_position;
	
void main()
{
	// Vertex position in eye coords
	ec_position = vec3(gl_ModelViewMatrix * gl_Vertex);

	// Transform normal into eye space
	tnorm = normalize(gl_NormalMatrix * gl_Normal);
	vec3 light_dir = normalize(gl_LightSource[0].position.xyz - ec_position);
    
    // calculate diffuse and ambient component of the color
    diffuse=gl_FrontMaterial.diffuse*gl_LightSource[0].diffuse;
    ambient=gl_FrontMaterial.ambient*gl_LightSource[0].ambient;
		
    
	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_Position = ftransform(); 
} 


