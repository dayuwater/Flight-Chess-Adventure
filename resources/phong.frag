#version 120


uniform sampler2D color_tex;

varying vec4 diffuse, ambient;
varying vec3 tnorm, ec_position;

void main()
{
    vec3 tex_color = texture2D(color_tex, gl_TexCoord[0].st).rgb;
    vec3 pixel=tex_color*ambient.rgb;
    vec3 n=normalize(tnorm);
    for(int i=0; i<5; i++){
   
        vec3 ldir=normalize(gl_LightSource[i].position.xyz-ec_position);
        float NdotL=max(dot(n,ldir),0.0);
        if(NdotL>0.0){
            pixel+=tex_color*diffuse.rgb*NdotL;
            vec3 r=reflect(-ldir,n);
            float VdotR=max(dot(normalize(-ec_position),r),0.0);
            vec4 specular=gl_FrontMaterial.specular*gl_LightSource[i].specular*pow(VdotR,gl_FrontMaterial.shininess);
            pixel+=specular.rgb;
        
        }
    }
		
	gl_FragColor = vec4(pixel, 1.0);
}