-- Simple scene demonstrating extra primitives: cones and cylinders.

mat1 = gr.material({0.7, 1.0, 0.7}, {0.5, 0.7, 0.5}, 25)
gr.bump(mat1, "bumpmap.png")
mat3 = gr.material({1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, 25)
gr.bump(mat3, "bumpmap.png")
wood = gr.textureMap('wood_texture.png', {0.0, 0.0, 0.0}, 10.0)
checkerboard = gr.textureMap('Checkerboard.png', {0.0, 0.0, 0.0}, 10.0)

root = gr.node('root');

z = -6.0;
y = 2.5;

--Spheres 

s1 = gr.sphere('s1')
s1:translate(-2.0, y + 4.0, z)
s1:set_material(mat1)
root:add_child(s1)

s2 = gr.sphere('s2')
s2:translate(2.0, y + 4.0, z)
s2:set_material(mat3)
root:add_child(s2)

-- Cylinder

c1 = gr.cylinder('c1')
c1:translate(-2.0, y + 1.0, z)
c1:rotate('X', 30)
c1:set_material(mat1)
root:add_child(c1)

c2 = gr.cylinder('c2')
c2:translate(2.0, y + 1.0, z)
c2:rotate('X', 120)
c2:set_material(mat3)
root:add_child(c2)

-- Cone

n1 = gr.cone('n1')
n1:translate(-2.0, y-1.5, z)
n1:rotate('X', 60)
n1:set_material(mat1)
root:add_child(n1)

n2 = gr.cone('n2')
n2:translate(2.0, y-1.5, z)
n2:rotate('X', 120)
n2:set_material(mat3)
root:add_child(n2)

-- Cubes

b1 = gr.cube('b1')
b1:translate(-2.5, y-5.0, z)
b1:set_material(mat1)
root:add_child(b1);

b2 = gr.cube('b2')
b2:translate(1.2, y-4.7, z)
b2:rotate('Y', 30)
b2:rotate('X', 30)
b2:set_material(mat3)
root:add_child(b2);

-- Meshes

require('icosa')

icosa:translate(-2.0, y - 7.0, z)
icosa:rotate('X', 30)
icosa:scale(0.5, 0.5, 0.5)
icosa:set_material(mat1)
root:add_child(icosa)

require('dodeca')

dodeca:translate(2.0, y - 7.0, z)
dodeca:rotate('Y', 30)
dodeca:scale(0.5, 0.5, 0.5)
dodeca:set_material(mat3)
root:add_child(dodeca)

white_light = gr.light({-100.0, 150.0, 400.0}, {0.9, 0.9, 0.9}, {1, 0, 0})

scene = gr.scene(root, 500, 800,
                 {0, 1, 10}, {0, 0, -1}, {0, 1, 0}, 50,
                 {0.3, 0.3, 0.3}, {white_light, orange_light})

return scene;
