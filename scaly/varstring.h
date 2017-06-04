#ifndef __scaly__varstring__
#define __scaly__varstring__

typedef struct scaly_VarString scaly_VarString; struct scaly_VarString {
    char* buffer;
    size_t length;
    size_t capacity;
};

scaly_VarString* scaly_VarString_new(scaly_Page* _page);
scaly_VarString* scaly_VarString_fromChar(scaly_Page* _page, const char c);
scaly_VarString* scaly_VarString_fromRawString(scaly_Page* _page, const char* theString);
scaly_VarString* scaly_VarString_fromString(scaly_Page* _page, scaly_string* theString);
scaly_VarString* scaly_VarString_fromVarString(scaly_Page* _page, scaly_VarString* theString);
scaly_VarString* scaly_VarString_newWithLength(scaly_Page* _page, size_t theLength);
char* scaly_VarString_getNativeString(scaly_VarString* this);
size_t scaly_VarString_getLength(scaly_VarString* this);
char scaly_VarString_charAt(scaly_VarString* this, size_t i);
void scaly_VarString_appendChar(scaly_VarString* this, char c);
void scaly_VarString_appendRawString(scaly_VarString* this, const char* theString);
void scaly_VarString_appendVarString(scaly_VarString* this, scaly_VarString* theString);
void scaly_VarString_appendString(scaly_VarString* this, scaly_string* theString);
int scaly_VarString_equalsRawString(scaly_VarString* this, const char* theString);
int scaly_VarString_notEqualsRawString(scaly_VarString* this, const char* theString);
int scaly_VarString_equals(scaly_VarString* this, scaly_VarString* theString);
int scaly_VarString_notEquals(scaly_VarString* this, scaly_VarString* theString);
int scaly_VarString_equalsVarString(scaly_VarString* this, scaly_VarString* theString);
int scaly_VarString_notEqualsVarString(scaly_VarString* this, scaly_VarString* theString);
scaly_Array* scaly_VarString_Split(scaly_Page* _rp, char c);
void scaly_VarString_copyNativeString(scaly_VarString* this, const char* theString, size_t length);
void scaly_VarString_appendChar(scaly_VarString* this, char c);
void scaly_VarString_allocateBuffer(scaly_VarString* this, size_t size);

#endif // __scaly__letstring__
