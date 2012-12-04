mat1 = gr.material({0.7, 1.0, 0.7}, {0.5, 0.7, 0.5}, 25)
mat3 = gr.material({1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, 25)


-- Objects

s = gr.node('s')

require('icosa')
icosa:set_material(mat1)
icosa:scale(0.5, 0.5, 0.5)
s:add_child(icosa)

c = gr.cylinder('c')
c:translate(0.0, 0.0, -1.0) 
c:scale(0.5, 0.5, 2.0)
c:set_material(mat3)

-- Union

union = gr.node('union')
union:translate(0.0, 4.0, 0.0)
--union:add_child(s)
union:add_child(c)

-- Intersection

inter = gr.intersectionNode('inter')
inter:translate(0.0, 1.0, 0.0)
inter:add_child(s)
inter:add_child(c)

-- Difference

diff1 = gr.differenceNode('diff1')
diff1:translate(0.0, -2.0, 0.0)
diff1:add_child(s)
diff1:add_child(c)

diff2 = gr.differenceNode('diff2')
diff2:translate(0.0, -5.0, 0.0)
diff2:add_child(c)
diff2:add_child(s)

-- Columns

left = gr.node('left')
left:translate(-3.0, 0.0, 0.0)
left:rotate('Y', -30)
left:add_child(union)
left:add_child(inter)
left:add_child(diff1)
left:add_child(diff2)

middle = gr.node('middle')
middle:add_child(union)
--middle:add_child(inter)
--middle:add_child(diff1)
--middle:add_child(diff2)

right = gr.node('right')
right:translate(3.0, 0.0, 0.0)
right:rotate('Y', 30)
right:add_child(union)
right:add_child(inter)
right:add_child(diff1)
right:add_child(diff2)

root = gr.node('root')
root:translate(0.0, 1.5, -3.0)
--root:add_child(left)
root:add_child(middle)
--root:add_child(right)


white_light = gr.light({-100.0, 150.0, 400.0}, {0.9, 0.9, 0.9}, {1, 0, 0})

scene = gr.scene(root, 500, 700,
                 {0, 1, 10}, {0, 0, -1}, {0, 1, 0}, 50,
                 {0.3, 0.3, 0.3}, {white_light, orange_light})

return scene;
