#include <inc/string.h>
#include <inc/memlayout.h>
#include <inc/assert.h>
#include <inc/dwarf.h>
#include <inc/elf.h>
#include <inc/x86.h>

#include <kern/kdebug.h>
#include <kern/pmap.h>
#include <kern/env.h>
#include <inc/uefi.h>

void
load_kernel_dwarf_info(struct Dwarf_Addrs *addrs) {
  addrs->aranges_begin  = (unsigned char *)(uefi_lp->DebugArangesStart);
  addrs->aranges_end    = (unsigned char *)(uefi_lp->DebugArangesEnd);
  addrs->abbrev_begin   = (unsigned char *)(uefi_lp->DebugAbbrevStart);
  addrs->abbrev_end     = (unsigned char *)(uefi_lp->DebugAbbrevEnd);
  addrs->info_begin     = (unsigned char *)(uefi_lp->DebugInfoStart);
  addrs->info_end       = (unsigned char *)(uefi_lp->DebugInfoEnd);
  addrs->line_begin     = (unsigned char *)(uefi_lp->DebugLineStart);
  addrs->line_end       = (unsigned char *)(uefi_lp->DebugLineEnd);
  addrs->str_begin      = (unsigned char *)(uefi_lp->DebugStrStart);
  addrs->str_end        = (unsigned char *)(uefi_lp->DebugStrEnd);
  addrs->pubnames_begin = (unsigned char *)(uefi_lp->DebugPubnamesStart);
  addrs->pubnames_end   = (unsigned char *)(uefi_lp->DebugPubnamesEnd);
  addrs->pubtypes_begin = (unsigned char *)(uefi_lp->DebugPubtypesStart);
  addrs->pubtypes_end   = (unsigned char *)(uefi_lp->DebugPubtypesEnd);
}

void
load_user_dwarf_info(struct Dwarf_Addrs *addrs) {
  assert(curenv);

  uint8_t *binary = curenv->binary;
  struct Elf *elf = (struct Elf *) binary;
  struct Secthdr *sh = (struct Secthdr *)(binary + elf->e_shoff);
  const char *shstr = (char *) binary + sh[elf->e_shstrndx].sh_offset;

  struct {
    const uint8_t **begin;
    const uint8_t **end;
    const char *name;
  } p[] = {
    {&addrs->aranges_begin,  &addrs->aranges_end,  ".debug_aranges"},
    {&addrs->abbrev_begin,   &addrs->abbrev_end,   ".debug_abbrev"},
    {&addrs->info_begin,     &addrs->info_end,     ".debug_info"},
    {&addrs->line_begin,     &addrs->line_end,     ".debug_line"},
    {&addrs->str_begin,      &addrs->str_end,      ".debug_str"},
    {&addrs->pubnames_begin, &addrs->pubnames_end, ".debug_pubnames"},
    {&addrs->pubtypes_begin, &addrs->pubtypes_end, ".debug_puptypes"}
  };

  memset(addrs, 0, sizeof(*addrs));
  
  for (int i = 0; i < elf->e_shnum; i++) {
    for (int k = 0; k < sizeof(p)/sizeof(*p); k++) {
      if (!strcmp(&shstr[sh[i].sh_name], p[k].name)) {
        *p[k].begin = binary + sh[i].sh_offset;
        *p[k].end   = binary + sh[i].sh_offset + sh[i].sh_size;
      }
    }
  }
}

// debuginfo_rip(addr, info)
//
//	Fill in the 'info' structure with information about the specified
//	instruction address, 'addr'.  Returns 0 if information was found, and
//	negative if not.  But even if it returns negative it has stored some
//	information into '*info'.
//
int
debuginfo_rip(uintptr_t addr, struct Ripdebuginfo *info) {
  int code = 0;
  // Initialize *info
  strcpy(info->rip_file, "<unknown>");
  info->rip_line = 0;
  strcpy(info->rip_fn_name, "<unknown>");
  info->rip_fn_namelen = 9;
  info->rip_fn_addr    = addr;
  info->rip_fn_narg    = 0;

  if (!addr) {
    return 0;
  }
  
  // Temporarily load kernel cr3 and return back once done.
  // Make sure that you fully understand why it is necessary.
  // LAB 8: Your code here.

  struct Dwarf_Addrs addrs;
  uint64_t cr3 = rcr3();
  if (cr3 != kern_cr3) {
    lcr3(kern_cr3);
  }
  if (addr < ULIM) {
    load_user_dwarf_info(&addrs);
  } else { 
    load_kernel_dwarf_info(&addrs);
  }
  enum {
    BUFSIZE = 20,
  };
  Dwarf_Off offset = 0, line_offset = 0;
  code = info_by_address(&addrs, addr, &offset);
  if (code < 0) {
    if (cr3 != kern_cr3) {
      lcr3(cr3);
    }
    return code;
  }
  char *tmp_buf;
  void *buf;
  buf  = &tmp_buf;
  code = file_name_by_info(&addrs, offset, buf, sizeof(char *), &line_offset);
  strncpy(info->rip_file, tmp_buf, 256);
  if (code < 0) {
    if (cr3 != kern_cr3) {
      lcr3(cr3);
    }
    return code;
  }
  // Find line number corresponding to given address.
  // Hint: note that we need the address of `call` instruction, but rip holds
  // address of the next instruction, so we should substract 5 from it.
  // Hint: use line_for_address from kern/dwarf_lines.c
  // LAB 2:
  addr -= 5;
  buf  = &info->rip_line;
  code = line_for_address(&addrs, addr, line_offset, buf);
  if (code < 0) {
    if (cr3 != kern_cr3) {
      lcr3(cr3);
    }
    return code;   
  }

  buf  = &tmp_buf;
  code = function_by_info(&addrs, addr, offset, buf, sizeof(char *), &info->rip_fn_addr);
  strncpy(info->rip_fn_name,  tmp_buf, 256);
  info->rip_fn_namelen = strnlen(info->rip_fn_name,  256);
  if (code < 0) {
    if (cr3 != kern_cr3) {
      lcr3(cr3);
    }
    return code;
  }
  if (cr3 != kern_cr3) {
    lcr3(cr3);
  }
  return 0;
}

uintptr_t
find_function(const char *const fname) {
  // There are two functions for function name lookup.
  // address_by_fname, which looks for function name in section .debug_pubnames
  // and naive_address_by_fname which performs full traversal of DIE tree.
  // LAB 3: Your code here
#ifdef CONFIG_KSPACE
  struct {
    const char *name;
    uintptr_t addr;
  } scentry[] = {
      { "sys_yield", (uintptr_t)sys_yield },
      { "sys_exit", (uintptr_t)sys_exit },
  };

  for (size_t i = 0; i < sizeof(scentry) / sizeof(scentry[0]); i++) {
    if (!strcmp(scentry[i].name, fname)) {
      return scentry[i].addr;
    }
  }
#endif
  struct Dwarf_Addrs addrs;
  load_kernel_dwarf_info(&addrs);
  uintptr_t offset = 0;

  if (!address_by_fname(&addrs, fname, &offset) && offset) {
    return offset;
  }

  if (!naive_address_by_fname(&addrs, fname, &offset)) {
    return offset;
  }

  return 0;
}
