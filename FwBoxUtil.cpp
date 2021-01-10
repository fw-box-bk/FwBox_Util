
#include "FwBoxUtil.h"

#if defined(ESP8266)
  #include <Hash.h>
#else
  //
  // For ESP32
  //
  #include "FwBoxSha1.h"
#endif


using namespace FwBox;


FwBoxUtil::FwBoxUtil()
{
}

void FwBoxUtil::untilNotSpace(const char* Context, int* Position)
{
  for (;Context[*Position] != 0; (*Position)++) {
    if(!isSpace(Context[*Position])) {
      return;
    }
  }
}

String FwBoxUtil::untilNotAlphaNumeric(const char* Context, int* Position)
{
  String temp = "";
  for (;Context[*Position] != 0; (*Position)++) {
    if(isAlphaNumeric(Context[*Position]) || (Context[*Position] == '_')) {
      temp += Context[*Position];
    }
    else
      return temp;
  }
  return temp;
}

String FwBoxUtil::untilNotNumber(const char* Context, int* Position, int* NumberType)
{
  String temp = "";
  bool is_hex = false;
  int dot_count = 0;
  bool is_negative = false;

  *NumberType = NUMBER_TYPE_INT;

  if(Context[(*Position)] == '-') {
    is_negative = true;
    temp += Context[*Position];
    (*Position)++;
  }

  //
  // String starts with "0x" or "0X".
  //
  if((Context[(*Position)] == '0') &&
     ((Context[(*Position) + 1] == 'x') || (Context[(*Position) + 1] == 'X'))) {

    (*Position) += 2;
    is_hex = true;
  }

  for (;Context[*Position] != 0; (*Position)++) {
    if(is_hex) { // This is a hex number string.
      if(isHexDigit(Context[*Position])) {
        temp += Context[*Position];
      }
      else {
        *NumberType = NUMBER_TYPE_INT_HEX;
        return temp;
      }
    }
    else {
      if(isDigit(Context[*Position])) { // found a digit.
        temp += Context[*Position];
      }
      else if(Context[*Position] == '.') { // found a dot char.
        temp += Context[*Position];
        dot_count++;
      }
      else {
        if(dot_count == 0)
          *NumberType = NUMBER_TYPE_INT;
        else if(dot_count == 1) // Example : "3.14159"
          *NumberType = NUMBER_TYPE_FLOAT;
        //else if(dot_count == 3) // Example : "192.168.1.5"
        //  *NumberType = NUMBER_TYPE_IP;
        else
          *NumberType = NUMBER_TYPE_ERROR;
        
        return temp;
      }
    }
  }

  if(is_hex) {
    *NumberType = NUMBER_TYPE_INT_HEX;
  }
  else {
    if(dot_count == 0)
      *NumberType = NUMBER_TYPE_INT;
    else if(dot_count == 1)
      *NumberType = NUMBER_TYPE_FLOAT;
    else
      *NumberType = NUMBER_TYPE_ERROR;
  }

  return temp;
}

int FwBoxUtil::indexOf(const char* context, const char* key)
{
  //
  // Search for key in context
  //
  char* p_key = strstr(context, key);
  if(p_key == NULL) { // Not found
    return -1; // Not found
  }
  return (p_key - context);
}

bool FwBoxUtil::isHexDigit(char Input)
{
  if((Input >= '0' && Input <= '9') ||
     (Input >= 'a' && Input <= 'f') ||
     (Input >= 'A' && Input <= 'F')) {

    return true;
  }
  else {
    return false;
  }
}

//
// https://man7.org/linux/man-pages/man7/utf8.7.html
//
int FwBoxUtil::uniToUtf8(unsigned long uni, char out[])
{
  if(uni <= 0x0000007f) {
    //
    // 0xxxxxxx
    //
    out[0] = (uni & 0x7f);
    out[1] = 0;
    return 1;
  }
  else if((uni >= 0x00000080) && (uni <= 0x000007ff)) {
    //
    // 110xxxxx 10xxxxxx
    //
    out[0] = ((uni >> 6) & 0x1f) | 0xc0;
    out[1] = (uni & 0x3f) | 0x80;
    out[2] = 0;
    return 2;
  }
  else if((uni >= 0x00000800 ) && (uni <= 0x0000ffff)) {
    //
    // 1110xxxx 10xxxxxx 10xxxxxx
    //
    out[0] = ((uni >> 12) & 0x0f) | 0xe0;
    out[1] = ((uni >> 6) & 0x3f) | 0x80;
    out[2] = (uni & 0x3f) | 0x80;
    out[3] = 0;
    return 3;
  }
  else if((uni >= 0x00010000 ) && (uni <= 0x001fffff)) {
    //
    // 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
    //
    out[0] = ((uni >> 18) & 0x07) | 0xf0;
    out[1] = ((uni >> 12) & 0x3f) | 0x80;
    out[2] = ((uni >> 6) & 0x3f) | 0x80;
    out[3] = (uni & 0x3f) | 0x80;
    out[4] = 0;
    return 4;
  }
  else if((uni >= 0x00200000 ) && (uni <= 0x03ffffff)) {
    //
    // 111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
    //
    out[0]= ((uni >> 24) & 0x03) | 0xf8;
    out[1]= ((uni >> 18) & 0x3f) | 0x80;
    out[2]= ((uni >> 12) & 0x3f) | 0x80;
    out[3]= ((uni >> 6) & 0x3f) | 0x80;
    out[4]= (uni & 0x3f) | 0x80;
    out[5] = 0;
    return 5;
  }
  else if((uni >= 0x04000000 ) && (uni <= 0x7fffffff)) {
    //
    // 1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
    //
    out[0] = ((uni >> 30) & 0x01) | 0xfc;
    out[1] = ((uni >> 24) & 0x3f) | 0x80;
    out[2] = ((uni >> 18) & 0x3f) | 0x80;
    out[3] = ((uni >> 12) & 0x3f) | 0x80;
    out[4] = ((uni >> 6) & 0x3f) | 0x80;
    out[5] = (uni & 0x3f) | 0x80;
    out[6] = 0;
    return 6;
  }
  else {
    return 0;
  }
}

