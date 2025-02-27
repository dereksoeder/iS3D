
#ifndef _ParameterReaderHeader
#define _ParameterReaderHeader

#include <vector>
#include <string>

using namespace std;

namespace iS3D
{

class ParameterReader
{
  private:
    vector<string>* names;
    vector<double>* values; // store all parameter names and values
    string removeComments(string str, string commentSymbol); // all substring after "symbol" in "str" will be removed
    void phraseEquationWithoutComments(string equation); // phrase an equation like "x=1", assume string has no comments
    long find(string name); // give the index of parameter with "name", or -1 if it does not exist
  public:
    ParameterReader();
    ~ParameterReader();
    void phraseOneLine(string str, string commentSymbol=(string)("#")); // read and phrase one setting string like "x=1"
    void readFromFile(string filename, string commentSymbol=(string)("#")); // read in parameters from a file
    void readFromArguments(long argc, char * argv[], string commentSymbol=(string)("#"), long start_from=1); // read in parameter from argument list. The process starts with index="start_from".
    bool exist(string name); // check if parameter with "name" exists
    void setVal(string name, double value); // set the parameter with "name" to value "value"
    double getVal(string name); // return the value for parameter with "name"
    void echo(); // print out all parameters to the screen
};

} //namespace iS3D

#endif
