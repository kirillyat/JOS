#ifndef JOS_KERN_KDEBUG_H
#define JOS_KERN_KDEBUG_H

#include <inc/types.h>

// Debug information about a particular instruction pointer
struct Ripdebuginfo {
  char rip_file[256]; // Source code filename for RIP
  int rip_line;       // Source code linenumber for RIP

  char rip_fn_name[256]; // Name of function containing RIP
                         //  - Note: not null terminated!
  int rip_fn_namelen;    // Length of function name
  uintptr_t rip_fn_addr; // Address of start of function
  int rip_fn_narg;       // Number of function arguments
};

int debuginfo_rip(uintptr_t eip, struct Ripdebuginfo *info);
uintptr_t find_function(const char *const fname);

#endif
