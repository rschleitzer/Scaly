; ModuleID = 'Object_Module.cpp'
source_filename = "Object_Module.cpp"
target datalayout = "e-m:o-i64:64-i128:128-n32:64-S128"
target triple = "arm64-apple-macosx14.0.0"

; Function Attrs: mustprogress noinline optnone ssp uwtable(sync)
define noundef ptr @_ZN5scaly6memory6ObjectnwEmmPNS0_4PageE(i64 noundef %size, i64 noundef %align, ptr noundef %page) #0 align 2 !dbg !9 {
entry:
  %size.addr = alloca i64, align 8
  %align.addr = alloca i64, align 8
  %page.addr = alloca ptr, align 8
  %object = alloca ptr, align 8
  store i64 %size, ptr %size.addr, align 8
  call void @llvm.dbg.declare(metadata ptr %size.addr, metadata !116, metadata !DIExpression()), !dbg !117
  store i64 %align, ptr %align.addr, align 8
  call void @llvm.dbg.declare(metadata ptr %align.addr, metadata !118, metadata !DIExpression()), !dbg !119
  store ptr %page, ptr %page.addr, align 8
  call void @llvm.dbg.declare(metadata ptr %page.addr, metadata !120, metadata !DIExpression()), !dbg !121
  call void @llvm.dbg.declare(metadata ptr %object, metadata !122, metadata !DIExpression()), !dbg !123
  %0 = load ptr, ptr %page.addr, align 8, !dbg !124
  %1 = load i64, ptr %size.addr, align 8, !dbg !125
  %2 = load i64, ptr %align.addr, align 8, !dbg !126
  %call = call noundef ptr @_ZN5scaly6memory4Page12allocate_rawEmm(ptr noundef nonnull align 8 dereferenceable(32) %0, i64 noundef %1, i64 noundef %2), !dbg !127
  store ptr %call, ptr %object, align 8, !dbg !123
  %3 = load ptr, ptr %object, align 8, !dbg !128
  %tobool = icmp ne ptr %3, null, !dbg !128
  br i1 %tobool, label %if.end, label %if.then, !dbg !130

if.then:                                          ; preds = %entry
  call void @exit(i32 noundef 16) #4, !dbg !131
  unreachable, !dbg !131

if.end:                                           ; preds = %entry
  %4 = load ptr, ptr %object, align 8, !dbg !132
  ret ptr %4, !dbg !133
}

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

declare noundef ptr @_ZN5scaly6memory4Page12allocate_rawEmm(ptr noundef nonnull align 8 dereferenceable(32), i64 noundef, i64 noundef) #2

; Function Attrs: noreturn
declare void @exit(i32 noundef) #3

; Function Attrs: mustprogress noinline optnone ssp uwtable(sync)
define noundef ptr @_ZN5scaly6memory6Object8get_pageEv(ptr noundef nonnull align 1 dereferenceable(1) %this) #0 align 2 !dbg !134 {
entry:
  %this.addr = alloca ptr, align 8
  store ptr %this, ptr %this.addr, align 8
  call void @llvm.dbg.declare(metadata ptr %this.addr, metadata !135, metadata !DIExpression()), !dbg !137
  %this1 = load ptr, ptr %this.addr, align 8
  %call = call noundef ptr @_ZN5scaly6memory4Page3getEPv(ptr noundef %this1), !dbg !138
  ret ptr %call, !dbg !139
}

declare noundef ptr @_ZN5scaly6memory4Page3getEPv(ptr noundef) #2

