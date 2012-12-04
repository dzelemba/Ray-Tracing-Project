skin = gr.material({0.1, 0.1, 0.1}, {0.1, 0.1, 0.1}, 10)
eyeMat = gr.material({0.1, 0.1, 0.1}, {0.1, 0.1, 0.1}, 25)

-- Ant Parameters

buttWidth = 1.0
buttHeight = 0.5
buttDepth = 0.75

torsoSize = 0.4

headWidth = 0.5
headHeight = 0.25
headDepth = 0.5 

backlegWidth = 0.05
backlegLength = 1.5

antennaeWidth = 0.03
antennaeLength = 1.0

-- Nodes

butt = gr.sphere('butt')
butt:translate(torsoSize + buttWidth , 0.0, 0.0)
butt:scale(buttWidth, buttHeight, buttDepth)
butt:set_material(skin)

torsoNode = gr.node('torsoNode')

torso = gr.sphere('torso')
torso:scale(torsoSize, torsoSize, torsoSize)
torso:set_material(skin)

-- Back Legs

backlegNode = gr.node('upperbacklegNode')
backlegNode:translate(0.0, -5 * torsoSize / 6.0 , 0.0)
backlegNode:rotate('Y', 45)
backlegNode:rotate('X', -30)

upperbackleg = gr.cylinder('upperbackleg')
upperbackleg:scale(backlegWidth, backlegWidth, backlegLength)
upperbackleg:set_material(skin)

lowerbackleg = gr.cone('lowerbackleg')
lowerbackleg:translate(0.0, 0.0, backlegLength)
lowerbackleg:rotate('X', 60)
lowerbackleg:scale(backlegWidth, backlegWidth, backlegLength)

-- Flip cone direction
lowerbackleg:translate(0.0, 0.0, 1.0)
lowerbackleg:rotate('X', 180)
lowerbackleg:set_material(skin)

backlegNode:add_child(upperbackleg)
backlegNode:add_child(lowerbackleg)

otherbackleg = gr.node('otherbackleg')
otherbackleg:rotate('Y', 180)
otherbackleg:add_child(backlegNode)

-- Mid Legs

midleg = gr.node('midleg')
midleg:rotate('Y', -30)
midleg:add_child(backlegNode);

othermidleg = gr.node('othermidleg')
othermidleg:rotate('Y', 180)
othermidleg:add_child(midleg) 

-- Front Legs

frontleg = gr.node('midleg')
frontleg:rotate('Y', -75)
frontleg:add_child(backlegNode);

otherfrontleg = gr.node('othermidleg')
otherfrontleg:rotate('Y', 180)
otherfrontleg:add_child(frontleg)

torsoNode:add_child(torso)
torsoNode:add_child(backlegNode)
torsoNode:add_child(otherbackleg)
torsoNode:add_child(midleg)
torsoNode:add_child(othermidleg)
torsoNode:add_child(frontleg)
torsoNode:add_child(otherfrontleg)

-- Head

headNode = gr.node('headNode')
headNode:translate(-torsoSize - headWidth, 0.0, 0.0)

head = gr.sphere('head')midleg = gr.node('midleg')
midleg:rotate('Y', -30)
midleg:add_child(backlegNode);

othermidleg = gr.node('othermidleg')
othermidleg:rotate('Y', 180)
othermidleg:add_child(midleg) 


head:scale(headWidth, headHeight, headDepth)
head:set_material(skin)

-- Antennae

antennaeNode = gr.node('antennaeNode')
antennaeNode:rotate('Y', -60)

lowerantennae = gr.cylinder('lowerantennae')
lowerantennae:scale(antennaeWidth, antennaeWidth, antennaeLength)
lowerantennae:set_material(skin)

upperantennae = gr.cone('upperantennae')
upperantennae:translate(0.0, 0.0, antennaeLength)
upperantennae:rotate('Y', -30)
upperantennae:scale(antennaeWidth, antennaeWidth, antennaeLength)
-- Flip cone direction
upperantennae:translate(0.0, 0.0, 1.0)
upperantennae:rotate('X', 180)
upperantennae:set_material(skin)

antennaeNode:add_child(lowerantennae)
antennaeNode:add_child(upperantennae)

otherantennae = gr.node('otherantennae')
otherantennae:rotate('X', -180)
otherantennae:add_child(antennaeNode)

headNode:add_child(head)
headNode:add_child(antennaeNode)
headNode:add_child(otherantennae)

--Ant 

ant = gr.node('ant')
ant:add_child(butt)
ant:add_child(torsoNode)
ant:add_child(headNode)
