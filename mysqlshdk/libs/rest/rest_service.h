/*
 * Copyright (c) 2019, Oracle and/or its affiliates. All rights reserved.
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

#ifndef MYSQLSHDK_LIBS_REST_REST_SERVICE_H_
#define MYSQLSHDK_LIBS_REST_REST_SERVICE_H_

#include <future>
#include <memory>
#include <string>

#include "mysqlshdk/include/scripting/types.h"

#include "mysqlshdk/libs/rest/authentication.h"
#include "mysqlshdk/libs/rest/error.h"
#include "mysqlshdk/libs/rest/headers.h"
#include "mysqlshdk/libs/rest/response.h"

namespace mysqlshdk {
namespace rest {

/**
 * A REST service. By default, requests will follow redirections and
 * keep the connections alive.
 *
 * This is a move-only type.
 */
class Rest_service {
 public:
  /**
   * Constructs an object which is going to handle requests to the REST service
   * located at the specified base URL.
   *
   * @param base_url Base URL of the REST service.
   * @param verify_ssl If false, disables the SSL verification of target host.
   *        Validity of SSL certificates is not going to be checked.
   */
  explicit Rest_service(const std::string &base_url, bool verify_ssl = true);

  Rest_service(const Rest_service &) = delete;
  Rest_service(Rest_service &&);

  Rest_service &operator=(const Rest_service &) = delete;
  Rest_service &operator=(Rest_service &&);

  ~Rest_service();

  /**
   * Sets the Basic authentication to be used when executing the requests.
   *
   * @param basic Authentication details.
   *
   * @returns Reference to self.
   */
  Rest_service &set(const Basic_authentication &basic);

  /**
   * Sets the HTTP headers to be used when executing each request.
   *
   * @param headers HTTP headers.
   *
   * @returns Reference to self.
   */
  Rest_service &set_default_headers(const Headers &headers);

  /**
   * Sets the maximum time in milliseconds the execution of a request can take.
   * By default, timeout is set to two seconds. Use 0 for an infinite timeout.
   *
   * @param timeout Maximum allowed time for the execution.
   *
   * @returns Reference to self.
   */
  Rest_service &set_timeout(uint32_t timeout);

  /**
   * Executes a GET request, blocks until response is available.
   *
   * @param path Path to the request, it is going to be appended to the base
   *        URL.
   * @param headers Optional request-specific headers. If default headers were
   *        also specified, request-specific headers are going to be appended
   *        that set, overwriting any duplicated values.
   *
   * @returns Received response.
   *
   * @throws Connection_error In case of any connection-related problems.
   */
  Response get(const std::string &path, const Headers &headers = {});

  /**
   * Executes a HEAD request, blocks until response is available.
   *
   * @param path Path to the request, it is going to be appended to the base
   *        URL.
   * @param headers Optional request-specific headers. If default headers were
   *        also specified, request-specific headers are going to be appended
   *        that set, overwriting any duplicated values.
   *
   * @returns Received response.
   *
   * @throws Connection_error In case of any connection-related problems.
   */
  Response head(const std::string &path, const Headers &headers = {});

  /**
   * Executes a POST request, blocks until response is available.
   *
   * @param path Path to the request, it is going to be appended to the base
   *        URL.
   * @param body Optional body which is going to be sent along with the request.
   *        Content-Type is going to be set to 'application/json', unless it is
   *        explicitly set in headers.
   * @param headers Optional request-specific headers. If default headers were
   *        also specified, request-specific headers are going to be appended
   *        that set, overwriting any duplicated values.
   *
   * @returns Received response.
   *
   * @throws Connection_error In case of any connection-related problems.
   */
  Response post(const std::string &path, const shcore::Value &body = {},
                const Headers &headers = {});

  /**
   * Executes a PUT request, blocks until response is available.
   *
   * @param path Path to the request, it is going to be appended to the base
   *        URL.
   * @param body Optional body which is going to be sent along with the request.
   *        Content-Type is going to be set to 'application/json', unless it is
   *        explicitly set in headers.
   * @param headers Optional request-specific headers. If default headers were
   *        also specified, request-specific headers are going to be appended
   *        that set, overwriting any duplicated values.
   *
   * @returns Received response.
   *
   * @throws Connection_error In case of any connection-related problems.
   */
  Response put(const std::string &path, const shcore::Value &body = {},
               const Headers &headers = {});

  /**
   * Executes a PATCH request, blocks until response is available.
   *
   * @param path Path to the request, it is going to be appended to the base
   *        URL.
   * @param body Optional body which is going to be sent along with the request.
   *        Content-Type is going to be set to 'application/json', unless it is
   *        explicitly set in headers.
   * @param headers Optional request-specific headers. If default headers were
   *        also specified, request-specific headers are going to be appended
   *        that set, overwriting any duplicated values.
   *
   * @returns Received response.
   *
   * @throws Connection_error In case of any connection-related problems.
   */
  Response patch(const std::string &path, const shcore::Value &body = {},
                 const Headers &headers = {});

