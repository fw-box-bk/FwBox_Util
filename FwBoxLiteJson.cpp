
#include "FwBoxLiteJson.h"

using namespace FwBox; 


FwBoxLiteJson::FwBoxLiteJson()
{

}

int FwBoxLiteJson::getString(String* strJson, const char* key, String* out)
{
  int pos = 0;
  return FwBoxLiteJson::getString(strJson, &pos, key, out);
}

//
// Ge the value of 'key'
// Example : {"name":"David","email":"xx@gg.cc"}
// key='email'
// The function will return 'xx@gg.cc'
//
int FwBoxLiteJson::getString(String* strJson, int* position, const char* key, String* out)
{
  //int pos = 0;
  //Serial.print("getString (01) : *position=");
  //Serial.println(*position);
  //Serial.println(strJson->substring(*position, *position+20));

  int err_code = FwBoxLiteJson::gotoKeyValue(strJson, key, position);

  if(err_code > 0) { // error
    return err_code;
  }

  //Serial.print("getString (02) : *position=");
  //Serial.println(*position);
  //Serial.println(strJson->substring(*position, *position+20));

  String temp_out = "";
  err_code = FwBoxLiteJson::getTokenString(strJson->c_str(), position, &temp_out);
  if(err_code > 0)
    return err_code; // error
  else {
    FwBoxUtil::convertUnicode(temp_out.c_str(), out);
    //DBGMSG("Final : *out=");
    //DBGMSGLN(*out);
    return 0; // success
  }
}

int FwBoxLiteJson::getInt(String* strJson, const char* key, int* out)
{
  int pos = 0;
  int err_code = FwBoxLiteJson::gotoKeyValue(strJson, key, &pos);

  if(err_code > 0) { // error
    return err_code;
  }
  
  int temp_out = 0;
  err_code = FwBoxLiteJson::getTokenInt(strJson->c_str(), &pos, &temp_out);
  //Serial.print("getInt (02) : temp_out=");
  //Serial.println(temp_out);
  if(err_code > 0) { // error
    return err_code;
  }
  else {
    *out = temp_out;
    //Serial.print("getInt (03) : *out=");
    //Serial.println(*out);
    return 0; // success
  }
}

int FwBoxLiteJson::getArrayString(String* strJson, const char* key, String out[], int* length)
{
  int pos = 0;
  int err_code = FwBoxLiteJson::gotoKeyValue(strJson, key, &pos);

  if(err_code > 0) { // error
    return err_code;
  }

  const char* p_str = strJson->c_str();
  if(p_str[pos] != '[')
    return ERROR_8; // error

  pos++; // skip '['

  FwBoxUtil::untilNotSpace(p_str, &pos);

  for(int ii = 0; ii < (*length); ii++) {
    String temp_out = "";
    err_code = FwBoxLiteJson::getTokenString(p_str, &pos, &temp_out);
    if(err_code > 0) {
      return err_code; // error
    }
    else {
      String temp_out2 = "";
      FwBoxUtil::convertUnicode(temp_out.c_str(), &temp_out2);
      out[ii] = temp_out2;
      //DBGMSGF("getArrayString : out[%d]=", ii);
      //DBGMSGLN(out[ii]);
    }
    
    FwBoxUtil::untilNotSpace(p_str, &pos);

    if(p_str[pos] == ',') {
      pos++; // skip ','
      FwBoxUtil::untilNotSpace(p_str, &pos);
    }
    else if(p_str[pos] == ']') {
      pos++; // skip ']'
      FwBoxUtil::untilNotSpace(p_str, &pos);
      (*length) = ii + 1; // return the real array size
      return 0; // success, reach the end of the array
    }
    else {
      return ERROR_9; // error
    }
  }

  return ERROR_10; // error
}

