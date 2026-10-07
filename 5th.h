#pragma once

#include "enumerate.h"

#include <functional>
#include <initializer_list>
#include <unordered_map>
#include <variant>
#include <vector>

#include <QObject>
#include <QDebug>
#include <QMap>
#include <QString>

// Choose one or the other if you want debugging
// #define DBG_FILE
#define DBG_OUTPUT

template <class T>
class stable_array {
public:
    stable_array()                              { }
    stable_array(const stable_array<T>& sa)     { for (const auto& data: sa.mData) mData[data.first] = data.second; }
    stable_array(stable_array<T>&& sa) noexcept { for (const auto& data: sa.mData) mData[data.first] = std::move(data.second); }
    stable_array(std::initializer_list<T>& lst) { for (auto [i, itm]: enumerate(lst)) mData[i] = itm; }
    ~stable_array()                             { };

    stable_array<T>& operator=(const stable_array<T>& sa) {
        if (this != &sa) mData = sa.mData;
        return *this;
    }
    stable_array<T>& operator=(stable_array<T>&& sa) noexcept { mData = std::move(sa.mData); return *this; }

    T& operator[](std::size_t idx) {
        if (idx > mData.size()) throw std::range_error("stable_array: " + std::to_string(idx) + " out of range [0," + std::to_string(mData.size()) + ")");
        return mData[idx];
    }

    auto begin() { return mData.begin(); }
    auto end()   { return mData.end(); }

    T& first() { return mData[0]; }
    T& last()  { return mData[size() - 1]; }

    void        append(const T& itm) { mData[mData.size()] = itm; }
    std::size_t size()               { return mData.size(); }

private:
    std::unordered_map<std::size_t, T> mData;
};

class FString {
public:
    FString() {}
    FString(const FString& s): mStr(s.mStr) { }
    FString(FString&& s) noexcept: mStr(s.mStr) { }
    FString(const char* s): mStr(s) { }
    FString(const QString& x): mStr(x) { }
    FString(const QChar c): mStr(c) { }
    FString(const char c): mStr(c) { }
    ~FString() { }

    FString& operator=(const FString& s)     { if (this != &s) mStr = s.mStr; return *this; }
    FString& operator=(FString&& s) noexcept { mStr = s.mStr; return *this; }

    FString operator+(const FString& s) const { return mStr + s.mStr; }
    FString operator+(const char* s) const    { return mStr + s; }

    FString& operator+=(const FString& s) { mStr = mStr + s.mStr; return *this; }
    FString& operator+=(const char* s)    { mStr = mStr + s; return *this; }
    FString& operator+=(const char c)     { mStr = mStr + c; return *this; }

    bool operator==(const FString& s) const { return mStr == s.mStr; }
    bool operator==(const char* s) const    { return mStr == s; }
    bool operator<(const FString& s) const  { return mStr < s.mStr; }
    bool operator<(const char* s) const     { return mStr < s; }
    bool operator>=(const FString& s) const { return !(*this < s); }
    bool operator>=(const char* s) const    { return !(*this < s); }
    bool operator<=(const FString& s) const { return (*this < s) || (*this == s); }
    bool operator<=(const char* s) const    { return (*this < s) || (*this == s); }
    bool operator>(const FString& s) const  { return !(*this <= s); }
    bool operator>(const char* s) const     { return !(*this <= s); }
    bool operator!=(const FString& s) const { return !(*this == s); }
    bool operator!=(const char* s) const    { return !(*this == s); }

    auto operator[](qsizetype x) { return mStr[x]; }

    auto begin() { return mStr.begin(); }
    auto end()   { return mStr.end(); }

    bool        empty() const                                     { return mStr.isEmpty(); }
    long long   indexOf(const FString& s)                         { return mStr.indexOf(s.mStr); }
    bool        isEmpty() const                                   { return empty(); }
    FString     left(long long len) const                         { return mStr.left(len); }
    auto        size() const                                      { return mStr.size(); }
    QStringList split(const FString& str)                         { return mStr.split(str.mStr); }
    QString     str() const                                       { return mStr; }
    FString     substr(long long start, long long len = -1) const { return mStr.mid(start, len); }
    long long   toLongLong(bool* ok = nullptr) const              { return mStr.toLongLong(ok); }
    std::string toStdString() const                               { return mStr.toStdString(); }

    static FString number(long long x) { return FString(QString("%1").arg(x)); }

private:
    QString mStr;
};