  /**
   * Executes a DELETE request, blocks until response is available.
   *
   * @param path Path to the request, it is going to be appended to the base
   *        URL.
   * @param body Optional body which is going to be sent along with the request.
   *        Content-Type is going to be set to 'application/json', unless it is
   *        explicitly set in headers.
   * @param headers Optional request-specific headers. If default headers were
   *        also specified, request-specific headers are going to be appended
   *        that set, overwriting any duplicated values.
   *
   * @returns Received response.
   *
   * @throws Connection_error In case of any connection-related problems.
   */
  Response delete_(const std::string &path, const shcore::Value &body = {},
                   const Headers &headers = {});

  /**
   * Asynchronously executes a GET request. This object must not be modified
   * nor any other request can be executed again until response is received.
   *
   * @param path Path to the request, it is going to be appended to the base
   *        URL.
   * @param headers Optional request-specific headers. If default headers were
   *        also specified, request-specific headers are going to be appended
   *        that set, overwriting any duplicated values.
   *
   * @returns Received response.
   *
   * @throws Connection_error In case of any connection-related problems. This
   *         method does not throw on its own, exception could be thrown from
   *         future object.
   */
  std::future<Response> async_get(const std::string &path,
                                  const Headers &headers = {});

  /**
   * Asynchronously executes a HEAD request. This object must not be modified
   * nor any other request can be executed again until response is received.
   *
   * @param path Path to the request, it is going to be appended to the base
   *        URL.
   * @param headers Optional request-specific headers. If default headers were
   *        also specified, request-specific headers are going to be appended
   *        that set, overwriting any duplicated values.
   *
   * @returns Received response.
   *
   * @throws Connection_error In case of any connection-related problems. This
   *         method does not throw on its own, exception could be thrown from
   *         future object.
   */
  std::future<Response> async_head(const std::string &path,
                                   const Headers &headers = {});

  /**
   * Asynchronously executes a POST request. This object must not be modified
   * nor any other request can be executed again until response is received.
   *
   * @param path Path to the request, it is going to be appended to the base
   *        URL.
   * @param body Optional body which is going to be sent along with the request.
   *        Content-Type is going to be set to 'application/json', unless it is
   *        explicitly set in headers.
   * @param headers Optional request-specific headers. If default headers were
   *        also specified, request-specific headers are going to be appended
   *        that set, overwriting any duplicated values.
   *
   * @returns Received response.
   *
   * @throws Connection_error In case of any connection-related problems. This
   *         method does not throw on its own, exception could be thrown from
   *         future object.
   */
  std::future<Response> async_post(const std::string &path,
                                   const shcore::Value &body = {},
                                   const Headers &headers = {});

  /**
   * Asynchronously executes a PUT request. This object must not be modified
   * nor any other request can be executed again until response is received.
   *
   * @param path Path to the request, it is going to be appended to the base
   *        URL.
   * @param body Optional body which is going to be sent along with the request.
   *        Content-Type is going to be set to 'application/json', unless it is
   *        explicitly set in headers.
   * @param headers Optional request-specific headers. If default headers were
   *        also specified, request-specific headers are going to be appended
   *        that set, overwriting any duplicated values.
   *
   * @returns Received response.
   *
   * @throws Connection_error In case of any connection-related problems. This
   *         method does not throw on its own, exception could be thrown from
   *         future object.
   */
  std::future<Response> async_put(const std::string &path,
                                  const shcore::Value &body = {},
                                  const Headers &headers = {});

  /**
   * Asynchronously executes a PATCH request. This object must not be modified
   * nor any other request can be executed again until response is received.
   *
   * @param path Path to the request, it is going to be appended to the base
   *        URL.
   * @param body Optional body which is going to be sent along with the request.
   *        Content-Type is going to be set to 'application/json', unless it is
   *        explicitly set in headers.
   * @param headers Optional request-specific headers. If default headers were
   *        also specified, request-specific headers are going to be appended
   *        that set, overwriting any duplicated values.
   *
   * @returns Received response.
   *
   * @throws Connection_error In case of any connection-related problems. This
   *         method does not throw on its own, exception could be thrown from
   *         future object.
   */
  std::future<Response> async_patch(const std::string &path,
                                    const shcore::Value &body = {},
                                    const Headers &headers = {});

  /**
   * Asynchronously executes a DELETE request. This object must not be modified
   * nor any other request can be executed again until response is received.
   *
   * @param path Path to the request, it is going to be appended to the base
   *        URL.
   * @param body Optional body which is going to be sent along with the request.
   *        Content-Type is going to be set to 'application/json', unless it is
   *        explicitly set in headers.
   * @param headers Optional request-specific headers. If default headers were
   *        also specified, request-specific headers are going to be appended
   *        that set, overwriting any duplicated values.
   *
   * @returns Received response.
   *
   * @throws Connection_error In case of any connection-related problems. This
   *         method does not throw on its own, exception could be thrown from
   *         future object.
   */
  std::future<Response> async_delete(const std::string &path,
                                     const shcore::Value &body = {},
                                     const Headers &headers = {});

 private:
  class Impl;
  std::unique_ptr<Impl> m_impl;
};

}  // namespace rest
}  // namespace mysqlshdk

#endif  // MYSQLSHDK_LIBS_REST_REST_SERVICE_H_
