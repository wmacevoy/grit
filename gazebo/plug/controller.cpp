#include <gazebo/gazebo.hh>
#include <gazebo/physics/Model.hh>

// http://gazebosim.org/wiki/Tutorials/intermediate/animate_joint
//
// used via "plugin" tag in SDF under 
// ~/.gazebo/models/URDF_Test/URDF_Test.SDF
//
namespace gazebo
{
  class CentaurModelPlugin : public ModelPlugin
  {
  public: CentaurModelPlugin() : ModelPlugin() 
    {
      printf("created centaur model plugin.\n");
    }
    
  public: void Load(gazebo::physics::ModelPtr _model, sdf::ElementPtr _sdf)
    {
      std::map<std::string, common::NumericAnimationPtr> anim;
      
      // Create a new animation for the "my_joint" define in the SDF file.
      // The animation will last for 5.0 seconds, and it will repeat
      anim["KneeJoint"].reset(new common::NumericAnimation("KneeJointAnimation", 10.0, true));

      // Create a key frame for the starting position of the joint
      common::NumericKeyFrame *key = anim["KneeJoint"]->CreateKeyFrame(0.0);
      key->SetValue(0.0);

      // Create a key frame half-way through the animation
      key = anim["KneeJoint"]->CreateKeyFrame(2.5);
      key->SetValue(3.0);

      // Create a key frame half-way through the animation
      key = anim["KneeJoint"]->CreateKeyFrame(5.0);
      key->SetValue(0.0);

      // Create a key frame half-way through the animation
      key = anim["KneeJoint"]->CreateKeyFrame(7.5);
      key->SetValue(-3.0);

      // Create a key frame half-way through the animation
      key = anim["KneeJoint"]->CreateKeyFrame(10.0);
      key->SetValue(0.0);

     // Attach the animation to the model
      _model->SetJointAnimation(anim);      
      printf("set KneeJoint animation.\n");
    }
  };
  GZ_REGISTER_MODEL_PLUGIN(CentaurModelPlugin)
}
