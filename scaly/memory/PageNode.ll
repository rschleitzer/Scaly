; ModuleID = '/Users/ralf/repos/Scaly/scaly/memory/PageNode_Module.cpp'
source_filename = "/Users/ralf/repos/Scaly/scaly/memory/PageNode_Module.cpp"
target datalayout = "e-m:o-i64:64-i128:128-n32:64-S128"
target triple = "arm64-apple-macosx14.0.0"

%"struct.scaly::memory::PageNode" = type { ptr, ptr }

; Function Attrs: mustprogress nofree norecurse nosync nounwind ssp willreturn memory(argmem: write) uwtable(sync)
define noundef nonnull ptr @_ZN5scaly6memory8PageNodeC2EPNS0_4PageEPS1_(ptr noundef nonnull returned writeonly align 8 dereferenceable(16) %this, ptr noundef %thePage, ptr noundef %nextNode) unnamed_addr #0 align 2 {
entry:
  store ptr %thePage, ptr %this, align 8, !tbaa !5
  %next = getelementptr inbounds %"struct.scaly::memory::PageNode", ptr %this, i64 0, i32 1
  store ptr %nextNode, ptr %next, align 8, !tbaa !10
  ret ptr %this
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind ssp willreturn memory(argmem: write) uwtable(sync)
define noundef nonnull ptr @_ZN5scaly6memory8PageNodeC1EPNS0_4PageEPS1_(ptr noundef nonnull returned writeonly align 8 dereferenceable(16) %this, ptr noundef %thePage, ptr noundef %nextNode) unnamed_addr #0 align 2 {
entry:
  store ptr %thePage, ptr %this, align 8, !tbaa !5
  %next.i = getelementptr inbounds %"struct.scaly::memory::PageNode", ptr %this, i64 0, i32 1
  store ptr %nextNode, ptr %next.i, align 8, !tbaa !10
  ret ptr %this
}

attributes #0 = { mustprogress nofree norecurse nosync nounwind ssp willreturn memory(argmem: write) uwtable(sync) "frame-pointer"="non-leaf" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+crc,+dotprod,+fp-armv8,+fp16fml,+fullfp16,+lse,+neon,+ras,+rcpc,+rdm,+sha2,+sha3,+v8.1a,+v8.2a,+v8.3a,+v8.4a,+v8.5a,+v8a,+zcm,+zcz" }

!llvm.module.flags = !{!0, !1, !2, !3}
!llvm.ident = !{!4}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"uwtable", i32 1}
!3 = !{i32 7, !"frame-pointer", i32 1}
!4 = !{!"Homebrew clang version 17.0.5"}
!5 = !{!6, !7, i64 0}
!6 = !{!"_ZTSN5scaly6memory8PageNodeE", !7, i64 0, !7, i64 8}
!7 = !{!"any pointer", !8, i64 0}
!8 = !{!"omnipotent char", !9, i64 0}
!9 = !{!"Simple C++ TBAA"}
!10 = !{!6, !7, i64 8}
