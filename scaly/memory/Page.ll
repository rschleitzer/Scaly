; ModuleID = '/Users/ralf/repos/Scaly/scaly/memory/Page_Module.cpp'
source_filename = "/Users/ralf/repos/Scaly/scaly/memory/Page_Module.cpp"
target datalayout = "e-m:o-i64:64-i128:128-n32:64-S128"
target triple = "arm64-apple-macosx14.0.0"

%"struct.scaly::memory::PageList" = type { ptr }
%"struct.scaly::memory::Page" = type { ptr, ptr, ptr, %"struct.scaly::memory::PageList" }
%"struct.scaly::memory::PageListIterator" = type { ptr }

; Function Attrs: mustprogress ssp uwtable
define void @_ZN5scaly6memory4Page5resetEv(ptr noundef nonnull align 8 dereferenceable(32) %this) local_unnamed_addr #0 align 2 {
entry:
  %ref.tmp = alloca %"struct.scaly::memory::PageList", align 8
  %current_page = getelementptr inbounds %"struct.scaly::memory::Page", ptr %this, i64 0, i32 1
  %add.ptr = getelementptr inbounds %"struct.scaly::memory::Page", ptr %this, i64 1
  tail call void @llvm.memset.p0.i64(ptr noundef nonnull align 8 dereferenceable(16) %current_page, i8 0, i64 16, i1 false)
  store ptr %add.ptr, ptr %this, align 8, !tbaa !5
  call void @llvm.lifetime.start.p0(i64 8, ptr nonnull %ref.tmp) #10
  %call = call noundef ptr @_ZN5scaly6memory8PageListC1Ev(ptr noundef nonnull align 8 dereferenceable(8) %ref.tmp)
  %exclusive_pages = getelementptr inbounds %"struct.scaly::memory::Page", ptr %this, i64 0, i32 3
  %0 = load i64, ptr %ref.tmp, align 8
  store i64 %0, ptr %exclusive_pages, align 8
  call void @llvm.lifetime.end.p0(i64 8, ptr nonnull %ref.tmp) #10
  ret void
}

; Function Attrs: argmemonly mustprogress nocallback nofree nosync nounwind willreturn
declare void @llvm.lifetime.start.p0(i64 immarg, ptr nocapture) #1

declare noundef ptr @_ZN5scaly6memory8PageListC1Ev(ptr noundef nonnull returned align 8 dereferenceable(8)) unnamed_addr #2

; Function Attrs: argmemonly mustprogress nocallback nofree nosync nounwind willreturn
declare void @llvm.lifetime.end.p0(i64 immarg, ptr nocapture) #1

; Function Attrs: argmemonly mustprogress nofree norecurse nosync nounwind readonly ssp willreturn uwtable
define noundef zeroext i1 @_ZN5scaly6memory4Page12is_oversizedEv(ptr nocapture noundef nonnull readonly align 8 dereferenceable(32) %this) local_unnamed_addr #3 align 2 {
entry:
  %0 = load ptr, ptr %this, align 8, !tbaa !5
  %cmp = icmp eq ptr %0, null
  ret i1 %cmp
}

; Function Attrs: argmemonly mustprogress nofree norecurse nosync nounwind readonly ssp willreturn uwtable
define noundef i64 @_ZN5scaly6memory4Page12get_capacityEm(ptr noundef nonnull align 8 dereferenceable(32) %this, i64 noundef %align) local_unnamed_addr #3 align 2 {
entry:
  %0 = load ptr, ptr %this, align 8, !tbaa !5
  %1 = ptrtoint ptr %0 to i64
  %add = add i64 %align, -1
  %sub = add i64 %add, %1
  %neg = sub i64 0, %align
  %and = and i64 %sub, %neg
  %2 = ptrtoint ptr %this to i64
  %add3 = add i64 %2, 4096
  %sub4 = sub i64 %add3, %and
  ret i64 %sub4
}

