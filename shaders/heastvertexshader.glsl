attribute vec4 a_position;
attribute vec2 a_tex_coord;

varying vec2 v_tex_coord_ir;
varying vec2 v_tex_coord_color;

//uniform mat4 u_calibrate;

void main()
{
    gl_Position = a_position;

	v_tex_coord_ir = a_tex_coord;
    //if (a_tex_coord.x < 0.5f)
    v_tex_coord_color = a_tex_coord;
}