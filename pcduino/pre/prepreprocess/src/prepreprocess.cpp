#include <iostream>
#include <string>
#include "prepreprocess.hpp"

namespace preprocess {

  static void trim(std::string &s) {
    int end=s.length();
    while (end > 0) {
      if (s[end-1] == '\n' || s[end-1] == '\r') --end;
      else break;
    }
    if (end < s.length()) s.resize(end);
  }

  static bool id0(char c) { 
    return ('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z') || (c == '_');
  }

  static bool id1(char c) { 
    return ('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z') || (c == '_')
      || ('0' <= c && c <= '9');
  }

  Prepreprocess::Prepreprocess()
  {
    in=&std::cin;
    out=&std::cout;
    prefix="std::cout";
    separator=" << ";
    suffix="std::endl";
  }

  void Prepreprocess::emit_char(char c) 
  {
    if (state == START) {
      (*out) << prefix;
      state=READY;
    }
    if (state == READY) {
      (*out) << separator;
      state=SEPARATED;
    }
    if (state == SEPARATED) {
      (*out) << "\"";
      state=LITERAL;
    }
    if (state == LITERAL) {
      if (c == '\"' || c == '\\') {
	(*out) << '\\' << c;
      } else {
	(*out) << c;
      }
    }
  }

  void Prepreprocess::emit_expression(const std::string &expression) {
    if (state == START) {
      (*out) << prefix;
      state=READY;
    }
    if (state == LITERAL) {
      (*out) << "\"";
      state=READY;
    }
    if (state == READY) {
      (*out) << separator;
      state=SEPARATED;
    }
    if (state == SEPARATED) {
      (*out) << expression;
      state=READY;
    }
  }

  void Prepreprocess::emit_end(bool use_suffix) { 
    if (use_suffix) emit_expression(suffix);
    if (state == SEPARATED) {
      (*out) << "\"";
      state=LITERAL;
    }
    if (state == LITERAL) {
      (*out) << "\"";
      state=READY;
    }
    if (state == READY) {
      (*out) << ";" << std::endl;
      state = START;
    }
  }

  void Prepreprocess::exact_quote(const std::string &line)  
  {
    int end=line.length();
    state=START;
    for (int pos=0; pos<end; ++pos) emit_char(line[pos]);
    emit_end();
  }

  void Prepreprocess::magic_quote(const std::string &line)
  {
    int end=line.length();
    bool use_suffix=true;

    state=START;

    for (int pos=0; pos<end; ++pos) {
      if (line[pos]=='$') {
	if (pos == end-1) {
	  use_suffix=false;
	  break;
	} else {
	  if (id0(line[pos+1])) {
	    ++pos;
	    int begin=pos;
	    while (pos < end && id1(line[pos])) ++pos;
	    emit_expression(line.substr(begin,(pos-begin)));
	    --pos;
	    continue;
	  }
	  if (line[pos+1] == '(') {
	    ++pos;
	    int begin=pos;
	    int parens=0;
	    while (pos < end) {
	      if (line[pos]=='(') ++parens;
	      if (line[pos]==')') {--parens; if (parens==0) { ++pos; break; } }
	      ++pos;
	    }
	    emit_expression(line.substr(begin,(pos-begin)));
	    --pos;
	    continue;
	  }
	  if (line[pos+1]=='$') {
	    ++pos;
	  }
	}
      }
      emit_char(line[pos]);
    }
    emit_end(use_suffix);
  }

  void Prepreprocess::process()
  {
    int blank_lines=0;
    std::string line;

    for (;;) {
      if (in->eof()) break;
      getline(*in,line);
      trim(line);
      if (line.length() > 0) {
	while (blank_lines > 0) { exact_quote(""); --blank_lines; }
      } else {
	++blank_lines;
	continue;
      }
      if (line.length() >= 2 && line[0] == '#') {
	if (line[1] == '!') { (*out) << line.substr(2) << std::endl; continue; }
	if (line[1] == '"') { magic_quote(line.substr(2)); continue; }
	if (line[1] == '\'') { exact_quote(line.substr(2)); continue; }
      }
      magic_quote(line);
    }
  }
}
