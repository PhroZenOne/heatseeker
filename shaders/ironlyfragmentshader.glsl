precision mediump float;
varying vec2 v_tex_coord_ir;
varying vec2 v_tex_coord_color;
uniform sampler2D s_ir;

void main()
{
	/*cost vec3 color_map[7] = vec3[](
		vec3(0,0,0),
		vec3(0,0,1),
		vec3(1,0,1),
		vec3(1,0,0),
		vec3(1,1,0),
		vec3(1,1,1),
		vec3(0,1,0) // Should never be used
	);*/

	float ir = texture2D( s_ir, v_tex_coord_ir ).r;
	ir *= 5.0;
	//vec3 color = mix(color_map[(int)ir], color_map[(int)ir + 1], fract(ir));
	vec3 color;
	if (ir < 1.0)
		color = mix(vec3(0,0,0), vec3(0,0,1), fract(ir));
	else if (ir < 2.0)
		color = mix(vec3(0,0,1), vec3(1,0,1), fract(ir));
	else if (ir < 3.0)
		color = mix(vec3(1,0,1), vec3(1,0,0), fract(ir));
	else if (ir < 4.0)
		color = mix(vec3(1,0,0), vec3(1,1,0), fract(ir));
	else if (ir < 5.0)
		color = mix(vec3(1,1,0), vec3(1,1,1), fract(ir));
	else
		color = vec3(1,1,1);

	gl_FragColor = vec4(color, 1);
}
