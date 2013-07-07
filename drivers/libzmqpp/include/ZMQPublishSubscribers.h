#pragma once

class ZMQPublishSubscribes
{
  std::vector < std::string > subscribeAddreses;
  std::string publishAddress;
  int rate;
  bool running;
  std::thread *goTx;
  std::thread *goRx;

  ZMQPublishSubscribes();

  void publish(const std::string &address);
  void subscribe(const std::string &address);

  void tx();
  void rx(const void *msg, ssize_t len);

  void start();
  ~ZMQPublishSubscribes();
};

