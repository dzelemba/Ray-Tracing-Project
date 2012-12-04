require('readobj')

hide = gr.material({0.84, 0.6, 0.53}, {0.3, 0.3, 0.3}, 20)

-- #############################################
-- Read in the cow model from a separate file.
-- #############################################

cow_poly = gr.mesh('cow', readobj('cow.obj'))
factor = 2.0/(2.76+3.637)

cow_poly:set_material(hide)

cow_poly:translate(0.0, -1.0, 0.0)
cow_poly:scale(factor, factor, factor)
cow_poly:translate(0.0, 3.637, 0.0)


root = gr.node('root')

cow_number = 1

for _, pt in pairs({
		      {{-7,10,0}, -90},
		      {{7,10,0}, -90},
		      {{-7,0,0}, -90},
		      {{7,0,0}, -90},
		      {{7,-9,0}, -90},
		      {{-7,-9,0}, -90}}) do
   cow_instance = gr.node('cow' .. tostring(cow_number))
   root:add_child(cow_instance)
   cow_instance:add_child(cow_poly)
   cow_instance:translate(unpack(pt[1]))
   cow_instance:rotate('Y', pt[2])
   cow_instance:scale(3, 3, 3)
   
   cow_number = cow_number + 1
end


scene = gr.scene(root, 500, 500, {0, 2, 30}, {0, 0, -1}, {0, 1, 0}, 50,
                 {0.4, 0.4, 0.4}, {gr.light({200, 202, 430}, {0.8, 0.8, 0.8}, {1, 0, 0})})

return scene;
