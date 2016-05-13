attribute vec4 a_position;
attribute vec2 a_tex_coord;

uniform mat3 u_transform_ir;

varying vec2 v_tex_coord_camera;
varying vec2 v_tex_coord_ir;

void main()                 
{                           
   v_tex_coord_camera = a_tex_coord; 
   
   vec3 transformedCoords = u_transform_ir * vec3(a_tex_coord, 1.0);

   v_tex_coord_ir = a_tex_coord;

   gl_Position = a_position;
}                           