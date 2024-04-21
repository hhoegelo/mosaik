#include "LessToCss.h"

#include <iostream>
#include <string>
#include <sstream>
#include <exception>

#include <less/less/LessTokenizer.h>
#include <less/less/LessParser.h>
#include <less/css/CssPrettyWriter.h>
#include <less/stylesheet/Stylesheet.h>
#include <less/lessstylesheet/LessStylesheet.h>

namespace
{
  bool parseInput(LessStylesheet &stylesheet, istream &in)
  {
    std::list<const char *>::iterator i;

    std::list<const char *> source_files;
    LessTokenizer tokenizer(in, nullptr);
    LessParser parser(tokenizer, source_files);

    try
    {
      parser.parseStylesheet(stylesheet);
    }
    catch(ParseException &e)
    {

      cerr << e.getSource() << ": Line " << e.getLineNumber() << ", Column " << e.getColumn()
           << " Parse Error: " << e.what() << endl;

      return false;
    }
    catch(exception &e)
    {
      cerr << " Error: " << e.what() << endl;

      return false;
    }

    return true;
  }

  bool processStylesheet(const LessStylesheet &stylesheet, Stylesheet &css)
  {
    ProcessingContext context;

    try
    {
      stylesheet.process(css, &context);
    }
    catch(ParseException &e)
    {

      cerr << e.getSource() << ": Line " << e.getLineNumber() << ", Column " << e.getColumn()
           << " Parse Error: " << e.what() << endl;
      return false;
    }
    catch(LessException &e)
    {

      cerr << e.getSource() << ": Line " << e.getLineNumber() << ", Column " << e.getColumn() << " Error: " << e.what()
           << endl;
      return false;
    }
    catch(exception &e)
    {
      cerr << "Error: " << e.what() << endl;
      return false;
    }
    return true;
  }

  std::string writeOutput(Stylesheet &css)
  {
    std::stringstream out;
    CssPrettyWriter writer(out);
    css.write(writer);
    return out.str();
  }
}

std::string compileLess(const std::string &less)
{
  LessStylesheet stylesheet;
  Stylesheet css;

  std::stringstream in(less);
  if(parseInput(stylesheet, in))
  {
    processStylesheet(stylesheet, css);
    return writeOutput(css);
  }

  return "";
}