inline FString operator+(const char* s1, const FString& s2) { return FString(s1) + s2; }
inline bool operator==(const char* s1, const FString& s2)   { return FString(s1) == s2; }
inline bool operator!=(const char* s1, const FString& s2)   { return FString(s1) != s2; }
inline bool operator<=(const char* s1, const FString& s2)   { return FString(s1) <= s2; }
inline bool operator>=(const char* s1, const FString& s2)   { return FString(s1) >= s2; }
inline bool operator<(const char* s1, const FString& s2)    { return FString(s1) < s2; }
inline bool operator>(const char* s1, const FString& s2)    { return FString(s1) > s2; }

namespace fifth {

  class callable;
  class vm;

  typedef const std::function<void(vm*)> function;
  typedef callable*   exe;
  typedef long long   num;
  typedef ::FString   str;

  class value {
  public:
      value()
          : mValue(str("")) { }
      value(const value& v)
        : mValue(v.mValue) { }
      value(value&& v) noexcept
        : mValue(v.mValue) { }
      value(const str& s)
        : mValue(s) { }
      value(const num& n)
        : mValue(n) { }
      value(exe e)
        : mValue(e) { }
      virtual ~value() = default;

      value& operator=(const value& v)     { mValue = v.mValue; return *this; }
      value& operator=(value&& v) noexcept { mValue = v.mValue; return *this; }
      value& operator=(const str& s)       { mValue = s; return *this; }
      value& operator=(const num n)        { mValue = n; return *this; }
      value& operator=(exe e)              { mValue = e; return *this; }

      bool operator<(const value v) const {
          switch (mValue.index()) {
          case NUM:
              switch (v.mValue.index()) {
              case NUM: return asNumber() < v.asNumber();
              case STR: return true;
              case EXE: return false;
              }
              break;
          case STR:
              switch (v.mValue.index()) {
              case NUM: return false;
              case STR: return asString() < v.asString();
              case EXE: return false;
              }
              break;
          case EXE:
              switch (v.mValue.index()) {
              case NUM: return true;
              case STR: return true;
              case EXE: return asCallable() < v.asCallable();
              }
              break;
          }
          return false;
      }
      bool operator<(const str& s) const { return *this < value(s); }
      bool operator<(const num n) const  { return *this < value(n); }

      bool operator==(const value v) const {
          switch (mValue.index()) {
          case NUM:
              switch (v.mValue.index()) {
              case NUM: return asNumber() == v.asNumber();
              case STR: return true;
              case EXE: return false;
              }
              break;
          case STR:
              switch (v.mValue.index()) {
              case NUM: return false;
              case STR: return asString() == v.asString();
              case EXE: return false;
              }
              break;
          case EXE:
              switch (v.mValue.index()) {
              case NUM: return true;
              case STR: return true;
              case EXE: return asCallable() == v.asCallable();
              }
              break;
          }
          return false;
      }
      bool operator==(const str& s) const { return *this == value(s); }
      bool operator==(const num n) const  { return *this == value(n); }

      virtual bool isNum() const { return mValue.index() == NUM; }
      virtual bool isStr() const { return mValue.index() == STR; }
      virtual bool isExe() const { return mValue.index() == EXE; }

      num toNumber(const str& s) const {
          bool ok = true;
          num x = s.toLongLong(&ok);
          return x;
      }

      virtual num asNumber() const {
          switch (mValue.index()) {
          case NUM: return std::get<NUM>(mValue);
          case STR: return toNumber(std::get<STR>(mValue));
          case EXE: return -1;
          }
          return 0;
      };
      virtual str asString() const {
          switch (mValue.index()) {
          case NUM: return FString::number(std::get<NUM>(mValue));
          case STR: return std::get<STR>(mValue);
          case EXE: return "<exe>";
          }
          return "?";
      }
      virtual exe asCallable() const {
          switch (mValue.index()) {
          case NUM:
          case STR: break;
          case EXE: return std::get<EXE>(mValue);
          }
          return nullptr;
      }

  private:
      enum type { STR, NUM, EXE};
      std::variant<str, num, exe> mValue;
  };

  template <class T> void swap(T& a, T& b) { T t = a; a = b; b = t; }

  inline bool operator<(const str& s, const value& b) { return value(s) < b; }
  inline bool operator<(const num s, const value& b) { return value(s) < b; }
  inline bool operator==(const str& s, const value& b) { return value(s) == b; }
  inline bool operator==(const num s, const value& b) { return value(s) == b; }

  class callable {
  public:
      callable() { }
      callable(const callable&) = default;
      callable(callable&&) = default;
      virtual ~callable() = default;

      callable& operator=(const callable&) = default;
      callable& operator=(callable&&) = default;

