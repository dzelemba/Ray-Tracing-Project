-- Simple scene demonstrating extra primitives: cones and cylinders.

mat1 = gr.material({0.7, 1.0, 0.7}, {0.5, 0.7, 0.5}, 25)
mat2 = gr.material({0.5, 0.5, 0.5}, {0.5, 0.7, 0.5}, 25)
mat3 = gr.material({1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, 25)
black = gr.material({0.1, 0.1, 0.1}, {0.5, 0.7, 0.5}, 25)

root = gr.node('root');
root:rotate('Y', 15);

cube = gr.cube('cube');
cube:translate(-0.5, -0.5, -0.5);
cube:set_material(mat1);
root:add_child(cube);

sphere = gr.sphere('sphere');
sphere:scale(0.5, 0.5, 0.5);
sphere:translate(0.0, 1.0, 0.0);
sphere:set_material(mat2);
root:add_child(sphere);

lefteye = gr.sphere('lefteye')
lefteye:scale(0.1, 0.1, 0.1)
lefteye:translate(-0.15, 1.2, 0.4)
lefteye:set_material(black)
root:add_child(lefteye)

righteye = gr.sphere('righteye')
righteye:scale(0.1, 0.1, 0.1)
righteye:translate(0.15, 1.2, 0.4)
righteye:set_material(black)
root:add_child(righteye)

cone = gr.cone('cone');
cone:scale(0.5, 0.5, 0.5);
cone:rotate('X', 90);
cone:translate(0.0, 2.0, 0.0);
cone:set_material(mat3);
root:add_child(cone);

cylinder = gr.cylinder('cylinder')
cylinder:scale(0.05, 0.05, 0.5)
cylinder:translate(0.0, 1.0, 0.5);
cylinder:set_material(mat3)
root:add_child(cylinder)

white_light = gr.light({-100.0, 150.0, 400.0}, {0.9, 0.9, 0.9}, {1, 0, 0})

gr.render(root, 'headonbox.png', 500, 500,
	  {0, 1, 5}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {white_light, orange_light})
