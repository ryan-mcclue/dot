// SPDX-License-Identifier: zlib-acknowledgement
#if !defined(BASE_DEV_LINUX_H)
#define BASE_DEV_LINUX_H

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
#include <termios.h>

// NOTE(Ryan): Allow for simple runtime debugger attachment
GLOBAL b32 global_debugger_present;

#if DEBUG_BUILD
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

// IMPORTANT(Ryan): x86intrin.h includes approx 46kLOC!
INTERNAL u64
read_cpu_timer(void)
{
  u32 a, d = 0;
  asm volatile("rdtsc" : "=a" (a), "=d" (d));
  return ((u64)d << 32) | a;
}

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

#define NOT_IMPLEMENTED() ASSERT(!"NOT_IMPLEMENTED")
#define TODO() ASSERT(!"TODO")

#include <time.h>
#include <sys/types.h>
#include <unistd.h>

INTERNAL void
linux_sleep(u64 ns)
{
  struct timespec rem = ZERO_STRUCT;
  struct timespec req = ZERO_STRUCT;
  req.tv_sec = ns / 1000000000ULL;
  req.tv_nsec = ns - req.tv_sec * 1000000000ULL;
  while (nanosleep(&req, &rem))
      req = rem;
}

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

INTERNAL void
initialise_serial_port(char *serial_port, u32 baud_rate, u32 read_timeout)
{
  // IMPORTANT(Ryan): If specifying non-blocking arguments in here can affect VTIM
  // and result in 'resource temporarily unavaiable'
	int serial_fd = open(serial_port, O_RDWR);
	if (serial_fd < 0) 
  {

    WARN("opening serial port %s failed (%s)\n", serial_port, strerror(errno));
		return;
	}

	struct termios serial_options = {0};
	if (tcgetattr(serial_fd, &serial_options) == -1)
  {
    WARN("obtaining original serial port settings failed (%s)\n", strerror(errno));
		return;
  }

  // NOTE(Ryan): Important to alter specific bits to avoid undefined behaviour 

  // CONTROL MODES
  // disable parity
  serial_options.c_cflag &= ~PARENB;

  // one stop bit
  serial_options.c_cflag &= ~CSTOPB;

  serial_options.c_cflag &= ~CSIZE;
  serial_options.c_cflag |= CS8;

  // disable hardware flow control
  serial_options.c_cflag &= ~CRTSCTS;

  // allow reads
  // no carrier detect if removed
  serial_options.c_cflag |= (CREAD | CLOCAL);

  // LOCAL MODES
  // disable canonical mode so as to recieve all raw bytes and say not interpret backspace specially
  serial_options.c_lflag &= ~ICANON;

  serial_options.c_lflag &= ~ECHO; // Disable echo
  serial_options.c_lflag &= ~ECHOE; // Disable erasure
  serial_options.c_lflag &= ~ECHONL; // Disable new-line echo

  // disable interpretation of certain signals
  serial_options.c_lflag &= ~ISIG;

  // INPUT MODES
  // disable software flow control
  serial_options.c_iflag &= ~(IXON | IXOFF | IXANY);
  // disble special handling of bytes
  serial_options.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL);

  // OUTPUT MODES
  // disable special handling of bytes
  serial_options.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
  serial_options.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed

  // whther blocking or not determined with VMIN and VTIME
  serial_options.c_cc[VTIME] = (read_timeout * 10);    // Wait for up to deciseconds, returning as soon as any data is received.
  serial_options.c_cc[VMIN] = 0;

  cfsetspeed(&serial_options, 9600);

	if (tcsetattr(serial_fd, TCSANOW, &serial_options) == -1)
  {
    WARN("setting serial port baud rate to %d failed (%s)\n", baud_rate, strerror(errno));
		return;
  }

  //if (flock(serial_fd, LOCK_EX | LOCK_NB) == -1)
  //{
  //  WARN("obtaining exclusive access to %s failed (%s)\n", serial_port, strerror(errno));
  //}
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

// IMPORTANT(Ryan): Potentially have to handle signals to not crash from external command
// sigaction(SIGPIPE, &act, NULL);
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

#endif

#if 0

typedef void (*dbus_callback_t)(struct l_dbus_message *msg);

INTERNAL void
dbus_callback_wrapper(struct l_dbus_message *reply_message, void *user_data)
{
  const char *path = l_dbus_message_get_path(reply_message);
  const char *interface = l_dbus_message_get_interface(reply_message);
  const char *member = l_dbus_message_get_member(reply_message);
  const char *destination = l_dbus_message_get_destination(reply_message);
  const char *sender = l_dbus_message_get_sender(reply_message);
  const char *signature = l_dbus_message_get_signature(reply_message);

  const char *error_name = NULL;
  const char *error_text = NULL;

  if (l_dbus_message_is_error(reply_message))
  {
    l_dbus_message_get_error(reply_message, &error_name, &error_text);

    char error_message[128] = {0};
    snprintf(error_message, sizeof(error_message), "%s: %s", error_name, 
             error_text);
    BP_MSG(error_message);

    l_free(error_name);
    l_free(error_text);
  }
  else
  {
    dbus_callback_t callback = (dbus_callback_t)user_data;
    callback(reply_message);
  }
}

