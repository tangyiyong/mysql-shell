/* Copyright (c) 2015, 2017, Oracle and/or its affiliates. All rights reserved.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA */

#include <fstream>
#include <list>
#include <memory>
#include <mutex>
#include <set>
#include <string>
#include <thread>
#include <typeinfo>
#include <utility>
#include <vector>
#include "gtest_clean.h"
#include "scripting/common.h"
#include "scripting/lang_base.h"
#include "shellcore/shell_core.h"
#include "shellcore/shell_notifications.h"
#include "src/mysqlsh/mysql_shell.h"
#include "unittest/test_utils/shell_base_test.h"
#include "shellcore/base_session.h"

#ifndef UNITTEST_TEST_UTILS_H_
#define UNITTEST_TEST_UTILS_H_

#define EXPECT_BECOMES_TRUE(timeout, pred)    \
  do {                                        \
    auto t = time(nullptr);                   \
    bool ok = false;                          \
    while (time(nullptr) - t < (timeout)) {   \
      if ((pred)) {                           \
        ok = true;                            \
        break;                                \
      }                                       \
    }                                         \
    if (!ok)                                  \
      FAIL() << "Timeout waiting for " #pred; \
  } while (0)

#define EXPECT_THROW_NOTHING(expr)                                \
  try {                                                           \
    expr;                                                         \
  } catch (std::exception & e) {                                  \
    ADD_FAILURE() << "Expected no exception thrown by " #expr     \
                  << typeid(e).name() << ":" << e.what() << "\n"; \
  } catch (...) {                                                 \
    ADD_FAILURE() << "Expected no exception thrown by " #expr     \
                  << " but got something\n";                      \
  }

namespace testing {
// Fake deleter for shared pointers to avoid they attempt deleting the passed
// object
struct SharedDoNotDelete {
  template <typename T>
  void operator()(T *) {}
};
}  // namespace testing

inline std::string makered(const std::string &s) {
  if (!getenv("COLOR_DEBUG"))
    return s;
  return "\e[31m" + s + "\e[0m";
}

inline std::string makeredbg(const std::string &s) {
  if (!getenv("COLOR_DEBUG"))
    return s;
  return "\e[41m" + s + "\e[0m";
}

inline std::string makeblue(const std::string &s) {
  if (!getenv("COLOR_DEBUG"))
    return s;
  return "\e[36m" + s + "\e[0m";
}

inline std::string makegreen(const std::string &s) {
  if (!getenv("COLOR_DEBUG"))
    return s;
  return "\e[32m" + s + "\e[0m";
}

inline std::string makeyellow(const std::string &s) {
  if (!getenv("COLOR_DEBUG"))
    return s;
  return "\e[33m" + s + "\e[0m";
}

class Shell_test_output_handler {
 public:
  // You can define per-test set-up and tear-down logic as usual.
  Shell_test_output_handler();
  ~Shell_test_output_handler();

  virtual void TearDown() {
  }

  static void deleg_print(void *user_data, const char *text);
  static void deleg_print_error(void *user_data, const char *text);
  static void deleg_print_value(void *user_data, const char *text);
  static shcore::Prompt_result deleg_prompt(void *user_data,
                                            const char *UNUSED(prompt),
                                            std::string *ret);
  static shcore::Prompt_result deleg_password(void *user_data,
                                              const char *UNUSED(prompt),
                                              std::string *ret);

  void wipe_out() {
    std::lock_guard<std::mutex> lock(stdout_mutex);
    std_out.clear();
  }

  void wipe_err() {
    std_err.clear();
  }
  void wipe_log() {
    log.clear();
  }
  void wipe_all() {
    wipe_out();
    std_err.clear();
  }

  bool grep_stdout_thread_safe(const std::string &text) {
    std::lock_guard<std::mutex> lock(stdout_mutex);
    return (std_out.find(text) != std::string::npos);
  }

  shcore::Interpreter_delegate deleg;
  std::string std_err;
  std::string std_out;
  std::stringstream full_output;
  std::mutex stdout_mutex;
  static std::vector<std::string> log;

  void set_log_level(ngcommon::Logger::LOG_LEVEL log_level) {
    _logger->set_log_level(log_level);
  }

  ngcommon::Logger::LOG_LEVEL get_log_level() {
    return _logger->get_log_level();
  }

  void validate_stdout_content(const std::string &content, bool expected);
  void validate_stderr_content(const std::string &content, bool expected);
  void validate_log_content(const std::vector<std::string> &content,
                            bool expected);
  void validate_log_content(const std::string &content, bool expected);

