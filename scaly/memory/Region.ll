; ModuleID = '/Users/ralf/repos/Scaly/scaly/memory/Region_Module.cpp'
source_filename = "/Users/ralf/repos/Scaly/scaly/memory/Region_Module.cpp"
target datalayout = "e-m:o-i64:64-i128:128-n32:64-S128"
target triple = "arm64-apple-macosx14.0.0"

; Function Attrs: mustprogress nofree norecurse nosync nounwind ssp willreturn memory(argmem: write) uwtable(sync)
define noundef nonnull ptr @_ZN5scaly6memory6RegionC2Ev(ptr noundef nonnull returned writeonly align 8 dereferenceable(8) %this) unnamed_addr #0 align 2 {
entry:
  store ptr null, ptr %this, align 8, !tbaa !5
  ret ptr %this
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind ssp willreturn memory(argmem: write) uwtable(sync)
define noundef nonnull ptr @_ZN5scaly6memory6RegionC1Ev(ptr noundef nonnull returned writeonly align 8 dereferenceable(8) %this) unnamed_addr #0 align 2 {
entry:
  store ptr null, ptr %this, align 8, !tbaa !5
  ret ptr %this
}

; Function Attrs: nounwind ssp uwtable(sync)
define noundef nonnull ptr @_ZN5scaly6memory6RegionD2Ev(ptr noundef nonnull readonly returned align 8 dereferenceable(8) %this) unnamed_addr #1 align 2 personality ptr @__gxx_personality_v0 {
entry:
  %0 = load ptr, ptr %this, align 8, !tbaa !5
  %cmp.not = icmp eq ptr %0, null
  br i1 %cmp.not, label %if.end, label %if.then

if.then:                                          ; preds = %entry
  invoke void @_ZN5scaly6memory4Page21deallocate_extensionsEv(ptr noundef nonnull align 8 dereferenceable(32) %0)
          to label %invoke.cont unwind label %terminate.lpad

invoke.cont:                                      ; preds = %if.then
  %1 = load ptr, ptr %this, align 8, !tbaa !5
  invoke void @_ZN5scaly6memory4Page6forgetEv(ptr noundef nonnull align 8 dereferenceable(32) %1)
          to label %if.end unwind label %terminate.lpad

if.end:                                           ; preds = %invoke.cont, %entry
  ret ptr %this

terminate.lpad:                                   ; preds = %invoke.cont, %if.then
  %2 = landingpad { ptr, i32 }
          catch ptr null
  %3 = extractvalue { ptr, i32 } %2, 0
  tail call void @__clang_call_terminate(ptr %3) #5
  unreachable
}

declare void @_ZN5scaly6memory4Page21deallocate_extensionsEv(ptr noundef nonnull align 8 dereferenceable(32)) local_unnamed_addr #2

declare i32 @__gxx_personality_v0(...)

; Function Attrs: noinline noreturn nounwind ssp uwtable(sync)
define linkonce_odr hidden void @__clang_call_terminate(ptr noundef %0) local_unnamed_addr #3 {
  %2 = tail call ptr @__cxa_begin_catch(ptr %0) #6
  tail call void @_ZSt9terminatev() #5
  unreachable
}

declare ptr @__cxa_begin_catch(ptr) local_unnamed_addr

declare void @_ZSt9terminatev() local_unnamed_addr

declare void @_ZN5scaly6memory4Page6forgetEv(ptr noundef nonnull align 8 dereferenceable(32)) local_unnamed_addr #2

; Function Attrs: nounwind ssp uwtable(sync)
define noundef nonnull ptr @_ZN5scaly6memory6RegionD1Ev(ptr noundef nonnull readonly returned align 8 dereferenceable(8) %this) unnamed_addr #1 align 2 personality ptr @__gxx_personality_v0 {
entry:
  %0 = load ptr, ptr %this, align 8, !tbaa !5
  %cmp.not.i = icmp eq ptr %0, null
  br i1 %cmp.not.i, label %_ZN5scaly6memory6RegionD2Ev.exit, label %if.then.i

if.then.i:                                        ; preds = %entry
  invoke void @_ZN5scaly6memory4Page21deallocate_extensionsEv(ptr noundef nonnull align 8 dereferenceable(32) %0)
          to label %invoke.cont.i unwind label %terminate.lpad.i

