/*
 * Copyright (c) 2017, 2019, Oracle and/or its affiliates. All rights reserved.
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

#ifndef MYSQLSHDK_LIBS_UTILS_UTILS_LEXING_H_
#define MYSQLSHDK_LIBS_UTILS_UTILS_LEXING_H_

#include <cassert>
#include <cctype>
#include <stdexcept>
#include <string>

namespace mysqlshdk {
namespace utils {

namespace internal {
#ifdef _WIN32
#define MSVC_WORKAROUND extern const __declspec(selectany)
#else
#define MSVC_WORKAROUND constexpr
#endif

// lookup table of number of bytes to skip when spanning a quoted string
// (single quote and double quote versions)
// skip 2 for escape, 0 for '\0', 0 for end quote and 1 for the rest
MSVC_WORKAROUND char k_quoted_string_span_skips_sq[256] = {
    0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

MSVC_WORKAROUND char k_quoted_string_span_skips_dq[256] = {
    0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

MSVC_WORKAROUND char k_keyword_chars[] =
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_";
}  // namespace internal

/*
 * Fast scanning of a quoted string (_dq for " or _sq for ') until the closing
 * quote or end of string.
 * Escaped quotes ("foo\"bar") are handled.
 * Double quoting ("foo""bar")is not handled, but can be handled by calling
 * this twice.
 *
 * Note that this will not process escapes or modify the input, it simply finds
 * the boundary of the string.
 *
 * @param s the string to be lexed. The first character must be a quote char.
 * @return offset after the end of the closing quote or npos on error
 */
inline size_t span_quoted_string_dq(const std::string &s, size_t offset) {
  assert(!s.empty());
  assert(offset < s.length());
  assert(s[offset] == '"');
  assert(s[s.size()] == '\0');

  // skip opening quote
  ++offset;

  unsigned char last_ch = 0;
  for (;;) {
    // Tight-loop to span until terminating 0 or closing quote
    while (offset < s.length() &&
           internal::k_quoted_string_span_skips_dq
                   [last_ch = static_cast<unsigned char>(s[offset])] > 0) {
      offset += internal::k_quoted_string_span_skips_dq[last_ch];
    }
    // Now check why we exited the loop
    if (last_ch == '\0' || offset >= s.length()) {
      // there was a '\0', but the string is not over, continue spanning
      if (offset < s.length()) {
        ++offset;
        continue;
      }
      // if this was indeed the terminator, it means we exited the loop
      // without seeing a "
      return std::string::npos;
    }
    // the only other possible way to exit the loop must be the end quote
    assert(last_ch == '"');
    return offset + 1;
  }
}

inline size_t span_quoted_string_sq(const std::string &s, size_t offset) {
  assert(!s.empty());
  assert(offset < s.length());
  assert(s[offset] == '\'');
  assert(s[s.size()] == '\0');

  // skip opening quote
  ++offset;

  unsigned char last_ch = 0;
  for (;;) {
    // Tight-loop to span until terminating 0 or closing quote
    while (offset < s.length() &&
           internal::k_quoted_string_span_skips_sq
                   [last_ch = static_cast<unsigned char>(s[offset])] > 0) {
      offset += internal::k_quoted_string_span_skips_sq[last_ch];
    }
    // Now check why we exited the loop
    if (last_ch == '\0' || offset >= s.length()) {
      // there was a '\0', but the string is not over, continue spanning
      if (offset < s.length()) {
        ++offset;
        continue;
      }
      // if this was indeed the terminator, it means we exited the loop
      // without seeing a '
      return std::string::npos;
    }
    // the only other possible way to exit the loop must be the end quote
    assert(last_ch == '\'');
    return offset + 1;
  }
}

