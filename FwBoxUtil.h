#ifndef __FWBOXUTIL__
#define __FWBOXUTIL__

#include <Arduino.h>
#include "BuildSettings.h"


#define NUMBER_TYPE_ERROR 0
#define NUMBER_TYPE_INT (1 << 1)
#define NUMBER_TYPE_INT_HEX (1 << 2)
#define NUMBER_TYPE_FLOAT (1 << 3)
//#define STRING_TYPE_IP (1 << 4) // Example : 192.168.1.10
#define STRING_TYPE_UUID_V4 (1 << 5) // // Example : 00112233-4455-6677-8899-aabbccddeeff


namespace FwBox {
  
    class FwBoxUtil {
      public:
        FwBoxUtil();

        void untilNotSpace(const char* Context, int* Position);
        String untilNotAlphaNumeric(const char* Context, int* Position);
        String untilNotNumber(const char* Context, int* Position, int* NumberType);
        int indexOf(const char* context, const char* key);
        bool isHexDigit(char Input);
        int uniToUtf8(unsigned long uni, char out[]);
        int convertUnicode(const char* strUnicode, String* out);
        String getBuildDateTime();
        String toSha1(const char* source);
    };
  
};

using namespace FwBox;

#endif // __FWBOXUTIL__
