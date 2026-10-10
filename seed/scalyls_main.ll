; ModuleID = 'main'
source_filename = "main"
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

declare i1 @_ZN3cliB6scalycB4v0_210adopt_homeEv()

declare i1 @_ZN6workerB7scalylsB4v0_213run_if_workerEiPPc(i64, ptr)

declare void @_ZN6serverB7scalylsB4v0_23runEv()

define i64 @main(i64 %0, ptr %1) {
entry:
  %call = call i1 @_ZN3cliB6scalycB4v0_210adopt_homeEv()
  %call1 = call i1 @_ZN6workerB7scalylsB4v0_213run_if_workerEiPPc(i64 %0, ptr %1)
  br i1 %call1, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret i64 0

if.end:                                           ; preds = %entry
  call void @scaly_proc_stdio_binary()
  call void @_ZN6serverB7scalylsB4v0_23runEv()
  ret i64 0
}

declare void @scaly_proc_stdio_binary(...)

define i64 @scaly_build_stamp() {
entry:
  ret i64 1818208185719959924
}
