#include "getJoints.hh"

const unsigned int REVOLUTE = 576;
const unsigned int PRISMATIC = 1088;

Data::Data(physics::ModelPtr _model, sdf::ElementPtr _sdf) {
  this->model = _model;
  this->sdf = sdf;
}

void Data::ReadVariables() {
  joint_param currentJoint;

  // read global velocity pid gains
  math::Vector3 global_vel_pid = math::Vector3(1, 0, 0); // default value
	if (this->sdf->HasElement("vel_pid")) {
		global_vel_pid = this->sdf->Get<math::Vector3>("vel_pid");
	} else if (this->sdf->HasElement("pid")) {
		global_vel_pid = this->sdf->Get<math::Vector3>("pid");
	}

	// read global position pid gains
	math::Vector3 global_pos_pid = math::Vector3(1, 0, 0); // default value
	if (this->sdf->HasElement("pos_pid")) {
		global_pos_pid = this->sdf->Get<math::Vector3>("pos_pid");
	} else if (this->sdf->HasElement("pid")) {
		global_pos_pid = this->sdf->Get<math::Vector3>("pid");
	}

	// read joints
	sdf::ElementPtr jointParameter;

	int idx = 1;
	std::ostringstream tag;
	tag << "joint" << idx;

	while (this->sdf->HasElement(tag.str())) {
		jointParameter = this->sdf->GetElementImpl(tag.str());
		currentJoint.valid = true;  // by default the current joint is valid, it's set to false if something is wrong

    // read specific joint's name
		if (jointParameter->HasAttribute("name")) {
			std::string jointName = jointParameter->GetAttribute("name")->GetAsString(); // get joint's name
			currentJoint.joint = this->model->GetJoint(jointName); // get joint from joint's name

      // transform joint on string to validate it
			std::ostringstream validateJoint;
			validateJoint << currentJoint.joint;

			if (validateJoint.str() == "0") {
				gzerr << jointName << " isn't a valid joint name, " << tag.str() << " will be ignored!" << std::endl;
				currentJoint.valid = false;
			} else {
				int jointType = currentJoint.joint->GetType();  // get joint's type to validate it

				if (jointType != REVOLUTE && jointType != PRISMATIC) {
					gzerr << jointName << " has an invalid type, " << tag.str() << " will be ignored!" << std::endl;
					currentJoint.valid = false;
				} else {
					currentJoint.name = currentJoint.joint->GetScopedName(); // save scoped name (will be used by PID controller)
				}
			}

		} else {
			gzerr << tag.str() << " doesn't have a name and will be ignored!" << std::endl;
			currentJoint.valid = false;
		}

    // read specific joint's velocity PID gains
    math::Vector3 vel_pid = global_vel_pid; // if no PID gains are given, use the global ones
		if (jointParameter->HasElement("vel_pid")) {
			vel_pid = jointParameter->Get<math::Vector3>("vel_pid");
		} else if (this->sdf->HasElement("pid")) {
			vel_pid = jointParameter->Get<math::Vector3>("pid");
		}
    currentJoint.vel_pid = common::PID(vel_pid[0], vel_pid[1], vel_pid[2]);

    // read specific joint's position PID gains
    math::Vector3 pos_pid = global_pos_pid; // if no PID gains are given, use the global ones
		if (jointParameter->HasElement("pos_pid")) {
			pos_pid = jointParameter->Get<math::Vector3>("pos_pid");
		} else if (this->sdf->HasElement("pid")) {
			pos_pid = jointParameter->Get<math::Vector3>("pid");
		}
    currentJoint.pos_pid = common::PID(pos_pid[0], pos_pid[1], pos_pid[2]);

    // read specific joint's velocity (if false or not set, velocity won't be controlled)
		currentJoint.velocity = false;
		if (jointParameter->HasElement("velocity")) {
			currentJoint.velocity = jointParameter->Get<bool>("velocity");
		}

    // read specific joint's position (if false or not set, position won't be controlled)
		currentJoint.position = false;
		if (jointParameter->HasElement("position")) {
			currentJoint.position = jointParameter->Get<bool>("position");
		}

    // read specific joint's velocity topic name
		if (currentJoint.velocity && jointParameter->HasAttribute("vel_topic")) {
			currentJoint.veltopic = "/" + jointParameter->GetAttribute("vel_topic")->GetAsString();
		} else {  // if velocity topic isn't given, use default name
			currentJoint.veltopic = "/" + this->model->GetName() + "/joint_vel_" + currentJoint.joint->GetName();
		}

    // read specific joint's position topic name
		if (currentJoint.position && jointParameter->HasAttribute("pos_topic")) {
			currentJoint.postopic = "/" + jointParameter->GetAttribute("pos_topic")->GetAsString();
		} else {  // if position topic isn't given, use default name
			currentJoint.postopic = "/" + this->model->GetName() + "/joint_pos_" + currentJoint.joint->GetName();
		}

    // add curent joint to joints vector
		this->joints.push_back(currentJoint);

    // increment index to test next joint
		idx++;
		tag.str("");
		tag << "joint" << idx;

  }

  // if no joint is specified (idx not incremented)
	if (idx == 1) {
		this->all = true;

    // read global velocity (if false or not set, velocity won't be controlled)
    bool global_velocity = false;
		if (this->sdf->HasElement("velocity")) {
			global_velocity = this->sdf->Get<bool>("velocity");
		}

    // read global position (if false or not set, position won't be controlled)
    bool global_position = false;
		if (this->sdf->HasElement("position")) {
			global_position = this->sdf->Get<bool>("position");
		}

    // save all valid joints on the joints vector
    int total_n_joints = this->model->GetJointCount();
  	std::vector<physics::JointPtr> allJoints = this->model->GetJoints();
    for (int i = 0; i < total_n_joints; i++) {
      // read joint
      currentJoint.joint = allJoints[i];
      currentJoint.name = allJoints[i]->GetScopedName();

      // validate according to type
      int jointType = currentJoint.joint->GetType();
      currentJoint.valid = jointType == REVOLUTE || jointType == PRISMATIC;

      // read global variables
      currentJoint.vel_pid = common::PID(global_vel_pid[0], global_vel_pid[1], global_vel_pid[2]);
      currentJoint.pos_pid = common::PID(global_pos_pid[0], global_pos_pid[1], global_pos_pid[2]);
      currentJoint.velocity = global_velocity;
      currentJoint.position = global_position;

      // add curent joint to joints vector
  		this->joints.push_back(currentJoint);
    }

	}
}
