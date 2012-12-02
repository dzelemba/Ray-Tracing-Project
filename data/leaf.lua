leafMat = gr.textureMap('leaf.png', {0.0, 0.0, 0.0}, 5)

leaf = gr.node('leaf')

l = gr.square('leaf')
l:translate(-0.5, -0.5, 0.0)
l:set_material(leafMat)

leaf:add_child(l)
