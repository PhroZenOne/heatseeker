attribute vec4 a_position;
attribute vec2 a_tex_coord;

varying vec2 v_tex_coord_camera;
varying vec2 v_tex_coord_ir;

mat3 scale(float x, float y){
    return mat3(
        vec3(x,   0.0, 0.0),
        vec3(0.0, y,   0.0),
        vec3(0.0, 0.0, 1.0)
    );
}

mat3 inverse(mat3 m) {
    float a00 = m[0][0], a01 = m[0][1], a02 = m[0][2];
    float a10 = m[1][0], a11 = m[1][1], a12 = m[1][2];
    float a20 = m[2][0], a21 = m[2][1], a22 = m[2][2];

    float b01 =  a22 * a11 - a12 * a21;
    float b11 = -a22 * a10 + a12 * a20;
    float b21 =  a21 * a10 - a11 * a20;

    float det = a00 * b01 + a01 * b11 + a02 * b21;

    return mat3(b01, (-a22 * a01 + a02 * a21), (a12 * a01 - a02 * a11),
                b11, (a22 * a00 - a02 * a20), (-a12 * a00 + a02 * a10),
                b21, (-a21 * a00 + a01 * a20), (a11 * a00 - a01 * a10)) / det;
}

mat3 translate(float x, float y){
    return mat3(
        vec3(1.0, 0.0, 0.0),
        vec3(0.0, 1.0, 0.0),
        vec3(x,   y,   1.0)
    );
}

void main()
{
	v_tex_coord_camera = a_tex_coord; 
	vec3 transformedCoords = inverse(scale(0.64, 0.78) * translate(0.32, 0.308)) * vec3(a_tex_coord, 1.0);
	v_tex_coord_ir = transformedCoords.xy;
	gl_Position = a_position;
}