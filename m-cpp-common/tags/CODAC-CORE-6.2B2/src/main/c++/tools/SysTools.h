/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-common/tags/CODAC-CORE-6.2B2/src/main/c++/tools/SysTools.h $
* $Id: SysTools.h 101603 2019-08-19 11:31:33Z bauvirb $
*
* Project       : CODAC Core System
*
* Description   : Misc. system-level helper routines
*
* Author        : Bertrand Bauvir
*
* Copyright (c) : 2010-2019 ITER Organization,
*                                 CS 90 046
*                                 13067 St. Paul-lez-Durance Cedex
*                                 France
*
* This file is part of ITER CODAC software.
* For the terms and conditions of redistribution or use of this software
* refer to the file ITER-LICENSE.TXT located in the top level directory
* of the distribution package.
******************************************************************************/

#ifndef _SysTools_h
#define _SysTools_h

/**
 * @file SysTools.h
 * @brief Header file for various system-level helper routines
 * @date 14/09/2017
 * @author Bertrand Bauvir (IO)
 * @copyright 2010-2017 ITER Organization
 * @detail This header file contains the definition of system-level helper routines.
 */

// Global header files

#include <sys/types.h>
#include <sys/stat.h> // File properties, e.g. timestamp, etc.
#include <sys/statvfs.h> // Disk usage, etc.
#include <dirent.h>
#include <errno.h> // errno, EEXIST, etc.
#include <dlfcn.h> // dlopen
#include <stdlib.h> // strtol, getenv, etc.
#include <string.h> // strtok, etc.
#include <pthread.h> // pthread_self, etc.
#include <sched.h> // sched_getcpu, etc.
#include <unistd.h> // getpid, etc.

#ifdef __cplusplus
#include <string> // std::string
#include <sstream> // std::cout
#include <fstream> // std::ifstream
#endif // __cplusplus

// Local header files

#include "TimeTools.h" // Time-related helper tools (ccs::HelperTools::GetCurrentTime)

// Constants

// Type definition

#ifdef __cplusplus

