#include <string.h>
#include <stdio.h>
#include "pesudo_mm.h"
#include "log.h"

void trimLeading(char * str)
{
    int index, i, j;

    index = 0;

    /* Find last index of whitespace character */
    while(str[index] == ' ' || str[index] == '\t' || str[index] == '\n')
    {
        index++;
    }


    if(index != 0)
    {
        /* Shit all trailing characters to its left */
        i = 0;
        while(str[i + index] != '\0')
        {
            str[i] = str[i + index];
            i++;
        }
        str[i] = '\0'; // Make sure that string is NULL terminated
    }
}


void unserialize_vm_area_item(const char *content, struct vm_area_struct *data)
{
    int rv;
    unsigned long start, end, pgoff;
    char flags[4], dev_no[32], node_no[32], description[1024];
    dev_no[31] = '\0';
    node_no[31] = '\0';
    description[1023] = '\0';
    description[0] = '\0';
    rv = sscanf(content, "%08lx-%08lx %4s %08lx %31s %31s %[^\n]",
			    &start, &end, flags, &pgoff, dev_no, node_no, description);
    if(rv < 6) {
        LOGD("read_vm_area_item: format error: %d", rv);
    }
    trimLeading(description);
    data->vm_start = start;
    data->vm_end = end;
    data->vm_flags = ((flags[0] != '-' ? VM_READ : 0) | (flags[1] != '-' ? VM_WRITE : 0) | (flags[2] != '-' ? VM_MAYEXEC : 0) | (flags[3] == 's' ? VM_SHARED : 0));
    data->vm_pgoff = pgoff;
    strncpy(data->vm_dev_no, dev_no, 31);
    strncpy(data->vm_node_no, node_no, 31);
    strncpy(data->vm_description, description, 255);
    data->vm_description[255] = '\0';
}

void serialize_vm_area_item(const struct vm_area_struct* data, char *content) {
    char flags[5];
    flags[0] = (data->vm_flags & VM_READ) ? 'r' : '-';
    flags[1] = (data->vm_flags & VM_WRITE) ? 'w' : '-';
    flags[2] = (data->vm_flags & VM_MAYEXEC) ? 'x' : '-';
    flags[3] = (data->vm_flags & VM_SHARED) ? 's' : 'p';
    flags[4] = (char)0;
    
    sprintf(content, "%08lx-%08lx %4s %08lx %s %s %s", data->vm_start, data->vm_end, flags, data->vm_pgoff, data->vm_dev_no, data->vm_node_no, data->vm_description);
}


int get_module_map(pid_t pid, const char *module_name, struct vm_area_struct* vma, int *nvma)
{
    FILE *fp = NULL;
    char file_name[64] = {0};
    char map_file_line[1024] = {0};
    int index = 0, count = 0;
    struct vm_area_struct tmp_vma;

    // 读取"/proc/pid/maps"可以获得该进程加载的模块
    if (pid < 0)
    {
        snprintf(file_name, sizeof(file_name), "/proc/self/maps");
    }
    else
    {
        snprintf(file_name, sizeof(file_name), "/proc/%d/maps", pid);
    }

    fp = fopen(file_name, "r");
    if (fp == NULL)
    {
        LOGD("get_module_map: fopen %s error", file_name);
        return -1;
    }

    while (fgets(map_file_line, sizeof(map_file_line), fp))
    {
        // LOGD("++++ %s", map_file_line);
        unserialize_vm_area_item(map_file_line, &tmp_vma);
        // LOGD("%d [%s]", index, tmp_vma.vm_description);
        if(strcmp(tmp_vma.vm_description, module_name) == 0)
        {
            memcpy(&vma[count++], &tmp_vma, sizeof(tmp_vma));
            if(count >= *nvma)
            {
                break;
            }
        } 
        index++;
        // serialize_vm_area_item(&tmp_vma, map_file_line);
        // LOGD("---- %s", map_file_line);
    }
    // LOGD("get_module_base: fgets %s error", map_file_line);
    fclose(fp);

    *nvma = count;
    return 0;
}

void *get_module_base(pid_t pid, const char *module_name)
{
    int i;
    struct vm_area_struct vma[16];
    int nvma = 16;
    unsigned long base = (unsigned long)-1;

    get_module_map(pid, module_name, vma, &nvma);
    for(i = 0; i < nvma; i++) {
        // serialize_vm_area_item(&vma[i], content);
        // LOGD("%s", content);
        if(base > vma[i].vm_start) {
            base = vma[i].vm_start;
        }
    }
    if(base == (unsigned long)-1) {
        LOGD("get_module_base: not find pid: %d path: %s", (int)pid, module_name);
    }
    return (void *)base;
}