//
// Move the position to the start of the value
//   EX: json string is {"DevTypeName":"Environment Sensor"}
//       key is DevTypeName
//       the function will goto the position to 'v'
//                      v
//       {"DevTypeName":"Environment Sensor"}
//
int FwBoxLiteJson::gotoKeyValue(String* strJson, const char* key, int* position)
{
  String new_key = "\"";
  new_key += key;
  new_key += "\"";
  int new_key_len = new_key.length();
  //Serial.print("gotoKeyValue (01) : *position=");
  //Serial.println(*position);
  //Serial.println(strJson->substring(*position, *position+20));

  //
  // Begin from (*position)
  //
  const char* p_str = strJson->c_str();

  //
  // Search for new_key in 'p_str + *position'
  //
  int offset_key = FwBoxUtil::indexOf(p_str + *position, new_key.c_str());
  if(offset_key < 0) { // Not found
    return ERROR_1; // error
  }

  (*position) += offset_key; // Move the pointer to the position of 'key'
  (*position) += new_key_len; // skip the key
  //Serial.print("gotoKeyValue (03) : *position=");
  //Serial.println(*position);
  //Serial.println(strJson->substring(*position, *position+20));

  FwBoxUtil::untilNotSpace(p_str, position);
  if(p_str[*position] != ':')
    return ERROR_3; // error
  (*position)++; // skip ':'

  //Serial.print("gotoKeyValue (04) : *position=");
  //Serial.println(*position);
  //Serial.println(strJson->substring(*position, *position+20));

  FwBoxUtil::untilNotSpace(p_str, position);

  //*position = pos;

  return 0; // success
}

//
// Parsing "hello" and get the string - hello
//
int FwBoxLiteJson::getTokenString(const char* strJson, int* position, String* out)
{
  //char buff[MAX_DEV_NAME_LENGTH];
  //int buff_i = 0;
  int pos = *position;

  //Serial.print("getTokenString (01) : *position=");
  //Serial.println(*position);
  //String temp9 = strJson;
  //Serial.println(temp9.substring(*position, *position+20));

  if(strJson[pos] != '\"') // check first double quotes
    return ERROR_5; // error

  pos++; // skip "
  
  (*out) = "";
  
  while(1) {

    if(strJson[pos] == 0) { // string end
      return ERROR_6; // error
    }
    else if(strJson[pos] == '\\') { // found a char - \
      //DBGMSG("(A)pos=");
      //DBGMSGLN(pos);
      pos++; // next
      if(strJson[pos] == '\"') { // char sequence - \"
        //DBGMSG("(B)pos=");
        //DBGMSGLN(pos);
        (*out) += '\"';
        pos++;
      }
      else if(strJson[pos] == '\\') { // char sequence - \\
        //DBGMSG("(C)pos=");
        //DBGMSGLN(pos);
        (*out) += '\\';
        pos++;
      }
      else {
        (*out) += '\\';
        (*out) += strJson[pos];
        pos++;
      }
    }
    else if(strJson[pos] == '\"') { // found the last double quotes
      //DBGMSG("found the last double quotes - pos=");
      //DBGMSGLN(pos);
      pos++;
      break;
    }
    else {
      (*out) += strJson[pos];
      pos++;
    }
  }
  //DBGMSG("(D)pos=");
  //DBGMSGLN(pos);
  *position = pos;
  
  //Serial.print("getTokenString (05) : *position=");
  //Serial.println(*position);
  //String temp9 = strJson;
  //Serial.println(temp9.substring(*position, *position+20));

  return 0; // success
}

int FwBoxLiteJson::getTokenInt(const char* strJson, int* position, int* out)
{
  int pos = *position;
  int num_type = NUMBER_TYPE_ERROR;

  String str_int = FwBoxUtil::untilNotNumber(strJson, &pos, &num_type);
  //Serial.print("str_int=");
  //Serial.println(str_int);
  //Serial.print("num_type=");
  //Serial.println(num_type);
  if(num_type == NUMBER_TYPE_INT) {
    *out = str_int.toInt();
    *position = pos;
    return 0; // success
  }
  else {
    return ERROR_7; // error
  }
}
/*
String FwBoxLiteJson::cutString(const char* str, int pos)
{
  char buff[11];
  char* p_new_str = str;
  p_new_str += pos;

  memset(&(buff[0]), p_new_str, 10);
  buff[10] = 0;
  return String(buff);
}*/
