// SPDX-License-Identifier: zlib-acknowledgement
#pragma once

#include <stdarg.h>

#include <signal.h>
#include <sys/types.h>
#include <sys/prctl.h>
#include <sys/sysinfo.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/stat.h> 
#include <unistd.h>
#include <dirent.h> 
#include <fcntl.h> 

// NOTE(Ryan): Allow for simple runtime debugger attachment
GLOBAL b32 global_debugger_present;

#if defined(DEBUG_BUILD)
  #define BP() \
  do \
  { \
    if (global_debugger_present) \
    { \
      raise(SIGTRAP); \
    } \
  } while (0)
#else
  #define BP()
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define FATAL_ERROR(fmt, ...) \
  __fatal_error(SOURCE_LOC, fmt, ##__VA_ARGS__)


#include <execinfo.h>
#include <link.h>

INTERNAL String8 linux_read_entire_cmd(MemArena *arena, char *args[], b32 echo);
INTERNAL void echo_cmd(char **argv);
INTERNAL String8 str8_shell_escape(MemArena *arena, String8 str);
INTERNAL b32 str8_is_shell_safe(String8 str);
INTERNAL b32 char_is_shell_safe(char ch);

#include <cxxabi.h>

#define NUM_ADDRESSES 64
INTERNAL void
linux_print_stacktrace(void)
{
// https://stackoverflow.com/a/63855266/9960107
  void *callstack_addr[NUM_ADDRESSES] = ZERO_STRUCT;
  u32 num_backtrace_frames = backtrace(callstack_addr, NUM_ADDRESSES);

  for (u32 i = 0; i < num_backtrace_frames; i += 1) 
  {
    char location[256] = ZERO_STRUCT;
    Dl_info info = ZERO_STRUCT; 
    if (dladdr(callstack_addr[i], &info))
    {
      link_map *map = NULL;
      Dl_info extra_info = ZERO_STRUCT;
      dladdr1(callstack_addr[i], &extra_info, (void **)&map, RTLD_DL_LINKMAP);
      memory_index vma = (memory_index)callstack_addr[i] - map->l_addr;
      // x86 PC one after current instruction
      vma -= 1;
      
      char cmd[64] = ZERO_STRUCT;
      snprintf(cmd, sizeof(cmd), "%zx", vma);
      char *args[] = {
        "addr2line",
        "-e",
        //(char *)info.dli_fname,
        "ui",
        "-f",
        "-s",
        cmd, 
        NULL,
      };

      MemArenaTemp temp = mem_arena_temp_begin(NULL, 0);

      // TODO(Ryan): Batch addresses and pass as list to addr2line
      String8 output = linux_read_entire_cmd(temp.arena, args, false);
      memory_index newline = str8_find_substring(output, str8_lit("\n"), 0, 0);
      String8 function_name = str8_prefix(output, newline);
      char cstr[64] = ZERO_STRUCT;
      str8_to_cstr(function_name, cstr, sizeof(cstr));

      int status = 0;
      char *demangled = abi::__cxa_demangle(cstr, NULL, NULL, &status);

      if (status == 0) printf("%s\n", demangled);
      // will fail if a C style function name
      else printf("%.*s\n", str8_varg(function_name));

      // NOTE(Ryan): No need to print libc
      if (str8_match(function_name, str8_lit("main"), 0)) break;

      mem_arena_temp_end(temp);

#if 0
      // IMPORTANT(Ryan): Require addr2line --version >= 2.38 (.debug_info bug fix)
      echo_cmd(args);
      char other[256] = ZERO_STRUCT;
      //snprintf(other, sizeof(other), "addr2line -e %s -Ci %zx", info.dli_fname, vma);
      snprintf(other, sizeof(other), "addr2line -e %s -f -s %zx", info.dli_fname, vma);
      system(other);
#endif
    }
  }

  /*
   * 1. Assume PIC so address subtraction required. Could check by inspecting elf file
   * 2. Load vaddr base with /proc/<PID>/maps 
   * 3. dl_iterate_phdr() to get .dlpi_addr field for base for shared objects
   * 4. Pass subtracted addresses to addr2line
   */
}

INTERNAL void
__fatal_error(SourceLoc source_loc, const char *fmt, ...)
{ 

  va_list args;
  va_start(args, fmt);

  printf(ASC_RED); fflush(stdout);
  fprintf(stderr, "%s:%ld:0: FATAL_ERROR: ", source_loc.file_name, source_loc.line_num);
  vfprintf(stderr, fmt, args);
  printf(ASC_CLEAR); fflush(stdout);

  va_end(args);

  linux_print_stacktrace();

  BP();

  kill(getpid(), SIGKILL); // pthread_exit()?
}

#if defined(DEBUG_BUILD)
#define DBG(fmt, ...) \
  do \
  { \
    printf(ASC_GREEN); fflush(stdout); \
    fprintf(stderr, "%s:%d:0: DBG: ", __FILE__, __LINE__); \
    fprintf(stderr, fmt, ##__VA_ARGS__); \
    printf(ASC_CLEAR); fflush(stdout); \
  } while (0)
#else
#define DBG(fmt, ...)
#endif

#define WARN(fmt, ...) \
  do \
  { \
    BP(); \
    printf(ASC_YELLOW); fflush(stdout); \
    fprintf(stderr, "%s:%d:0: WARN: ", __FILE__, __LINE__); \
    fprintf(stderr, fmt, ##__VA_ARGS__); \
    printf(ASC_CLEAR); fflush(stdout); \
  } while (0)

#if defined(DEBUG_BUILD)
  #define ASSERT(c) do { if (!(c)) { FATAL_ERROR("Assertion Error\n\t%s\n", STRINGIFY(c)); } } while (0)
  #define NO_CODE_PATH ASSERT(!"UNREACHABLE_CODE_PATH")
  #define NO_DEFAULT_CASE default: { NO_CODE_PATH; } break
#else
  #define ASSERT(c)
  #define NO_CODE_PATH UNREACHABLE() 
  #define NO_DEFAULT_CASE default: { UNREACHABLE(); } break
#endif

#define STATIC_ASSERT(cond) typedef u8 UNIQUE_NAME(v) [(cond)?1:-1]
#define NOT_IMPLEMENTED() ASSERT(!"NOT_IMPLEMENTED")
#define TODO() ASSERT(!"TODO")

#include <time.h>
#include <sys/types.h>
#include <unistd.h>

#define LINUX_WALLTIME_FREQ NANO_TO_SEC(1)
INTERNAL u64
linux_walltime(void)
{
  u64 result = 0;

  struct timespec time_spec = ZERO_STRUCT;
  // not actually time since epoch, 1 jan 1970
  // rather time since some unspecified period in past

  int clock_res = clock_gettime(CLOCK_MONOTONIC_RAW, &time_spec);
  if (clock_res == -1)
    WARN("clock_gettime failed\n\t%s", strerror(errno));

  result = ((u64)time_spec.tv_sec * LINUX_WALLTIME_FREQ) + (u64)time_spec.tv_nsec;

  return result;
}

INTERNAL u64
linux_estimate_cpu_timer_freq(void)
{
  u64 cpu_start = read_cpu_timer();
  u64 linux_start = linux_walltime();
  u64 linux_end = 0;
  u64 linux_elapsed = 0;

  u64 ms_to_wait = 100;
  // IMPORTANT(Ryan): Integer math, multiple numerator first
  u64 linux_wait_time = LINUX_WALLTIME_FREQ * ms_to_wait / 1000;

  while (linux_elapsed < linux_wait_time)
  {
    linux_end = linux_walltime();
    linux_elapsed = linux_end - linux_start;
  }

  u64 cpu_end = read_cpu_timer();
  u64 cpu_elapsed = cpu_end - cpu_start;

  u64 cpu_freq = 0;
  if (linux_elapsed > 0)
  {
    cpu_freq = LINUX_WALLTIME_FREQ * cpu_elapsed / linux_elapsed;
  }

  return cpu_freq;
}

INTERNAL u32
linux_get_seed_u32(void)
{
  u32 result = 0;

  if (getentropy(&result, sizeof(result)) == -1)
  {
    WARN("getentropy failed\n\t%s", strerror(errno));
  }

  return result;
}

INTERNAL b32
linux_was_launched_by_gdb(void)
{
  b32 result = false;

  pid_t ppid = getppid(); 
  char path[64] = {0};
  char buf[64] = {0};
  snprintf(path, sizeof(path), "/proc/%d/exe", ppid);
  if (readlink(path, buf, sizeof(buf)) == -1)
  {
    WARN("Unable to readlink\n\t%s", strerror(errno));
  }
  else if (strncmp(buf, "/usr/bin/gdb", sizeof("/usr/bin/gdb")) == 0)
  {
    result = true;
  }

  return result;
}


INTERNAL b32
char_is_shell_safe(char ch)
{
  String8 safe_chars = str8_lit("@%+=:,./-_");
  for (u32 i = 0; i < safe_chars.size; i += 1)
  {
    if (ch == safe_chars.content[i]) return true;
  }

  // NOTE(Ryan): Is a number
  if (ch >= 48 && ch <= 57) return true;

  // NOTE(Ryan): Is a character
  ch &= ~0x20;
  if (ch >= 65 && ch <= 90) return true;

  return false;
}

INTERNAL b32
str8_is_shell_safe(String8 str)
{
  for (u32 i = 0; i < str.size; i += 1)
  {
    if (!char_is_shell_safe(str.content[i])) return false;
  }

  return true;
}

INTERNAL String8
str8_shell_escape(MemArena *arena, String8 str)
{
  if (str.size == 0) return str8_lit("''");

  if (str8_is_shell_safe(str)) return str;

  String8List list = ZERO_STRUCT;
  str8_list_push(arena, &list, str8_lit("'"));

  String8 quote_escape = str8_lit("'\"'\"'");

  u32 cursor = 0;
  while (true)
  {
    memory_index quote = str8_find_substring(str, str8_lit("'"), cursor, 0); 
    str8_list_push(arena, &list, str8_up_to(str.content + cursor, str.content + quote));

    if (quote == str.size) break;

    str8_list_push(arena, &list, quote_escape);
    cursor += (quote + 1);
  }

  str8_list_push(arena, &list, str8_lit("'"));
  
  String8Join join = ZERO_STRUCT;
  String8 escaped_str = str8_list_join(arena, list, &join);

  return escaped_str;
}

INTERNAL void
echo_cmd(char **argv)
{
  MEM_ARENA_TEMP_BLOCK(temp, NULL, 0)
  {
    printf("[CMD]");
    for (; *argv != NULL; argv++)
    {
      printf(" ");
      String8 e = str8_shell_escape(temp.arena, str8_cstr(*argv));
      printf("%.*s", str8_varg(e));
    }
    printf("\n");
  }
}

INTERNAL String8 
linux_read_entire_cmd(MemArena *arena, char *args[], b32 echo)
{
  String8 result = ZERO_STRUCT;

  if (echo) echo_cmd(args);

  // want this to echo the cmd invocation such that can be copied and run seperately and work
  // we don't have to shellescape internally, only for output
  // LOG(args);
  // escape single quotes: ' -> '"'"'

  int stdout_pair[2] = ZERO_STRUCT;
  if (pipe(stdout_pair) == -1)
  {
    result = str8_fmt(arena, "Creating pipes failed: %s", strerror(errno));
    return result;
  }

  pid_t pid = fork();
  if (pid == -1)
  {
    close(stdout_pair[0]);
    close(stdout_pair[1]);
    result = str8_fmt(arena, "Forking failed: %s", strerror(errno));
    return result;
  }

  if (pid == 0)
  {
    int dup_stdout_res = dup2(stdout_pair[1], STDOUT_FILENO);
    int dup_stderr_res = dup2(stdout_pair[1], STDERR_FILENO);
    close(stdout_pair[1]);
    close(stdout_pair[0]);
    if (dup_stdout_res == -1 || dup_stderr_res == -1)
    {
      WARN("Dup failed: %s", strerror(errno));
      exit(1);
    }

    execvp(args[0], args);

    WARN("Exec failed: %s", strerror(errno));

    exit(1);
  }
  else
  {
    int status = 0;
    pid_t wait_res = waitpid(pid, &status, 0);
    if (wait_res == -1)
    {
      result = str8_fmt(arena, "Waitpid failed %s", strerror(errno));
      return result;
    }

    if (!WIFEXITED(status))
    {
      result = str8_fmt(arena, "Command did not exit normally");
      return result;
    }

    u32 buffer_cap = 4096;
    String8 buffer = str8_allocate(arena, buffer_cap);

    // TODO(Ryan): Read multiple times
    s32 bytes_read = read(stdout_pair[0], buffer.content, buffer_cap);
    close(stdout_pair[0]);
    close(stdout_pair[1]);

    if (bytes_read == -1)
    {
      result = str8_fmt(arena, "Reading from pipe failed %s", strerror(errno));
    }
    else
    {
      result.content = buffer.content;
      result.size = bytes_read;
    }

    return result;
  }
}

INTERNAL void
linux_command(char *args[], b32 persist, b32 echo)
{
  if (echo) echo_cmd(args);

  pid_t pid = fork();
  if (pid == -1) 
  {
    WARN("Forking failed: %s", strerror(errno));
    return;
  }

  if (pid == 0)
  {
    if (!persist && prctl(PR_SET_PDEATHSIG, SIGTERM) == -1) 
      WARN("Prctl failed: %s", strerror(errno));

    execvp(args[0], args);

    WARN("Exec failed: %s", strerror(errno));

    exit(1);
  }
}

INTERNAL u32 linux_logical_cores(void) { return (u32)get_nprocs(); }

INTERNAL void
linux_append_ldlibrary(String8 path)
{
  MEM_ARENA_TEMP_BLOCK(temp, NULL, 0)
  {
    String8List ld_library_path_list = ZERO_STRUCT;

    char *ld_library_path = getenv("LD_LIBRARY_PATH");
    if (ld_library_path != NULL)
    {
      str8_list_push(temp.arena, &ld_library_path_list, str8_cstr(ld_library_path));
    }

    str8_list_push(temp.arena, &ld_library_path_list, path);

    String8Join join = ZERO_STRUCT;
    join.mid = str8_lit(":");
    join.post = str8_lit("\0");
    String8 ld_library_path_final = str8_list_join(temp.arena, ld_library_path_list, &join);

    if (setenv("LD_LIBRARY_PATH", (char *)ld_library_path_final.content, 1) == -1)
    {
      WARN("Failed to set $LD_LIBRARY_PATH\n\t%s\n", strerror(errno));
    }
  }
}

INTERNAL void
linux_set_cwd_to_self(void)
{
  MEM_ARENA_TEMP_BLOCK(temp, NULL, 0)
  {
    String8 binary_path = str8_allocate(temp.arena, 128);
    s32 binary_path_size = readlink("/proc/self/exe", (char *)binary_path.content, 128);
    if (binary_path_size == -1)
    {
      WARN("Failed to get binary path\n\t%s\n", strerror(errno));
    }
    else
    {
      // IMPORTANT(Ryan): readlink() won't append NULL byte, so no need to subtract
      binary_path.size = binary_path_size; 

      memory_index last_slash = str8_find_substring(binary_path, str8_lit("/"), 0, MATCH_FLAG_FIND_LAST);
      binary_path.size = last_slash;

      char binary_folder[128] = ZERO_STRUCT;
      str8_to_cstr(binary_path, binary_folder, sizeof(binary_folder));

      if (chdir(binary_folder) == -1)
      {
        WARN("Failed to set cwd to %s\n\t%s\n", binary_folder, strerror(errno));
      }
    }
  }
}

typedef u32 FILE_INFO_FLAG;
enum
{
  FILE_INFO_FLAG_DIRECTORY = (1 << 0),
  FILE_INFO_FLAG_READ_ACCESS = (1 << 1),
  FILE_INFO_FLAG_WRITE_ACCESS = (1 << 2),
  FILE_INFO_FLAG_EXECUTE_ACCESS = (1 << 3),
};

typedef struct LinuxFileInfo LinuxFileInfo;
struct LinuxFileInfo
{
  FILE_INFO_FLAG flags;
  String8 full_name;
  u64 file_size;
  u64 modify_time;
};

INTERNAL LinuxFileInfo
linux_file_info(MemArena *arena, String8 file_name)
{
  LinuxFileInfo file_info = ZERO_STRUCT;

  char buf[512] = ZERO_STRUCT;
  str8_to_cstr(file_name, buf, sizeof(buf));

  int fd = open(buf, O_RDONLY);
  if (fd == -1)
  {
    WARN("Failed to open file %.*s\n\t%s\n", str8_varg(file_name), strerror(errno));
  }
  else
  {
    file_info.full_name = str8_allocate(arena, 512);
    if (realpath(buf, (char *)file_info.full_name.content) == NULL)
    {
      WARN("Failed to realpath file %.*s\n\t%s\n", str8_varg(file_name), strerror(errno));
    }
    else
    {
      file_info.full_name.size = strlen((char *)file_info.full_name.content);
    }


    struct stat file_stat = ZERO_STRUCT;
    // TODO(Ryan): handle symlinks, currently just look at symlink itself
    if (fstatat(fd, buf, &file_stat, AT_NO_AUTOMOUNT | AT_SYMLINK_NOFOLLOW) == 0)
    {
      if ((file_stat.st_mode & S_IFMT) == S_IFDIR) file_info.flags |= FILE_INFO_FLAG_DIRECTORY;
      if (file_stat.st_mode & S_IRUSR) file_info.flags |= FILE_INFO_FLAG_READ_ACCESS;
      if (file_stat.st_mode & S_IWUSR) file_info.flags |= FILE_INFO_FLAG_WRITE_ACCESS;
      if (file_stat.st_mode & S_IXUSR) file_info.flags |= FILE_INFO_FLAG_EXECUTE_ACCESS;

      file_info.modify_time = ((u64)file_stat.st_mtim.tv_sec * 1000) + \
                              (u64)((f32)file_stat.st_mtim.tv_nsec / 1000000.0f);

      file_info.file_size = (u64)file_stat.st_size;
    }
    else
    {
      WARN("Failed to fstatat file %.*s\n\t%s\n", str8_varg(file_name), strerror(errno));
    }

    close(fd);
  }

  return file_info;
}

#if 0
typedef struct FileIter FileIter;
struct FileIter
{
  int dir_fd;
  DIR *dir;
};

typedef void (*visit_files_cb)(MemArena *arena, LinuxFileInfo *file_info, void *user_data);

INTERNAL void
linux_iterate_dir(MemArena *arena, String8 path, visit_files_cb visit_cb, void *user_data, b32 want_recursive = false)
{
  char buf[512] = ZERO_STRUCT;
  str8_to_cstr(file_name, buf, sizeof(buf));

  DIR *dir = opendir((char *)path.content);
  int dir_fd = open((char *)path.content, O_PATH | O_CLOEXEC);

  if (dir != NULL && dir_fd != -1)
  {
    char procfs_buf[64] = ZERO_STRUCT;
    int procfs_buf_len = snprintf(procfs_buf, sizeof(procfs_buf), "/proc/self/fd/%d", dir_fd);

    char dir_full_name[2048] = ZERO_STRUCT;
    readlink(procfs_buf, dir_full_name, sizeof(dir_full_name));

    while (true)
    {
      // this advances iterator, NULL if at end
     .contentuct dirent *dir_entry = readdir(dir);
      if (dir_entry == NULL) 
      {
        break;
      }

      if .contentcmp(dir_entry->d_name, "..") == 0 ||.contentcmp(dir_entry->d_name, ".") == 0)
      {
        continue;
      }

      LinuxFileInfo file_info = ZERO_STRUCT;
      file_info.short_name =.content8_.contenting(dir_entry->d_name);
      
      char full_file_name_buf[PATH_MAX] = ZERO_STRUCT;
      snprintf(full_file_name_buf, sizeof(full_file_name_buf), "%s/%s", dir_full_name, dir_entry->d_name);
      file_info.full_name =.content8_.contenting(full_file_name_buf);

     .contentuct stat file_stat = ZERO_STRUCT;
      // TODO(Ryan): handle symlinks, currently just look at symlink itself
      if (fstatat(dir_fd, dir_entry->d_name, &file_stat, AT_NO_AUTOMOUNT | AT_SYMLINK_NOFOLLOW) == 0)
      {
        if ((file_stat.st_mode & S_IFMT) == S_IFDIR)
        {
          file_info.flags |= FILE_INFO_FLAG_DIRECTORY;
        }

        file_info.modify_time = ((u64)file_stat.st_mtim.tv_sec * 1000) + \
                                  (u64)((f32)file_stat.st_mtim.tv_nsec / 1000000.0f);

        file_info.file_size = (u64)file_stat.st_size;
      }

      ASSERT(visit_cb != NULL);
      visit_cb(arena, &file_info, user_data);

      if (file_info.flags & FILE_INFO_FLAG_DIRECTORY && want_recursive) 
      {
        linux_visit_files(arena, file_info.full_name, visit_cb, user_data);
      }
    }

    closedir(dir);
    close(dir_fd);
  }

}
#endif

INTERNAL u64 
linux_page_fault_count(void)
{
  struct rusage usage = ZERO_STRUCT;
  if (getrusage(RUSAGE_SELF, &usage) == -1) WARN("rusage failed: %s\n", strerror(errno));

  u64 result = usage.ru_minflt + usage.ru_majflt;

  return result;
}

INTERNAL b32
linux_rename_file(String8 og_name, String8 new_name)
{
  char buf1[512] = ZERO_STRUCT;
  str8_to_cstr(og_name, buf1, sizeof(buf1));

  char buf2[512] = ZERO_STRUCT;
  str8_to_cstr(new_name, buf2, sizeof(buf2));

  return (rename(buf1, buf2) == 0);
}

INTERNAL b32
linux_delete_file(String8 path)
{
  char buf[512] = ZERO_STRUCT;
  str8_to_cstr(path, buf, sizeof(buf));

	return (remove(buf) == 0);
}

INTERNAL b32
linux_create_directory(String8 directory_name)
{
  char buf[512] = ZERO_STRUCT;
  str8_to_cstr(directory_name, buf, sizeof(buf));

	return (mkdir(buf, S_IRWXU) == 0);
}

INTERNAL b32
linux_does_file_exist(String8 path)
{
  char buf[512] = ZERO_STRUCT;
  str8_to_cstr(path, buf, sizeof(buf));

	return (access(buf, F_OK) == 0);
}
