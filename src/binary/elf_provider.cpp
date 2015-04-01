/*
 * elf_provider.cpp
 *
 *  Created on: May 8, 2014
 *      Author: Julian Kranz
 */

#include <bjutil/binary/elf_provider.h>
#include <bjutil/sliced_memory.h>
#include <bjutil/binary/file_provider.h>
#include <stdlib.h>
#include <stdio.h>
#include <gelf.h>
#include <err.h>
#include <fcntl.h>
#include <stdint.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include <tuple>
#include <vector>

using std::make_tuple;
using std::string;
using std::tie;
using std::vector;

void elf_provider::init() {
  if(elf_version(EV_CURRENT) == EV_NONE) {
    throw new string("EV_NONE :-(");
  }

  if(elf_kind(elf->get_elf()) != ELF_K_ELF) throw new string("Wrong elf kind :-(");

  //Collect symbols
  /*
   * Todo: Uncopy
   */
  vector<slice> slices;

  auto add_to_slices = [&](GElf_Sym sym, string name) {
    if(sym.st_value != 0 && sym.st_size != 0) slices.push_back(
        slice((void*) sym.st_value, sym.st_size, name));
    return false;
  };

  symbols(add_to_slices);

  elf_mem = new sliced_memory(slices);
}

elf_provider::elf_provider(const char *file) :
    file_provider(file) {
  _file_fd *fd = new _file_fd(open(file, O_RDONLY));
  this->fd = fd;
  elf = new _Elf(elf_begin(fd->get_fd(), ELF_C_READ, NULL));
  init();
}

elf_provider::elf_provider(char *buffer, size_t size) :
  file_provider(buffer, size) {
  _mem_fd *fd = new _mem_fd(buffer);
  this->fd = fd;
  elf = new _Elf(elf_memory(fd->get_memory(), size));
  init();
}

elf_provider::~elf_provider() {
  delete elf;
  delete fd;
  delete elf_mem;
}

tuple<bool, binary_provider::entry_t> elf_provider::entry(string symbol_name) {
  entry_t entry;

  size_t shstrndx;
  if(elf_getshstrndx(elf->get_elf(), &shstrndx) != 0) throw new string(":-(");

  GElf_Sym sym;

  auto next_symbol = [&](GElf_Sym next_sym, string next_name) {
    if(next_name == symbol_name) {
      sym = next_sym;
      return true;
    }
    return false;
  };

  if(!symbols(next_symbol))
    return binary_provider::entry(symbol_name);

  Elf_Scn *scn = elf_getscn(elf->get_elf(), sym.st_shndx);
  if(scn == NULL) throw new string("elf_getscn() :/");
  GElf_Shdr shdr;
  if(gelf_getshdr(scn, &shdr) != &shdr) throw new string("gelf_getshdr() :-(");

  entry.size = sym.st_size;
  entry.address = sym.st_value;
  entry.offset = shdr.sh_offset + (entry.address - shdr.sh_addr);

  return make_tuple(true, entry);
}

binary_provider::entry_t elf_provider::bin_range() {
  return section(".text");
}

binary_provider::entry_t elf_provider::section(std::string name) {
  entry_t range;

  size_t shstrndx;
  if(elf_getshstrndx(elf->get_elf(), &shstrndx) != 0) throw new string(":-(");

  Elf_Scn *scn = NULL;

  bool found_section = false;

  while((scn = elf_nextscn(elf->get_elf(), scn)) != NULL) {
    GElf_Shdr shdr;
    if(gelf_getshdr(scn, &shdr) != &shdr) throw new string("gelf_getshdr() :-(");

    char *section_name = elf_strptr(elf->get_elf(), shstrndx, shdr.sh_name);
    if(!section_name) throw new string("gelf_strptr() :-(");

    if(!strcmp(section_name, name.c_str())) {
      range.address = shdr.sh_addr;
      range.offset = shdr.sh_offset;
      range.size = shdr.sh_size;
      found_section = true;
    }
  }

  if(!found_section) throw new string("Invalid section");

  return range;
}

std::tuple<bool, uint64_t> elf_provider::deref(void *address) {
  bool success;
  slice s;
  tie(success, s) = elf_mem->deref(address);

  if(!success) return make_tuple(false, 0);

  binary_provider::entry_t e;
  tie(success, e) = entry(s.symbol);

  size_t offset = e.offset + ((size_t) address - e.address);

  data_t d = get_data();
  if(offset >= d.size) throw string("offset >= d.size");

  uint64_t *value = (uint64_t*) (get_data().data + offset);

  return make_tuple(true, *value);
}

bool elf_provider::symbols(std::function<bool(GElf_Sym, string)> callback) {
  size_t shstrndx;
  if(elf_getshstrndx(elf->get_elf(), &shstrndx) != 0) throw new string(":-(");

  Elf_Scn *scn = NULL;
  while((scn = elf_nextscn(elf->get_elf(), scn)) != NULL) {
    GElf_Shdr shdr;
    if(gelf_getshdr(scn, &shdr) != &shdr) throw new string("gelf_getshdr() :-(");

    Elf_Data *d = elf_getdata(scn, NULL);

    char *section_name = elf_strptr(elf->get_elf(), shstrndx, shdr.sh_name);
    if(!section_name) throw new string("gelf_strptr() :-(");

    if(!strcmp(section_name, ".symtab")) {
      if(!shdr.sh_entsize) throw new string("Empty entries in symbol table?!");
      for(uint64_t i = 0; i < shdr.sh_size / shdr.sh_entsize; ++i) {
        GElf_Sym sym;
        if(gelf_getsym(d, i, &sym) != &sym) throw new string("getsym() failed\n");
        const char *sym_name = elf_strptr(elf->get_elf(), shdr.sh_link, sym.st_name);
        if(callback(sym, string(sym_name))) return true;
      }
    }
  }

  return false;
}
