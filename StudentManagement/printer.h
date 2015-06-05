#ifndef PRINTER_H
#define PRINTER_H
#include <string>

class Student;
class Printer
  {
  public:
   bool schedule_job(std::string filename, const Student & owner) const;
  };

#endif // PRINTER_H
