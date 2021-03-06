#include "EventsPlugin.hh"

using namespace gazebo;
GZ_REGISTER_WORLD_PLUGIN(EventsPlugins)

// Constructor
EventsPlugins::EventsPlugins() {
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

void EventsPlugins::Load(physics::WorldPtr _world, sdf::ElementPtr _sdf) {
	// error check
	if (!_world || !_sdf) {
		gzerr << "No world or SDF element specified. Events Plugin won't load." << std::endl;
		return;
	}

	this->Pub = this->rosNode->advertise<std_msgs::Int8>("/Gazebo/ResetWorld", 100);

	this->ResetWorld = event::Events::ConnectWorldReset(
		boost::bind(&EventsPlugins::onResetWorld, this));
}

/*-------------------*/

void EventsPlugins::onResetWorld() {
	std_msgs::Int8 event;
	event.data = 1;

	this->Pub.publish(event);
}