; Function Attrs: mustprogress ssp uwtable
define noundef ptr @_ZN5scaly6memory4Page12allocate_rawEmm(ptr noundef nonnull align 8 dereferenceable(32) %this, i64 noundef %size, i64 noundef %align) local_unnamed_addr #0 align 2 {
entry:
  %ref.tmp.i.i = alloca %"struct.scaly::memory::PageList", align 8
  %0 = load ptr, ptr %this, align 8, !tbaa !5
  %1 = ptrtoint ptr %0 to i64
  %add = add i64 %align, -1
  %sub = add i64 %add, %1
  %neg = sub i64 0, %align
  %and = and i64 %sub, %neg
  %2 = ptrtoint ptr %this to i64
  %add3 = add i64 %2, 4096
  %sub4 = sub i64 %add3, %and
  %cmp = icmp ult i64 %sub4, %size
  br i1 %cmp, label %if.then, label %if.end22

if.then:                                          ; preds = %entry
  %3 = add i64 %size, -4065
  %cmp6 = icmp ult i64 %3, -4097
  br i1 %cmp6, label %if.then7, label %if.end

if.then7:                                         ; preds = %if.then
  %sub.i = add i64 %size, 4127
  %and.i = and i64 %sub.i, -4096
  %call.i = tail call align 4096 ptr @aligned_alloc(i64 noundef 4096, i64 noundef %and.i) #11
  %cmp.i = icmp eq ptr %call.i, null
  br i1 %cmp.i, label %if.then.i, label %_ZN5scaly6memory4Page18allocate_oversizedEm.exit

if.then.i:                                        ; preds = %if.then7
  tail call void @exit(i32 noundef 1) #12
  unreachable

_ZN5scaly6memory4Page18allocate_oversizedEm.exit: ; preds = %if.then7
  store ptr null, ptr %call.i, align 4096, !tbaa !5
  %exclusive_pages.i = getelementptr inbounds %"struct.scaly::memory::Page", ptr %this, i64 0, i32 3
  tail call void @_ZN5scaly6memory8PageList3addEPNS0_4PageES3_(ptr noundef nonnull align 8 dereferenceable(8) %exclusive_pages.i, ptr noundef nonnull %this, ptr noundef nonnull %call.i)
  %add.ptr.i = getelementptr inbounds %"struct.scaly::memory::Page", ptr %call.i, i64 1
  br label %cleanup26

if.end:                                           ; preds = %if.then
  %current_page = getelementptr inbounds %"struct.scaly::memory::Page", ptr %this, i64 0, i32 1
  %4 = load ptr, ptr %current_page, align 8, !tbaa !11
  %cmp8.not = icmp eq ptr %4, null
  br i1 %cmp8.not, label %if.end18, label %if.then9

if.then9:                                         ; preds = %if.end
  %call11 = tail call noundef ptr @_ZN5scaly6memory4Page12allocate_rawEmm(ptr noundef nonnull align 8 dereferenceable(32) %4, i64 noundef %size, i64 noundef %align)
  %5 = ptrtoint ptr %call11 to i64
  %and.i44 = and i64 %5, -4096
  %6 = inttoptr i64 %and.i44 to ptr
  %7 = load ptr, ptr %current_page, align 8, !tbaa !11
  %cmp14.not = icmp eq ptr %7, %6
  br i1 %cmp14.not, label %cleanup26, label %if.then15

if.then15:                                        ; preds = %if.then9
  store ptr %6, ptr %current_page, align 8, !tbaa !11
  br label %cleanup26

if.end18:                                         ; preds = %if.end
  %call.i45 = tail call align 4096 dereferenceable_or_null(4096) ptr @aligned_alloc(i64 noundef 4096, i64 noundef 4096) #11
  %current_page.i.i = getelementptr inbounds %"struct.scaly::memory::Page", ptr %call.i45, i64 0, i32 1
  %add.ptr.i.i = getelementptr inbounds %"struct.scaly::memory::Page", ptr %call.i45, i64 1
  tail call void @llvm.memset.p0.i64(ptr noundef nonnull align 8 dereferenceable(16) %current_page.i.i, i8 0, i64 16, i1 false)
  store ptr %add.ptr.i.i, ptr %call.i45, align 4096, !tbaa !5
  call void @llvm.lifetime.start.p0(i64 8, ptr nonnull %ref.tmp.i.i) #10
  %call.i.i = call noundef ptr @_ZN5scaly6memory8PageListC1Ev(ptr noundef nonnull align 8 dereferenceable(8) %ref.tmp.i.i)
  %exclusive_pages.i.i = getelementptr inbounds %"struct.scaly::memory::Page", ptr %call.i45, i64 0, i32 3
  %8 = load i64, ptr %ref.tmp.i.i, align 8
  store i64 %8, ptr %exclusive_pages.i.i, align 8
  call void @llvm.lifetime.end.p0(i64 8, ptr nonnull %ref.tmp.i.i) #10
  store ptr %call.i45, ptr %current_page, align 8, !tbaa !11
  %next_page = getelementptr inbounds %"struct.scaly::memory::Page", ptr %this, i64 0, i32 2
  store ptr %call.i45, ptr %next_page, align 8, !tbaa !12
  %call21 = call noundef ptr @_ZN5scaly6memory4Page12allocate_rawEmm(ptr noundef nonnull align 8 dereferenceable(32) %call.i45, i64 noundef %size, i64 noundef %align)
  br label %cleanup26

