/*
 * elf_provider.h
 *
 *  Created on: May 8, 2014
 *      Author: Julian Kranz
 */

#pragma once

#include <bjutil/binary/elf_provider.h>
#include <bjutil/binary/file_provider.h>
#include <bjutil/sliced_memory.h>
#include <gelf.h>
#include <libelf.h>
#include <unistd.h>
#include <string>
#include <tuple>
#include <functional>

class elf_provider : public file_provider {
private:
  sliced_memory *elf_mem;

  struct _fd {
    int fd;

    int get_fd() {
      return fd;
    }

    _fd(int fd) {
      this->fd = fd;
      if(!fd) throw new std::string("Unable to open file");
    }
    ~_fd() {
      close(fd);
    }
  };

  _fd *fd = NULL;

  struct _Elf {
    Elf *e;

    Elf *get_elf() {
      return e;
    }

    _Elf(Elf *e) {
      this->e = e;
      if(!e) throw new std::string("Unable to open elf");
    }
    ~_Elf() {
      elf_end(e);
    }
  };

  _Elf *elf = NULL;

  bool symbols(std::function<bool(GElf_Sym, string)> callback);
public:
  elf_provider(char const *file);
  ~elf_provider();

  std::tuple<bool, entry_t> entry(std::string symbol);
  entry_t bin_range();
  entry_t section(std::string name);

  std::tuple<bool, size_t> deref(void *address);
};
