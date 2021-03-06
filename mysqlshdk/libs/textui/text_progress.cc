/*
 * Copyright (c) 2018, Oracle and/or its affiliates. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2.0,
 * as published by the Free Software Foundation.
 *
 * This program is also distributed with certain software (including
 * but not limited to OpenSSL) that is licensed under separate terms, as
 * designated in a particular file or component or in included license
 * documentation.  The authors of MySQL hereby grant you an additional
 * permission to link the program and your derivative works with the
 * separately licensed software that they have included with MySQL.
 * This program is distributed in the hope that it will be useful,  but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
 * the GNU General Public License, version 2.0, for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "mysqlshdk/libs/textui/text_progress.h"

#ifdef _WIN32
#include <io.h>
#define fileno _fileno
#define write _write
#else
#include <unistd.h>
#endif

#include <chrono>

#include "mysqlshdk/include/shellcore/console.h"
#include "mysqlshdk/libs/utils/strformat.h"

namespace mysqlshdk {
namespace textui {

Throughput::Throughput() {
  auto current_time = std::chrono::steady_clock::now();
  for (auto &s : m_history) {
    s.bytes = 0;
    s.time = current_time;
  }
}

uint64_t Throughput::rate() const {
  const auto current_point = m_history[(m_index + k_moving_average_points - 1) %
                                       k_moving_average_points];
  const auto past_point = m_history[(m_index + 1) % k_moving_average_points];

  const auto bytes_diff = current_point.bytes - past_point.bytes;
  const auto time_diff = std::chrono::duration_cast<std::chrono::milliseconds>(
                             current_point.time - past_point.time)
                             .count();
  if (time_diff == 0) {
    return bytes_diff * 1000;
  }
  return bytes_diff * 1000 / time_diff;  // bytes per second
}

void Text_progress::show_status(bool force) {
  const auto current_time = std::chrono::steady_clock::now();
  const auto time_diff = std::chrono::duration_cast<std::chrono::milliseconds>(
                             current_time - m_refresh_clock)
                             .count();
  const bool refresh_timeout = time_diff >= 500;  // update status every 500ms
  if ((refresh_timeout && m_changed) || force) {
    render_status();
    m_changed = false;
    const auto status_printable_size =
        m_status.size() - 1;  // -1 because of leading '\r'
    if (status_printable_size < m_last_status_size) {
      clear_status();
    }
    write(fileno(stderr), &m_status[0], m_status.size());
    m_last_status_size = status_printable_size;
    m_refresh_clock = current_time;
  }
}

void Text_progress::clear_status() {
  std::string space(m_last_status_size + 2, ' ');
  space[0] = '\r';
  space[m_last_status_size + 1] = '\r';
  write(fileno(stderr), &space[0], space.size());
}

void Text_progress::shutdown() { write(fileno(stderr), "\n", 1); }

void Text_progress::render_status() {
  // 100% (1024.00 MB / 1024.00 MB), 1024.00 MB/s
  m_status.clear();
  m_status +=
      "\r" + std::to_string(percent()) + "% (" +
      mysqlshdk::utils::format_bytes(m_current) + " / " +
      mysqlshdk::utils::format_bytes(m_total) + "), " +
      mysqlshdk::utils::format_throughput_bytes(m_throughput.rate(), 1.0);
}

void Json_progress::show_status(bool force) {
  const auto current_time = std::chrono::steady_clock::now();
  const auto time_diff = std::chrono::duration_cast<std::chrono::milliseconds>(
                             current_time - m_refresh_clock)
                             .count();
  const bool refresh_timeout = time_diff >= 2000;  // update status every 2000ms
  if ((refresh_timeout && m_changed) || force) {
    render_status();
    m_changed = false;
    mysqlsh::current_console()->raw_print(m_status,
                                          mysqlsh::Output_stream::STDOUT, true);
    m_refresh_clock = current_time;
  }
}

void Json_progress::render_status() {
  // 100% (1024.00 MB / 1024.00 MB), 1024.00 MB/s
  // todo(kg): build proper json doc
  m_status.clear();
  m_status +=
      std::to_string(percent()) + "% (" +
      mysqlshdk::utils::format_bytes(m_current) + " / " +
      mysqlshdk::utils::format_bytes(m_total) + "), " +
      mysqlshdk::utils::format_throughput_bytes(m_throughput.rate(), 1.0);
}

}  // namespace textui
}  // namespace mysqlshdk
