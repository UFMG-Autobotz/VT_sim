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

#include "SetParametersPlugin.hh"

using namespace gazebo;

GZ_REGISTER_WORLD_PLUGIN(SetParametersPlugin)


SetParametersPlugin::SetParametersPlugin() {
  // Initialize ROS
  if (!ros::isInitialized()) {
    int argc = 0;
    char **argv = NULL;
    ros::init(argc, argv, "gazebo", ros::init_options::AnonymousName);
  }

  // Create ROS node
  this->rosNode.reset(new ros::NodeHandle("gazebo"));
}

/*-------------------*/

void SetParametersPlugin::Load(physics::WorldPtr _parent, sdf::ElementPtr _sdf) {
  if (!_parent || !_sdf) {
    gzerr << "No world or SDF element specified. Set Parameters Plugin won't load." << std::endl;
    return;
  }

  sdf::ElementPtr paramElement;
  std::string param, value;

  std::cout << std::endl << "------------------------" << std::endl;
  ROS_INFO_STREAM("On \033[1m" << _parent->GetName() << "\033[0m, Set Parameters Plugin set:");
  std::cout << "------------------------" << std::endl;

  // loop through tags <paramx> starting with <param1>
  int idx = 1;
  std::ostringstream tag;
  tag << "param" << idx;
  while (_sdf->HasElement(tag.str())) {
    paramElement = _sdf->GetElementImpl(tag.str());

    // save ros parameter with name on the attribute parameter with value inside of the tag
    if (paramElement->HasAttribute("parameter")) {
      param = "/" + paramElement->GetAttribute("parameter")->GetAsString();
      value = _sdf->Get<std::string>(tag.str());
      this->rosNode->setParam(param, value);
      ROS_INFO_STREAM("Parameter " << param << " = " << value);
    } else {
      ROS_ERROR_STREAM("Invalid parameter");
    }

    idx++;
    tag.str("");
    tag << "param" << idx;
  }

  std::cout << "------------------------" << std::endl;

}
