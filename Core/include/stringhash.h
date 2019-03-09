#pragma once
#include <string>
#include <utility>

#define DEBUG_KEEP_DEV_STRING 1


class StringHash {
public:
  // using std::hash for now, but should revisit later to prevent the 
  // hash function from happening at runtime on a const char *
  // std::hash<const char *> doesn't actually hash, which may
  // cause problems in situations where strings live on the stack at the 
  // time of construction
  StringHash() : m_hash(0) {}
  StringHash(std::string & string) {
    std::hash<std::string> hasher;
    if (string.size()) {
      m_hash = hasher(string);
    }
    else {
      // make sure empty string has same hash as no init
      m_hash = 0;
    }
    
#ifdef DEBUG_KEEP_DEV_STRING
    m_devString = string;
#endif // DEBUG_KEEP_DEV_STRING
  }
  StringHash(const char * string)
  : StringHash(std::string(string)){}

  bool operator< (const StringHash & rhs) const {
    return m_hash < rhs.m_hash;
  }
  
  bool operator<= (const StringHash & rhs) const {
    return m_hash <= rhs.m_hash;
  }

  bool operator> (const StringHash & rhs) const {
    return m_hash > rhs.m_hash;
  }

  bool operator>= (const StringHash & rhs) const {
    return m_hash >= rhs.m_hash;
  }

  bool operator== (const StringHash & rhs) const {
    return m_hash == rhs.m_hash;
  }

  bool operator!= (const StringHash & rhs) const {
    return m_hash != rhs.m_hash;
  }

#ifdef DEBUG_KEEP_DEV_STRING
  std::string GetDevString() const {
    return m_devString;
  }
#endif

private:
  size_t m_hash;
#ifdef DEBUG_KEEP_DEV_STRING
  std::string m_devString;
#endif // DEBUG_KEEP_DEV_STRING
};