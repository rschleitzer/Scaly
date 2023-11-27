; ModuleID = '/Users/ralf/repos/Scaly/scaly/memory/PageList_Module.cpp'
source_filename = "/Users/ralf/repos/Scaly/scaly/memory/PageList_Module.cpp"
target datalayout = "e-m:o-i64:64-i128:128-n32:64-S128"
target triple = "arm64-apple-macosx14.0.0"

%"struct.scaly::memory::PageNode" = type { ptr, ptr }
%"struct.scaly::memory::PageListIterator" = type { ptr }

; Function Attrs: mustprogress nofree norecurse nosync nounwind ssp willreturn memory(argmem: write) uwtable(sync)
define noundef nonnull ptr @_ZN5scaly6memory8PageListC2Ev(ptr noundef nonnull returned writeonly align 8 dereferenceable(8) %this) unnamed_addr #0 align 2 {
entry:
  store ptr null, ptr %this, align 8, !tbaa !5
  ret ptr %this
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind ssp willreturn memory(argmem: write) uwtable(sync)
define noundef nonnull ptr @_ZN5scaly6memory8PageListC1Ev(ptr noundef nonnull returned writeonly align 8 dereferenceable(8) %this) unnamed_addr #0 align 2 {
entry:
  store ptr null, ptr %this, align 8, !tbaa !5
  ret ptr %this
}

; Function Attrs: mustprogress ssp uwtable(sync)
define void @_ZN5scaly6memory8PageList3addEPNS0_4PageES3_(ptr nocapture noundef nonnull align 8 dereferenceable(8) %this, ptr noundef %_rp, ptr noundef %element) local_unnamed_addr #1 align 2 {
entry:
  %call = tail call noundef ptr @_ZN5scaly6memory6ObjectnwEmmPNS0_4PageE(i64 noundef 16, i64 noundef 8, ptr noundef %_rp)
  %0 = load ptr, ptr %this, align 8, !tbaa !5
  %call2 = tail call noundef ptr @_ZN5scaly6memory8PageNodeC1EPNS0_4PageEPS1_(ptr noundef nonnull align 8 dereferenceable(16) %call, ptr noundef %element, ptr noundef %0)
  store ptr %call, ptr %this, align 8, !tbaa !5
  ret void
}

; Function Attrs: mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: readwrite)
declare void @llvm.lifetime.start.p0(i64 immarg, ptr nocapture) #2

declare noundef ptr @_ZN5scaly6memory6ObjectnwEmmPNS0_4PageE(i64 noundef, i64 noundef, ptr noundef) local_unnamed_addr #3

declare noundef ptr @_ZN5scaly6memory8PageNodeC1EPNS0_4PageEPS1_(ptr noundef nonnull returned align 8 dereferenceable(16), ptr noundef, ptr noundef) unnamed_addr #3

; Function Attrs: mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: readwrite)
declare void @llvm.lifetime.end.p0(i64 immarg, ptr nocapture) #2

; Function Attrs: mustprogress nofree norecurse nosync nounwind ssp memory(readwrite, inaccessiblemem: none) uwtable(sync)
define noundef zeroext i1 @_ZN5scaly6memory8PageList6removeEPNS0_4PageE(ptr nocapture noundef nonnull align 8 dereferenceable(8) %this, ptr noundef readnone %element) local_unnamed_addr #4 align 2 {
entry:
  %node.024 = load ptr, ptr %this, align 8, !tbaa !10
  %cmp.not25.not = icmp eq ptr %node.024, null
  br i1 %cmp.not25.not, label %cleanup, label %while.body.preheader

while.body.preheader:                             ; preds = %entry
  %0 = load ptr, ptr %node.024, align 8, !tbaa !11
  %cmp236 = icmp eq ptr %0, %element
  br i1 %cmp236, label %if.then, label %if.end11

while.body:                                       ; preds = %if.end11
  %1 = load ptr, ptr %node.0, align 8, !tbaa !11
  %cmp2 = icmp eq ptr %1, %element
  br i1 %cmp2, label %if.then, label %if.end11, !llvm.loop !13

