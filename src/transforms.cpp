// Dusan Zelembaba
// 20349855
// dzelemba

#include "transforms.hpp"

// Return a matrix to represent a counterclockwise rotation of "angle"
// degrees around the axis "axis", where "axis" is one of the
// characters 'x', 'y', or 'z'.
Matrix4x4 rotation(double angle, char axis)
{
  Matrix4x4 r;

	angle = (angle / 360) * 2 * M_PI;

	if (axis == 'z' || axis == 'Z') {
		r[0][0] = cos(angle); r[0][1] = -sin(angle); r[0][2] = 0; r[0][3] = 0; 
		r[1][0] = sin(angle); r[1][1] = cos(angle);  r[1][2] = 0; r[1][3] = 0;
		r[2][0] = 0; 					r[2][1] = 0; 					 r[2][2] = 1; r[2][3] = 0; 
		r[3][0] = 0; 					r[3][1] = 0;				   r[3][2] = 0; r[3][3] = 1;
	} else if (axis == 'x' || axis == 'X') {
		r[0][0] = 1; r[0][1] = 0; 				 r[0][2] = 0; 					r[0][3] = 0; 
		r[1][0] = 0; r[1][1] = cos(angle); r[1][2] = -sin(angle); r[1][3] = 0;
		r[2][0] = 0; r[2][1] = sin(angle); r[2][2] = cos(angle);  r[2][3] = 0; 
		r[3][0] = 0; r[3][1] = 0; 				 r[3][2] = 0; 					r[3][3] = 1;
	} else if (axis == 'y' || axis == 'Y') {
		r[0][0] = cos(angle);  r[0][1] = 0; r[0][2] = sin(angle); r[0][3] = 0; 
		r[1][0] = 0; 					 r[1][1] = 1; r[1][2] = 0; 					r[1][3] = 0;
		r[2][0] = -sin(angle); r[2][1] = 0; r[2][2] = cos(angle); r[2][3] = 0; 
		r[3][0] = 0; 					 r[3][1] = 0; r[3][2] = 0; 					r[3][3] = 1;
	}	else {
		std::cerr << "Undefined Axis" << std::endl;
	}

  return r;
}

// Return a matrix to represent a displacement of the given vector.
Matrix4x4 translation(const Vector3D& displacement)
{
  Matrix4x4 t;
	
	t[0][0] = 1; t[0][1] = 0; t[0][2] = 0; t[0][3] = displacement[0];
	t[1][0] = 0; t[1][1] = 1; t[1][2] = 0; t[1][3] = displacement[1];
	t[2][0] = 0; t[2][1] = 0; t[2][2] = 1; t[2][3] = displacement[2];
	t[3][0] = 0; t[3][1] = 0; t[3][2] = 0; t[3][3] = 1;

  return t;
}

// Return a matrix to represent a nonuniform scale with the given factors.
Matrix4x4 scaling(const Vector3D& scale)
{
  Matrix4x4 s;

	s[0][0] = scale[0]; s[0][1] = 0; 				s[0][2] = 0; 				s[0][3] = 0;
	s[1][0] = 0; 				s[1][1] = scale[1]; s[1][2] = 0; 				s[1][3] = 0;
	s[2][0] = 0; 				s[2][1] = 0; 				s[2][2] = scale[2]; s[2][3] = 0;
	s[3][0] = 0; 				s[3][1] = 0; 				s[3][2] = 0; 				s[3][3] = 1;

  return s;
}
