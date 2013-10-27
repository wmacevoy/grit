#include "encode_string.hpp"
#include <iomanip>
#include <sstream>

namespace utilities {

  void skip_ws_and_comments(std::istream &is) {
    char ch,nextch;
    is >> std::noskipws;
    while (is >> ch) {
      if (isspace(ch)) continue;
      if (ch == '/') {
	if (is >> nextch) {
	  if (nextch == '*') {
	    while (is >> ch) {
	      if (ch != '*') continue;
	      if (is >> nextch && nextch=='/') break;
	    }
	    continue;
	  } else {
	    is.putback(nextch);
	    is.putback('/');
	    return;
	  }
	} else {
	  is.putback('/');
	  return;
	}
      } else {
	is.putback(ch);
	return;
      }
    }
  }

  bool oddball_string(const std::string &s) {
    //    return true;
    int n=s.size();
    if (n == 0) return true; // null string
    {for (int i=0; i<n; ++i) {
	unsigned char si=s[i];
	if ( 'A' <= si && si <= 'Z') continue;
	if ( 'a' <= si && si <= 'z') continue;
	if (i>0) if ( '0' <= si && si <= '9') continue;
	return true;
      }}
    return false;
  }

  void encode_string(std::ostream &os, const std::string &s) {
    if (oddball_string(s)) {
      encode_string_always(os,s);
    } else {
      os << s;
    }
  }

  void encode_string(std::string &es, const std::string &s) {
    es = encode_string(s);
  }

  std::string encode_string(const std::string &s) {
    std::ostringstream os;
    encode_string(os,s);
    return os.str();
  }

  void encode_string_always(std::ostream &os, const std::string &s) {
    os << "\"";
    {for (int i=0; i<s.size(); ++i) {
	unsigned char si=s[i];
	if (si < ' ' || si >= 128) {
	  switch (si) {
	  case '\n': os << "\\n"; break;
	  case '\t': os << "\\t"; break;
	  case '\v': os << "\\v"; break;
	  case '\b': os << "\\b"; break;
	  case '\r': os << "\\r"; break;
	  case '\f': os << "\\f"; break;
	  case '\a': os << "\\a"; break;
	  default :  os << "\\x" << std::setw(2) << std::hex << (unsigned) (unsigned char) si; break;
	  }
	} else {
	  if (si == '\\' || si == '\"' || si == '\'') {
	    os << "\\" << si;
	  } else {
	    os << si;
	  }
	}
      }}
    os << "\"";
  }

  std::string encode_string_always(const std::string &s) {
    std::ostringstream os;
    encode_string_always(os,s);
    return os.str();
  }

  void decode_string(std::string &s, std::istream &is) {
    skip_ws_and_comments(is);
    if (is.peek() != '\"') {
      is >> s;
      return;
    }
    s="";
    char ch,nextch,buf[2];
    is >> std::noskipws >> ch;
    buf[1]=0;
    while (is >> ch) {
      if (ch == '\"') break;
      if (ch == '\\') {
	is >> nextch;
	switch(nextch) {
	case 'n': s.append("\n"); break;
	case 't': s.append("\t"); break;
	case 'v': s.append("\v"); break;
	case 'b': s.append("\b"); break;
	case 'r': s.append("\r"); break;
	case 'f': s.append("\f"); break;
	case 'a': s.append("\a"); break;
	case 'x': {
	  unsigned h;
	  char buf[2];
	  is >> std::setw(2) >> std::hex >> h;
	  buf[0]=(char)h;
	  s.append(buf);

	  break;
	}
	default: buf[0]=nextch; s.append(buf);
	}
      } else {
	buf[0]=ch;
	s.append(buf);
      }
    }
  }

  std::string decode_string(std::istream &is)
  {
    std::string ans;
    decode_string(ans,is);
    return ans;
  }

  std::string decode_string(const std::string &s)
  {
    std::string ans;
    std::istringstream is(s);
    decode_string(ans,is);
    return ans;
  }


}
