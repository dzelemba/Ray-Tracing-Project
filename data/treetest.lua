root = gr.tree('tree')

white_light = gr.light({-100.0, 150.0, 400.0}, {0.9, 0.9, 0.9}, {1, 0, 0})

scene = gr.scene(root, 500, 500,
                 {0, 5.0, 20}, {0, 0, -1}, {0, 1, 0}, 50,
                 {0.3, 0.3, 0.3}, {white_light})

return scene;
