/*
 * Copyright (C) 2016 Open Source Robotics Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
*/
#ifndef GAZEBO_SET_PARAMETERS_HH_
#define GAZEBO_SET_PARAMETERS_HH_

#include "gazebo/gazebo.hh"
#include <gazebo/physics/physics.hh>

#include "ros/ros.h"
#include <ros/console.h>

namespace gazebo
{
  class GAZEBO_VISIBLE SetParametersPlugin : public WorldPlugin {
    // methods
    public: SetParametersPlugin();
    public: virtual void Load(physics::WorldPtr _parent, sdf::ElementPtr _sdf);

    // attributes
    private: std::unique_ptr<ros::NodeHandle> rosNode; /// \brief A node use for ROS transport
  };
}

#endif
