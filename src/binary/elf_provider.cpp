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
#include <iostream>

using namespace std;

bool elf_provider::symbols(entity_callbacks const& callbacks) const {
  size_t shstrndx;
  if(elf_getshstrndx(elf->get_elf(), &shstrndx) != 0) throw new string(":-(");

  Elf_Scn *scn = NULL;
  while((scn = elf_nextscn(elf->get_elf(), scn)) != NULL) {
    GElf_Shdr shdr;
    if(gelf_getshdr(scn, &shdr) != &shdr) throw new string("gelf_getshdr() :-(");

    Elf_Data *d = elf_getdata(scn, NULL);

    char *section_name = elf_strptr(elf->get_elf(), shstrndx, shdr.sh_name);
    if(!section_name) throw new string("gelf_strptr() :-(");

//    cout << "section " << string(section_name) << " at " << shdr.sh_offset << " with address " << shdr.sh_addr << " and size " << shdr.sh_size << endl;
    if(callbacks.section(shdr, string(section_name))) return true;

//    if(!strcmp(section_name, ".symtab")) {
    if(true || shdr.sh_type == SHT_SYMTAB) {
      cout << "entry size: " << shdr.sh_entsize << endl;

      if(!shdr.sh_entsize) continue;// throw new string("Empty entries in symbol table?!");
      for(Elf64_Xword i = 0; i < shdr.sh_size / shdr.sh_entsize; ++i) {
//        cout << "NEXT ENTRY " << i << " with address " << (shdr.sh_addr + i*shdr.sh_entsize) << endl;
        if(callbacks.section_entry(i, (shdr.sh_addr + i*shdr.sh_entsize))) return true;

        auto symbol = [&](entity_callbacks::symbol_callback_t symbol_cb) {
          GElf_Sym sym;
          if(gelf_getsym(d, i, &sym) != &sym) throw new string("getsym() failed\n");
          const char *sym_name = elf_strptr(elf->get_elf(), shdr.sh_link, sym.st_name);
          cout << "sym_name: " << sym_name << ", and: " << sym.st_info << endl;
          char st_type = ELF64_ST_TYPE(sym.st_info);
          if(symbol_cb(sym, st_type, string(sym_name))) return true;
          return false;
        };

        if(shdr.sh_type == SHT_SYMTAB) {
          if(symbol(callbacks.symbol))
            return true;
        }

        if(shdr.sh_type == SHT_DYNSYM) {
//          GElf_Dyn dyn;
//          if(gelf_getdyn(d, i, &dyn) == &dyn) {
//            cout << dyn.d_tag << endl;
//          }

          if(symbol(callbacks.dyn_symbol))
            return true;
        }

        if(shdr.sh_type == SHT_RELA) {
//          GElf_Rela rela;
//          if(gelf_getrela(d, i, &rela) == &rela) {
//            cout << "RELA offset: " << rela.r_offset << ", addend: " << rela.r_addend << ", info: " << ELF64_R_SYM(rela.r_info) << endl;
//          }
        }

        if(shdr.sh_type == SHT_REL) {
//          GElf_Rel rel;
//          if(gelf_getrel(d, i, &rel) == &rel) {
//            cout << "REL" << endl;
//            cout << "offset: " << rel.r_offset << "info: " << ELF64_R_SYM(rela.r_info) << endl;
//          }
        }

//        GElf_Phdr vv;
//        if(gelf_getphdr(elf->get_elf(), i, &vv) == &vv) {
////          const char *sym_name = elf_strptr(elf->get_elf(), shdr.sh_link, vv.p_filesz);
//          cout << "PHDR offset: " << vv.p_offset << ", paddr: " << vv.p_paddr << ", vaddr: " << vv.p_vaddr << endl;
//        }
      }
    }
  }

  return false;
}

void elf_provider::init() {
  if(elf_kind(elf->get_elf()) != ELF_K_ELF) throw new string("Wrong elf kind :-(");

  //Collect symbols
  /*
   * Todo: Uncopy
   */
  vector<slice> slices;

//  auto add_to_slices = [&](GElf_Sym sym, string name) {
//    if(sym.st_value != 0 && sym.st_size != 0) slices.push_back(
//        slice((void*) sym.st_value, sym.st_size, name));
//    return false;
//  };
  auto add_to_slices = [&](GElf_Shdr shdr, string name) {
    if(shdr.sh_size != 0) slices.push_back(
        slice((void*) shdr.sh_addr, shdr.sh_size, name));
    return false;
  };
  entity_callbacks callbacks;
  callbacks.section = add_to_slices;

  symbols(callbacks);

  elf_mem = new sliced_memory(slices);
}

elf_provider::elf_provider(const char *file) :
    file_provider(file) {
  _file_fd *fd = new _file_fd(open(file, O_RDONLY));
  this->fd = fd;

  if(elf_version(EV_CURRENT) == EV_NONE) {
    throw new string("EV_NONE :-(");
  }

  elf = new _Elf(elf_begin(fd->get_fd(), ELF_C_READ, NULL));
  init();
}

