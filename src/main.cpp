#include <iostream>
#include "scene.hpp"
#include "scene_lua.hpp"
#include "renderer.hpp"

int main(int argc, char** argv)
{
  std::string filename = "scene.lua";
  if (argc >= 2) {
    filename = argv[1];
  }

  std::string outfile = filename.substr(0, filename.find_first_of('.')).append(".png");

  Scene* scene = import_lua(filename);
  if (!scene) {
    std::cerr << "Could not open " << filename << std::endl;
    return 1;
  }

  StochasticRenderer renderer(scene);
  renderer.render(outfile);
}

