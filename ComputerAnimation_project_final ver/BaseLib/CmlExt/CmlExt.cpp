

#include <math.h>
#include "BaseLib/CmlExt/CmlExt.h"


namespace cml
{
	cml::matrix44d_c MatrixRotationQuaternion(const cml::quaterniond &q)
	{
		cml::matrix44d_c m;
		cml::matrix_rotation_quaternion(m, q);

		return m;
	}

	cml::matrix44d_c MatrixRotationEuler(const cml::vector3d& euler)
	{
		cml::matrix44d_c m;
		cml::matrix_rotation_euler(m, euler[0], euler[1], euler[2], cml::euler_order_xyz);

		return m;
	}

	cml::matrix44d_c MatrixRotationEuler(double x, double y, double z)
	{
		cml::matrix44d_c m;
		cml::matrix_rotation_euler(m, x, y, z, cml::euler_order_xyz);

		return m;
	}


	cml::matrix44d_c MatrixRotationAxisAngle(const cml::vector3d &axis, double angle)
	{
		cml::matrix44d_c m;
		cml::matrix_rotation_axis_angle(m, axis, angle);

		return m;
	}

	cml::matrix44d_c MatrixTranslation(double x, double y, double z)
	{
		cml::matrix44d_c m;
		cml::matrix_translation(m, x, y, z);

		return m;
	}

	cml::matrix44d_c MatrixTranslation(const cml::vector3d &t)
	{
		cml::matrix44d_c m;
		cml::matrix_translation(m, t);

		return m;
	}


	cml::matrix44d_c MatrixUniformScaling(double s)
	{
		cml::matrix44d_c m;
		cml::matrix_uniform_scale(m, s);

		return m;
	}

	cml::matrix44d_c MatrixScaling(const cml::vector3d &s)
	{
		cml::matrix44d_c m;
		cml::matrix_scale(m, s);

		return m;
	}

	cml::matrix44d_c MatrixScaling(double x, double y, double z)
	{
		cml::matrix44d_c m;
		cml::matrix_scale(m, x, y, z);

		return m;
	}








	cml::matrix44f_c MatrixRotationEuler(const cml::vector3f& euler)
	{
		cml::matrix44f_c m;
		cml::matrix_rotation_euler(m, euler[0], euler[1], euler[2], cml::euler_order_xyz);

		return m;
	}

	cml::matrix44f_c MatrixRotationEuler(float x, float y, float z)
	{
		cml::matrix44f_c m;
		cml::matrix_rotation_euler(m, x, y, z, cml::euler_order_xyz);

		return m;
	}


	cml::matrix44f_c MatrixRotationAxisAngle(const cml::vector3f &axis, float angle)
	{
		cml::matrix44f_c m;
		cml::matrix_rotation_axis_angle(m, axis, angle);

		return m;
	}

	cml::matrix44f_c MatrixTranslation(float x, float y, float z)
	{
		cml::matrix44f_c m;
		cml::matrix_translation(m, x, y, z);

		return m;
	}

	cml::matrix44f_c MatrixTranslation(const cml::vector3f &t)
	{
		cml::matrix44f_c m;
		cml::matrix_translation(m, t);

		return m;
	}


	cml::matrix44f_c MatrixUniformScaling(float s)
	{
		cml::matrix44f_c m;
		cml::matrix_uniform_scale(m, s);

		return m;
	}

	cml::matrix44f_c MatrixScaling(const cml::vector3f &s)
	{
		cml::matrix44f_c m;
		cml::matrix_scale(m, s);

		return m;
	}

	cml::matrix44f_c MatrixScaling(float x, float y, float z)
	{
		cml::matrix44f_c m;
		cml::matrix_scale(m, x, y, z);

		return m;
	}

	cml::quaterniond QuaterRotationAxisAngle(const cml::vector3d &n, double angle)
	{
		cml::quaterniond q;
		cml::quaternion_rotation_axis_angle(q, n, angle);
		return q;
	}

	cml::quaterniond QuaterRotationEuler(const cml::vector3d &euler)
	{
		cml::quaterniond q;
		cml::quaternion_rotation_euler(q, euler[0], euler[1], euler[2], cml::euler_order_xyz);
		return q;
	}

	cml::quaterniond QuaterRotationEuler(double x, double y, double z)
	{
		cml::quaterniond q;
		cml::quaternion_rotation_euler(q, x, y, z, cml::euler_order_xyz);
		return q;
	}


	cml::vector3d Rotate(const cml::quaterniond &q, const cml::vectord &in)
	{
		double angle;
		cml::vector3d axis;
		cml::quaternion_to_axis_angle( q, axis, angle );
		return cml::rotate_vector(in, axis, angle);
	}


	double operator%(const cml::vector3d a, const cml::vector3d b)
	{
		return cml::dot(a, b);
	}

};