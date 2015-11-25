/*
  ByteQueue is a non-standard queue that stores a fixed number of
  bytes (unsigned chars). When it reaches the end, it loops back up
  to the top and starts overwriting old data.

  Our system will pull 100 bytes at a time (one packet).

  We assume we have a shared pointer to an int value for 
  the cursor from the Realtime unit.
*/
#ifndef BYTEQUEUE_HPP
#define BYTEQUEUE_HPP

#include <stdint.h>
#include <cstring>
#include <assert.h>
#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
#include "Util.hpp"


class ByteQueue {

public:

  ByteQueue(uint32_t max_bytes);
  ~ByteQueue();

  // Returns pointer to current location of queue and moves cursor
  // Assumes you want 87 bytes at a time
  void pop(uint8_t* packet);
  
  // Adds data to the queue
  void push(uint8_t* bytes);

  void setLength(int n) { *((uint32_t*)_length) = n; }
  uint32_t getLength() { return *((uint32_t*)_length); }
  
  // Pru will have cursor be 5, 97, 189, 281 etc.
  // We have a set addr for _data, so we
  // want to reference it by 0, 92, 184 etc.
  uint32_t pruCursor() {
    if (*((uint32_t*)_pru_cursor) == 5)
      return 0;
    else
      return *((uint32_t*)_pru_cursor) - 5;
  }
  
  void* dataLocation() { return _data; }

  // Returns pointer to current location of queue
  uint8_t* peek() { return (uint8_t*)_data + _internal_cursor; }

  // four bytes for length
  void* _length;
  // Four bytes for PRU cursor
  void* _pru_cursor;
  // The rest is data
  void* _data;
  
private:
  uint32_t _max_bytes;
  
  // Internal cursor controls knowing where we are writing
  // or reading data coming from encoding or going towards
  // decoding.
  uint32_t _internal_cursor;

  uint32_t _mem_fd;
};

#endif // BYTEQUEUE_HPP
