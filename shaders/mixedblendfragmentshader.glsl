precision mediump float;
varying vec2 v_tex_coord_ir;
varying vec2 v_tex_coord_color;
uniform sampler2D s_colorMap;

void main()
{
	vec4 baseColor = texture2D( s_colorMap, v_tex_coord_camera );
	gl_FragColor = baseColor.bgra;
}
