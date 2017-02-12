/*
 * Copyright (C) 2012-2015 Open Source Robotics Foundation
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
#include "gazebo/gazebo.hh"
#include "gazebo/plugins/CameraPlugin.hh"

/** Shared Memory **/
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>

static key_t key[3];


static int fd[2];
#define SHMSZ     1000000 //921600
int shmid[3];
void *shmvoid[3];
namespace gazebo
{
  class CameraDump : public CameraPlugin
  {
    public: CameraDump() : CameraPlugin(), saveCount(0) {printf("Hello CameraDump!\n");}

    public: void Load(sensors::SensorPtr _parent, sdf::ElementPtr _sdf)
    {
      // Don't forget to load the camera plugin
          const char *tmp;
          int i;

          //tmp = (char *)malloc(1000);
          tmp = _sdf->GetDescription().c_str();
          //strcpy(tmp, (char *)&this->sdf->GetDescription());
         printf("Michele Loading ... \n");
        gzmsg << "Michele Loading ...\n";
        // printf("Michele Loading ... %s\n",tmp);

      CameraPlugin::Load(_parent, _sdf);
      gzmsg << "Michele Loading after CameraPlugin Load...\n";
      pipe(fd);
      //Create shared memory segment
      key[0] = 8400;
      key[1] = 8401;
      key[2] = 8402;
      for (i=0; i<3; i++) {
        if ((shmid[i] = shmget(key[i], SHMSZ, IPC_CREAT | 0666)) < 0) {
            perror("shmget");
            // exit(1);
         }
        else printf("Segment %d has been created <%d> ?!\n", i, shmid[i]);
      /*
       * Now we attach the segment to our data space.
       */
        if ((shmvoid[i] = shmat(shmid[i], NULL, 0)) == (void *) -1) {
            perror("shmat");
            //exit(1);
        }
        printf("Segment %d has been attached <%d> ?!\n", i, shmid[i]);
       }

    }

    // Update the controller
    public: void OnNewFrame(const unsigned char *_image,
        unsigned int _width, unsigned int _height, unsigned int _depth,
        const std::string &_format)
    {
      char tmp[1024];
      char *shm[3];
      size_t imagesize;
      
      if (this->saveCount < 3) {
        imagesize=this->parentSensor->GetCamera()->GetImageByteSize(_width, _height, format);
        if (imagesize <= SHMSZ) {
   //         shm = (char *)shmvoid;
            shm[this->saveCount] = (char *)shmvoid[this->saveCount];
            memcpy(shm[this->saveCount], _image, imagesize);
           printf("Image has been copied to shared memory segment %d\n", this->saveCount);
        }
        else printf("Shared Memory Segment too small %ld\n", imagesize);
      
        snprintf(tmp, sizeof(tmp), "/tmp/%s-%02d.jpg",
                    this->parentSensor->GetCamera()->GetName().c_str(), this->saveCount);

        this->parentSensor->GetCamera()->SaveFrame(_image, _width, _height, _depth, _format, tmp);
        gzmsg << "Saving frame [" << this->saveCount
              << "] as [" << tmp << "]\n";
        this->saveCount++;
      }
      else saveCount=0;
    }
    private: int saveCount;
  };

  // Register this plugin with the simulator
  GZ_REGISTER_SENSOR_PLUGIN(CameraDump)
}
