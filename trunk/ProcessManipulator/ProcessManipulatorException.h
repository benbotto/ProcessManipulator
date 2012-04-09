#ifndef _PROCESS_MANIPULATOR_EXCEPTOIN_H
#define _PROCESS_MANIPULATOR_EXCEPTOIN_H

#include <cstring>
#include <exception>
#include <string>
using std::exception;
using std::string;

class ProcessManipulatorException : public exception
{
  string _what;

  ProcessManipulatorException();
public:
  ProcessManipulatorException(const char* const what);
  ProcessManipulatorException(const string& what);
  virtual const char* what() const throw();
  ~ProcessManipulatorException() throw();
};

#endif
