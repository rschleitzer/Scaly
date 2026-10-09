; ModuleID = 'scaly_main'
source_filename = "scaly_main"
target datalayout = "e-m:o-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-n32:64-S128-Fn32"

declare ptr @memcpy(...)

declare ptr @memset(...)

define linkonce_odr i64 @_Z7printlnP2i8(ptr %0) {
entry:
  %call = call i32 @puts(ptr %0)
  %as.sext = sext i32 %call to i64
  ret i64 %as.sext
}

define linkonce_odr i64 @_Z5printP2i8(ptr %0) {
entry:
  %call = call i32 @puts(ptr %0)
  %as.sext = sext i32 %call to i64
  ret i64 %as.sext
}

declare i32 @puts(ptr)

declare i64 @_ZN3cliB6scalyc9tool_mainEiPPc(i64, ptr)

define i64 @main(i64 %0, ptr %1) {
entry:
  %call = call i64 @_ZN3cliB6scalyc9tool_mainEiPPc(i64 %0, ptr %1)
  ret i64 %call
}

define i64 @scaly_build_stamp() {
entry:
  ret i64 7033102753115649904
}