attributes #0 = { mustprogress noinline optnone ssp uwtable(sync) "frame-pointer"="non-leaf" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+crc,+dotprod,+fp-armv8,+fp16fml,+fullfp16,+lse,+neon,+ras,+rcpc,+rdm,+sha2,+sha3,+v8.1a,+v8.2a,+v8.3a,+v8.4a,+v8.5a,+v8a,+zcm,+zcz" }
attributes #1 = { nocallback nofree nosync nounwind speculatable willreturn memory(none) }
attributes #2 = { "frame-pointer"="non-leaf" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+crc,+dotprod,+fp-armv8,+fp16fml,+fullfp16,+lse,+neon,+ras,+rcpc,+rdm,+sha2,+sha3,+v8.1a,+v8.2a,+v8.3a,+v8.4a,+v8.5a,+v8a,+zcm,+zcz" }
attributes #3 = { noreturn "frame-pointer"="non-leaf" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+crc,+dotprod,+fp-armv8,+fp16fml,+fullfp16,+lse,+neon,+ras,+rcpc,+rdm,+sha2,+sha3,+v8.1a,+v8.2a,+v8.3a,+v8.4a,+v8.5a,+v8a,+zcm,+zcz" }
attributes #4 = { noreturn }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!2, !3, !4, !5, !6, !7}
!llvm.ident = !{!8}