int FwBoxUtil::convertUnicode(const char* strUnicode, String* out)
{
  //String out = "";
  int len = strlen(strUnicode);
  char cc;
  char* error;
  for (int i = 0; i < len; i++) {
     cc = strUnicode[i];
     if(cc == '\\') { // got escape char
       cc = strUnicode[++i];
       if(cc == 'u') { // got unicode hex
         char buff[8];
         memset(buff, 0, 8); // clear it
         buff[0] = '0';
         buff[1] = 'x';
         for (int j = 0; j < 4; j++) {
           cc = strUnicode[++i];
           buff[j + 2] = cc;
         }
         //DBGMSGLN(buff);
         unsigned long uni_val = strtol(buff, &error, 16); //convert the string
         
         memset(buff, 0, 8); // clear it
         int result_len = FwBoxUtil::uniToUtf8(uni_val, buff);
         
         (*out) += buff;
       }
       else if(cc == '/') {
         (*out) += cc;
       }
     }
     else {
       (*out) += cc;
     }
  }
  return 0; // success
}

String FwBoxUtil::getBuildDateTime()
{
  const char *months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
  String TokenArr[3];
  int mon_num = 1;
  String result = "";
  const char* str_date = __DATE__;
  int pos = 0;
  //UtilStr sp;
  int num_type = 0;

  PRINT_VAR(str_date);

  FwBoxUtil::untilNotSpace(str_date, &pos);
  TokenArr[0] = FwBoxUtil::untilNotAlphaNumeric(str_date, &pos);
  PRINT_VAR(TokenArr[0]);
  if(str_date[pos] != ' ')
    return "";

  FwBoxUtil::untilNotSpace(str_date, &pos);
  TokenArr[1] = FwBoxUtil::untilNotNumber(str_date, &pos, &num_type);
  PRINT_VAR(TokenArr[1]);
  if(num_type != NUMBER_TYPE_INT)
    return "";
  if(str_date[pos] != ' ')
    return "";

  FwBoxUtil::untilNotSpace(str_date, &pos);
  TokenArr[2] = FwBoxUtil::untilNotNumber(str_date, &pos, &num_type);
  if(num_type != NUMBER_TYPE_INT)
    return "";
  PRINT_VAR(TokenArr[2]);

  for(; mon_num <= 12; mon_num++) {
    if(TokenArr[0].equals(months[mon_num - 1]))
      break;
  }
  if(mon_num > 12)
    return ""; // ERROR

  result += TokenArr[2];
  result += "-";
  result += mon_num;
  result += "-";
  result += TokenArr[1];
  result += "%20";
  result += __TIME__;
  
  return result;
}

String FwBoxUtil::toSha1(const char* source)
{
#if defined(ESP8266)
  //Serial.print("source=");
  //Serial.println(source);
  //String re = sha1(source);
  //Serial.print("re=");
  //Serial.println(re);
  return String(sha1(source));
#else
  //
  // For ESP32
  //
  SHA1Context sha;
  char result[41];
  uint8_t Message_Digest[20];

  int err = SHA1Reset(&sha);
  err = SHA1Input(&sha, (const unsigned char *) source, strlen(source));
  if (err) {
    //Serial.printf("SHA1Input Error %d.\n", err );
    return String("");
  }

  err = SHA1Result(&sha, Message_Digest);
  if (err) {
    //Serial.printf("SHA1Result Error %d, could not compute message digest.\n", err);
    return String("");
  }

  //Serial.print("RESULT = ");

  char buff[3];
  for (int i = 0; i < 20 ; ++i) {
    sprintf(&(buff[0]), "%02x", Message_Digest[i]);
    buff[2] = 0;
    result[(i * 2)] = buff[0];
    result[(i * 2) + 1] = buff[1];
  }
  result[40] = 0;
  //Serial.printf("%s\n", result);

  return String(result);
#endif
}
