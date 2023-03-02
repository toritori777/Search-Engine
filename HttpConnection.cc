/*
 * Copyright ©2022 Travis McGaha.  All rights reserved.  Permission is
 * hereby granted to students registered for University of Pennsylvania
 * CIT 595 for use solely during Spring Semester 2022 for purposes of
 * the course.  No other use, copying, distribution, or modification
 * is permitted without prior written consent. Copyrights for
 * third-party components of this work must be honored.  Instructors
 * interested in reusing these course materials should contact the
 * author.
 */

#include <cstdint>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <map>
#include <string>
#include <vector>
#include <iostream>
#include "./HttpRequest.h"
#include "./HttpUtils.h"
#include "./HttpConnection.h"

using namespace std;
using std::map;
using std::string;
using std::vector;
using namespace boost::algorithm;

namespace searchserver {

static const char *kHeaderEnd = "\r\n\r\n";
static const int kHeaderEndLen = 4;

bool HttpConnection::next_request(HttpRequest *request) {
  // Use "wrapped_read" to read data into the buffer_
  // instance variable.  Keep reading data until either the
  // connection drops or you see a "\r\n\r\n" that demarcates
  // the end of the request header.
  //
  // Once you've seen the request header, use parse_request()
  // to parse the header into the *request argument.
  //
  // Very tricky part:  clients can send back-to-back requests
  // on the same socket.  So, you need to preserve everything
  // after the "\r\n\r\n" in buffer_ for the next time the
  // caller invokes next_request()!

  // TODO: implement
  string tmp = "";
  bool res;
  size_t pos = buffer_.find(kHeaderEnd, 0, kHeaderEndLen);
  // if the buffer does not contain what we need for the next request
  if (pos == string::npos) {
    while (true) {
      int read = wrapped_read(fd_, &tmp);
      if (read == 0) {
        // connection drop
        break;
      } else if (read == -1) {
        // error w/ reading
        return false;
      }
      buffer_ += tmp;
      pos = buffer_.find(kHeaderEnd, 0, kHeaderEndLen);
      if (pos != string::npos) {
        break;
      }
    }
  }
  if (pos != string::npos) {
    if (pos < buffer_.length() - 4) {
      res = parse_request(buffer_.substr(0, pos + 4), request);
      buffer_ = buffer_.substr(pos + kHeaderEndLen);
    } else {
      res = parse_request(buffer_, request);
      buffer_ = "";
    } 
  } else {
    return false;
  }
  return res;
}

bool HttpConnection::write_response(const HttpResponse &response) {
  // Implement so that the response is converted to a string
  // and written out to the socket for this connection

  // TODO: implement
  string resp = response.GenerateResponseString();
  int res = static_cast<size_t>(wrapped_write(fd_, resp));
  if (res == -1) {
    return false;
  }
  return true;
}

bool HttpConnection::parse_request(const string &request, HttpRequest* out) {
  HttpRequest req("/");  // by default, get "/".

  // Split the request into lines.  Extract the URI from the first line
  // and store it in req.URI.  For each additional line beyond the
  // first, extract out the header name and value and store them in
  // req.headers_ (i.e., HttpRequest::AddHeader).  You should look
  // at HttpRequest.h for details about the HTTP header format that
  // you need to parse.
  //
  // You'll probably want to look up boost functions for (a) splitting
  // a string into lines on a "\r\n" delimiter, (b) trimming
  // whitespace from the end of a string, and (c) converting a string
  // to lowercase.
  //
  // If a request is malfrormed, return false, otherwise true and 
  // the parsed request is retrned via *out
  
  // TODO: implement
  vector<string> lines;
  split(lines, request, is_any_of("\r\n"));
  for (size_t i = 0; i < lines.size(); i++) {
    trim(lines[i]);
  }
  // extract URI from the first line
  // e.x.
  // GET /foo/bar?baz=bam HTTP/1.1\r\n

  vector<string> first_line;
  split(first_line, lines[0], is_any_of(" "));
  if (first_line.size() < 3) {
    // malfrormed
    return false;
  }
  trim(first_line[1]);
  to_lower(first_line[1]);
  req.set_uri(first_line[1]);

  // parse headers
  // e.x.
  // Host: www.news.com\r\n
  // extract out the header name and value
  vector<string> header_line;
  for (size_t i = 1; i < lines.size(); i++) {
    size_t pos = lines[i].find(":");
    if (pos == string::npos) {
      continue;
    }
    string name = lines[i].substr(0, pos);
    string value = lines[i].substr(pos + 2, lines[i].length());
    trim(name);
    to_lower(name);
    req.AddHeader(name, value);
  }
  *out = req;
  return true;
}

}  // namespace searchserver
