#include "zmq.h"
#include <string>
#include <iostream>
#include <string.h>

#include "ZMQ_OK.h"
#include "ZMQMessage.h"

ZMQMessage::ZMQMessage() 
{ 
  ZMQ_OK(zmq_msg_init (&me)); 
}

ZMQMessage::ZMQMessage(size_t size) 
{ 
  ZMQ_OK(zmq_msg_init_size(&me,size)); 
}

ZMQMessage::ZMQMessage(const void *data, size_t size) 
{ 
  ZMQ_OK(zmq_msg_init_size(&me,size)); 
  memcpy(zmq_msg_data(&me),data,size);
}

void *ZMQMessage::data() 
{ 
  return zmq_msg_data(&me); 
}

int ZMQMessage::recv(ZMQSocket &socket, int arg) 
{ 
  return zmq_recvmsg(socket.me,&me,arg); 
}

int ZMQMessage::recv_timeout(ZMQSocket &socket, int millisecondTimeout) 
{ 
  zmq_pollitem_t items[1];
  items[0].socket = socket.me;
  items[0].events = ZMQ_POLLIN;
  if (zmq_poll (items, 1, millisecondTimeout) > 0) {
    return zmq_recvmsg(socket.me,&me,ZMQ_NOBLOCK);
  } else {
      return -1;
  }
}

int ZMQMessage::send(ZMQSocket &socket, int arg) 
{ 
  return zmq_sendmsg(socket.me,&me,arg); 
}

ZMQMessage::~ZMQMessage() 
{ 
  ZMQ_OK(zmq_msg_close (&me)); 
}
