#ifndef SCENE_LUA_HPP
#define SCENE_LUA_HPP

#include <string>
#include "scene_node.hpp"
#include "scene.hpp"

Scene* import_lua(const std::string& filename);

#endif