      bool isImmediate()  { return mImmediate; }
      void setImmediate() { mImmediate = true; }

      virtual bool isExe() const { return true; }

      virtual num asNumber() const { return 0; }
      virtual str asString() const { return ""; }
      virtual exe asCallable()     { return this; }

      virtual str disassemble(int indent, vm*) { return ""; };

      virtual void eval(vm*) const { }

  private:
      bool mImmediate = false;
  };

  class builtin: public callable {
    public:
      builtin(const function& f)
        : callable()
        , mFunction(f) { }

      void eval(vm* v) const override { mFunction(v); }

    private:
      const function mFunction;
  };

  class instruction {
    public:
      instruction()
        : mPayload(int(0)) { }
      instruction(const instruction& i)
        : mPayload(i.mPayload) { }
      instruction(instruction&& i) noexcept
        : mPayload(i.mPayload) { }
      instruction(const value& v)
        : mPayload(v) { }
      instruction(exe c)
        : mPayload(c) { }
      virtual ~instruction() { }

      instruction& operator=(const instruction& i)     { if (this != &i) mPayload = i.mPayload; return *this; }
      instruction& operator=(instruction&& i) noexcept { mPayload = i.mPayload; return *this; }

      str  disassemble(int indent, vm*);
      void eval(vm*) const;

  private:
      enum Type { VALUE, EXE, INT };
      std::variant<value, exe, int> mPayload;
  };

  class compiled: public callable {
    public:
      compiled()
        : callable() { }

      void eval(vm*) const override;

      void clear()              { mCode.clear(); }
      void push(const num& n)   { push(value(n)); }
      void push(const str& s)   { push(value(s)); }

      void push(value v)        { mCode.push_back(instruction(v)); }
      void call(exe e)          { mCode.push_back(instruction(e)); }
      void indirect()           { mCode.push_back(instruction(nullptr)); }

      str  disassemble(int indent, vm* v) override;

    private:
      std::vector<instruction> mCode;
  };

  class stack {
    public:
      stack() { }

      value operator[](size_t);

      void push(const value& v)    { mStack.push_back(v); }
      void push(const num& n)      { push(value(n)); }
      void push(const str& s)      { push(value(s)); }

      bool   empty() { return size() == 0; }
      size_t size()  { return mStack.size(); }

      void  dump();
      value top();
      value pop();

    private:
      std::vector<value> mStack;
  };

  class precedence {
    public:
      int operator[](const str& s) {
        if (mPrec.find(s) == mPrec.end()) return 0;
        else return mPrec[s];
      }

      void add(const str& s, int p) { mPrec[s] = p ? p : 1; }

    private:
      QMap<str, int> mPrec;
  };

class vm {
  public:
    vm();

    stack& system() { return mSystem; }
    stack& user()   { return mUser; }

    exe operator[](const str& s) { return mBuiltin[s]; }
    str operator[](const exe& e) { return mReverse[e]; }

    bool compiling() const      { return mCompiling != 0; }
    bool contains(const str& s) { return mBuiltin.find(s) != mBuiltin.end(); }
    bool contains(exe e)        { return mReverse.find(e) != mReverse.end(); }

    void addBuiltin(const str& n, const function& f);
    void addBuiltin(const str& n, exe e);
    void addImmediate(const str& n, const function& f);
    void addImmediate(const str& n, exe e);
    void unAdd(const str& n);
    exe  getBlock(exe word = exe(nullptr), const str& n = "{", compiled* code = nullptr);

    QMap<str, value>& bag(const str& name)    { return mBags[name]; }
    exe               code()                  { return mCode; }
    void              drop(const str& name)   { mBags.erase(mBags.find(name)); }
    void              finish()                { --mCompiling; }
    void              forget(const str& name) { mVars.erase(mVars.find(name)); }
    str               input() const           { return mInput; }
    str               varName(const str& s)   { if (isVar(s)) return mVars[s].asString(); else return str("?"); }
    compiled*         newCompiled()           { mCompiled.append({ }); return &mCompiled.last(); }
    str               output() const          { return mOutput; }
    auto&             precTable()             { return mPrec; }
    void              setCode(exe code)       { mCode = code; }
    void              setInput(const str& s)  { mInput = s; }
    void              setOutput(const str& s) { mOutput = s; }
    void              start()                 { ++mCompiling; }

