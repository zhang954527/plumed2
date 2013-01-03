/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   Copyright (c) 2012 The plumed team
   (see the PEOPLE file at the root of the distribution for a list of names)

   See http://www.plumed-code.org for more information.

   This file is part of plumed, version 2.0.

   plumed is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   plumed is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with plumed.  If not, see <http://www.gnu.org/licenses/>.
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
#ifndef __PLUMED_tools_OFile_h
#define __PLUMED_tools_OFile_h

#include "FileBase.h"
#include <vector>
#include <sstream>

namespace PLMD{

/**
\ingroup TOOLBOX
Class for output files

This class provides features similar to those in the standard C "FILE*" type,
but only for sequential output. See IFile for sequential input.

See the example here for a possible use:
\verbatim
#include "File.h"

int main(){
  PLMD::OFile pof;
  pof.open("ciao","w");
  pof.printf("%s\n","test1");
  pof.setLinePrefix("plumed: ");
  pof.printf("%s\n","test2");
  pof.setLinePrefix("");
  pof.addConstantField("x2").printField("x2",67.0);
  pof.printField("x1",10.0).printField("x3",20.12345678901234567890).printField();
  pof.printField("x1",10.0).printField("x3",-1e70*20.12345678901234567890).printField();
  pof.printField("x3",10.0).printField("x2",777.0).printField("x1",-1e70*20.12345678901234567890).printField();
  pof.printField("x3",67.0).printField("x1",18.0).printField();
  pof.close();
  return 0;
}
\endverbatim

This program is expected to produce a file "ciao" which reads
\verbatim
test1
plumed: test2
#! FIELDS x1 x3
#! SET x2                      67
                     10      20.12345678901234
                     10 -2.012345678901235e+71
#! FIELDS x1 x3
#! SET x2                     777
 -2.012345678901235e+71                     10
                     18                     67
\endverbatim

Notes
- "x2" is declared as "constant", which means that it is written using the "SET"
keyword. Thus, everytime it is modified, all the headers are repeated in the output file.
- printField() without arguments is used as a "newline".
- most methods return a reference to the OFile itself, to allow chaining many calls on the same line
(this is similar to << operator in std::ostream)

*/

class OFile:
public virtual FileBase{
/// Pointer to a linked OFile.
/// see link(OFile&)
  OFile* linked;
/// Internal buffer for printf
  char* buffer_string;
/// Internal buffer (generic use)
  char* buffer;
/// Internal buffer length
  int buflen;
/// Class identifying a single field for fielded output
  class Field:
  public FieldBase{
  };
/// Low-level write
  size_t llwrite(const char*,size_t);
/// True if fields has changed.
/// This could be due to a change in the list of fields or a reset
/// of a nominally constant field
  bool fieldChanged;
/// Format for fields writing
  std::string fieldFmt;
/// All the previously defined variable fields
  std::vector<Field> previous_fields;
/// All the defined variable fields
  std::vector<Field> fields;
/// All the defined constant fields
  std::vector<Field> const_fields;
/// Prefix for line (e.g. "PLUMED: ")
  std::string linePrefix;
/// Temporary ostringstream for << output
  std::ostringstream oss;
/// Find field index given name
  unsigned findField(const std::string&name)const;
public:
/// Constructor
  OFile();
/// Destructor
  ~OFile();
/// Allows overloading of link
  using FileBase::link;
/// Allows overloading of open
  using FileBase::open;
/// Allows linking this OFile to another one.
/// In this way, everything written to this OFile will be immediately
/// written on the linked OFile. Notice that a OFile should
/// be either opened explicitly, linked to a FILE or linked to a OFile
  OFile& link(OFile&);
/// Opens the file using automatic append/backup
  OFile& open(const std::string&name);
/// Set the prefix for output.
/// Typically "PLUMED: ". Notice that lines with a prefix cannot
/// be parsed using fields in a IFile.
  OFile& setLinePrefix(const std::string&);
/// Set the format for writing double precision fields
  OFile& fmtField(const std::string&);
/// Reset the format for writing double precision fields to its default
  OFile& fmtField();
/// Set the value of a double precision field
  OFile& printField(const std::string&,double);
/// Set the value of a int field
  OFile& printField(const std::string&,int);
/// Set the value of a string field
  OFile& printField(const std::string&,const std::string&);
///
  OFile& addConstantField(const std::string&);
/// Used to setup printing of values
  OFile& setupPrintValue( Value *val );
/// Print a value
  OFile& printField( Value* val, const double& v );
/** Close a line.
Typically used as
\verbatim
  of.printField("a",a).printField("b",b).printField();
\endverbatim
*/
  OFile& printField();
/**
Resets the list of fields.
As it is only possible to add new constant fields (addConstantField()),
this method can be used to clean the field list.
*/
  OFile& clearFields();
/// Formatted output with explicit format - a la printf
  int printf(const char*fmt,...);
/// Formatted output with << operator
  template <class T>
  friend OFile& operator<<(OFile&,const T &);
};

/// Write using << syntax
template <class T>
OFile& operator<<(OFile&of,const T &t){
  of.oss<<t;
  of.printf("%s",of.oss.str().c_str());
  of.oss.str("");
  return of;
}


}

#endif