if.then:                                          ; preds = %while.body, %while.body.preheader
  %node.027.lcssa = phi ptr [ %node.024, %while.body.preheader ], [ %node.0, %while.body ]
  %previous_node.026.lcssa = phi ptr [ null, %while.body.preheader ], [ %node.02737, %while.body ]
  %cmp3.not = icmp eq ptr %previous_node.026.lcssa, null
  br i1 %cmp3.not, label %if.end, label %if.then4

if.then4:                                         ; preds = %if.then
  %next = getelementptr inbounds %"struct.scaly::memory::PageNode", ptr %node.027.lcssa, i64 0, i32 1
  %2 = load ptr, ptr %next, align 8, !tbaa !15
  %next5 = getelementptr inbounds %"struct.scaly::memory::PageNode", ptr %previous_node.026.lcssa, i64 0, i32 1
  store ptr %2, ptr %next5, align 8, !tbaa !15
  br label %if.end

if.end:                                           ; preds = %if.then4, %if.then
  %cmp7 = icmp eq ptr %node.027.lcssa, %node.024
  br i1 %cmp7, label %if.then8, label %cleanup

if.then8:                                         ; preds = %if.end
  store ptr null, ptr %this, align 8, !tbaa !5
  br label %cleanup

if.end11:                                         ; preds = %while.body.preheader, %while.body
  %node.02737 = phi ptr [ %node.0, %while.body ], [ %node.024, %while.body.preheader ]
  %next12 = getelementptr inbounds %"struct.scaly::memory::PageNode", ptr %node.02737, i64 0, i32 1
  %node.0 = load ptr, ptr %next12, align 8, !tbaa !10
  %cmp.not.not = icmp eq ptr %node.0, null
  br i1 %cmp.not.not, label %cleanup, label %while.body, !llvm.loop !13

cleanup:                                          ; preds = %if.end11, %entry, %if.end, %if.then8
  %cmp.not21 = phi i1 [ true, %if.end ], [ true, %if.then8 ], [ false, %entry ], [ false, %if.end11 ]
  ret i1 %cmp.not21
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind ssp willreturn memory(argmem: read) uwtable(sync)
define noundef ptr @_ZN5scaly6memory8PageList8get_headEv(ptr nocapture noundef nonnull readonly align 8 dereferenceable(8) %this) local_unnamed_addr #5 align 2 {
entry:
  %0 = load ptr, ptr %this, align 8, !tbaa !5
  ret ptr %0
}

; Function Attrs: mustprogress ssp uwtable(sync)
define noundef i64 @_ZN5scaly6memory8PageList5countEv(ptr nocapture noundef nonnull readonly align 8 dereferenceable(8) %this) local_unnamed_addr #1 align 2 {
entry:
  %list_iterator = alloca %"struct.scaly::memory::PageListIterator", align 8
  call void @llvm.lifetime.start.p0(i64 8, ptr nonnull %list_iterator) #6
  %0 = load ptr, ptr %this, align 8, !tbaa !5
  %call.i = call noundef ptr @_ZN5scaly6memory16PageListIteratorC1EPNS0_8PageNodeE(ptr noundef nonnull align 8 dereferenceable(8) %list_iterator, ptr noundef %0)
  br label %while.cond

while.cond:                                       ; preds = %while.cond, %entry
  %i.0 = phi i64 [ 0, %entry ], [ %inc, %while.cond ]
  %call2 = call noundef ptr @_ZN5scaly6memory16PageListIterator4nextEv(ptr noundef nonnull align 8 dereferenceable(8) %list_iterator)
  %tobool.not = icmp eq ptr %call2, null
  %inc = add i64 %i.0, 1
  br i1 %tobool.not, label %while.end, label %while.cond

while.end:                                        ; preds = %while.cond
  call void @llvm.lifetime.end.p0(i64 8, ptr nonnull %list_iterator) #6
  ret i64 %i.0
}