if.end22:                                         ; preds = %entry
  %add23 = add i64 %and, %size
  %9 = inttoptr i64 %add23 to ptr
  store ptr %9, ptr %this, align 8, !tbaa !5
  %10 = inttoptr i64 %and to ptr
  br label %cleanup26

cleanup26:                                        ; preds = %_ZN5scaly6memory4Page18allocate_oversizedEm.exit, %if.end18, %if.then15, %if.then9, %if.end22
  %retval.1 = phi ptr [ %10, %if.end22 ], [ %add.ptr.i, %_ZN5scaly6memory4Page18allocate_oversizedEm.exit ], [ %call21, %if.end18 ], [ %call11, %if.then15 ], [ %call11, %if.then9 ]
  ret ptr %retval.1
}

; Function Attrs: mustprogress ssp uwtable
define noundef nonnull ptr @_ZN5scaly6memory4Page18allocate_oversizedEm(ptr noundef nonnull align 8 dereferenceable(32) %this, i64 noundef %size) local_unnamed_addr #0 align 2 {
entry:
  %sub = add i64 %size, 4095
  %and = and i64 %sub, -4096
  %call = tail call align 4096 ptr @aligned_alloc(i64 noundef 4096, i64 noundef %and) #11
  %cmp = icmp eq ptr %call, null
  br i1 %cmp, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  tail call void @exit(i32 noundef 1) #12
  unreachable

if.end:                                           ; preds = %entry
  store ptr null, ptr %call, align 4096, !tbaa !5
  %exclusive_pages = getelementptr inbounds %"struct.scaly::memory::Page", ptr %this, i64 0, i32 3
  tail call void @_ZN5scaly6memory8PageList3addEPNS0_4PageES3_(ptr noundef nonnull align 8 dereferenceable(8) %exclusive_pages, ptr noundef nonnull %this, ptr noundef nonnull %call)
  %add.ptr = getelementptr inbounds %"struct.scaly::memory::Page", ptr %call, i64 1
  ret ptr %add.ptr
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind readnone ssp willreturn uwtable
define noundef ptr @_ZN5scaly6memory4Page3getEPv(ptr noundef %address) local_unnamed_addr #4 align 2 {
entry:
  %0 = ptrtoint ptr %address to i64
  %and = and i64 %0, -4096
  %1 = inttoptr i64 %and to ptr
  ret ptr %1
}

; Function Attrs: mustprogress ssp uwtable
define noundef ptr @_ZN5scaly6memory4Page13allocate_pageEv() local_unnamed_addr #0 align 2 {
entry:
  %ref.tmp.i = alloca %"struct.scaly::memory::PageList", align 8
  %call = tail call align 4096 dereferenceable_or_null(4096) ptr @aligned_alloc(i64 noundef 4096, i64 noundef 4096) #11
  %current_page.i = getelementptr inbounds %"struct.scaly::memory::Page", ptr %call, i64 0, i32 1
  %add.ptr.i = getelementptr inbounds %"struct.scaly::memory::Page", ptr %call, i64 1
  tail call void @llvm.memset.p0.i64(ptr noundef nonnull align 8 dereferenceable(16) %current_page.i, i8 0, i64 16, i1 false)
  store ptr %add.ptr.i, ptr %call, align 4096, !tbaa !5
  call void @llvm.lifetime.start.p0(i64 8, ptr nonnull %ref.tmp.i) #10
  %call.i = call noundef ptr @_ZN5scaly6memory8PageListC1Ev(ptr noundef nonnull align 8 dereferenceable(8) %ref.tmp.i)
  %exclusive_pages.i = getelementptr inbounds %"struct.scaly::memory::Page", ptr %call, i64 0, i32 3
  %0 = load i64, ptr %ref.tmp.i, align 8
  store i64 %0, ptr %exclusive_pages.i, align 8
  call void @llvm.lifetime.end.p0(i64 8, ptr nonnull %ref.tmp.i) #10
  ret ptr %call
}

