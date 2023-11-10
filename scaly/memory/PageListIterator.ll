; ModuleID = '/Users/ralf/repos/Scaly/scaly/memory/PageListIterator_Module.cpp'
source_filename = "/Users/ralf/repos/Scaly/scaly/memory/PageListIterator_Module.cpp"
target datalayout = "e-m:o-i64:64-i128:128-n32:64-S128"
target triple = "arm64-apple-macosx14.0.0"

%"struct.scaly::memory::PageNode" = type { ptr, ptr }

; Function Attrs: argmemonly mustprogress nofree norecurse nosync nounwind ssp willreturn writeonly uwtable
define noundef nonnull ptr @_ZN5scaly6memory16PageListIteratorC2EPNS0_8PageNodeE(ptr noundef nonnull returned writeonly align 8 dereferenceable(8) %this, ptr noundef %head) unnamed_addr #0 align 2 {
entry:
  store ptr %head, ptr %this, align 8, !tbaa !5
  ret ptr %this
}

; Function Attrs: argmemonly mustprogress nofree norecurse nosync nounwind ssp willreturn writeonly uwtable
define noundef nonnull ptr @_ZN5scaly6memory16PageListIteratorC1EPNS0_8PageNodeE(ptr noundef nonnull returned writeonly align 8 dereferenceable(8) %this, ptr noundef %head) unnamed_addr #0 align 2 {
entry:
  store ptr %head, ptr %this, align 8, !tbaa !5
  ret ptr %this
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind ssp willreturn uwtable
define noundef ptr @_ZN5scaly6memory16PageListIterator4nextEv(ptr nocapture noundef nonnull align 8 dereferenceable(8) %this) local_unnamed_addr #1 align 2 {
entry:
  %0 = load ptr, ptr %this, align 8, !tbaa !5
  %cmp.not = icmp eq ptr %0, null
  br i1 %cmp.not, label %return, label %if.then

if.then:                                          ; preds = %entry
  %next = getelementptr inbounds %"struct.scaly::memory::PageNode", ptr %0, i64 0, i32 1
  %1 = load ptr, ptr %next, align 8, !tbaa !10
  store ptr %1, ptr %this, align 8, !tbaa !5
  br label %return

return:                                           ; preds = %entry, %if.then
  ret ptr %0
}

attributes #0 = { argmemonly mustprogress nofree norecurse nosync nounwind ssp willreturn writeonly uwtable "frame-pointer"="non-leaf" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+crc,+crypto,+dotprod,+fp-armv8,+fp16fml,+fullfp16,+lse,+neon,+ras,+rcpc,+rdm,+sha2,+sha3,+sm4,+v8.5a,+zcm,+zcz" }
attributes #1 = { mustprogress nofree norecurse nosync nounwind ssp willreturn uwtable "frame-pointer"="non-leaf" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+crc,+crypto,+dotprod,+fp-armv8,+fp16fml,+fullfp16,+lse,+neon,+ras,+rcpc,+rdm,+sha2,+sha3,+sm4,+v8.5a,+zcm,+zcz" }

!llvm.module.flags = !{!0, !1, !2, !3}
!llvm.ident = !{!4}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 7, !"PIC Level", i32 2}
!2 = !{i32 7, !"uwtable", i32 2}
!3 = !{i32 7, !"frame-pointer", i32 1}
!4 = !{!"Homebrew clang version 15.0.7"}
!5 = !{!6, !7, i64 0}
!6 = !{!"_ZTSN5scaly6memory16PageListIteratorE", !7, i64 0}
!7 = !{!"any pointer", !8, i64 0}
!8 = !{!"omnipotent char", !9, i64 0}
!9 = !{!"Simple C++ TBAA"}
!10 = !{!11, !7, i64 8}
!11 = !{!"_ZTSN5scaly6memory8PageNodeE", !7, i64 0, !7, i64 8}