INTERNAL void 
dbus_request_name_callback(struct l_dbus *dbus_connection, bool success, bool queued, void *user_data)
{
  if (!success)
  {
    BP_MSG("Failed to acquire dbus name"); 
  }
}

typedef struct BluetoothDevice
{
  char dbus_path[128];
  char address[128];
  s32 rssi;
} BluetoothDevice;

GLOBAL b32 global_want_to_run = true;

GLOBAL struct l_hashmap *global_bluetooth_devices_map = NULL;
#define MAX_BLUETOOTH_DEVICES_COUNT 16
GLOBAL BluetoothDevice global_bluetooth_devices[MAX_BLUETOOTH_DEVICES_COUNT];
GLOBAL u32 global_bluetooth_device_count = 0;

GLOBAL struct l_dbus *global_dbus_connection = NULL;


INTERNAL void 
bluez_interfaces_added_callback(struct l_dbus_message *reply_message)
{
  BluetoothDevice *active_bluetooth_device = NULL;

  struct l_dbus_message_iter root_dict_keys_iter, root_dict_values_iter = {0};
  const char *dbus_path = NULL;
  if (l_dbus_message_get_arguments(reply_message, "oa{sa{sv}}", &dbus_path, &root_dict_keys_iter))
  {
    printf("Found device: %s\n", dbus_path);
    const char *root_dict_key = NULL;
    while (l_dbus_message_iter_next_entry(&root_dict_keys_iter, &root_dict_key, &root_dict_values_iter))
    {
      if (strcmp(root_dict_key, "org.bluez.Device1") == 0)
      {
        const char *device_dict_key = NULL;
        struct l_dbus_message_iter device_dict_values_iter = {0};
        while (l_dbus_message_iter_next_entry(&root_dict_values_iter, &device_dict_key, &device_dict_values_iter))
        {
          if (strcmp(device_dict_key, "Address") == 0)
          {
            const char *address = NULL;
            l_dbus_message_iter_get_variant(&device_dict_values_iter, "s", &address);
            printf("Found device: %s\n", address);
            
            ASSERT(global_bluetooth_device_count != MAX_BLUETOOTH_DEVICES_COUNT);
            active_bluetooth_device = &global_bluetooth_devices[global_bluetooth_device_count++];
            strcpy(active_bluetooth_device->dbus_path, dbus_path); 
            strcpy(active_bluetooth_device->address, address); 

            bool bluetooth_device_insert_status = l_hashmap_insert(global_bluetooth_devices_map, address, active_bluetooth_device);
            ASSERT(bluetooth_device_insert_status);
          }
          if (strcmp(device_dict_key, "RSSI") == 0)
          {
            s16 rssi = 0;
            l_dbus_message_iter_get_variant(&device_dict_values_iter, "n", &rssi);

            ASSERT(active_bluetooth_device != NULL);
            active_bluetooth_device->rssi = rssi;
          }
        }
      }
      else if (strcmp(root_dict_key, "org.bluez.GattService1") == 0)
      {
        const char *service_dict_key = NULL;
        struct l_dbus_message_iter service_dict_values_iter = {0};
        while (l_dbus_message_iter_next_entry(&root_dict_values_iter, &service_dict_key, &service_dict_values_iter))
        {
// SIG UUIDs --> 0000xxxx-0000-1000-8000-00805f9b34fb (https://www.bluetooth.com/specifications/assigned-numbers/)
// 0x2a05 --> 00002a05-0000-1000-8000-00805f9b34fb
// 0x1801 -- > 00001801-0000-1000-8000-00805f9b34fb
          // probably want dbus path as well
          // NOTE(Ryan): We could determine UUID from d-feet (allows calling methods as well)
          if (strcmp(service_dict_key, "UUID") == 0)
          {
            
          }
          // TODO(Ryan): ServicesResolved when done
        }
      }
      else if (strcmp(root_dict_key, "org.bluez.GattCharacteristic1") == 0)
      {
        // UUID and Flags (ensure has 'read' flag)
        // ReadValue({})
        // WriteValue(bytes, {}) (if throughput becomes an issue, may have to specify write command instead of request?)
        // better to just write less than 20 bytes at a time?
      }
      else if (strcmp(root_dict_key, "org.bluez.GattDescriptor1") == 0)
      {
        // UUID
      }
    }
  }
  else
  {
    BP_MSG("InterfacesAdded dict expected but not recieved");
  }
}

INTERNAL void 
bluez_start_discovery_callback(struct l_dbus_message *reply_message)
{
  printf("Searching for unmanaged bluetooth devices...\n");
}

INTERNAL void
bluez_connect_callback(struct l_dbus_message *reply_message)
{
}