namespace ccs {

namespace types {

typedef uint32 Endianness;

typedef struct {

  uint64  total;
  uint64  avail;
  float64 usage;

} ResourceStatistics;

// Global variables

static const Endianness BigEndian = 0u;
static const Endianness LittleEndian = 1u;
static const Endianness NetworkByteOrder = 0u;

} // namespace types

namespace HelperTools {

// Function declaration

// Function definition

/**
 * @brief Tests if the platform is big-endian.
 */

static inline bool IsBigEndian (void)
{

  ccs::types::uint16 value = 0x0001u;
  ccs::types::uint8* ref = reinterpret_cast<ccs::types::uint8*>(&value);
  
  return (*ref == 0x00u);

}

/**
 * @brief Tests if the platform is little-endian.
 */

static inline bool IsLittleEndian (void)
{

  ccs::types::uint16 value = 0x0001u;
  ccs::types::uint8* ref = reinterpret_cast<ccs::types::uint8*>(&value);
  
  return (*ref == 0x01u);

}

/**
 * @return The platform native endinanness, i.e. types::LittleEndian or types::BigEndian.
 */

static inline ccs::types::Endianness GetNativeByteOrder (void)
{ 

  ccs::types::Endianness endianness = ccs::types::LittleEndian;

  if (!IsLittleEndian())
    {
      endianness = ccs::types::BigEndian;
    }

  return endianness;

}

/**
 * @brief Converts char array to uppercase characters.
 * @return The modified char array.
 */

static inline ccs::types::char8* ToUppercase (ccs::types::char8 * const s)
{ 

  ccs::types::char8* p = s; 

  for (; *p; ++p)
    {
      *p = static_cast<ccs::types::char8>(toupper(static_cast<int>(*p)));
    }

  return s; 

}

/**
 * @brief Converts char array to lowercase characters.
 * @return The modified char array.
 */

static inline ccs::types::char8* ToLowercase (ccs::types::char8 * const s)
{ 

  ccs::types::char8* p = s; 

  for (; *p; ++p)
    {
      *p = static_cast<ccs::types::char8>(tolower(static_cast<int>(*p)));
    }

  return s; 

}

/**
 * @brief Performs a string comparison.
 * @return TRUE if strings are identical.
 */

static inline bool StringCompare (const ccs::types::char8 * const str1, const ccs::types::char8 * const str2, const ccs::types::int32 size = 0) 
{ 

  bool status = ((str1 != NULL_PTR_CAST(const ccs::types::char8 *)) && (str2 != NULL_PTR_CAST(const ccs::types::char8 *)));

  if (status)
    {
      if (0 == size)
        {
          status = (0 == strcmp(str1, str2));
        }
      else
        {
          status = (0 == strncmp(str1, str2, size));
        }
    }

  return status; 

}

/**
 * @brief Performs a safe string copy by ensuring the destination char array is zero terminated.
 * @return The destination char array.
 */

static inline ccs::types::char8* SafeStringCopy (ccs::types::char8 * const dst, const ccs::types::char8 * const src, const ccs::types::int32 size, const ccs::types::char8 trm = 0) 
{ 

  if ((dst != NULL_PTR_CAST(ccs::types::char8*)) && (src != NULL_PTR_CAST(const ccs::types::char8 *)) && (size > 0)) 
    { 
      (void)strncpy(dst, src, size); 
      dst[size-1] = 0; // Force buffer termination
    } 

  if (trm != 0)
    {
      ccs::types::char8* buf = dst;
      while ((*buf != 0) && (*buf != trm)) { buf++; } // Skip character
      *buf = 0; // Force buffer termination
    }

  return dst; 

}

/**
 * @brief Performs a safe string append operation by ensuring the destination char array is zero terminated.
 * @return The destination char array.
 */

static inline ccs::types::char8* SafeStringAppend (ccs::types::char8 * const dst, const ccs::types::char8 * const src, const ccs::types::int32 size, const ccs::types::char8 separator = '\0')
{ 

  ccs::types::int32 length = static_cast<ccs::types::int32>(strlen(dst));

  if ((dst != NULL_PTR_CAST(ccs::types::char8*)) && (src != NULL_PTR_CAST(const ccs::types::char8 *)) && (size > 0) && (size > (length + 1))) 
    {
      if ('\0' == separator)
        {
          (void)SafeStringCopy(dst+length, src, size-length);
        }
      else
        {
          ccs::types::char8* p_buf = dst+length;

          *p_buf = separator; 
          p_buf++; 
          *p_buf = 0; 
          length++;

          (void)SafeStringCopy(dst+length, src, size-length);
        }
    } 

  return dst; 

}

/**
 * @brief Creates folder on the file system.
 * @return TRUE if the folder exists.
 */

static inline bool CreatePath (const ccs::types::char8 * const path, mode_t mode = 0777)
{

  // Take a copy of the parameter - strtok replaces '/' by '/0'
  ccs::types::char8 cpy_path [PATH_MAX_LENGTH] = STRING_UNDEFINED; 
  ccs::types::char8 tmp_path [PATH_MAX_LENGTH] = STRING_UNDEFINED; 
  ccs::types::char8* p_path = tmp_path;

  (void)ccs::HelperTools::SafeStringCopy(cpy_path, path, ccs::types::MaxPathLength);

  if (cpy_path[0] == '/') // There is no starting '/' character
    {
      tmp_path[0] = '/'; 
      p_path++;
    }

  ccs::types::uint32 length = strlen(cpy_path);

  if (cpy_path[length-1] != '/') // There is no trailing '/' character
    {
      cpy_path[length] = '/';
      cpy_path[length+1] = 0;
    }

  ccs::types::char8* p_first = NULL_PTR_CAST(ccs::types::char8*);
  ccs::types::char8* p_curr = cpy_path;
  ccs::types::char8* p_next = NULL_PTR_CAST(ccs::types::char8*);

  // Clear process umask
  mode_t mask = umask(0);

  bool status = true;

  while (status && ((p_first = strtok_r(p_curr, "/", &p_next)) != NULL_PTR_CAST(ccs::types::char8*)))
    {
      p_curr = NULL_PTR_CAST(ccs::types::char8*);

      (void)snprintf(p_path, STRING_MAX_LENGTH, "%s/", p_first); p_path = tmp_path + strlen(tmp_path);

      status = ((mkdir(tmp_path, mode) == 0) || (errno == EEXIST));
    }

  // Restore process umask
  (void)umask(mask);

  return status;

}

/**
 * @brief Tests if the specified file exists on the file system.
 * @return TRUE if the file exists.
 */

static inline bool Exist (const ccs::types::char8 * const file) 
{ 

  FILE* file_handle = fopen(file, "r"); 

  bool status = (file_handle != NULL_PTR_CAST(FILE*)); 

  if (status) 
    { 
      (void)fclose(file_handle); 
    } 

  return status; 

}

static inline bool Exist (const ccs::types::char8 * const file_path, const ccs::types::char8 * const file_name) 
{ 

  ccs::types::char8 file [PATH_MAX_LENGTH]; 

  (void)snprintf(file, PATH_MAX_LENGTH, "%s/%s", file_path, file_name); 

  ccs::types::char8* buffer = file;

  return Exist(buffer); 

}

static inline bool Exist (const ccs::types::char8 * const file_path, const ccs::types::char8 * const file_prefix, const ccs::types::char8 * const file_suffix) 
{ 

  ccs::types::char8 file [PATH_MAX_LENGTH]; 

  (void)snprintf(file, PATH_MAX_LENGTH, "%s/%s%s", file_path, file_prefix, file_suffix); 

  ccs::types::char8* buffer = file;

  return Exist(buffer); 

}

static inline const ccs::types::char8* GetEnvironmentVariable (const ccs::types::char8 * const envvar) { return getenv(envvar); }
static inline bool IsVariableDefined (const ccs::types::char8 * const envvar) { return (GetEnvironmentVariable(envvar) != NULL_PTR_CAST(const ccs::types::char8*)); }

static inline bool GetEnvironmentVariable (const ccs::types::char8 * const envvar, ccs::types::char8 * const value, ccs::types::int32 size = STRING_MAX_LENGTH) 
{

  bool status = IsVariableDefined(envvar);

  if (status) 
    {
      status = (strlen(GetEnvironmentVariable(envvar)) < static_cast<ccs::types::uint32>(size)); // strlen does not count trailing '\0'
    }

  if (status)
    {
      (void)SafeStringCopy(value, GetEnvironmentVariable(envvar), size);
    }

  return status; 

}

static inline bool SetEnvironmentVariable (const ccs::types::char8 * const envvar, const ccs::types::char8 * const value) { return (setenv(envvar, value, 1) == 0); }
static inline bool ClearEnvironmentVariable (const ccs::types::char8 * const envvar) { return (unsetenv(envvar) == 0); }

static inline bool IsAlphaNumeric (ccs::types::char8 c)
{

  bool status = (((c >= '0') && (c <= '9')) ||
                 ((c >= 'A') && (c <= 'Z')) ||
                 ((c >= 'a') && (c <= 'z')));

  return status;

}

/**
 * @brief Tests if the char array is invalid or empty.
 * @return TRUE if the char array is invalid or empty.
 */

static inline bool IsUndefinedString (const ccs::types::char8 * const buffer) { return ((buffer == NULL_PTR_CAST(const ccs::types::char8 *)) || (*buffer == 0)); }

/**
 * @brief Tests is the string contains the specified token.
 * @return TRUE if the token is found in the string.
 */

static inline bool Contain (const ccs::types::char8 * const buffer, const ccs::types::char8 * const token, const ccs::types::uint32 offset = 0u)
{ 

  bool status = ((false == IsUndefinedString(buffer)) && 
                 (false == IsUndefinedString(token)) &&
                 (offset < strlen(buffer)));

  if (status)
    {
      std::string helper (buffer);
      status = (std::string::npos != helper.find(token, offset));
    }
  
  return status; 
  
}
 
/**
 * @brief Locate token in the string buffer.
 * @return Offset within buffer if successful, -1 otherwise.
 */
 
static inline ccs::types::int32 Find (const ccs::types::char8 * const buffer, const ccs::types::char8 * const token, const ccs::types::uint32 offset = 0u)
{ 

  bool status = ccs::HelperTools::Contain(buffer, token, offset);
  ccs::types::int32 ret = -1;
  
  if (status)
    {
      std::string helper (buffer);
      ret = static_cast<ccs::types::int32>(helper.find(token, offset));
    }
  
  return ret; 
  
}

/**
 * @brief Count number of tokens in the string buffer.
 * @return Number of token instances within buffer.
 */

static inline ccs::types::uint32 Count (const ccs::types::char8 * const buffer, const ccs::types::char8 * const token, const ccs::types::uint32 offset = 0u)
{ 

  ccs::types::uint32 ret = 0u;
  
  bool status = ccs::HelperTools::Contain(buffer, token, offset);
  
  if (status)
    {
      ccs::types::uint32 index = static_cast<ccs::types::uint32>(Find(buffer, token, offset));
      ret = 1u + ccs::HelperTools::Count(buffer, token, index + 1u);
    }
  
  return ret; 
  
}

/**
 * @brief Locate nth instance of the token in the string buffer.
 * @return Offset within buffer if successful, -1 otherwise.
 */

static inline ccs::types::int32 Locate (const ccs::types::char8 * const buffer, const ccs::types::char8 * const token, const ccs::types::uint32 instance = 1u)
{ 

  ccs::types::int32 ret = -1;
  
  bool status = ((0u != instance) &&
                 (instance <= ccs::HelperTools::Count(buffer, token)));
  
  if (status)
    {
      ccs::types::uint32 offset = 0u;
      ccs::types::uint32 count = instance;

      while (count != 0u)
        {
          ret = ccs::HelperTools::Find(buffer, token, offset);
          offset += ret + 1u;
          count -= 1u;
        }
    }

  return ret; 

}

/**
 * @brief Strip token out of the string buffer.
 * @return TRUE if token removed, FALSE otherwise.
 */

static inline bool Strip (ccs::types::char8 * const buffer, const ccs::types::char8 * const token)
{ 

  bool status = ((NULL_PTR_CAST(ccs::types::char8*) != buffer) &&
                 (true == Contain(buffer, token)));
  
  if (status)
    {
      std::string helper (buffer);
      (void)SafeStringCopy(buffer, "", helper.size());

      while (std::string::npos != helper.find(token))
        {
          (void)helper.erase(helper.find(token), strlen(token));
        }

      (void)SafeStringCopy(buffer, helper.c_str(), static_cast<ccs::types::uint32>(helper.size()) + 1u);
    }

  return status; 

}

static inline ccs::types::int32 FindMatchingBrace (const ccs::types::char8 * const buffer)
{

  ccs::types::int32 ret = -1;

  bool status = (false == IsUndefinedString(buffer));

  const ccs::types::char8 * p_buf = buffer;

  if (status)
    {
      status = ((*p_buf == '(') || (*p_buf == '[') || (*p_buf == '{') || (*p_buf == '<'));
    }

  if (status) // p_buf points to the opening brace
    {
      ccs::types::char8 brace;
      ccs::types::char8 close;
      ccs::types::uint32 count = 0u;

      ret = 0;
      brace = *p_buf;
      p_buf++; ret++; 
      count++;

      if (brace == '(')
        {
          close = ')';
        }

      if (brace == '[')
        {
          close = ']';
        }

      if (brace == '{')
        {
          close = '}';
        }

      if (brace == '<')
        {
          close = '>';
        }

      while (count > 0u)
        {
          while ((*p_buf != 0) && 
                 (*p_buf != '(') && (*p_buf != '[') && (*p_buf != '{') && (*p_buf != '<') &&
                 (*p_buf != ')') && (*p_buf != ']') && (*p_buf != '}') && (*p_buf != '>')) 
            { 
              p_buf++; ret++;
            }

          status = (*p_buf != 0);

          if (status) // p_buf points at a brace
            {
               if ((*p_buf == '(') || (*p_buf == '[') || (*p_buf == '{') || (*p_buf == '<')) 
                 {
                   count++;
                 }
               else
                 {
                   count--;
                 }

               if (count == 0u)
                 {
                   status = (*p_buf == close);
                 }
               else
                 {
                   p_buf++; ret++;
                 }
            }
        }
    }

  if (!status)
    {
      ret = -1;
    }

  return ret;

}

/**
 * @brief Tests if the char array refers to an environment variable.
 * @return TRUE if the char array is interpretable by reference to an environment variable.
 */

static inline bool IsVariableString (const ccs::types::char8 * const buffer) 
{ 

  bool status = (IsUndefinedString(buffer) == false);

  if (status)
    {
      status = (*buffer == '$');
    }

  return status; 

}

/**
 * @brief Converts an input char array to its value by reference to an environment variable.
 * @return TRUE is the output buffer is modified.
 */

static inline bool ResolveVariableString (const ccs::types::char8 * const in_str, ccs::types::char8 * const out_str, ccs::types::int32 size = STRING_MAX_LENGTH) 
{ 

  bool status = (IsUndefinedString(in_str) == false);

  if (status)
    {
      status = (IsVariableString(in_str) == true);
    }

  if (status)
    {
      ccs::types::char8 tmp_str [1024] = STRING_UNDEFINED; 
      (void)SafeStringCopy(tmp_str, in_str, 1024);

      // Extract environment variable name
      ccs::types::char8* p_suf = NULL_PTR_CAST(ccs::types::char8*);
      ccs::types::char8* p_env = strtok_r(tmp_str, "${}", &p_suf); // Modifies the buffer by inserting a zero for matched delimiter

      status = (IsVariableDefined(p_env) == true);

      if (status)
        {
          const ccs::types::char8* p_var = GetEnvironmentVariable(p_env);

          // Compose the resulting string
          if (*p_suf != 0)
            {
              (void)snprintf(out_str, size, "%s%s", p_var, p_suf); // WARNING - Suffix may be overwritten
            }
          else
            {
              (void)SafeStringCopy(out_str, p_var, size);
            }
        }
    }

  return status; 

}

/**
 * @brief Retrieves the name of the host.
 * @return TRUE if the buffer is updated to contain the host name.
 */

// Bug 10623 - Systemd does not provide environment variables
static inline bool GetHostName (ccs::types::char8 * const host_name, ccs::types::int32 size = STRING_MAX_LENGTH) { return (gethostname(host_name, size) == 0); }

/**
 * @brief Tests if the char array corresponds to an integer number.
 * @return TRUE if the char array is interpretable as an integer.
 */

static inline bool IsIntegerString (const ccs::types::char8 * const buffer)
{

  bool status = (IsUndefinedString(buffer) == false);

  if (status)
    {
      status = ((0 != isdigit(*buffer)) || (*buffer == '-') || (*buffer == '+'));
    }

  if (status)
    {
      // Try and convert the string in base 10
      ccs::types::char8* p = NULL_PTR_CAST(ccs::types::char8*);
      (void)strtol(buffer, &p, 10); // p would point to the first character not interpreted as part of a number
      status = (*p == '\0');
    }

  return status;

}

static inline ccs::types::int32 ToInteger (const ccs::types::char8 * const buffer)
{

  ccs::types::int32 ret = 0u; 

  bool status = (IsIntegerString(buffer) == true);

  if (status)
    {
      ccs::types::char8* p = NULL_PTR_CAST(ccs::types::char8*);
      ret = static_cast<ccs::types::int32>(strtol(buffer, &p, 10));
    }

  return ret;

}

/**
 * @brief Returns the <pid> of the process calling the method.
 * @return The <pid> of the process.
 */

static inline ccs::types::int32 GetProcessId (void) { return getpid(); } // This process

/**
 * @brief Retrieves the <pid> of a named process.
 * @detail This method parses the '/proc' file system to try and locate a '/proc/<pid>/stat'
 * file which contains the reference to the named process. The iterative search breaks as soon
 * as a match is found.
 * @return int <pid> of the named process, -1 if not found.
 * @since Thread-safety achieved with v1.4.7 (Bug 12069).
 */

static inline ccs::types::int32 GetProcessId (const ccs::types::char8 * const process) // Named process
{

  ccs::types::int32 pid = -1;

  DIR* dir_handle = opendir("/proc");

  bool status = (NULL_PTR_CAST(DIR*) != dir_handle);

  if (status)
    {
#if 0
      struct dirent * dir = NULL_PTR_CAST(struct dirent *);

      do
        {
          dir = readdir(dir_handle);

          if (!IsIntegerString(dir->d_name)) 
            {
              continue;
            }
          
          // The fodler corresponds to a pid
          
          std::string file_path = "/proc/"; file_path += dir->d_name; file_path += "/stat";
          std::ifstream file;
          
          file.open(file_path.c_str());
          
          std::string proc_id;
          std::string proc_name;
          
          getline(file, proc_id, ' ');
          getline(file, proc_name, ' ');
          
          file.close();
          
          if (proc_name.find(process) != std::string::npos) // Process found by name
            {
              pid = atoi(proc_id.c_str());
              break;
            }
        }
      while (NULL_PTR_CAST(struct dirent *) != dir);
#else
      struct dirent * res = NULL_PTR_CAST(struct dirent *);

      do
        {
          struct dirent dir; // Bug 12069 - Thread-safety issue with readdir
          status = (0 == readdir_r(dir_handle, &dir, &res));

          if (status && !IsIntegerString(dir.d_name)) 
            {
              continue;
            }
          
          // The fodler corresponds to a pid
          
          std::string file_path = "/proc/"; file_path += dir.d_name; file_path += "/stat";
          std::ifstream file;
          
          file.open(file_path.c_str());
          
          std::string proc_id;
          std::string proc_name;
          
          getline(file, proc_id, ' ');
          getline(file, proc_name, ' ');
          
          file.close();
          
          if (proc_name.find(process) != std::string::npos) // Process found by name
            {
              pid = atoi(proc_id.c_str());
              break;
            }
        }
      while (NULL_PTR_CAST(struct dirent *) != res);
#endif
    }

  (void)closedir(dir_handle);

  return pid;

}

/**
 * @brief Retrieves the name of the process calling the method.
 * @detail This method parses the '/proc/self/exe' link and identifies the process
 * name from the last element of the command line.
 * @return TRUE if the attribute is updated to contain the process name.
 */

static inline bool GetProcessName (ccs::types::char8 * const process, ccs::types::int32 size = STRING_MAX_LENGTH)
{

  ccs::types::char8 p_link [256] = STRING_UNDEFINED;

  bool status = (readlink("/proc/self/exe", p_link, 256u) > 0);

  if (status)
    {
      ccs::types::char8* p_first = NULL_PTR_CAST(ccs::types::char8*);
      ccs::types::char8* p_curr = p_link;
      ccs::types::char8* p_next = NULL_PTR_CAST(ccs::types::char8*);
      
      while (strtok_r(p_curr, "/", &p_next) != NULL_PTR_CAST(ccs::types::char8*))
        {
          p_first = p_curr;
          p_curr = p_next;
        }
      
      (void)SafeStringCopy(process, p_first, size);
    }

  return status;

}

/**
 * @brief Computes the process uptime.
 * @detail This method checks the '/proc/<pid>' properties to get folder
 * creation time and computes uptime by comparing to current system time.
 * @return Process uptime, 0 if <pid> is not valid.
 */

static inline ccs::types::uint32 GetProcessUptime (ccs::types::int32 pid)
{

  ccs::types::uint32 uptime = 0u;

  // Get properties
  std::stringstream ss; ss << pid;
  std::string dir_name = ss.str();
  std::string file_path = "/proc/"; file_path += dir_name.c_str();

  struct stat properties;

  bool status = (stat(file_path.c_str(), &properties) == 0);

  if (status)
    {
      ccs::types::uint32 start_time_sec = properties.st_mtime;
      ccs::types::uint32 curr_time_sec = static_cast<ccs::types::uint32>(GetCurrentTime() / 1000000000ul);
      
      uptime = curr_time_sec - start_time_sec;
    }

  return uptime;

}

static inline ccs::types::uint32 GetProcessUptime (const ccs::types::char8 * const process) { return GetProcessUptime(GetProcessId(process)); } // Named process
static inline ccs::types::uint32 GetProcessUptime (void) { return GetProcessUptime(GetProcessId()); } // This process

/**
 * @brief Computes the process used memory.
 * @detail This method checks the content of the '/proc/<pid>/status' file.
 * @return Process memory, 0 if <pid> is not valid.
 */

static inline ccs::types::uint64 GetProcessMemory (ccs::types::int32 pid)
{

  ccs::types::uint64 memory = 0ul;

  // Get status file path
  std::stringstream ss; ss << pid;
  std::string dir_name = ss.str();
  std::string file_path = "/proc/"; file_path += dir_name.c_str(); file_path += "/status";

  bool status = Exist(file_path.c_str());

  std::string result;
  std::string unit;

  if (status)
    {
      std::stringstream sstr; sstr << std::ifstream(file_path.c_str()).rdbuf();
      result = sstr.str();
      status = (std::string::npos != result.find("VmData"));
    }

  if (status)
    {
      result = result.substr(result.find("VmData"), std::string::npos);
      result = result.substr(0, result.find("\n")); // result should contain 'VmData:   <number> <unit>'
      result = result.substr(result.find(":")+1, std::string::npos); // result should contain '   <number> <unit>'
    }

  if (status)
    {
      unit = result.substr(result.find_last_of(' ')+1, std::string::npos);
    }

  if (status)
    {
      result = result.substr(0, result.find_last_of(' ')); // result should contain '   <number>'
      result = result.substr(result.find_last_of(' ')+1, std::string::npos); // result should contain '<number>'
    }

  if (status)
    {
      status = (sscanf(result.c_str(), "%lu", &memory) > 0);

      if (status && (unit == "kB"))
        {
          memory = memory * 1024ul;
        }
    }

  return memory;

}

static inline ccs::types::uint32 GetProcessMemory (const ccs::types::char8 * const process) { return GetProcessMemory(GetProcessId(process)); } // Named process
static inline ccs::types::uint32 GetProcessMemory (void) { return GetProcessMemory(GetProcessId()); } // This process

/**
 * @brief Computes the system used CPU time.
 * @return System CPU statistics.
 * @note The returned information is cumulative since system boot.
 */

static inline ccs::types::ResourceStatistics GetSystemCPUUsage (void)
{

  ccs::types::ResourceStatistics cpu = { 0ul, 0ul, 0.0 };

  bool status = true;

  std::string result;

  if (status)
    {
      std::stringstream sstr; 
      sstr << std::ifstream("/proc/stat").rdbuf();
      result = sstr.str();
      status = (std::string::npos != result.find("cpu"));
    }

  if (status)
    {
      result = result.substr(result.find("cpu"), std::string::npos);
      result = result.substr(0, result.find("\n"));
    }

  if (status) // Result should be something like 'cpu  <user> <nice> ..'
    {
      result = result.substr(result.find_first_of(' '), std::string::npos);
      while ((result[0] == ' ') || (result[0] == '\t')) 
        {
          result = result.substr(1, std::string::npos); // Skip spaces
        }
    }

  std::string value;

  for (ccs::types::uint32 index = 0u; (status && (index < 8u)); index++) // Result should be something like '<user> <nice> <system> <idle> <io> <irq> <softirq> <steal> .. <guest>'
    {
      value = result.substr(0, result.find_first_of(' '));
      result = result.substr(result.find_first_of(' ')+1, std::string::npos);

      if (index == 3u)
        {
          status = (sscanf(value.c_str(), "%lu", &(cpu.avail)) > 0);
        }

      ccs::types::uint64 total = 0ul;
      status = (sscanf(value.c_str(), "%lu", &total) > 0);

      if (status)
        {
          cpu.total += total;
        }
    }

  if (status)
    {
      cpu.usage = 100.0f * (static_cast<ccs::types::float64>(cpu.total - cpu.avail) / static_cast<ccs::types::float64>(cpu.total));
    }

  return cpu;

}

/**
 * @brief Computes the system used disk space.
 * @return System disk statistics.
 */

static inline ccs::types::ResourceStatistics GetSystemDiskUsage (void)
{

  ccs::types::ResourceStatistics disk = { 0ul, 0ul, 0.0 };

  struct statvfs fs_data;

  bool status = (0 == statvfs("/", &fs_data));

  if (status)
    {
      disk.total = fs_data.f_frsize*fs_data.f_blocks;
      disk.avail = fs_data.f_frsize*fs_data.f_bfree;
      disk.usage = 100.0f * (static_cast<ccs::types::float64>(disk.total - disk.avail) / static_cast<ccs::types::float64>(disk.total));
    }

  return disk;

}

/**
 * @brief Computes the system used memory.
 * @detail This method checks the content of the '/proc/meminfo' file.
 * @return System memory statistics.
 */

static inline ccs::types::ResourceStatistics GetSystemMemoryUsage (void)
{

  ccs::types::ResourceStatistics memory = { 0ul, 0ul, 0.0 };

  bool status = true;

  std::string result;

  if (status)
    {
      std::stringstream sstr; sstr << std::ifstream("/proc/meminfo").rdbuf();
      result = sstr.str();
    }

  ccs::types::uint32 found = 0u;

  while (status && (found < 2u))
    {
      status = (std::string::npos != result.find("\n"));

      std::string line;

      if (status)
        {
          line = result.substr(0, result.find("\n"));
          result = result.substr(result.find("\n")+1, std::string::npos);
        }

      std::string keyword;
      std::string value;
      std::string unit;

      if (status)
        {
          status = (std::string::npos != line.find(":"));
        }

      if (status)
        {
          keyword = line.substr(0, line.find(":"));
          line = line.substr(line.find(":")+1, std::string::npos);
        }

      if (status)
        {
          unit = line.substr(line.find_last_of(' ')+1, std::string::npos);
          line = line.substr(0, line.find_last_of(' '));
        }

      if (status)
        {
          value = line.substr(line.find_last_of(' ')+1, std::string::npos);
        }

      if (keyword == "MemAvailable")
        {
          status = (sscanf(value.c_str(), "%lu", &(memory.avail)) > 0);

          if (status && (unit == "kB"))
            {
              memory.avail *= 1024u;
            }

          found++;
        }
      else if (keyword == "MemTotal")
        {
          status = (sscanf(value.c_str(), "%lu", &(memory.total)) > 0);

          if (status && (unit == "kB"))
            {
              memory.total *= 1024u;
            }

          found++;
        }
      else
        {
          // Ignore
        }

    }

  if (status)
    {
      memory.usage = 100.0f * (static_cast<ccs::types::float64>(memory.total - memory.avail) / static_cast<ccs::types::float64>(memory.total));
    }

  return memory;

}

/**
 * @brief Tests if the specified thread has affinity to the specified core.
 * @detail This method also tests if the specified core is valid for the platform.
 * @return TRUE if pre-coditions are met and the affinity is set for the specified core.
 */

static inline bool HasAffinityToCore (pthread_t tid, ccs::types::int32 cid)
{

  cpu_set_t cpuset; 
  CPU_ZERO(&cpuset); 

  bool status = ((cid >= 0) && (cid < sysconf(_SC_NPROCESSORS_ONLN)));

  if (status)
    {
      status = (pthread_getaffinity_np(tid, sizeof(cpu_set_t), &cpuset) == 0);
    }

  if (status)
    {
      status = CPU_ISSET(cid, &cpuset);
    }
  
  return status;

}

/**
 * @brief Clears the affinity of the specified thread to the specified core.
 * @detail This method tests if the specified core is valid for the platform.
 * @return TRUE if pre-coditions are met and the cpu mask is successfully changed.
 */

static inline bool ClearAffinityToCore (pthread_t tid, ccs::types::int32 cid)
{

  cpu_set_t cpuset; 
  CPU_ZERO(&cpuset); 

  bool status = ((cid >= 0) && (cid < sysconf(_SC_NPROCESSORS_ONLN)));

  if (status)
    {
      status = (pthread_getaffinity_np(tid, sizeof(cpu_set_t), &cpuset) == 0);
    }

  if (status)
    {
      CPU_CLR(cid, &cpuset);
      status = (pthread_setaffinity_np(tid, sizeof(cpu_set_t), &cpuset) == 0);
    }
  
  return status;

}

/**
 * @brief Sets the affinity of the specified thread to the specified core.
 * @detail This method adds the specified core to the thread cpu mask. It 
 * also tests if the specified core is valid for the platform. It can be used to
 * modify the current mask (excl = false) or in exclusive mode (default) in which
 * case the thread is nailed to the core and none other.
 * @return TRUE if pre-coditions are met and the cpu mask is successfully changed.
 */

static inline bool SetAffinityToCore (pthread_t tid, ccs::types::int32 cid, bool excl = true)
{

  cpu_set_t cpuset; 
  CPU_ZERO(&cpuset); 

  bool status = ((cid >= 0) && (cid < sysconf(_SC_NPROCESSORS_ONLN)));

  if (status && (false == excl))
    {
      status = (pthread_getaffinity_np(tid, sizeof(cpu_set_t), &cpuset) == 0);
    }

  if (status)
    {
      CPU_SET(cid, &cpuset);
      status = (pthread_setaffinity_np(tid, sizeof(cpu_set_t), &cpuset) == 0);
    }
  
  return status;

}

/**
 * @brief Sets the scheduling policy and priority of the specified thread.
 * @detail This method changes the thread scheduling policy and priority. It tests
 * if the policy is SCHED_FIFO or SCHED_RR (Round-Robin) and if the priority is
 * greater than 0.
 * @return TRUE if pre-conditions are met and the scheduling parameters are
 * successfully changed.
 */

static inline bool SetPriority (pthread_t tid, ccs::types::int32 policy = SCHED_FIFO, ccs::types::int32 priority = 80)
{

  bool status = (priority > 0);

  if (status)
    {
      status = ((policy == SCHED_FIFO) || (policy == SCHED_RR));
    }

  if (status)
    {
      struct sched_param sp;
      // Assign scheduling policy and priority
      sp.sched_priority = priority;
      status = (pthread_setschedparam(tid, policy, &sp) == 0);
    }

  return status;

}

static inline ccs::types::int32 GetCurrentCore (void) { return sched_getcpu(); } // This thread
static inline pthread_t GetThreadId (void) { return pthread_self(); } // This thread

/**
 * @brief Tests if the calling thread has affinity to the specified core.
 * @detail This method also tests if the specified core is valid for the platform.
 * @return TRUE if pre-conditions are met and the affinity is set for the specified core.
 */

static inline bool HasAffinityToCore (ccs::types::int32 cid) { return HasAffinityToCore(GetThreadId(), cid); }

/**
 * @brief Clears the affinity of the calling thread to the specified core.
 * @detail This method tests if the specified core is valid for the platform.
 * @return TRUE if pre-conditions are met and the cpu mask is successfully changed.
 */

static inline bool ClearAffinityToCore (ccs::types::int32 cid) { return ClearAffinityToCore(GetThreadId(), cid); }

/**
 * @brief Sets the affinity of the calling thread to the specified core.
 * @detail This method tests if the specified core is valid for the platform.
 * @return TRUE if pre-conditions are met and the affinity is successfully changed.
 */

static inline bool SetAffinityToCore (ccs::types::int32 cid, bool excl = true) { return SetAffinityToCore(GetThreadId(), cid, excl); }

/**
 * @brief Sets the scheduling policy and priority of the calling thread.
 * @detail This method changes the thread scheduling policy and priority. It tests
 * if the policy is SCHED_FIFO or SCHED_RR (Round-Robin) and if the priority is
 * greater than 0.
 * @return TRUE if pre-conditions are met and the scheduling parameters are
 * successfully changed.
 */

static inline bool SetPriority (ccs::types::int32 policy = SCHED_FIFO, ccs::types::int32 priority = 80) { return SetPriority(GetThreadId(), policy, priority); }

static inline bool LoadSharedLibrary (const ccs::types::char8 * const library)
{

  bool status = !IsUndefinedString(library);

  if (status)
    {
      // Load library
      ccs::types::string lib_name; 
      (void)SafeStringCopy(lib_name, library, ccs::types::MaxStringLength); 
      void* lib_handle = dlopen(lib_name, RTLD_NOW);
      status = (NULL_PTR_CAST(void*) != lib_handle);
    }

  return status;

}

} // namespace HelperTools

} // namespace ccs

#endif // __cplusplus

#endif // _SysTools_h
