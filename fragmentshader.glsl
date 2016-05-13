precision mediump float;
varying vec2 v_tex_coord_camera;
varying vec2 v_tex_coord_ir;
uniform sampler2D s_baseMap;
uniform sampler2D s_irMap;

void main()
{
	vec4 baseColor;
	vec4 baseColorCorrected;
	vec4 irColor;

	baseColor = texture2D( s_baseMap, v_tex_coord_camera );
	baseColorCorrected = vec4(baseColor.b, baseColor.g, baseColor.r, baseColor.a);
	irColor = texture2D( s_irMap, v_tex_coord_ir );
	gl_FragColor = baseColorCorrected * (irColor + 0.25);
}