INTERNAL void 
bluez_stop_discovery_callback(struct l_dbus_message *reply_message)
{
  printf("Found devices:\n");

  BluetoothDevice *device = l_hashmap_lookup(global_bluetooth_devices_map, "E4:15:F6:5F:FA:9D");
  if (device != NULL)
  {
    // TODO(Ryan): Ensure connection successful by looking at Connected property
    struct l_dbus_message *bluez_connect_msg = l_dbus_message_new_method_call(global_dbus_connection, "org.bluez", device->dbus_path, 
                                                                              "org.bluez.Device1", "Connect");
    ASSERT(bluez_connect_msg != NULL);

    bool bluez_connect_msg_set_argument_status = l_dbus_message_set_arguments(bluez_connect_msg, "");
    ASSERT(bluez_connect_msg_set_argument_status);

    l_dbus_send_with_reply(global_dbus_connection, bluez_connect_msg, dbus_callback_wrapper, bluez_connect_callback, NULL);
  }
  else
  {
    printf("HMSoft not found!\n");
  }

  global_want_to_run = false;
}

INTERNAL void
bluez_get_managed_objects_callback(struct l_dbus_message *reply_message)
{
  struct l_dbus_message_iter root_dict_keys_iter = {0}, root_dict_values_iter = {0};
  if (l_dbus_message_get_arguments(reply_message, "a{oa{sa{sv}}}", &root_dict_keys_iter))
  {
    const char *root_dict_key = NULL;
    while (l_dbus_message_iter_next_entry(&root_dict_keys_iter, &root_dict_key, &root_dict_values_iter))
    {
      // IMPORTANT(Ryan): Our device: /org/bluez/hci0, remote device: /org/bluez/hci0/dev_*
      if (strncmp(root_dict_key, "/org/bluez/hci0/", sizeof("/org/bluez/hci0/") - 1) == 0)
      {
        const char *objects_dict_key = NULL;
        struct l_dbus_message_iter objects_dict_values_iter = {0};
        while (l_dbus_message_iter_next_entry(&root_dict_values_iter, &objects_dict_key, &objects_dict_values_iter))
        {
          if (strcmp(objects_dict_key, "org.bluez.Device1") == 0)
          {
            const char *properties_dict_key = NULL;
            struct l_dbus_message_iter properties_dict_values_iter = {0};
            while (l_dbus_message_iter_next_entry(&objects_dict_values_iter, &properties_dict_key, &properties_dict_values_iter))
            {
              if (strcmp(properties_dict_key, "Address") == 0)
              {
                const char *address = NULL;
                l_dbus_message_iter_get_variant(&properties_dict_values_iter, "s", &address);

                ASSERT(global_bluetooth_device_count != MAX_BLUETOOTH_DEVICES_COUNT);
                BluetoothDevice *bluetooth_device = &global_bluetooth_devices[global_bluetooth_device_count++];
                strcpy(bluetooth_device->dbus_path, root_dict_key); 
                strcpy(bluetooth_device->address, address); 

                bool bluetooth_device_insert_status = l_hashmap_insert(global_bluetooth_devices_map, address, bluetooth_device);
                ASSERT(bluetooth_device_insert_status);
              }
            }
          }
        }
      }
    }
  }
  else
  {
    BP_MSG("GetManagedObjects dict expected but not recieved");
  }
}


INTERNAL void
falsify_global_want_to_run(int signum)
{
  global_want_to_run = false;
}

// IMPORTANT(Ryan): If we want to inspect the type information of a message, use
// $(sudo dbus-monitor --system)

// IMPORTANT(Ryan): Don't have $(bluetoothctl) open when running application, as it will intercept bluez devices first!!!

// IMPORTANT(Ryan): $(usermod -a -G dialout ryan) to get access to serial port

