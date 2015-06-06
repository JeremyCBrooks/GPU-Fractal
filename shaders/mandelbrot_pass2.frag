uniform sampler2D input;
uniform float iteration;
void main()
{
	vec4 initial_value = texture2D(input, gl_TexCoord[1].xy);
	vec2 z = initial_value.rg;
	vec2 c = gl_TexCoord[0].xy;
	if( dot(z, z) > 4.0 ) 
	{
		gl_FragColor = initial_value;
	}
	else
	{
		gl_FragColor.rg = vec2(z.x*z.x - z.y*z.y, 2.0*z.x*z.y) + c;
		gl_FragColor.b = iteration;
		gl_FragColor.a = 1.0;
	}
}