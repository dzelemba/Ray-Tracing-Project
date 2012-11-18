#include "scene_node.hpp"
#include <iostream>
#include <cfloat>
#include "transforms.hpp"

SceneNode::SceneNode(const std::string& name)
  : m_name(name)
{
}

SceneNode::~SceneNode()
{
}

void SceneNode::rotate(char axis, double angle)
{
	set_transform(m_trans * rotation(angle, axis) );
}

void SceneNode::scale(const Vector3D& amount)
{
	set_transform(m_trans * scaling(amount));
}

void SceneNode::translate(const Vector3D& amount)
{
	set_transform(m_trans * translation(amount));
}

bool SceneNode::is_joint() const
{
  return false;
}
 
bool SceneNode::intersect(const Point3D& eye, const Vector3D& ray, double offset) const
{
  Vector3D normal;
  Point3D poi;
  return (intersect(eye, ray, offset, poi, normal) != NULL);
}

bool SceneNode::intersect(const Point3D& eye, const Vector3D& ray, const Colour& ambient, const std::list<Light*>& lights, double offset, Colour& c) const
{
  Vector3D normal;
  Point3D poi;

  const GeometryNode* retVal = intersect(eye, ray, offset, poi, normal);
  if (retVal) {
    c = retVal->getColour(this, eye, poi, normal, ambient, lights);
    return true;
  }

  return false;
}

const GeometryNode* SceneNode::intersect(const Point3D& eye, const Vector3D& ray, double offset, Point3D& poi, Vector3D& normal) const
{
  double minT = DBL_MAX;
  
  const GeometryNode* retVal = intersect(eye, ray, offset, normal, minT);
  if (retVal) {
    poi = eye + minT * ray;
  }

  return retVal;
}

const GeometryNode* SceneNode::intersect(const Point3D& eye, const Vector3D& ray, double offset, Vector3D& normal, double& minT) const
{
  const Point3D transEye = m_invtrans * eye;
  const Vector3D transRay = m_invtrans * ray; // Hints say we should be normalizing this everytime?

  const GeometryNode* retVal = NULL;
  const GeometryNode* temp = NULL;
  for (std::list<SceneNode*>::const_iterator it = m_children.begin(); it != m_children.end(); it++) {
    temp = (*it)->intersect(transEye, transRay, offset, normal, minT);
    if (temp) {
      retVal = temp;
    }
  }
  
  if (retVal != NULL) {
    normal = m_invtrans.transpose() * normal;
  }
  return retVal;
}

/* 
  ******** Joint Node *************
*/

JointNode::JointNode(const std::string& name)
  : SceneNode(name)
{
}

JointNode::~JointNode()
{
}

bool JointNode::is_joint() const
{
  return true;
}

void JointNode::set_joint_x(double min, double init, double max)
{
  m_joint_x.min = min;
  m_joint_x.init = init;
  m_joint_x.max = max;
}

void JointNode::set_joint_y(double min, double init, double max)
{
  m_joint_y.min = min;
  m_joint_y.init = init;
  m_joint_y.max = max;
}

/*
  ************ GeometryNode ****************
*/

GeometryNode::GeometryNode(const std::string& name, Primitive* primitive)
  : SceneNode(name),
    m_primitive(primitive)
{
}

GeometryNode::~GeometryNode()
{
}


const GeometryNode* GeometryNode::intersect(const Point3D& eye, const Vector3D& ray, double offset, Vector3D& normal, double& minT) const
{
  const Point3D transEye = m_invtrans * eye;
  const Vector3D transRay = m_invtrans * ray;

  if (m_primitive->intersect(transEye, transRay, offset, minT, normal)) {
    normal = m_invtrans.transpose() * normal;
    return this;
  } else {
    return NULL;
  }

}

Colour GeometryNode::getColour(const SceneNode* rootNode, 
                               const Point3D& eye, const Point3D& poi, const Vector3D& normal,
                               const Colour& ambient, const std::list<Light*>& lights, int recursiveDepth) const
{
  // Normalize the normal
  Vector3D norm = normal;
  norm.normalize();

   // First add ambient light.
  Colour c = m_material->getAmbient(ambient);

  Vector3D viewDirection = eye - poi;
  viewDirection.normalize();

  // Mirror reflections
  double mirrorRefl = m_material->getReflection();
  if (mirrorRefl != 0 && recursiveDepth < 5) {
    Vector3D mirrorDirection = -1 * viewDirection + 2 * viewDirection.dot(norm) * norm;
    Point3D objPOI;
    Vector3D objNormal;
    const GeometryNode* obj = rootNode->intersect(poi, mirrorDirection, 0.1,  objPOI, objNormal);

    Colour reflectionColour(0.0);  
    if (obj) {
      reflectionColour = obj->getColour(rootNode, poi, objPOI, objNormal, ambient, lights, recursiveDepth + 1);
    }

    c = c + mirrorRefl * reflectionColour;
  }
  
  // Now add contributions of all light sources.
  c = c + (1 - mirrorRefl) * getLightContribution(rootNode, poi, viewDirection, norm, lights);

  return c;
}

Colour GeometryNode::getLightContribution(const SceneNode* rootNode, const Point3D& poi, const Vector3D& viewDirection, 
                                          const Vector3D& normal, const std::list<Light*>& lights) const
{
  Colour c(0.0);

  // Using Phong Lighting Model
  for (std::list<Light*>::const_iterator it = lights.begin(); it != lights.end(); it++) {
    Light* light = (*it);

    Vector3D lightDirection = light->position - poi; // Note this needs to point towards the light.
    double r = lightDirection.length();
    lightDirection.normalize();

    // Check if we get a contribution from this light (i.e. check if any objects are in the way)
    if (rootNode->intersect(poi, lightDirection, 0.001)) {
      continue;
    }

    Colour contribution = m_material->getCoefficient(normal, lightDirection, viewDirection) * 
                          light->colour * lightDirection.dot(normal) * 
                          (1 / (light->falloff[0] + light->falloff[1] * r + light->falloff[2] * r * r)); 

    // Ignore negative contributions
    if (contribution.R() >= 0 && contribution.G() >= 0 && contribution.B() >= 0) {
      c = c + contribution;
    }
  }

  return c;
}