int main(int argc, char *argv[])
{
  if (l_main_init())
  {
    global_dbus_connection = l_dbus_new_default(L_DBUS_SYSTEM_BUS);
    if (global_dbus_connection != NULL)
    { 
      __sighandler_t prev_signal_handler = signal(SIGINT, falsify_global_want_to_run);
      ASSERTE(prev_signal_handler != SIG_ERR);

      global_bluetooth_devices_map = l_hashmap_string_new();
      ASSERT(global_bluetooth_devices_map != NULL);

      // NOTE(Ryan): Cannot acquire name on system bus without altering dbus permissions  
      // l_dbus_name_acquire(global_dbus_connection, "my.bluetooth.app", false, false, false, dbus_request_name_callback, NULL);
      
      // TODO(Ryan): Watch for InterfacesRemoved and PropertiesChanged during discovery phase
      unsigned int bluez_interfaces_added_id = l_dbus_add_signal_watch(global_dbus_connection, "org.bluez", "/", 
                                                                       "org.freedesktop.DBus.ObjectManager", "InterfacesAdded", 
                                                                       L_DBUS_MATCH_NONE, dbus_callback_wrapper, bluez_interfaces_added_callback);
      ASSERT(bluez_interfaces_added_id != 0);

      struct l_dbus_message *bluez_start_discovery_msg = l_dbus_message_new_method_call(global_dbus_connection, "org.bluez", "/org/bluez/hci0", 
                                                                                       "org.bluez.Adapter1", "StartDiscovery");
      ASSERT(bluez_start_discovery_msg != NULL);

      bool bluez_start_discovery_msg_set_argument_status = l_dbus_message_set_arguments(bluez_start_discovery_msg, "");
      ASSERT(bluez_start_discovery_msg_set_argument_status);

      l_dbus_send_with_reply(global_dbus_connection, bluez_start_discovery_msg, dbus_callback_wrapper, bluez_start_discovery_callback, NULL);


      struct l_dbus_message *bluez_get_managed_objects_msg = l_dbus_message_new_method_call(global_dbus_connection, "org.bluez", "/", 
                                                                                            "org.freedesktop.DBus.ObjectManager", 
                                                                                            "GetManagedObjects");
      ASSERT(bluez_get_managed_objects_msg != NULL);

      bool bluez_get_managed_objects_msg_set_argument_status = l_dbus_message_set_arguments(bluez_get_managed_objects_msg, "");
      ASSERT(bluez_get_managed_objects_msg_set_argument_status);

      l_dbus_send_with_reply(global_dbus_connection, bluez_get_managed_objects_msg, dbus_callback_wrapper, bluez_get_managed_objects_callback, NULL);


      u64 start_time = get_ns();
      u64 discovery_time = SECONDS_NS(5);
      b32 are_discovering = true;
      while (global_want_to_run)
      {
        if (are_discovering)
        {
          if (get_ns() - start_time >= discovery_time)
          {
            are_discovering = false;

            struct l_dbus_message *bluez_stop_discovery_msg = l_dbus_message_new_method_call(global_dbus_connection, "org.bluez", "/org/bluez/hci0", 
                                                                                            "org.bluez.Adapter1", "StopDiscovery");
            ASSERT(bluez_stop_discovery_msg != NULL);

            bool bluez_stop_discovery_msg_set_argument_status = l_dbus_message_set_arguments(bluez_stop_discovery_msg, "");
            ASSERT(bluez_stop_discovery_msg_set_argument_status);

            l_dbus_send_with_reply(global_dbus_connection, bluez_stop_discovery_msg, dbus_callback_wrapper, bluez_stop_discovery_callback, NULL);

            l_dbus_remove_signal_watch(global_dbus_connection, bluez_interfaces_added_id);
          }
        }

        // NOTE(Ryan): This will hang when no events left, i.e. return -1
        // int timeout = l_main_prepare();
        l_main_iterate(0);
      }

      l_dbus_destroy(global_dbus_connection);
      l_main_exit();
    }
    else
    {
      BP_MSG("Unable to create dbus connection");
      l_main_exit();
    }

  }
  else
  {
    BP_MSG("Unable to initialise ELL main loop");
  }

}

