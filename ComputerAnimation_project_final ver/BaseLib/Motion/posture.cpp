/*
 *  posture.cpp
 *  unagi
 *
 *  Created by normit on 09. 09. 09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
#include "BaseLib/Motion/ml.h"

using namespace cml;
using namespace ml;

static double ACOS( double x )
{
  if (x > 1.0) return 0;
  else if (x < -1.0) return pi();
  else return std::acos(x);
}

Posture::Posture(): leftFootTouched(false), rightFootTouched(false) {
  m_body = NULL;
  init_variable();
}

Posture::Posture(int num_joint): leftFootTouched(false), rightFootTouched(false) {
  m_rotates.resize(num_joint);
  init_variable();
}

void ml::Posture::init_variable() {
  type_ = -1;
  object = 0;
  object_height_correction = 0.0;
  rigid = false;
  status.is_highlight = false;
  status.color = cml::vector3(0., 0., 0.);
  is_interact = false;
}

void ml::Posture::SwapBody( const Body *tobody )
{
  std::vector<cml::matrix3> new_rotates;
  for (int i = 0; i < (int)tobody->num_joint(); ++i) {
	int index = m_body->joint_index(tobody->joint_name(i));
	if (index < 0 || index >= (int)m_body->num_joint()) {
		new_rotates.push_back(cml::identity_3x3());
	} else {
	  new_rotates.push_back(m_rotates[index]);
	}
  }
  m_rotates = new_rotates;
  m_body = tobody;
}

void Posture::body(const Body *body)
{
  m_body = body;
  if(body) m_rotates.resize(body->num_joint());
}

void Posture::ApplyTransf( const cml::transf& t )
{
  cml::transf new_t = t * GetGlobalTransf(0);
  m_rotates[0] = mat3(new_t);
  m_trans = cml::trans(new_t);
}


cml::transf Posture::GetGlobalTransf(int i) const 
{
  transf f;
  f.identity();
	
  while(i) {
	  f = make_transf(m_rotates[i], m_body->offset(i)) * f;
	  i = m_body->parent(i);
  }
  f = make_transf(m_rotates[0], trans()) * f;
  return f;
}

cml::matrix3 Posture::GetGlobalRoation( int i ) const
{
  return cml::mat3(GetGlobalTransf(i));
}
cml::vector3 Posture::GetGlobalTranslation(int i) const
{
  return cml::trans(GetGlobalTransf(i) );
}

void Posture::IkLimb( int joint, const cml::vector3& pos, bool reverse )
{
  int lower = body()->parent(joint);
  int upper = body()->parent(lower);

  vector3 B = GetGlobalTranslation (joint);
  vector3 C = GetGlobalTranslation (lower);
  vector3 A = GetGlobalTranslation (upper);

  //error ó��
  if ((B - pos).length() < 0.000001)
    return;

  vector3 L = B - A;
  vector3 N = B - C;
  vector3 M = C - A;

  double l = L.length();
  double n = N.length();
  double m = M.length();

  double a = ACOS((l*l + n*n - m*m) / (2*l*n));
  double b = ACOS((l*l + m*m - n*n) / (2*l*m));

  vector3 B_new = pos;
  vector3 L_new = B_new - A;

  double l_ = L_new.length();

  double a_ = ACOS((l_*l_ + n*n - m*m) / (2*l_*n));
  double b_ = ACOS((l_*l_ + m*m - n*n) / (2*l_*m));

  //���� �ʱⰪ������ �ݴ�� ���̴� ��찡 ����� �̸� �ݴ�� ����
  if (reverse == true)
  {
    a_ *= -1.0;
    b_ *= -1.0;
  }

  vector3 rotV = normalize(cross(M,L));

  double rotb = b - b_;
  double rota = a_ - a - rotb;

  //���󿡼� ȸ���ؼ� ���߱�
  RotateGlobalRotation(upper, exp_mat3(rotV * rotb));
  RotateGlobalRotation(lower, exp_mat3(rotV * rota));

  ////���ۿ��� ȸ���ؼ� ���߱�
  vector3 rotV2 = normalize(cross(L, L_new) );

  double l_new = L_new.length();
  double l_diff = (L_new - L).length();

  double rot2 = ACOS((l_new * l_new + l * l - l_diff * l_diff) / (2.0 * l_new * l));
  RotateGlobalRotation(upper, exp_mat3(rotV2 * rot2));
}

void Posture::SetGlobalRotation( int i, const cml::matrix3& rot )
{
  matrix3 parent_gloal = GetGlobalRoation(m_body->parent(i));
  m_rotates[i] = transpose(parent_gloal) * rot;
}

void Posture::RotateGlobalRotation( int i, const cml::matrix3 &rot )
{
  SetGlobalRotation(i, rot * GetGlobalRoation(i));
}

void ml::smoothing( Posture& p_modified, const Posture& p_fixed, const Posture& orig, int time, int range, bool forward )
{
  std::pair<cml::vector3d, std::vector<cml::vector3d>> tmp_d;
  if (forward) {
	add_difference(p_modified, tmp_d=difference(p_fixed, orig), scalarTransitionFunc(static_cast<double>(time), static_cast<double>(range)));
  } else {
	add_difference(p_modified, tmp_d=difference(p_fixed, orig), 1.0-scalarTransitionFunc(static_cast<double>(time), static_cast<double>(range)));
  }
}

cml::vector3 ml::shoulder_orientation( const Posture &pos )
{
  cml::vector3 right_shoulder(pos.GetGlobalTranslation(12));
  cml::vector3 left_shoulder(pos.GetGlobalTranslation(8));
  cml::vector3 head(pos.GetGlobalTranslation(7));
  return plane_vec3(cross(left_shoulder - right_shoulder, head - right_shoulder)).normalize();
}