invoke.cont.i:                                    ; preds = %if.then.i
  %1 = load ptr, ptr %this, align 8, !tbaa !5
  invoke void @_ZN5scaly6memory4Page6forgetEv(ptr noundef nonnull align 8 dereferenceable(32) %1)
          to label %_ZN5scaly6memory6RegionD2Ev.exit unwind label %terminate.lpad.i

terminate.lpad.i:                                 ; preds = %invoke.cont.i, %if.then.i
  %2 = landingpad { ptr, i32 }
          catch ptr null
  %3 = extractvalue { ptr, i32 } %2, 0
  tail call void @__clang_call_terminate(ptr %3) #5
  unreachable

_ZN5scaly6memory6RegionD2Ev.exit:                 ; preds = %entry, %invoke.cont.i
  ret ptr %this
}

; Function Attrs: mustprogress ssp uwtable(sync)
define noundef ptr @_ZN5scaly6memory6Region8get_pageEv(ptr nocapture noundef nonnull align 8 dereferenceable(8) %this) local_unnamed_addr #4 align 2 {
entry:
  %0 = load ptr, ptr %this, align 8, !tbaa !5
  %cmp = icmp eq ptr %0, null
  br i1 %cmp, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %call = tail call noundef ptr @_ZN5scaly6memory4Page13allocate_pageEv()
  store ptr %call, ptr %this, align 8, !tbaa !5
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %1 = phi ptr [ %call, %if.then ], [ %0, %entry ]
  ret ptr %1
}

declare noundef ptr @_ZN5scaly6memory4Page13allocate_pageEv() local_unnamed_addr #2

attributes #0 = { mustprogress nofree norecurse nosync nounwind ssp willreturn memory(argmem: write) uwtable(sync) "frame-pointer"="non-leaf" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+crc,+dotprod,+fp-armv8,+fp16fml,+fullfp16,+lse,+neon,+ras,+rcpc,+rdm,+sha2,+sha3,+v8.1a,+v8.2a,+v8.3a,+v8.4a,+v8.5a,+v8a,+zcm,+zcz" }
attributes #1 = { nounwind ssp uwtable(sync) "frame-pointer"="non-leaf" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+crc,+dotprod,+fp-armv8,+fp16fml,+fullfp16,+lse,+neon,+ras,+rcpc,+rdm,+sha2,+sha3,+v8.1a,+v8.2a,+v8.3a,+v8.4a,+v8.5a,+v8a,+zcm,+zcz" }
attributes #2 = { "frame-pointer"="non-leaf" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+crc,+dotprod,+fp-armv8,+fp16fml,+fullfp16,+lse,+neon,+ras,+rcpc,+rdm,+sha2,+sha3,+v8.1a,+v8.2a,+v8.3a,+v8.4a,+v8.5a,+v8a,+zcm,+zcz" }
attributes #3 = { noinline noreturn nounwind ssp uwtable(sync) "frame-pointer"="non-leaf" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+crc,+dotprod,+fp-armv8,+fp16fml,+fullfp16,+lse,+neon,+ras,+rcpc,+rdm,+sha2,+sha3,+v8.1a,+v8.2a,+v8.3a,+v8.4a,+v8.5a,+v8a,+zcm,+zcz" }
attributes #4 = { mustprogress ssp uwtable(sync) "frame-pointer"="non-leaf" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+crc,+dotprod,+fp-armv8,+fp16fml,+fullfp16,+lse,+neon,+ras,+rcpc,+rdm,+sha2,+sha3,+v8.1a,+v8.2a,+v8.3a,+v8.4a,+v8.5a,+v8a,+zcm,+zcz" }
attributes #5 = { noreturn nounwind }
attributes #6 = { nounwind }

!llvm.module.flags = !{!0, !1, !2, !3}
!llvm.ident = !{!4}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"uwtable", i32 1}
!3 = !{i32 7, !"frame-pointer", i32 1}
!4 = !{!"Homebrew clang version 17.0.5"}
!5 = !{!6, !7, i64 0}
!6 = !{!"_ZTSN5scaly6memory6RegionE", !7, i64 0}
!7 = !{!"any pointer", !8, i64 0}
!8 = !{!"omnipotent char", !9, i64 0}
!9 = !{!"Simple C++ TBAA"}