/*
 *
UUID_NAMES = {
    "00001801-0000-1000-8000-00805f9b34fb" : "Generic Attribute Service",
    "0000180a-0000-1000-8000-00805f9b34fb" : "Device Information Service",
    "e95d93b0-251d-470a-a062-fa1922dfa9a8" : "DFU Control Service",
    "e95d93af-251d-470a-a062-fa1922dfa9a8" : "Event Service",
    "e95d9882-251d-470a-a062-fa1922dfa9a8" : "Button Service",
    "e95d6100-251d-470a-a062-fa1922dfa9a8" : "Temperature Service",
    "e95dd91d-251d-470a-a062-fa1922dfa9a8" : "LED Service",
    "00002a05-0000-1000-8000-00805f9b34fb" : "Service Changed",
    "e95d93b1-251d-470a-a062-fa1922dfa9a8" : "DFU Control",
    "00002a05-0000-1000-8000-00805f9b34fb" : "Service Changed",
    "00002a24-0000-1000-8000-00805f9b34fb" : "Model Number String",
    "00002a25-0000-1000-8000-00805f9b34fb" : "Serial Number String",
    "00002a26-0000-1000-8000-00805f9b34fb" : "Firmware Revision String",
    "e95d9775-251d-470a-a062-fa1922dfa9a8" : "micro:bit Event",
    "e95d5404-251d-470a-a062-fa1922dfa9a8" : "Client Event",
    "e95d23c4-251d-470a-a062-fa1922dfa9a8" : "Client Requirements",
    "e95db84c-251d-470a-a062-fa1922dfa9a8" : "micro:bit Requirements",
    "e95dda90-251d-470a-a062-fa1922dfa9a8" : "Button A State",
    "e95dda91-251d-470a-a062-fa1922dfa9a8" : "Button B State",
    "e95d9250-251d-470a-a062-fa1922dfa9a8" : "Temperature",
    "e95d93ee-251d-470a-a062-fa1922dfa9a8" : "LED Text",
    "00002902-0000-1000-8000-00805f9b34fb" : "Client Characteristic Configuration",
}    

DEVICE_INF_SVC_UUID = "0000180a-0000-1000-8000-00805f9b34fb"
MODEL_NUMBER_UUID    = "00002a24-0000-1000-8000-00805f9b34fb"

TEMPERATURE_SVC_UUID = "e95d6100-251d-470a-a062-fa1922dfa9a8"
TEMPERATURE_CHR_UUID = "e95d9250-251d-470a-a062-fa1922dfa9a8"

LED_SVC_UUID = "e95dd91d-251d-470a-a062-fa1922dfa9a8"
LED_TEXT_CHR_UUID = "e95d93ee-251d-470a-a062-fa1922dfa9a8"
 *
 * HM-10 capabilties: http://www.martyncurrey.com/hm-10-bluetooth-4ble-modules/#HM-10_Services_and_Characteristics
 *
 * Values to TX/RX are only interpreted as AT commands prior to connection 
 *
 * On device connect, recieve: OK+CONN
 * 
 * AT+ADDR? 
 * AT+VERR?
 * AT+NAME? (AT+NAMERYAN)
 * AT+PASS? (AT+PASS123456)
 * AT+ROLE? (0 for peripheral, 1 for central). AT+ROLE0 to set
 *
 * AT+TYPE2 (set bond mode to authorise, i.e. require password and pair to connect)
 *
 * AT+UUID? (service UUID)
 * AT+CHAR? (characteristic value)
 *
 * So, if just reading bytes from TX/RX this is the default characteristic, i.e. can only have 1 characteristic on HM-10?
 * When writing raw bytes to the TX/RX we are altering the value of the default characteristic (however, does this send it or will remote only get it if they subscribe?)
 * (remote can directly read, however notification of change also)
 */

      // this won't work if device requires pairing or isn't advertising
      // IMPORTANT: Once connected, the Connected property of the device will be set, as will be seen in device PropertiesChanged
      // We could check the Connected property prior to attempting a connection using an appropriate Get()

      /*
        l_dbus_add_signal_watch(global_dbus_connection, "org.bluez", "/", 
                                "org.freedesktop.DBus.ObjectManager", "InterfacesRemoved", 
                                L_DBUS_MATCH_NONE, dbus_interfaces_removed_callback, NULL);
        IMPORTANT: This PropertiesChanged is only during discovery phase
        l_dbus_add_signal_watch(global_dbus_connection, "org.bluez", "/org/bluez/hci0", 
                                "org.freedesktop.DBus.Properties", "PropertiesChanged", 
                                L_DBUS_MATCH_NONE, dbus_properties_changed_callback, NULL);
       */

      // IMPORTANT(Ryan): InterfacesRemoved could be called during this discovery time
      //if (dbus_interfaces_added_id != 0)
      //{
      //  l_dbus_remove_signal_watch(global_dbus_connection, dbus_interfaces_added_id);
      //}



// gateway --> $(route -n)
// mac -> $(ip address show)
// create static ip lease 
// add in hostname in /etc/hosts (however if DHCP on server, just reconnect to get this?)

// RFCOMM protocol. 
// Serial Port Profile is based on RFCOMM protocol
// profile will have a UUID
//
// to connect to bluetooth socket, require mac address like AB:12:4B:59:23:0A
// so, convert from "Connecting to /org/bluez/hci0/dev_5C_03_39_C5_BA_C7"

// L2CAP, MGMT, HCI sockets?

// investigate $(btmon) $(btmgt)

#include "types.h"

#if defined(GUI_INTERNAL)
  INTERNAL void __bp(char const *file_name, char const *func_name, int line_num,
                     char const *optional_message)
  { 
    fprintf(stderr, "BREAKPOINT TRIGGERED! (%s:%s:%d)\n\"%s\"\n", file_name, func_name, 
            line_num, optional_message);
#if !defined(GUI_DEBUGGER)
    exit(1);
#endif
  }
  INTERNAL void __ebp(char const *file_name, char const *func_name, int line_num)
  { 
    char *errno_msg = strerror(errno);
    fprintf(stderr, "ERRNO BREAKPOINT TRIGGERED! (%s:%s:%d)\n\"%s\"\n", file_name, 
            func_name, line_num, errno_msg);
#if !defined(GUI_DEBUGGER)
    exit(1);
#endif
  }
  #define BP_MSG(msg) __bp(__FILE__, __func__, __LINE__, msg)
  #define BP() __bp(__FILE__, __func__, __LINE__, "")
  #define EBP() __ebp(__FILE__, __func__, __LINE__)
  #define ASSERT(cond) if (!(cond)) {BP();}
#else
  #define BP_MSG(msg)
  #define BP()
  #define EBP()
  #define ASSERT(cond)
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

#include <time.h>

#include <signal.h>

#include <ell/ell.h>

// IMPORTANT(Ryan): may have to "sudo chmod 666 /dev/ttyUSB*" 

// Johannes 4GNU_Linux

