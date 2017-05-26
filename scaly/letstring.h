#ifndef __scaly__letstring__
#define __scaly__letstring__

typedef struct scaly_VarString scaly_VarString; struct scaly_VarString;

typedef struct scaly_string scaly_string; struct scaly_string {
    char* buffer;
    size_t length;
};

scaly_string* scaly_string_new(scaly_Page* _page);
scaly_string* scaly_string_fromChar(scaly_Page* _page, const char c);
scaly_string* scaly_string_fromRawString(scaly_Page* _page, const char* theString);
scaly_string* scaly_string_fromString(scaly_Page* _page, scaly_string* theString);
scaly_string* scaly_string_fromVarString(scaly_Page* _page, scaly_VarString* theString);
scaly_string* scaly_string_newWithLength(scaly_Page* _page, size_t theLength);
char* scaly_string_getNativeString(scaly_string* this);
size_t scaly_string_getLength(scaly_string* this);
char scaly_string_charAt(scaly_string* this, size_t i);
int scaly_string_equalsRawString(scaly_string* this, const char* theString);
int scaly_string_notEqualsRawString(scaly_string* this, const char* theString);
int scaly_string_equals(scaly_string* this, scaly_string* theString);
int scaly_string_notEquals(scaly_string* this, scaly_string* theString);
int scaly_string_equalsVarString(scaly_string* this, scaly_VarString* theString);
int scaly_string_notEqualsVarString(scaly_string* this, scaly_VarString* theString);
scaly_Array* scaly_String_Split(scaly_Page* _rp, char c);
void scaly_string_copyNativeString(scaly_string* this, const char* theString, size_t length);

#endif // __scaly__letstring__
