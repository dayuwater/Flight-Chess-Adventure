#version 120


varying vec2 position;
varying float intensity;
const float max_iterations=100.0;
const float zoom=1.0;
const vec2 center = vec2(-0.5, 0.0);
float layers=10;

varying vec4 diffuse,ambient, specular;
varying vec3 normal, light_dir, eye_dir;
const float bump_density = 40.0;
const float bump_size = 0.2;

float part1(int a){
    return mod(a,8)/4+.0;
}
float part2(int a){
    return mod(a,4)/2+.0;
}
float part3(int a){
    return mod(a,2)+.0;
}
void main()
{
    // Step 1: Mandelbrot Set
    vec3 color = vec3(1.0f, 0.0f, 0.0f); // inner color
    float real = position.x * zoom + center.x;
    float imag = position.y * zoom + center.y;
    float cReal = real;
    float cImag = imag;
    float iter, r2 = 0.0;

    for (iter = 0; iter<max_iterations && r2<500.0; ++iter){
            float tempreal = real;
            real = (tempreal * tempreal) - (imag * imag) + cReal;
            imag = 50.0 * tempreal * imag + cImag;
            r2= (real*real)+(imag*imag);
        }
    
    for(int i=8; i>0; i--){
       
       
            if(r2>=(pow(2,i*2))){
                color=mix(vec3(part1(i),part2(i),part3(i)),vec3(part3(i),part2(i),part1(i)),fract(iter*0.3));
                break;
            }
        
    }
    
    // Step 2: Bump Mapping
    vec3 color2;
    vec2 c = bump_density*gl_TexCoord[0].st;
    vec2 p = (fract(c) - vec2(0.5)); // negate for inward bumps
    float d = dot(p,p);
    float f = inversesqrt(d+1.0);
    // Not on a ‘bump’, use unchanged normal
    if (d >= bump_size) {
        p = vec2(0.0);
        f=1.0;
    }
    vec3 ldir = normalize(light_dir);
    vec3 norm_delta = normalize(vec3(p.x*f, p.y*f*2, 1.0));
    color2 = ambient.rgb;
    float ndotl = dot(norm_delta, ldir);
    if (ndotl > 0.0) {
        color2 += diffuse.rgb * ndotl;
        vec3 reflect_dir = reflect(-ldir, norm_delta);
        float spec = max(dot(-normalize(eye_dir), reflect_dir), 0.0);
        vec3 spec_color = specular.rgb * pow(spec, gl_FrontMaterial.shininess);
        color2 = min(color2 + spec_color, vec3(1.0));
    }
    
    color=mix(color,color2,0.5);

    
               
               
               
               
               
               
               
           
        
    
    /*
    if (r2 >= 640000.0)
        color = mix(vec3(1.0,1.0,1.0), vec3(1.0,1.0,1.0), fract(iter*0.3));
    else if (r2 >= 65536.0)
        color = mix(vec3(1.0,1.1,0.0), vec3(1.0,0.0,1.0), fract(iter*0.3));
    else if (r2 >= 256.0)
        color = mix(vec3(1.0,0.1,1.0), vec3(0.0,1.0,1.0), fract(iter*0.3));
    else if (r2 >= 16.0)
        color = mix(vec3(0.0,1.1,1.0), vec3(1.0,1.0,0.0), fract(iter*0.3));
    else if (r2 >= 4.0)
        color = mix(vec3(0.0,1.1,0.0), vec3(0.0,0.0,1.0), fract(iter*0.3));
    else if (r2 >= 2.0)
        color = mix(vec3(1.0,0.1,0.0), vec3(0.0,1.0,0.0), fract(iter*0.3));
    else if (r2 >= 1.0)
        color = mix(vec3(0.0,0.1,1.0), vec3(1.0,0.0,0.0), fract(iter*0.3));
    else if (r2 >= 0.5)
        color = mix(vec3(1.0,0.1,1.0), vec3(1.0,0.0,0.0), fract(iter*0.3));

    else if (r2 >= 0.25)
        color = mix(vec3(0.0,1.1,1.0), vec3(1.0,0.0,0.0), fract(iter*0.3));
    else if (r2 >= 0.125)
        color = mix(vec3(1.0,1.1,0.0), vec3(1.0,0.0,0.0), fract(iter*0.3));

    else if (r2 >= 0.0625)
        color = mix(vec3(0.0,0.1,1.0), vec3(1.0,0.0,0.0), fract(iter*0.3));
    else if (r2 >= 0.03125)
        color = mix(vec3(0.0,1.1,0.0), vec3(1.0,0.0,0.0), fract(iter*0.3));
    else if (r2 >= 0.015625)
        color = mix(vec3(1.0,0.1,0.0), vec3(1.0,0.0,0.0), fract(iter*0.3));
    else if (r2 >= 0.008888)
        color = mix(vec3(1.0,1.1,1.0), vec3(1.0,0.0,0.0), fract(iter*0.3));
     */





               

    gl_FragColor = vec4 (color, 1.0);
}