!0 = distinct !DICompileUnit(language: DW_LANG_C_plus_plus_14, file: !1, producer: "Homebrew clang version 17.0.5", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, splitDebugInlining: false, nameTableKind: Apple, sysroot: "/Library/Developer/CommandLineTools/SDKs/MacOSX14.sdk", sdk: "MacOSX14.sdk")
!1 = !DIFile(filename: "Object_Module.cpp", directory: "/Users/ralf/repos/Scaly/scaly/memory")
!2 = !{i32 7, !"Dwarf Version", i32 4}
!3 = !{i32 2, !"Debug Info Version", i32 3}
!4 = !{i32 1, !"wchar_size", i32 4}
!5 = !{i32 8, !"PIC Level", i32 2}
!6 = !{i32 7, !"uwtable", i32 1}
!7 = !{i32 7, !"frame-pointer", i32 1}
!8 = !{!"Homebrew clang version 17.0.5"}
!9 = distinct !DISubprogram(name: "operator new", linkageName: "_ZN5scaly6memory6ObjectnwEmmPNS0_4PageE", scope: !11, file: !10, line: 4, type: !17, scopeLine: 4, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !0, declaration: !16, retainedNodes: !115)
!10 = !DIFile(filename: "./Object.cpp", directory: "/Users/ralf/repos/Scaly/scaly/memory")
!11 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "Object", scope: !13, file: !12, line: 8, size: 8, flags: DIFlagTypePassByValue, elements: !15, identifier: "_ZTSN5scaly6memory6ObjectE")
!12 = !DIFile(filename: "./Object.h", directory: "/Users/ralf/repos/Scaly/scaly/memory")
!13 = !DINamespace(name: "memory", scope: !14)
!14 = !DINamespace(name: "scaly", scope: null)
!15 = !{!16, !111}
!16 = !DISubprogram(name: "operator new", linkageName: "_ZN5scaly6memory6ObjectnwEmmPNS0_4PageE", scope: !11, file: !12, line: 9, type: !17, scopeLine: 9, flags: DIFlagPrototyped | DIFlagStaticMember, spFlags: 0)
!17 = !DISubroutineType(types: !18)
!18 = !{!19, !20, !20, !25}
!19 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: null, size: 64)
!20 = !DIDerivedType(tag: DW_TAG_typedef, name: "size_t", file: !21, line: 31, baseType: !22)
!21 = !DIFile(filename: "/Library/Developer/CommandLineTools/SDKs/MacOSX14.sdk/usr/include/sys/_types/_size_t.h", directory: "")
!22 = !DIDerivedType(tag: DW_TAG_typedef, name: "__darwin_size_t", file: !23, line: 70, baseType: !24)
!23 = !DIFile(filename: "/Library/Developer/CommandLineTools/SDKs/MacOSX14.sdk/usr/include/arm/_types.h", directory: "")
!24 = !DIBasicType(name: "unsigned long", size: 64, encoding: DW_ATE_unsigned)
!25 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !26, size: 64)
!26 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "Page", scope: !13, file: !27, line: 5, size: 256, flags: DIFlagTypePassByValue | DIFlagNonTrivial, elements: !28, identifier: "_ZTSN5scaly6memory4PageE")
!27 = !DIFile(filename: "./Page.h", directory: "/Users/ralf/repos/Scaly/scaly/memory")
!28 = !{!29, !30, !31, !32, !81, !85, !88, !91, !94, !97, !100, !103, !104, !105, !108}
!29 = !DIDerivedType(tag: DW_TAG_member, name: "next_object", scope: !26, file: !27, line: 6, baseType: !19, size: 64)
!30 = !DIDerivedType(tag: DW_TAG_member, name: "current_page", scope: !26, file: !27, line: 7, baseType: !25, size: 64, offset: 64)
!31 = !DIDerivedType(tag: DW_TAG_member, name: "next_page", scope: !26, file: !27, line: 8, baseType: !25, size: 64, offset: 128)
!32 = !DIDerivedType(tag: DW_TAG_member, name: "exclusive_pages", scope: !26, file: !27, line: 9, baseType: !33, size: 64, offset: 192)
!33 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "PageList", scope: !13, file: !34, line: 5, size: 64, flags: DIFlagTypePassByValue | DIFlagNonTrivial, elements: !35, identifier: "_ZTSN5scaly6memory8PageListE")
!34 = !DIFile(filename: "./PageList.h", directory: "/Users/ralf/repos/Scaly/scaly/memory")
!35 = !{!36, !37, !49, !53, !56, !60, !64, !67}
!36 = !DIDerivedType(tag: DW_TAG_inheritance, scope: !33, baseType: !11, extraData: i32 0)
!37 = !DIDerivedType(tag: DW_TAG_member, name: "head", scope: !33, file: !34, line: 6, baseType: !38, size: 64)
!38 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !39, size: 64)
!39 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "PageNode", scope: !13, file: !40, line: 5, size: 128, flags: DIFlagTypePassByValue | DIFlagNonTrivial, elements: !41, identifier: "_ZTSN5scaly6memory8PageNodeE")
!40 = !DIFile(filename: "./PageNode.h", directory: "/Users/ralf/repos/Scaly/scaly/memory")
!41 = !{!42, !43, !44, !45}
!42 = !DIDerivedType(tag: DW_TAG_inheritance, scope: !39, baseType: !11, extraData: i32 0)
!43 = !DIDerivedType(tag: DW_TAG_member, name: "page", scope: !39, file: !40, line: 6, baseType: !25, size: 64)
!44 = !DIDerivedType(tag: DW_TAG_member, name: "next", scope: !39, file: !40, line: 7, baseType: !38, size: 64, offset: 64)
!45 = !DISubprogram(name: "PageNode", scope: !39, file: !40, line: 8, type: !46, scopeLine: 8, flags: DIFlagPrototyped, spFlags: 0)
!46 = !DISubroutineType(types: !47)
!47 = !{null, !48, !25, !38}
!48 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !39, size: 64, flags: DIFlagArtificial | DIFlagObjectPointer)
!49 = !DISubprogram(name: "PageList", scope: !33, file: !34, line: 7, type: !50, scopeLine: 7, flags: DIFlagPrototyped, spFlags: 0)
!50 = !DISubroutineType(types: !51)
!51 = !{null, !52}
!52 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !33, size: 64, flags: DIFlagArtificial | DIFlagObjectPointer)
!53 = !DISubprogram(name: "add", linkageName: "_ZN5scaly6memory8PageList3addEPNS0_4PageES3_", scope: !33, file: !34, line: 8, type: !54, scopeLine: 8, flags: DIFlagPrototyped, spFlags: 0)
!54 = !DISubroutineType(types: !55)
!55 = !{null, !52, !25, !25}
!56 = !DISubprogram(name: "remove", linkageName: "_ZN5scaly6memory8PageList6removeEPNS0_4PageE", scope: !33, file: !34, line: 9, type: !57, scopeLine: 9, flags: DIFlagPrototyped, spFlags: 0)
!57 = !DISubroutineType(types: !58)
!58 = !{!59, !52, !25}
!59 = !DIBasicType(name: "bool", size: 8, encoding: DW_ATE_boolean)
!60 = !DISubprogram(name: "get_head", linkageName: "_ZN5scaly6memory8PageList8get_headEv", scope: !33, file: !34, line: 10, type: !61, scopeLine: 10, flags: DIFlagPrototyped, spFlags: 0)
!61 = !DISubroutineType(types: !62)
!62 = !{!63, !52}
!63 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !25, size: 64)
!64 = !DISubprogram(name: "count", linkageName: "_ZN5scaly6memory8PageList5countEv", scope: !33, file: !34, line: 11, type: !65, scopeLine: 11, flags: DIFlagPrototyped, spFlags: 0)
!65 = !DISubroutineType(types: !66)
!66 = !{!20, !52}
!67 = !DISubprogram(name: "get_iterator", linkageName: "_ZN5scaly6memory8PageList12get_iteratorEv", scope: !33, file: !34, line: 12, type: !68, scopeLine: 12, flags: DIFlagPrototyped, spFlags: 0)
!68 = !DISubroutineType(types: !69)
!69 = !{!70, !52}
!70 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "PageListIterator", scope: !13, file: !71, line: 5, size: 64, flags: DIFlagTypePassByValue | DIFlagNonTrivial, elements: !72, identifier: "_ZTSN5scaly6memory16PageListIteratorE")
!71 = !DIFile(filename: "./PageListIterator.h", directory: "/Users/ralf/repos/Scaly/scaly/memory")
!72 = !{!73, !74, !78}
!73 = !DIDerivedType(tag: DW_TAG_member, name: "current", scope: !70, file: !71, line: 6, baseType: !38, size: 64)
!74 = !DISubprogram(name: "PageListIterator", scope: !70, file: !71, line: 7, type: !75, scopeLine: 7, flags: DIFlagPrototyped, spFlags: 0)
!75 = !DISubroutineType(types: !76)
!76 = !{null, !77, !38}
!77 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !70, size: 64, flags: DIFlagArtificial | DIFlagObjectPointer)
!78 = !DISubprogram(name: "next", linkageName: "_ZN5scaly6memory16PageListIterator4nextEv", scope: !70, file: !71, line: 8, type: !79, scopeLine: 8, flags: DIFlagPrototyped, spFlags: 0)
!79 = !DISubroutineType(types: !80)
!80 = !{!63, !77}
!81 = !DISubprogram(name: "reset", linkageName: "_ZN5scaly6memory4Page5resetEv", scope: !26, file: !27, line: 11, type: !82, scopeLine: 11, flags: DIFlagPrototyped, spFlags: 0)
!82 = !DISubroutineType(types: !83)
!83 = !{null, !84}
!84 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !26, size: 64, flags: DIFlagArtificial | DIFlagObjectPointer)
!85 = !DISubprogram(name: "is_oversized", linkageName: "_ZN5scaly6memory4Page12is_oversizedEv", scope: !26, file: !27, line: 12, type: !86, scopeLine: 12, flags: DIFlagPrototyped, spFlags: 0)
!86 = !DISubroutineType(types: !87)
!87 = !{!59, !84}
!88 = !DISubprogram(name: "get_capacity", linkageName: "_ZN5scaly6memory4Page12get_capacityEm", scope: !26, file: !27, line: 13, type: !89, scopeLine: 13, flags: DIFlagPrototyped, spFlags: 0)
!89 = !DISubroutineType(types: !90)
!90 = !{!20, !84, !20}
!91 = !DISubprogram(name: "allocate_raw", linkageName: "_ZN5scaly6memory4Page12allocate_rawEmm", scope: !26, file: !27, line: 14, type: !92, scopeLine: 14, flags: DIFlagPrototyped, spFlags: 0)
!92 = !DISubroutineType(types: !93)
!93 = !{!19, !84, !20, !20}
!94 = !DISubprogram(name: "allocate_oversized", linkageName: "_ZN5scaly6memory4Page18allocate_oversizedEm", scope: !26, file: !27, line: 15, type: !95, scopeLine: 15, flags: DIFlagPrototyped, spFlags: 0)
!95 = !DISubroutineType(types: !96)
!96 = !{!19, !84, !20}
!97 = !DISubprogram(name: "allocate_page", linkageName: "_ZN5scaly6memory4Page13allocate_pageEv", scope: !26, file: !27, line: 16, type: !98, scopeLine: 16, flags: DIFlagPrototyped | DIFlagStaticMember, spFlags: 0)
!98 = !DISubroutineType(types: !99)
!99 = !{!25}
!100 = !DISubprogram(name: "allocate_exclusive_page", linkageName: "_ZN5scaly6memory4Page23allocate_exclusive_pageEv", scope: !26, file: !27, line: 17, type: !101, scopeLine: 17, flags: DIFlagPrototyped, spFlags: 0)
!101 = !DISubroutineType(types: !102)
!102 = !{!25, !84}
!103 = !DISubprogram(name: "deallocate_extensions", linkageName: "_ZN5scaly6memory4Page21deallocate_extensionsEv", scope: !26, file: !27, line: 18, type: !82, scopeLine: 18, flags: DIFlagPrototyped, spFlags: 0)
!104 = !DISubprogram(name: "forget", linkageName: "_ZN5scaly6memory4Page6forgetEv", scope: !26, file: !27, line: 19, type: !82, scopeLine: 19, flags: DIFlagPrototyped, spFlags: 0)
!105 = !DISubprogram(name: "get", linkageName: "_ZN5scaly6memory4Page3getEPv", scope: !26, file: !27, line: 20, type: !106, scopeLine: 20, flags: DIFlagPrototyped | DIFlagStaticMember, spFlags: 0)
!106 = !DISubroutineType(types: !107)
!107 = !{!25, !19}
!108 = !DISubprogram(name: "deallocate_exclusive_page", linkageName: "_ZN5scaly6memory4Page25deallocate_exclusive_pageEPS1_", scope: !26, file: !27, line: 21, type: !109, scopeLine: 21, flags: DIFlagPrototyped, spFlags: 0)
!109 = !DISubroutineType(types: !110)
!110 = !{null, !84, !25}
!111 = !DISubprogram(name: "get_page", linkageName: "_ZN5scaly6memory6Object8get_pageEv", scope: !11, file: !12, line: 10, type: !112, scopeLine: 10, flags: DIFlagPrototyped, spFlags: 0)
!112 = !DISubroutineType(types: !113)
!113 = !{!25, !114}
!114 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !11, size: 64, flags: DIFlagArtificial | DIFlagObjectPointer)
!115 = !{}
!116 = !DILocalVariable(name: "size", arg: 1, scope: !9, file: !10, line: 4, type: !20)
!117 = !DILocation(line: 4, column: 35, scope: !9)
!118 = !DILocalVariable(name: "align", arg: 2, scope: !9, file: !10, line: 4, type: !20)
!119 = !DILocation(line: 4, column: 48, scope: !9)
!120 = !DILocalVariable(name: "page", arg: 3, scope: !9, file: !10, line: 4, type: !25)
!121 = !DILocation(line: 4, column: 61, scope: !9)
!122 = !DILocalVariable(name: "object", scope: !9, file: !10, line: 5, type: !19)
!123 = !DILocation(line: 5, column: 11, scope: !9)
!124 = !DILocation(line: 5, column: 20, scope: !9)
!125 = !DILocation(line: 5, column: 39, scope: !9)
!126 = !DILocation(line: 5, column: 45, scope: !9)
!127 = !DILocation(line: 5, column: 26, scope: !9)
!128 = !DILocation(line: 6, column: 10, scope: !129)
!129 = distinct !DILexicalBlock(scope: !9, file: !10, line: 6, column: 9)
!130 = !DILocation(line: 6, column: 9, scope: !9)
!131 = !DILocation(line: 7, column: 9, scope: !129)
!132 = !DILocation(line: 9, column: 12, scope: !9)
!133 = !DILocation(line: 9, column: 5, scope: !9)
!134 = distinct !DISubprogram(name: "get_page", linkageName: "_ZN5scaly6memory6Object8get_pageEv", scope: !11, file: !10, line: 12, type: !112, scopeLine: 12, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !0, declaration: !111, retainedNodes: !115)
!135 = !DILocalVariable(name: "this", arg: 1, scope: !134, type: !136, flags: DIFlagArtificial | DIFlagObjectPointer)
!136 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !11, size: 64)
!137 = !DILocation(line: 0, scope: !134)
!138 = !DILocation(line: 13, column: 12, scope: !134)
!139 = !DILocation(line: 13, column: 5, scope: !134)
