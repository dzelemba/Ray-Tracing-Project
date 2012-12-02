#ifndef TREE_HPP
#define TREE_HPP

#include "scene_node.hpp"
#include "primitive.hpp"

/*
  Note: All this logic should really be in a lua file, but this is a lot easier.
  Also, we will allocate a lot of memory on the heap, but since none of it can
  be freed until the ray tracer completes, we won't free it.
*/

class Tree : public SceneNode {
 public:
  Tree(const std::string& name);
  ~Tree();
};

class Branch : public SceneNode {
 public:
  Branch(const std::string& name, int level, const double thickness, const double length);
  Branch(const std::string& name, int level, const double thickness, const double length, 
               double upDist, double upAngle, double zAngle);
  ~Branch();

 private:
  void createGeometryNode(const double length, const double thickness, const double upDist,
                          const double upAngle, const double zAngle);
  void spawnSubBranches(const int level, const double length, const double thickness);
  void createLeaves(const int level, const double thickness, const double branchLength);
};

class Leaf : public SceneNode {
 public:
  Leaf(const std::string& name, const double branchThickness, const double branchLength);
  ~Leaf();
};

#endif
