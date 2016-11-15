#include <boost/bind.hpp>
#include <gazebo/gazebo.hh>
#include <gazebo/physics/physics.hh>
#include <gazebo/common/common.hh>
#include <stdio.h>

namespace gazebo
{
  class ModelPush : public ModelPlugin
  {
    public: void Init()
    {
     printf("Init called !\n");
     int mytype=(int)this->type;
     printf("type=<%d>\n", mytype);
    }
    public: void Load(physics::ModelPtr _parent, sdf::ElementPtr /*_sdf*/)
    {
     printf("Load called !\n");
      // Store the pointer to the model
      this->model = _parent;

      // Listen to the update event. This event is broadcast every
      // simulation iteration.
      this->updateConnection = event::Events::ConnectWorldUpdateBegin(
          boost::bind(&ModelPush::OnUpdate, this, _1));
      size_t st=this->model->GetGripperCount();
      printf("st=<%ld> -- ", st);
      uint32_t id=this->model->GetId();
      printf("id=<%d>\n", id);
    }

    // Called by the world update start event
    public: void OnUpdate(const common::UpdateInfo & /*_info*/)
    {
      // Apply a small linear velocity to the model.
      this->model->SetLinearVel(math::Vector3(.03, 0, 0));
    }

    // Pointer to the model
    private: physics::ModelPtr model;

    // Pointer to the update event connection
    private: event::ConnectionPtr updateConnection;
  };

  // Register this plugin with the simulator
  GZ_REGISTER_MODEL_PLUGIN(ModelPush)
}
