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

/** Shared Memory and Semaphores**/
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>

/** Semaphores  **/
#include <semaphore.h>
#include <sys/stat.h>
#include <fcntl.h>
//And signal
#include <csignal>
#include <sys/wait.h>
#include <errno.h>

#define SHMSZ     1000000 //921600
int shmid[3];
void *shmvoid[3];
static key_t key[] = {8400,8401,8402};

const char *SEM_NAME[]= {"sem0", "sem1", "sem2" };
sem_t *mutex[3];

namespace gazebo
{
  class CameraDump : public CameraPlugin
  {
    public: CameraDump() : CameraPlugin(), saveCount(0) {gzmsg<<"Hello CameraDump!\n";}
    public: ~CameraDump() {
          clean_all(2);
      }

    public: void Load(sensors::SensorPtr _parent, sdf::ElementPtr _sdf)
    {
      // Don't forget to load the camera plugin
          int i;

        gzmsg << "Michele Loading ...\n";

      CameraPlugin::Load(_parent, _sdf);
      gzmsg << "Michele Loading after CameraPlugin Load...\n";

      //Create mutex 0, 1, 2
      for (i=0;i<3;i++){
        mutex[i] = sem_open(SEM_NAME[i],O_CREAT,0644,1);
        if(mutex[i] == SEM_FAILED)
        {
          perror("unable to create semaphore");
          sem_unlink(SEM_NAME[i]);
          exit(-1);
        }
      }

      //Create shared memory segment
      for (i=0; i<3; i++) {
        if ((shmid[i] = shmget(key[i], SHMSZ, IPC_CREAT | 0666)) < 0) {
            perror("shmget");
            // exit(1);
         }
        else gzmsg << "Segment "<< i << " has been created <"<<shmid[i]<<"> ?!\n";
      /*
       * Now we attach the segment to our data space.
       */
        if ((shmvoid[i] = shmat(shmid[i], NULL, 0)) == (void *) -1) {
            perror("shmat");
            //exit(1);
        }
        else gzmsg << "Segment "<< i << " has been attached <"<<shmid[i]<<"> ?!\n";
       }

      //signal handler
      signal(SIGINT, clean_all);

      if ((this->chpid = fork()) < 0){
          perror("Could not fork, run gazebo-streamer manually");
      }
      else if (this->chpid==0){
              //Child process run gazebo-streamer
            //  execl("/home/michele/gst-rtsp-server-1.2.3/examples/run-gazebo-streamer.sh","run-gazebo-streamer.sh", (char*)0);
              execl("/home/michele//gst-rtsp-server-1.2.3/examples/run_gazebo_streamer.sh","gazebo-streamer", (char*)0);
              _exit(127);
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
            sem_wait(mutex[this->saveCount]);
            shm[this->saveCount] = (char *)shmvoid[this->saveCount];
            memcpy(shm[this->saveCount], _image, imagesize);
            sem_post(mutex[this->saveCount]);
           gzmsg<<"Image has been copied to shared memory segment " <<this->saveCount <<"\n";
        }
        else gzerr << "Shared Memory Segment too small " << imagesize;
      
        /**
        snprintf(tmp, sizeof(tmp), "/tmp/%s-%02d.jpg",
                    this->parentSensor->GetCamera()->GetName().c_str(), this->saveCount);

        this->parentSensor->GetCamera()->SaveFrame(_image, _width, _height, _depth, _format, tmp);
        gzmsg << "Saving frame [" << this->saveCount
              << "] as [" << tmp << "]\n";**/
        this->saveCount++;
      }
      else this->saveCount=0;
    }

    static void clean_all(int signum){
        int i, status, pid;
        //Kill child
#if 1
        for (;;) {
                // Remove the zombie process, and get the pid and return code
                pid = wait(&status);
                if (pid < 0) {
                    if (errno == ECHILD) {
                        printf("All children have exited\n");
                        break;
                    }
                    else {
                        perror("Could not wait");
                    }
                }
                else {
                    printf("Child %d exited with status %d\n", pid, status);
                }
            }
#endif
        gzmsg << "ZZZ Free shared memory and semaphores signal "<<signum<<"...\n";
        // printf("Got signal %d ==> Free shared memory and semaphores...\n", signum);
         for (i=0;i<3;i++){
             sem_close(mutex[i]);
             sem_unlink(SEM_NAME[i]);
             shmctl(shmid[i], IPC_RMID,0 );

         }
          gzmsg << "ZZZ Shared memory and semaphores freed...\n";
          //According litterature, no need to call the destructor of CameraPlugin...
          // The destructors are called in reverse order of construction
    }

    private: int saveCount;
    private: int chpid;
  };

  // Register this plugin with the simulator
  GZ_REGISTER_SENSOR_PLUGIN(CameraDump)
}