; Function Attrs: inaccessiblememonly mustprogress nofree nounwind willreturn allockind("alloc,uninitialized,aligned") allocsize(1)
declare noalias noundef ptr @aligned_alloc(i64 allocalign noundef, i64 noundef) local_unnamed_addr #5

; Function Attrs: noreturn
declare void @exit(i32 noundef) local_unnamed_addr #6

declare void @_ZN5scaly6memory8PageList3addEPNS0_4PageES3_(ptr noundef nonnull align 8 dereferenceable(8), ptr noundef, ptr noundef) local_unnamed_addr #2

; Function Attrs: mustprogress ssp uwtable
define noundef ptr @_ZN5scaly6memory4Page23allocate_exclusive_pageEv(ptr noundef nonnull align 8 dereferenceable(32) %this) local_unnamed_addr #0 align 2 {
entry:
  %ref.tmp.i.i = alloca %"struct.scaly::memory::PageList", align 8
  %call.i = tail call align 4096 dereferenceable_or_null(4096) ptr @aligned_alloc(i64 noundef 4096, i64 noundef 4096) #11
  %current_page.i.i = getelementptr inbounds %"struct.scaly::memory::Page", ptr %call.i, i64 0, i32 1
  %add.ptr.i.i = getelementptr inbounds %"struct.scaly::memory::Page", ptr %call.i, i64 1
  tail call void @llvm.memset.p0.i64(ptr noundef nonnull align 8 dereferenceable(16) %current_page.i.i, i8 0, i64 16, i1 false)
  store ptr %add.ptr.i.i, ptr %call.i, align 4096, !tbaa !5
  call void @llvm.lifetime.start.p0(i64 8, ptr nonnull %ref.tmp.i.i) #10
  %call.i.i = call noundef ptr @_ZN5scaly6memory8PageListC1Ev(ptr noundef nonnull align 8 dereferenceable(8) %ref.tmp.i.i)
  %exclusive_pages.i.i = getelementptr inbounds %"struct.scaly::memory::Page", ptr %call.i, i64 0, i32 3
  %0 = load i64, ptr %ref.tmp.i.i, align 8
  store i64 %0, ptr %exclusive_pages.i.i, align 8
  call void @llvm.lifetime.end.p0(i64 8, ptr nonnull %ref.tmp.i.i) #10
  %exclusive_pages = getelementptr inbounds %"struct.scaly::memory::Page", ptr %this, i64 0, i32 3
  call void @_ZN5scaly6memory8PageList3addEPNS0_4PageES3_(ptr noundef nonnull align 8 dereferenceable(8) %exclusive_pages, ptr noundef nonnull %call.i, ptr noundef nonnull %call.i)
  ret ptr %call.i
}

; Function Attrs: mustprogress ssp uwtable
define void @_ZN5scaly6memory4Page21deallocate_extensionsEv(ptr noundef nonnull align 8 dereferenceable(32) %this) local_unnamed_addr #0 align 2 {
entry:
  %_exclusive_pages_iterator = alloca %"struct.scaly::memory::PageListIterator", align 8
  %0 = load ptr, ptr %this, align 8, !tbaa !5
  %cmp = icmp eq ptr %0, null
  br i1 %cmp, label %return, label %if.end

if.end:                                           ; preds = %entry
  call void @llvm.lifetime.start.p0(i64 8, ptr nonnull %_exclusive_pages_iterator) #10
  %exclusive_pages = getelementptr inbounds %"struct.scaly::memory::Page", ptr %this, i64 0, i32 3
  %call = tail call i64 @_ZN5scaly6memory8PageList12get_iteratorEv(ptr noundef nonnull align 8 dereferenceable(8) %exclusive_pages)
  %coerce.val.ip = inttoptr i64 %call to ptr
  store ptr %coerce.val.ip, ptr %_exclusive_pages_iterator, align 8
  %call215 = call noundef ptr @_ZN5scaly6memory16PageListIterator4nextEv(ptr noundef nonnull align 8 dereferenceable(8) %_exclusive_pages_iterator)
  %tobool.not16 = icmp eq ptr %call215, null
  br i1 %tobool.not16, label %while.end, label %cleanup

