uniform sampler2D input;
uniform float max_iterations;
uniform float iteration_scale;
void main()
{
	vec4 final_value = texture2D(input, gl_TexCoord[0].xy);
	vec2 z = final_value.rg;

	if( dot(z, z) > 4.0 )
	{
		float iteration = final_value.b*iteration_scale;
		float color = fract(iteration/max_iterations*20.0);
		gl_FragColor = vec4(cos(color*3.141592), cos((color-0.5)*3.141592), cos((color-1.0)*3.141592), 1.0);
	}
	else 
	{
		gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
	}
}