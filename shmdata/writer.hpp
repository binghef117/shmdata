/*
 * Copyright (C) 2015 Nicolas Bouillot (http://www.nicolasbouillot.net)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 */

#ifndef _SHMDATA_WRITER_H_
#define _SHMDATA_WRITER_H_

#include <string>
#include <memory>
#include "shmdata/unix-socket-server.hpp"
#include "shmdata/unix-socket-protocol.hpp"
#include "shmdata/sysv-shm.hpp"
#include "shmdata/sysv-sem.hpp"
#include "./safe-bool-idiom.hpp"

namespace shmdata{
class OneWriteAccess;
class Writer: public SafeBoolIdiom {
 public:
  Writer(const std::string &path, size_t memsize, const std::string &data_descr);
  ~Writer() = default;
  Writer() = delete;
  Writer(const Writer &) = delete;
  Writer& operator=(const Writer&) = delete;
  Writer& operator=(Writer&&) = default;

  bool copy_to_shm(void *data, size_t size);
  std::unique_ptr<OneWriteAccess> get_one_write_access();
 private:
  std::string path_; 
  UnixSocketProtocol::onConnectDataMaker connect_data_;
  UnixSocketProtocol::ServerSide proto_;
  UnixSocketServer srv_;
  sysVShm shm_;
  sysVSem sem_;
  bool is_valid_{true};
  bool is_valid() const final{return is_valid_;}
  void on_client_notifyed();
};

// see check-shmdata
class OneWriteAccess {
  friend Writer;
 public:
  void *get_mem() {return mem_;};
 private:
  OneWriteAccess(sysVSem *sem, void *mem, UnixSocketServer *srv) :
      wlock_(sem),
      mem_(mem){
  }
  OneWriteAccess& operator=(OneWriteAccess&&) = default;
  WriteLock wlock_;
  void *mem_;
};

}  // namespace shmdata
#endif
