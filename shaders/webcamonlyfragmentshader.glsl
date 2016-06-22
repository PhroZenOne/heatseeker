precision mediump float;
varying vec2 v_tex_coord_camera;
uniform sampler2D s_baseMap;

void main()
{
	vec4 baseColor = texture2D( s_baseMap, v_tex_coord_camera );
	gl_FragColor = vec4(baseColor.b, baseColor.g, baseColor.r, baseColor.a);
}
