#include <gazebo/gazebo.hh>

namespace gazebo
{
  class WorldPluginTutorial : public WorldPlugin
  {
    public: WorldPluginTutorial() : WorldPlugin()
            {
              printf("Hello Plugin of World Model!\n");
            }

    public: void Load(physics::WorldPtr _world, sdf::ElementPtr _sdf)
            {
		printf("Loading the famous plugin...\n");
            }
  };
  GZ_REGISTER_WORLD_PLUGIN(WorldPluginTutorial)
}