INTERNAL int
obtain_serial_connection(char *path)
{
  int result = 0;

  int fd = open(path, O_RDWR | O_NDELAY | O_NOCTTY);
  if (fd > 0)
  {
    struct termios serial_options = {0};
    u32 config_8n1 = CS8 | CLOCAL | CREAD;
    serial_options.c_cflag = config_8n1 | B9600;
    serial_options.c_iflag = IGNPAR;

    tcflush(fd, TCIFLUSH);
    tcsetattr(fd, TCSANOW, &serial_options);

    //write(bluetooth_serial_fd, data, len);
    //if (read(tty_fd,&c,1)>0)

    ////read a webpage that i have downloaded link https://www.cmrr.umn.edu/~strupp/serial.html
    //// 8n1, see termios.h for more 
    //bluetooth_serial_port.c_cc[VMIN] = 1; //timeout period it should be set to zero if you want to print 0 if nothing is received before timeout, 
    //bluetooth_serial_port.c_cc[VTIME] = 5; //time out period in units of 1/10th of a second, so here the period is 500ms
  }
  else
  {
    EBP();
  }

  result = fd;

  return result;
}

INTERNAL void 
proxy_added(struct l_dbus_proxy *proxy, void *user_data)
{
	const char *interface = l_dbus_proxy_get_interface(proxy);
	const char *path = l_dbus_proxy_get_path(proxy);

	printf("(PROXY ADDED) Path: %s, Interface: %s\n", path, interface);

	//if (!strcmp(interface, "org.bluez.Adapter1") ||
	//			!strcmp(interface, "org.bluez.Device1")) {
	//	char *str;

	//	if (!l_dbus_proxy_get_property(proxy, "Address", "s", &str))
	//		return;

	//	l_info("   Address: %s", str);
	//}
}

INTERNAL void 
proxy_removed(struct l_dbus_proxy *proxy, void *user_data)
{
	const char *interface = l_dbus_proxy_get_interface(proxy);
	const char *path = l_dbus_proxy_get_path(proxy);
	printf("(PROXY REMOVED) Path: %s, Interface: %s\n", path, interface);
}

INTERNAL void 
property_changed(struct l_dbus_proxy *proxy, const char *name,
				struct l_dbus_message *msg, void *user_data)
{
	const char *interface = l_dbus_proxy_get_interface(proxy);
	const char *path = l_dbus_proxy_get_path(proxy);
	printf("(PROPERTY CHANGED) Path: %s, Interface: %s\n", path, interface);

	//if (!strcmp(name, "Address")) {
	//	char *str;

	//	if (!l_dbus_message_get_arguments(msg, "s", &str)) {
	//		return;
	//	}

	//	l_info("   Address: %s", str);
	//}
}

INTERNAL void
get_hostname_cb(struct l_dbus_message *reply, void *user_data)
{
	struct l_dbus_message_iter iter = {0};
	const char *hostname = NULL;

	l_dbus_message_get_arguments(reply, "v", &iter);

	l_dbus_message_iter_get_variant(&iter, "s", &hostname);

  printf("Hostname: %s\n", hostname);

  // l_free()
}

INTERNAL void
parse_array(struct l_dbus_message *reply, void *user_data)
{
	struct l_dbus_message_iter iter = {0};
	const char *arr = NULL;

  l_dbus_message_get_arguments(reply, "as", &iter);

  b32 elem_recieved = false;
  do
  {
    elem_recieved = l_dbus_message_iter_next_entry(&iter, &arr);
  } while (elem_recieved);

  // need to l_dbus_message_unref(msg)?
}

GLOBAL b32 global_want_to_run = true;

INTERNAL void
falsify_global_want_to_run(int signum)
{
  global_want_to_run = false;
}

// TODO(Ryan): Proxies are for DBus.Properties? How?
// Memory managed just will l_free() or also need message_unref()?

#define KILO (1000LL)
#define MEGA (KILO * 1000LL)
#define GIGA (MEGA * 1000LL)
#define TERA (GIGA * 1000LL)

INTERNAL u64
get_ns(void)
{
  u64 result = 0;

  struct timespec cur_timespec = {0}; 

  if (clock_gettime(CLOCK_MONOTONIC, &cur_timespec) != -1)
  {
    result = cur_timespec.tv_nsec + (cur_timespec.tv_sec * (1 * TERA));
  }
  else
  {
    EBP();
  }

  return result;
}

