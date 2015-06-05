#version 120



varying float intensity;

float layers=1000;
void main()
{
    vec4 color;
    for(float i=1; i<layers+1; i++){
        if(intensity>=1-(1/layers)*i){
            if(intensity<=1-(1/layers)*(i-1)){
                color=vec4(0.9-(0.9/layers)*i,0.2-(0.2/layers)*i,0.1-(0.1/layers)*i,1.0);
            }
        }
    }
    
    
    
    
    gl_FragColor=color;
}