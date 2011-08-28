// Filename: bulletWheel.cxx
// Created by:  enn0x (17Feb10)
//
////////////////////////////////////////////////////////////////////
//
// PANDA 3D SOFTWARE
// Copyright (c) Carnegie Mellon University.  All rights reserved.
//
// All use of this software is subject to the terms of the revised BSD
// license.  You should have received a copy of this license along
// with this source code in a file named "LICENSE."
//
////////////////////////////////////////////////////////////////////

#include "bulletWheel.h"

////////////////////////////////////////////////////////////////////
//     Function: BulletWheel::Constructor
//       Access: Public
//  Description:
////////////////////////////////////////////////////////////////////
BulletWheel::
BulletWheel(btWheelInfo &info) : _info(info) {

}

////////////////////////////////////////////////////////////////////
//     Function: BulletWheel::get_suspension_rest_length
//       Access: Published
//  Description: Returns the length of the suspension when the
//               vehicle is standing still.
////////////////////////////////////////////////////////////////////
float BulletWheel::
get_suspension_rest_length() const {

  return _info.getSuspensionRestLength();
}

////////////////////////////////////////////////////////////////////
//     Function: BulletWheel::set_suspension_stiffness
//       Access: Published
//  Description: Sets how stiff the suspension shall be.
////////////////////////////////////////////////////////////////////
void BulletWheel::
set_suspension_stiffness(float value) {

  _info.m_suspensionStiffness = value;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletWheel::get_suspension_stiffness
//       Access: Published
//  Description: Returns the stiffness of the suspension.
////////////////////////////////////////////////////////////////////
float BulletWheel::
get_suspension_stiffness() const {

  return _info.m_suspensionStiffness;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletWheel::set_max_suspension_travel_cm
//       Access: Published
//  Description: Sets the maximum distance the suspension can travel
//               out of the resting position in centimeters.
////////////////////////////////////////////////////////////////////
void BulletWheel::
set_max_suspension_travel_cm(float value) {

  _info.m_maxSuspensionTravelCm = value;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletWheel::get_max_suspension_travel_cm
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
float BulletWheel::
get_max_suspension_travel_cm() const {

  return _info.m_maxSuspensionTravelCm;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletWheel::set_friction_slip
//       Access: Published
//  Description: Sets the slipperyness of the tyre.
////////////////////////////////////////////////////////////////////
void BulletWheel::
set_friction_slip(float value) {

  _info.m_frictionSlip = value;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletWheel::get_friction_slip
//       Access: Published
//  Description: Returns how slippery the tyres are.
////////////////////////////////////////////////////////////////////
float BulletWheel::
get_friction_slip() const {

  return _info.m_frictionSlip;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletWheel::set_max_suspension_force
//       Access: Published
//  Description: Sets the maximum suspension force the wheel can
//               handle.
////////////////////////////////////////////////////////////////////
void BulletWheel::
set_max_suspension_force(float value) {

  _info.m_maxSuspensionForce = value;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletWheel::get_max_suspension_force
//       Access: Published
//  Description: Returns the maximum force (weight) the suspension
//               can handle.
////////////////////////////////////////////////////////////////////
float BulletWheel::
get_max_suspension_force() const {

  return _info.m_maxSuspensionForce;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletWheel::set_wheels_damping_compression
//       Access: Published
//  Description: Sets the damping forces applied when the suspension
//               gets compressed.
////////////////////////////////////////////////////////////////////
void BulletWheel::
set_wheels_damping_compression(float value) {

  _info.m_wheelsDampingCompression = value;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletWheel::get_wheels_damping_compression
//       Access: Published
//  Description: Returns the  damping applied to the compressing
//               suspension.
////////////////////////////////////////////////////////////////////
float BulletWheel::
get_wheels_damping_compression() const {

  return _info.m_wheelsDampingCompression;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletWheel::set_wheels_damping_relaxation
//       Access: Published
//  Description: Sets the damping forces applied when the suspension
//               relaxes.
////////////////////////////////////////////////////////////////////
void BulletWheel::
set_wheels_damping_relaxation(float value) {

  _info.m_wheelsDampingRelaxation = value;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletWheel::get_wheels_damping_relaxation
//       Access: Published
//  Description: Returns the damping applied to the relaxing
//               suspension.
////////////////////////////////////////////////////////////////////
float BulletWheel::
get_wheels_damping_relaxation() const {

  return _info.m_wheelsDampingRelaxation;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletWheel::set_roll_influence
//       Access: Published
//  Description: Defines a scaling factor for roll forces that affect
//               the chassis. 0.0 means no roll - the chassis won't
//               ever flip over - while 1.0 means original physical
//               behaviour. Basically, this allows moving the center
//               of mass up and down.
////////////////////////////////////////////////////////////////////
void BulletWheel::
set_roll_influence(float value) {

  _info.m_rollInfluence = value;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletWheel::get_roll_influence
//       Access: Published
//  Description: Returns the factor by which roll forces are scaled.
//               See set_roll_influence.
////////////////////////////////////////////////////////////////////
float BulletWheel::
get_roll_influence() const {

  return _info.m_rollInfluence;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletWheel::set_wheel_radius
//       Access: Published
//  Description: Sets the wheel radius.
////////////////////////////////////////////////////////////////////
void BulletWheel::
set_wheel_radius(float value) {

  _info.m_wheelsRadius = value;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletWheel::get_wheel_radius
//       Access: Published
//  Description: Returns the wheel radius.
////////////////////////////////////////////////////////////////////
float BulletWheel::
get_wheel_radius() const {

  return _info.m_wheelsRadius;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletWheel::set_steering
//       Access: Published
//  Description: Sets the steering angle.
////////////////////////////////////////////////////////////////////
void BulletWheel::
set_steering(float value) {

  _info.m_steering = value;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletWheel::get_steering
//       Access: Published
//  Description: Returns the steering angle in degrees.
////////////////////////////////////////////////////////////////////
float BulletWheel::
get_steering() const {

  return _info.m_steering;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletWheel::set_rotation
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
void BulletWheel::
set_rotation(float value) {

  _info.m_rotation = value;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletWheel::get_rotation
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
float BulletWheel::
get_rotation() const {

  return _info.m_rotation;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletWheel::set_delta_rotation
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
void BulletWheel::
set_delta_rotation(float value) {

  _info.m_deltaRotation = value;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletWheel::get_delta_rotation
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
float BulletWheel::
get_delta_rotation() const {

  return _info.m_deltaRotation;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletWheel::set_engine_force
//       Access: Published
//  Description: Defines how much force should be used to rotate the
//               wheel.
////////////////////////////////////////////////////////////////////
void BulletWheel::
set_engine_force(float value) {

  _info.m_engineForce = value;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletWheel::get_engine_force
//       Access: Published
//  Description: Returns the amount of accelleration force currently
//               applied.
////////////////////////////////////////////////////////////////////
float BulletWheel::
get_engine_force() const {

  return _info.m_engineForce;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletWheel::set_brake
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
void BulletWheel::
set_brake(float value) {

  _info.m_brake = value;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletWheel::get_brake
//       Access: Published
//  Description: Returns the amount of braking force currently
//               applied.
////////////////////////////////////////////////////////////////////
float BulletWheel::
get_brake() const {

  return _info.m_brake;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletWheel::set_skid_info
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
void BulletWheel::
set_skid_info(float value) {

  _info.m_skidInfo = value;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletWheel::get_skid_info
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
float BulletWheel::
get_skid_info() const {

  return _info.m_skidInfo;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletWheel::set_wheels_suspension_force
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
void BulletWheel::
set_wheels_suspension_force(float value) {

  _info.m_wheelsSuspensionForce = value;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletWheel::get_wheels_suspension_force
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
float BulletWheel::
get_wheels_suspension_force() const {

  return _info.m_wheelsSuspensionForce;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletWheel::set_suspension_relative_velocity
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
void BulletWheel::
set_suspension_relative_velocity(float value) {

  _info.m_suspensionRelativeVelocity = value;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletWheel::get_suspension_relative_velocity
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
float BulletWheel::
get_suspension_relative_velocity() const {

  return _info.m_suspensionRelativeVelocity;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletWheel::set_clipped_inv_connection_point_cs
//       Access: Published
//  Description: 
////////////////////////////////////////////////////////////////////
void BulletWheel::
set_clipped_inv_connection_point_cs(float value) {

  _info.m_clippedInvContactDotSuspension = value;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletWheel::get_clipped_inv_connection_point_cs
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
float BulletWheel::
get_clipped_inv_connection_point_cs() const {

  return _info.m_clippedInvContactDotSuspension;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletWheel::set_chassis_connection_point_cs
//       Access: Published
//  Description: Sets the point where the wheel is connected to the
//               chassis.
////////////////////////////////////////////////////////////////////
void BulletWheel::
set_chassis_connection_point_cs(const LPoint3f &pos) {

  nassertv(!pos.is_nan());
  _info.m_chassisConnectionPointCS = LVecBase3f_to_btVector3(pos);
}

////////////////////////////////////////////////////////////////////
//     Function: BulletWheel::get_chassis_connection_point_cs
//       Access: Published
//  Description: Returns the point where the wheel is connected to
//               the chassis.
////////////////////////////////////////////////////////////////////
LPoint3f BulletWheel::
get_chassis_connection_point_cs() const {

  return btVector3_to_LPoint3f(_info.m_chassisConnectionPointCS);
}

////////////////////////////////////////////////////////////////////
//     Function: BulletWheel::set_wheel_direction_cs
//       Access: Published
//  Description: Sets the wheel's forward vector. (Most likely
//               orthogonal to the axle vector.)
////////////////////////////////////////////////////////////////////
void BulletWheel::
set_wheel_direction_cs(const LVector3f &dir) {

  nassertv(!dir.is_nan());
  _info.m_wheelDirectionCS = LVecBase3f_to_btVector3(dir);
}

////////////////////////////////////////////////////////////////////
//     Function: BulletWheel::get_wheel_direction_cs
//       Access: Published
//  Description: Returns the wheel's forward vector relative to the
//               chassis.
////////////////////////////////////////////////////////////////////
LVector3f BulletWheel::
get_wheel_direction_cs() const {

  return btVector3_to_LVector3f(_info.m_wheelDirectionCS);
}

////////////////////////////////////////////////////////////////////
//     Function: BulletWheel::set_wheel_axle_cs
//       Access: Published
//  Description: Determines the wheel axle normal vector.
////////////////////////////////////////////////////////////////////
void BulletWheel::
set_wheel_axle_cs(const LVector3f &axle) {

  nassertv(!axle.is_nan());
  _info.m_wheelAxleCS = LVecBase3f_to_btVector3(axle);
}

////////////////////////////////////////////////////////////////////
//     Function: BulletWheel::get_wheel_axle_cs
//       Access: Published
//  Description: Returns the normal vector of the wheel axle.
////////////////////////////////////////////////////////////////////
LVector3f BulletWheel::
get_wheel_axle_cs() const {

  return btVector3_to_LVector3f(_info.m_wheelAxleCS);
}

////////////////////////////////////////////////////////////////////
//     Function: BulletWheel::set_world_transform
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
void BulletWheel::
set_world_transform(const LMatrix4f &mat) {

  nassertv(!mat.is_nan());
  _info.m_worldTransform = LMatrix4f_to_btTrans(mat);
}

////////////////////////////////////////////////////////////////////
//     Function: BulletWheel::get_world_transform
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
LMatrix4f BulletWheel::
get_world_transform() const {

  return btTrans_to_LMatrix4f(_info.m_worldTransform);
}

////////////////////////////////////////////////////////////////////
//     Function: BulletWheel::set_front_wheel
//       Access: Published
//  Description: Sets if the wheel is steerable.
////////////////////////////////////////////////////////////////////
void BulletWheel::
set_front_wheel(bool value) {

  _info.m_bIsFrontWheel = value;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletWheel::is_front_wheel
//       Access: Published
//  Description: Determines if a wheel is steerable.
////////////////////////////////////////////////////////////////////
bool BulletWheel::
is_front_wheel() const {

  return _info.m_bIsFrontWheel;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletWheel::set_node
//       Access: Published
//  Description: Sets the PandaNode which representates the visual
//               appearance of this wheel.
////////////////////////////////////////////////////////////////////
void BulletWheel::
set_node(PandaNode *node) {

  _info.m_clientInfo = (void *)node;
}

////////////////////////////////////////////////////////////////////
//     Function: BulletWheel::get_node
//       Access: Published
//  Description: Returns the PandaNode which representates the
//               visual appearance of this wheel, if such a
//               representation has been set previously.
////////////////////////////////////////////////////////////////////
PandaNode *BulletWheel::
get_node() const {

  return (_info.m_clientInfo == NULL) ? NULL : (PandaNode *)_info.m_clientInfo;
}

