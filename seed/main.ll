; ModuleID = 'main'
source_filename = "main"
target datalayout = "e-m:o-i64:64-i128:128-n32:64-S128"

declare ptr @memcpy(...)

declare ptr @memset(...)

define linkonce_odr i64 @_Z7printlnP2i8(ptr %0) {
entry:
  %call = call i64 @puts(ptr %0)
  ret i64 %call
}

define linkonce_odr i64 @_Z5printP2i8(ptr %0) {
entry:
  %call = call i64 @puts(ptr %0)
  ret i64 %call
}

declare i64 @puts(ptr)

declare i64 @_ZN3cli4mainEiPPc(i64, ptr)

define linkonce_odr i64 @main(i64 %0, ptr %1) {
entry:
  %call = call i64 @_ZN3cli4mainEiPPc(i64 %0, ptr %1)
  ret i64 %call
}