cleanup:                                          ; preds = %if.end, %cleanup
  %call217 = phi ptr [ %call2, %cleanup ], [ %call215, %if.end ]
  %1 = load ptr, ptr %call217, align 8, !tbaa !13
  call void @_ZN5scaly6memory4Page21deallocate_extensionsEv(ptr noundef nonnull align 8 dereferenceable(32) %1)
  call void @free(ptr noundef nonnull %1)
  %call2 = call noundef ptr @_ZN5scaly6memory16PageListIterator4nextEv(ptr noundef nonnull align 8 dereferenceable(8) %_exclusive_pages_iterator)
  %tobool.not = icmp eq ptr %call2, null
  br i1 %tobool.not, label %while.end, label %cleanup

while.end:                                        ; preds = %cleanup, %if.end
  %next_page = getelementptr inbounds %"struct.scaly::memory::Page", ptr %this, i64 0, i32 2
  %2 = load ptr, ptr %next_page, align 8, !tbaa !12
  %cmp4.not18 = icmp eq ptr %2, null
  br i1 %cmp4.not18, label %while.end8, label %while.body5

while.body5:                                      ; preds = %while.end, %while.body5
  %page.019 = phi ptr [ %3, %while.body5 ], [ %2, %while.end ]
  %next_page7 = getelementptr inbounds %"struct.scaly::memory::Page", ptr %page.019, i64 0, i32 2
  %3 = load ptr, ptr %next_page7, align 8, !tbaa !12
  call void @free(ptr noundef nonnull %page.019)
  %cmp4.not = icmp eq ptr %3, null
  br i1 %cmp4.not, label %while.end8, label %while.body5, !llvm.loop !14

while.end8:                                       ; preds = %while.body5, %while.end
  call void @llvm.lifetime.end.p0(i64 8, ptr nonnull %_exclusive_pages_iterator) #10
  br label %return

return:                                           ; preds = %entry, %while.end8
  ret void
}

declare i64 @_ZN5scaly6memory8PageList12get_iteratorEv(ptr noundef nonnull align 8 dereferenceable(8)) local_unnamed_addr #2

declare noundef ptr @_ZN5scaly6memory16PageListIterator4nextEv(ptr noundef nonnull align 8 dereferenceable(8)) local_unnamed_addr #2

; Function Attrs: mustprogress nounwind ssp willreturn uwtable
define void @_ZN5scaly6memory4Page6forgetEv(ptr nocapture noundef nonnull align 8 dereferenceable(32) %this) local_unnamed_addr #7 align 2 {
entry:
  tail call void @free(ptr noundef nonnull %this)
  ret void
}

; Function Attrs: inaccessiblemem_or_argmemonly mustprogress nounwind willreturn allockind("free")
declare void @free(ptr allocptr nocapture noundef) local_unnamed_addr #8

; Function Attrs: mustprogress ssp uwtable
define void @_ZN5scaly6memory4Page25deallocate_exclusive_pageEPS1_(ptr noundef nonnull align 8 dereferenceable(32) %this, ptr noundef nonnull %page) local_unnamed_addr #0 align 2 {
entry:
  tail call void @_ZN5scaly6memory4Page21deallocate_extensionsEv(ptr noundef nonnull align 8 dereferenceable(32) %page)
  tail call void @free(ptr noundef nonnull %page)
  %exclusive_pages = getelementptr inbounds %"struct.scaly::memory::Page", ptr %this, i64 0, i32 3
  %call = tail call noundef zeroext i1 @_ZN5scaly6memory8PageList6removeEPNS0_4PageE(ptr noundef nonnull align 8 dereferenceable(8) %exclusive_pages, ptr noundef nonnull %page)
  br i1 %call, label %if.end, label %if.then

if.then:                                          ; preds = %entry
  tail call void @exit(i32 noundef 2) #12
  unreachable

if.end:                                           ; preds = %entry
  ret void
}

declare noundef zeroext i1 @_ZN5scaly6memory8PageList6removeEPNS0_4PageE(ptr noundef nonnull align 8 dereferenceable(8), ptr noundef) local_unnamed_addr #2

