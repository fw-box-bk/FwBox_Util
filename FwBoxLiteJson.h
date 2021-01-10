#ifndef __FWBOXLITEJSON__
#define __FWBOXLITEJSON__

#include <Arduino.h>
//#include "BuildSettings.h"
#include "FwBoxUtil.h"

#define ERROR_1 1
#define ERROR_2 2
#define ERROR_3 3
#define ERROR_4 4
#define ERROR_5 5
#define ERROR_6 6
#define ERROR_7 7
#define ERROR_8 8
#define ERROR_9 9
#define ERROR_10 10

namespace FwBox {

  class FwBoxLiteJson : public FwBoxUtil {
    public:
      FwBoxLiteJson();
      
      int gotoKeyValue(String* strJson, const char* key, int* position);
      int getString(String* strJson, const char* key, String* out);
      int getString(String* strJson, int* position, const char* key, String* out);
      int getInt(String* strJson, const char* key, int* out);
      int getArrayString(String* strJson, const char* key, String out[], int* length);
      int getTokenString(const char* strJson, int* position, String* out);
      int getTokenInt(const char* strJson, int* position, int* out);
      //String cutString(String* str, int pos);

    protected:

  };

};

#endif // __FWBOXLITEJSON__