elf_provider::elf_provider(char *buffer, size_t size) :
  file_provider(buffer, size) {
  _mem_fd *fd = new _mem_fd(buffer);
  this->fd = fd;

  if(elf_version(EV_CURRENT) == EV_NONE) {
    throw new string("EV_NONE :-(");
  }

  elf = new _Elf(elf_memory(fd->get_memory(), size));
  init();
}

elf_provider::~elf_provider() {
  delete elf;
  delete fd;
  delete elf_mem;
}

vector<std::tuple<string, binary_provider::entry_t>>  elf_provider::functions() const {
  vector<std::tuple<string, binary_provider::entry_t>> result;

  auto next_symbol = [&](GElf_Sym next_sym, char st_type, string next_name) {
    if(st_type == STT_FUNC) {
      Elf_Scn *scn = elf_getscn(elf->get_elf(), next_sym.st_shndx);
      if(scn == NULL) throw new string("elf_getscn() :/");
      GElf_Shdr shdr;
      if(gelf_getshdr(scn, &shdr) != &shdr) throw new string("gelf_getshdr() :-(");

      entry_t entry;
      entry.address = next_sym.st_value;
      entry.offset = shdr.sh_offset + (entry.address - shdr.sh_addr);;
      entry.size = next_sym.st_size;

      result.push_back(make_tuple(next_name, entry));
    }
    return false;
  };

  entity_callbacks callbacks;
  callbacks.symbol = next_symbol;

  symbols(callbacks);

  return result;
}

tuple<bool, binary_provider::entry_t> elf_provider::symbol(string symbol_name) const {
  entry_t entry;

  size_t shstrndx;
  if(elf_getshstrndx(elf->get_elf(), &shstrndx) != 0) throw new string(":-(");

  GElf_Sym sym;

  auto next_symbol = [&](GElf_Sym next_sym, char st_type, string next_name) {
    if(next_name == symbol_name) {
      sym = next_sym;
      return true;
    }
    return false;
  };
  entity_callbacks callbacks;
  callbacks.symbol = next_symbol;

  if(!symbols(callbacks))
    return binary_provider::symbol(symbol_name);

  Elf_Scn *scn = elf_getscn(elf->get_elf(), sym.st_shndx);
  if(scn == NULL) throw new string("elf_getscn() :/");
  GElf_Shdr shdr;
  if(gelf_getshdr(scn, &shdr) != &shdr) throw new string("gelf_getshdr() :-(");

  entry.size = sym.st_size;
  entry.address = sym.st_value;
  entry.offset = shdr.sh_offset + (entry.address - shdr.sh_addr);

  return make_tuple(true, entry);
}

tuple<bool, binary_provider::entry_t> elf_provider::section(std::string section_name) const {
  entry_t range;

  size_t shstrndx;
  if(elf_getshstrndx(elf->get_elf(), &shstrndx) != 0) throw new string(":-(");

  Elf_Scn *scn = NULL;

  bool found_section = false;

  while((scn = elf_nextscn(elf->get_elf(), scn)) != NULL) {
    GElf_Shdr shdr;
    if(gelf_getshdr(scn, &shdr) != &shdr) throw new string("gelf_getshdr() :-(");

    char *current_section_name = elf_strptr(elf->get_elf(), shstrndx, shdr.sh_name);
    if(!current_section_name) throw new string("gelf_strptr() :-(");

    if(!strcmp(current_section_name, section_name.c_str())) {
      range.address = shdr.sh_addr;
      range.offset = shdr.sh_offset;
      range.size = shdr.sh_size;
      found_section = true;
    }
  }

  return make_tuple(found_section, range);
}

binary_provider::entry_t elf_provider::bin_range() {
  bool success;
  entry_t entry;
  tie(success, entry) = section(".text");
  if(!success)
    throw string("binary_provider::entry_t elf_provider::bin_range()");
  return entry;
}

std::tuple<binary_provider::data_t, size_t, bool> elf_provider::deref(void *address, size_t bytes) const {
  bool success;
  slice s;
  tie(success, s) = elf_mem->deref(address);

  if(!success) return make_tuple(data_t(), 0, false);

  binary_provider::entry_t e;
  tie(success, e) = section(s.symbol);

  size_t offset = e.offset + ((size_t) address - e.address);

  data_t d = get_data();
//  if(offset + size >= d.size) throw string("elf_provider::deref(void*,size_t,uint8_t*): offset + size >= d.size");
  return make_tuple(d, offset, offset + bytes < d.size);
}

bool elf_provider::deref(void *address, size_t bytes, uint8_t *buffer) const {
  bool success;
  size_t offset;
  data_t d;
  tie(d, offset, success) = deref(address, bytes);
  if(success) memcpy(buffer, get_data().data + offset, bytes);
  return success;
}

std::tuple<bool, uint64_t> elf_provider::deref(void *address) const {
  uint64_t value;
  bool success = deref(address, sizeof(value), (uint8_t*)&value);
  return make_tuple(success, value);
}

bool elf_provider::check(void *address, size_t bytes) const {
  bool success;
  tie(ignore, ignore, success) = deref(address, bytes);
  return success;
}

std::tuple<bool, slice> elf_provider::deref_slice(void *address) const {
  return elf_mem->deref(address);
}
