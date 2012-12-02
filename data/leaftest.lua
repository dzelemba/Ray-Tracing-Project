root = gr.node('root')

require('leaf')

l1 = gr.node('l1')
l1:add_child(leaf)

l2 = gr.node('l2')
l2:add_child(leaf)
l2:translate(0.0, 1.0, 0.0);
l2:rotate('Y', 180)

root:add_child(l1)
root:add_child(l2)

root:scale(3.0, 3.0, 3.0)
root:rotate('Y', 45)

white_light = gr.light({-100.0, 150.0, 400.0}, {0.9, 0.9, 0.9}, {1, 0, 0})

scene = gr.scene(root, 500, 500,
                 {0, 1, 10}, {0, 0, -1}, {0, 1, 0}, 50,
                 {0.3, 0.3, 0.3}, {white_light})

return scene;
