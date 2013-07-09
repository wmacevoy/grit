void reopen()
  {
    okSince = now();
    if (fd != -1) close(fd);
    fd = open(dev.c_str(),O_RDWR|O_NONBLOCK);
    if (fd == -1) {
      cout << "reopen " << dev << " failed" << endl;
      return;
    }
    FD_ZERO(&read_fds);
    FD_SET(fd,&read_fds);
    
    currentTimeout.tv_sec=0;
    currentTimeout.tv_nsec=100000000*0.010;
  }

  dxio(const char *dev_, size_t baud_) 
    : dev(dev_), baud(baud_)
  {
    fd = -1;
    reopen();
  }

  ~dxio()
  {
    if (fd != -1) close(fd);
  }

  void timeout(double t)
  {
    ssize_t tsec=t;
    currentTimeout.tv_sec = tsec;
    currentTimeout.tv_nsec = 1000000000*(t-tsec);
  }

  bool write(ssize_t size, const unsigned char *data)
  {
    ssize_t ans = ::write(fd,data,size);
    if (ans == size) {
      okSince = now();
    } else if (now()-okSince > 0.500) {
      reopen();
    }
    //    cout << "wrote " << ans << " of " << size << " bytes" << endl;
    return ans == size;
  }

  ssize_t read0(size_t size, unsigned char *data)
  {
    ssize_t total = 0;
    while (size > 0) {
      currentTimeout.tv_sec=0;
      currentTimeout.tv_nsec=100000000*0.010;
      int status = pselect(1,&read_fds,0,0,&currentTimeout,0);
      if (status == 1) {
	ssize_t ans = ::read(fd,data,size);
	if (ans == -1) break;
	data += ans;
	size -= ans;
	total += ans;
      } else {
	break;
      }
    }
    return total;
  }

  bool read(ssize_t size, unsigned char *data)
  {
    ssize_t ans = read0(size,data);
    //    cout << "read " << ans << " bytes" << endl;
    return (ans == size);
  }

  bool write_word(int id, int address, unsigned value)
  {
    unsigned char obuf[9] /*,ibuf[6] */;
    obuf[0]=0xFF;
    obuf[1]=0xFF;
    obuf[2]=id;
    obuf[3]=5; /* length */;
    obuf[4]=3; /* write */
    obuf[5]=address;
    obuf[6]=value;
    obuf[7]=(value >> 8);
    obuf[8]=~(obuf[2]+obuf[3]+obuf[4]+obuf[5]+obuf[6]+obuf[7]);
    return write(sizeof(obuf),obuf) == sizeof(obuf);
#if 0
    if (write(sizeof(obuf),obuf) && read(sizeof(ibuf),ibuf)) {
      if (~(ibuf[2]+ibuf[3]+ibuf[4]) == ibuf[5]) {
	return ibuf[4] == 0;
      }
    }
    return false;
#endif
  }

  bool write_byte(int id, int address, unsigned value)
  {
    unsigned char obuf[8],ibuf[6];
    obuf[0]=0xFF;
    obuf[1]=0xFF;
    obuf[2]=id;
    obuf[3]=4; /* length */;
    obuf[4]=3; /* write */
    obuf[5]=address;
    obuf[6]=value;
    obuf[7]=~(obuf[2]+obuf[3]+obuf[4]+obuf[5]+obuf[6]);
    return write(sizeof(obuf),obuf) && read(sizeof(ibuf),ibuf);
  }

  int read_word(int id, int address)
  {
    unsigned char obuf[8],ibuf[9];

    obuf[0]=0xFF;
    obuf[1]=0xFF;
    obuf[2]=id;
    obuf[3]=4; /* length*/
    obuf[4]=2; /* read */
    obuf[5]=address;
    obuf[6]=2;
    obuf[7]=~(obuf[2]+obuf[3]+obuf[4]+obuf[5]+obuf[6]);
    if (write(sizeof(obuf),obuf) && read(sizeof(ibuf),ibuf)) {
      if (~(ibuf[0]+ibuf[1]+ibuf[2]+ibuf[3]+ibuf[4]+ibuf[5]+ibuf[6]) == ibuf[7]) {
	if (ibuf[4] == 0) {
	  return ibuf[5]+(ibuf[6]<<8);
	}
      }
    }
    return -1;
  }
};