  void debug_print(const std::string &line);
  void debug_print_header(const std::string &line);
  void flush_debug_log();
  void whipe_debug_log() {
    full_output.clear();
  }

  bool debug;

  void feed_to_prompt(const std::string &line) {
    prompts.push_back(line);
  }

  std::list<std::string> prompts;
  std::list<std::string> passwords;

 protected:
  static ngcommon::Logger *_logger;

  static void log_hook(const char *message, ngcommon::Logger::LOG_LEVEL level,
                       const char *domain);
};

#define MY_EXPECT_STDOUT_CONTAINS(x)                 \
  do {                                               \
    SCOPED_TRACE("...in stdout check\n");            \
    output_handler.validate_stdout_content(x, true); \
  } while (0)
#define MY_EXPECT_STDERR_CONTAINS(x)                 \
  do {                                               \
    SCOPED_TRACE("...in stderr check\n");            \
    output_handler.validate_stderr_content(x, true); \
  } while (0)
#define MY_EXPECT_LOG_CONTAINS(x)                 \
  do {                                            \
    SCOPED_TRACE("...in log check\n");            \
    output_handler.validate_log_content(x, true); \
  } while (0)
#define MY_EXPECT_STDOUT_NOT_CONTAINS(x)              \
  do {                                                \
    SCOPED_TRACE("...in stdout check\n");             \
    output_handler.validate_stdout_content(x, false); \
  } while (0)
#define MY_EXPECT_STDERR_NOT_CONTAINS(x)              \
  do {                                                \
    SCOPED_TRACE("...in stderr check\n");             \
    output_handler.validate_stderr_content(x, false); \
  } while (0)
#define MY_EXPECT_LOG_NOT_CONTAINS(x)              \
  do {                                             \
    SCOPED_TRACE("...in log check\n");             \
    output_handler.validate_log_content(x, false); \
  } while (0)

class Shell_core_test_wrapper : public tests::Shell_base_test,
                                public shcore::NotificationObserver {
 protected:
  // You can define per-test set-up and tear-down logic as usual.
  virtual void SetUp();
  virtual void TearDown();
  virtual void set_defaults() {
  }
  virtual ::testing::TestInfo *info() {
    return nullptr;
  }
  virtual std::string context_identifier();

  void observe_session_notifications();
  void ignore_session_notifications();

  std::string _custom_context;

  // void process_result(shcore::Value result);
  void execute(const std::string &code);
  void execute_noerr(const std::string &code);
  void exec_and_out_equals(const std::string &code, const std::string &out = "",
                           const std::string &err = "");
  void exec_and_out_contains(const std::string &code,
                             const std::string &out = "",
                             const std::string &err = "");

  virtual void handle_notification(const std::string &name,
                                   const shcore::Object_bridge_ref &sender,
                                   shcore::Value::Map_type_ref data);

  // This can be use to reinitialize the interactive shell with different
  // options First set the options on _options
  void reset_options() {
    extern char *g_mppath;
    _opts.reset(new mysqlsh::Shell_options());
    _options = const_cast<mysqlsh::Shell_options::Storage*>(&_opts->get());
    _options->gadgets_path = g_mppath;
    _options->db_name_cache = false;
  }

  bool debug;
  void enable_debug() {
    debug = true;
    output_handler.debug = true;
  }
  virtual void set_options() {
  }

  virtual void reset_shell() {
    _interactive_shell.reset(
        new mysqlsh::Mysql_shell(_opts, &output_handler.deleg));

    _interactive_shell->finish_init();
    set_defaults();
  }

  void connect_classic();
  void connect_x();

  Shell_test_output_handler output_handler;
  std::shared_ptr<mysqlsh::Mysql_shell> _interactive_shell;
  std::shared_ptr<mysqlsh::Shell_options> _opts;
  mysqlsh::Shell_options::Storage* _options;
  void wipe_out() {
    output_handler.wipe_out();
  }
  void wipe_err() {
    output_handler.wipe_err();
  }
  void wipe_log() {
    output_handler.wipe_log();
  }
  void wipe_all() {
    output_handler.wipe_all();
  }

 private:
  std::map<shcore::Object_bridge_ref, std::string> _open_sessions;
};

// Helper class to ease the creation of tests on the CRUD operations
// Specially on the chained methods
class Crud_test_wrapper : public ::Shell_core_test_wrapper {
 protected:
  std::set<std::string> _functions;

  // Sets the functions that will be available for chaining
  // in a CRUD operation
  void set_functions(const std::string &functions);

  // Validates only the specified functions are available
  // non listed functions are validated for unavailability
  void ensure_available_functions(const std::string &functions);
};

#endif  // UNITTEST_TEST_UTILS_H_
