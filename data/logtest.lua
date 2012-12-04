root = gr.node('root')

wood1 = gr.textureMap('wood_texture.png', {0.0, 0.0, 0.0}, 1.0)
gr.bump(wood1, 'woodbump.png')

log1 = gr.cylinder('log1')
log1:set_material(wood1)

root:add_child(log1)
root:rotate('Z', -45)
root:rotate('X', -90)
root:scale(0.75, 0.75, 3.0)
root:translate(0.0, 0.0, -0.5)

white_light = gr.light({-100.0, 150.0, 400.0}, {0.9, 0.9, 0.9}, {1, 0, 0})

scene = gr.scene(root, 500, 800,
                 {0, 0, 7}, {0, 0, -1}, {0, 1, 0}, 50,
                 {0.3, 0.3, 0.3}, {white_light})

return scene;
