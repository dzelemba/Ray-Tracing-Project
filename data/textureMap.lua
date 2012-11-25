-- Simple scene demonstrating extra primitives: cones and cylinders.

mat1 = gr.material({0.7, 1.0, 0.7}, {0.5, 0.7, 0.5}, 25)
mat3 = gr.material({1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, 25)
wood = gr.textureMap('wood_texture.png', {0.0, 0.0, 0.0}, 1.0)
checkerboard = gr.textureMap('Checkerboard.png', {0.0, 0.0, 0.0}, 1.0)

root = gr.node('root');

--Spheres 

s1 = gr.sphere('s1')
s1:translate(-3.0, 4.0, 0.0)
s1:set_material(checkerboard)
root:add_child(s1)

s2 = gr.sphere('s2')
s2:translate(3.0, 4.0, 0.0)
s2:set_material(wood)
root:add_child(s2)

white_light = gr.light({-100.0, 150.0, 400.0}, {0.9, 0.9, 0.9}, {1, 0, 0})

scene = gr.scene(root, 500, 500,
                 {0, 1, 10}, {0, 0, -1}, {0, 1, 0}, 50,
                 {0.3, 0.3, 0.3}, {white_light, orange_light})

return scene;
