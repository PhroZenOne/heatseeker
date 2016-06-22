precision mediump float;
varying vec2 v_tex_coord_ir;
uniform sampler2D s_irMap;

void main()
{
	gl_FragColor = texture2D( s_irMap, v_tex_coord_ir );
}
