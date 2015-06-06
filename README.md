Mandelbrot Fractal Explorer on the GPU

left mouse button - zoom in
right mouse button - zoom out
middle mouse drag - pan 

This demonstrates use of the GPU as a general purpose processor.
GPUs are basically massively parallel processors. To compute the mandelbrot
set, we iterate over a function on each pixel until our function reaches
a certain value, or until we cross a max threshold of number of iterations.
Since each pixel can be processed independently of any other (ie, no "scatter"
operations are performed), it means we can implement the algorithm for computing 
the set on the GPU.
I'm using a three pass approach here rather than just implement the entire
algorithm in a single shader. This helps on older (Geforce 5,6,7) hardware that 
had poor support for looping and branching, and even helps on newer hardware since
there is still an upper limit on number of instructions.
 The first pass populates a texture with the real and imaginary cooridates for 
each pixel on the screen.
The second pass computes the iterative process of determining if a pixel is or is
not in the fractal set. I use two textures for this pass.
One stores the previous iteration, and is used by the current iteration
to process a new value. The output from the current iteration will be
used as the input to the next iteration (this technique is called "ping-ponging").
The third and final pass uses the number of iterations for each pixel (computed
in the second pass) to assign a color value.
During user interaction, the application uses less iterations to speed up processing, so there will be "holes" in the set at higher zoom levels.
When the user stops interacting, the application increases the number of iterations to improve the quality.
I am only using single precision floats, so the zoom is limited before the image becomes too pixelated.