; Function Attrs: argmemonly nocallback nofree nounwind willreturn writeonly
declare void @llvm.memset.p0.i64(ptr nocapture writeonly, i8, i64, i1 immarg) #9

attributes #0 = { mustprogress ssp uwtable "frame-pointer"="non-leaf" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+crc,+crypto,+dotprod,+fp-armv8,+fp16fml,+fullfp16,+lse,+neon,+ras,+rcpc,+rdm,+sha2,+sha3,+sm4,+v8.5a,+zcm,+zcz" }
attributes #1 = { argmemonly mustprogress nocallback nofree nosync nounwind willreturn }
attributes #2 = { "frame-pointer"="non-leaf" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+crc,+crypto,+dotprod,+fp-armv8,+fp16fml,+fullfp16,+lse,+neon,+ras,+rcpc,+rdm,+sha2,+sha3,+sm4,+v8.5a,+zcm,+zcz" }
attributes #3 = { argmemonly mustprogress nofree norecurse nosync nounwind readonly ssp willreturn uwtable "frame-pointer"="non-leaf" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+crc,+crypto,+dotprod,+fp-armv8,+fp16fml,+fullfp16,+lse,+neon,+ras,+rcpc,+rdm,+sha2,+sha3,+sm4,+v8.5a,+zcm,+zcz" }
attributes #4 = { mustprogress nofree norecurse nosync nounwind readnone ssp willreturn uwtable "frame-pointer"="non-leaf" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+crc,+crypto,+dotprod,+fp-armv8,+fp16fml,+fullfp16,+lse,+neon,+ras,+rcpc,+rdm,+sha2,+sha3,+sm4,+v8.5a,+zcm,+zcz" }
attributes #5 = { inaccessiblememonly mustprogress nofree nounwind willreturn allockind("alloc,uninitialized,aligned") allocsize(1) "alloc-family"="malloc" "frame-pointer"="non-leaf" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+crc,+crypto,+dotprod,+fp-armv8,+fp16fml,+fullfp16,+lse,+neon,+ras,+rcpc,+rdm,+sha2,+sha3,+sm4,+v8.5a,+zcm,+zcz" }
attributes #6 = { noreturn "frame-pointer"="non-leaf" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+crc,+crypto,+dotprod,+fp-armv8,+fp16fml,+fullfp16,+lse,+neon,+ras,+rcpc,+rdm,+sha2,+sha3,+sm4,+v8.5a,+zcm,+zcz" }
attributes #7 = { mustprogress nounwind ssp willreturn uwtable "frame-pointer"="non-leaf" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+crc,+crypto,+dotprod,+fp-armv8,+fp16fml,+fullfp16,+lse,+neon,+ras,+rcpc,+rdm,+sha2,+sha3,+sm4,+v8.5a,+zcm,+zcz" }
attributes #8 = { inaccessiblemem_or_argmemonly mustprogress nounwind willreturn allockind("free") "alloc-family"="malloc" "frame-pointer"="non-leaf" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+crc,+crypto,+dotprod,+fp-armv8,+fp16fml,+fullfp16,+lse,+neon,+ras,+rcpc,+rdm,+sha2,+sha3,+sm4,+v8.5a,+zcm,+zcz" }
attributes #9 = { argmemonly nocallback nofree nounwind willreturn writeonly }
attributes #10 = { nounwind }
attributes #11 = { allocsize(1) }
attributes #12 = { noreturn }

!llvm.module.flags = !{!0, !1, !2, !3}
!llvm.ident = !{!4}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 7, !"PIC Level", i32 2}
!2 = !{i32 7, !"uwtable", i32 2}
!3 = !{i32 7, !"frame-pointer", i32 1}
!4 = !{!"Homebrew clang version 15.0.7"}
!5 = !{!6, !7, i64 0}
!6 = !{!"_ZTSN5scaly6memory4PageE", !7, i64 0, !7, i64 8, !7, i64 16, !10, i64 24}
!7 = !{!"any pointer", !8, i64 0}
!8 = !{!"omnipotent char", !9, i64 0}
!9 = !{!"Simple C++ TBAA"}
!10 = !{!"_ZTSN5scaly6memory8PageListE", !7, i64 0}
!11 = !{!6, !7, i64 8}
!12 = !{!6, !7, i64 16}
!13 = !{!7, !7, i64 0}
!14 = distinct !{!14, !15}
!15 = !{!"llvm.loop.mustprogress"}
