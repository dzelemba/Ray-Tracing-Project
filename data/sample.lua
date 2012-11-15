 -- Description
reflective = gr.material({0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, 0, 1.0)
mat1 = gr.material({0.7, 1.0, 0.7}, {0.5, 0.7, 0.5}, 25)
mat2 = gr.material({0.7, 0.6, 1.0}, {0.5, 0.4, 0.8}, 25)
stone = gr.material({0.8, 0.7, 0.7}, {0.0, 0.0, 0.0}, 0)
hide = gr.material({0.84, 0.6, 0.53}, {0.3, 0.3, 0.3}, 20)
grass = gr.material({0.1, 0.7, 0.1}, {0.0, 0.0, 0.0}, 0)
skyblue = gr.material({0.1, 0.1, 0.8}, {0.0, 0.0, 0.0}, 0)
yellow = gr.material({0.9, 0.9, 0.0}, {0.0, 0.0, 0.0}, 0)

scene_root = gr.node('root')

-- Read in the cow model from a separate file.

require('readobj')

cow_poly = gr.mesh('cow', readobj('cow.obj'))
factor = 2.0/(2.76+3.637)

cow_poly:set_material(hide)

cow_poly:translate(0.0, -1.0, 0.0)
cow_poly:scale(factor, factor, factor)
cow_poly:translate(0.0, 3.637, 0.0)

-- Create the mirrors

frame = gr.cube('frame')
frame:translate(-11, -49, -20)
frame:scale(22.0, 42.0, 1.0)
frame:set_material(stone)
scene_root:add_child(frame)

mirror1 = gr.cube('mirror1')
mirror1:translate(-10, -48, -19)
mirror1:scale(20.0, 40.0, 1.0)
mirror1:set_material(reflective)
scene_root:add_child(mirror1)

mirror2 = gr.nh_box('mirror1', {-50, -50, 100}, 100)
mirror2:set_material(reflective)
scene_root:add_child(mirror2)

-- Add walls

wall = gr.nh_box('wall', {-50, -50, 0}, 100)
wall:set_material(grass)

plane = gr.mesh('plane', {
		   { -1, 0, -1 }, 
		   {  1, 0, -1 }, 
		   {  1,  0, 1 }, 
		   { -1, 0, 1  }
		}, {
		   {3, 2, 1, 0}
		})

scene_root:add_child(plane)
plane:set_material(grass)
plane:translate(0, -50, 0)
plane:scale(300, 300, 300)

sky = gr.cube('sky')
sky:translate(-400, -100, -400)
sky:scale(800, 800, 1)
sky:set_material(skyblue)
scene_root:add_child(sky)

-- the arch

inst = gr.node('inst')

arc = gr.node('arc')
inst:add_child(arc)
arc:translate(0, 0, -15)

p1 = gr.nh_box('p1', {0, 0, 0}, 1)
arc:add_child(p1)
p1:set_material(stone)
p1:translate(-2.4, 0, -0.4)
p1:scale(0.8, 4, 0.8)

p2 = gr.nh_box('p2', {0, 0, 0}, 1)
arc:add_child(p2)
p2:set_material(stone)
p2:translate(1.6, 0, -0.4)
p2:scale(0.8, 4, 0.8)

s = gr.nh_sphere('s', {0, 0, 0}, 1)
arc:add_child(s)
s:set_material(stone)
s:translate(0, 4, 0)
s:scale(4, 0.6, 0.6)

-- Place a ring of arches.

for i = 1, 6 do
   an_arc = gr.node('arc' .. tostring(i))
   an_arc:translate(10, -50, -25)
   an_arc:scale(6.0, 6.0, 6.0)
   an_arc:rotate('Y', 20 + (i-1) * 60)
   scene_root:add_child(an_arc)
   an_arc:add_child(arc)
end

-- Add the cow

cow = gr.node('cow')
cow:translate(10, -40, 50)
cow:rotate('Y', 90)
cow:scale(9, 9, 9)
cow:add_child(cow_poly)
scene_root:add_child(cow)

-- First try a sphere

sphere = gr.nh_sphere('s', {10, -40, 30}, 5)
sphere:set_material(hide)
--scene_root:add_child(sphere)

white_light = gr.light({0, 100.0, 100.0}, {0.9, 0.9, 0.9}, {1, 0, 0})

gr.render(scene_root, 'sample.png', 500, 500,
	  {0, -20, 99}, {0, 0, -1}, {0, 1, 0}, 70,
	  {0.3, 0.3, 0.3}, {white_light})