INTERNAL void 
interfaces_added_cb(struct l_dbus_message *message, void *user_data)
{
  const char *unique_device_path = l_dbus_message_get_path(message);

  // We know is dictionary of variants.
  // So, print out top-level dictionary keys and progress further ...

  // message is a dictionary:
  // we know Address is first, however order of other items are not fixed
  // so, iterate over whole dictionary...
  // we expect "org.bluez.Device1" : {
  //   "Address": ,
  //   ....
  //   "Name": ,
  //   "RSSI": ,
  // } 
  l_dbus_message_get_arguments(message, "a{sv}",);

	const char *interface, *property, *value;
	struct l_dbus_message_iter variant, changed, invalidated;

	if (!signal_timeout)
		return;

	test_assert(l_dbus_message_get_arguments(message, "sa{sv}as",
							&interface, &changed,
							&invalidated));

	test_assert(l_dbus_message_iter_next_entry(&changed, &property,
							&variant));
	test_assert(!strcmp(property, "String"));
	test_assert(l_dbus_message_iter_get_variant(&variant, "s", &value));
	test_assert(!strcmp(value, "foo"));

	test_assert(!l_dbus_message_iter_next_entry(&changed, &property,
							&variant));
	test_assert(!l_dbus_message_iter_next_entry(&invalidated,
							&property));

	test_assert(!new_signal_received);
	new_signal_received = true;

	test_check_signal_success();
}

//GLOBAL b32 global_dbus_name_has_been_acquired = false;
//      //l_dbus_name_acquire(dbus_conn, "my.bluetooth.app", false, false, false, 
//      //                  set_name, NULL);
//INTERNAL void 
//request_name_callback(struct l_dbus *dbus, bool success, bool queued, void *user_data)
//{
//  global_dbus_name_has_been_acquired = success ? (queued ? "queued" : "success") : "failed";
//}

// $(d-feet) useful!!!!
INTERNAL void
dbus(void)
{
  if (l_main_init())
  {
    struct l_dbus *dbus_conn = l_dbus_new_default(L_DBUS_SYSTEM_BUS);
    if (dbus_conn != NULL)
    {
      signal(SIGINT, falsify_global_want_to_run);

      int ell_main_loop_timeout = l_main_prepare();
      while (global_want_to_run)
      {
        // NOTE(Ryan): Be notified of advertising packets from unknown devices
        // This will scan for both classic and le devices?
        l_dbus_add_signal_watch(dbus_conn, "org.freedesktop.DBus.ObjectManager", 
                                "/org/bluez/hci0", "InterfacesAdded", L_DBUS_MATCH_NONE, 
                                interfaces_added_cb, NULL);

        const char *service = "org.bluez";
        // IMPORTANT(Ryan): More elegant to obtain adapter object from GetManagedObjects()
        // However, this is the default in most circumstances 
        const char *adapter_object = "/org/bluez/hci0";
        const char *adapter_interface = "org.bluez.Adapter1";
        const char *start_discovery_method = "StartDiscovery";

        // StartDiscovery method
        struct l_dbus_message *msg = \
          l_dbus_message_new_method_call(dbus_conn, service, object, interface, method);

        const char *get_interface = "org.freedesktop.hostname1"; 
        const char *get_property = "Hostname";
        l_dbus_message_set_arguments(msg, "ss", get_interface, get_property);

        // this is where an error could occur?
        l_dbus_send_with_reply(dbus_conn, msg, get_hostname_cb, NULL, NULL);

        // l_free()

        l_main_iterate(ell_main_loop_timeout);
      }

      //const char *err_name, *err_text = NULL;
      //if (l_dbus_message_is_error(reply_msg))
      //{
      //  printf("error\n");
      //  l_dbus_message_get_error(reply_msg, &err_name, &err_text);
      //}

      printf("Exiting...\n");

      l_dbus_destroy(dbus_conn);
      l_main_exit();
    }
    else
    {
      EBP();
    }
  }
}