; Function Attrs: mustprogress ssp uwtable(sync)
define i64 @_ZN5scaly6memory8PageList12get_iteratorEv(ptr nocapture noundef nonnull readonly align 8 dereferenceable(8) %this) local_unnamed_addr #1 align 2 {
entry:
  %retval = alloca %"struct.scaly::memory::PageListIterator", align 8
  %0 = load ptr, ptr %this, align 8, !tbaa !5
  %call = call noundef ptr @_ZN5scaly6memory16PageListIteratorC1EPNS0_8PageNodeE(ptr noundef nonnull align 8 dereferenceable(8) %retval, ptr noundef %0)
  %1 = load ptr, ptr %retval, align 8
  %coerce.val.pi = ptrtoint ptr %1 to i64
  ret i64 %coerce.val.pi
}

declare noundef ptr @_ZN5scaly6memory16PageListIterator4nextEv(ptr noundef nonnull align 8 dereferenceable(8)) local_unnamed_addr #3

declare noundef ptr @_ZN5scaly6memory16PageListIteratorC1EPNS0_8PageNodeE(ptr noundef nonnull returned align 8 dereferenceable(8), ptr noundef) unnamed_addr #3

attributes #0 = { mustprogress nofree norecurse nosync nounwind ssp willreturn memory(argmem: write) uwtable(sync) "frame-pointer"="non-leaf" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+crc,+dotprod,+fp-armv8,+fp16fml,+fullfp16,+lse,+neon,+ras,+rcpc,+rdm,+sha2,+sha3,+v8.1a,+v8.2a,+v8.3a,+v8.4a,+v8.5a,+v8a,+zcm,+zcz" }
attributes #1 = { mustprogress ssp uwtable(sync) "frame-pointer"="non-leaf" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+crc,+dotprod,+fp-armv8,+fp16fml,+fullfp16,+lse,+neon,+ras,+rcpc,+rdm,+sha2,+sha3,+v8.1a,+v8.2a,+v8.3a,+v8.4a,+v8.5a,+v8a,+zcm,+zcz" }
attributes #2 = { mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: readwrite) }
attributes #3 = { "frame-pointer"="non-leaf" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+crc,+dotprod,+fp-armv8,+fp16fml,+fullfp16,+lse,+neon,+ras,+rcpc,+rdm,+sha2,+sha3,+v8.1a,+v8.2a,+v8.3a,+v8.4a,+v8.5a,+v8a,+zcm,+zcz" }
attributes #4 = { mustprogress nofree norecurse nosync nounwind ssp memory(readwrite, inaccessiblemem: none) uwtable(sync) "frame-pointer"="non-leaf" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+crc,+dotprod,+fp-armv8,+fp16fml,+fullfp16,+lse,+neon,+ras,+rcpc,+rdm,+sha2,+sha3,+v8.1a,+v8.2a,+v8.3a,+v8.4a,+v8.5a,+v8a,+zcm,+zcz" }
attributes #5 = { mustprogress nofree norecurse nosync nounwind ssp willreturn memory(argmem: read) uwtable(sync) "frame-pointer"="non-leaf" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+crc,+dotprod,+fp-armv8,+fp16fml,+fullfp16,+lse,+neon,+ras,+rcpc,+rdm,+sha2,+sha3,+v8.1a,+v8.2a,+v8.3a,+v8.4a,+v8.5a,+v8a,+zcm,+zcz" }
attributes #6 = { nounwind }

!llvm.module.flags = !{!0, !1, !2, !3}
!llvm.ident = !{!4}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"uwtable", i32 1}
!3 = !{i32 7, !"frame-pointer", i32 1}
!4 = !{!"Homebrew clang version 17.0.5"}
!5 = !{!6, !7, i64 0}
!6 = !{!"_ZTSN5scaly6memory8PageListE", !7, i64 0}
!7 = !{!"any pointer", !8, i64 0}
!8 = !{!"omnipotent char", !9, i64 0}
!9 = !{!"Simple C++ TBAA"}
!10 = !{!7, !7, i64 0}
!11 = !{!12, !7, i64 0}
!12 = !{!"_ZTSN5scaly6memory8PageNodeE", !7, i64 0, !7, i64 8}
!13 = distinct !{!13, !14}
!14 = !{!"llvm.loop.mustprogress"}
!15 = !{!12, !7, i64 8}
