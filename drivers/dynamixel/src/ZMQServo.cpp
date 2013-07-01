#include "ZMQServo.h"

#include <ctime>
#include <zmq.h>
#include <iostream>
#include <assert.h>


using namespace std;

class FakeServo : public Servo
{
  float currentAngle;
  float goalAngle;
  float minAngle;
  float maxAngle;

  float rate;
  double t;
  
  FakeServo()
  {
    currentAngle = 0;
    goalAngle = 0;
    minAngle = -10*360;
    maxAngle =  10*360;
    rate = 90.0;
    t = double(clock())/double(CLOCKS_PER_SEC);
  }

  void update()
  {
    double now = double(clock())/double(CLOCKS_PER_SEC);
    double delta = now-t;
    t = now;

    float dist = rate*delta;
    float effectiveGoalAngle = goalAngle;

    if (goalAngle < minAngle) effectiveGoalAngle = minAngle;
    if (goalAngle > maxAngle) effectiveGoalAngle = maxAngle;

    if (currentAngle < effectiveGoalAngle - dist) {
      currentAngle += dist;
    } else if (currentAngle < effectiveGoalAngle + dist) {
      currentAngle = effectiveGoalAngle;
    } else {
      currentAngle -= dist;
    }
  }

  float angle() const
  {
    update();
    return currentAngle;
  }

  void angle(float value)
  {
    update();
    goalAngle = value;
  }
};

typedef std:map<int,Servo*> Servos;
Servos servos;

class ZMQServoMessage
{
public:
  enum { SET_ANGLE, GET_ANGLE };

  int id;
  int servo;
  float value;

  Servo& me()
  {
    Servo *ans = 0;
    Servos::iterator i = servos.find(servo);
    if (i == servos.end()) {
      ans = new FakeServo();
    } else {
      ans = i->second;
    }
    return *ans;
  }

  void rx()
  {
    switch(id) {
    case SET_ANGLE: me().angle(value); break;
    }
  }

  void tx()
  {
    switch(id) {
    case GET_ANGLE: value = me().angle(); break;
    }
  }
};

void rx(void *arg)
{
  while (1) {
    zmq_msg_t msg;
    assert(zmq_msg_init (&msg) == 0);
    assert(zmq_recv (socket, &msg, 0) == 0);
    ((ZMQServoMessage*)zmq_msg_data(&msg))->rx();
    zmq_msg_close (&msg);
  }
}

void tx(void *arg) {
  while (1) {
    usleep(int(0.01*1000000));
    
    size_t k = 0, n = servos.size();
    for (Servos::iterator i = servos.begin();
	 i != servos.end();
	 ++i) {
      zmq_msg_t msg;
      ZMQServoMessage data;
      data.id = ZMQServoMessage::GET_ANGLE;
      data.servo = i->first;
      data.tx();
      zmq_msg_init_size(&msg,sizeof(ZMQServoMessage));
      memcpy(zmq_msg_data(&msg),&data,sizeof(ZMQServoMessage));
      zmq_send(tx_socket,&msg, (++c < n) ? ZMQ_SNDMORE : 0);
    }
  }
}

int main(int argc,char **argv) {
  pthread_attr_t tx_attr;
  pthread_t tx_thread;

  pthread_create(tx,0);
  pthread_attr_init(&tx_attr);
  pthread_create(&tx_thread,&tx_attr,tx,0);

  pthread_create(rx,0);
  pthread_attr_init(&rx_attr);
  pthread_create(&rx_thread,&rx_attr,rx,0);

  pthread_join(rx_thread,0);
  pthread_join(tx_thread,0);

  return 0;
}
