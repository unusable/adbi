
#include <unistd.h>

/*
 * vm_flags..
 */
#define VM_READ         0x0001  /* currently active flags */
#define VM_WRITE        0x0002
#define VM_EXEC         0x0004
#define VM_SHARED       0x0008

#define VM_MAYREAD      0x0010  /* limits for mprotect() etc */
#define VM_MAYWRITE     0x0020
#define VM_MAYEXEC      0x0040
#define VM_MAYSHARE     0x0080

#define VM_GROWSDOWN    0x0100  /* general info on the segment */
#define VM_GROWSUP      0x0200
#define VM_SHM          0x0400  /* shared memory area, don't swap out */
#define VM_DENYWRITE    0x0800  /* ETXTBSY on write attempts.. */

#define VM_EXECUTABLE   0x1000
#define VM_LOCKED       0x2000


struct vm_area_struct {
    unsigned long vm_start;
    unsigned long vm_end;
    unsigned long vm_flags;
    unsigned long vm_pgoff;

    char vm_dev_no[32];
    char vm_node_no[32];
    char vm_description[256];
};


int get_module_map(pid_t pid, const char *module_nam, struct vm_area_struct* vma, int *nvmae);
void *get_module_base(pid_t pid, const char *module_name);
void serialize_vm_area_item(const struct vm_area_struct* data, char *content);