    exe     compile(const str& end = "", exe cd = nullptr, bool dbg = false);
    void    create(const str& n);
    void    createBag(const str& n);
    void    disassemble(exe call);
    bool    doNextWord(exe word, const str& end = "", exe cd = nullptr, bool dbg = false);
    void    exec(exe c);
    void    dump();
    void    evaluate(const str& code);
    value   get(callable* word);
    value   get(const str& n);
    value   get(const str& n, const str& idx);
    bool    isVar(const str& n);
    void    remove(const str& bag, const str& var);
    void    set(const str& n, const value& v);
    void    set(const str& n, const str& idx, const value& v);
    void    skipBlock(exe& word);
    void    skipIfs(exe& word, exe& peek, bool toElse = false);
    void    skipToEnd();
    void    step(stack& user, exe& word, const QString& body, const QString& end);

  private:
    QMap<str, exe>              mBuiltin;
    QMap<str, QMap<str, value>> mBags;
    stable_array<compiled>      mCompiled;
    QMap<str, value>            mVars;
    precedence                  mPrec;
    QMap<exe, str>              mReverse;
    stack mSystem;
    stack mUser;

    str mInput;
    str mOutput;

    exe mCode;
    int mCompiling;

    void init();
};

}

#define BIND(x, y) std::bind(&x, y, std::placeholders::_1)

#ifdef QT_DEBUG
#ifdef DBG_FILE
inline void DBG_VAR(const QString& n, const QString& x) {
    FILE* fp = fopen("C:\\Users\\chris\\dbg.txt", "a");
    if (fp == nullptr) return;
    fprintf(fp, "%s: %s\n", n.toStdString().c_str(), x.toStdString().c_str()); // NOLINT
    fclose(fp);                                                                // NOLINT
}

template <typename T>
void DBG_VAR(const QString& n, const T& x) {
    FILE* fp = fopen("C:\\Users\\chris\\dbg.txt", "a");
    if (fp == nullptr) return;
    fprintf(fp,"%s: %s\n", n.toStdString().c_str(), std::to_string(x).c_str()); // NOLINT
    fclose(fp);                                                                 // NOLINT
}

inline void DBG_MSG(const char* x) {
    FILE* fp = fopen("C:\\Users\\chris\\dbg.txt", "a");
    if (fp == nullptr) return;
    fprintf(fp, "%s\n", x);  // NOLINT
    fclose(fp);              // NOLINT
}

inline void DBG_MSG(const QString& x) {
    FILE* fp = fopen("C:\\Users\\chris\\dbg.txt", "a");
    if (fp == nullptr) return;
    fprintf(fp, "%s\n", x.toStdString().c_str()); // NOLINT
    fclose(fp);                                   // NOLINT
}

template <typename T>
void DBG_MSG(const T& x) {
    FILE* fp = fopen("C:\\Users\\chris\\dbg.txt", "a");
    if (fp == nullptr) return;
    fprintf(fp, "%s\n", std::to_string(x).c_str()); // NOLINT
    fclose(fp);                                     // NOLINT
}

inline void DBG_VAL(const QString& n, const QString& x) {
    FILE* fp = fopen("C:\\Users\\chris\\dbg.txt", "a");
    if (fp == nullptr) return;
    fprintf(fp, "%s: %s\n", n.toStdString().c_str(), x.toStdString().c_str());  // NOLINT
    fclose(fp);                                                                 // NOLINT
}

template <typename T>
void DBG_VAL(const QString& n, const T& x) {
    FILE* fp = fopen("C:\\Users\\chris\\dbg.txt", "a");
    if (fp == nullptr) return;
    fprintf(fp, "%s: %s\n", n.toStdString().c_str(), x.asString().toStdString().c_str()); // NOLINT
    fclose(fp);                                                                           // NOLINT
}
#else
inline void DBG_VAR(const QString& n, const QString& x) {
    qDebug().noquote() << n << ":" << x;
}

inline void DBG_MSG(const char* x) {
    qDebug().noquote() << x;
}

inline void DBG_MSG(const QString& x) {
    qDebug().noquote() << x;
}

template <typename T>
void DBG_MSG(const T& x) {
    qDebug().noquote() << x;
}

inline void DBG_VAL(const QString& n, const QString& x) {
    qDebug().noquote() << n << ":" << x;
}

template <typename T>
void DBG_VAL(const QString& n, const T& x) {
    qDebug().noquote() << n << ":" << x.asString().str();
}
#endif
#else
inline void DBG_VAR(const QString&, const QString&) { }
template <typename T> void DBG_VAR(const QString&, const T&) { }
inline void DBG_MSG(const char*) { }
inline void DBG_MSG(const QString&) { }
template <typename T> void DBG_MSG(const T&) { }
inline void DBG_VAL(const QString&, const QString&) { }
template <typename T> void DBG_VAL(const QString&, const T&) { }
#endif
