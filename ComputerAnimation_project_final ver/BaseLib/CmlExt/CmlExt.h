
#pragma once

#include <cml/cml.h>

namespace cml
{
	cml::matrix44d_c MatrixRotationQuaternion(const cml::quaterniond &q);
	cml::matrix44d_c MatrixRotationEuler(const cml::vector3d &euler);
	cml::matrix44d_c MatrixRotationEuler(double x, double y, double z);
	cml::matrix44d_c MatrixRotationAxisAngle(const cml::vector3d &axis, double angle);

	cml::matrix44d_c MatrixTranslation(const cml::vector3d &t);
	cml::matrix44d_c MatrixTranslation(double x, double y, double z);

	cml::matrix44d_c MatrixUniformScaling(double s);
	cml::matrix44d_c MatrixScaling(const cml::vector3d &s);
	cml::matrix44d_c MatrixScaling(double x, double y, double z);


	cml::matrix44f_c MatrixRotationEuler(const cml::vector3f &euler);
	cml::matrix44f_c MatrixRotationEuler(float x, float y, float z);
	cml::matrix44f_c MatrixRotationAxisAngle(const cml::vector3f &axis, float angle);

	cml::matrix44f_c MatrixTranslation(const cml::vector3f &t);
	cml::matrix44f_c MatrixTranslation(float x, float y, float z);

	cml::matrix44f_c MatrixUniformScaling(float s);
	cml::matrix44f_c MatrixScaling(const cml::vector3f &s);
	cml::matrix44f_c MatrixScaling(float x, float y, float z);

	cml::quaterniond QuaterRotationAxisAngle(const cml::vector3d &n, double angle);
	cml::quaterniond QuaterRotationEuler(const cml::vector3d &euler);
	cml::quaterniond QuaterRotationEuler(double x, double y, double z);
	
	cml::vector3d Rotate(const cml::quaterniond &q, const cml::vectord &in);

	double operator%(const cml::vector3d a, const cml::vector3d b);

	static vector3d x_axis(1, 0, 0);
	static vector3d y_axis(0, 1, 0);
	static vector3d z_axis(0, 0, 1);
};