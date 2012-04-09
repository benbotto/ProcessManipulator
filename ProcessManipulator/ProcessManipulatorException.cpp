#include "ProcessManipulatorException.h"

ProcessManipulatorException::ProcessManipulatorException(const char* const what)
{
  _what = what;
}

ProcessManipulatorException::ProcessManipulatorException(const string& what)
{
  _what = what;
}

const char* ProcessManipulatorException::what() const throw()
{
  return _what.c_str();
}

ProcessManipulatorException::~ProcessManipulatorException() throw()
{
}