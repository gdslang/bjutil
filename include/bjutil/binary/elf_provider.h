/*
 * elf_provider.h
 *
 *  Created on: May 8, 2014
 *      Author: Julian Kranz
 */

#pragma once

#include <bjutil/binary/file_provider.h>
#include <bjutil/sliced_memory.h>
#include <gelf.h>
#include <libelf.h>
#include <unistd.h>
#include <string>
#include <tuple>
#include <functional>

class elf_provider: public file_provider {
private:
  sliced_memory *elf_mem;

  struct _fd {
    virtual ~_fd() {
    }
  };

  struct _file_fd: public _fd {
    int fd;

    int get_fd() {
      return fd;
    }

    _file_fd(int fd) {
      this->fd = fd;
      if(!fd) throw new std::string("Unable to open file");
    }
    ~_file_fd() {
      close(fd);
    }
  };

  struct _mem_fd: public _fd {
    char *memory;

    char *get_memory() {
      return memory;
    }

    _mem_fd(char *memory) {
      this->memory = memory;

    }
    ~_mem_fd() {
      free(memory);
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

  struct entity_callbacks {
    typedef std::function<bool(GElf_Sym, char st_type, string)> symbol_callback_t;

    std::function<bool(GElf_Shdr, string)> section;
    symbol_callback_t symbol;
//    std::function<bool(GElf_Rela, Elf64_Xword dynsym_index)> rela;
    std::function<bool(Elf64_Xword index, Elf64_Xword address)> section_entry;
    symbol_callback_t dyn_symbol;

    entity_callbacks() {
      auto _default = [](auto... args) {
        return false;
      };
      this->section = _default;
      this->symbol = _default;
      this->section_entry = _default;
      this->dyn_symbol = _default;
    }
  };

  bool traverse_sections(entity_callbacks const& callbacks) const;
  void init();
public:
  elf_provider(char const *file);
  elf_provider(char *buffer, size_t size);
  ~elf_provider();

  std::vector<std::tuple<string, entry_t>> functions() const;
  std::vector<std::tuple<string, entry_t>> functions_dynamic() const;
  std::tuple<bool, entry_t> symbol(std::string symbol_name) const;
  std::tuple<bool, entry_t> section(std::string section_name) const;
  entry_t bin_range();

  std::tuple<data_t, size_t, bool> deref(void *address, size_t bytes) const;
  bool deref(void *address, size_t bytes, uint8_t *buffer) const;
  std::tuple<bool, size_t> deref(void *address) const;
  bool check(void *address, size_t bytes) const;
  std::tuple<bool, slice> deref_slice(void *address) const;
};
