#version 120



varying vec4 diffuse,ambient, specular;
varying vec3 normal, light_dir, eye_dir;
uniform sampler2D diffuse_tex, normal_map;
void main() {
    vec3 tex_color = texture2D(diffuse_tex, gl_TexCoord[0].st).rgb;
    // RGB vectors have no negative numbers so rescale from 0..1 to -1.0..1.0
    vec3 bump_normal = vec3(texture2D(normal_map, gl_TexCoord[0].st))*2.0 - 1.0;
    float bump_scaler = 2.0;
    bump_normal = normalize(vec3(bump_normal.xy* bump_scaler, bump_normal.z));
    // Perform relatively normal per-pixel lighting
    vec3 ld = normalize(light_dir);
    vec3 color = ambient.rgb;
    float ndotl = dot(bump_normal, ld);
    if (ndotl > 0) {
        color += tex_color.rgb * ndotl;
        vec3 reflect_dir = reflect(-ld, bump_normal);
        float spec = max(dot(-normalize(eye_dir), reflect_dir), 0.0);
        vec3 spec_color = specular.rgb * pow(spec, gl_FrontMaterial.shininess);
        color = min(color+spec_color, vec3(1.0));
    }
        gl_FragColor = vec4(color, 1.0);
}