inline size_t span_quoted_sql_identifier_bt(const std::string &s,
                                            size_t offset) {
  assert(!s.empty());
  assert(offset < s.length());
  assert(s[offset] == '`');
  assert(s[s.size()] == '\0');

  // unlike strings, identifiers don't allow escaping with the \ char
  size_t p = offset + 1;
  for (;;) {
    p = s.find('`', p);
    if (p == std::string::npos) {
      break;
    }
    if (s[p + 1] == '`') {
      p += 2;
    } else {
      ++p;
      break;
    }
  }
  return p;
}

// Span spaces. If offset is npos, return npos
inline size_t span_spaces(const std::string &s, size_t offset) {
  return s.find_first_not_of(" \t\r\n", offset);
}

inline size_t span_not_spaces(const std::string &s, size_t offset) {
  size_t p = s.find_first_of(" \t\r\n", offset);
  if (p == std::string::npos) return s.size();
  return p;
}

inline size_t span_quoted_sql_identifier_dquote(const std::string &s,
                                                size_t offset) {
  assert(!s.empty());
  assert(offset < s.length());
  assert(s[offset] == '"');
  assert(s[s.size()] == '\0');

  // unlike strings, identifiers don't allow escaping with the \ char
  size_t p = offset + 1;
  for (;;) {
    p = s.find('"', p);
    if (p == std::string::npos) {
      break;
    }
    if (s[p + 1] == '"') {
      p += 2;
    } else {
      ++p;
      break;
    }
  }
  return p;
}

inline size_t span_keyword(const std::string &s, size_t offset) {
  assert(!s.empty());
  assert(offset < s.length());

  if (std::isalpha(s[offset]) || s[offset] == '_') {
    size_t p = s.find_first_not_of(internal::k_keyword_chars, offset + 1);
    if (p == std::string::npos) p = s.length();
    return p;
  } else {
    return offset;
  }
}

inline size_t span_to_eol(const std::string &s, size_t offset) {
  offset = s.find('\n', offset);
  if (offset == std::string::npos) return s.length();
  return offset + 1;
}

inline size_t span_cstyle_comment(const std::string &s, size_t offset) {
  assert(!s.empty());
  assert(s[offset] == '/' && s[offset + 1] == '*');

  offset += 2;
  size_t p = s.find("*/", offset);
  if (p == std::string::npos) return std::string::npos;
  return p + 2;
}

/** Class enabling iteration over characters in SQL string skipping comments and
 * quoted strings.
 *
 * This iterator casts implicitly to size_t and can be used as substitute for
 * size_t in standard for loop iterating over std::string.
 */
class SQL_string_iterator {
 public:
  typedef std::string::value_type value_type;

  /** Create SQL_string_iterator.
   *
   * @arg str string containing SQL query.
   * @arg offset offset in str, need to point to valid part of SQL.
   */
  explicit SQL_string_iterator(const std::string &str,
                               std::string::size_type offset = 0);

  SQL_string_iterator &operator++();

  SQL_string_iterator operator++(int) {
    SQL_string_iterator ans = *this;
    ++(*this);
    return ans;
  }

  void advance() { ++(*this); }

  std::string::value_type get_char() const {
    assert(m_offset < m_s.length());
    return m_s[m_offset];
  }

  std::string::value_type operator*() const { return get_char(); }

  bool operator==(const SQL_string_iterator &a) const {
    return m_offset == a.m_offset && m_s == a.m_s;
  }

  bool operator!=(const SQL_string_iterator &a) const { return !(*this == a); }

  operator std::string::size_type() const { return m_offset; }

  std::string::size_type position() const { return m_offset; }

  void set_position(std::string::size_type position) { m_offset = position; }

  /** Is iterator pointing to valid character inside SQL string */
  bool valid() const { return m_offset < m_s.length(); }

  std::string get_next_sql_token();

 private:
  const std::string &m_s;
  std::string::size_type m_offset;
};

}  // namespace utils
}  // namespace mysqlshdk

#endif  // MYSQLSHDK_LIBS_UTILS_UTILS_LEXING_H_
