#include "scene_node.hpp"
#include <iostream>
#include <cfloat>
#include "transforms.hpp"
#include "scene.hpp"

const Scene* SceneNode::m_scene = NULL;

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

bool SceneNode::intersect(const Point3D& eye, const Vector3D& ray, double offset) const
{
  Vector3D normal;
  Point3D poi;
  return (intersect(eye, ray, offset, poi, normal) != NULL);
}

bool SceneNode::intersect(const Point3D& eye, const Vector3D& ray, const double offset, Colour& c) const
{
  Vector3D normal;
  Point3D poi;

  const GeometryNode* retVal = intersect(eye, ray, offset, poi, normal);
  if (retVal) {
    c = retVal->getColour(eye, poi, normal);
    return true;
  }

  return false;
}

const GeometryNode* SceneNode::intersect(const Point3D& eye, const Vector3D& ray,
                                         const double offset, double& minT) const
{
  Vector3D normal;
  return intersect(eye, ray, offset, normal, minT);
}

const GeometryNode* SceneNode::intersect(const Point3D& eye, const Vector3D& ray, const double offset,
                                         Point3D& poi, Vector3D& normal) const
{
  double minT = DBL_MAX;
  
  const GeometryNode* retVal = intersect(eye, ray, offset, normal, minT);
  if (retVal) {
    poi = eye + minT * ray;
  }

  return retVal;
}

const GeometryNode* SceneNode::intersect(const Point3D& eye, const Vector3D& ray, const double offset,
                                         Vector3D& normal, double& minT) const
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
  ************ GeometryNode ****************
*/

static const double epsilon = 0.0001;

GeometryNode::GeometryNode(const std::string& name, Primitive* primitive)
  : SceneNode(name),
    m_primitive(primitive)
{
}

GeometryNode::~GeometryNode()
{
}


const GeometryNode* GeometryNode::intersect(const Point3D& eye, const Vector3D& ray, const double offset,
                                            Vector3D& normal, double& minT) const
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

Colour GeometryNode::getColour(const Point3D& eye, const Point3D& poi, const Vector3D& normal,
                               const double refractiveIndex, int recursiveDepth) const
{
  Colour c(0.0);

  // Normalize the normal
  // Check if we're inside an object and flip the normal if we are.
  Vector3D norm =  refractiveIndex != 1.0 ? -1 * normal : normal;
  norm.normalize();

  Vector3D viewDirection = eye - poi;
  viewDirection.normalize();

  // Add contribution of reflection + refraction.
  double transparency = m_material->m_transparency;
  if (transparency > 0) {
    double reflectance = getReflectiveRatio(viewDirection, norm, refractiveIndex);
    double transmittance = 1.0 - reflectance;

    Colour c1(0.0);
    if (reflectance > epsilon) {
     c1 = c1 + reflectance * reflectionContribution(viewDirection, norm, poi, refractiveIndex, recursiveDepth);
    }

    if (transmittance > epsilon) {
      c1 = c1 +  transmittance * refractionContribution(viewDirection, norm, poi,
                                                        refractiveIndex, recursiveDepth);  
    }

    c = c + transparency * c1;
  }

  // Now add contributions of all light sources.
  double materialCoeff = 1.0 - transparency;
  if (materialCoeff > 0) {
    c = c + materialCoeff * getLightContribution(poi, viewDirection, norm);
  }
  
  return c;
}

double GeometryNode::getReflectiveRatio(const Vector3D& viewDirection, const Vector3D& normal,
                                        const double refractiveIndex) const
{
  double n1 = refractiveIndex;
  double n2 = m_material->m_refractiveIndex;

  // n2 == 0 signifies fully reflective material
  if (n2 == 0) {
    return 1.0;
  }

  double cosInc = viewDirection.dot(normal); 
  double indexRatio = n1 / n2; 
  double sinT2 = indexRatio * indexRatio * (1.0 - cosInc * cosInc);

  // Check for Total Internal Reflection
  if (sinT2 > 1.0) {
    return 1.0; 
  }

  // Use Fresnel equation to determine the ratio between reflectance and transmittance.
  double cosT = sqrt(1.0 - sinT2);
  double rOrth = (n1 * cosInc - n2 * cosT) / (n1 * cosInc + n2 * cosT);
  double rPar = (n2 * cosInc - n1 * cosT) / (n2 * cosInc + n1 * cosT);

  return (rOrth * rOrth + rPar * rPar) / 2.0;
}

Colour GeometryNode::reflectionContribution(const Vector3D& viewDirection, const Vector3D& normal,
                                            const Point3D& poi, const double refractiveIndex,
                                            int recursiveDepth) const
{
  if (recursiveDepth < 5) {
    Vector3D mirrorDirection = -1 * viewDirection + 2 * viewDirection.dot(normal) * normal;
    Point3D objPOI;
    Vector3D objNormal;
    const GeometryNode* obj = m_scene->root->intersect(poi, mirrorDirection, epsilon,  objPOI, objNormal);

    if (obj) {
      return obj->getColour(poi, objPOI, objNormal, refractiveIndex, recursiveDepth + 1);
    }
  }

  return Colour(0.0);
}

// Assumptions:
//  - No refractive objects inside other refractive objects.
//  - Materials can't have refractiveIndex = 1.
//  - No single plane refractive objects
Colour GeometryNode::refractionContribution(const Vector3D& viewDirection, const Vector3D& normal,
                                            const Point3D& poi, const double refractiveIndex,
                                            int recursiveDepth) const
{
  double n1 = refractiveIndex;
  double n2 = m_material->m_refractiveIndex;
  double cosInc = viewDirection.dot(normal); 
  double indexRatio = n1 / n2; 
  double sinT2 = indexRatio * indexRatio * (1.0 - cosInc * cosInc);

  Vector3D transDirection = -indexRatio * viewDirection +
                             (indexRatio * cosInc - sqrt(1.0 - sinT2)) * normal;
  Point3D objPOI;
  Vector3D objNormal;
  const GeometryNode* obj = m_scene->root->intersect(poi, transDirection, epsilon, objPOI, objNormal);
  
  if (obj) {
    return obj->getColour(poi, objPOI, objNormal, n1 == 1.0 ? n2 : 1.0, recursiveDepth);
  } else {
    return m_scene->getBackground(poi, transDirection);
  }
}

Colour GeometryNode::getLightContribution(const Point3D& poi, const Vector3D& viewDirection, 
                                          const Vector3D& normal) const
{
  m_material->calcDiffuse(m_primitive, poi);
  
  // First add ambient light.
  Colour c = m_material->getAmbient(m_scene->ambient);

  // Using Phong Lighting Model
  for (std::list<Light*>::const_iterator it = m_scene->lights.begin(); it != m_scene->lights.end(); it++) {
    Light* light = (*it);

    Vector3D lightDirection = light->position - poi; // Note this needs to point towards the light.
    double r = lightDirection.length();
    lightDirection.normalize();

    // Check if we get a contribution from this light (i.e. check if any objects are in the way)
    // Ignore transparent objects.
    // TODO: Add supprt for non-fully transparent objects.
    double t = DBL_MAX;
    const GeometryNode* obj = m_scene->root->intersect(poi, lightDirection, epsilon, t);
    while (obj && obj->m_material->m_transparency != 0) {
      obj = m_scene->root->intersect(poi, lightDirection, t + epsilon, t);
    }
    if (obj) {
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
