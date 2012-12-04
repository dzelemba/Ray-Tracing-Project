-- Materials

water = gr.material({0.0, 0.0, 0.7}, {0.0, 0.0, 0.7}, 25, 1.33, 0.7)
water2 = gr.material({0.0, 0.0, 0.7}, {0.0, 0.0, 0.7}, 25, 1.33, 0.8)
gr.bump(water, 'waterbump.png')

wood = gr.textureMap('wood_texture.png', {0.0, 0.0, 0.0}, 1.0)
gr.bump(wood, 'woodbump.png')

-- Foreground

foreground = gr.node('foreground')

require('leaf')
require('ant')

leafNode = gr.node('leafNode')
leafNode:translate(1.0, 0.0, 0.0)
leafNode:rotate('Z', -45)
leafNode:rotate('X', -45)
leafNode:rotate('Z', -45)

leaf:scale(4.0, 4.0, 4.0)

ant:translate(-0.25, -0.35, 0.15)
ant:rotate('Z', 45)
ant:rotate('X', 90)
ant:scale(0.4, 0.4, 0.4)

b1 = gr.cylinder('branch1')
b1:translate(4.0, 0.5, 0.0)
b1:rotate('Z', 35) 
b1:rotate('X', -90) 
b1:scale(1.0, 1.0, 8.0)
b1:set_material(wood)

w = gr.sphere('droplet')
w:scale(0.2, 0.2, 0.2)
w:set_material(water2)

droplet_number = 1
for _, pt in pairs({
		      {{0.75, -0.25, 0}},
		      {{0, 0.75, 0}},
		      {{-1, -1, 0}}}) do
   droplet = gr.node('droplet' .. tostring(droplet_number))
   leafNode:add_child(droplet)
   droplet:add_child(w)
   droplet:translate(unpack(pt[1]))
   
   droplet_number = droplet_number + 1
end

leafNode:add_child(leaf)
leafNode:add_child(ant)
leafNode:add_child(b1)

b2 = gr.cylinder('branch2')
b2:translate(-4.0, 0.0, 0.0);
b2:rotate('Z', 45)
b2:rotate('X', 90)
b2:scale(0.5, 0.5, 5.0)
b2:set_material(wood)

foreground:add_child(leafNode)
foreground:add_child(b2)

-- Background

skyPic = gr.textureMap('Sky.png', {0.0, 0.0, 0.0}, 1)
sky = gr.square('sky')
sky:scale(100, 110, 1.0)
sky:translate(-0.5, -0.5, -70)
sky:set_material(skyPic)

tree1 = gr.tree('tree', 7.0, 1.0, 6, 3, 2, 2.0, 1.25, 4, 1354577781)
tree1:translate(-3.0, -3.0, -12.0)

tree2 = gr.tree('tree', 7.0, 1.0, 6, 3, 2, 2.0, 1.25, 4, 1354577781)
tree2:translate(3.0, -3.0, -10.0)

-- Water

stream = gr.node('stream')
stream:translate(-3.0, -3.0, -5.0)
stream:rotate('Y', 75)
stream:translate(0.0, 0.0, -10.0)
stream:scale(2.0, 2.0, 20.0)

streamDiff = gr.differenceNode('diff')
stream_c = gr.cylinder('stream')
stream_c:set_material(water)

minusstream = gr.cube('minusStream')
minusstream:translate(-1.0, 0.0, 0.0)
minusstream:scale(2.0, 1.0, 1.0)
minusstream:set_material(water)

streamDiff:add_child(stream_c)
streamDiff:add_child(minusstream)

brown = {139/255, 69/255, 19/255}
mudTexture = gr.material(brown, brown, 5 )
gr.bump(mudTexture, "bumpmap.png")

mud = gr.square('mud')
mud:translate(0.0, -1.1, 0.0)
mud:scale(4.0, 1.0, 1.0)
mud:rotate('X', 90)
mud:translate(-0.5, 0.0, 0.0)
mud:set_material(mudTexture)

stream:add_child(streamDiff)
stream:add_child(mud)

-- Ground

groundDiff = gr.differenceNode('groundDiff')

groundMat = gr.material({0.0, 0.8, 0.0}, {0.0, 0.4, 0.0}, 5)
gr.bump(groundMat, "grassbump.png") 

ground = gr.square('ground', 100)
ground:translate(0.0, -3.0, 0.0)
ground:rotate('X', 90)
ground:translate(-50, -50, 0.0)
ground:set_material(groundMat)

minusground = gr.node('minusground')
minusground:translate(0.0, 0.2, 0.0)
minusground:add_child(stream)

groundDiff:add_child(ground)
groundDiff:add_child(minusground)

root = gr.node('root')
root:add_child(foreground)
root:add_child(sky)
root:add_child(groundDiff)
root:add_child(tree1)
root:add_child(tree2)
root:add_child(stream)

white_light = gr.light({-100.0, 150.0, 400.0}, {0.9, 0.9, 0.9}, {1, 0, 0})

scene = gr.scene(root, 500, 500,
                 {0, 1, 7}, {0, 0, -1}, {0, 1, 0}, 50,
                 {0.3, 0.3, 0.3}, {white_light}, {0.0, 0.0, 0.0})

return scene;
