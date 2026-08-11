; ModuleID = 'main'
source_filename = "main"
target datalayout = "e-m:o-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-n32:64-S128-Fn32"

declare ptr @memcpy(...)

declare ptr @memset(...)

define linkonce_odr i64 @_Z7printlnP2i8(ptr %0) {
entry:
  %call = call i32 @puts(ptr %0)
  ret i64 0
}

define linkonce_odr i64 @_Z5printP2i8(ptr %0) {
entry:
  %call = call i32 @puts(ptr %0)
  ret i64 0
}

declare i32 @puts(ptr)

declare void @_ZN6server3runEv()

define i64 @main(i64 %0, ptr %1) {
entry:
  call void @_ZN6server3runEv()
  ret i64 0
}

define i64 @scaly_build_stamp() {
entry:
  ret i64 1298144578545528496
}
