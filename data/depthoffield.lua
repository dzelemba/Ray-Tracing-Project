-- Simple scene demonstrating extra primitives: cones and cylinders.

mat1 = gr.material({0.7, 1.0, 0.7}, {0.5, 0.7, 0.5}, 25)
mat3 = gr.material({1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, 25)

root = gr.node('root')
root:scale(2.0, 2.0, 2.0)

s1 = gr.sphere('s1')
s1:set_material(mat1)
root:add_child(s1)

s2 = gr.sphere ('s2')
s2:translate(0.0, 2.0, -8.0)
s2:set_material(mat1)
root:add_child(s2)

s3 = gr.sphere ('s3')
s3:translate(0.0, 6.0, -16.0)
s3:set_material(mat1)
root:add_child(s3)

white_light = gr.light({-100.0, 150.0, 400.0}, {0.9, 0.9, 0.9}, {1, 0, 0})

-- Last Parameter is a point on the focal plane.
-- The focal plane is parallel to the view plane.
scene = gr.scene(root, 500, 500,
                 {0, 2, 10}, {0, 0, -1}, {0, 1, 0}, 50,
                 {0.3, 0.3, 0.3}, {white_light}, {0.0, 12.0, -32.0})

return scene;
