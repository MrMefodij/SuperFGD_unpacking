/* This file is part of SuperFGD software package. This software
 * package is designed for internal use for the SuperFGD detector
 * collaboration and is tailored for this use primarily.
 *
 * Unpacking is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Unpacking is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with SuperFGD Unpacking.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

//
// Created by amefodev on 13.06.2023. mrmefodij@gmail.com
//

//
// Created by amefodev on 13.06.2023. mrmefodij@gmail.com
//

#include <string.h>
#include <vector>
#include "MDargumentHandler.h"

using namespace std;


MDargumentHandler::MDargumentHandler(string aDescription):_name(""),_description(aDescription){
}

bool MDargumentHandler::Init(){
    this->AddArgument("help", "print this message", "h");
    this->AddArgument("directory", "Path for a data file", "d", "<string>", ".");
    this->AddArgument("file", "Name of a data file", "f", "<string>", "test.bin");
    this->AddArgument("begin", "Initial position in input file", "b", "<int>", "0");
    this->AddArgument("nwords", "Number of data words to be processed", "n", "<int>", "0");

    // Check the user arguments consistancy
    // All mandatory arguments should be provided and
    // There should be no extra arguments
}

void MDargumentHandler::AddArgument( string aName, string aDescription, string aSwitch, string aFormat, string aDefault ){
  MDargument * arg = new MDargument( aName, aDescription, aSwitch, aFormat, aDefault );
  _argList.push_back( arg );
}

void MDargumentHandler::Usage()
{
  ArgListIter it;
  MDargument * val;

  cout << _name << " : " << _description << endl
       << "Usage: " << endl;
  for( it = _argList.begin(); it != _argList.end(); it++ ){
    val = *it;
    cout << " -" << val->GetSwitch() 
	 << " / --" << setw(12) << left << val->GetName() 
	 << " " << setw(8) << left << val->GetFormat() << "\t:\t" << val->GetDescription();
    if ( val->GetDefault().size() ) cout << " [ " << val->GetDefault() << " ]";
    cout << endl;
  }
}

int MDargumentHandler::ProcessArguments( int argc, char **argv ){
  _name = argv[ 0 ];
  MDargument * arg;
  char buf[64]="";
  char *substr;
  int i(1);
  string search;

  for(; i < argc; i++ ){
    //    cout << "Processing Argument : " << i << " : " << argv[i] << endl;
    arg = NULL;
    search="";
    strcpy(buf,"");
    switch (ArgumentType(argv[i])){
    case MDARGUMENT_TYPE_SWITCH: 
      { 
	//	cout << " This is a switch " << endl;
	strcpy(buf,argv[i]);
	buf[2]='\0';
	search = &buf[1];
    _mode = search;
        if ( strlen(argv[i])>2 ) strncpy(buf,&argv[i][2],strlen(argv[i]));
	else  strcpy(buf,"");
	break;
      }
    case MDARGUMENT_TYPE_NAME: 
      { 
	//	cout << " This is a full name " << endl;
	strcpy(buf,&argv[i][2]);
	// Check if there is a '=' character in the name
        substr = strstr(argv[i],"=");
	if (substr) { // there is a '=' in the argument
	  if ( strstr(&substr[1],"=") ) {
	    cerr << " Argument " << argv[i] << " is not valid (multiple =). " << endl;
	    return i;
	  }
	  search = strtok( buf, "=" );
	  strcpy(buf,&substr[1]);
	} else {
	  search = buf;
	  strcpy(buf,"");	  
	}
	// if the next argument is a '=', ignore it
	if (i+1<argc) { // There is a next argument
	  if ( argv[i+1][0] == '=' ) { // It starts with '='
	    i++;
	    if ( strlen(argv[i]) != 1 ) { // It is not just "="
	      if ( buf[0] ) strcat(buf," "); 
	      strcat(buf,&argv[i][1]);
	    }
	  }
	}
	break;
      }
    case MDARGUMENT_TYPE_VALUE: 
      {
	cerr << " Unexpected Argument value: " << argv[i] << ". Ignoring... " << endl;
	break;
      }
    case MDARGUMENT_TYPE_ERROR: 
      {
	cerr << " Argument " << argv[i] << " is not valid (Type Error). " << endl;
	return i;
	break;
      }
    }
    if (search.size()){
      arg = Find( search );
      if (arg) {
	//	cout << "Processing Argument : " << arg->GetName() << endl;
	switch( arg->GetFormatID() ){
	case MDARGUMENT_FORMAT_NOVALUE:
	  {
	    //	    cout << " Format is null (no value)" << endl;
	    if (buf[0]) {
	      cerr << "Format error in argument " << arg->GetName() 
		   << ". " << " A value is given when none is expected. " 
		   << buf << " is ignored." << endl;
	    }
	    arg->SetValue( "true" );
	    //	    cout << " ... Value is " <<  arg->GetValue() << endl;
	    break;
	  }
	case MDARGUMENT_FORMAT_DOUBLE: // no break here
	case MDARGUMENT_FORMAT_INT:
	  {
	    //	    cout << " Format is <int> or <double> " << endl;
	    if (buf[0]) {
	      arg->SetValue( buf );
	    } else {
	      if ( argc>i+1 && !IsArgName(argv[i+1]) ) {
		arg->SetValue( argv[++i] );
	      } else {
		cerr << "Format error in argument list after " << argv[i] 
		     << " : A value is expected. " << endl;
		return -1*i;
	      }
	    }
	    //	    cout << " ... Value is " <<  arg->GetValue() << endl;
	    break;
	  }
	case MDARGUMENT_FORMAT_STRING:
	  { // Everything until the next switch is accepted as value
	    //	    cout << " Format is <string> " << endl;
	    string strValue="";
	    if (buf[0]) {
	      strValue = buf;
	    }
	    for (int ia=i+1; ia<argc; ia++) {
	      if (IsArgValue(argv[ia])){
		if ( strValue.size() ) strValue += " ";
		strValue +=  argv[++i];
	      }
	      else break;
	    }
	    if ( strValue.size() ) {
	      arg->SetValue( strValue );
	      //	      cout << " ... Value is " <<  arg->GetValue() << endl;
	    } else {
	      cerr << "Format error in the argument list after " << argv[i] 
		   << " : A string is expected. " << endl;
	      return -1*i;
	    }
	    break;
	  }
	default: // no break here
	case MDARGUMENT_FORMAT_ERROR:
	  {
	    cerr << "Format error in argument " << arg->GetName() 
		 << ". " << arg->GetFormat() << " is not implemented." << endl;
	    return i;
	    break;
	  } 
	}
      } else {
	cerr << "Error in " << _name << " : " << search << " is not a valid argument" << endl;
	return i;
      }
    }
  }
  // loop over the arguments to check if there is one mandatory argument not specified
  string tmpStr;
  for ( ArgListIter it = _argList.begin(); (it != _argList.end() ); it++ ){
    (*it)->GetValue(tmpStr);
    transform(tmpStr.begin(), tmpStr.end(), tmpStr.begin(), (int(*)(int))tolower);
    if ( tmpStr=="mandatory" ) {
      cerr << " Error in " << _name << ". Argument --" <<  (*it)->GetName() << " is mandatory" << endl;
      return -1;
    }
  }
  return 0;
}

bool MDargumentHandler::GetValue( string aName )
{
  MDargument * arg = Find( aName );
  if (arg) { 
    if ( strlen(arg->GetValue()) ) return true; 
  }
  return false; 
}

MDargumentStatus_t MDargumentHandler::GetValue( string aName, string & aVal )
{
  MDargument * arg = Find( aName );
  if (arg) {
    arg->GetValue( aVal );
    return MDARGUMENT_STATUS_OK;
  }
  cerr << "Argument " << aName << " not found." << endl;
  return MDARGUMENT_STATUS_NOT_FOUND ;
}

MDargumentStatus_t MDargumentHandler::GetValue( string aName, int & aVal )
{
  MDargument * arg = Find( aName );
  if (arg) {
    if ( sscanf(arg->GetValue(),"%d",&aVal) == 1) return MDARGUMENT_STATUS_OK;
    cerr << "Format error in argument " << arg->GetName() << ". " << endl;
    return MDARGUMENT_STATUS_FORMAT_ERROR; 
  } 
  cerr << "Argument " << aName << " not found." << endl;
  return MDARGUMENT_STATUS_NOT_FOUND; 
}

MDargumentStatus_t MDargumentHandler::GetValue( string aName, double & aVal )
{
  MDargument * arg = Find( aName );
  if (arg) {
    if ( sscanf(arg->GetValue(),"%lf",&aVal) == 1) return MDARGUMENT_STATUS_OK;
    cerr << "Format error in argument " << arg->GetName() << ". " << endl;
    return MDARGUMENT_STATUS_FORMAT_ERROR; 
  }
  cerr << "Argument " << aName << " not found." << endl;
  return MDARGUMENT_STATUS_NOT_FOUND;
}

MDargument * MDargumentHandler::Find( string aNameOrSwitch ){
  ArgListIter it = _argList.begin();
  bool found = false;
  MDargument * res = NULL;
  MDargument * tmp;
  while( (it != _argList.end() ) && !found ){
    tmp = *it;
    if( ( strcmp( tmp->GetName().c_str(), aNameOrSwitch.c_str()) == 0 ) || 
	( strcmp( tmp->GetSwitch().c_str(), aNameOrSwitch.c_str()) == 0 ) ){
      res = tmp;
      found = true;
    } else it++;
  }
  return res;
}

bool      MDargumentHandler::IsArgName(const char * str)
{
  if ( str[0]=='-' && str[1]=='-' ) { 
    return true;
  } else {
    return false;
  }
}

bool      MDargumentHandler::IsArgSwitch(const char * str)
{
  if ( str[0]=='-' && str[1]!='-' ) { 
    return true;
  } else {
    return false;
  }
}

bool      MDargumentHandler::IsArgValue(const char * str)
{
  if ( str[0]=='-' ) { 
    return false;
  } else {
    return true;
  }
}

MDargumentType_t  MDargumentHandler::ArgumentType(const char * str)
{
  if ( str[0]!='-' ) { 
    return MDARGUMENT_TYPE_VALUE;
  } else if ( str[1]!='-' ) {
    if (strlen(str)<2) return MDARGUMENT_TYPE_ERROR;
    return MDARGUMENT_TYPE_SWITCH;
  } else {
    if (strlen(str)<3) return MDARGUMENT_TYPE_ERROR;
    return MDARGUMENT_TYPE_NAME;
  }
}

vector<string> MDargumentHandler::GetDataFiles(const string& stringBuf, const string& extension){
    vector<string> vFileNames;
    if (this->GetMode() == "f"){
        vFileNames.push_back(stringBuf);
    } else if (this->GetMode() == "d"){
        for (fs::directory_iterator it(stringBuf), end; it !=end; ++it) {
            if (it->path().extension() == extension) {
                std::cout << *it << std::endl;
//                auto a =it->path().string();
                vFileNames.push_back(it->path().string());
            }
        }
    }
    return vFileNames;
}

vector<string> MDargumentHandler::GetDataFiles(const string& stringBuf, const string& extension, const string& filter){
    vector<string> vFileNames;

    if (this->GetMode() == "f"){
        vFileNames.push_back(stringBuf);
    } else if (this->GetMode() == "d"){
        for (fs::directory_iterator it(stringBuf), end; it !=end; ++it) {
            if (it->path().extension() == extension) {
                vFileNames.push_back(it->path().string());
            }
        }
    }
    auto it = std::remove_if(vFileNames.begin(), vFileNames.end(), [&](auto &item) {
        if (item.find(filter) != std::string::npos){
            std::cout << item << std::endl;
            return false;
        }
        return true;
    });
    vFileNames.erase(it, vFileNames.end());

    return vFileNames;
}