-- Simple scene demonstrating extra primitives: cones and cylinders.

mat1 = gr.material({0.7, 1.0, 0.7}, {0.5, 0.7, 0.5}, 25)
mat3 = gr.material({1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, 25)

root = gr.node('root');

-- Cones

cone1 = gr.cone('cone1')
cone1:translate(-3.0, 4.0, 0.0)
cone1:rotate('X', 90);
cone1:set_material(mat3)
root:add_child(cone1)

cone2 = gr.cone('cone2')
cone2:translate(-3.3, 1.0, 0.0)
cone2:rotate('X', 180);
cone2:set_material(mat3)
root:add_child(cone2)

cone3 = gr.cone('cone3')
cone3:translate(-3.0, -2.0, 0.0)
cone3:rotate('X', 270);
cone3:set_material(mat3)
root:add_child(cone3)

-- Cylinder

cylinder1 = gr.cylinder('cylinder1')
cylinder1:translate(3.0, 4.0, 0.0)
cylinder1:rotate('X', 90)
cylinder1:set_material(mat1)
root:add_child(cylinder1)

cylinder2 = gr.cylinder('cylinder2')
cylinder2:translate(3.5, 1.0, 0.0)
cylinder2:rotate('Y', -45)
cylinder2:scale(0.25, 0.25, 2.0)
cylinder2:set_material(mat1)
root:add_child(cylinder2)

cylinder3 = gr.cylinder('cylinder3')
cylinder3:translate(3.0, -1.0, 0.0)
cylinder3:rotate('Y', -30)
cylinder3:rotate('X', 25)
cylinder3:set_material(mat1)
root:add_child(cylinder3)

white_light = gr.light({-100.0, 150.0, 400.0}, {0.9, 0.9, 0.9}, {1, 0, 0})

gr.render(root, 'extraPrimitives.png', 500, 500,
	  {0, 1, 10}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {white_light, orange_light})