int 
main(int argc, char *argv[])
{
  dbus();
// build from Makefile.am: $(autoreconf -f -i; ./configure)

// FROM BLUETOOTH SIG
// profile is method of obtaining data from device
// GATT (Generic Attribute Profile) defines a table of data that lists
// state and operations that can be performed on them (Attribute Table)
// service -> characteristic -> optional descriptor  
// server and client

// GAP (Generic Access Profile) is how devices discover and connect to each other
// peripheral -> advertises and accepts
// broadcaster -> advertises
// observer -> scans and processes
// central -> scans and connects

// ATT (Attribute Protocol) is how GATT client and server communicate

// HCI is how to interact with bluetooth adapter directly.
// However, we want BlueZ to handle things like GATT/GAP for us

// Could also be part of a bluetooth mesh network

// Once connected to bus, get name starting with colon, e.g. :1.16 (bluetoothd has well known name org.bluez)
// Objects (/org/bluez/...) -> interfaces (org.bluez.GattManager1) -> methods
// Data returned is in another message
// interfaces can return signals (unprompted messages)
// interfaces also have properties which are interacted with Get() and Set() methods

// A proxy object emulates a remote object, and handles routing for us

// DBus messages also have data types

// Will have to explicitly allow connection to DBus bus in a configuration file

// could use the ELL (embedded linux library for use with DBUS)

// stackoverflow user: ukBaz

// systemctl status bluetooth (determine actual binary from /lib/systemd/system/bluetooth.service, so $(man bluetoothd). might have to pass --experimental?)
// $(sudo bluetoothctl; list; show; select; power on;)
// 'devices' command will list devices found through 'scan on/off'. then do a 'connect' to list peripheral characteristics.
// Once we are connected, will print out DBUS paths to the characteristics and services we want 
// (or just do 'menu gatt' then 'list-attributes <dev>')
// Now do 'select-attribute <dbus-path>'. Subsequently running 'attribute-info' will give UUID and flags
// DBUS tree view $(busctl tree org.bluez)
// Now could do 'write 0x12 0x23 ...'
// Finally run 'disconnect'
// For more informative error information use $(sudo btmgmt)
// Discover that by default bluetooth is soft blocked by rfkill $(sudo rfkill list), so $(sudo rfkill unblock bluetooth), 
// $(sudo systemctl restart bluetooth). For some reason this might require $(pulseaudio -k)

// bluetooth LE was a part of 4.0 specification 
// used for control signals or sensor data.

// Classic bluetooth requires a connection to send data. used for mice, keyboards, etc.
// Has fixed set of profiles to use, e.g. serial, audio, etc.

// Does BLE only advertise or can it connect directly?
// GATT is a hierarchy of data a device exposes (profile -> service -> characteristic (actual data))
// So when talking to a device, we are really dealing with a particular characteristic of a service
// So, we say our central looks at a peripherals GATT services that it exposes.
// TODO(Ryan): GATT is a type of service? Other types of services may include 'Tx Power', 'Battery Service'
// We will modify the characteristics of that service
// There are standard services that we would expect to find

// Interestingly, without extensions, bluetooth has no encryption or authentication so can just connect to any?

// BLE transmitter only on if being read or written to (so it just broadcasts data that others listen to?)
// subscribe to data changes in GATT database?
// e.g. GATT keys are 128bit UUIDs, 95DDA90-251D-470A-A062-FA1922DFA9A8
// peripheral advertises; central scans and connects
// also have Beacon (only transmitting) and Observer
// can create custom profiles (generic architecture; is this a gatt?)

// Special interest group has reserved values for official profiles
// Serial port profile referred to by 0x1101 (in actuality 128bits, just shortened because official)

// async by nature?

// pairing only for secure connection, not necessary

// little-endian except for beacons?
  
// dbus service (org.bluez), object path (/org/bluez/hci0)

  return 0;
}

#if 0

#define BT_BLUEZ_NAME "org.bluez"
#define BT_MANAGER_PATH "/"
#define BT_ADAPTER_INTERFACE    "org.bluez.Adapter1"
#define BT_DEVICE_IFACE     "org.bluez.Device1"
#define BT_MANAGER_INTERFACE "org.freedesktop.DBus.ObjectManager"
#define BT_PROPERTIES_INTERFACE "org.freedesktop.DBus.Properties"

int main(void)
{
    char *known_address = "2C:F0:A2:26:D7:F5"; /*This is your address to search */

        conn = g_bus_get_sync(G_BUS_TYPE_SYSTEM, NULL, &error);
        proxy =  g_dbus_proxy_new_sync(conn, G_DBUS_PROXY_FLAGS_NONE, NULL, BT_BLUEZ_NAME, BT_MANAGER_PATH, BT_MANAGER_INTERFACE, NULL, &err);
        result = g_dbus_proxy_call_sync(proxy, "GetManagedObjects", NULL, G_DBUS_CALL_FLAGS_NONE, -1, NULL, &err);

    g_variant_get(result, "(a{oa{sa{sv}}})", &iter);

        char *device_path = NULL;
        char device_address[18] = { 0 };
        /* Parse the signature:  oa{sa{sv}}} */
        while (g_variant_iter_loop(iter, "{&oa{sa{sv}}}", &device_path, NULL)) {
        {
            char address[BT_ADDRESS_STRING_SIZE] = { 0 };
            char *dev_addr;

            dev_addr = strstr(device_path, "dev_");
            if (dev_addr != NULL) {
                char *pos = NULL;
                dev_addr += 4;
                g_strlcpy(address, dev_addr, sizeof(address));

                while ((pos = strchr(address, '_')) != NULL) {
                    *pos = ':';
                }

                g_strlcpy(device_address, address, BT_ADDRESS_STRING_SIZE);
            }

        }

        if (g_strcmp0(known_address, device_address) == 0) {
            /* Find the name of the device */
            device_property_proxy = g_dbus_proxy_new_sync(conn, G_DBUS_PROXY_FLAGS_NONE, NULL, BT_BLUEZ_NAME, &device_path, BT_PROPERTIES_INTERFACE, NULL, NULL);
            result = g_dbus_proxy_call_sync(proxy, "Get", g_variant_new("(ss)", BT_DEVICE_IFACE, "Alias"), G_DBUS_CALL_FLAGS_NONE, -1, NULL, &error);

            const char *local = NULL;
            g_variant_get(result, "(v)", &temp);
            local = g_variant_get_string(temp, NULL);
            printf("Your desired name : %s\n", local);
        }
        }
}

#endif



#endif
