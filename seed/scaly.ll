; ModuleID = 'scaly'
source_filename = "scaly"

%_Z16PageListIterator = type { ptr }
%_Z8PageList = type { ptr }
%_Z4Page = type { ptr, ptr, ptr, %_Z8PageList }
%_Z8PageNode = type { ptr, ptr }
%_Z14BlockWatermark = type { ptr, ptr, ptr, ptr }
%_Z17StackBucketHeader = type { ptr, ptr }
%_Z16HeapBucketHeader = type { ptr, ptr, i64 }
%_Z10TraceEntry = type { ptr, i64, i64 }
%_Z6VectorIiE = type { i64, ptr }
%_Z14VectorIteratorIiE = type { ptr, i64 }
%_Z5Slice = type { ptr, i64 }
%_Z5ArrayIiE = type { i64, ptr }
%_Z13ArrayIteratorIiE = type { ptr, i64 }
%_Z4ListIiE = type { ptr }
%_Z4NodeIiE = type { i64, ptr }
%_Z12ListIteratorIiE = type { ptr }
%_Z14VectorIteratorI1TE = type { ptr, i64 }
%_Z13ArrayIteratorI1TE = type { ptr, i64 }
%_Z12ListIteratorI1TE = type { ptr }
%_Z6String = type { ptr }
%_Z13StringBuilder = type { %_Z5ArrayIcE }
%_Z5ArrayIcE = type { i64, ptr }
%_Z6VectorIcE = type { i64, ptr }
%_Z6VectorI6StringE = type { i64, ptr }
%_Z14VectorIteratorI6StringE = type { ptr, i64 }
%_Z5ArrayI6StringE = type { i64, ptr }
%_Z4ListI6StringE = type { ptr }
%_Z4NodeI6StringE = type { %_Z6String, ptr }
%_Z12ListIteratorI6StringE = type { ptr }
%_Z13ArrayIteratorI6StringE = type { ptr, i64 }
%_Z11BuilderListI6StringE = type { ptr }
%_Z19BuilderListIteratorI6StringE = type { ptr }
%_Z6VectorI11BuilderListI4SlotI6StringEEE = type { i64, ptr }
%_Z11BuilderListI4SlotI6StringEE = type { ptr }
%_Z11BuilderListI11BuilderListI4SlotI6StringEEE = type { ptr }
%_Z4NodeI11BuilderListI4SlotI6StringEEE = type { %_Z11BuilderListI4SlotI6StringEE, ptr }
%_Z19BuilderListIteratorI11BuilderListI4SlotI6StringEEE = type { ptr }
%_Z14VectorIteratorI11BuilderListI4SlotI6StringEEE = type { ptr, i64 }
%_Z5ArrayI11BuilderListI4SlotI6StringEEE = type { i64, ptr }
%_Z13ArrayIteratorI11BuilderListI4SlotI6StringEEE = type { ptr, i64 }
%_Z4ListI11BuilderListI4SlotI6StringEEE = type { ptr }
%_Z12ListIteratorI11BuilderListI4SlotI6StringEEE = type { ptr }
%_Z19BuilderListIteratorI4SlotI6StringEE = type { ptr }
%_Z4NodeI4SlotI6StringEE = type { %_Z4SlotI6StringE, ptr }
%_Z4SlotI6StringE = type { %_Z6String, i64 }
%_Z14HashSetBuilderI6StringE = type { i64, ptr }
%_Z6VectorI6VectorI6StringEE = type { i64, ptr }
%_Z14VectorIteratorI6VectorI6StringEE = type { ptr, i64 }
%_Z5ArrayI6VectorI6StringEE = type { i64, ptr }
%_Z13ArrayIteratorI6VectorI6StringEE = type { ptr, i64 }
%_Z4ListI6VectorI6StringEE = type { ptr }
%_Z4NodeI6VectorI6StringEE = type { %_Z6VectorI6StringE, ptr }
%_Z12ListIteratorI6VectorI6StringEE = type { ptr }
%_Z7HashSetI6StringE = type { ptr }
%_Z12ListIteratorI4SlotI6StringEE = type { ptr }
%_Z19BuilderListIteratorI1TE = type { ptr }
%_Z19BuilderListIteratorI11BuilderListI4SlotI1TEEE = type { ptr }
%_Z14VectorIteratorI11BuilderListI4SlotI1TEEE = type { ptr, i64 }
%_Z13ArrayIteratorI11BuilderListI4SlotI1TEEE = type { ptr, i64 }
%_Z12ListIteratorI11BuilderListI4SlotI1TEEE = type { ptr }
%_Z19BuilderListIteratorI4SlotI1TEE = type { ptr }
%_Z6VectorI12KeyValuePairI6StringiEE = type { i64, ptr }
%_Z12KeyValuePairI6StringiE = type { %_Z6String, i64 }
%_Z14VectorIteratorI12KeyValuePairI6StringiEE = type { ptr, i64 }
%_Z5ArrayI12KeyValuePairI6StringiEE = type { i64, ptr }
%_Z4ListI12KeyValuePairI6StringiEE = type { ptr }
%_Z4NodeI12KeyValuePairI6StringiEE = type { %_Z12KeyValuePairI6StringiE, ptr }
%_Z12ListIteratorI12KeyValuePairI6StringiEE = type { ptr }
%_Z13ArrayIteratorI12KeyValuePairI6StringiEE = type { ptr, i64 }
%_Z14VectorIteratorI12KeyValuePairI1K1VEE = type { ptr, i64 }
%_Z13ArrayIteratorI12KeyValuePairI1K1VEE = type { ptr, i64 }
%_Z12ListIteratorI12KeyValuePairI1K1VEE = type { ptr }
%_Z13SliceIteratorI1TE = type { %_Z5Slice, i64 }
%_Z13SliceIterator = type { %_Z5Slice, i64 }
%_Z4NodeI1TE = type { ptr, ptr }
%_Z4ListI1TE = type { ptr }
%_Z6VectorI1TE = type { i64, ptr }
%_Z6Vector = type { i64, ptr }
%_Z5ArrayI1TE = type { i64, ptr }
%_Z5Array = type { i64, ptr }
%_Z11BuilderListI1TE = type { ptr }
%_Z11BuilderList = type { ptr }
%_Z4SlotI1TE = type { ptr, i64 }
%_Z6VectorI11BuilderListI4SlotI1TEEE = type { i64, ptr }
%_Z14HashSetBuilderI1TE = type { i64, ptr }
%_Z11BuilderListI4SlotI1TEE = type { ptr }
%_Z14VectorIteratorI6VectorI1TEE = type { ptr, i64 }
%_Z13ArrayIteratorI6VectorI1TEE = type { ptr, i64 }
%_Z12ListIteratorI6VectorI1TEE = type { ptr }
%_Z19BuilderListIteratorI4SlotI12KeyValuePairI1K1VEEE = type { ptr }
%_Z14VectorIteratorI11BuilderListI4SlotI12KeyValuePairI1K1VEEEE = type { ptr, i64 }
%_Z13ArrayIteratorI11BuilderListI4SlotI12KeyValuePairI1K1VEEEE = type { ptr, i64 }
%_Z12ListIteratorI11BuilderListI4SlotI12KeyValuePairI1K1VEEEE = type { ptr }
%_Z4SlotI12KeyValuePairI1K1VEE = type { %_Z12KeyValuePairI1K1VE, i64 }
%_Z12KeyValuePairI1K1VE = type { ptr, ptr }
%_Z6VectorI11BuilderListI4SlotI12KeyValuePairI1K1VEEEE = type { i64, ptr }
%_Z14HashMapBuilder = type { i64, ptr }
%_Z11BuilderListI4SlotI12KeyValuePairI1K1VEEE = type { ptr }
%_Z14VectorIteratorI6VectorI12KeyValuePairI1K1VEEE = type { ptr, i64 }
%_Z13ArrayIteratorI6VectorI12KeyValuePairI1K1VEEE = type { ptr, i64 }
%_Z12ListIteratorI6VectorI12KeyValuePairI1K1VEEE = type { ptr }
%_Z15HashMapIteratorI1K1VE = type { %_Z14VectorIteratorI6VectorI12KeyValuePairI1K1VEEE, %_Z14VectorIteratorI12KeyValuePairI1K1VEE }
%_Z14VectorIteratorIcE = type { ptr, i64 }
%_Z13ArrayIteratorIcE = type { ptr, i64 }
%_Z4ListIcE = type { ptr }
%_Z4NodeIcE = type { i8, ptr }
%_Z12ListIteratorIcE = type { ptr }
%_Z14StringIterator = type { ptr, ptr }
%_Z6Result = type { i8, [8 x i8] }
%_Z6Option = type { i8, [8 x i8] }

@"9PAGE_SIZE" = constant i64 4096
@"12BUCKET_PAGES" = constant i64 64
@"11BUCKET_SIZE" = constant i64 262144
@"11BUCKET_MASK" = constant i64 262143
@"15BITMAP_ALL_FREE" = constant i64 9223372036854775807
@stack_head = global ptr null
@stack_top = global ptr null
@heap_head = global ptr null
@trace_enabled = global i64 0
@trace_atexit_registered = global i64 0
@trace_count = global i64 0
@trace_entries = global ptr null
@"9TRACE_MAX" = constant i64 8192
@fmt_scratch = global ptr null
@"11PACKED_SIZE" = constant i64 9
@"9STDOUT_FD" = constant i64 1
@"8SEEK_SET" = constant i64 0
@"8SEEK_CUR" = constant i64 1
@"8SEEK_END" = constant i64 2
@"4F_OK" = constant i64 0
@"4F_OK.1" = constant i64 0
@.sconst = private constant [4 x i8] c"\02\\n\00"
@.str = private unnamed_addr constant [53 x i8] c"scaly_release_root_page: LIFO violation \E2\80\94 release=\00", align 1
@.str.2 = private unnamed_addr constant [6 x i8] c" top=\00", align 1
@.str.3 = private unnamed_addr constant [30 x i8] c"scaly_trace_root: UNBALANCED \00", align 1
@.str.4 = private unnamed_addr constant [7 x i8] c" push=\00", align 1
@.str.5 = private unnamed_addr constant [6 x i8] c" pop=\00", align 1
@.str.6 = private unnamed_addr constant [7 x i8] c" leak=\00", align 1
@.str.7 = private unnamed_addr constant [50 x i8] c"scaly_trace_root: all root-page push/pop balanced\00", align 1
@.str.8 = private unnamed_addr constant [17 x i8] c"SCALY_TRACE_ROOT\00", align 1
@.str.9 = private unnamed_addr constant [32 x i8] c"Vector.put: index out of bounds\00", align 1
@.str.10 = private unnamed_addr constant [31 x i8] c"Array.put: index out of bounds\00", align 1
@.str.11 = private unnamed_addr constant [32 x i8] c"Vector.put: index out of bounds\00", align 1
@.str.12 = private unnamed_addr constant [31 x i8] c"Array.put: index out of bounds\00", align 1
@.str.13 = private unnamed_addr constant [32 x i8] c"Vector.put: index out of bounds\00", align 1
@.str.14 = private unnamed_addr constant [31 x i8] c"Array.put: index out of bounds\00", align 1
@.str.15 = private unnamed_addr constant [32 x i8] c"Vector.put: index out of bounds\00", align 1
@.str.16 = private unnamed_addr constant [31 x i8] c"Array.put: index out of bounds\00", align 1
@.str.17 = private unnamed_addr constant [32 x i8] c"Vector.put: index out of bounds\00", align 1
@.str.18 = private unnamed_addr constant [31 x i8] c"Array.put: index out of bounds\00", align 1
@.str.19 = private unnamed_addr constant [32 x i8] c"Vector.put: index out of bounds\00", align 1
@.str.20 = private unnamed_addr constant [31 x i8] c"Array.put: index out of bounds\00", align 1
@.str.21 = private unnamed_addr constant [3 x i8] c"rb\00", align 1
@.str.22 = private unnamed_addr constant [3 x i8] c"wb\00", align 1
@.str.23 = private unnamed_addr constant [2 x i8] c".\00", align 1

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

define linkonce_odr ptr @_ZN4Page13allocate_pageEv() {
entry:
  %call = call ptr @_Z16scaly_alloc_pagev()
  ret ptr %call
}

define linkonce_odr void @_ZN4Page21deallocate_extensionsEv(ptr %0) {
entry:
  %iter.alloca11 = alloca %_Z16PageListIterator, align 8
  %coll.tmp10 = alloca %_Z8PageList, align 8
  %page = alloca ptr, align 8
  %local_page = call ptr @_Z21scaly_alloc_root_pagev()
  %iter.alloca = alloca %_Z16PageListIterator, align 8
  %coll.tmp = alloca %_Z8PageList, align 8
  %load.struct = load %_Z4Page, ptr %0, align 8
  %next_object = extractvalue %_Z4Page %load.struct, 0
  %eq = icmp eq ptr %next_object, null
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  call void @_Z23scaly_release_root_pageP4Page(ptr %local_page)
  ret void

if.end:                                           ; preds = %entry
  %load.struct1 = load %_Z4Page, ptr %0, align 8
  %exclusive_pages = extractvalue %_Z4Page %load.struct1, 3
  store %_Z8PageList %exclusive_pages, ptr %coll.tmp, align 8
  call void @_ZN8PageList12get_iteratorEPN4scaly6memory4PageE(ptr %iter.alloca, ptr %local_page, ptr %coll.tmp)
  br label %for.cond

for.cond:                                         ; preds = %for.body, %if.end
  %next = call ptr @_ZN16PageListIterator4nextEv(ptr %iter.alloca)
  %is.done = icmp eq ptr %next, null
  br i1 %is.done, label %for.exit, label %for.body

for.body:                                         ; preds = %for.cond
  call void @_ZN4Page21deallocate_extensionsEv(ptr %next)
  call void @_Z18scaly_release_pageP4Page(ptr %next)
  br label %for.cond

for.exit:                                         ; preds = %for.cond
  %load.struct2 = load %_Z4Page, ptr %0, align 8
  %next_page = extractvalue %_Z4Page %load.struct2, 2
  store ptr %next_page, ptr %page, align 1
  br label %while.cond

while.cond:                                       ; preds = %for.exit14, %for.exit
  %page3 = load ptr, ptr %page, align 8
  %ne = icmp ne ptr %page3, null
  br i1 %ne, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %page4 = load ptr, ptr %page, align 8
  %load.struct5 = load %_Z4Page, ptr %page4, align 8
  %next_page6 = extractvalue %_Z4Page %load.struct5, 2
  %page7 = load ptr, ptr %page, align 8
  %load.struct8 = load %_Z4Page, ptr %page7, align 8
  %exclusive_pages9 = extractvalue %_Z4Page %load.struct8, 3
  store %_Z8PageList %exclusive_pages9, ptr %coll.tmp10, align 8
  call void @_ZN8PageList12get_iteratorEPN4scaly6memory4PageE(ptr %iter.alloca11, ptr %local_page, ptr %coll.tmp10)
  br label %for.cond12

while.exit:                                       ; preds = %while.cond
  call void @_Z28scaly_release_root_page_fullP4Page(ptr %local_page)
  ret void

for.cond12:                                       ; preds = %for.body13, %while.body
  %next15 = call ptr @_ZN16PageListIterator4nextEv(ptr %iter.alloca11)
  %is.done16 = icmp eq ptr %next15, null
  br i1 %is.done16, label %for.exit14, label %for.body13

for.body13:                                       ; preds = %for.cond12
  call void @_ZN4Page21deallocate_extensionsEv(ptr %next15)
  call void @_Z18scaly_release_pageP4Page(ptr %next15)
  br label %for.cond12

for.exit14:                                       ; preds = %for.cond12
  %page17 = load ptr, ptr %page, align 8
  call void @_Z18scaly_release_pageP4Page(ptr %page17)
  store ptr %next_page6, ptr %page, align 1
  br label %while.cond
}

define linkonce_odr ptr @_ZN4Page8allocateEmm(ptr %0, i64 %1, i64 %2) {
entry:
  %page = alloca ptr, align 8
  %load.struct = load %_Z4Page, ptr %0, align 8
  %next_object = extractvalue %_Z4Page %load.struct, 0
  %as.ptrtoint = ptrtoint ptr %next_object to i64
  %add = add i64 %as.ptrtoint, %2
  %sub = sub i64 %add, 1
  %sub1 = sub i64 %2, 1
  %bitnot = xor i64 %sub1, -1
  %and = and i64 %sub, %bitnot
  %as.ptrtoint2 = ptrtoint ptr %0 to i64
  %add3 = add i64 %as.ptrtoint2, 4096
  %sub4 = sub i64 %add3, %and
  %lt = icmp ult i64 %sub4, %1
  br i1 %lt, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %add5 = add i64 %1, ptrtoint (ptr getelementptr (%_Z4Page, ptr null, i32 1) to i64)
  %gt = icmp ugt i64 %add5, 4096
  br i1 %gt, label %if.then6, label %if.end7

if.end:                                           ; preds = %entry
  %add25 = add i64 %and, %1
  %as.inttoptr = inttoptr i64 %add25 to ptr
  %next_object26 = getelementptr inbounds %_Z4Page, ptr %0, i32 0, i32 0
  store ptr %as.inttoptr, ptr %next_object26, align 8
  %as.inttoptr27 = inttoptr i64 %and to ptr
  ret ptr %as.inttoptr27

if.then6:                                         ; preds = %if.then
  %call = call ptr @_ZN4Page18allocate_oversizedEm(ptr %0, i64 %add5)
  ret ptr %call

if.end7:                                          ; preds = %if.then
  %load.struct8 = load %_Z4Page, ptr %0, align 8
  %current_page = extractvalue %_Z4Page %load.struct8, 1
  %ne = icmp ne ptr %current_page, null
  br i1 %ne, label %if.then9, label %if.end10

if.then9:                                         ; preds = %if.end7
  %field.inplace = getelementptr inbounds %_Z4Page, ptr %0, i32 0, i32 1
  %deref.recv = load ptr, ptr %field.inplace, align 8
  %call11 = call ptr @_ZN4Page8allocateEmm(ptr %deref.recv, i64 %1, i64 %2)
  %call12 = call ptr @_ZN4Page3getEPv(ptr %call11)
  %load.struct13 = load %_Z4Page, ptr %0, align 8
  %current_page14 = extractvalue %_Z4Page %load.struct13, 1
  %ne15 = icmp ne ptr %call12, %current_page14
  br i1 %ne15, label %if.then16, label %if.end17

if.end10:                                         ; preds = %if.end7
  %call19 = call ptr @_ZN4Page13allocate_pageEv()
  store ptr %call19, ptr %page, align 1
  %page20 = load ptr, ptr %page, align 8
  %current_page21 = getelementptr inbounds %_Z4Page, ptr %0, i32 0, i32 1
  store ptr %page20, ptr %current_page21, align 8
  %page22 = load ptr, ptr %page, align 8
  %next_page = getelementptr inbounds %_Z4Page, ptr %0, i32 0, i32 2
  store ptr %page22, ptr %next_page, align 8
  %page23 = load ptr, ptr %page, align 8
  %call24 = call ptr @_ZN4Page8allocateEmm(ptr %page23, i64 %1, i64 %2)
  ret ptr %call24

if.then16:                                        ; preds = %if.then9
  %current_page18 = getelementptr inbounds %_Z4Page, ptr %0, i32 0, i32 1
  store ptr %call12, ptr %current_page18, align 8
  br label %if.end17

if.end17:                                         ; preds = %if.then16, %if.then9
  ret ptr %call11
}

define linkonce_odr ptr @_ZN4Page3getEPv(ptr %0) {
entry:
  %as.ptrtoint = ptrtoint ptr %0 to i64
  %and = and i64 %as.ptrtoint, -4096
  %as.inttoptr = inttoptr i64 %and to ptr
  ret ptr %as.inttoptr
}

declare ptr @aligned_alloc(i64, i64)

declare void @free(ptr)

declare void @exit(i64)

declare void @abort()

declare i64 @strlen(ptr)

declare i64 @write(i64, ptr, i64)

declare i64 @read(i64, ptr, i64)

declare ptr @fopen(ptr, ptr)

declare i64 @fclose(ptr)

declare i64 @fread(ptr, i64, i64, ptr)

declare i64 @fwrite(ptr, i64, i64, ptr)

declare i64 @fseek(ptr, i64, i64)

declare i64 @ftell(ptr)

declare void @rewind(ptr)

declare ptr @opendir(ptr)

declare i64 @closedir(ptr)

declare i64 @access(ptr, i64)

declare i64 @unlink(ptr)

declare i64 @rmdir(ptr)

declare i64 @mkdir(ptr, i64)

declare i64 @system(ptr)

declare ptr @dirname(ptr)

declare ptr @basename(ptr)

declare i64 @strcmp(ptr, ptr)

declare i32 @memcmp(ptr, ptr, i64)

define linkonce_odr void @_ZN8PageList3addEP4Page(ptr %0, ptr %1) {
entry:
  %call = call ptr @_ZN4Page3getEPv(ptr %0)
  %load.struct = load %_Z8PageList, ptr %0, align 8
  %head = extractvalue %_Z8PageList %load.struct, 0
  %tuple.region = call ptr @_ZN4Page8allocateEmm(ptr %call, i64 ptrtoint (ptr getelementptr (%_Z8PageNode, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr ({ i1, %_Z8PageNode }, ptr null, i64 0, i32 1) to i64))
  %tuple.field = getelementptr inbounds %_Z8PageNode, ptr %tuple.region, i32 0, i32 0
  store ptr %1, ptr %tuple.field, align 1
  %tuple.field1 = getelementptr inbounds %_Z8PageNode, ptr %tuple.region, i32 0, i32 1
  store ptr %head, ptr %tuple.field1, align 1
  %head2 = getelementptr inbounds %_Z8PageList, ptr %0, i32 0, i32 0
  store ptr %tuple.region, ptr %head2, align 8
  ret void
}

define linkonce_odr i1 @_ZN8PageList6removeEP4Page(ptr %0, ptr %1) {
entry:
  %load.struct = load %_Z8PageList, ptr %0, align 8
  %head = extractvalue %_Z8PageList %load.struct, 0
  %node = alloca ptr, align 8
  store ptr %head, ptr %node, align 1
  %previous_node = alloca ptr, align 8
  store ptr null, ptr %previous_node, align 1
  br label %while.cond

while.cond:                                       ; preds = %if.end, %entry
  %node1 = load ptr, ptr %node, align 8
  %ne = icmp ne ptr %node1, null
  br i1 %ne, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %node2 = load ptr, ptr %node, align 8
  %load.struct3 = load %_Z8PageNode, ptr %node2, align 8
  %page = extractvalue %_Z8PageNode %load.struct3, 0
  %eq = icmp eq ptr %page, %1
  br i1 %eq, label %if.then, label %if.end

while.exit:                                       ; preds = %while.cond
  ret i1 false

if.then:                                          ; preds = %while.body
  %previous_node4 = load ptr, ptr %previous_node, align 8
  %ne5 = icmp ne ptr %previous_node4, null
  br i1 %ne5, label %if.then6, label %if.end7

if.end:                                           ; preds = %while.body
  %node21 = load ptr, ptr %node, align 8
  store ptr %node21, ptr %previous_node, align 1
  %node22 = load ptr, ptr %node, align 8
  %load.struct23 = load %_Z8PageNode, ptr %node22, align 8
  %next24 = extractvalue %_Z8PageNode %load.struct23, 1
  store ptr %next24, ptr %node, align 1
  br label %while.cond

if.then6:                                         ; preds = %if.then
  %node8 = load ptr, ptr %node, align 8
  %load.struct9 = load %_Z8PageNode, ptr %node8, align 8
  %next = extractvalue %_Z8PageNode %load.struct9, 1
  %ptr.load = load ptr, ptr %previous_node, align 8
  %next10 = getelementptr inbounds %_Z8PageNode, ptr %ptr.load, i32 0, i32 1
  store ptr %next, ptr %next10, align 8
  br label %if.end7

if.end7:                                          ; preds = %if.then6, %if.then
  %node11 = load ptr, ptr %node, align 8
  %load.struct12 = load %_Z8PageList, ptr %0, align 8
  %head13 = extractvalue %_Z8PageList %load.struct12, 0
  %eq14 = icmp eq ptr %node11, %head13
  br i1 %eq14, label %if.then15, label %if.end16

if.then15:                                        ; preds = %if.end7
  %node17 = load ptr, ptr %node, align 8
  %load.struct18 = load %_Z8PageNode, ptr %node17, align 8
  %next19 = extractvalue %_Z8PageNode %load.struct18, 1
  %head20 = getelementptr inbounds %_Z8PageList, ptr %0, i32 0, i32 0
  store ptr %next19, ptr %head20, align 8
  br label %if.end16

if.end16:                                         ; preds = %if.then15, %if.end7
  ret i1 true
}

define linkonce_odr void @_ZN8PageList12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z16PageListIterator) %0, ptr %1, ptr %2) {
entry:
  %load.struct = load %_Z8PageList, ptr %2, align 8
  %head = extractvalue %_Z8PageList %load.struct, 0
  %tuple = alloca %_Z16PageListIterator, align 8
  %tuple.field = getelementptr inbounds %_Z16PageListIterator, ptr %tuple, i32 0, i32 0
  store ptr %head, ptr %tuple.field, align 1
  %tuple.val = load %_Z16PageListIterator, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z16PageListIterator, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN8PageListC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr ptr @_ZN16PageListIterator4nextEv(ptr %0) {
entry:
  %load.struct = load %_Z16PageListIterator, ptr %0, align 8
  %current = extractvalue %_Z16PageListIterator %load.struct, 0
  %ne = icmp ne ptr %current, null
  br i1 %ne, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %load.struct1 = load %_Z16PageListIterator, ptr %0, align 8
  %current2 = extractvalue %_Z16PageListIterator %load.struct1, 0
  %load.struct3 = load %_Z16PageListIterator, ptr %0, align 8
  %current4 = extractvalue %_Z16PageListIterator %load.struct3, 0
  %deref = load %_Z8PageNode, ptr %current4, align 8
  %next = extractvalue %_Z8PageNode %deref, 1
  %current5 = getelementptr inbounds %_Z16PageListIterator, ptr %0, i32 0, i32 0
  store ptr %next, ptr %current5, align 8
  %load.struct6 = load %_Z8PageNode, ptr %current2, align 8
  %page = extractvalue %_Z8PageNode %load.struct6, 0
  ret ptr %page

if.else:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; No predecessors!
  ret ptr null
}

define linkonce_odr void @_ZN16PageListIteratorC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr void @_ZN14BlockWatermarkC1EP4PagePvP4PageP8PageNode(ptr %0, ptr %1, ptr %2, ptr %3, ptr %4) {
entry:
  %page = getelementptr inbounds %_Z14BlockWatermark, ptr %0, i32 0, i32 0
  store ptr %1, ptr %page, align 8
  %next_object = getelementptr inbounds %_Z14BlockWatermark, ptr %0, i32 0, i32 1
  store ptr %2, ptr %next_object, align 8
  %extension_tail = getelementptr inbounds %_Z14BlockWatermark, ptr %0, i32 0, i32 2
  store ptr %3, ptr %extension_tail, align 8
  %exclusive_head = getelementptr inbounds %_Z14BlockWatermark, ptr %0, i32 0, i32 3
  store ptr %4, ptr %exclusive_head, align 8
  ret void
}

define linkonce_odr ptr @_ZN4Page18allocate_root_pageEv() {
entry:
  %call = call ptr @_Z21scaly_alloc_root_pagev()
  ret ptr %call
}

define linkonce_odr void @_ZN4Page5resetEv(ptr %0) {
entry:
  %current_page = getelementptr inbounds %_Z4Page, ptr %0, i32 0, i32 1
  store ptr null, ptr %current_page, align 8
  %next_page = getelementptr inbounds %_Z4Page, ptr %0, i32 0, i32 2
  store ptr null, ptr %next_page, align 8
  %ptr.add = getelementptr inbounds %_Z4Page, ptr %0, i64 1
  %next_object = getelementptr inbounds %_Z4Page, ptr %0, i32 0, i32 0
  store ptr %ptr.add, ptr %next_object, align 8
  %struct.init = alloca %_Z8PageList, align 8
  %tuple.field = getelementptr inbounds %_Z8PageList, ptr %struct.init, i32 0, i32 0
  store ptr null, ptr %tuple.field, align 8
  %parent.page = call ptr @_ZN4Page3getEPv(ptr %0)
  %ctor.heap = call ptr @_ZN4Page8allocateEmm(ptr %parent.page, i64 ptrtoint (ptr getelementptr (%_Z8PageList, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr ({ i1, %_Z8PageList }, ptr null, i64 0, i32 1) to i64))
  %1 = call ptr @memcpy(ptr %ctor.heap, ptr %struct.init, i64 ptrtoint (ptr getelementptr (%_Z8PageList, ptr null, i32 1) to i64))
  %exclusive_pages = getelementptr inbounds %_Z4Page, ptr %0, i32 0, i32 3
  %field.load = load %_Z8PageList, ptr %ctor.heap, align 8
  store %_Z8PageList %field.load, ptr %exclusive_pages, align 8
  ret void
}

define linkonce_odr ptr @_ZN4Page18allocate_oversizedEm(ptr %0, i64 %1) {
entry:
  %page = alloca ptr, align 8
  %add = add i64 %1, 4096
  %sub = sub i64 %add, 1
  %and = and i64 %sub, -4096
  %call = call ptr @aligned_alloc(i64 4096, i64 %and)
  %eq = icmp eq ptr %call, null
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  call void @exit(i64 1)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  store ptr %call, ptr %page, align 1
  %ptr.load = load ptr, ptr %page, align 8
  %next_object = getelementptr inbounds %_Z4Page, ptr %ptr.load, i32 0, i32 0
  store ptr null, ptr %next_object, align 8
  %field.inplace = getelementptr inbounds %_Z4Page, ptr %0, i32 0, i32 3
  %page1 = load ptr, ptr %page, align 8
  call void @_ZN8PageList3addEP4Page(ptr %field.inplace, ptr %page1)
  %page2 = load ptr, ptr %page, align 8
  %ptr.add = getelementptr inbounds %_Z4Page, ptr %page2, i64 1
  ret ptr %ptr.add
}

define linkonce_odr i64 @_ZN4Page12get_capacityEm(ptr %0, i64 %1) {
entry:
  %load.struct = load %_Z4Page, ptr %0, align 8
  %next_object = extractvalue %_Z4Page %load.struct, 0
  %as.ptrtoint = ptrtoint ptr %next_object to i64
  %location = alloca i64, align 8
  store i64 %as.ptrtoint, ptr %location, align 1
  %location1 = load i64, ptr %location, align 4
  %add = add i64 %location1, %1
  %sub = sub i64 %add, 1
  %sub2 = sub i64 %1, 1
  %bitnot = xor i64 %sub2, -1
  %and = and i64 %sub, %bitnot
  %as.ptrtoint3 = ptrtoint ptr %0 to i64
  %add4 = add i64 %as.ptrtoint3, 4096
  %location_after_page = alloca i64, align 8
  store i64 %add4, ptr %location_after_page, align 1
  %location_after_page5 = load i64, ptr %location_after_page, align 4
  %sub6 = sub i64 %location_after_page5, %and
  %capacity = alloca i64, align 8
  store i64 %sub6, ptr %capacity, align 1
  %capacity7 = load i64, ptr %capacity, align 4
  ret i64 %capacity7
}

define linkonce_odr ptr @_ZN4Page23allocate_exclusive_pageEv(ptr %0) {
entry:
  %call = call ptr @_ZN4Page13allocate_pageEv()
  %field.inplace = getelementptr inbounds %_Z4Page, ptr %0, i32 0, i32 3
  call void @_ZN8PageList3addEP4Page(ptr %field.inplace, ptr %call)
  ret ptr %call
}

define linkonce_odr void @_ZN4Page25deallocate_exclusive_pageEP4Page(ptr %0, ptr %1) {
entry:
  call void @_ZN4Page21deallocate_extensionsEv(ptr %1)
  call void @_Z18scaly_release_pageP4Page(ptr %1)
  %field.inplace = getelementptr inbounds %_Z4Page, ptr %0, i32 0, i32 3
  %call = call i1 @_ZN8PageList6removeEP4Page(ptr %field.inplace, ptr %1)
  %eq = icmp eq i1 %call, false
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  call void @exit(i64 2)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  ret void
}

define linkonce_odr void @_ZN4Page14save_watermarkEPN4scaly6memory4PageE(ptr noalias sret(%_Z14BlockWatermark) %0, ptr %1, ptr %2) {
entry:
  %struct.init7 = alloca %_Z14BlockWatermark, align 8
  %struct.init = alloca %_Z14BlockWatermark, align 8
  %load.struct = load %_Z4Page, ptr %2, align 8
  %current_page = extractvalue %_Z4Page %load.struct, 1
  %ne = icmp ne ptr %current_page, null
  br i1 %ne, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %load.struct1 = load %_Z4Page, ptr %2, align 8
  %current_page2 = extractvalue %_Z4Page %load.struct1, 1
  %load.struct3 = load %_Z4Page, ptr %2, align 8
  %current_page4 = extractvalue %_Z4Page %load.struct3, 1
  %deref = load %_Z4Page, ptr %current_page4, align 8
  %next_object = extractvalue %_Z4Page %deref, 0
  %load.struct5 = load %_Z4Page, ptr %2, align 8
  %next_page = extractvalue %_Z4Page %load.struct5, 2
  %load.struct6 = load %_Z4Page, ptr %2, align 8
  %exclusive_pages = extractvalue %_Z4Page %load.struct6, 3
  %head = extractvalue %_Z8PageList %exclusive_pages, 0
  call void @_ZN14BlockWatermarkC1EP4PagePvP4PageP8PageNode(ptr %struct.init, ptr %current_page2, ptr %next_object, ptr %next_page, ptr %head)
  %sret.body = load %_Z14BlockWatermark, ptr %struct.init, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %struct.init, i64 ptrtoint (ptr getelementptr (%_Z14BlockWatermark, ptr null, i32 1) to i64), i1 false)
  ret void

if.end:                                           ; preds = %entry
  %field.inplace = getelementptr inbounds %_Z4Page, ptr %2, i32 0, i32 0
  %deref.recv = load ptr, ptr %field.inplace, align 8
  %call = call ptr @_ZN4Page3getEPv(ptr %deref.recv)
  %load.struct8 = load %_Z4Page, ptr %2, align 8
  %next_object9 = extractvalue %_Z4Page %load.struct8, 0
  %load.struct10 = load %_Z4Page, ptr %2, align 8
  %next_page11 = extractvalue %_Z4Page %load.struct10, 2
  %load.struct12 = load %_Z4Page, ptr %2, align 8
  %exclusive_pages13 = extractvalue %_Z4Page %load.struct12, 3
  %head14 = extractvalue %_Z8PageList %exclusive_pages13, 0
  call void @_ZN14BlockWatermarkC1EP4PagePvP4PageP8PageNode(ptr %struct.init7, ptr %call, ptr %next_object9, ptr %next_page11, ptr %head14)
  %sret.body15 = load %_Z14BlockWatermark, ptr %struct.init7, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %struct.init7, i64 ptrtoint (ptr getelementptr (%_Z14BlockWatermark, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN4Page17restore_watermarkE14BlockWatermark(ptr %0, ptr %1) {
entry:
  br label %while.cond

while.cond:                                       ; preds = %while.body, %entry
  %load.struct = load %_Z4Page, ptr %0, align 8
  %exclusive_pages = extractvalue %_Z4Page %load.struct, 3
  %head = extractvalue %_Z8PageList %exclusive_pages, 0
  %load.struct1 = load %_Z14BlockWatermark, ptr %1, align 8
  %exclusive_head = extractvalue %_Z14BlockWatermark %load.struct1, 3
  %ne = icmp ne ptr %head, %exclusive_head
  br i1 %ne, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %load.struct2 = load %_Z4Page, ptr %0, align 8
  %exclusive_pages3 = extractvalue %_Z4Page %load.struct2, 3
  %head4 = extractvalue %_Z8PageList %exclusive_pages3, 0
  %load.struct5 = load %_Z8PageNode, ptr %head4, align 8
  %page = extractvalue %_Z8PageNode %load.struct5, 0
  call void @_ZN4Page21deallocate_extensionsEv(ptr %page)
  call void @_Z18scaly_release_pageP4Page(ptr %page)
  %load.struct6 = load %_Z8PageNode, ptr %head4, align 8
  %next = extractvalue %_Z8PageNode %load.struct6, 1
  %exclusive_pages7 = getelementptr inbounds %_Z4Page, ptr %0, i32 0, i32 3
  %head8 = getelementptr inbounds %_Z8PageList, ptr %exclusive_pages7, i32 0, i32 0
  store ptr %next, ptr %head8, align 8
  br label %while.cond

while.exit:                                       ; preds = %while.cond
  %field.inplace = getelementptr inbounds %_Z14BlockWatermark, ptr %1, i32 0, i32 2
  %deref.recv = load ptr, ptr %field.inplace, align 8
  call void @_ZN4Page27deallocate_extensions_afterEP4Page(ptr %0, ptr %deref.recv)
  %load.struct9 = load %_Z14BlockWatermark, ptr %1, align 8
  %page10 = extractvalue %_Z14BlockWatermark %load.struct9, 0
  %current_page = getelementptr inbounds %_Z4Page, ptr %0, i32 0, i32 1
  store ptr %page10, ptr %current_page, align 8
  %load.struct11 = load %_Z14BlockWatermark, ptr %1, align 8
  %page12 = extractvalue %_Z14BlockWatermark %load.struct11, 0
  %ne13 = icmp ne ptr %page12, null
  br i1 %ne13, label %if.then, label %if.else

if.then:                                          ; preds = %while.exit
  %load.struct14 = load %_Z14BlockWatermark, ptr %1, align 8
  %next_object = extractvalue %_Z14BlockWatermark %load.struct14, 1
  br label %if.end

if.else:                                          ; preds = %while.exit
  %load.struct15 = load %_Z14BlockWatermark, ptr %1, align 8
  %next_object16 = extractvalue %_Z14BlockWatermark %load.struct15, 1
  %next_object17 = getelementptr inbounds %_Z4Page, ptr %0, i32 0, i32 0
  store ptr %next_object16, ptr %next_object17, align 8
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  %if.value = phi ptr [ %next_object, %if.then ], [ %next_object16, %if.else ]
  ret void
}

define linkonce_odr void @_ZN4Page27deallocate_extensions_afterEP4Page(ptr %0, ptr %1) {
entry:
  br label %while.cond

while.cond:                                       ; preds = %while.body, %entry
  %load.struct = load %_Z4Page, ptr %0, align 8
  %next_page = extractvalue %_Z4Page %load.struct, 2
  %ne = icmp ne ptr %next_page, null
  %load.struct1 = load %_Z4Page, ptr %0, align 8
  %next_page2 = extractvalue %_Z4Page %load.struct1, 2
  %ne3 = icmp ne ptr %next_page2, %1
  %and = and i1 %ne, %ne3
  br i1 %and, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %load.struct4 = load %_Z4Page, ptr %0, align 8
  %next_page5 = extractvalue %_Z4Page %load.struct4, 2
  %load.struct6 = load %_Z4Page, ptr %next_page5, align 8
  %next_page7 = extractvalue %_Z4Page %load.struct6, 2
  %next_page8 = getelementptr inbounds %_Z4Page, ptr %0, i32 0, i32 2
  store ptr %next_page7, ptr %next_page8, align 8
  call void @_Z18scaly_release_pageP4Page(ptr %next_page5)
  br label %while.cond

while.exit:                                       ; preds = %while.cond
  ret void
}

define linkonce_odr void @_ZN4PageC1Ev(ptr %0) {
entry:
  ret void
}

declare ptr @getenv(ptr)

declare i64 @atexit(ptr)

declare ptr @strdup(ptr)

define linkonce_odr ptr @_Z21scaly_fmt_scratch_ptrv() {
entry:
  %global.load = load ptr, ptr @fmt_scratch, align 8
  %eq = icmp eq ptr %global.load, null
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %call = call ptr @aligned_alloc(i64 1, i64 32)
  store ptr %call, ptr @fmt_scratch, align 8
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %global.load1 = load ptr, ptr @fmt_scratch, align 8
  ret ptr %global.load1
}

define linkonce_odr void @_Z11scaly_eputsP10const_char(ptr %0) {
entry:
  %call = call i64 @strlen(ptr %0)
  %call1 = call i64 @write(i64 2, ptr %0, i64 %call)
  ret void
}

define linkonce_odr void @_Z12scaly_eputnlv() {
entry:
  %arg.tmp = alloca { ptr }, align 8
  store { ptr } { ptr @.sconst }, ptr %arg.tmp, align 1
  %call = call i64 @write(i64 2, ptr %arg.tmp, i64 1)
  ret void
}

define linkonce_odr void @_Z11scaly_eputi3i64(i64 %0) {
entry:
  %call = call ptr @_Z21scaly_fmt_scratch_ptrv()
  %pos = alloca i64, align 8
  store i64 31, ptr %pos, align 1
  %val = alloca i64, align 8
  store i64 %0, ptr %val, align 1
  %negative = alloca i1, align 1
  store i1 false, ptr %negative, align 1
  %val1 = load i64, ptr %val, align 4
  %lt = icmp slt i64 %val1, 0
  br i1 %lt, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  store i1 true, ptr %negative, align 1
  %val2 = load i64, ptr %val, align 4
  %sub = sub i64 0, %val2
  store i64 %sub, ptr %val, align 1
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %val3 = load i64, ptr %val, align 4
  %ne = icmp ne i64 %val3, 0
  br i1 %ne, label %if.then4, label %if.end5

if.then4:                                         ; preds = %if.end
  %pos6 = load i64, ptr %pos, align 4
  %sub7 = sub i64 %pos6, 1
  store i64 %sub7, ptr %pos, align 1
  %pos8 = load i64, ptr %pos, align 4
  %ptr.add = getelementptr inbounds i8, ptr %call, i64 %pos8
  store i8 48, ptr %ptr.add, align 1
  br label %if.end5

if.end5:                                          ; preds = %if.then4, %if.end
  br label %while.cond

while.cond:                                       ; preds = %while.body, %if.end5
  %val9 = load i64, ptr %val, align 4
  %gt = icmp sgt i64 %val9, 0
  br i1 %gt, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %pos10 = load i64, ptr %pos, align 4
  %sub11 = sub i64 %pos10, 1
  store i64 %sub11, ptr %pos, align 1
  %val12 = load i64, ptr %val, align 4
  %val13 = load i64, ptr %val, align 4
  %sdiv = sdiv i64 %val13, 10
  %mul = mul i64 %sdiv, 10
  %sub14 = sub i64 %val12, %mul
  %add = add i64 %sub14, 48
  %as.trunc = trunc i64 %add to i8
  %pos15 = load i64, ptr %pos, align 4
  %ptr.add16 = getelementptr inbounds i8, ptr %call, i64 %pos15
  store i8 %as.trunc, ptr %ptr.add16, align 1
  %val17 = load i64, ptr %val, align 4
  %sdiv18 = sdiv i64 %val17, 10
  store i64 %sdiv18, ptr %val, align 1
  br label %while.cond

while.exit:                                       ; preds = %while.cond
  %negative19 = load i1, ptr %negative, align 1
  br i1 %negative19, label %if.then20, label %if.end21

if.then20:                                        ; preds = %while.exit
  %pos22 = load i64, ptr %pos, align 4
  %sub23 = sub i64 %pos22, 1
  store i64 %sub23, ptr %pos, align 1
  %pos24 = load i64, ptr %pos, align 4
  %ptr.add25 = getelementptr inbounds i8, ptr %call, i64 %pos24
  store i8 45, ptr %ptr.add25, align 1
  br label %if.end21

if.end21:                                         ; preds = %if.then20, %while.exit
  %pos26 = load i64, ptr %pos, align 4
  %sub27 = sub i64 31, %pos26
  %pos28 = load i64, ptr %pos, align 4
  %ptr.add29 = getelementptr inbounds i8, ptr %call, i64 %pos28
  %call30 = call i64 @write(i64 2, ptr %ptr.add29, i64 %sub27)
  ret void
}

define linkonce_odr i8 @_Z9hex_digit3i64(i64 %0) {
entry:
  %lt = icmp slt i64 %0, 10
  br i1 %lt, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %add = add i64 %0, 48
  %as.trunc = trunc i64 %add to i8
  ret i8 %as.trunc

if.end:                                           ; preds = %entry
  %sub = sub i64 %0, 10
  %add1 = add i64 %sub, 97
  %as.trunc2 = trunc i64 %add1 to i8
  ret i8 %as.trunc2
}

define linkonce_odr void @_Z11scaly_eputpPv(ptr %0) {
entry:
  %call = call ptr @_Z21scaly_fmt_scratch_ptrv()
  %ptr.add = getelementptr inbounds i8, ptr %call, i64 0
  store i8 48, ptr %ptr.add, align 1
  %ptr.add1 = getelementptr inbounds i8, ptr %call, i64 1
  store i8 120, ptr %ptr.add1, align 1
  %as.ptrtoint = ptrtoint ptr %0 to i64
  %val = alloca i64, align 8
  store i64 %as.ptrtoint, ptr %val, align 1
  %i = alloca i64, align 8
  store i64 17, ptr %i, align 1
  br label %while.cond

while.cond:                                       ; preds = %while.body, %entry
  %i2 = load i64, ptr %i, align 4
  %ge = icmp sge i64 %i2, 2
  br i1 %ge, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %val3 = load i64, ptr %val, align 4
  %and = and i64 %val3, 15
  %call4 = call i8 @_Z9hex_digit3i64(i64 %and)
  %i5 = load i64, ptr %i, align 4
  %ptr.add6 = getelementptr inbounds i8, ptr %call, i64 %i5
  store i8 %call4, ptr %ptr.add6, align 1
  %val7 = load i64, ptr %val, align 4
  %lshr = lshr i64 %val7, 4
  store i64 %lshr, ptr %val, align 1
  %i8 = load i64, ptr %i, align 4
  %sub = sub i64 %i8, 1
  store i64 %sub, ptr %i, align 1
  br label %while.cond

while.exit:                                       ; preds = %while.cond
  %call9 = call i64 @write(i64 2, ptr %call, i64 18)
  ret void
}

define linkonce_odr ptr @_Z15stack_bucket_ofP4Page(ptr %0) {
entry:
  %as.ptrtoint = ptrtoint ptr %0 to i64
  %and = and i64 %as.ptrtoint, -262144
  %as.inttoptr = inttoptr i64 %and to ptr
  ret ptr %as.inttoptr
}

define linkonce_odr ptr @_Z14heap_bucket_ofP4Page(ptr %0) {
entry:
  %as.ptrtoint = ptrtoint ptr %0 to i64
  %and = and i64 %as.ptrtoint, -262144
  %as.inttoptr = inttoptr i64 %and to ptr
  ret ptr %as.inttoptr
}

define linkonce_odr ptr @_Z17first_usable_pagePv(ptr %0) {
entry:
  %as.ptrtoint = ptrtoint ptr %0 to i64
  %add = add i64 %as.ptrtoint, 4096
  %as.inttoptr = inttoptr i64 %add to ptr
  ret ptr %as.inttoptr
}

define linkonce_odr ptr @_Z16last_usable_pagePv(ptr %0) {
entry:
  %as.ptrtoint = ptrtoint ptr %0 to i64
  %add = add i64 %as.ptrtoint, 258048
  %as.inttoptr = inttoptr i64 %add to ptr
  ret ptr %as.inttoptr
}

define linkonce_odr void @_Z10reset_pageP4Page(ptr %0) {
entry:
  %as.ptrtoint = ptrtoint ptr %0 to i64
  %add = add i64 %as.ptrtoint, ptrtoint (ptr getelementptr (%_Z4Page, ptr null, i32 1) to i64)
  %as.inttoptr = inttoptr i64 %add to ptr
  %next_object = getelementptr inbounds %_Z4Page, ptr %0, i32 0, i32 0
  store ptr %as.inttoptr, ptr %next_object, align 8
  %current_page = getelementptr inbounds %_Z4Page, ptr %0, i32 0, i32 1
  store ptr null, ptr %current_page, align 8
  %next_page = getelementptr inbounds %_Z4Page, ptr %0, i32 0, i32 2
  store ptr null, ptr %next_page, align 8
  %struct.init = alloca %_Z8PageList, align 8
  %tuple.field = getelementptr inbounds %_Z8PageList, ptr %struct.init, i32 0, i32 0
  store ptr null, ptr %tuple.field, align 8
  %exclusive_pages = getelementptr inbounds %_Z4Page, ptr %0, i32 0, i32 3
  %field.load = load %_Z8PageList, ptr %struct.init, align 8
  store %_Z8PageList %field.load, ptr %exclusive_pages, align 8
  ret void
}

define linkonce_odr ptr @_Z19create_stack_bucketP17StackBucketHeader(ptr %0) {
entry:
  %call = call ptr @aligned_alloc(i64 262144, i64 262144)
  %eq = icmp eq ptr %call, null
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  call void @exit(i64 101)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %prev = getelementptr inbounds %_Z17StackBucketHeader, ptr %call, i32 0, i32 0
  store ptr %0, ptr %prev, align 8
  %next = getelementptr inbounds %_Z17StackBucketHeader, ptr %call, i32 0, i32 1
  store ptr null, ptr %next, align 8
  %ne = icmp ne ptr %0, null
  br i1 %ne, label %if.then1, label %if.end2

if.then1:                                         ; preds = %if.end
  %next3 = getelementptr inbounds %_Z17StackBucketHeader, ptr %0, i32 0, i32 1
  store ptr %call, ptr %next3, align 8
  br label %if.end2

if.end2:                                          ; preds = %if.then1, %if.end
  ret ptr %call
}

define linkonce_odr ptr @_Z18create_heap_bucketv() {
entry:
  %call = call ptr @aligned_alloc(i64 262144, i64 262144)
  %eq = icmp eq ptr %call, null
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  call void @exit(i64 102)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %prev = getelementptr inbounds %_Z16HeapBucketHeader, ptr %call, i32 0, i32 0
  store ptr null, ptr %prev, align 8
  %next = getelementptr inbounds %_Z16HeapBucketHeader, ptr %call, i32 0, i32 1
  store ptr null, ptr %next, align 8
  %bitmap = getelementptr inbounds %_Z16HeapBucketHeader, ptr %call, i32 0, i32 2
  store i64 9223372036854775807, ptr %bitmap, align 4
  ret ptr %call
}

define linkonce_odr i32 @_Z7ctz_u643u64(i64 %0) {
entry:
  %n = alloca i64, align 8
  store i64 0, ptr %n, align 1
  %v = alloca i64, align 8
  store i64 %0, ptr %v, align 1
  %v1 = load i64, ptr %v, align 4
  %and = and i64 %v1, 4294967295
  %eq = icmp eq i64 %and, 0
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %n2 = load i64, ptr %n, align 4
  %add = add i64 %n2, 32
  store i64 %add, ptr %n, align 1
  %v3 = load i64, ptr %v, align 4
  %lshr = lshr i64 %v3, 32
  store i64 %lshr, ptr %v, align 1
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %v4 = load i64, ptr %v, align 4
  %and5 = and i64 %v4, 65535
  %eq6 = icmp eq i64 %and5, 0
  br i1 %eq6, label %if.then7, label %if.end8

if.then7:                                         ; preds = %if.end
  %n9 = load i64, ptr %n, align 4
  %add10 = add i64 %n9, 16
  store i64 %add10, ptr %n, align 1
  %v11 = load i64, ptr %v, align 4
  %lshr12 = lshr i64 %v11, 16
  store i64 %lshr12, ptr %v, align 1
  br label %if.end8

if.end8:                                          ; preds = %if.then7, %if.end
  %v13 = load i64, ptr %v, align 4
  %and14 = and i64 %v13, 255
  %eq15 = icmp eq i64 %and14, 0
  br i1 %eq15, label %if.then16, label %if.end17

if.then16:                                        ; preds = %if.end8
  %n18 = load i64, ptr %n, align 4
  %add19 = add i64 %n18, 8
  store i64 %add19, ptr %n, align 1
  %v20 = load i64, ptr %v, align 4
  %lshr21 = lshr i64 %v20, 8
  store i64 %lshr21, ptr %v, align 1
  br label %if.end17

if.end17:                                         ; preds = %if.then16, %if.end8
  %v22 = load i64, ptr %v, align 4
  %and23 = and i64 %v22, 15
  %eq24 = icmp eq i64 %and23, 0
  br i1 %eq24, label %if.then25, label %if.end26

if.then25:                                        ; preds = %if.end17
  %n27 = load i64, ptr %n, align 4
  %add28 = add i64 %n27, 4
  store i64 %add28, ptr %n, align 1
  %v29 = load i64, ptr %v, align 4
  %lshr30 = lshr i64 %v29, 4
  store i64 %lshr30, ptr %v, align 1
  br label %if.end26

if.end26:                                         ; preds = %if.then25, %if.end17
  %v31 = load i64, ptr %v, align 4
  %and32 = and i64 %v31, 3
  %eq33 = icmp eq i64 %and32, 0
  br i1 %eq33, label %if.then34, label %if.end35

if.then34:                                        ; preds = %if.end26
  %n36 = load i64, ptr %n, align 4
  %add37 = add i64 %n36, 2
  store i64 %add37, ptr %n, align 1
  %v38 = load i64, ptr %v, align 4
  %lshr39 = lshr i64 %v38, 2
  store i64 %lshr39, ptr %v, align 1
  br label %if.end35

if.end35:                                         ; preds = %if.then34, %if.end26
  %v40 = load i64, ptr %v, align 4
  %and41 = and i64 %v40, 1
  %eq42 = icmp eq i64 %and41, 0
  br i1 %eq42, label %if.then43, label %if.end44

if.then43:                                        ; preds = %if.end35
  %n45 = load i64, ptr %n, align 4
  %add46 = add i64 %n45, 1
  store i64 %add46, ptr %n, align 1
  br label %if.end44

if.end44:                                         ; preds = %if.then43, %if.end35
  %n47 = load i64, ptr %n, align 4
  %as.trunc = trunc i64 %n47 to i32
  ret i32 %as.trunc
}

define linkonce_odr ptr @_Z21scaly_alloc_root_pagev() {
entry:
  %global.load = load ptr, ptr @stack_top, align 8
  %eq = icmp eq ptr %global.load, null
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %global.load1 = load ptr, ptr @stack_head, align 8
  %eq2 = icmp eq ptr %global.load1, null
  br i1 %eq2, label %if.then3, label %if.end4

if.end:                                           ; preds = %entry
  %global.load7 = load ptr, ptr @stack_top, align 8
  %call8 = call ptr @_Z15stack_bucket_ofP4Page(ptr %global.load7)
  %global.load9 = load ptr, ptr @stack_top, align 8
  %as.ptrtoint = ptrtoint ptr %global.load9 to i64
  %add = add i64 %as.ptrtoint, 4096
  %as.inttoptr = inttoptr i64 %add to ptr
  %call10 = call ptr @_Z16last_usable_pagePv(ptr %call8)
  %gt = icmp ugt ptr %as.inttoptr, %call10
  br i1 %gt, label %if.then11, label %if.end12

if.then3:                                         ; preds = %if.then
  %call = call ptr @_Z19create_stack_bucketP17StackBucketHeader(ptr null)
  store ptr %call, ptr @stack_head, align 8
  br label %if.end4

if.end4:                                          ; preds = %if.then3, %if.then
  %global.load5 = load ptr, ptr @stack_head, align 8
  %call6 = call ptr @_Z17first_usable_pagePv(ptr %global.load5)
  store ptr %call6, ptr @stack_top, align 8
  call void @_Z10reset_pageP4Page(ptr %call6)
  ret ptr %call6

if.then11:                                        ; preds = %if.end
  %load.struct = load %_Z17StackBucketHeader, ptr %call8, align 8
  %next = extractvalue %_Z17StackBucketHeader %load.struct, 1
  %eq13 = icmp eq ptr %next, null
  br i1 %eq13, label %if.then14, label %if.end15

if.end12:                                         ; preds = %if.end
  store ptr %as.inttoptr, ptr @stack_top, align 8
  call void @_Z10reset_pageP4Page(ptr %as.inttoptr)
  ret ptr %as.inttoptr

if.then14:                                        ; preds = %if.then11
  %call16 = call ptr @_Z19create_stack_bucketP17StackBucketHeader(ptr %call8)
  %next17 = getelementptr inbounds %_Z17StackBucketHeader, ptr %call8, i32 0, i32 1
  store ptr %call16, ptr %next17, align 8
  br label %if.end15

if.end15:                                         ; preds = %if.then14, %if.then11
  %field.inplace = getelementptr inbounds %_Z17StackBucketHeader, ptr %call8, i32 0, i32 1
  %deref.recv = load ptr, ptr %field.inplace, align 8
  %call18 = call ptr @_Z17first_usable_pagePv(ptr %deref.recv)
  store ptr %call18, ptr @stack_top, align 8
  call void @_Z10reset_pageP4Page(ptr %call18)
  ret ptr %call18
}

define linkonce_odr void @_Z23scaly_release_root_pageP4Page(ptr %0) {
entry:
  %global.load = load ptr, ptr @stack_top, align 8
  %ne = icmp ne ptr %0, %global.load
  br i1 %ne, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  call void @_Z11scaly_eputsP10const_char(ptr @.str)
  call void @_Z11scaly_eputpPv(ptr %0)
  call void @_Z11scaly_eputsP10const_char(ptr @.str.2)
  %global.load1 = load ptr, ptr @stack_top, align 8
  call void @_Z11scaly_eputpPv(ptr %global.load1)
  call void @_Z12scaly_eputnlv()
  call void @abort()
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %call = call ptr @_Z15stack_bucket_ofP4Page(ptr %0)
  %call2 = call ptr @_Z17first_usable_pagePv(ptr %call)
  %ne3 = icmp ne ptr %0, %call2
  br i1 %ne3, label %if.then4, label %if.end5

if.then4:                                         ; preds = %if.end
  %as.ptrtoint = ptrtoint ptr %0 to i64
  %sub = sub i64 %as.ptrtoint, 4096
  %as.inttoptr = inttoptr i64 %sub to ptr
  store ptr %as.inttoptr, ptr @stack_top, align 8
  ret void

if.end5:                                          ; preds = %if.end
  %load.struct = load %_Z17StackBucketHeader, ptr %call, align 8
  %prev = extractvalue %_Z17StackBucketHeader %load.struct, 0
  %eq = icmp eq ptr %prev, null
  br i1 %eq, label %if.then6, label %if.end7

if.then6:                                         ; preds = %if.end5
  store ptr null, ptr @stack_top, align 8
  ret void

if.end7:                                          ; preds = %if.end5
  %field.inplace = getelementptr inbounds %_Z17StackBucketHeader, ptr %call, i32 0, i32 0
  %deref.recv = load ptr, ptr %field.inplace, align 8
  %call8 = call ptr @_Z16last_usable_pagePv(ptr %deref.recv)
  store ptr %call8, ptr @stack_top, align 8
  ret void
}

define linkonce_odr void @_Z28scaly_release_root_page_fullP4Page(ptr %0) {
entry:
  %used = alloca i1, align 1
  %eq = icmp eq ptr %0, null
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret void

if.end:                                           ; preds = %entry
  %as.ptrtoint = ptrtoint ptr %0 to i64
  %add = add i64 %as.ptrtoint, ptrtoint (ptr getelementptr (%_Z4Page, ptr null, i32 1) to i64)
  %as.inttoptr = inttoptr i64 %add to ptr
  store i1 false, ptr %used, align 1
  %load.struct = load %_Z4Page, ptr %0, align 8
  %next_object = extractvalue %_Z4Page %load.struct, 0
  %ne = icmp ne ptr %next_object, %as.inttoptr
  br i1 %ne, label %if.then1, label %if.end2

if.then1:                                         ; preds = %if.end
  store i1 true, ptr %used, align 1
  br label %if.end2

if.end2:                                          ; preds = %if.then1, %if.end
  %load.struct3 = load %_Z4Page, ptr %0, align 8
  %next_page = extractvalue %_Z4Page %load.struct3, 2
  %ne4 = icmp ne ptr %next_page, null
  br i1 %ne4, label %if.then5, label %if.end6

if.then5:                                         ; preds = %if.end2
  store i1 true, ptr %used, align 1
  br label %if.end6

if.end6:                                          ; preds = %if.then5, %if.end2
  %load.struct7 = load %_Z4Page, ptr %0, align 8
  %exclusive_pages = extractvalue %_Z4Page %load.struct7, 3
  %head = extractvalue %_Z8PageList %exclusive_pages, 0
  %ne8 = icmp ne ptr %head, null
  br i1 %ne8, label %if.then9, label %if.end10

if.then9:                                         ; preds = %if.end6
  store i1 true, ptr %used, align 1
  br label %if.end10

if.end10:                                         ; preds = %if.then9, %if.end6
  %used11 = load i1, ptr %used, align 1
  br i1 %used11, label %if.then12, label %if.end13

if.then12:                                        ; preds = %if.end10
  call void @_ZN4Page21deallocate_extensionsEv(ptr %0)
  br label %if.end13

if.end13:                                         ; preds = %if.then12, %if.end10
  call void @_Z23scaly_release_root_pageP4Page(ptr %0)
  ret void
}

define linkonce_odr ptr @_Z16scaly_alloc_pagev() {
entry:
  %global.load = load ptr, ptr @heap_head, align 8
  %eq = icmp eq ptr %global.load, null
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %call = call ptr @_Z18create_heap_bucketv()
  store ptr %call, ptr @heap_head, align 8
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %global.load1 = load ptr, ptr @heap_head, align 8
  %field.inplace = getelementptr inbounds %_Z16HeapBucketHeader, ptr %global.load1, i32 0, i32 2
  %field.val = load i64, ptr %field.inplace, align 4
  %call2 = call i32 @_Z7ctz_u643u64(i64 %field.val)
  %load.struct = load %_Z16HeapBucketHeader, ptr %global.load1, align 8
  %bitmap = extractvalue %_Z16HeapBucketHeader %load.struct, 2
  %zext = zext i32 %call2 to i64
  %shl = shl i64 1, %zext
  %bitnot = xor i64 %shl, -1
  %and = and i64 %bitmap, %bitnot
  %bitmap3 = getelementptr inbounds %_Z16HeapBucketHeader, ptr %global.load1, i32 0, i32 2
  store i64 %and, ptr %bitmap3, align 4
  %as.ptrtoint = ptrtoint ptr %global.load1 to i64
  %zext4 = zext i32 %call2 to i64
  %add = add i64 %zext4, 1
  %mul = mul i64 %add, 4096
  %add5 = add i64 %as.ptrtoint, %mul
  %as.inttoptr = inttoptr i64 %add5 to ptr
  call void @_Z10reset_pageP4Page(ptr %as.inttoptr)
  %load.struct6 = load %_Z16HeapBucketHeader, ptr %global.load1, align 8
  %bitmap7 = extractvalue %_Z16HeapBucketHeader %load.struct6, 2
  %eq8 = icmp eq i64 %bitmap7, 0
  br i1 %eq8, label %if.then9, label %if.end10

if.then9:                                         ; preds = %if.end
  %load.struct11 = load %_Z16HeapBucketHeader, ptr %global.load1, align 8
  %next = extractvalue %_Z16HeapBucketHeader %load.struct11, 1
  store ptr %next, ptr @heap_head, align 8
  %global.load12 = load ptr, ptr @heap_head, align 8
  %ne = icmp ne ptr %global.load12, null
  br i1 %ne, label %if.then13, label %if.end14

if.end10:                                         ; preds = %if.end14, %if.end
  ret ptr %as.inttoptr

if.then13:                                        ; preds = %if.then9
  br label %if.end14

if.end14:                                         ; preds = %if.then13, %if.then9
  %next15 = getelementptr inbounds %_Z16HeapBucketHeader, ptr %global.load1, i32 0, i32 1
  store ptr null, ptr %next15, align 8
  %prev = getelementptr inbounds %_Z16HeapBucketHeader, ptr %global.load1, i32 0, i32 0
  store ptr null, ptr %prev, align 8
  br label %if.end10
}

define linkonce_odr void @_Z18scaly_release_pageP4Page(ptr %0) {
entry:
  %load.struct = load %_Z4Page, ptr %0, align 8
  %next_object = extractvalue %_Z4Page %load.struct, 0
  %eq = icmp eq ptr %next_object, null
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  call void @free(ptr %0)
  ret void

if.end:                                           ; preds = %entry
  %call = call ptr @_Z14heap_bucket_ofP4Page(ptr %0)
  %as.ptrtoint = ptrtoint ptr %0 to i64
  %as.ptrtoint1 = ptrtoint ptr %call to i64
  %sub = sub i64 %as.ptrtoint, %as.ptrtoint1
  %udiv = udiv i64 %sub, 4096
  %sub2 = sub i64 %udiv, 1
  %load.struct3 = load %_Z16HeapBucketHeader, ptr %call, align 8
  %bitmap = extractvalue %_Z16HeapBucketHeader %load.struct3, 2
  %eq4 = icmp eq i64 %bitmap, 0
  %load.struct5 = load %_Z16HeapBucketHeader, ptr %call, align 8
  %bitmap6 = extractvalue %_Z16HeapBucketHeader %load.struct5, 2
  %shl = shl i64 1, %sub2
  %or = or i64 %bitmap6, %shl
  %bitmap7 = getelementptr inbounds %_Z16HeapBucketHeader, ptr %call, i32 0, i32 2
  store i64 %or, ptr %bitmap7, align 4
  br i1 %eq4, label %if.then8, label %if.end9

if.then8:                                         ; preds = %if.end
  %global.load = load ptr, ptr @heap_head, align 8
  %next = getelementptr inbounds %_Z16HeapBucketHeader, ptr %call, i32 0, i32 1
  store ptr %global.load, ptr %next, align 8
  %prev = getelementptr inbounds %_Z16HeapBucketHeader, ptr %call, i32 0, i32 0
  store ptr null, ptr %prev, align 8
  %global.load10 = load ptr, ptr @heap_head, align 8
  %ne = icmp ne ptr %global.load10, null
  br i1 %ne, label %if.then11, label %if.end12

if.end9:                                          ; preds = %if.end12, %if.end
  ret void

if.then11:                                        ; preds = %if.then8
  br label %if.end12

if.end12:                                         ; preds = %if.then11, %if.then8
  store ptr %call, ptr @heap_head, align 8
  br label %if.end9
}

define linkonce_odr void @_Z16scaly_trace_dumpv() {
entry:
  %deref.tmp6 = alloca %_Z10TraceEntry, align 8
  %deref.tmp4 = alloca %_Z10TraceEntry, align 8
  %deref.tmp = alloca %_Z10TraceEntry, align 8
  %i = alloca i64, align 8
  store i64 0, ptr %i, align 1
  %any = alloca i1, align 1
  store i1 false, ptr %any, align 1
  br label %while.cond

while.cond:                                       ; preds = %if.end, %entry
  %i1 = load i64, ptr %i, align 4
  %global.load = load i64, ptr @trace_count, align 4
  %lt = icmp slt i64 %i1, %global.load
  br i1 %lt, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %global.load2 = load ptr, ptr @trace_entries, align 8
  %i3 = load i64, ptr %i, align 4
  %ptr.add = getelementptr inbounds %_Z10TraceEntry, ptr %global.load2, i64 %i3
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %deref.tmp, ptr align 1 %ptr.add, i64 ptrtoint (ptr getelementptr (%_Z10TraceEntry, ptr null, i32 1) to i64), i1 false)
  %grp.deref.val = load %_Z10TraceEntry, ptr %deref.tmp, align 8
  %push_count = extractvalue %_Z10TraceEntry %grp.deref.val, 1
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %deref.tmp4, ptr align 1 %ptr.add, i64 ptrtoint (ptr getelementptr (%_Z10TraceEntry, ptr null, i32 1) to i64), i1 false)
  %grp.deref.val5 = load %_Z10TraceEntry, ptr %deref.tmp4, align 8
  %pop_count = extractvalue %_Z10TraceEntry %grp.deref.val5, 2
  %eq = icmp eq i64 %push_count, %pop_count
  br i1 %eq, label %if.then, label %if.end

while.exit:                                       ; preds = %while.cond
  %any13 = load i1, ptr %any, align 1
  br i1 %any13, label %if.then14, label %if.end15

if.then:                                          ; preds = %while.body
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %deref.tmp6, ptr align 1 %ptr.add, i64 ptrtoint (ptr getelementptr (%_Z10TraceEntry, ptr null, i32 1) to i64), i1 false)
  %grp.deref.val7 = load %_Z10TraceEntry, ptr %deref.tmp6, align 8
  %push_count8 = extractvalue %_Z10TraceEntry %grp.deref.val7, 1
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %deref.tmp6, ptr align 1 %ptr.add, i64 ptrtoint (ptr getelementptr (%_Z10TraceEntry, ptr null, i32 1) to i64), i1 false)
  %grp.deref.val9 = load %_Z10TraceEntry, ptr %deref.tmp6, align 8
  %pop_count10 = extractvalue %_Z10TraceEntry %grp.deref.val9, 2
  call void @_Z11scaly_eputsP10const_char(ptr @.str.3)
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %deref.tmp6, ptr align 1 %ptr.add, i64 ptrtoint (ptr getelementptr (%_Z10TraceEntry, ptr null, i32 1) to i64), i1 false)
  %grp.deref.val11 = load %_Z10TraceEntry, ptr %deref.tmp6, align 8
  %name = extractvalue %_Z10TraceEntry %grp.deref.val11, 0
  call void @_Z11scaly_eputsP10const_char(ptr %name)
  call void @_Z11scaly_eputsP10const_char(ptr @.str.4)
  call void @_Z11scaly_eputi3i64(i64 %push_count8)
  call void @_Z11scaly_eputsP10const_char(ptr @.str.5)
  call void @_Z11scaly_eputi3i64(i64 %pop_count10)
  call void @_Z11scaly_eputsP10const_char(ptr @.str.6)
  %sub = sub i64 %push_count8, %pop_count10
  call void @_Z11scaly_eputi3i64(i64 %sub)
  call void @_Z12scaly_eputnlv()
  store i1 true, ptr %any, align 1
  br label %if.end

if.end:                                           ; preds = %if.then, %while.body
  %i12 = load i64, ptr %i, align 4
  %add = add i64 %i12, 1
  store i64 %add, ptr %i, align 1
  br label %while.cond

if.then14:                                        ; preds = %while.exit
  ret void

if.end15:                                         ; preds = %while.exit
  call void @_Z11scaly_eputsP10const_char(ptr @.str.7)
  call void @_Z12scaly_eputnlv()
  ret void
}

define linkonce_odr void @_Z23scaly_rt_register_tracev() {
entry:
  %global.load = load i64, ptr @trace_atexit_registered, align 4
  %ne = icmp ne i64 %global.load, 0
  br i1 %ne, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret void

if.end:                                           ; preds = %entry
  store i64 1, ptr @trace_atexit_registered, align 4
  %call = call ptr @getenv(ptr @.str.8)
  %eq = icmp eq ptr %call, null
  br i1 %eq, label %if.then1, label %if.end2

if.then1:                                         ; preds = %if.end
  ret void

if.end2:                                          ; preds = %if.end
  store i64 1, ptr @trace_enabled, align 4
  %call3 = call ptr @aligned_alloc(i64 8, i64 mul (i64 ptrtoint (ptr getelementptr (%_Z10TraceEntry, ptr null, i32 1) to i64), i64 8192))
  store ptr %call3, ptr @trace_entries, align 8
  ret void
}

define linkonce_odr ptr @_Z23scaly_trace_find_or_addP10const_char(ptr %0) {
entry:
  %deref.tmp = alloca %_Z10TraceEntry, align 8
  %i = alloca i64, align 8
  store i64 0, ptr %i, align 1
  br label %while.cond

while.cond:                                       ; preds = %if.end, %entry
  %i1 = load i64, ptr %i, align 4
  %global.load = load i64, ptr @trace_count, align 4
  %lt = icmp slt i64 %i1, %global.load
  br i1 %lt, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %global.load2 = load ptr, ptr @trace_entries, align 8
  %i3 = load i64, ptr %i, align 4
  %ptr.add = getelementptr inbounds %_Z10TraceEntry, ptr %global.load2, i64 %i3
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %deref.tmp, ptr align 1 %ptr.add, i64 ptrtoint (ptr getelementptr (%_Z10TraceEntry, ptr null, i32 1) to i64), i1 false)
  %grp.deref.val = load %_Z10TraceEntry, ptr %deref.tmp, align 8
  %name = extractvalue %_Z10TraceEntry %grp.deref.val, 0
  %call = call i64 @strcmp(ptr %name, ptr %0)
  %eq = icmp eq i64 %call, 0
  br i1 %eq, label %if.then, label %if.end

while.exit:                                       ; preds = %while.cond
  %global.load5 = load i64, ptr @trace_count, align 4
  %ge = icmp sge i64 %global.load5, 8192
  br i1 %ge, label %if.then6, label %if.end7

if.then:                                          ; preds = %while.body
  ret ptr %ptr.add

if.end:                                           ; preds = %while.body
  %i4 = load i64, ptr %i, align 4
  %add = add i64 %i4, 1
  store i64 %add, ptr %i, align 1
  br label %while.cond

if.then6:                                         ; preds = %while.exit
  ret ptr null

if.end7:                                          ; preds = %while.exit
  %global.load8 = load ptr, ptr @trace_entries, align 8
  %global.load9 = load i64, ptr @trace_count, align 4
  %ptr.add10 = getelementptr inbounds %_Z10TraceEntry, ptr %global.load8, i64 %global.load9
  %call11 = call ptr @strdup(ptr %0)
  %global.load12 = load i64, ptr @trace_count, align 4
  %add13 = add i64 %global.load12, 1
  store i64 %add13, ptr @trace_count, align 4
  ret ptr %ptr.add10
}

define linkonce_odr void @_Z16scaly_trace_pushP10const_char(ptr %0) {
entry:
  %deref.tmp = alloca %_Z10TraceEntry, align 8
  call void @_Z23scaly_rt_register_tracev()
  %global.load = load i64, ptr @trace_enabled, align 4
  %eq = icmp eq i64 %global.load, 0
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret void

if.end:                                           ; preds = %entry
  %call = call ptr @_Z23scaly_trace_find_or_addP10const_char(ptr %0)
  %ne = icmp ne ptr %call, null
  br i1 %ne, label %if.then1, label %if.end2

if.then1:                                         ; preds = %if.end
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %deref.tmp, ptr align 1 %call, i64 ptrtoint (ptr getelementptr (%_Z10TraceEntry, ptr null, i32 1) to i64), i1 false)
  %grp.deref.val = load %_Z10TraceEntry, ptr %deref.tmp, align 8
  %push_count = extractvalue %_Z10TraceEntry %grp.deref.val, 1
  %add = add i64 %push_count, 1
  br label %if.end2

if.end2:                                          ; preds = %if.then1, %if.end
  ret void
}

define linkonce_odr void @_Z15scaly_trace_popP10const_char(ptr %0) {
entry:
  %deref.tmp = alloca %_Z10TraceEntry, align 8
  call void @_Z23scaly_rt_register_tracev()
  %global.load = load i64, ptr @trace_enabled, align 4
  %eq = icmp eq i64 %global.load, 0
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret void

if.end:                                           ; preds = %entry
  %call = call ptr @_Z23scaly_trace_find_or_addP10const_char(ptr %0)
  %ne = icmp ne ptr %call, null
  br i1 %ne, label %if.then1, label %if.end2

if.then1:                                         ; preds = %if.end
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %deref.tmp, ptr align 1 %call, i64 ptrtoint (ptr getelementptr (%_Z10TraceEntry, ptr null, i32 1) to i64), i1 false)
  %grp.deref.val = load %_Z10TraceEntry, ptr %deref.tmp, align 8
  %pop_count = extractvalue %_Z10TraceEntry %grp.deref.val, 2
  %add = add i64 %pop_count, 1
  br label %if.end2

if.end2:                                          ; preds = %if.then1, %if.end
  ret void
}

define linkonce_odr ptr @_ZN6VectorIiE3getEPN4scaly6memory4PageEm(ptr %0, ptr %1, i64 %2) {
entry:
  %load.struct = load %_Z6VectorIiE, ptr %1, align 8
  %length = extractvalue %_Z6VectorIiE %load.struct, 0
  %ge = icmp uge i64 %2, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %load.struct1 = load %_Z6VectorIiE, ptr %1, align 8
  %data = extractvalue %_Z6VectorIiE %load.struct1, 1
  %ptr.add = getelementptr inbounds i64, ptr %data, i64 %2
  ret ptr %ptr.add
}

define linkonce_odr ptr @_ZN6VectorIiE7get_ptrEm(ptr %0, i64 %1) {
entry:
  %load.struct = load %_Z6VectorIiE, ptr %0, align 8
  %length = extractvalue %_Z6VectorIiE %load.struct, 0
  %ge = icmp uge i64 %1, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %load.struct1 = load %_Z6VectorIiE, ptr %0, align 8
  %data = extractvalue %_Z6VectorIiE %load.struct1, 1
  %ptr.add = getelementptr inbounds i64, ptr %data, i64 %1
  ret ptr %ptr.add
}

define linkonce_odr void @_ZN6VectorIiE3putEmi(ptr %0, i64 %1, i64 %2) {
entry:
  %load.struct = load %_Z6VectorIiE, ptr %0, align 8
  %length = extractvalue %_Z6VectorIiE %load.struct, 0
  %ge = icmp uge i64 %1, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  call void @_Z11scaly_eputsP10const_char(ptr @.str.9)
  call void @_Z12scaly_eputnlv()
  call void @exit(i64 15)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %load.struct1 = load %_Z6VectorIiE, ptr %0, align 8
  %data = extractvalue %_Z6VectorIiE %load.struct1, 1
  %ptr.add = getelementptr inbounds i64, ptr %data, i64 %1
  store i64 %2, ptr %ptr.add, align 4
  ret void
}

define linkonce_odr ptr @_ZN14VectorIteratorIiE4nextEv(ptr %0) {
entry:
  %load.struct = load %_Z14VectorIteratorIiE, ptr %0, align 8
  %vector = extractvalue %_Z14VectorIteratorIiE %load.struct, 0
  %eq = icmp eq ptr %vector, null
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %load.struct1 = load %_Z14VectorIteratorIiE, ptr %0, align 8
  %position = extractvalue %_Z14VectorIteratorIiE %load.struct1, 1
  %load.struct2 = load %_Z14VectorIteratorIiE, ptr %0, align 8
  %vector3 = extractvalue %_Z14VectorIteratorIiE %load.struct2, 0
  %deref = load %_Z6VectorIiE, ptr %vector3, align 8
  %length = extractvalue %_Z6VectorIiE %deref, 0
  %eq4 = icmp eq i64 %position, %length
  br i1 %eq4, label %if.then5, label %if.end6

if.then5:                                         ; preds = %if.end
  ret ptr null

if.end6:                                          ; preds = %if.end
  %load.struct7 = load %_Z14VectorIteratorIiE, ptr %0, align 8
  %position8 = extractvalue %_Z14VectorIteratorIiE %load.struct7, 1
  %add = add i64 %position8, 1
  %position9 = getelementptr inbounds %_Z14VectorIteratorIiE, ptr %0, i32 0, i32 1
  store i64 %add, ptr %position9, align 4
  %field.inplace = getelementptr inbounds %_Z14VectorIteratorIiE, ptr %0, i32 0, i32 0
  %deref.recv = load ptr, ptr %field.inplace, align 8
  %load.struct10 = load %_Z14VectorIteratorIiE, ptr %0, align 8
  %position11 = extractvalue %_Z14VectorIteratorIiE %load.struct10, 1
  %sub = sub i64 %position11, 1
  %call = call ptr @_ZN6VectorIiE7get_ptrEm(ptr %deref.recv, i64 %sub)
  ret ptr %call
}

define linkonce_odr void @_ZN14VectorIteratorIiEC1EP6VectorIiE(ptr %0, ptr %1) {
entry:
  %vector = getelementptr inbounds %_Z14VectorIteratorIiE, ptr %0, i32 0, i32 0
  store ptr %1, ptr %vector, align 8
  %position = getelementptr inbounds %_Z14VectorIteratorIiE, ptr %0, i32 0, i32 1
  store i64 0, ptr %position, align 4
  ret void
}

define linkonce_odr void @_ZN14VectorIteratorIiEC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr void @_ZN6VectorIiE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z14VectorIteratorIiE) %0, ptr %1, ptr %2) {
entry:
  %struct.init = alloca %_Z14VectorIteratorIiE, align 8
  call void @_ZN14VectorIteratorIiEC1EP6VectorIiE(ptr %struct.init, ptr %2)
  %sret.body = load %_Z14VectorIteratorIiE, ptr %struct.init, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %struct.init, i64 ptrtoint (ptr getelementptr (%_Z14VectorIteratorIiE, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN6VectorIiE8as_sliceEPN4scaly6memory4PageE(ptr noalias sret(%_Z5Slice) %0, ptr %1, ptr %2) {
entry:
  %load.struct = load %_Z6VectorIiE, ptr %2, align 8
  %data = extractvalue %_Z6VectorIiE %load.struct, 1
  %load.struct1 = load %_Z6VectorIiE, ptr %2, align 8
  %length = extractvalue %_Z6VectorIiE %load.struct1, 0
  %tuple = alloca { ptr, i64 }, align 8
  %tuple.field = getelementptr inbounds { ptr, i64 }, ptr %tuple, i32 0, i32 0
  store ptr %data, ptr %tuple.field, align 1
  %tuple.field2 = getelementptr inbounds { ptr, i64 }, ptr %tuple, i32 0, i32 1
  store i64 %length, ptr %tuple.field2, align 1
  %tuple.val = load { ptr, i64 }, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr ({ ptr, i64 }, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN6VectorIiEC1Ev(ptr %0) {
entry:
  %length = getelementptr inbounds %_Z6VectorIiE, ptr %0, i32 0, i32 0
  store i64 0, ptr %length, align 4
  %data = getelementptr inbounds %_Z6VectorIiE, ptr %0, i32 0, i32 1
  store ptr null, ptr %data, align 8
  ret void
}

define linkonce_odr void @_ZN6VectorIiEC1EPim(ptr %0, ptr %1, i64 %2) {
entry:
  %length = getelementptr inbounds %_Z6VectorIiE, ptr %0, i32 0, i32 0
  store i64 %2, ptr %length, align 4
  %data = getelementptr inbounds %_Z6VectorIiE, ptr %0, i32 0, i32 1
  store ptr %1, ptr %data, align 8
  ret void
}

define linkonce_odr void @_ZN6VectorIiEC1EPN4scaly6memory4PageEm(ptr %0, ptr %1, i64 %2) {
entry:
  %length = getelementptr inbounds %_Z6VectorIiE, ptr %0, i32 0, i32 0
  store i64 %2, ptr %length, align 4
  %gt = icmp ugt i64 %2, 0
  br i1 %gt, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %mul = mul i64 %2, ptrtoint (ptr getelementptr (i64, ptr null, i32 1) to i64)
  %call = call ptr @_ZN4Page8allocateEmm(ptr %1, i64 %mul, i64 8)
  %data = getelementptr inbounds %_Z6VectorIiE, ptr %0, i32 0, i32 1
  store ptr %call, ptr %data, align 8
  %field.inplace = getelementptr inbounds %_Z6VectorIiE, ptr %0, i32 0, i32 1
  %deref.recv = load ptr, ptr %field.inplace, align 8
  %mul1 = mul i64 %2, ptrtoint (ptr getelementptr (i64, ptr null, i32 1) to i64)
  %call2 = call ptr @memset(ptr %deref.recv, i64 0, i64 %mul1)
  br label %if.end

if.else:                                          ; preds = %entry
  %data3 = getelementptr inbounds %_Z6VectorIiE, ptr %0, i32 0, i32 1
  store ptr null, ptr %data3, align 8
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  %if.value = phi ptr [ %call2, %if.then ], [ null, %if.else ]
  ret void
}

define linkonce_odr void @_ZN6VectorIiEC1EPN4scaly6memory4PageEPim(ptr %0, ptr %1, ptr %2, i64 %3) {
entry:
  %length = getelementptr inbounds %_Z6VectorIiE, ptr %0, i32 0, i32 0
  store i64 %3, ptr %length, align 4
  %gt = icmp ugt i64 %3, 0
  br i1 %gt, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %mul = mul i64 %3, ptrtoint (ptr getelementptr (i64, ptr null, i32 1) to i64)
  %call = call ptr @_ZN4Page8allocateEmm(ptr %1, i64 %mul, i64 8)
  %data = getelementptr inbounds %_Z6VectorIiE, ptr %0, i32 0, i32 1
  store ptr %call, ptr %data, align 8
  %field.inplace = getelementptr inbounds %_Z6VectorIiE, ptr %0, i32 0, i32 1
  %deref.recv = load ptr, ptr %field.inplace, align 8
  %mul1 = mul i64 %3, ptrtoint (ptr getelementptr (i64, ptr null, i32 1) to i64)
  %call2 = call ptr @memcpy(ptr %deref.recv, ptr %2, i64 %mul1)
  br label %if.end

if.else:                                          ; preds = %entry
  %data3 = getelementptr inbounds %_Z6VectorIiE, ptr %0, i32 0, i32 1
  store ptr null, ptr %data3, align 8
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  %if.value = phi ptr [ %call2, %if.then ], [ null, %if.else ]
  ret void
}

define linkonce_odr void @_ZN6VectorIiEC1EPN4scaly6memory4PageE6VectorIiE(ptr %0, ptr %1, ptr %2) {
entry:
  %load.struct = load %_Z6VectorIiE, ptr %2, align 8
  %length = extractvalue %_Z6VectorIiE %load.struct, 0
  %length1 = getelementptr inbounds %_Z6VectorIiE, ptr %0, i32 0, i32 0
  store i64 %length, ptr %length1, align 4
  %load.struct2 = load %_Z6VectorIiE, ptr %0, align 8
  %length3 = extractvalue %_Z6VectorIiE %load.struct2, 0
  %gt = icmp ugt i64 %length3, 0
  br i1 %gt, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %load.struct4 = load %_Z6VectorIiE, ptr %0, align 8
  %length5 = extractvalue %_Z6VectorIiE %load.struct4, 0
  %mul = mul i64 %length5, ptrtoint (ptr getelementptr (i64, ptr null, i32 1) to i64)
  %call = call ptr @_ZN4Page8allocateEmm(ptr %1, i64 %mul, i64 8)
  %data = getelementptr inbounds %_Z6VectorIiE, ptr %0, i32 0, i32 1
  store ptr %call, ptr %data, align 8
  %field.inplace = getelementptr inbounds %_Z6VectorIiE, ptr %0, i32 0, i32 1
  %deref.recv = load ptr, ptr %field.inplace, align 8
  %field.inplace6 = getelementptr inbounds %_Z6VectorIiE, ptr %2, i32 0, i32 1
  %deref.recv7 = load ptr, ptr %field.inplace6, align 8
  %load.struct8 = load %_Z6VectorIiE, ptr %0, align 8
  %length9 = extractvalue %_Z6VectorIiE %load.struct8, 0
  %mul10 = mul i64 %length9, ptrtoint (ptr getelementptr (i64, ptr null, i32 1) to i64)
  %call11 = call ptr @memcpy(ptr %deref.recv, ptr %deref.recv7, i64 %mul10)
  br label %if.end

if.else:                                          ; preds = %entry
  %data12 = getelementptr inbounds %_Z6VectorIiE, ptr %0, i32 0, i32 1
  store ptr null, ptr %data12, align 8
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  %if.value = phi ptr [ %call11, %if.then ], [ null, %if.else ]
  ret void
}

define linkonce_odr ptr @_ZN5ArrayIiE10get_bufferEv(ptr %0) {
entry:
  %load.struct = load %_Z5ArrayIiE, ptr %0, align 8
  %vector = extractvalue %_Z5ArrayIiE %load.struct, 1
  %eq = icmp eq ptr %vector, null
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %load.struct1 = load %_Z5ArrayIiE, ptr %0, align 8
  %vector2 = extractvalue %_Z5ArrayIiE %load.struct1, 1
  %deref = load %_Z6VectorIiE, ptr %vector2, align 8
  %data = extractvalue %_Z6VectorIiE %deref, 1
  ret ptr %data
}

define linkonce_odr i64 @_ZN5ArrayIiE10get_lengthEv(ptr %0) {
entry:
  %load.struct = load %_Z5ArrayIiE, ptr %0, align 8
  %length = extractvalue %_Z5ArrayIiE %load.struct, 0
  ret i64 %length
}

define linkonce_odr i64 @_ZN5ArrayIiE12get_capacityEv(ptr %0) {
entry:
  %load.struct = load %_Z5ArrayIiE, ptr %0, align 8
  %vector = extractvalue %_Z5ArrayIiE %load.struct, 1
  %eq = icmp eq ptr %vector, null
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret i64 0

if.end:                                           ; preds = %entry
  %load.struct1 = load %_Z5ArrayIiE, ptr %0, align 8
  %vector2 = extractvalue %_Z5ArrayIiE %load.struct1, 1
  %deref = load %_Z6VectorIiE, ptr %vector2, align 8
  %length = extractvalue %_Z6VectorIiE %deref, 0
  ret i64 %length
}

define linkonce_odr void @_ZN5ArrayIiE10reallocateEv(ptr %0) {
entry:
  %call = call ptr @_ZN4Page3getEPv(ptr %0)
  %length = alloca i64, align 8
  store i64 0, ptr %length, align 1
  %load.struct = load %_Z5ArrayIiE, ptr %0, align 8
  %vector = extractvalue %_Z5ArrayIiE %load.struct, 1
  %eq = icmp eq ptr %vector, null
  br i1 %eq, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %call1 = call ptr @_ZN4Page23allocate_exclusive_pageEv(ptr %call)
  %call2 = call i64 @_ZN4Page12get_capacityEm(ptr %call1, i64 8)
  %sub = sub i64 %call2, ptrtoint (ptr getelementptr (%_Z6VectorIiE, ptr null, i32 1) to i64)
  %udiv = udiv i64 %sub, ptrtoint (ptr getelementptr (i64, ptr null, i32 1) to i64)
  store i64 %udiv, ptr %length, align 1
  %struct.region = call ptr @_ZN4Page8allocateEmm(ptr %call1, i64 ptrtoint (ptr getelementptr (%_Z6VectorIiE, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr ({ i1, %_Z6VectorIiE }, ptr null, i64 0, i32 1) to i64))
  %length3 = load i64, ptr %length, align 4
  call void @_ZN6VectorIiEC1EPN4scaly6memory4PageEm(ptr %struct.region, ptr %call1, i64 %length3)
  %vector4 = getelementptr inbounds %_Z5ArrayIiE, ptr %0, i32 0, i32 1
  store ptr %struct.region, ptr %vector4, align 8
  br label %if.end

if.else:                                          ; preds = %entry
  %load.struct5 = load %_Z5ArrayIiE, ptr %0, align 8
  %vector6 = extractvalue %_Z5ArrayIiE %load.struct5, 1
  %deref = load %_Z6VectorIiE, ptr %vector6, align 8
  %length7 = extractvalue %_Z6VectorIiE %deref, 0
  %mul = mul i64 %length7, 2
  store i64 %mul, ptr %length, align 1
  %call8 = call ptr @_ZN4Page23allocate_exclusive_pageEv(ptr %call)
  %struct.region9 = call ptr @_ZN4Page8allocateEmm(ptr %call8, i64 ptrtoint (ptr getelementptr (%_Z6VectorIiE, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr ({ i1, %_Z6VectorIiE }, ptr null, i64 0, i32 1) to i64))
  %length10 = load i64, ptr %length, align 4
  call void @_ZN6VectorIiEC1EPN4scaly6memory4PageEm(ptr %struct.region9, ptr %call8, i64 %length10)
  %load.struct11 = load %_Z5ArrayIiE, ptr %0, align 8
  %vector12 = extractvalue %_Z5ArrayIiE %load.struct11, 1
  %deref13 = load %_Z6VectorIiE, ptr %vector12, align 8
  %length14 = extractvalue %_Z6VectorIiE %deref13, 0
  %mul15 = mul i64 %length14, ptrtoint (ptr getelementptr (i64, ptr null, i32 1) to i64)
  %field.inplace = getelementptr inbounds %_Z6VectorIiE, ptr %struct.region9, i32 0, i32 1
  %deref.recv = load ptr, ptr %field.inplace, align 8
  %field.inplace16 = getelementptr inbounds %_Z5ArrayIiE, ptr %0, i32 0, i32 1
  %load.struct17 = load %_Z5ArrayIiE, ptr %0, align 8
  %vector18 = extractvalue %_Z5ArrayIiE %load.struct17, 1
  %deref19 = load %_Z6VectorIiE, ptr %vector18, align 8
  %data = extractvalue %_Z6VectorIiE %deref19, 1
  %call20 = call ptr @memcpy(ptr %deref.recv, ptr %data, i64 %mul15)
  %load.struct21 = load %_Z5ArrayIiE, ptr %0, align 8
  %vector22 = extractvalue %_Z5ArrayIiE %load.struct21, 1
  %call23 = call ptr @_ZN4Page3getEPv(ptr %vector22)
  call void @_ZN4Page25deallocate_exclusive_pageEP4Page(ptr %call, ptr %call23)
  %vector24 = getelementptr inbounds %_Z5ArrayIiE, ptr %0, i32 0, i32 1
  store ptr %struct.region9, ptr %vector24, align 8
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  ret void
}

define linkonce_odr void @_ZN5ArrayIiE3addEi(ptr %0, i64 %1) {
entry:
  %load.struct = load %_Z5ArrayIiE, ptr %0, align 8
  %vector = extractvalue %_Z5ArrayIiE %load.struct, 1
  %eq = icmp eq ptr %vector, null
  br i1 %eq, label %lor.end, label %lor.rhs

lor.rhs:                                          ; preds = %entry
  %load.struct1 = load %_Z5ArrayIiE, ptr %0, align 8
  %length = extractvalue %_Z5ArrayIiE %load.struct1, 0
  %load.struct2 = load %_Z5ArrayIiE, ptr %0, align 8
  %vector3 = extractvalue %_Z5ArrayIiE %load.struct2, 1
  %deref = load %_Z6VectorIiE, ptr %vector3, align 8
  %length4 = extractvalue %_Z6VectorIiE %deref, 0
  %eq5 = icmp eq i64 %length, %length4
  br label %lor.end

lor.end:                                          ; preds = %lor.rhs, %entry
  %lor.result = phi i1 [ true, %entry ], [ %eq5, %lor.rhs ]
  br i1 %lor.result, label %if.then, label %if.end

if.then:                                          ; preds = %lor.end
  call void @_ZN5ArrayIiE10reallocateEv(ptr %0)
  br label %if.end

if.end:                                           ; preds = %if.then, %lor.end
  %load.struct6 = load %_Z5ArrayIiE, ptr %0, align 8
  %vector7 = extractvalue %_Z5ArrayIiE %load.struct6, 1
  %deref8 = load %_Z6VectorIiE, ptr %vector7, align 8
  %data = extractvalue %_Z6VectorIiE %deref8, 1
  %load.struct9 = load %_Z5ArrayIiE, ptr %0, align 8
  %length10 = extractvalue %_Z5ArrayIiE %load.struct9, 0
  %ptr.add = getelementptr inbounds i64, ptr %data, i64 %length10
  store i64 %1, ptr %ptr.add, align 4
  %load.struct11 = load %_Z5ArrayIiE, ptr %0, align 8
  %length12 = extractvalue %_Z5ArrayIiE %load.struct11, 0
  %add = add i64 %length12, 1
  %length13 = getelementptr inbounds %_Z5ArrayIiE, ptr %0, i32 0, i32 0
  store i64 %add, ptr %length13, align 4
  ret void
}

define linkonce_odr void @_ZN5ArrayIiE3addE6VectorIiE(ptr %0, ptr %1) {
entry:
  %own_page = alloca ptr, align 8
  %load.struct = load %_Z5ArrayIiE, ptr %0, align 8
  %length = extractvalue %_Z5ArrayIiE %load.struct, 0
  %load.struct1 = load %_Z6VectorIiE, ptr %1, align 8
  %length2 = extractvalue %_Z6VectorIiE %load.struct1, 0
  %add = add i64 %length, %length2
  %new_length = alloca i64, align 8
  store i64 %add, ptr %new_length, align 1
  %new_length3 = load i64, ptr %new_length, align 4
  %load.struct4 = load %_Z5ArrayIiE, ptr %0, align 8
  %length5 = extractvalue %_Z5ArrayIiE %load.struct4, 0
  %lt = icmp ult i64 %new_length3, %length5
  br i1 %lt, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  call void @exit(i64 14)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %load.struct6 = load %_Z5ArrayIiE, ptr %0, align 8
  %vector = extractvalue %_Z5ArrayIiE %load.struct6, 1
  %eq = icmp eq ptr %vector, null
  br i1 %eq, label %lor.end, label %lor.rhs

lor.rhs:                                          ; preds = %if.end
  %new_length7 = load i64, ptr %new_length, align 4
  %load.struct8 = load %_Z5ArrayIiE, ptr %0, align 8
  %vector9 = extractvalue %_Z5ArrayIiE %load.struct8, 1
  %deref = load %_Z6VectorIiE, ptr %vector9, align 8
  %length10 = extractvalue %_Z6VectorIiE %deref, 0
  %gt = icmp ugt i64 %new_length7, %length10
  br label %lor.end

lor.end:                                          ; preds = %lor.rhs, %if.end
  %lor.result = phi i1 [ true, %if.end ], [ %gt, %lor.rhs ]
  br i1 %lor.result, label %if.then11, label %if.end12

if.then11:                                        ; preds = %lor.end
  call void @_ZN5ArrayIiE10reallocateEv(ptr %0)
  br label %if.end12

if.end12:                                         ; preds = %if.then11, %lor.end
  %new_length13 = load i64, ptr %new_length, align 4
  %load.struct14 = load %_Z5ArrayIiE, ptr %0, align 8
  %vector15 = extractvalue %_Z5ArrayIiE %load.struct14, 1
  %deref16 = load %_Z6VectorIiE, ptr %vector15, align 8
  %length17 = extractvalue %_Z6VectorIiE %deref16, 0
  %gt18 = icmp ugt i64 %new_length13, %length17
  br i1 %gt18, label %if.then19, label %if.end20

if.then19:                                        ; preds = %if.end12
  %call = call ptr @_ZN4Page3getEPv(ptr %0)
  store ptr %call, ptr %own_page, align 1
  %own_page21 = load ptr, ptr %own_page, align 8
  %new_length22 = load i64, ptr %new_length, align 4
  %mul = mul i64 %new_length22, ptrtoint (ptr getelementptr (i64, ptr null, i32 1) to i64)
  %call23 = call ptr @_ZN4Page8allocateEmm(ptr %own_page21, i64 %mul, i64 8)
  %load.struct24 = load %_Z5ArrayIiE, ptr %0, align 8
  %length25 = extractvalue %_Z5ArrayIiE %load.struct24, 0
  %mul26 = mul i64 %length25, ptrtoint (ptr getelementptr (i64, ptr null, i32 1) to i64)
  %load.struct27 = load %_Z5ArrayIiE, ptr %0, align 8
  %length28 = extractvalue %_Z5ArrayIiE %load.struct27, 0
  %gt29 = icmp ugt i64 %length28, 0
  br i1 %gt29, label %if.then30, label %if.end31

if.end20:                                         ; preds = %if.end31, %if.end12
  %load.struct48 = load %_Z6VectorIiE, ptr %1, align 8
  %length49 = extractvalue %_Z6VectorIiE %load.struct48, 0
  %gt50 = icmp ugt i64 %length49, 0
  br i1 %gt50, label %if.then51, label %if.end52

if.then30:                                        ; preds = %if.then19
  %field.inplace = getelementptr inbounds %_Z5ArrayIiE, ptr %0, i32 0, i32 1
  %load.struct32 = load %_Z5ArrayIiE, ptr %0, align 8
  %vector33 = extractvalue %_Z5ArrayIiE %load.struct32, 1
  %deref34 = load %_Z6VectorIiE, ptr %vector33, align 8
  %data = extractvalue %_Z6VectorIiE %deref34, 1
  %call35 = call ptr @memcpy(ptr %call23, ptr %data, i64 %mul26)
  br label %if.end31

if.end31:                                         ; preds = %if.then30, %if.then19
  %load.struct36 = load %_Z5ArrayIiE, ptr %0, align 8
  %vector37 = extractvalue %_Z5ArrayIiE %load.struct36, 1
  %deref38 = load %_Z6VectorIiE, ptr %vector37, align 8
  %data39 = extractvalue %_Z6VectorIiE %deref38, 1
  %call40 = call ptr @_ZN4Page3getEPv(ptr %data39)
  %own_page41 = load ptr, ptr %own_page, align 8
  call void @_ZN4Page25deallocate_exclusive_pageEP4Page(ptr %own_page41, ptr %call40)
  %vector42 = getelementptr inbounds %_Z5ArrayIiE, ptr %0, i32 0, i32 1
  %field.deref = load ptr, ptr %vector42, align 8
  %data43 = getelementptr inbounds %_Z6VectorIiE, ptr %field.deref, i32 0, i32 1
  store ptr %call23, ptr %data43, align 8
  %new_length44 = load i64, ptr %new_length, align 4
  %vector45 = getelementptr inbounds %_Z5ArrayIiE, ptr %0, i32 0, i32 1
  %field.deref46 = load ptr, ptr %vector45, align 8
  %length47 = getelementptr inbounds %_Z6VectorIiE, ptr %field.deref46, i32 0, i32 0
  store i64 %new_length44, ptr %length47, align 4
  br label %if.end20

if.then51:                                        ; preds = %if.end20
  %load.struct53 = load %_Z5ArrayIiE, ptr %0, align 8
  %vector54 = extractvalue %_Z5ArrayIiE %load.struct53, 1
  %deref55 = load %_Z6VectorIiE, ptr %vector54, align 8
  %data56 = extractvalue %_Z6VectorIiE %deref55, 1
  %load.struct57 = load %_Z5ArrayIiE, ptr %0, align 8
  %length58 = extractvalue %_Z5ArrayIiE %load.struct57, 0
  %mul59 = mul i64 %length58, ptrtoint (ptr getelementptr (i64, ptr null, i32 1) to i64)
  %ptr.add = getelementptr inbounds i64, ptr %data56, i64 %mul59
  %field.inplace60 = getelementptr inbounds %_Z6VectorIiE, ptr %1, i32 0, i32 1
  %deref.recv = load ptr, ptr %field.inplace60, align 8
  %load.struct61 = load %_Z6VectorIiE, ptr %1, align 8
  %length62 = extractvalue %_Z6VectorIiE %load.struct61, 0
  %mul63 = mul i64 %length62, ptrtoint (ptr getelementptr (i64, ptr null, i32 1) to i64)
  %call64 = call ptr @memcpy(ptr %ptr.add, ptr %deref.recv, i64 %mul63)
  br label %if.end52

if.end52:                                         ; preds = %if.then51, %if.end20
  %load.struct65 = load %_Z5ArrayIiE, ptr %0, align 8
  %length66 = extractvalue %_Z5ArrayIiE %load.struct65, 0
  %load.struct67 = load %_Z6VectorIiE, ptr %1, align 8
  %length68 = extractvalue %_Z6VectorIiE %load.struct67, 0
  %add69 = add i64 %length66, %length68
  %length70 = getelementptr inbounds %_Z5ArrayIiE, ptr %0, i32 0, i32 0
  store i64 %add69, ptr %length70, align 4
  ret void
}

define linkonce_odr ptr @_ZN5ArrayIiE3getEPN4scaly6memory4PageEm(ptr %0, ptr %1, i64 %2) {
entry:
  %load.struct = load %_Z5ArrayIiE, ptr %1, align 8
  %length = extractvalue %_Z5ArrayIiE %load.struct, 0
  %ge = icmp uge i64 %2, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %load.struct1 = load %_Z5ArrayIiE, ptr %1, align 8
  %vector = extractvalue %_Z5ArrayIiE %load.struct1, 1
  %deref = load %_Z6VectorIiE, ptr %vector, align 8
  %data = extractvalue %_Z6VectorIiE %deref, 1
  %ptr.add = getelementptr inbounds i64, ptr %data, i64 %2
  ret ptr %ptr.add
}

define linkonce_odr void @_ZN5ArrayIiE3putEmi(ptr %0, i64 %1, i64 %2) {
entry:
  %load.struct = load %_Z5ArrayIiE, ptr %0, align 8
  %length = extractvalue %_Z5ArrayIiE %load.struct, 0
  %ge = icmp uge i64 %1, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  call void @_Z11scaly_eputsP10const_char(ptr @.str.10)
  call void @_Z12scaly_eputnlv()
  call void @exit(i64 15)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %load.struct1 = load %_Z5ArrayIiE, ptr %0, align 8
  %vector = extractvalue %_Z5ArrayIiE %load.struct1, 1
  %deref = load %_Z6VectorIiE, ptr %vector, align 8
  %data = extractvalue %_Z6VectorIiE %deref, 1
  %ptr.add = getelementptr inbounds i64, ptr %data, i64 %1
  store i64 %2, ptr %ptr.add, align 4
  ret void
}

define linkonce_odr ptr @_ZN13ArrayIteratorIiE4nextEv(ptr %0) {
entry:
  %load.struct = load %_Z13ArrayIteratorIiE, ptr %0, align 8
  %array = extractvalue %_Z13ArrayIteratorIiE %load.struct, 0
  %eq = icmp eq ptr %array, null
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %load.struct1 = load %_Z13ArrayIteratorIiE, ptr %0, align 8
  %position = extractvalue %_Z13ArrayIteratorIiE %load.struct1, 1
  %load.struct2 = load %_Z13ArrayIteratorIiE, ptr %0, align 8
  %array3 = extractvalue %_Z13ArrayIteratorIiE %load.struct2, 0
  %deref = load %_Z5ArrayIiE, ptr %array3, align 8
  %length = extractvalue %_Z5ArrayIiE %deref, 0
  %eq4 = icmp eq i64 %position, %length
  br i1 %eq4, label %if.then5, label %if.end6

if.then5:                                         ; preds = %if.end
  ret ptr null

if.end6:                                          ; preds = %if.end
  %load.struct7 = load %_Z13ArrayIteratorIiE, ptr %0, align 8
  %position8 = extractvalue %_Z13ArrayIteratorIiE %load.struct7, 1
  %add = add i64 %position8, 1
  %position9 = getelementptr inbounds %_Z13ArrayIteratorIiE, ptr %0, i32 0, i32 1
  store i64 %add, ptr %position9, align 4
  %field.inplace = getelementptr inbounds %_Z13ArrayIteratorIiE, ptr %0, i32 0, i32 0
  %deref.recv = load ptr, ptr %field.inplace, align 8
  %call = call ptr @_ZN5ArrayIiE10get_bufferEv(ptr %deref.recv)
  %load.struct10 = load %_Z13ArrayIteratorIiE, ptr %0, align 8
  %position11 = extractvalue %_Z13ArrayIteratorIiE %load.struct10, 1
  %sub = sub i64 %position11, 1
  %ptr.add = getelementptr inbounds i64, ptr %call, i64 %sub
  ret ptr %ptr.add
}

define linkonce_odr void @_ZN13ArrayIteratorIiEC1EP5ArrayIiE(ptr %0, ptr %1) {
entry:
  %array = getelementptr inbounds %_Z13ArrayIteratorIiE, ptr %0, i32 0, i32 0
  store ptr %1, ptr %array, align 8
  %position = getelementptr inbounds %_Z13ArrayIteratorIiE, ptr %0, i32 0, i32 1
  store i64 0, ptr %position, align 4
  ret void
}

define linkonce_odr void @_ZN13ArrayIteratorIiEC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr void @_ZN5ArrayIiE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z13ArrayIteratorIiE) %0, ptr %1, ptr %2) {
entry:
  %struct.init = alloca %_Z13ArrayIteratorIiE, align 8
  call void @_ZN13ArrayIteratorIiEC1EP5ArrayIiE(ptr %struct.init, ptr %2)
  %sret.body = load %_Z13ArrayIteratorIiE, ptr %struct.init, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %struct.init, i64 ptrtoint (ptr getelementptr (%_Z13ArrayIteratorIiE, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN5ArrayIiEC1Ev(ptr %0) {
entry:
  %length = getelementptr inbounds %_Z5ArrayIiE, ptr %0, i32 0, i32 0
  store i64 0, ptr %length, align 4
  %vector = getelementptr inbounds %_Z5ArrayIiE, ptr %0, i32 0, i32 1
  store ptr null, ptr %vector, align 8
  ret void
}

define linkonce_odr void @_ZN5ArrayIiEC1Em(ptr %0, i64 %1) {
entry:
  %length = getelementptr inbounds %_Z5ArrayIiE, ptr %0, i32 0, i32 0
  store i64 0, ptr %length, align 4
  %vector = getelementptr inbounds %_Z5ArrayIiE, ptr %0, i32 0, i32 1
  store ptr null, ptr %vector, align 8
  %call = call ptr @_ZN4Page3getEPv(ptr %0)
  %call1 = call ptr @_ZN4Page23allocate_exclusive_pageEv(ptr %call)
  %struct.region = call ptr @_ZN4Page8allocateEmm(ptr %call1, i64 ptrtoint (ptr getelementptr (%_Z6VectorIiE, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr ({ i1, %_Z6VectorIiE }, ptr null, i64 0, i32 1) to i64))
  call void @_ZN6VectorIiEC1EPN4scaly6memory4PageEm(ptr %struct.region, ptr %call1, i64 %1)
  %vector2 = getelementptr inbounds %_Z5ArrayIiE, ptr %0, i32 0, i32 1
  store ptr %struct.region, ptr %vector2, align 8
  ret void
}

define linkonce_odr void @_ZN5ArrayIiEC1EPN4scaly6memory4PageE(ptr %0, ptr %1) {
entry:
  %length = getelementptr inbounds %_Z5ArrayIiE, ptr %0, i32 0, i32 0
  store i64 0, ptr %length, align 4
  %vector = getelementptr inbounds %_Z5ArrayIiE, ptr %0, i32 0, i32 1
  store ptr null, ptr %vector, align 8
  ret void
}

define linkonce_odr void @_ZN5ArrayIiEC1EPN4scaly6memory4PageEm(ptr %0, ptr %1, i64 %2) {
entry:
  %length = getelementptr inbounds %_Z5ArrayIiE, ptr %0, i32 0, i32 0
  store i64 0, ptr %length, align 4
  %vector = getelementptr inbounds %_Z5ArrayIiE, ptr %0, i32 0, i32 1
  store ptr null, ptr %vector, align 8
  %call = call ptr @_ZN4Page23allocate_exclusive_pageEv(ptr %1)
  %struct.region = call ptr @_ZN4Page8allocateEmm(ptr %call, i64 ptrtoint (ptr getelementptr (%_Z6VectorIiE, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr ({ i1, %_Z6VectorIiE }, ptr null, i64 0, i32 1) to i64))
  call void @_ZN6VectorIiEC1EPN4scaly6memory4PageEm(ptr %struct.region, ptr %call, i64 %2)
  %vector1 = getelementptr inbounds %_Z5ArrayIiE, ptr %0, i32 0, i32 1
  store ptr %struct.region, ptr %vector1, align 8
  ret void
}

define linkonce_odr void @_ZN6VectorIiEC1EPN4scaly6memory4PageE5ArrayIiE(ptr %0, ptr %1, ptr %2) {
entry:
  %load.struct = load %_Z5ArrayIiE, ptr %2, align 8
  %length = extractvalue %_Z5ArrayIiE %load.struct, 0
  %length1 = getelementptr inbounds %_Z6VectorIiE, ptr %0, i32 0, i32 0
  store i64 %length, ptr %length1, align 4
  %load.struct2 = load %_Z6VectorIiE, ptr %0, align 8
  %length3 = extractvalue %_Z6VectorIiE %load.struct2, 0
  %gt = icmp ugt i64 %length3, 0
  br i1 %gt, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %load.struct4 = load %_Z6VectorIiE, ptr %0, align 8
  %length5 = extractvalue %_Z6VectorIiE %load.struct4, 0
  %mul = mul i64 %length5, ptrtoint (ptr getelementptr (i64, ptr null, i32 1) to i64)
  %call = call ptr @_ZN4Page8allocateEmm(ptr %1, i64 %mul, i64 8)
  %data = getelementptr inbounds %_Z6VectorIiE, ptr %0, i32 0, i32 1
  store ptr %call, ptr %data, align 8
  %load.struct6 = load %_Z5ArrayIiE, ptr %2, align 8
  %vector = extractvalue %_Z5ArrayIiE %load.struct6, 1
  %field.inplace = getelementptr inbounds %_Z6VectorIiE, ptr %0, i32 0, i32 1
  %deref.recv = load ptr, ptr %field.inplace, align 8
  %field.inplace7 = getelementptr inbounds %_Z6VectorIiE, ptr %vector, i32 0, i32 1
  %deref.recv8 = load ptr, ptr %field.inplace7, align 8
  %load.struct9 = load %_Z6VectorIiE, ptr %0, align 8
  %length10 = extractvalue %_Z6VectorIiE %load.struct9, 0
  %mul11 = mul i64 %length10, ptrtoint (ptr getelementptr (i64, ptr null, i32 1) to i64)
  %call12 = call ptr @memcpy(ptr %deref.recv, ptr %deref.recv8, i64 %mul11)
  br label %if.end

if.else:                                          ; preds = %entry
  %data13 = getelementptr inbounds %_Z6VectorIiE, ptr %0, i32 0, i32 1
  store ptr null, ptr %data13, align 8
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  %if.value = phi ptr [ %call12, %if.then ], [ null, %if.else ]
  ret void
}

define linkonce_odr ptr @_ZN4ListIiE8get_headEPN4scaly6memory4PageE(ptr %0, ptr %1) {
entry:
  %load.struct = load %_Z4ListIiE, ptr %1, align 8
  %head = extractvalue %_Z4ListIiE %load.struct, 0
  %eq = icmp eq ptr %head, null
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %addr.gep = getelementptr inbounds %_Z4ListIiE, ptr %1, i32 0, i32 0
  %addr.gep1 = getelementptr inbounds %_Z4NodeIiE, ptr %addr.gep, i32 0, i32 0
  ret ptr %addr.gep1
}

define linkonce_odr ptr @_ZN12ListIteratorIiE4nextEv(ptr %0) {
entry:
  %old_current = alloca ptr, align 8
  %load.struct = load %_Z12ListIteratorIiE, ptr %0, align 8
  %current = extractvalue %_Z12ListIteratorIiE %load.struct, 0
  %ne = icmp ne ptr %current, null
  br i1 %ne, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %load.struct1 = load %_Z12ListIteratorIiE, ptr %0, align 8
  %current2 = extractvalue %_Z12ListIteratorIiE %load.struct1, 0
  store ptr %current2, ptr %old_current, align 1
  %load.struct3 = load %_Z12ListIteratorIiE, ptr %0, align 8
  %current4 = extractvalue %_Z12ListIteratorIiE %load.struct3, 0
  %deref = load %_Z4NodeIiE, ptr %current4, align 8
  %next = extractvalue %_Z4NodeIiE %deref, 1
  %current5 = getelementptr inbounds %_Z12ListIteratorIiE, ptr %0, i32 0, i32 0
  store ptr %next, ptr %current5, align 8
  %old_current6 = load ptr, ptr %old_current, align 8
  %addr.gep = getelementptr inbounds %_Z4NodeIiE, ptr %old_current6, i32 0, i32 0
  ret ptr %addr.gep

if.else:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; No predecessors!
  ret ptr null
}

define linkonce_odr void @_ZN12ListIteratorIiEC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr i64 @_ZN4ListIiE5countEv(ptr %0) {
entry:
  %local_page = call ptr @_Z21scaly_alloc_root_pagev()
  %sret.result = alloca %_Z12ListIteratorIiE, align 8
  call void @_ZN4ListIiE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z12ListIteratorIiE) %sret.result, ptr %local_page, ptr %0)
  %list_iterator = alloca ptr, align 8
  store ptr %sret.result, ptr %list_iterator, align 1
  %i = alloca i64, align 8
  store i64 0, ptr %i, align 1
  br label %while.cond

while.cond:                                       ; preds = %while.body, %entry
  %list_iterator1 = load ptr, ptr %list_iterator, align 8
  %call = call ptr @_ZN12ListIteratorIiE4nextEv(ptr %list_iterator1)
  %ne = icmp ne ptr %call, null
  br i1 %ne, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %i2 = load i64, ptr %i, align 4
  %add = add i64 %i2, 1
  store i64 %add, ptr %i, align 1
  br label %while.cond

while.exit:                                       ; preds = %while.cond
  %i3 = load i64, ptr %i, align 4
  call void @_Z28scaly_release_root_page_fullP4Page(ptr %local_page)
  ret i64 %i3
}

define linkonce_odr i1 @_ZN4ListIiE6removeEi(ptr %0, i64 %1) {
entry:
  %load.struct = load %_Z4ListIiE, ptr %0, align 8
  %head = extractvalue %_Z4ListIiE %load.struct, 0
  %node = alloca ptr, align 8
  store ptr %head, ptr %node, align 1
  %previous_node = alloca ptr, align 8
  store ptr null, ptr %previous_node, align 1
  br label %while.cond

while.cond:                                       ; preds = %if.end, %entry
  %node1 = load ptr, ptr %node, align 8
  %ne = icmp ne ptr %node1, null
  br i1 %ne, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %node2 = load ptr, ptr %node, align 8
  %load.struct3 = load %_Z4NodeIiE, ptr %node2, align 8
  %element = extractvalue %_Z4NodeIiE %load.struct3, 0
  %eq = icmp eq i64 %element, %1
  br i1 %eq, label %if.then, label %if.end

while.exit:                                       ; preds = %while.cond
  ret i1 false

if.then:                                          ; preds = %while.body
  %previous_node4 = load ptr, ptr %previous_node, align 8
  %ne5 = icmp ne ptr %previous_node4, null
  br i1 %ne5, label %if.then6, label %if.end7

if.end:                                           ; preds = %while.body
  %node18 = load ptr, ptr %node, align 8
  store ptr %node18, ptr %previous_node, align 1
  %node19 = load ptr, ptr %node, align 8
  %load.struct20 = load %_Z4NodeIiE, ptr %node19, align 8
  %next21 = extractvalue %_Z4NodeIiE %load.struct20, 1
  store ptr %next21, ptr %node, align 1
  br label %while.cond

if.then6:                                         ; preds = %if.then
  %node8 = load ptr, ptr %node, align 8
  %load.struct9 = load %_Z4NodeIiE, ptr %node8, align 8
  %next = extractvalue %_Z4NodeIiE %load.struct9, 1
  %ptr.load = load ptr, ptr %previous_node, align 8
  %next10 = getelementptr inbounds %_Z4NodeIiE, ptr %ptr.load, i32 0, i32 1
  store ptr %next, ptr %next10, align 8
  br label %if.end7

if.end7:                                          ; preds = %if.then6, %if.then
  %node11 = load ptr, ptr %node, align 8
  %load.struct12 = load %_Z4ListIiE, ptr %0, align 8
  %head13 = extractvalue %_Z4ListIiE %load.struct12, 0
  %eq14 = icmp eq ptr %node11, %head13
  br i1 %eq14, label %if.then15, label %if.end16

if.then15:                                        ; preds = %if.end7
  %head17 = getelementptr inbounds %_Z4ListIiE, ptr %0, i32 0, i32 0
  store ptr null, ptr %head17, align 8
  br label %if.end16

if.end16:                                         ; preds = %if.then15, %if.end7
  ret i1 true
}

define linkonce_odr void @_ZN4ListIiE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z12ListIteratorIiE) %0, ptr %1, ptr %2) {
entry:
  %load.struct = load %_Z4ListIiE, ptr %2, align 8
  %head = extractvalue %_Z4ListIiE %load.struct, 0
  %tuple = alloca %_Z12ListIteratorIiE, align 8
  %tuple.field = getelementptr inbounds %_Z12ListIteratorIiE, ptr %tuple, i32 0, i32 0
  store ptr %head, ptr %tuple.field, align 1
  %tuple.val = load %_Z12ListIteratorIiE, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z12ListIteratorIiE, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN4ListIiE3addEi(ptr %0, i64 %1) {
entry:
  %load.struct = load %_Z4ListIiE, ptr %0, align 8
  %head = extractvalue %_Z4ListIiE %load.struct, 0
  %own_page = call ptr @_Z3getPv(ptr %0)
  %tuple.region = call ptr @_ZN4Page8allocateEmm(ptr %own_page, i64 ptrtoint (ptr getelementptr (%_Z4NodeIiE, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr ({ i1, %_Z4NodeIiE }, ptr null, i64 0, i32 1) to i64))
  %tuple.field = getelementptr inbounds %_Z4NodeIiE, ptr %tuple.region, i32 0, i32 0
  store i64 %1, ptr %tuple.field, align 1
  %tuple.field1 = getelementptr inbounds %_Z4NodeIiE, ptr %tuple.region, i32 0, i32 1
  store ptr %head, ptr %tuple.field1, align 1
  %head2 = getelementptr inbounds %_Z4ListIiE, ptr %0, i32 0, i32 0
  store ptr %tuple.region, ptr %head2, align 8
  ret void
}

define linkonce_odr void @_ZN4ListIiEC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr void @_ZN6VectorIiEC1EPN4scaly6memory4PageE4ListIiE(ptr %0, ptr %1, ptr %2) {
entry:
  %i = alloca i64, align 8
  %list_iterator = alloca ptr, align 8
  %local_page = call ptr @_Z21scaly_alloc_root_pagev()
  %sret.result = alloca %_Z12ListIteratorIiE, align 8
  %call = call i64 @_ZN4ListIiE5countEv(ptr %2)
  %length = getelementptr inbounds %_Z6VectorIiE, ptr %0, i32 0, i32 0
  store i64 %call, ptr %length, align 4
  %load.struct = load %_Z6VectorIiE, ptr %0, align 8
  %length1 = extractvalue %_Z6VectorIiE %load.struct, 0
  %gt = icmp ugt i64 %length1, 0
  br i1 %gt, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %load.struct2 = load %_Z6VectorIiE, ptr %0, align 8
  %length3 = extractvalue %_Z6VectorIiE %load.struct2, 0
  %mul = mul i64 %length3, ptrtoint (ptr getelementptr (i64, ptr null, i32 1) to i64)
  %call4 = call ptr @_ZN4Page8allocateEmm(ptr %1, i64 %mul, i64 8)
  %data = getelementptr inbounds %_Z6VectorIiE, ptr %0, i32 0, i32 1
  store ptr %call4, ptr %data, align 8
  call void @_ZN4ListIiE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z12ListIteratorIiE) %sret.result, ptr %local_page, ptr %2)
  store ptr %sret.result, ptr %list_iterator, align 1
  %load.struct5 = load %_Z6VectorIiE, ptr %0, align 8
  %length6 = extractvalue %_Z6VectorIiE %load.struct5, 0
  store i64 %length6, ptr %i, align 1
  br label %while.cond

if.else:                                          ; preds = %entry
  %data13 = getelementptr inbounds %_Z6VectorIiE, ptr %0, i32 0, i32 1
  store ptr null, ptr %data13, align 8
  br label %if.end

if.end:                                           ; preds = %if.else, %while.exit
  call void @_Z28scaly_release_root_page_fullP4Page(ptr %local_page)
  ret void

while.cond:                                       ; preds = %while.body, %if.then
  %list_iterator7 = load ptr, ptr %list_iterator, align 8
  %call8 = call ptr @_ZN12ListIteratorIiE4nextEv(ptr %list_iterator7)
  %while.tobool = icmp ne ptr %call8, null
  br i1 %while.tobool, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %i9 = load i64, ptr %i, align 4
  %sub = sub i64 %i9, 1
  store i64 %sub, ptr %i, align 1
  %deref = load i64, ptr %call8, align 4
  %load.struct10 = load %_Z6VectorIiE, ptr %0, align 8
  %data11 = extractvalue %_Z6VectorIiE %load.struct10, 1
  %i12 = load i64, ptr %i, align 4
  %ptr.add = getelementptr inbounds i64, ptr %data11, i64 %i12
  store i64 %deref, ptr %ptr.add, align 4
  br label %while.cond

while.exit:                                       ; preds = %while.cond
  br label %if.end
}

define linkonce_odr ptr @_ZN6VectorI1TE3getEPN4scaly6memory4PageEm(ptr %0, ptr %1, i64 %2) {
entry:
  ret ptr null
}

define linkonce_odr ptr @_ZN6VectorI1TE7get_ptrEm(ptr %0, i64 %1) {
entry:
  ret ptr null
}

define linkonce_odr void @_ZN6VectorI1TE3putEm1T(ptr %0, i64 %1, ptr %2) {
entry:
  ret void
}

define linkonce_odr ptr @_ZN14VectorIteratorI1TE4nextEv(ptr %0) {
entry:
  ret ptr null
}

define linkonce_odr void @_ZN14VectorIteratorI1TEC1EP6VectorI1TE(ptr %0, ptr %1) {
entry:
  ret void
}

define linkonce_odr void @_ZN14VectorIteratorI1TEC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr void @_ZN6VectorI1TE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z14VectorIteratorI1TE) %0, ptr %1, ptr %2) {
entry:
  ret void
}

define linkonce_odr void @_ZN6VectorI1TE8as_sliceEPN4scaly6memory4PageE(ptr noalias sret(%_Z5Slice) %0, ptr %1, ptr %2) {
entry:
  ret void
}

define linkonce_odr void @_ZN6VectorI1TEC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr void @_ZN6VectorI1TEC1EP1Tm(ptr %0, ptr %1, i64 %2) {
entry:
  ret void
}

define linkonce_odr void @_ZN6VectorI1TEC1EPN4scaly6memory4PageEm(ptr %0, ptr %1, i64 %2) {
entry:
  ret void
}

define linkonce_odr void @_ZN6VectorI1TEC1EPN4scaly6memory4PageEP1Tm(ptr %0, ptr %1, ptr %2, i64 %3) {
entry:
  ret void
}

define linkonce_odr void @_ZN6VectorI1TEC1EPN4scaly6memory4PageE6VectorI1TE(ptr %0, ptr %1, ptr %2) {
entry:
  ret void
}

define linkonce_odr ptr @_ZN5ArrayI1TE10get_bufferEv(ptr %0) {
entry:
  ret ptr null
}

define linkonce_odr i64 @_ZN5ArrayI1TE10get_lengthEv(ptr %0) {
entry:
  ret i64 0
}

define linkonce_odr i64 @_ZN5ArrayI1TE12get_capacityEv(ptr %0) {
entry:
  ret i64 0
}

define linkonce_odr void @_ZN5ArrayI1TE10reallocateEv(ptr %0) {
entry:
  ret void
}

define linkonce_odr void @_ZN5ArrayI1TE3addE1T(ptr %0, ptr %1) {
entry:
  ret void
}

define linkonce_odr void @_ZN5ArrayI1TE3addE6VectorI1TE(ptr %0, ptr %1) {
entry:
  ret void
}

define linkonce_odr ptr @_ZN5ArrayI1TE3getEPN4scaly6memory4PageEm(ptr %0, ptr %1, i64 %2) {
entry:
  ret ptr null
}

define linkonce_odr void @_ZN5ArrayI1TE3putEm1T(ptr %0, i64 %1, ptr %2) {
entry:
  ret void
}

define linkonce_odr ptr @_ZN13ArrayIteratorI1TE4nextEv(ptr %0) {
entry:
  ret ptr null
}

define linkonce_odr void @_ZN13ArrayIteratorI1TEC1EP5ArrayI1TE(ptr %0, ptr %1) {
entry:
  ret void
}

define linkonce_odr void @_ZN13ArrayIteratorI1TEC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr void @_ZN5ArrayI1TE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z13ArrayIteratorI1TE) %0, ptr %1, ptr %2) {
entry:
  ret void
}

define linkonce_odr void @_ZN5ArrayI1TEC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr void @_ZN5ArrayI1TEC1Em(ptr %0, i64 %1) {
entry:
  ret void
}

define linkonce_odr void @_ZN5ArrayI1TEC1EPN4scaly6memory4PageE(ptr %0, ptr %1) {
entry:
  ret void
}

define linkonce_odr void @_ZN5ArrayI1TEC1EPN4scaly6memory4PageEm(ptr %0, ptr %1, i64 %2) {
entry:
  ret void
}

define linkonce_odr void @_ZN6VectorI1TEC1EPN4scaly6memory4PageE5ArrayI1TE(ptr %0, ptr %1, ptr %2) {
entry:
  ret void
}

define linkonce_odr ptr @_ZN4ListI1TE8get_headEPN4scaly6memory4PageE(ptr %0, ptr %1) {
entry:
  ret ptr null
}

define linkonce_odr ptr @_ZN12ListIteratorI1TE4nextEv(ptr %0) {
entry:
  ret ptr null
}

define linkonce_odr void @_ZN12ListIteratorI1TEC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr i64 @_ZN4ListI1TE5countEv(ptr %0) {
entry:
  ret i64 0
}

define linkonce_odr i1 @_ZN4ListI1TE6removeE1T(ptr %0, ptr %1) {
entry:
  ret i1 false
}

define linkonce_odr void @_ZN4ListI1TE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z12ListIteratorI1TE) %0, ptr %1, ptr %2) {
entry:
  ret void
}

define linkonce_odr void @_ZN4ListI1TE3addE1T(ptr %0, ptr %1) {
entry:
  ret void
}

define linkonce_odr void @_ZN4ListI1TEC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr void @_ZN6VectorI1TEC1EPN4scaly6memory4PageE4ListI1TE(ptr %0, ptr %1, ptr %2) {
entry:
  ret void
}

define linkonce_odr i64 @_ZN6String10get_lengthEv(ptr %0) {
entry:
  %byte = alloca i8, align 1
  %index = alloca i64, align 8
  %bit_count = alloca i64, align 8
  %result = alloca i64, align 8
  %load.struct = load %_Z6String, ptr %0, align 8
  %data = extractvalue %_Z6String %load.struct, 0
  %eq = icmp eq ptr %data, null
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret i64 0

if.end:                                           ; preds = %entry
  store i64 0, ptr %result, align 1
  store i64 0, ptr %bit_count, align 1
  store i64 0, ptr %index, align 1
  br label %while.cond

while.cond:                                       ; preds = %if.end15, %if.end
  br i1 true, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %bit_count1 = load i64, ptr %bit_count, align 4
  %eq2 = icmp eq i64 %bit_count1, 63
  br i1 %eq2, label %if.then3, label %if.end4

while.exit:                                       ; preds = %if.then14, %while.cond
  %result19 = load i64, ptr %result, align 4
  ret i64 %result19

if.then3:                                         ; preds = %while.body
  call void @exit(i64 11)
  br label %if.end4

if.end4:                                          ; preds = %if.then3, %while.body
  %load.struct5 = load %_Z6String, ptr %0, align 8
  %data6 = extractvalue %_Z6String %load.struct5, 0
  %index7 = load i64, ptr %index, align 4
  %ptr.add = getelementptr inbounds i8, ptr %data6, i64 %index7
  %deref = load i8, ptr %ptr.add, align 1
  store i8 %deref, ptr %byte, align 1
  %result8 = load i64, ptr %result, align 4
  %byte9 = load i8, ptr %byte, align 1
  %and = and i8 %byte9, 127
  %as.zext = zext i8 %and to i64
  %bit_count10 = load i64, ptr %bit_count, align 4
  %shl = shl i64 %as.zext, %bit_count10
  %or = or i64 %result8, %shl
  store i64 %or, ptr %result, align 1
  %byte11 = load i8, ptr %byte, align 1
  %and12 = and i8 %byte11, -128
  %eq13 = icmp eq i8 %and12, 0
  br i1 %eq13, label %if.then14, label %if.end15

if.then14:                                        ; preds = %if.end4
  br label %while.exit

if.end15:                                         ; preds = %if.end4
  %bit_count16 = load i64, ptr %bit_count, align 4
  %add = add i64 %bit_count16, 7
  store i64 %add, ptr %bit_count, align 1
  %index17 = load i64, ptr %index, align 4
  %add18 = add i64 %index17, 1
  store i64 %add18, ptr %index, align 1
  br label %while.cond
}

define linkonce_odr i1 @_ZN6String6equalsE6String(ptr %0, ptr %1) {
entry:
  %byte = alloca i8, align 1
  %bit_count = alloca i64, align 8
  %length = alloca i64, align 8
  store i64 0, ptr %length, align 1
  %index = alloca i64, align 8
  store i64 0, ptr %index, align 1
  %load.struct = load %_Z6String, ptr %0, align 8
  %data = extractvalue %_Z6String %load.struct, 0
  %ne = icmp ne ptr %data, null
  br i1 %ne, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  store i64 0, ptr %bit_count, align 1
  br label %while.cond

if.end:                                           ; preds = %while.exit, %entry
  %length18 = load i64, ptr %length, align 4
  %call = call i64 @_ZN6String10get_lengthEv(ptr %1)
  %ne19 = icmp ne i64 %length18, %call
  br i1 %ne19, label %if.then20, label %if.end21

while.cond:                                       ; preds = %if.end14, %if.then
  br i1 true, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %bit_count1 = load i64, ptr %bit_count, align 4
  %eq = icmp eq i64 %bit_count1, 63
  br i1 %eq, label %if.then2, label %if.end3

while.exit:                                       ; preds = %if.then13, %while.cond
  br label %if.end

if.then2:                                         ; preds = %while.body
  call void @exit(i64 11)
  br label %if.end3

if.end3:                                          ; preds = %if.then2, %while.body
  %load.struct4 = load %_Z6String, ptr %0, align 8
  %data5 = extractvalue %_Z6String %load.struct4, 0
  %index6 = load i64, ptr %index, align 4
  %ptr.add = getelementptr inbounds i8, ptr %data5, i64 %index6
  %deref = load i8, ptr %ptr.add, align 1
  store i8 %deref, ptr %byte, align 1
  %length7 = load i64, ptr %length, align 4
  %byte8 = load i8, ptr %byte, align 1
  %and = and i8 %byte8, 127
  %as.zext = zext i8 %and to i64
  %bit_count9 = load i64, ptr %bit_count, align 4
  %shl = shl i64 %as.zext, %bit_count9
  %or = or i64 %length7, %shl
  store i64 %or, ptr %length, align 1
  %byte10 = load i8, ptr %byte, align 1
  %and11 = and i8 %byte10, -128
  %eq12 = icmp eq i8 %and11, 0
  br i1 %eq12, label %if.then13, label %if.end14

if.then13:                                        ; preds = %if.end3
  br label %while.exit

if.end14:                                         ; preds = %if.end3
  %bit_count15 = load i64, ptr %bit_count, align 4
  %add = add i64 %bit_count15, 7
  store i64 %add, ptr %bit_count, align 1
  %index16 = load i64, ptr %index, align 4
  %add17 = add i64 %index16, 1
  store i64 %add17, ptr %index, align 1
  br label %while.cond

if.then20:                                        ; preds = %if.end
  ret i1 false

if.end21:                                         ; preds = %if.end
  %load.struct22 = load %_Z6String, ptr %0, align 8
  %data23 = extractvalue %_Z6String %load.struct22, 0
  %eq24 = icmp eq ptr %data23, null
  br i1 %eq24, label %if.then25, label %if.end26

if.then25:                                        ; preds = %if.end21
  ret i1 true

if.end26:                                         ; preds = %if.end21
  %load.struct27 = load %_Z6String, ptr %0, align 8
  %data28 = extractvalue %_Z6String %load.struct27, 0
  %index29 = load i64, ptr %index, align 4
  %ptr.add30 = getelementptr inbounds i8, ptr %data28, i64 %index29
  %ptr.add31 = getelementptr inbounds i8, ptr %ptr.add30, i64 1
  %load.struct32 = load %_Z6String, ptr %1, align 8
  %data33 = extractvalue %_Z6String %load.struct32, 0
  %index34 = load i64, ptr %index, align 4
  %ptr.add35 = getelementptr inbounds i8, ptr %data33, i64 %index34
  %ptr.add36 = getelementptr inbounds i8, ptr %ptr.add35, i64 1
  %length37 = load i64, ptr %length, align 4
  %call38 = call i32 @memcmp(ptr %ptr.add31, ptr %ptr.add36, i64 %length37)
  %zext = zext i32 %call38 to i64
  %eq39 = icmp eq i64 %zext, 0
  ret i1 %eq39
}

define linkonce_odr i64 @_ZN13StringBuilder10get_lengthEv(ptr %0) {
entry:
  %field.inplace = getelementptr inbounds %_Z13StringBuilder, ptr %0, i32 0, i32 0
  %call = call i64 @_ZN5ArrayIcE10get_lengthEv(ptr %field.inplace)
  ret i64 %call
}

define linkonce_odr void @_ZN13StringBuilder6appendEc(ptr %0, i8 %1) {
entry:
  %field.inplace = getelementptr inbounds %_Z13StringBuilder, ptr %0, i32 0, i32 0
  call void @_ZN5ArrayIcE3addEc(ptr %field.inplace, i8 %1)
  ret void
}

define linkonce_odr void @_ZN13StringBuilder9to_stringEPN4scaly6memory4PageE(ptr noalias sret(%_Z6String) %0, ptr %1, ptr %2) {
entry:
  %struct.region = call ptr @_ZN4Page8allocateEmm(ptr %1, i64 ptrtoint (ptr getelementptr (%_Z6String, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr ({ i1, %_Z6String }, ptr null, i64 0, i32 1) to i64))
  %field.inplace = getelementptr inbounds %_Z13StringBuilder, ptr %2, i32 0, i32 0
  %call = call ptr @_ZN5ArrayIcE10get_bufferEv(ptr %field.inplace)
  %field.inplace1 = getelementptr inbounds %_Z13StringBuilder, ptr %2, i32 0, i32 0
  %call2 = call i64 @_ZN5ArrayIcE10get_lengthEv(ptr %field.inplace1)
  call void @_ZN6StringC1EPN4scaly6memory4PageEP10const_charm(ptr %struct.region, ptr %1, ptr %call, i64 %call2)
  %sret.body = load %_Z6String, ptr %struct.region, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %struct.region, i64 ptrtoint (ptr getelementptr (%_Z6String, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN13StringBuilder6appendE6String(ptr %0, ptr %1) {
entry:
  %field.inplace = getelementptr inbounds %_Z13StringBuilder, ptr %0, i32 0, i32 0
  %struct.init = alloca %_Z6VectorIcE, align 8
  %call = call ptr @_ZN6String10get_bufferEv(ptr %1)
  %call1 = call i64 @_ZN6String10get_lengthEv(ptr %1)
  call void @_ZN6VectorIcEC1EPcm(ptr %struct.init, ptr %call, i64 %call1)
  call void @_ZN5ArrayIcE3addE6VectorIcE(ptr %field.inplace, ptr %struct.init)
  ret void
}

define linkonce_odr ptr @_ZN6VectorI6StringE3getEPN4scaly6memory4PageEm(ptr %0, ptr %1, i64 %2) {
entry:
  %load.struct = load %_Z6VectorI6StringE, ptr %1, align 8
  %length = extractvalue %_Z6VectorI6StringE %load.struct, 0
  %ge = icmp uge i64 %2, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %load.struct1 = load %_Z6VectorI6StringE, ptr %1, align 8
  %data = extractvalue %_Z6VectorI6StringE %load.struct1, 1
  %ptr.add = getelementptr inbounds %_Z6String, ptr %data, i64 %2
  ret ptr %ptr.add
}

define linkonce_odr ptr @_ZN6VectorI6StringE7get_ptrEm(ptr %0, i64 %1) {
entry:
  %load.struct = load %_Z6VectorI6StringE, ptr %0, align 8
  %length = extractvalue %_Z6VectorI6StringE %load.struct, 0
  %ge = icmp uge i64 %1, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %load.struct1 = load %_Z6VectorI6StringE, ptr %0, align 8
  %data = extractvalue %_Z6VectorI6StringE %load.struct1, 1
  %ptr.add = getelementptr inbounds %_Z6String, ptr %data, i64 %1
  ret ptr %ptr.add
}

define linkonce_odr void @_ZN6VectorI6StringE3putEm6String(ptr %0, i64 %1, ptr %2) {
entry:
  %load.struct = load %_Z6VectorI6StringE, ptr %0, align 8
  %length = extractvalue %_Z6VectorI6StringE %load.struct, 0
  %ge = icmp uge i64 %1, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  call void @_Z11scaly_eputsP10const_char(ptr @.str.11)
  call void @_Z12scaly_eputnlv()
  call void @exit(i64 15)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %load.struct1 = load %_Z6VectorI6StringE, ptr %0, align 8
  %data = extractvalue %_Z6VectorI6StringE %load.struct1, 1
  %ptr.add = getelementptr inbounds %_Z6String, ptr %data, i64 %1
  %store.load = load %_Z6String, ptr %2, align 8
  store %_Z6String %store.load, ptr %ptr.add, align 8
  ret void
}

define linkonce_odr ptr @_ZN14VectorIteratorI6StringE4nextEv(ptr %0) {
entry:
  %load.struct = load %_Z14VectorIteratorI6StringE, ptr %0, align 8
  %vector = extractvalue %_Z14VectorIteratorI6StringE %load.struct, 0
  %eq = icmp eq ptr %vector, null
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %load.struct1 = load %_Z14VectorIteratorI6StringE, ptr %0, align 8
  %position = extractvalue %_Z14VectorIteratorI6StringE %load.struct1, 1
  %load.struct2 = load %_Z14VectorIteratorI6StringE, ptr %0, align 8
  %vector3 = extractvalue %_Z14VectorIteratorI6StringE %load.struct2, 0
  %deref = load %_Z6VectorI6StringE, ptr %vector3, align 8
  %length = extractvalue %_Z6VectorI6StringE %deref, 0
  %eq4 = icmp eq i64 %position, %length
  br i1 %eq4, label %if.then5, label %if.end6

if.then5:                                         ; preds = %if.end
  ret ptr null

if.end6:                                          ; preds = %if.end
  %load.struct7 = load %_Z14VectorIteratorI6StringE, ptr %0, align 8
  %position8 = extractvalue %_Z14VectorIteratorI6StringE %load.struct7, 1
  %add = add i64 %position8, 1
  %position9 = getelementptr inbounds %_Z14VectorIteratorI6StringE, ptr %0, i32 0, i32 1
  store i64 %add, ptr %position9, align 4
  %field.inplace = getelementptr inbounds %_Z14VectorIteratorI6StringE, ptr %0, i32 0, i32 0
  %deref.recv = load ptr, ptr %field.inplace, align 8
  %load.struct10 = load %_Z14VectorIteratorI6StringE, ptr %0, align 8
  %position11 = extractvalue %_Z14VectorIteratorI6StringE %load.struct10, 1
  %sub = sub i64 %position11, 1
  %call = call ptr @_ZN6VectorI6StringE7get_ptrEm(ptr %deref.recv, i64 %sub)
  ret ptr %call
}

define linkonce_odr void @_ZN14VectorIteratorI6StringEC1EP6VectorI6StringE(ptr %0, ptr %1) {
entry:
  %vector = getelementptr inbounds %_Z14VectorIteratorI6StringE, ptr %0, i32 0, i32 0
  store ptr %1, ptr %vector, align 8
  %position = getelementptr inbounds %_Z14VectorIteratorI6StringE, ptr %0, i32 0, i32 1
  store i64 0, ptr %position, align 4
  ret void
}

define linkonce_odr void @_ZN14VectorIteratorI6StringEC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr void @_ZN6VectorI6StringE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z14VectorIteratorI6StringE) %0, ptr %1, ptr %2) {
entry:
  %struct.init = alloca %_Z14VectorIteratorI6StringE, align 8
  call void @_ZN14VectorIteratorI6StringEC1EP6VectorI6StringE(ptr %struct.init, ptr %2)
  %sret.body = load %_Z14VectorIteratorI6StringE, ptr %struct.init, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %struct.init, i64 ptrtoint (ptr getelementptr (%_Z14VectorIteratorI6StringE, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN6VectorI6StringE8as_sliceEPN4scaly6memory4PageE(ptr noalias sret(%_Z5Slice) %0, ptr %1, ptr %2) {
entry:
  %load.struct = load %_Z6VectorI6StringE, ptr %2, align 8
  %data = extractvalue %_Z6VectorI6StringE %load.struct, 1
  %load.struct1 = load %_Z6VectorI6StringE, ptr %2, align 8
  %length = extractvalue %_Z6VectorI6StringE %load.struct1, 0
  %tuple = alloca { ptr, i64 }, align 8
  %tuple.field = getelementptr inbounds { ptr, i64 }, ptr %tuple, i32 0, i32 0
  store ptr %data, ptr %tuple.field, align 1
  %tuple.field2 = getelementptr inbounds { ptr, i64 }, ptr %tuple, i32 0, i32 1
  store i64 %length, ptr %tuple.field2, align 1
  %tuple.val = load { ptr, i64 }, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr ({ ptr, i64 }, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN6VectorI6StringEC1Ev(ptr %0) {
entry:
  %length = getelementptr inbounds %_Z6VectorI6StringE, ptr %0, i32 0, i32 0
  store i64 0, ptr %length, align 4
  %data = getelementptr inbounds %_Z6VectorI6StringE, ptr %0, i32 0, i32 1
  store ptr null, ptr %data, align 8
  ret void
}

define linkonce_odr void @_ZN6VectorI6StringEC1EP6Stringm(ptr %0, ptr %1, i64 %2) {
entry:
  %length = getelementptr inbounds %_Z6VectorI6StringE, ptr %0, i32 0, i32 0
  store i64 %2, ptr %length, align 4
  %data = getelementptr inbounds %_Z6VectorI6StringE, ptr %0, i32 0, i32 1
  store ptr %1, ptr %data, align 8
  ret void
}

define linkonce_odr void @_ZN6VectorI6StringEC1EPN4scaly6memory4PageEm(ptr %0, ptr %1, i64 %2) {
entry:
  %length = getelementptr inbounds %_Z6VectorI6StringE, ptr %0, i32 0, i32 0
  store i64 %2, ptr %length, align 4
  %gt = icmp ugt i64 %2, 0
  br i1 %gt, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %mul = mul i64 %2, ptrtoint (ptr getelementptr (%_Z6String, ptr null, i32 1) to i64)
  %call = call ptr @_ZN4Page8allocateEmm(ptr %1, i64 %mul, i64 8)
  %data = getelementptr inbounds %_Z6VectorI6StringE, ptr %0, i32 0, i32 1
  store ptr %call, ptr %data, align 8
  %field.inplace = getelementptr inbounds %_Z6VectorI6StringE, ptr %0, i32 0, i32 1
  %deref.recv = load ptr, ptr %field.inplace, align 8
  %mul1 = mul i64 %2, ptrtoint (ptr getelementptr (%_Z6String, ptr null, i32 1) to i64)
  %call2 = call ptr @memset(ptr %deref.recv, i64 0, i64 %mul1)
  br label %if.end

if.else:                                          ; preds = %entry
  %data3 = getelementptr inbounds %_Z6VectorI6StringE, ptr %0, i32 0, i32 1
  store ptr null, ptr %data3, align 8
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  %if.value = phi ptr [ %call2, %if.then ], [ null, %if.else ]
  ret void
}

define linkonce_odr void @_ZN6VectorI6StringEC1EPN4scaly6memory4PageEP6Stringm(ptr %0, ptr %1, ptr %2, i64 %3) {
entry:
  %length = getelementptr inbounds %_Z6VectorI6StringE, ptr %0, i32 0, i32 0
  store i64 %3, ptr %length, align 4
  %gt = icmp ugt i64 %3, 0
  br i1 %gt, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %mul = mul i64 %3, ptrtoint (ptr getelementptr (%_Z6String, ptr null, i32 1) to i64)
  %call = call ptr @_ZN4Page8allocateEmm(ptr %1, i64 %mul, i64 8)
  %data = getelementptr inbounds %_Z6VectorI6StringE, ptr %0, i32 0, i32 1
  store ptr %call, ptr %data, align 8
  %field.inplace = getelementptr inbounds %_Z6VectorI6StringE, ptr %0, i32 0, i32 1
  %deref.recv = load ptr, ptr %field.inplace, align 8
  %mul1 = mul i64 %3, ptrtoint (ptr getelementptr (%_Z6String, ptr null, i32 1) to i64)
  %call2 = call ptr @memcpy(ptr %deref.recv, ptr %2, i64 %mul1)
  br label %if.end

if.else:                                          ; preds = %entry
  %data3 = getelementptr inbounds %_Z6VectorI6StringE, ptr %0, i32 0, i32 1
  store ptr null, ptr %data3, align 8
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  %if.value = phi ptr [ %call2, %if.then ], [ null, %if.else ]
  ret void
}

define linkonce_odr void @_ZN6VectorI6StringEC1EPN4scaly6memory4PageE6VectorI6StringE(ptr %0, ptr %1, ptr %2) {
entry:
  %load.struct = load %_Z6VectorI6StringE, ptr %2, align 8
  %length = extractvalue %_Z6VectorI6StringE %load.struct, 0
  %length1 = getelementptr inbounds %_Z6VectorI6StringE, ptr %0, i32 0, i32 0
  store i64 %length, ptr %length1, align 4
  %load.struct2 = load %_Z6VectorI6StringE, ptr %0, align 8
  %length3 = extractvalue %_Z6VectorI6StringE %load.struct2, 0
  %gt = icmp ugt i64 %length3, 0
  br i1 %gt, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %load.struct4 = load %_Z6VectorI6StringE, ptr %0, align 8
  %length5 = extractvalue %_Z6VectorI6StringE %load.struct4, 0
  %mul = mul i64 %length5, ptrtoint (ptr getelementptr (%_Z6String, ptr null, i32 1) to i64)
  %call = call ptr @_ZN4Page8allocateEmm(ptr %1, i64 %mul, i64 8)
  %data = getelementptr inbounds %_Z6VectorI6StringE, ptr %0, i32 0, i32 1
  store ptr %call, ptr %data, align 8
  %field.inplace = getelementptr inbounds %_Z6VectorI6StringE, ptr %0, i32 0, i32 1
  %deref.recv = load ptr, ptr %field.inplace, align 8
  %field.inplace6 = getelementptr inbounds %_Z6VectorI6StringE, ptr %2, i32 0, i32 1
  %deref.recv7 = load ptr, ptr %field.inplace6, align 8
  %load.struct8 = load %_Z6VectorI6StringE, ptr %0, align 8
  %length9 = extractvalue %_Z6VectorI6StringE %load.struct8, 0
  %mul10 = mul i64 %length9, ptrtoint (ptr getelementptr (%_Z6String, ptr null, i32 1) to i64)
  %call11 = call ptr @memcpy(ptr %deref.recv, ptr %deref.recv7, i64 %mul10)
  br label %if.end

if.else:                                          ; preds = %entry
  %data12 = getelementptr inbounds %_Z6VectorI6StringE, ptr %0, i32 0, i32 1
  store ptr null, ptr %data12, align 8
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  %if.value = phi ptr [ %call11, %if.then ], [ null, %if.else ]
  ret void
}

define linkonce_odr void @_ZN6VectorI6StringEC1EPN4scaly6memory4PageE5ArrayI6StringE(ptr %0, ptr %1, ptr %2) {
entry:
  %load.struct = load %_Z5ArrayI6StringE, ptr %2, align 8
  %length = extractvalue %_Z5ArrayI6StringE %load.struct, 0
  %length1 = getelementptr inbounds %_Z6VectorI6StringE, ptr %0, i32 0, i32 0
  store i64 %length, ptr %length1, align 4
  %load.struct2 = load %_Z6VectorI6StringE, ptr %0, align 8
  %length3 = extractvalue %_Z6VectorI6StringE %load.struct2, 0
  %gt = icmp ugt i64 %length3, 0
  br i1 %gt, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %load.struct4 = load %_Z6VectorI6StringE, ptr %0, align 8
  %length5 = extractvalue %_Z6VectorI6StringE %load.struct4, 0
  %mul = mul i64 %length5, ptrtoint (ptr getelementptr (%_Z6String, ptr null, i32 1) to i64)
  %call = call ptr @_ZN4Page8allocateEmm(ptr %1, i64 %mul, i64 8)
  %data = getelementptr inbounds %_Z6VectorI6StringE, ptr %0, i32 0, i32 1
  store ptr %call, ptr %data, align 8
  %load.struct6 = load %_Z5ArrayI6StringE, ptr %2, align 8
  %vector = extractvalue %_Z5ArrayI6StringE %load.struct6, 1
  %field.inplace = getelementptr inbounds %_Z6VectorI6StringE, ptr %0, i32 0, i32 1
  %deref.recv = load ptr, ptr %field.inplace, align 8
  %field.inplace7 = getelementptr inbounds %_Z6VectorI6StringE, ptr %vector, i32 0, i32 1
  %deref.recv8 = load ptr, ptr %field.inplace7, align 8
  %load.struct9 = load %_Z6VectorI6StringE, ptr %0, align 8
  %length10 = extractvalue %_Z6VectorI6StringE %load.struct9, 0
  %mul11 = mul i64 %length10, ptrtoint (ptr getelementptr (%_Z6String, ptr null, i32 1) to i64)
  %call12 = call ptr @memcpy(ptr %deref.recv, ptr %deref.recv8, i64 %mul11)
  br label %if.end

if.else:                                          ; preds = %entry
  %data13 = getelementptr inbounds %_Z6VectorI6StringE, ptr %0, i32 0, i32 1
  store ptr null, ptr %data13, align 8
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  %if.value = phi ptr [ %call12, %if.then ], [ null, %if.else ]
  ret void
}

define linkonce_odr ptr @_ZN4ListI6StringE8get_headEPN4scaly6memory4PageE(ptr %0, ptr %1) {
entry:
  %load.struct = load %_Z4ListI6StringE, ptr %1, align 8
  %head = extractvalue %_Z4ListI6StringE %load.struct, 0
  %eq = icmp eq ptr %head, null
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %addr.gep = getelementptr inbounds %_Z4ListI6StringE, ptr %1, i32 0, i32 0
  %addr.gep1 = getelementptr inbounds %_Z4NodeI6StringE, ptr %addr.gep, i32 0, i32 0
  ret ptr %addr.gep1
}

define linkonce_odr ptr @_ZN12ListIteratorI6StringE4nextEv(ptr %0) {
entry:
  %old_current = alloca ptr, align 8
  %load.struct = load %_Z12ListIteratorI6StringE, ptr %0, align 8
  %current = extractvalue %_Z12ListIteratorI6StringE %load.struct, 0
  %ne = icmp ne ptr %current, null
  br i1 %ne, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %load.struct1 = load %_Z12ListIteratorI6StringE, ptr %0, align 8
  %current2 = extractvalue %_Z12ListIteratorI6StringE %load.struct1, 0
  store ptr %current2, ptr %old_current, align 1
  %load.struct3 = load %_Z12ListIteratorI6StringE, ptr %0, align 8
  %current4 = extractvalue %_Z12ListIteratorI6StringE %load.struct3, 0
  %deref = load %_Z4NodeI6StringE, ptr %current4, align 8
  %next = extractvalue %_Z4NodeI6StringE %deref, 1
  %current5 = getelementptr inbounds %_Z12ListIteratorI6StringE, ptr %0, i32 0, i32 0
  store ptr %next, ptr %current5, align 8
  %old_current6 = load ptr, ptr %old_current, align 8
  %addr.gep = getelementptr inbounds %_Z4NodeI6StringE, ptr %old_current6, i32 0, i32 0
  ret ptr %addr.gep

if.else:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; No predecessors!
  ret ptr null
}

define linkonce_odr void @_ZN12ListIteratorI6StringEC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr i64 @_ZN4ListI6StringE5countEv(ptr %0) {
entry:
  %local_page = call ptr @_Z21scaly_alloc_root_pagev()
  %sret.result = alloca %_Z12ListIteratorI6StringE, align 8
  call void @_ZN4ListI6StringE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z12ListIteratorI6StringE) %sret.result, ptr %local_page, ptr %0)
  %list_iterator = alloca ptr, align 8
  store ptr %sret.result, ptr %list_iterator, align 1
  %i = alloca i64, align 8
  store i64 0, ptr %i, align 1
  br label %while.cond

while.cond:                                       ; preds = %while.body, %entry
  %list_iterator1 = load ptr, ptr %list_iterator, align 8
  %call = call ptr @_ZN12ListIteratorI6StringE4nextEv(ptr %list_iterator1)
  %ne = icmp ne ptr %call, null
  br i1 %ne, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %i2 = load i64, ptr %i, align 4
  %add = add i64 %i2, 1
  store i64 %add, ptr %i, align 1
  br label %while.cond

while.exit:                                       ; preds = %while.cond
  %i3 = load i64, ptr %i, align 4
  call void @_Z28scaly_release_root_page_fullP4Page(ptr %local_page)
  ret i64 %i3
}

define linkonce_odr i1 @_ZN4ListI6StringE6removeE6String(ptr %0, ptr %1) {
entry:
  %load.struct = load %_Z4ListI6StringE, ptr %0, align 8
  %head = extractvalue %_Z4ListI6StringE %load.struct, 0
  %node = alloca ptr, align 8
  store ptr %head, ptr %node, align 1
  %previous_node = alloca ptr, align 8
  store ptr null, ptr %previous_node, align 1
  br label %while.cond

while.cond:                                       ; preds = %while.body, %entry
  %node1 = load ptr, ptr %node, align 8
  %ne = icmp ne ptr %node1, null
  br i1 %ne, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %node2 = load ptr, ptr %node, align 8
  %load.struct3 = load %_Z4NodeI6StringE, ptr %node2, align 8
  %element = extractvalue %_Z4NodeI6StringE %load.struct3, 0
  %node4 = load ptr, ptr %node, align 8
  store ptr %node4, ptr %previous_node, align 1
  %node5 = load ptr, ptr %node, align 8
  %load.struct6 = load %_Z4NodeI6StringE, ptr %node5, align 8
  %next = extractvalue %_Z4NodeI6StringE %load.struct6, 1
  store ptr %next, ptr %node, align 1
  br label %while.cond

while.exit:                                       ; preds = %while.cond
  ret i1 false
}

define linkonce_odr void @_ZN4ListI6StringE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z12ListIteratorI6StringE) %0, ptr %1, ptr %2) {
entry:
  %load.struct = load %_Z4ListI6StringE, ptr %2, align 8
  %head = extractvalue %_Z4ListI6StringE %load.struct, 0
  %tuple = alloca %_Z12ListIteratorI6StringE, align 8
  %tuple.field = getelementptr inbounds %_Z12ListIteratorI6StringE, ptr %tuple, i32 0, i32 0
  store ptr %head, ptr %tuple.field, align 1
  %tuple.val = load %_Z12ListIteratorI6StringE, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z12ListIteratorI6StringE, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN4ListI6StringE3addE6String(ptr %0, ptr %1) {
entry:
  %load.struct = load %_Z4ListI6StringE, ptr %0, align 8
  %head = extractvalue %_Z4ListI6StringE %load.struct, 0
  %own_page = call ptr @_Z3getPv(ptr %0)
  %tuple.region = call ptr @_ZN4Page8allocateEmm(ptr %own_page, i64 ptrtoint (ptr getelementptr (%_Z4NodeI6StringE, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr ({ i1, %_Z4NodeI6StringE }, ptr null, i64 0, i32 1) to i64))
  %field.load = load %_Z6String, ptr %1, align 8
  %tuple.field = getelementptr inbounds %_Z4NodeI6StringE, ptr %tuple.region, i32 0, i32 0
  store %_Z6String %field.load, ptr %tuple.field, align 1
  %tuple.field1 = getelementptr inbounds %_Z4NodeI6StringE, ptr %tuple.region, i32 0, i32 1
  store ptr %head, ptr %tuple.field1, align 1
  %head2 = getelementptr inbounds %_Z4ListI6StringE, ptr %0, i32 0, i32 0
  store ptr %tuple.region, ptr %head2, align 8
  ret void
}

define linkonce_odr void @_ZN4ListI6StringEC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr void @_ZN6VectorI6StringEC1EPN4scaly6memory4PageE4ListI6StringE(ptr %0, ptr %1, ptr %2) {
entry:
  %deref.tmp = alloca %_Z6String, align 8
  %i = alloca i64, align 8
  %list_iterator = alloca ptr, align 8
  %local_page = call ptr @_Z21scaly_alloc_root_pagev()
  %sret.result = alloca %_Z12ListIteratorI6StringE, align 8
  %call = call i64 @_ZN4ListI6StringE5countEv(ptr %2)
  %length = getelementptr inbounds %_Z6VectorI6StringE, ptr %0, i32 0, i32 0
  store i64 %call, ptr %length, align 4
  %load.struct = load %_Z6VectorI6StringE, ptr %0, align 8
  %length1 = extractvalue %_Z6VectorI6StringE %load.struct, 0
  %gt = icmp ugt i64 %length1, 0
  br i1 %gt, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %load.struct2 = load %_Z6VectorI6StringE, ptr %0, align 8
  %length3 = extractvalue %_Z6VectorI6StringE %load.struct2, 0
  %mul = mul i64 %length3, ptrtoint (ptr getelementptr (%_Z6String, ptr null, i32 1) to i64)
  %call4 = call ptr @_ZN4Page8allocateEmm(ptr %1, i64 %mul, i64 8)
  %data = getelementptr inbounds %_Z6VectorI6StringE, ptr %0, i32 0, i32 1
  store ptr %call4, ptr %data, align 8
  call void @_ZN4ListI6StringE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z12ListIteratorI6StringE) %sret.result, ptr %local_page, ptr %2)
  store ptr %sret.result, ptr %list_iterator, align 1
  %load.struct5 = load %_Z6VectorI6StringE, ptr %0, align 8
  %length6 = extractvalue %_Z6VectorI6StringE %load.struct5, 0
  store i64 %length6, ptr %i, align 1
  br label %while.cond

if.else:                                          ; preds = %entry
  %data13 = getelementptr inbounds %_Z6VectorI6StringE, ptr %0, i32 0, i32 1
  store ptr null, ptr %data13, align 8
  br label %if.end

if.end:                                           ; preds = %if.else, %while.exit
  call void @_Z28scaly_release_root_page_fullP4Page(ptr %local_page)
  ret void

while.cond:                                       ; preds = %while.body, %if.then
  %list_iterator7 = load ptr, ptr %list_iterator, align 8
  %call8 = call ptr @_ZN12ListIteratorI6StringE4nextEv(ptr %list_iterator7)
  %while.tobool = icmp ne ptr %call8, null
  br i1 %while.tobool, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %i9 = load i64, ptr %i, align 4
  %sub = sub i64 %i9, 1
  store i64 %sub, ptr %i, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %deref.tmp, ptr align 1 %call8, i64 ptrtoint (ptr getelementptr (%_Z6String, ptr null, i32 1) to i64), i1 false)
  %load.struct10 = load %_Z6VectorI6StringE, ptr %0, align 8
  %data11 = extractvalue %_Z6VectorI6StringE %load.struct10, 1
  %i12 = load i64, ptr %i, align 4
  %ptr.add = getelementptr inbounds %_Z6String, ptr %data11, i64 %i12
  %store.load = load %_Z6String, ptr %deref.tmp, align 8
  store %_Z6String %store.load, ptr %ptr.add, align 8
  br label %while.cond

while.exit:                                       ; preds = %while.cond
  br label %if.end
}

define linkonce_odr ptr @_ZN5ArrayI6StringE10get_bufferEv(ptr %0) {
entry:
  %load.struct = load %_Z5ArrayI6StringE, ptr %0, align 8
  %vector = extractvalue %_Z5ArrayI6StringE %load.struct, 1
  %eq = icmp eq ptr %vector, null
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %load.struct1 = load %_Z5ArrayI6StringE, ptr %0, align 8
  %vector2 = extractvalue %_Z5ArrayI6StringE %load.struct1, 1
  %deref = load %_Z6VectorI6StringE, ptr %vector2, align 8
  %data = extractvalue %_Z6VectorI6StringE %deref, 1
  ret ptr %data
}

define linkonce_odr i64 @_ZN5ArrayI6StringE10get_lengthEv(ptr %0) {
entry:
  %load.struct = load %_Z5ArrayI6StringE, ptr %0, align 8
  %length = extractvalue %_Z5ArrayI6StringE %load.struct, 0
  ret i64 %length
}

define linkonce_odr i64 @_ZN5ArrayI6StringE12get_capacityEv(ptr %0) {
entry:
  %load.struct = load %_Z5ArrayI6StringE, ptr %0, align 8
  %vector = extractvalue %_Z5ArrayI6StringE %load.struct, 1
  %eq = icmp eq ptr %vector, null
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret i64 0

if.end:                                           ; preds = %entry
  %load.struct1 = load %_Z5ArrayI6StringE, ptr %0, align 8
  %vector2 = extractvalue %_Z5ArrayI6StringE %load.struct1, 1
  %deref = load %_Z6VectorI6StringE, ptr %vector2, align 8
  %length = extractvalue %_Z6VectorI6StringE %deref, 0
  ret i64 %length
}

define linkonce_odr void @_ZN5ArrayI6StringE10reallocateEv(ptr %0) {
entry:
  %call = call ptr @_ZN4Page3getEPv(ptr %0)
  %length = alloca i64, align 8
  store i64 0, ptr %length, align 1
  %load.struct = load %_Z5ArrayI6StringE, ptr %0, align 8
  %vector = extractvalue %_Z5ArrayI6StringE %load.struct, 1
  %eq = icmp eq ptr %vector, null
  br i1 %eq, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %call1 = call ptr @_ZN4Page23allocate_exclusive_pageEv(ptr %call)
  %call2 = call i64 @_ZN4Page12get_capacityEm(ptr %call1, i64 8)
  %sub = sub i64 %call2, ptrtoint (ptr getelementptr (%_Z6VectorI6StringE, ptr null, i32 1) to i64)
  %udiv = udiv i64 %sub, ptrtoint (ptr getelementptr (%_Z6String, ptr null, i32 1) to i64)
  store i64 %udiv, ptr %length, align 1
  %struct.region = call ptr @_ZN4Page8allocateEmm(ptr %call1, i64 ptrtoint (ptr getelementptr (%_Z6VectorI6StringE, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr ({ i1, %_Z6VectorI6StringE }, ptr null, i64 0, i32 1) to i64))
  %length3 = load i64, ptr %length, align 4
  call void @_ZN6VectorI6StringEC1EPN4scaly6memory4PageEm(ptr %struct.region, ptr %call1, i64 %length3)
  %vector4 = getelementptr inbounds %_Z5ArrayI6StringE, ptr %0, i32 0, i32 1
  store ptr %struct.region, ptr %vector4, align 8
  br label %if.end

if.else:                                          ; preds = %entry
  %load.struct5 = load %_Z5ArrayI6StringE, ptr %0, align 8
  %vector6 = extractvalue %_Z5ArrayI6StringE %load.struct5, 1
  %deref = load %_Z6VectorI6StringE, ptr %vector6, align 8
  %length7 = extractvalue %_Z6VectorI6StringE %deref, 0
  %mul = mul i64 %length7, 2
  store i64 %mul, ptr %length, align 1
  %call8 = call ptr @_ZN4Page23allocate_exclusive_pageEv(ptr %call)
  %struct.region9 = call ptr @_ZN4Page8allocateEmm(ptr %call8, i64 ptrtoint (ptr getelementptr (%_Z6VectorI6StringE, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr ({ i1, %_Z6VectorI6StringE }, ptr null, i64 0, i32 1) to i64))
  %length10 = load i64, ptr %length, align 4
  call void @_ZN6VectorI6StringEC1EPN4scaly6memory4PageEm(ptr %struct.region9, ptr %call8, i64 %length10)
  %load.struct11 = load %_Z5ArrayI6StringE, ptr %0, align 8
  %vector12 = extractvalue %_Z5ArrayI6StringE %load.struct11, 1
  %deref13 = load %_Z6VectorI6StringE, ptr %vector12, align 8
  %length14 = extractvalue %_Z6VectorI6StringE %deref13, 0
  %mul15 = mul i64 %length14, ptrtoint (ptr getelementptr (%_Z6String, ptr null, i32 1) to i64)
  %field.inplace = getelementptr inbounds %_Z6VectorI6StringE, ptr %struct.region9, i32 0, i32 1
  %deref.recv = load ptr, ptr %field.inplace, align 8
  %field.inplace16 = getelementptr inbounds %_Z5ArrayI6StringE, ptr %0, i32 0, i32 1
  %load.struct17 = load %_Z5ArrayI6StringE, ptr %0, align 8
  %vector18 = extractvalue %_Z5ArrayI6StringE %load.struct17, 1
  %deref19 = load %_Z6VectorI6StringE, ptr %vector18, align 8
  %data = extractvalue %_Z6VectorI6StringE %deref19, 1
  %call20 = call ptr @memcpy(ptr %deref.recv, ptr %data, i64 %mul15)
  %load.struct21 = load %_Z5ArrayI6StringE, ptr %0, align 8
  %vector22 = extractvalue %_Z5ArrayI6StringE %load.struct21, 1
  %call23 = call ptr @_ZN4Page3getEPv(ptr %vector22)
  call void @_ZN4Page25deallocate_exclusive_pageEP4Page(ptr %call, ptr %call23)
  %vector24 = getelementptr inbounds %_Z5ArrayI6StringE, ptr %0, i32 0, i32 1
  store ptr %struct.region9, ptr %vector24, align 8
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  ret void
}

define linkonce_odr void @_ZN5ArrayI6StringE3addE6String(ptr %0, ptr %1) {
entry:
  %load.struct = load %_Z5ArrayI6StringE, ptr %0, align 8
  %vector = extractvalue %_Z5ArrayI6StringE %load.struct, 1
  %eq = icmp eq ptr %vector, null
  br i1 %eq, label %lor.end, label %lor.rhs

lor.rhs:                                          ; preds = %entry
  %load.struct1 = load %_Z5ArrayI6StringE, ptr %0, align 8
  %length = extractvalue %_Z5ArrayI6StringE %load.struct1, 0
  %load.struct2 = load %_Z5ArrayI6StringE, ptr %0, align 8
  %vector3 = extractvalue %_Z5ArrayI6StringE %load.struct2, 1
  %deref = load %_Z6VectorI6StringE, ptr %vector3, align 8
  %length4 = extractvalue %_Z6VectorI6StringE %deref, 0
  %eq5 = icmp eq i64 %length, %length4
  br label %lor.end

lor.end:                                          ; preds = %lor.rhs, %entry
  %lor.result = phi i1 [ true, %entry ], [ %eq5, %lor.rhs ]
  br i1 %lor.result, label %if.then, label %if.end

if.then:                                          ; preds = %lor.end
  call void @_ZN5ArrayI6StringE10reallocateEv(ptr %0)
  br label %if.end

if.end:                                           ; preds = %if.then, %lor.end
  %load.struct6 = load %_Z5ArrayI6StringE, ptr %0, align 8
  %vector7 = extractvalue %_Z5ArrayI6StringE %load.struct6, 1
  %deref8 = load %_Z6VectorI6StringE, ptr %vector7, align 8
  %data = extractvalue %_Z6VectorI6StringE %deref8, 1
  %load.struct9 = load %_Z5ArrayI6StringE, ptr %0, align 8
  %length10 = extractvalue %_Z5ArrayI6StringE %load.struct9, 0
  %ptr.add = getelementptr inbounds %_Z6String, ptr %data, i64 %length10
  %store.load = load %_Z6String, ptr %1, align 8
  store %_Z6String %store.load, ptr %ptr.add, align 8
  %load.struct11 = load %_Z5ArrayI6StringE, ptr %0, align 8
  %length12 = extractvalue %_Z5ArrayI6StringE %load.struct11, 0
  %add = add i64 %length12, 1
  %length13 = getelementptr inbounds %_Z5ArrayI6StringE, ptr %0, i32 0, i32 0
  store i64 %add, ptr %length13, align 4
  ret void
}

define linkonce_odr void @_ZN5ArrayI6StringE3addE6VectorI6StringE(ptr %0, ptr %1) {
entry:
  %own_page = alloca ptr, align 8
  %load.struct = load %_Z5ArrayI6StringE, ptr %0, align 8
  %length = extractvalue %_Z5ArrayI6StringE %load.struct, 0
  %load.struct1 = load %_Z6VectorI6StringE, ptr %1, align 8
  %length2 = extractvalue %_Z6VectorI6StringE %load.struct1, 0
  %add = add i64 %length, %length2
  %new_length = alloca i64, align 8
  store i64 %add, ptr %new_length, align 1
  %new_length3 = load i64, ptr %new_length, align 4
  %load.struct4 = load %_Z5ArrayI6StringE, ptr %0, align 8
  %length5 = extractvalue %_Z5ArrayI6StringE %load.struct4, 0
  %lt = icmp ult i64 %new_length3, %length5
  br i1 %lt, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  call void @exit(i64 14)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %load.struct6 = load %_Z5ArrayI6StringE, ptr %0, align 8
  %vector = extractvalue %_Z5ArrayI6StringE %load.struct6, 1
  %eq = icmp eq ptr %vector, null
  br i1 %eq, label %lor.end, label %lor.rhs

lor.rhs:                                          ; preds = %if.end
  %new_length7 = load i64, ptr %new_length, align 4
  %load.struct8 = load %_Z5ArrayI6StringE, ptr %0, align 8
  %vector9 = extractvalue %_Z5ArrayI6StringE %load.struct8, 1
  %deref = load %_Z6VectorI6StringE, ptr %vector9, align 8
  %length10 = extractvalue %_Z6VectorI6StringE %deref, 0
  %gt = icmp ugt i64 %new_length7, %length10
  br label %lor.end

lor.end:                                          ; preds = %lor.rhs, %if.end
  %lor.result = phi i1 [ true, %if.end ], [ %gt, %lor.rhs ]
  br i1 %lor.result, label %if.then11, label %if.end12

if.then11:                                        ; preds = %lor.end
  call void @_ZN5ArrayI6StringE10reallocateEv(ptr %0)
  br label %if.end12

if.end12:                                         ; preds = %if.then11, %lor.end
  %new_length13 = load i64, ptr %new_length, align 4
  %load.struct14 = load %_Z5ArrayI6StringE, ptr %0, align 8
  %vector15 = extractvalue %_Z5ArrayI6StringE %load.struct14, 1
  %deref16 = load %_Z6VectorI6StringE, ptr %vector15, align 8
  %length17 = extractvalue %_Z6VectorI6StringE %deref16, 0
  %gt18 = icmp ugt i64 %new_length13, %length17
  br i1 %gt18, label %if.then19, label %if.end20

if.then19:                                        ; preds = %if.end12
  %call = call ptr @_ZN4Page3getEPv(ptr %0)
  store ptr %call, ptr %own_page, align 1
  %own_page21 = load ptr, ptr %own_page, align 8
  %new_length22 = load i64, ptr %new_length, align 4
  %mul = mul i64 %new_length22, ptrtoint (ptr getelementptr (%_Z6String, ptr null, i32 1) to i64)
  %call23 = call ptr @_ZN4Page8allocateEmm(ptr %own_page21, i64 %mul, i64 8)
  %load.struct24 = load %_Z5ArrayI6StringE, ptr %0, align 8
  %length25 = extractvalue %_Z5ArrayI6StringE %load.struct24, 0
  %mul26 = mul i64 %length25, ptrtoint (ptr getelementptr (%_Z6String, ptr null, i32 1) to i64)
  %load.struct27 = load %_Z5ArrayI6StringE, ptr %0, align 8
  %length28 = extractvalue %_Z5ArrayI6StringE %load.struct27, 0
  %gt29 = icmp ugt i64 %length28, 0
  br i1 %gt29, label %if.then30, label %if.end31

if.end20:                                         ; preds = %if.end31, %if.end12
  %load.struct48 = load %_Z6VectorI6StringE, ptr %1, align 8
  %length49 = extractvalue %_Z6VectorI6StringE %load.struct48, 0
  %gt50 = icmp ugt i64 %length49, 0
  br i1 %gt50, label %if.then51, label %if.end52

if.then30:                                        ; preds = %if.then19
  %field.inplace = getelementptr inbounds %_Z5ArrayI6StringE, ptr %0, i32 0, i32 1
  %load.struct32 = load %_Z5ArrayI6StringE, ptr %0, align 8
  %vector33 = extractvalue %_Z5ArrayI6StringE %load.struct32, 1
  %deref34 = load %_Z6VectorI6StringE, ptr %vector33, align 8
  %data = extractvalue %_Z6VectorI6StringE %deref34, 1
  %call35 = call ptr @memcpy(ptr %call23, ptr %data, i64 %mul26)
  br label %if.end31

if.end31:                                         ; preds = %if.then30, %if.then19
  %load.struct36 = load %_Z5ArrayI6StringE, ptr %0, align 8
  %vector37 = extractvalue %_Z5ArrayI6StringE %load.struct36, 1
  %deref38 = load %_Z6VectorI6StringE, ptr %vector37, align 8
  %data39 = extractvalue %_Z6VectorI6StringE %deref38, 1
  %call40 = call ptr @_ZN4Page3getEPv(ptr %data39)
  %own_page41 = load ptr, ptr %own_page, align 8
  call void @_ZN4Page25deallocate_exclusive_pageEP4Page(ptr %own_page41, ptr %call40)
  %vector42 = getelementptr inbounds %_Z5ArrayI6StringE, ptr %0, i32 0, i32 1
  %field.deref = load ptr, ptr %vector42, align 8
  %data43 = getelementptr inbounds %_Z6VectorI6StringE, ptr %field.deref, i32 0, i32 1
  store ptr %call23, ptr %data43, align 8
  %new_length44 = load i64, ptr %new_length, align 4
  %vector45 = getelementptr inbounds %_Z5ArrayI6StringE, ptr %0, i32 0, i32 1
  %field.deref46 = load ptr, ptr %vector45, align 8
  %length47 = getelementptr inbounds %_Z6VectorI6StringE, ptr %field.deref46, i32 0, i32 0
  store i64 %new_length44, ptr %length47, align 4
  br label %if.end20

if.then51:                                        ; preds = %if.end20
  %load.struct53 = load %_Z5ArrayI6StringE, ptr %0, align 8
  %vector54 = extractvalue %_Z5ArrayI6StringE %load.struct53, 1
  %deref55 = load %_Z6VectorI6StringE, ptr %vector54, align 8
  %data56 = extractvalue %_Z6VectorI6StringE %deref55, 1
  %load.struct57 = load %_Z5ArrayI6StringE, ptr %0, align 8
  %length58 = extractvalue %_Z5ArrayI6StringE %load.struct57, 0
  %mul59 = mul i64 %length58, ptrtoint (ptr getelementptr (%_Z6String, ptr null, i32 1) to i64)
  %ptr.add = getelementptr inbounds %_Z6String, ptr %data56, i64 %mul59
  %field.inplace60 = getelementptr inbounds %_Z6VectorI6StringE, ptr %1, i32 0, i32 1
  %deref.recv = load ptr, ptr %field.inplace60, align 8
  %load.struct61 = load %_Z6VectorI6StringE, ptr %1, align 8
  %length62 = extractvalue %_Z6VectorI6StringE %load.struct61, 0
  %mul63 = mul i64 %length62, ptrtoint (ptr getelementptr (%_Z6String, ptr null, i32 1) to i64)
  %call64 = call ptr @memcpy(ptr %ptr.add, ptr %deref.recv, i64 %mul63)
  br label %if.end52

if.end52:                                         ; preds = %if.then51, %if.end20
  %load.struct65 = load %_Z5ArrayI6StringE, ptr %0, align 8
  %length66 = extractvalue %_Z5ArrayI6StringE %load.struct65, 0
  %load.struct67 = load %_Z6VectorI6StringE, ptr %1, align 8
  %length68 = extractvalue %_Z6VectorI6StringE %load.struct67, 0
  %add69 = add i64 %length66, %length68
  %length70 = getelementptr inbounds %_Z5ArrayI6StringE, ptr %0, i32 0, i32 0
  store i64 %add69, ptr %length70, align 4
  ret void
}

define linkonce_odr ptr @_ZN5ArrayI6StringE3getEPN4scaly6memory4PageEm(ptr %0, ptr %1, i64 %2) {
entry:
  %load.struct = load %_Z5ArrayI6StringE, ptr %1, align 8
  %length = extractvalue %_Z5ArrayI6StringE %load.struct, 0
  %ge = icmp uge i64 %2, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %load.struct1 = load %_Z5ArrayI6StringE, ptr %1, align 8
  %vector = extractvalue %_Z5ArrayI6StringE %load.struct1, 1
  %deref = load %_Z6VectorI6StringE, ptr %vector, align 8
  %data = extractvalue %_Z6VectorI6StringE %deref, 1
  %ptr.add = getelementptr inbounds %_Z6String, ptr %data, i64 %2
  ret ptr %ptr.add
}

define linkonce_odr void @_ZN5ArrayI6StringE3putEm6String(ptr %0, i64 %1, ptr %2) {
entry:
  %load.struct = load %_Z5ArrayI6StringE, ptr %0, align 8
  %length = extractvalue %_Z5ArrayI6StringE %load.struct, 0
  %ge = icmp uge i64 %1, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  call void @_Z11scaly_eputsP10const_char(ptr @.str.12)
  call void @_Z12scaly_eputnlv()
  call void @exit(i64 15)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %load.struct1 = load %_Z5ArrayI6StringE, ptr %0, align 8
  %vector = extractvalue %_Z5ArrayI6StringE %load.struct1, 1
  %deref = load %_Z6VectorI6StringE, ptr %vector, align 8
  %data = extractvalue %_Z6VectorI6StringE %deref, 1
  %ptr.add = getelementptr inbounds %_Z6String, ptr %data, i64 %1
  %store.load = load %_Z6String, ptr %2, align 8
  store %_Z6String %store.load, ptr %ptr.add, align 8
  ret void
}

define linkonce_odr ptr @_ZN13ArrayIteratorI6StringE4nextEv(ptr %0) {
entry:
  %load.struct = load %_Z13ArrayIteratorI6StringE, ptr %0, align 8
  %array = extractvalue %_Z13ArrayIteratorI6StringE %load.struct, 0
  %eq = icmp eq ptr %array, null
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %load.struct1 = load %_Z13ArrayIteratorI6StringE, ptr %0, align 8
  %position = extractvalue %_Z13ArrayIteratorI6StringE %load.struct1, 1
  %load.struct2 = load %_Z13ArrayIteratorI6StringE, ptr %0, align 8
  %array3 = extractvalue %_Z13ArrayIteratorI6StringE %load.struct2, 0
  %deref = load %_Z5ArrayI6StringE, ptr %array3, align 8
  %length = extractvalue %_Z5ArrayI6StringE %deref, 0
  %eq4 = icmp eq i64 %position, %length
  br i1 %eq4, label %if.then5, label %if.end6

if.then5:                                         ; preds = %if.end
  ret ptr null

if.end6:                                          ; preds = %if.end
  %load.struct7 = load %_Z13ArrayIteratorI6StringE, ptr %0, align 8
  %position8 = extractvalue %_Z13ArrayIteratorI6StringE %load.struct7, 1
  %add = add i64 %position8, 1
  %position9 = getelementptr inbounds %_Z13ArrayIteratorI6StringE, ptr %0, i32 0, i32 1
  store i64 %add, ptr %position9, align 4
  %field.inplace = getelementptr inbounds %_Z13ArrayIteratorI6StringE, ptr %0, i32 0, i32 0
  %deref.recv = load ptr, ptr %field.inplace, align 8
  %call = call ptr @_ZN5ArrayI6StringE10get_bufferEv(ptr %deref.recv)
  %load.struct10 = load %_Z13ArrayIteratorI6StringE, ptr %0, align 8
  %position11 = extractvalue %_Z13ArrayIteratorI6StringE %load.struct10, 1
  %sub = sub i64 %position11, 1
  %ptr.add = getelementptr inbounds %_Z6String, ptr %call, i64 %sub
  ret ptr %ptr.add
}

define linkonce_odr void @_ZN13ArrayIteratorI6StringEC1EP5ArrayI6StringE(ptr %0, ptr %1) {
entry:
  %array = getelementptr inbounds %_Z13ArrayIteratorI6StringE, ptr %0, i32 0, i32 0
  store ptr %1, ptr %array, align 8
  %position = getelementptr inbounds %_Z13ArrayIteratorI6StringE, ptr %0, i32 0, i32 1
  store i64 0, ptr %position, align 4
  ret void
}

define linkonce_odr void @_ZN13ArrayIteratorI6StringEC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr void @_ZN5ArrayI6StringE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z13ArrayIteratorI6StringE) %0, ptr %1, ptr %2) {
entry:
  %struct.init = alloca %_Z13ArrayIteratorI6StringE, align 8
  call void @_ZN13ArrayIteratorI6StringEC1EP5ArrayI6StringE(ptr %struct.init, ptr %2)
  %sret.body = load %_Z13ArrayIteratorI6StringE, ptr %struct.init, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %struct.init, i64 ptrtoint (ptr getelementptr (%_Z13ArrayIteratorI6StringE, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN5ArrayI6StringEC1Ev(ptr %0) {
entry:
  %length = getelementptr inbounds %_Z5ArrayI6StringE, ptr %0, i32 0, i32 0
  store i64 0, ptr %length, align 4
  %vector = getelementptr inbounds %_Z5ArrayI6StringE, ptr %0, i32 0, i32 1
  store ptr null, ptr %vector, align 8
  ret void
}

define linkonce_odr void @_ZN5ArrayI6StringEC1Em(ptr %0, i64 %1) {
entry:
  %length = getelementptr inbounds %_Z5ArrayI6StringE, ptr %0, i32 0, i32 0
  store i64 0, ptr %length, align 4
  %vector = getelementptr inbounds %_Z5ArrayI6StringE, ptr %0, i32 0, i32 1
  store ptr null, ptr %vector, align 8
  %call = call ptr @_ZN4Page3getEPv(ptr %0)
  %call1 = call ptr @_ZN4Page23allocate_exclusive_pageEv(ptr %call)
  %struct.region = call ptr @_ZN4Page8allocateEmm(ptr %call1, i64 ptrtoint (ptr getelementptr (%_Z6VectorI6StringE, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr ({ i1, %_Z6VectorI6StringE }, ptr null, i64 0, i32 1) to i64))
  call void @_ZN6VectorI6StringEC1EPN4scaly6memory4PageEm(ptr %struct.region, ptr %call1, i64 %1)
  %vector2 = getelementptr inbounds %_Z5ArrayI6StringE, ptr %0, i32 0, i32 1
  store ptr %struct.region, ptr %vector2, align 8
  ret void
}

define linkonce_odr void @_ZN5ArrayI6StringEC1EPN4scaly6memory4PageE(ptr %0, ptr %1) {
entry:
  %length = getelementptr inbounds %_Z5ArrayI6StringE, ptr %0, i32 0, i32 0
  store i64 0, ptr %length, align 4
  %vector = getelementptr inbounds %_Z5ArrayI6StringE, ptr %0, i32 0, i32 1
  store ptr null, ptr %vector, align 8
  ret void
}

define linkonce_odr void @_ZN5ArrayI6StringEC1EPN4scaly6memory4PageEm(ptr %0, ptr %1, i64 %2) {
entry:
  %length = getelementptr inbounds %_Z5ArrayI6StringE, ptr %0, i32 0, i32 0
  store i64 0, ptr %length, align 4
  %vector = getelementptr inbounds %_Z5ArrayI6StringE, ptr %0, i32 0, i32 1
  store ptr null, ptr %vector, align 8
  %call = call ptr @_ZN4Page23allocate_exclusive_pageEv(ptr %1)
  %struct.region = call ptr @_ZN4Page8allocateEmm(ptr %call, i64 ptrtoint (ptr getelementptr (%_Z6VectorI6StringE, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr ({ i1, %_Z6VectorI6StringE }, ptr null, i64 0, i32 1) to i64))
  call void @_ZN6VectorI6StringEC1EPN4scaly6memory4PageEm(ptr %struct.region, ptr %call, i64 %2)
  %vector1 = getelementptr inbounds %_Z5ArrayI6StringE, ptr %0, i32 0, i32 1
  store ptr %struct.region, ptr %vector1, align 8
  ret void
}

define linkonce_odr void @_ZN11BuilderListI6StringE3addE6String(ptr %0, ptr %1) {
entry:
  %load.struct = load %_Z11BuilderListI6StringE, ptr %0, align 8
  %head = extractvalue %_Z11BuilderListI6StringE %load.struct, 0
  %own_page = call ptr @_Z3getPv(ptr %0)
  %tuple.region = call ptr @_ZN4Page8allocateEmm(ptr %own_page, i64 ptrtoint (ptr getelementptr (%_Z4NodeI6StringE, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr ({ i1, %_Z4NodeI6StringE }, ptr null, i64 0, i32 1) to i64))
  %field.load = load %_Z6String, ptr %1, align 8
  %tuple.field = getelementptr inbounds %_Z4NodeI6StringE, ptr %tuple.region, i32 0, i32 0
  store %_Z6String %field.load, ptr %tuple.field, align 1
  %tuple.field1 = getelementptr inbounds %_Z4NodeI6StringE, ptr %tuple.region, i32 0, i32 1
  store ptr %head, ptr %tuple.field1, align 1
  %head2 = getelementptr inbounds %_Z11BuilderListI6StringE, ptr %0, i32 0, i32 0
  store ptr %tuple.region, ptr %head2, align 8
  ret void
}

define linkonce_odr i1 @_ZN11BuilderListI6StringE6removeE6String(ptr %0, ptr %1) {
entry:
  %load.struct = load %_Z11BuilderListI6StringE, ptr %0, align 8
  %head = extractvalue %_Z11BuilderListI6StringE %load.struct, 0
  %node = alloca ptr, align 8
  store ptr %head, ptr %node, align 1
  %previous_node = alloca ptr, align 8
  store ptr null, ptr %previous_node, align 1
  br label %while.cond

while.cond:                                       ; preds = %while.body, %entry
  %node1 = load ptr, ptr %node, align 8
  %ne = icmp ne ptr %node1, null
  br i1 %ne, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %node2 = load ptr, ptr %node, align 8
  %load.struct3 = load %_Z4NodeI6StringE, ptr %node2, align 8
  %element = extractvalue %_Z4NodeI6StringE %load.struct3, 0
  %node4 = load ptr, ptr %node, align 8
  store ptr %node4, ptr %previous_node, align 1
  %node5 = load ptr, ptr %node, align 8
  %load.struct6 = load %_Z4NodeI6StringE, ptr %node5, align 8
  %next = extractvalue %_Z4NodeI6StringE %load.struct6, 1
  store ptr %next, ptr %node, align 1
  br label %while.cond

while.exit:                                       ; preds = %while.cond
  ret i1 false
}

define linkonce_odr ptr @_ZN11BuilderListI6StringE8get_headEPN4scaly6memory4PageE(ptr %0, ptr %1) {
entry:
  %load.struct = load %_Z11BuilderListI6StringE, ptr %1, align 8
  %head = extractvalue %_Z11BuilderListI6StringE %load.struct, 0
  %eq = icmp eq ptr %head, null
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %addr.gep = getelementptr inbounds %_Z11BuilderListI6StringE, ptr %1, i32 0, i32 0
  %addr.gep1 = getelementptr inbounds %_Z4NodeI6StringE, ptr %addr.gep, i32 0, i32 0
  ret ptr %addr.gep1
}

define linkonce_odr ptr @_ZN19BuilderListIteratorI6StringE4nextEv(ptr %0) {
entry:
  %load.struct = load %_Z19BuilderListIteratorI6StringE, ptr %0, align 8
  %current = extractvalue %_Z19BuilderListIteratorI6StringE %load.struct, 0
  %ne = icmp ne ptr %current, null
  br i1 %ne, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %load.struct1 = load %_Z19BuilderListIteratorI6StringE, ptr %0, align 8
  %current2 = extractvalue %_Z19BuilderListIteratorI6StringE %load.struct1, 0
  %load.struct3 = load %_Z19BuilderListIteratorI6StringE, ptr %0, align 8
  %current4 = extractvalue %_Z19BuilderListIteratorI6StringE %load.struct3, 0
  %deref = load %_Z4NodeI6StringE, ptr %current4, align 8
  %next = extractvalue %_Z4NodeI6StringE %deref, 1
  %current5 = getelementptr inbounds %_Z19BuilderListIteratorI6StringE, ptr %0, i32 0, i32 0
  store ptr %next, ptr %current5, align 8
  %addr.gep = getelementptr inbounds %_Z4NodeI6StringE, ptr %current2, i32 0, i32 0
  ret ptr %addr.gep

if.else:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; No predecessors!
  ret ptr null
}

define linkonce_odr void @_ZN19BuilderListIteratorI6StringEC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr i64 @_ZN11BuilderListI6StringE5countEv(ptr %0) {
entry:
  %local_page = call ptr @_Z21scaly_alloc_root_pagev()
  %sret.result = alloca %_Z19BuilderListIteratorI6StringE, align 8
  call void @_ZN11BuilderListI6StringE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z19BuilderListIteratorI6StringE) %sret.result, ptr %local_page, ptr %0)
  %list_iterator = alloca ptr, align 8
  store ptr %sret.result, ptr %list_iterator, align 1
  %i = alloca i64, align 8
  store i64 0, ptr %i, align 1
  br label %while.cond

while.cond:                                       ; preds = %while.body, %entry
  %list_iterator1 = load ptr, ptr %list_iterator, align 8
  %call = call ptr @_ZN19BuilderListIteratorI6StringE4nextEv(ptr %list_iterator1)
  %ne = icmp ne ptr %call, null
  br i1 %ne, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %i2 = load i64, ptr %i, align 4
  %add = add i64 %i2, 1
  store i64 %add, ptr %i, align 1
  br label %while.cond

while.exit:                                       ; preds = %while.cond
  %i3 = load i64, ptr %i, align 4
  call void @_Z28scaly_release_root_page_fullP4Page(ptr %local_page)
  ret i64 %i3
}

define linkonce_odr void @_ZN11BuilderListI6StringE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z19BuilderListIteratorI6StringE) %0, ptr %1, ptr %2) {
entry:
  %load.struct = load %_Z11BuilderListI6StringE, ptr %2, align 8
  %head = extractvalue %_Z11BuilderListI6StringE %load.struct, 0
  %tuple = alloca %_Z19BuilderListIteratorI6StringE, align 8
  %tuple.field = getelementptr inbounds %_Z19BuilderListIteratorI6StringE, ptr %tuple, i32 0, i32 0
  store ptr %head, ptr %tuple.field, align 1
  %tuple.val = load %_Z19BuilderListIteratorI6StringE, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z19BuilderListIteratorI6StringE, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN11BuilderListI6StringEC1Ev(ptr %0) {
entry:
  %head = getelementptr inbounds %_Z11BuilderListI6StringE, ptr %0, i32 0, i32 0
  store ptr null, ptr %head, align 8
  ret void
}

define linkonce_odr ptr @_ZN6VectorI11BuilderListI4SlotI6StringEEE3getEPN4scaly6memory4PageEm(ptr %0, ptr %1, i64 %2) {
entry:
  %load.struct = load %_Z6VectorI11BuilderListI4SlotI6StringEEE, ptr %1, align 8
  %length = extractvalue %_Z6VectorI11BuilderListI4SlotI6StringEEE %load.struct, 0
  %ge = icmp uge i64 %2, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %load.struct1 = load %_Z6VectorI11BuilderListI4SlotI6StringEEE, ptr %1, align 8
  %data = extractvalue %_Z6VectorI11BuilderListI4SlotI6StringEEE %load.struct1, 1
  %ptr.add = getelementptr inbounds %_Z11BuilderListI4SlotI6StringEE, ptr %data, i64 %2
  ret ptr %ptr.add
}

define linkonce_odr ptr @_ZN6VectorI11BuilderListI4SlotI6StringEEE7get_ptrEm(ptr %0, i64 %1) {
entry:
  %load.struct = load %_Z6VectorI11BuilderListI4SlotI6StringEEE, ptr %0, align 8
  %length = extractvalue %_Z6VectorI11BuilderListI4SlotI6StringEEE %load.struct, 0
  %ge = icmp uge i64 %1, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %load.struct1 = load %_Z6VectorI11BuilderListI4SlotI6StringEEE, ptr %0, align 8
  %data = extractvalue %_Z6VectorI11BuilderListI4SlotI6StringEEE %load.struct1, 1
  %ptr.add = getelementptr inbounds %_Z11BuilderListI4SlotI6StringEE, ptr %data, i64 %1
  ret ptr %ptr.add
}

define linkonce_odr void @_ZN6VectorI11BuilderListI4SlotI6StringEEE3putEm11BuilderListI4SlotI6StringEE(ptr %0, i64 %1, ptr %2) {
entry:
  %load.struct = load %_Z6VectorI11BuilderListI4SlotI6StringEEE, ptr %0, align 8
  %length = extractvalue %_Z6VectorI11BuilderListI4SlotI6StringEEE %load.struct, 0
  %ge = icmp uge i64 %1, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  call void @_Z11scaly_eputsP10const_char(ptr @.str.13)
  call void @_Z12scaly_eputnlv()
  call void @exit(i64 15)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %load.struct1 = load %_Z6VectorI11BuilderListI4SlotI6StringEEE, ptr %0, align 8
  %data = extractvalue %_Z6VectorI11BuilderListI4SlotI6StringEEE %load.struct1, 1
  %ptr.add = getelementptr inbounds %_Z11BuilderListI4SlotI6StringEE, ptr %data, i64 %1
  %store.load = load %_Z11BuilderListI4SlotI6StringEE, ptr %2, align 8
  store %_Z11BuilderListI4SlotI6StringEE %store.load, ptr %ptr.add, align 8
  ret void
}

define linkonce_odr void @_ZN11BuilderListI11BuilderListI4SlotI6StringEEE3addE11BuilderListI4SlotI6StringEE(ptr %0, ptr %1) {
entry:
  %load.struct = load %_Z11BuilderListI11BuilderListI4SlotI6StringEEE, ptr %0, align 8
  %head = extractvalue %_Z11BuilderListI11BuilderListI4SlotI6StringEEE %load.struct, 0
  %own_page = call ptr @_Z3getPv(ptr %0)
  %tuple.region = call ptr @_ZN4Page8allocateEmm(ptr %own_page, i64 ptrtoint (ptr getelementptr (%_Z4NodeI11BuilderListI4SlotI6StringEEE, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr ({ i1, %_Z4NodeI11BuilderListI4SlotI6StringEEE }, ptr null, i64 0, i32 1) to i64))
  %field.load = load %_Z11BuilderListI4SlotI6StringEE, ptr %1, align 8
  %tuple.field = getelementptr inbounds %_Z4NodeI11BuilderListI4SlotI6StringEEE, ptr %tuple.region, i32 0, i32 0
  store %_Z11BuilderListI4SlotI6StringEE %field.load, ptr %tuple.field, align 1
  %tuple.field1 = getelementptr inbounds %_Z4NodeI11BuilderListI4SlotI6StringEEE, ptr %tuple.region, i32 0, i32 1
  store ptr %head, ptr %tuple.field1, align 1
  %head2 = getelementptr inbounds %_Z11BuilderListI11BuilderListI4SlotI6StringEEE, ptr %0, i32 0, i32 0
  store ptr %tuple.region, ptr %head2, align 8
  ret void
}

define linkonce_odr i1 @_ZN11BuilderListI11BuilderListI4SlotI6StringEEE6removeE11BuilderListI4SlotI6StringEE(ptr %0, ptr %1) {
entry:
  %load.struct = load %_Z11BuilderListI11BuilderListI4SlotI6StringEEE, ptr %0, align 8
  %head = extractvalue %_Z11BuilderListI11BuilderListI4SlotI6StringEEE %load.struct, 0
  %node = alloca ptr, align 8
  store ptr %head, ptr %node, align 1
  %previous_node = alloca ptr, align 8
  store ptr null, ptr %previous_node, align 1
  br label %while.cond

while.cond:                                       ; preds = %while.body, %entry
  %node1 = load ptr, ptr %node, align 8
  %ne = icmp ne ptr %node1, null
  br i1 %ne, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %node2 = load ptr, ptr %node, align 8
  %load.struct3 = load %_Z4NodeI11BuilderListI4SlotI6StringEEE, ptr %node2, align 8
  %element = extractvalue %_Z4NodeI11BuilderListI4SlotI6StringEEE %load.struct3, 0
  %node4 = load ptr, ptr %node, align 8
  store ptr %node4, ptr %previous_node, align 1
  %node5 = load ptr, ptr %node, align 8
  %load.struct6 = load %_Z4NodeI11BuilderListI4SlotI6StringEEE, ptr %node5, align 8
  %next = extractvalue %_Z4NodeI11BuilderListI4SlotI6StringEEE %load.struct6, 1
  store ptr %next, ptr %node, align 1
  br label %while.cond

while.exit:                                       ; preds = %while.cond
  ret i1 false
}

define linkonce_odr ptr @_ZN11BuilderListI11BuilderListI4SlotI6StringEEE8get_headEPN4scaly6memory4PageE(ptr %0, ptr %1) {
entry:
  %load.struct = load %_Z11BuilderListI11BuilderListI4SlotI6StringEEE, ptr %1, align 8
  %head = extractvalue %_Z11BuilderListI11BuilderListI4SlotI6StringEEE %load.struct, 0
  %eq = icmp eq ptr %head, null
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %addr.gep = getelementptr inbounds %_Z11BuilderListI11BuilderListI4SlotI6StringEEE, ptr %1, i32 0, i32 0
  %addr.gep1 = getelementptr inbounds %_Z4NodeI11BuilderListI4SlotI6StringEEE, ptr %addr.gep, i32 0, i32 0
  ret ptr %addr.gep1
}

define linkonce_odr ptr @_ZN19BuilderListIteratorI11BuilderListI4SlotI6StringEEE4nextEv(ptr %0) {
entry:
  %load.struct = load %_Z19BuilderListIteratorI11BuilderListI4SlotI6StringEEE, ptr %0, align 8
  %current = extractvalue %_Z19BuilderListIteratorI11BuilderListI4SlotI6StringEEE %load.struct, 0
  %ne = icmp ne ptr %current, null
  br i1 %ne, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %load.struct1 = load %_Z19BuilderListIteratorI11BuilderListI4SlotI6StringEEE, ptr %0, align 8
  %current2 = extractvalue %_Z19BuilderListIteratorI11BuilderListI4SlotI6StringEEE %load.struct1, 0
  %load.struct3 = load %_Z19BuilderListIteratorI11BuilderListI4SlotI6StringEEE, ptr %0, align 8
  %current4 = extractvalue %_Z19BuilderListIteratorI11BuilderListI4SlotI6StringEEE %load.struct3, 0
  %deref = load %_Z4NodeI11BuilderListI4SlotI6StringEEE, ptr %current4, align 8
  %next = extractvalue %_Z4NodeI11BuilderListI4SlotI6StringEEE %deref, 1
  %current5 = getelementptr inbounds %_Z19BuilderListIteratorI11BuilderListI4SlotI6StringEEE, ptr %0, i32 0, i32 0
  store ptr %next, ptr %current5, align 8
  %addr.gep = getelementptr inbounds %_Z4NodeI11BuilderListI4SlotI6StringEEE, ptr %current2, i32 0, i32 0
  ret ptr %addr.gep

if.else:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; No predecessors!
  ret ptr null
}

define linkonce_odr void @_ZN19BuilderListIteratorI11BuilderListI4SlotI6StringEEEC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr i64 @_ZN11BuilderListI11BuilderListI4SlotI6StringEEE5countEv(ptr %0) {
entry:
  %local_page = call ptr @_Z21scaly_alloc_root_pagev()
  %sret.result = alloca %_Z19BuilderListIteratorI11BuilderListI4SlotI6StringEEE, align 8
  call void @_ZN11BuilderListI11BuilderListI4SlotI6StringEEE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z19BuilderListIteratorI11BuilderListI4SlotI6StringEEE) %sret.result, ptr %local_page, ptr %0)
  %list_iterator = alloca ptr, align 8
  store ptr %sret.result, ptr %list_iterator, align 1
  %i = alloca i64, align 8
  store i64 0, ptr %i, align 1
  br label %while.cond

while.cond:                                       ; preds = %while.body, %entry
  %list_iterator1 = load ptr, ptr %list_iterator, align 8
  %call = call ptr @_ZN19BuilderListIteratorI11BuilderListI4SlotI6StringEEE4nextEv(ptr %list_iterator1)
  %ne = icmp ne ptr %call, null
  br i1 %ne, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %i2 = load i64, ptr %i, align 4
  %add = add i64 %i2, 1
  store i64 %add, ptr %i, align 1
  br label %while.cond

while.exit:                                       ; preds = %while.cond
  %i3 = load i64, ptr %i, align 4
  call void @_Z28scaly_release_root_page_fullP4Page(ptr %local_page)
  ret i64 %i3
}

define linkonce_odr void @_ZN11BuilderListI11BuilderListI4SlotI6StringEEE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z19BuilderListIteratorI11BuilderListI4SlotI6StringEEE) %0, ptr %1, ptr %2) {
entry:
  %load.struct = load %_Z11BuilderListI11BuilderListI4SlotI6StringEEE, ptr %2, align 8
  %head = extractvalue %_Z11BuilderListI11BuilderListI4SlotI6StringEEE %load.struct, 0
  %tuple = alloca %_Z19BuilderListIteratorI11BuilderListI4SlotI6StringEEE, align 8
  %tuple.field = getelementptr inbounds %_Z19BuilderListIteratorI11BuilderListI4SlotI6StringEEE, ptr %tuple, i32 0, i32 0
  store ptr %head, ptr %tuple.field, align 1
  %tuple.val = load %_Z19BuilderListIteratorI11BuilderListI4SlotI6StringEEE, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z19BuilderListIteratorI11BuilderListI4SlotI6StringEEE, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN11BuilderListI11BuilderListI4SlotI6StringEEEC1Ev(ptr %0) {
entry:
  %head = getelementptr inbounds %_Z11BuilderListI11BuilderListI4SlotI6StringEEE, ptr %0, i32 0, i32 0
  store ptr null, ptr %head, align 8
  ret void
}

define linkonce_odr void @_ZN6VectorI11BuilderListI4SlotI6StringEEE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z14VectorIteratorI11BuilderListI4SlotI6StringEEE) %0, ptr %1, ptr %2) {
entry:
  %struct.init = alloca %_Z14VectorIteratorI11BuilderListI4SlotI6StringEEE, align 8
  call void @_ZN14VectorIteratorI11BuilderListI4SlotI6StringEEEC1EP6VectorI11BuilderListI4SlotI6StringEEE(ptr %struct.init, ptr %2)
  %sret.body = load %_Z14VectorIteratorI11BuilderListI4SlotI6StringEEE, ptr %struct.init, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %struct.init, i64 ptrtoint (ptr getelementptr (%_Z14VectorIteratorI11BuilderListI4SlotI6StringEEE, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN6VectorI11BuilderListI4SlotI6StringEEE8as_sliceEPN4scaly6memory4PageE(ptr noalias sret(%_Z5Slice) %0, ptr %1, ptr %2) {
entry:
  %load.struct = load %_Z6VectorI11BuilderListI4SlotI6StringEEE, ptr %2, align 8
  %data = extractvalue %_Z6VectorI11BuilderListI4SlotI6StringEEE %load.struct, 1
  %load.struct1 = load %_Z6VectorI11BuilderListI4SlotI6StringEEE, ptr %2, align 8
  %length = extractvalue %_Z6VectorI11BuilderListI4SlotI6StringEEE %load.struct1, 0
  %tuple = alloca { ptr, i64 }, align 8
  %tuple.field = getelementptr inbounds { ptr, i64 }, ptr %tuple, i32 0, i32 0
  store ptr %data, ptr %tuple.field, align 1
  %tuple.field2 = getelementptr inbounds { ptr, i64 }, ptr %tuple, i32 0, i32 1
  store i64 %length, ptr %tuple.field2, align 1
  %tuple.val = load { ptr, i64 }, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr ({ ptr, i64 }, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN6VectorI11BuilderListI4SlotI6StringEEEC1Ev(ptr %0) {
entry:
  %length = getelementptr inbounds %_Z6VectorI11BuilderListI4SlotI6StringEEE, ptr %0, i32 0, i32 0
  store i64 0, ptr %length, align 4
  %data = getelementptr inbounds %_Z6VectorI11BuilderListI4SlotI6StringEEE, ptr %0, i32 0, i32 1
  store ptr null, ptr %data, align 8
  ret void
}

define linkonce_odr void @_ZN6VectorI11BuilderListI4SlotI6StringEEEC1EP11BuilderListI4SlotI6StringEEm(ptr %0, ptr %1, i64 %2) {
entry:
  %length = getelementptr inbounds %_Z6VectorI11BuilderListI4SlotI6StringEEE, ptr %0, i32 0, i32 0
  store i64 %2, ptr %length, align 4
  %data = getelementptr inbounds %_Z6VectorI11BuilderListI4SlotI6StringEEE, ptr %0, i32 0, i32 1
  store ptr %1, ptr %data, align 8
  ret void
}

define linkonce_odr void @_ZN6VectorI11BuilderListI4SlotI6StringEEEC1EPN4scaly6memory4PageEm(ptr %0, ptr %1, i64 %2) {
entry:
  %length = getelementptr inbounds %_Z6VectorI11BuilderListI4SlotI6StringEEE, ptr %0, i32 0, i32 0
  store i64 %2, ptr %length, align 4
  %gt = icmp ugt i64 %2, 0
  br i1 %gt, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %mul = mul i64 %2, ptrtoint (ptr getelementptr (%_Z11BuilderListI4SlotI6StringEE, ptr null, i32 1) to i64)
  %call = call ptr @_ZN4Page8allocateEmm(ptr %1, i64 %mul, i64 8)
  %data = getelementptr inbounds %_Z6VectorI11BuilderListI4SlotI6StringEEE, ptr %0, i32 0, i32 1
  store ptr %call, ptr %data, align 8
  %field.inplace = getelementptr inbounds %_Z6VectorI11BuilderListI4SlotI6StringEEE, ptr %0, i32 0, i32 1
  %deref.recv = load ptr, ptr %field.inplace, align 8
  %mul1 = mul i64 %2, ptrtoint (ptr getelementptr (%_Z11BuilderListI4SlotI6StringEE, ptr null, i32 1) to i64)
  %call2 = call ptr @memset(ptr %deref.recv, i64 0, i64 %mul1)
  br label %if.end

if.else:                                          ; preds = %entry
  %data3 = getelementptr inbounds %_Z6VectorI11BuilderListI4SlotI6StringEEE, ptr %0, i32 0, i32 1
  store ptr null, ptr %data3, align 8
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  %if.value = phi ptr [ %call2, %if.then ], [ null, %if.else ]
  ret void
}

define linkonce_odr void @_ZN6VectorI11BuilderListI4SlotI6StringEEEC1EPN4scaly6memory4PageEP11BuilderListI4SlotI6StringEEm(ptr %0, ptr %1, ptr %2, i64 %3) {
entry:
  %length = getelementptr inbounds %_Z6VectorI11BuilderListI4SlotI6StringEEE, ptr %0, i32 0, i32 0
  store i64 %3, ptr %length, align 4
  %gt = icmp ugt i64 %3, 0
  br i1 %gt, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %mul = mul i64 %3, ptrtoint (ptr getelementptr (%_Z11BuilderListI4SlotI6StringEE, ptr null, i32 1) to i64)
  %call = call ptr @_ZN4Page8allocateEmm(ptr %1, i64 %mul, i64 8)
  %data = getelementptr inbounds %_Z6VectorI11BuilderListI4SlotI6StringEEE, ptr %0, i32 0, i32 1
  store ptr %call, ptr %data, align 8
  %field.inplace = getelementptr inbounds %_Z6VectorI11BuilderListI4SlotI6StringEEE, ptr %0, i32 0, i32 1
  %deref.recv = load ptr, ptr %field.inplace, align 8
  %mul1 = mul i64 %3, ptrtoint (ptr getelementptr (%_Z11BuilderListI4SlotI6StringEE, ptr null, i32 1) to i64)
  %call2 = call ptr @memcpy(ptr %deref.recv, ptr %2, i64 %mul1)
  br label %if.end

if.else:                                          ; preds = %entry
  %data3 = getelementptr inbounds %_Z6VectorI11BuilderListI4SlotI6StringEEE, ptr %0, i32 0, i32 1
  store ptr null, ptr %data3, align 8
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  %if.value = phi ptr [ %call2, %if.then ], [ null, %if.else ]
  ret void
}

define linkonce_odr void @_ZN6VectorI11BuilderListI4SlotI6StringEEEC1EPN4scaly6memory4PageE6VectorI11BuilderListI4SlotI6StringEEE(ptr %0, ptr %1, ptr %2) {
entry:
  %load.struct = load %_Z6VectorI11BuilderListI4SlotI6StringEEE, ptr %2, align 8
  %length = extractvalue %_Z6VectorI11BuilderListI4SlotI6StringEEE %load.struct, 0
  %length1 = getelementptr inbounds %_Z6VectorI11BuilderListI4SlotI6StringEEE, ptr %0, i32 0, i32 0
  store i64 %length, ptr %length1, align 4
  %load.struct2 = load %_Z6VectorI11BuilderListI4SlotI6StringEEE, ptr %0, align 8
  %length3 = extractvalue %_Z6VectorI11BuilderListI4SlotI6StringEEE %load.struct2, 0
  %gt = icmp ugt i64 %length3, 0
  br i1 %gt, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %load.struct4 = load %_Z6VectorI11BuilderListI4SlotI6StringEEE, ptr %0, align 8
  %length5 = extractvalue %_Z6VectorI11BuilderListI4SlotI6StringEEE %load.struct4, 0
  %mul = mul i64 %length5, ptrtoint (ptr getelementptr (%_Z11BuilderListI4SlotI6StringEE, ptr null, i32 1) to i64)
  %call = call ptr @_ZN4Page8allocateEmm(ptr %1, i64 %mul, i64 8)
  %data = getelementptr inbounds %_Z6VectorI11BuilderListI4SlotI6StringEEE, ptr %0, i32 0, i32 1
  store ptr %call, ptr %data, align 8
  %field.inplace = getelementptr inbounds %_Z6VectorI11BuilderListI4SlotI6StringEEE, ptr %0, i32 0, i32 1
  %deref.recv = load ptr, ptr %field.inplace, align 8
  %field.inplace6 = getelementptr inbounds %_Z6VectorI11BuilderListI4SlotI6StringEEE, ptr %2, i32 0, i32 1
  %deref.recv7 = load ptr, ptr %field.inplace6, align 8
  %load.struct8 = load %_Z6VectorI11BuilderListI4SlotI6StringEEE, ptr %0, align 8
  %length9 = extractvalue %_Z6VectorI11BuilderListI4SlotI6StringEEE %load.struct8, 0
  %mul10 = mul i64 %length9, ptrtoint (ptr getelementptr (%_Z11BuilderListI4SlotI6StringEE, ptr null, i32 1) to i64)
  %call11 = call ptr @memcpy(ptr %deref.recv, ptr %deref.recv7, i64 %mul10)
  br label %if.end

if.else:                                          ; preds = %entry
  %data12 = getelementptr inbounds %_Z6VectorI11BuilderListI4SlotI6StringEEE, ptr %0, i32 0, i32 1
  store ptr null, ptr %data12, align 8
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  %if.value = phi ptr [ %call11, %if.then ], [ null, %if.else ]
  ret void
}

define linkonce_odr ptr @_ZN5ArrayI11BuilderListI4SlotI6StringEEE10get_bufferEv(ptr %0) {
entry:
  ret ptr null
}

define linkonce_odr i64 @_ZN5ArrayI11BuilderListI4SlotI6StringEEE10get_lengthEv(ptr %0) {
entry:
  %load.struct = load %_Z5ArrayI11BuilderListI4SlotI6StringEEE, ptr %0, align 8
  %length = extractvalue %_Z5ArrayI11BuilderListI4SlotI6StringEEE %load.struct, 0
  ret i64 %length
}

define linkonce_odr i64 @_ZN5ArrayI11BuilderListI4SlotI6StringEEE12get_capacityEv(ptr %0) {
entry:
  ret i64 0
}

define linkonce_odr void @_ZN5ArrayI11BuilderListI4SlotI6StringEEE10reallocateEv(ptr %0) {
entry:
  %call = call ptr @_ZN4Page3getEPv(ptr %0)
  %length = alloca i64, align 8
  store i64 0, ptr %length, align 1
  ret void
}

define linkonce_odr void @_ZN5ArrayI11BuilderListI4SlotI6StringEEE3addE11BuilderListI4SlotI6StringEE(ptr %0, ptr %1) {
entry:
  br i1 false, label %lor.end, label %lor.rhs

lor.rhs:                                          ; preds = %entry
  %load.struct = load %_Z5ArrayI11BuilderListI4SlotI6StringEEE, ptr %0, align 8
  %length = extractvalue %_Z5ArrayI11BuilderListI4SlotI6StringEEE %load.struct, 0
  br label %lor.end

lor.end:                                          ; preds = %lor.rhs, %entry
  %lor.result = phi i1 [ true, %entry ], [ true, %lor.rhs ]
  br i1 %lor.result, label %if.then, label %if.end

if.then:                                          ; preds = %lor.end
  call void @_ZN5ArrayI11BuilderListI4SlotI6StringEEE10reallocateEv(ptr %0)
  br label %if.end

if.end:                                           ; preds = %if.then, %lor.end
  %load.struct1 = load %_Z5ArrayI11BuilderListI4SlotI6StringEEE, ptr %0, align 8
  %length2 = extractvalue %_Z5ArrayI11BuilderListI4SlotI6StringEEE %load.struct1, 0
  %load.struct3 = load %_Z5ArrayI11BuilderListI4SlotI6StringEEE, ptr %0, align 8
  %length4 = extractvalue %_Z5ArrayI11BuilderListI4SlotI6StringEEE %load.struct3, 0
  %add = add i64 %length4, 1
  %length5 = getelementptr inbounds %_Z5ArrayI11BuilderListI4SlotI6StringEEE, ptr %0, i32 0, i32 0
  store i64 %add, ptr %length5, align 4
  ret void
}

define linkonce_odr void @_ZN5ArrayI11BuilderListI4SlotI6StringEEE3addE6VectorI11BuilderListI4SlotI6StringEEE(ptr %0, ptr %1) {
entry:
  %load.struct = load %_Z5ArrayI11BuilderListI4SlotI6StringEEE, ptr %0, align 8
  %length = extractvalue %_Z5ArrayI11BuilderListI4SlotI6StringEEE %load.struct, 0
  %load.struct1 = load %_Z6VectorI11BuilderListI4SlotI6StringEEE, ptr %1, align 8
  %length2 = extractvalue %_Z6VectorI11BuilderListI4SlotI6StringEEE %load.struct1, 0
  %add = add i64 %length, %length2
  %new_length = alloca i64, align 8
  store i64 %add, ptr %new_length, align 1
  %new_length3 = load i64, ptr %new_length, align 4
  %load.struct4 = load %_Z5ArrayI11BuilderListI4SlotI6StringEEE, ptr %0, align 8
  %length5 = extractvalue %_Z5ArrayI11BuilderListI4SlotI6StringEEE %load.struct4, 0
  %lt = icmp ult i64 %new_length3, %length5
  br i1 %lt, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  call void @exit(i64 14)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  br i1 false, label %lor.end, label %lor.rhs

lor.rhs:                                          ; preds = %if.end
  %new_length6 = load i64, ptr %new_length, align 4
  br label %lor.end

lor.end:                                          ; preds = %lor.rhs, %if.end
  %lor.result = phi i1 [ true, %if.end ], [ true, %lor.rhs ]
  br i1 %lor.result, label %if.then7, label %if.end8

if.then7:                                         ; preds = %lor.end
  call void @_ZN5ArrayI11BuilderListI4SlotI6StringEEE10reallocateEv(ptr %0)
  br label %if.end8

if.end8:                                          ; preds = %if.then7, %lor.end
  %new_length9 = load i64, ptr %new_length, align 4
  %load.struct10 = load %_Z6VectorI11BuilderListI4SlotI6StringEEE, ptr %1, align 8
  %length11 = extractvalue %_Z6VectorI11BuilderListI4SlotI6StringEEE %load.struct10, 0
  %gt = icmp ugt i64 %length11, 0
  br i1 %gt, label %if.then12, label %if.end13

if.then12:                                        ; preds = %if.end8
  %load.struct14 = load %_Z5ArrayI11BuilderListI4SlotI6StringEEE, ptr %0, align 8
  %length15 = extractvalue %_Z5ArrayI11BuilderListI4SlotI6StringEEE %load.struct14, 0
  %mul = mul i64 %length15, ptrtoint (ptr getelementptr (%_Z11BuilderListI4SlotI6StringEE, ptr null, i32 1) to i64)
  br label %if.end13

if.end13:                                         ; preds = %if.then12, %if.end8
  %load.struct16 = load %_Z5ArrayI11BuilderListI4SlotI6StringEEE, ptr %0, align 8
  %length17 = extractvalue %_Z5ArrayI11BuilderListI4SlotI6StringEEE %load.struct16, 0
  %load.struct18 = load %_Z6VectorI11BuilderListI4SlotI6StringEEE, ptr %1, align 8
  %length19 = extractvalue %_Z6VectorI11BuilderListI4SlotI6StringEEE %load.struct18, 0
  %add20 = add i64 %length17, %length19
  %length21 = getelementptr inbounds %_Z5ArrayI11BuilderListI4SlotI6StringEEE, ptr %0, i32 0, i32 0
  store i64 %add20, ptr %length21, align 4
  ret void
}

define linkonce_odr ptr @_ZN5ArrayI11BuilderListI4SlotI6StringEEE3getEPN4scaly6memory4PageEm(ptr %0, ptr %1, i64 %2) {
entry:
  %load.struct = load %_Z5ArrayI11BuilderListI4SlotI6StringEEE, ptr %1, align 8
  %length = extractvalue %_Z5ArrayI11BuilderListI4SlotI6StringEEE %load.struct, 0
  %ge = icmp uge i64 %2, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  ret ptr null
}

define linkonce_odr void @_ZN5ArrayI11BuilderListI4SlotI6StringEEE3putEm11BuilderListI4SlotI6StringEE(ptr %0, i64 %1, ptr %2) {
entry:
  %load.struct = load %_Z5ArrayI11BuilderListI4SlotI6StringEEE, ptr %0, align 8
  %length = extractvalue %_Z5ArrayI11BuilderListI4SlotI6StringEEE %load.struct, 0
  %ge = icmp uge i64 %1, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  call void @_Z11scaly_eputsP10const_char(ptr @.str.14)
  call void @_Z12scaly_eputnlv()
  call void @exit(i64 15)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  ret void
}

define linkonce_odr ptr @_ZN13ArrayIteratorI11BuilderListI4SlotI6StringEEE4nextEv(ptr %0) {
entry:
  %load.struct = load %_Z13ArrayIteratorI11BuilderListI4SlotI6StringEEE, ptr %0, align 8
  %position = extractvalue %_Z13ArrayIteratorI11BuilderListI4SlotI6StringEEE %load.struct, 1
  %load.struct1 = load %_Z13ArrayIteratorI11BuilderListI4SlotI6StringEEE, ptr %0, align 8
  %position2 = extractvalue %_Z13ArrayIteratorI11BuilderListI4SlotI6StringEEE %load.struct1, 1
  %add = add i64 %position2, 1
  %position3 = getelementptr inbounds %_Z13ArrayIteratorI11BuilderListI4SlotI6StringEEE, ptr %0, i32 0, i32 1
  store i64 %add, ptr %position3, align 4
  %load.struct4 = load %_Z13ArrayIteratorI11BuilderListI4SlotI6StringEEE, ptr %0, align 8
  %position5 = extractvalue %_Z13ArrayIteratorI11BuilderListI4SlotI6StringEEE %load.struct4, 1
  %sub = sub i64 %position5, 1
  ret ptr null
}

define linkonce_odr void @_ZN13ArrayIteratorI11BuilderListI4SlotI6StringEEEC1EP5ArrayI11BuilderListI4SlotI6StringEEE(ptr %0, ptr %1) {
entry:
  %position = getelementptr inbounds %_Z13ArrayIteratorI11BuilderListI4SlotI6StringEEE, ptr %0, i32 0, i32 1
  store i64 0, ptr %position, align 4
  ret void
}

define linkonce_odr void @_ZN13ArrayIteratorI11BuilderListI4SlotI6StringEEEC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr void @_ZN5ArrayI11BuilderListI4SlotI6StringEEE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z13ArrayIteratorI11BuilderListI4SlotI6StringEEE) %0, ptr %1, ptr %2) {
entry:
  %struct.init = alloca %_Z13ArrayIteratorI11BuilderListI4SlotI6StringEEE, align 8
  call void @_ZN13ArrayIteratorI11BuilderListI4SlotI6StringEEEC1EP5ArrayI11BuilderListI4SlotI6StringEEE(ptr %struct.init, ptr %2)
  %sret.body = load %_Z13ArrayIteratorI11BuilderListI4SlotI6StringEEE, ptr %struct.init, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %struct.init, i64 ptrtoint (ptr getelementptr (%_Z13ArrayIteratorI11BuilderListI4SlotI6StringEEE, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN5ArrayI11BuilderListI4SlotI6StringEEEC1Ev(ptr %0) {
entry:
  %length = getelementptr inbounds %_Z5ArrayI11BuilderListI4SlotI6StringEEE, ptr %0, i32 0, i32 0
  store i64 0, ptr %length, align 4
  ret void
}

define linkonce_odr void @_ZN5ArrayI11BuilderListI4SlotI6StringEEEC1Em(ptr %0, i64 %1) {
entry:
  %length = getelementptr inbounds %_Z5ArrayI11BuilderListI4SlotI6StringEEE, ptr %0, i32 0, i32 0
  store i64 0, ptr %length, align 4
  %call = call ptr @_ZN4Page3getEPv(ptr %0)
  %call1 = call ptr @_ZN4Page23allocate_exclusive_pageEv(ptr %call)
  %struct.region = call ptr @_ZN4Page8allocateEmm(ptr %call1, i64 ptrtoint (ptr getelementptr (%_Z6VectorI11BuilderListI4SlotI6StringEEE, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr ({ i1, %_Z6VectorI11BuilderListI4SlotI6StringEEE }, ptr null, i64 0, i32 1) to i64))
  call void @_ZN6VectorI11BuilderListI4SlotI6StringEEEC1EPN4scaly6memory4PageEm(ptr %struct.region, ptr %call1, i64 %1)
  ret void
}

define linkonce_odr void @_ZN5ArrayI11BuilderListI4SlotI6StringEEEC1EPN4scaly6memory4PageE(ptr %0, ptr %1) {
entry:
  %length = getelementptr inbounds %_Z5ArrayI11BuilderListI4SlotI6StringEEE, ptr %0, i32 0, i32 0
  store i64 0, ptr %length, align 4
  ret void
}

define linkonce_odr void @_ZN5ArrayI11BuilderListI4SlotI6StringEEEC1EPN4scaly6memory4PageEm(ptr %0, ptr %1, i64 %2) {
entry:
  %length = getelementptr inbounds %_Z5ArrayI11BuilderListI4SlotI6StringEEE, ptr %0, i32 0, i32 0
  store i64 0, ptr %length, align 4
  %call = call ptr @_ZN4Page23allocate_exclusive_pageEv(ptr %1)
  %struct.region = call ptr @_ZN4Page8allocateEmm(ptr %call, i64 ptrtoint (ptr getelementptr (%_Z6VectorI11BuilderListI4SlotI6StringEEE, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr ({ i1, %_Z6VectorI11BuilderListI4SlotI6StringEEE }, ptr null, i64 0, i32 1) to i64))
  call void @_ZN6VectorI11BuilderListI4SlotI6StringEEEC1EPN4scaly6memory4PageEm(ptr %struct.region, ptr %call, i64 %2)
  ret void
}

define linkonce_odr void @_ZN6VectorI11BuilderListI4SlotI6StringEEEC1EPN4scaly6memory4PageE5ArrayI11BuilderListI4SlotI6StringEEE(ptr %0, ptr %1, ptr %2) {
entry:
  %load.struct = load %_Z5ArrayI11BuilderListI4SlotI6StringEEE, ptr %2, align 8
  %length = extractvalue %_Z5ArrayI11BuilderListI4SlotI6StringEEE %load.struct, 0
  %length1 = getelementptr inbounds %_Z6VectorI11BuilderListI4SlotI6StringEEE, ptr %0, i32 0, i32 0
  store i64 %length, ptr %length1, align 4
  %load.struct2 = load %_Z6VectorI11BuilderListI4SlotI6StringEEE, ptr %0, align 8
  %length3 = extractvalue %_Z6VectorI11BuilderListI4SlotI6StringEEE %load.struct2, 0
  %gt = icmp ugt i64 %length3, 0
  br i1 %gt, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %load.struct4 = load %_Z6VectorI11BuilderListI4SlotI6StringEEE, ptr %0, align 8
  %length5 = extractvalue %_Z6VectorI11BuilderListI4SlotI6StringEEE %load.struct4, 0
  %mul = mul i64 %length5, ptrtoint (ptr getelementptr (%_Z11BuilderListI4SlotI6StringEE, ptr null, i32 1) to i64)
  %call = call ptr @_ZN4Page8allocateEmm(ptr %1, i64 %mul, i64 8)
  %data = getelementptr inbounds %_Z6VectorI11BuilderListI4SlotI6StringEEE, ptr %0, i32 0, i32 1
  store ptr %call, ptr %data, align 8
  %load.struct6 = load %_Z5ArrayI11BuilderListI4SlotI6StringEEE, ptr %2, align 8
  %vector = extractvalue %_Z5ArrayI11BuilderListI4SlotI6StringEEE %load.struct6, 1
  %field.inplace = getelementptr inbounds %_Z6VectorI11BuilderListI4SlotI6StringEEE, ptr %0, i32 0, i32 1
  %deref.recv = load ptr, ptr %field.inplace, align 8
  %field.inplace7 = getelementptr inbounds %_Z6VectorI11BuilderListI4SlotI6StringEEE, ptr %vector, i32 0, i32 1
  %deref.recv8 = load ptr, ptr %field.inplace7, align 8
  %load.struct9 = load %_Z6VectorI11BuilderListI4SlotI6StringEEE, ptr %0, align 8
  %length10 = extractvalue %_Z6VectorI11BuilderListI4SlotI6StringEEE %load.struct9, 0
  %mul11 = mul i64 %length10, ptrtoint (ptr getelementptr (%_Z11BuilderListI4SlotI6StringEE, ptr null, i32 1) to i64)
  %call12 = call ptr @memcpy(ptr %deref.recv, ptr %deref.recv8, i64 %mul11)
  br label %if.end

if.else:                                          ; preds = %entry
  %data13 = getelementptr inbounds %_Z6VectorI11BuilderListI4SlotI6StringEEE, ptr %0, i32 0, i32 1
  store ptr null, ptr %data13, align 8
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  %if.value = phi ptr [ %call12, %if.then ], [ null, %if.else ]
  ret void
}

define linkonce_odr ptr @_ZN4ListI11BuilderListI4SlotI6StringEEE8get_headEPN4scaly6memory4PageE(ptr %0, ptr %1) {
entry:
  %load.struct = load %_Z4ListI11BuilderListI4SlotI6StringEEE, ptr %1, align 8
  %head = extractvalue %_Z4ListI11BuilderListI4SlotI6StringEEE %load.struct, 0
  %eq = icmp eq ptr %head, null
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %addr.gep = getelementptr inbounds %_Z4ListI11BuilderListI4SlotI6StringEEE, ptr %1, i32 0, i32 0
  %addr.gep1 = getelementptr inbounds %_Z4NodeI11BuilderListI4SlotI6StringEEE, ptr %addr.gep, i32 0, i32 0
  ret ptr %addr.gep1
}

define linkonce_odr ptr @_ZN12ListIteratorI11BuilderListI4SlotI6StringEEE4nextEv(ptr %0) {
entry:
  %old_current = alloca ptr, align 8
  %load.struct = load %_Z12ListIteratorI11BuilderListI4SlotI6StringEEE, ptr %0, align 8
  %current = extractvalue %_Z12ListIteratorI11BuilderListI4SlotI6StringEEE %load.struct, 0
  %ne = icmp ne ptr %current, null
  br i1 %ne, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %load.struct1 = load %_Z12ListIteratorI11BuilderListI4SlotI6StringEEE, ptr %0, align 8
  %current2 = extractvalue %_Z12ListIteratorI11BuilderListI4SlotI6StringEEE %load.struct1, 0
  store ptr %current2, ptr %old_current, align 1
  %load.struct3 = load %_Z12ListIteratorI11BuilderListI4SlotI6StringEEE, ptr %0, align 8
  %current4 = extractvalue %_Z12ListIteratorI11BuilderListI4SlotI6StringEEE %load.struct3, 0
  %deref = load %_Z4NodeI11BuilderListI4SlotI6StringEEE, ptr %current4, align 8
  %next = extractvalue %_Z4NodeI11BuilderListI4SlotI6StringEEE %deref, 1
  %current5 = getelementptr inbounds %_Z12ListIteratorI11BuilderListI4SlotI6StringEEE, ptr %0, i32 0, i32 0
  store ptr %next, ptr %current5, align 8
  %old_current6 = load ptr, ptr %old_current, align 8
  %addr.gep = getelementptr inbounds %_Z4NodeI11BuilderListI4SlotI6StringEEE, ptr %old_current6, i32 0, i32 0
  ret ptr %addr.gep

if.else:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; No predecessors!
  ret ptr null
}

define linkonce_odr void @_ZN12ListIteratorI11BuilderListI4SlotI6StringEEEC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr i64 @_ZN4ListI11BuilderListI4SlotI6StringEEE5countEv(ptr %0) {
entry:
  %local_page = call ptr @_Z21scaly_alloc_root_pagev()
  %sret.result = alloca %_Z12ListIteratorI11BuilderListI4SlotI6StringEEE, align 8
  call void @_ZN4ListI11BuilderListI4SlotI6StringEEE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z12ListIteratorI11BuilderListI4SlotI6StringEEE) %sret.result, ptr %local_page, ptr %0)
  %list_iterator = alloca ptr, align 8
  store ptr %sret.result, ptr %list_iterator, align 1
  %i = alloca i64, align 8
  store i64 0, ptr %i, align 1
  br label %while.cond

while.cond:                                       ; preds = %while.body, %entry
  %list_iterator1 = load ptr, ptr %list_iterator, align 8
  %call = call ptr @_ZN12ListIteratorI11BuilderListI4SlotI6StringEEE4nextEv(ptr %list_iterator1)
  %ne = icmp ne ptr %call, null
  br i1 %ne, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %i2 = load i64, ptr %i, align 4
  %add = add i64 %i2, 1
  store i64 %add, ptr %i, align 1
  br label %while.cond

while.exit:                                       ; preds = %while.cond
  %i3 = load i64, ptr %i, align 4
  call void @_Z28scaly_release_root_page_fullP4Page(ptr %local_page)
  ret i64 %i3
}

define linkonce_odr i1 @_ZN4ListI11BuilderListI4SlotI6StringEEE6removeE11BuilderListI4SlotI6StringEE(ptr %0, ptr %1) {
entry:
  %load.struct = load %_Z4ListI11BuilderListI4SlotI6StringEEE, ptr %0, align 8
  %head = extractvalue %_Z4ListI11BuilderListI4SlotI6StringEEE %load.struct, 0
  %node = alloca ptr, align 8
  store ptr %head, ptr %node, align 1
  %previous_node = alloca ptr, align 8
  store ptr null, ptr %previous_node, align 1
  br label %while.cond

while.cond:                                       ; preds = %while.body, %entry
  %node1 = load ptr, ptr %node, align 8
  %ne = icmp ne ptr %node1, null
  br i1 %ne, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %node2 = load ptr, ptr %node, align 8
  %load.struct3 = load %_Z4NodeI11BuilderListI4SlotI6StringEEE, ptr %node2, align 8
  %element = extractvalue %_Z4NodeI11BuilderListI4SlotI6StringEEE %load.struct3, 0
  %node4 = load ptr, ptr %node, align 8
  store ptr %node4, ptr %previous_node, align 1
  %node5 = load ptr, ptr %node, align 8
  %load.struct6 = load %_Z4NodeI11BuilderListI4SlotI6StringEEE, ptr %node5, align 8
  %next = extractvalue %_Z4NodeI11BuilderListI4SlotI6StringEEE %load.struct6, 1
  store ptr %next, ptr %node, align 1
  br label %while.cond

while.exit:                                       ; preds = %while.cond
  ret i1 false
}

define linkonce_odr void @_ZN4ListI11BuilderListI4SlotI6StringEEE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z12ListIteratorI11BuilderListI4SlotI6StringEEE) %0, ptr %1, ptr %2) {
entry:
  %load.struct = load %_Z4ListI11BuilderListI4SlotI6StringEEE, ptr %2, align 8
  %head = extractvalue %_Z4ListI11BuilderListI4SlotI6StringEEE %load.struct, 0
  %tuple = alloca %_Z12ListIteratorI11BuilderListI4SlotI6StringEEE, align 8
  %tuple.field = getelementptr inbounds %_Z12ListIteratorI11BuilderListI4SlotI6StringEEE, ptr %tuple, i32 0, i32 0
  store ptr %head, ptr %tuple.field, align 1
  %tuple.val = load %_Z12ListIteratorI11BuilderListI4SlotI6StringEEE, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z12ListIteratorI11BuilderListI4SlotI6StringEEE, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN4ListI11BuilderListI4SlotI6StringEEE3addE11BuilderListI4SlotI6StringEE(ptr %0, ptr %1) {
entry:
  %load.struct = load %_Z4ListI11BuilderListI4SlotI6StringEEE, ptr %0, align 8
  %head = extractvalue %_Z4ListI11BuilderListI4SlotI6StringEEE %load.struct, 0
  %own_page = call ptr @_Z3getPv(ptr %0)
  %tuple.region = call ptr @_ZN4Page8allocateEmm(ptr %own_page, i64 ptrtoint (ptr getelementptr (%_Z4NodeI11BuilderListI4SlotI6StringEEE, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr ({ i1, %_Z4NodeI11BuilderListI4SlotI6StringEEE }, ptr null, i64 0, i32 1) to i64))
  %field.load = load %_Z11BuilderListI4SlotI6StringEE, ptr %1, align 8
  %tuple.field = getelementptr inbounds %_Z4NodeI11BuilderListI4SlotI6StringEEE, ptr %tuple.region, i32 0, i32 0
  store %_Z11BuilderListI4SlotI6StringEE %field.load, ptr %tuple.field, align 1
  %tuple.field1 = getelementptr inbounds %_Z4NodeI11BuilderListI4SlotI6StringEEE, ptr %tuple.region, i32 0, i32 1
  store ptr %head, ptr %tuple.field1, align 1
  %head2 = getelementptr inbounds %_Z4ListI11BuilderListI4SlotI6StringEEE, ptr %0, i32 0, i32 0
  store ptr %tuple.region, ptr %head2, align 8
  ret void
}

define linkonce_odr void @_ZN4ListI11BuilderListI4SlotI6StringEEEC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr void @_ZN6VectorI11BuilderListI4SlotI6StringEEEC1EPN4scaly6memory4PageE4ListI11BuilderListI4SlotI6StringEEE(ptr %0, ptr %1, ptr %2) {
entry:
  %deref.tmp = alloca %_Z11BuilderListI4SlotI6StringEE, align 8
  %i = alloca i64, align 8
  %list_iterator = alloca ptr, align 8
  %local_page = call ptr @_Z21scaly_alloc_root_pagev()
  %sret.result = alloca %_Z12ListIteratorI11BuilderListI4SlotI6StringEEE, align 8
  %call = call i64 @_ZN4ListI11BuilderListI4SlotI6StringEEE5countEv(ptr %2)
  %length = getelementptr inbounds %_Z6VectorI11BuilderListI4SlotI6StringEEE, ptr %0, i32 0, i32 0
  store i64 %call, ptr %length, align 4
  %load.struct = load %_Z6VectorI11BuilderListI4SlotI6StringEEE, ptr %0, align 8
  %length1 = extractvalue %_Z6VectorI11BuilderListI4SlotI6StringEEE %load.struct, 0
  %gt = icmp ugt i64 %length1, 0
  br i1 %gt, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %load.struct2 = load %_Z6VectorI11BuilderListI4SlotI6StringEEE, ptr %0, align 8
  %length3 = extractvalue %_Z6VectorI11BuilderListI4SlotI6StringEEE %load.struct2, 0
  %mul = mul i64 %length3, ptrtoint (ptr getelementptr (%_Z11BuilderListI4SlotI6StringEE, ptr null, i32 1) to i64)
  %call4 = call ptr @_ZN4Page8allocateEmm(ptr %1, i64 %mul, i64 8)
  %data = getelementptr inbounds %_Z6VectorI11BuilderListI4SlotI6StringEEE, ptr %0, i32 0, i32 1
  store ptr %call4, ptr %data, align 8
  call void @_ZN4ListI11BuilderListI4SlotI6StringEEE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z12ListIteratorI11BuilderListI4SlotI6StringEEE) %sret.result, ptr %local_page, ptr %2)
  store ptr %sret.result, ptr %list_iterator, align 1
  %load.struct5 = load %_Z6VectorI11BuilderListI4SlotI6StringEEE, ptr %0, align 8
  %length6 = extractvalue %_Z6VectorI11BuilderListI4SlotI6StringEEE %load.struct5, 0
  store i64 %length6, ptr %i, align 1
  br label %while.cond

if.else:                                          ; preds = %entry
  %data13 = getelementptr inbounds %_Z6VectorI11BuilderListI4SlotI6StringEEE, ptr %0, i32 0, i32 1
  store ptr null, ptr %data13, align 8
  br label %if.end

if.end:                                           ; preds = %if.else, %while.exit
  call void @_Z28scaly_release_root_page_fullP4Page(ptr %local_page)
  ret void

while.cond:                                       ; preds = %while.body, %if.then
  %list_iterator7 = load ptr, ptr %list_iterator, align 8
  %call8 = call ptr @_ZN12ListIteratorI11BuilderListI4SlotI6StringEEE4nextEv(ptr %list_iterator7)
  %while.tobool = icmp ne ptr %call8, null
  br i1 %while.tobool, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %i9 = load i64, ptr %i, align 4
  %sub = sub i64 %i9, 1
  store i64 %sub, ptr %i, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %deref.tmp, ptr align 1 %call8, i64 ptrtoint (ptr getelementptr (%_Z11BuilderListI4SlotI6StringEE, ptr null, i32 1) to i64), i1 false)
  %load.struct10 = load %_Z6VectorI11BuilderListI4SlotI6StringEEE, ptr %0, align 8
  %data11 = extractvalue %_Z6VectorI11BuilderListI4SlotI6StringEEE %load.struct10, 1
  %i12 = load i64, ptr %i, align 4
  %ptr.add = getelementptr inbounds %_Z11BuilderListI4SlotI6StringEE, ptr %data11, i64 %i12
  %store.load = load %_Z11BuilderListI4SlotI6StringEE, ptr %deref.tmp, align 8
  store %_Z11BuilderListI4SlotI6StringEE %store.load, ptr %ptr.add, align 8
  br label %while.cond

while.exit:                                       ; preds = %while.cond
  br label %if.end
}

define linkonce_odr ptr @_ZN14VectorIteratorI11BuilderListI4SlotI6StringEEE4nextEv(ptr %0) {
entry:
  %load.struct = load %_Z14VectorIteratorI11BuilderListI4SlotI6StringEEE, ptr %0, align 8
  %position = extractvalue %_Z14VectorIteratorI11BuilderListI4SlotI6StringEEE %load.struct, 1
  %load.struct1 = load %_Z14VectorIteratorI11BuilderListI4SlotI6StringEEE, ptr %0, align 8
  %position2 = extractvalue %_Z14VectorIteratorI11BuilderListI4SlotI6StringEEE %load.struct1, 1
  %add = add i64 %position2, 1
  %position3 = getelementptr inbounds %_Z14VectorIteratorI11BuilderListI4SlotI6StringEEE, ptr %0, i32 0, i32 1
  store i64 %add, ptr %position3, align 4
  ret ptr null
}

define linkonce_odr void @_ZN14VectorIteratorI11BuilderListI4SlotI6StringEEEC1EP6VectorI11BuilderListI4SlotI6StringEEE(ptr %0, ptr %1) {
entry:
  %position = getelementptr inbounds %_Z14VectorIteratorI11BuilderListI4SlotI6StringEEE, ptr %0, i32 0, i32 1
  store i64 0, ptr %position, align 4
  ret void
}

define linkonce_odr void @_ZN14VectorIteratorI11BuilderListI4SlotI6StringEEEC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr ptr @_ZN19BuilderListIteratorI4SlotI6StringEE4nextEv(ptr %0) {
entry:
  %load.struct = load %_Z19BuilderListIteratorI4SlotI6StringEE, ptr %0, align 8
  %current = extractvalue %_Z19BuilderListIteratorI4SlotI6StringEE %load.struct, 0
  %ne = icmp ne ptr %current, null
  br i1 %ne, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %load.struct1 = load %_Z19BuilderListIteratorI4SlotI6StringEE, ptr %0, align 8
  %current2 = extractvalue %_Z19BuilderListIteratorI4SlotI6StringEE %load.struct1, 0
  %load.struct3 = load %_Z19BuilderListIteratorI4SlotI6StringEE, ptr %0, align 8
  %current4 = extractvalue %_Z19BuilderListIteratorI4SlotI6StringEE %load.struct3, 0
  %deref = load %_Z4NodeI4SlotI6StringEE, ptr %current4, align 8
  %next = extractvalue %_Z4NodeI4SlotI6StringEE %deref, 1
  %current5 = getelementptr inbounds %_Z19BuilderListIteratorI4SlotI6StringEE, ptr %0, i32 0, i32 0
  store ptr %next, ptr %current5, align 8
  %addr.gep = getelementptr inbounds %_Z4NodeI4SlotI6StringEE, ptr %current2, i32 0, i32 0
  ret ptr %addr.gep

if.else:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; No predecessors!
  ret ptr null
}

define linkonce_odr void @_ZN19BuilderListIteratorI4SlotI6StringEEC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr void @_ZN11BuilderListI4SlotI6StringEE3addE4SlotI6StringE(ptr %0, ptr %1) {
entry:
  %load.struct = load %_Z11BuilderListI4SlotI6StringEE, ptr %0, align 8
  %head = extractvalue %_Z11BuilderListI4SlotI6StringEE %load.struct, 0
  %own_page = call ptr @_Z3getPv(ptr %0)
  %tuple.region = call ptr @_ZN4Page8allocateEmm(ptr %own_page, i64 ptrtoint (ptr getelementptr (%_Z4NodeI4SlotI6StringEE, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr ({ i1, %_Z4NodeI4SlotI6StringEE }, ptr null, i64 0, i32 1) to i64))
  %field.load = load %_Z4SlotI6StringE, ptr %1, align 8
  %tuple.field = getelementptr inbounds %_Z4NodeI4SlotI6StringEE, ptr %tuple.region, i32 0, i32 0
  store %_Z4SlotI6StringE %field.load, ptr %tuple.field, align 1
  %tuple.field1 = getelementptr inbounds %_Z4NodeI4SlotI6StringEE, ptr %tuple.region, i32 0, i32 1
  store ptr %head, ptr %tuple.field1, align 1
  %head2 = getelementptr inbounds %_Z11BuilderListI4SlotI6StringEE, ptr %0, i32 0, i32 0
  store ptr %tuple.region, ptr %head2, align 8
  ret void
}

define linkonce_odr i1 @_ZN11BuilderListI4SlotI6StringEE6removeE4SlotI6StringE(ptr %0, ptr %1) {
entry:
  %load.struct = load %_Z11BuilderListI4SlotI6StringEE, ptr %0, align 8
  %head = extractvalue %_Z11BuilderListI4SlotI6StringEE %load.struct, 0
  %node = alloca ptr, align 8
  store ptr %head, ptr %node, align 1
  %previous_node = alloca ptr, align 8
  store ptr null, ptr %previous_node, align 1
  br label %while.cond

while.cond:                                       ; preds = %while.body, %entry
  %node1 = load ptr, ptr %node, align 8
  %ne = icmp ne ptr %node1, null
  br i1 %ne, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %node2 = load ptr, ptr %node, align 8
  %load.struct3 = load %_Z4NodeI4SlotI6StringEE, ptr %node2, align 8
  %element = extractvalue %_Z4NodeI4SlotI6StringEE %load.struct3, 0
  %node4 = load ptr, ptr %node, align 8
  store ptr %node4, ptr %previous_node, align 1
  %node5 = load ptr, ptr %node, align 8
  %load.struct6 = load %_Z4NodeI4SlotI6StringEE, ptr %node5, align 8
  %next = extractvalue %_Z4NodeI4SlotI6StringEE %load.struct6, 1
  store ptr %next, ptr %node, align 1
  br label %while.cond

while.exit:                                       ; preds = %while.cond
  ret i1 false
}

define linkonce_odr ptr @_ZN11BuilderListI4SlotI6StringEE8get_headEPN4scaly6memory4PageE(ptr %0, ptr %1) {
entry:
  %load.struct = load %_Z11BuilderListI4SlotI6StringEE, ptr %1, align 8
  %head = extractvalue %_Z11BuilderListI4SlotI6StringEE %load.struct, 0
  %eq = icmp eq ptr %head, null
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %addr.gep = getelementptr inbounds %_Z11BuilderListI4SlotI6StringEE, ptr %1, i32 0, i32 0
  %addr.gep1 = getelementptr inbounds %_Z4NodeI4SlotI6StringEE, ptr %addr.gep, i32 0, i32 0
  ret ptr %addr.gep1
}

define linkonce_odr i64 @_ZN11BuilderListI4SlotI6StringEE5countEv(ptr %0) {
entry:
  %local_page = call ptr @_Z21scaly_alloc_root_pagev()
  %sret.result = alloca %_Z19BuilderListIteratorI4SlotI6StringEE, align 8
  call void @_ZN11BuilderListI4SlotI6StringEE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z19BuilderListIteratorI4SlotI6StringEE) %sret.result, ptr %local_page, ptr %0)
  %list_iterator = alloca ptr, align 8
  store ptr %sret.result, ptr %list_iterator, align 1
  %i = alloca i64, align 8
  store i64 0, ptr %i, align 1
  br label %while.cond

while.cond:                                       ; preds = %while.body, %entry
  %list_iterator1 = load ptr, ptr %list_iterator, align 8
  %call = call ptr @_ZN19BuilderListIteratorI4SlotI6StringEE4nextEv(ptr %list_iterator1)
  %ne = icmp ne ptr %call, null
  br i1 %ne, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %i2 = load i64, ptr %i, align 4
  %add = add i64 %i2, 1
  store i64 %add, ptr %i, align 1
  br label %while.cond

while.exit:                                       ; preds = %while.cond
  %i3 = load i64, ptr %i, align 4
  call void @_Z28scaly_release_root_page_fullP4Page(ptr %local_page)
  ret i64 %i3
}

define linkonce_odr void @_ZN11BuilderListI4SlotI6StringEE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z19BuilderListIteratorI4SlotI6StringEE) %0, ptr %1, ptr %2) {
entry:
  %load.struct = load %_Z11BuilderListI4SlotI6StringEE, ptr %2, align 8
  %head = extractvalue %_Z11BuilderListI4SlotI6StringEE %load.struct, 0
  %tuple = alloca %_Z19BuilderListIteratorI4SlotI6StringEE, align 8
  %tuple.field = getelementptr inbounds %_Z19BuilderListIteratorI4SlotI6StringEE, ptr %tuple, i32 0, i32 0
  store ptr %head, ptr %tuple.field, align 1
  %tuple.val = load %_Z19BuilderListIteratorI4SlotI6StringEE, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z19BuilderListIteratorI4SlotI6StringEE, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN11BuilderListI4SlotI6StringEEC1Ev(ptr %0) {
entry:
  %head = getelementptr inbounds %_Z11BuilderListI4SlotI6StringEE, ptr %0, i32 0, i32 0
  store ptr null, ptr %head, align 8
  ret void
}

define linkonce_odr void @_ZN14HashSetBuilderI6StringE10reallocateEm(ptr %0, i64 %1) {
entry:
  %deref.tmp = alloca %_Z4SlotI6StringE, align 8
  %list_iterator = alloca %_Z19BuilderListIteratorI4SlotI6StringEE, align 8
  %tuple7 = alloca %_Z19BuilderListIteratorI4SlotI6StringEE, align 8
  %vector_iterator = alloca %_Z14VectorIteratorI11BuilderListI4SlotI6StringEEE, align 8
  %tuple = alloca %_Z14VectorIteratorI11BuilderListI4SlotI6StringEEE, align 8
  %call = call i64 @_ZN7hashing9get_primeEm(i64 %1)
  %call1 = call ptr @_ZN4Page3getEPv(ptr %0)
  %call2 = call ptr @_ZN4Page23allocate_exclusive_pageEv(ptr %call1)
  %struct.region = call ptr @_ZN4Page8allocateEmm(ptr %call2, i64 ptrtoint (ptr getelementptr (%_Z6VectorI11BuilderListI4SlotI6StringEEE, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr ({ i1, %_Z6VectorI11BuilderListI4SlotI6StringEEE }, ptr null, i64 0, i32 1) to i64))
  call void @_ZN6VectorI11BuilderListI4SlotI6StringEEEC1EPN4scaly6memory4PageEm(ptr %struct.region, ptr %call2, i64 %call)
  %load.struct = load %_Z14HashSetBuilderI6StringE, ptr %0, align 8
  %slots = extractvalue %_Z14HashSetBuilderI6StringE %load.struct, 1
  %ne = icmp ne ptr %slots, null
  br i1 %ne, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %load.struct3 = load %_Z14HashSetBuilderI6StringE, ptr %0, align 8
  %slots4 = extractvalue %_Z14HashSetBuilderI6StringE %load.struct3, 1
  %tuple.field = getelementptr inbounds %_Z14VectorIteratorI11BuilderListI4SlotI6StringEEE, ptr %tuple, i32 0, i32 0
  store ptr %slots4, ptr %tuple.field, align 1
  %tuple.val = load %_Z14VectorIteratorI11BuilderListI4SlotI6StringEEE, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %vector_iterator, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z14VectorIteratorI11BuilderListI4SlotI6StringEEE, ptr null, i32 1) to i64), i1 false)
  br label %while.cond

if.end:                                           ; preds = %while.exit, %entry
  %slots21 = getelementptr inbounds %_Z14HashSetBuilderI6StringE, ptr %0, i32 0, i32 1
  store ptr %struct.region, ptr %slots21, align 8
  ret void

while.cond:                                       ; preds = %while.exit12, %if.then
  %call5 = call ptr @_ZN14VectorIteratorI11BuilderListI4SlotI6StringEEE4nextEv(ptr %vector_iterator)
  %while.tobool = icmp ne ptr %call5, null
  br i1 %while.tobool, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %load.struct6 = load %_Z11BuilderListI4SlotI6StringEE, ptr %call5, align 8
  %head = extractvalue %_Z11BuilderListI4SlotI6StringEE %load.struct6, 0
  %tuple.field8 = getelementptr inbounds %_Z19BuilderListIteratorI4SlotI6StringEE, ptr %tuple7, i32 0, i32 0
  store ptr %head, ptr %tuple.field8, align 1
  %tuple.val9 = load %_Z19BuilderListIteratorI4SlotI6StringEE, ptr %tuple7, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %list_iterator, ptr align 1 %tuple7, i64 ptrtoint (ptr getelementptr (%_Z19BuilderListIteratorI4SlotI6StringEE, ptr null, i32 1) to i64), i1 false)
  br label %while.cond10

while.exit:                                       ; preds = %while.cond
  %load.struct18 = load %_Z14HashSetBuilderI6StringE, ptr %0, align 8
  %slots19 = extractvalue %_Z14HashSetBuilderI6StringE %load.struct18, 1
  %call20 = call ptr @_ZN4Page3getEPv(ptr %slots19)
  call void @_ZN4Page25deallocate_exclusive_pageEP4Page(ptr %call1, ptr %call20)
  br label %if.end

while.cond10:                                     ; preds = %while.body11, %while.body
  %call13 = call ptr @_ZN19BuilderListIteratorI4SlotI6StringEE4nextEv(ptr %list_iterator)
  %while.tobool14 = icmp ne ptr %call13, null
  br i1 %while.tobool14, label %while.body11, label %while.exit12

while.body11:                                     ; preds = %while.cond10
  %load.struct15 = load %_Z4SlotI6StringE, ptr %call13, align 8
  %hash_code = extractvalue %_Z4SlotI6StringE %load.struct15, 1
  %load.struct16 = load %_Z6VectorI11BuilderListI4SlotI6StringEEE, ptr %struct.region, align 8
  %length = extractvalue %_Z6VectorI11BuilderListI4SlotI6StringEEE %load.struct16, 0
  %urem = urem i64 %hash_code, %length
  %call17 = call ptr @_ZN6VectorI11BuilderListI4SlotI6StringEEE7get_ptrEm(ptr %struct.region, i64 %urem)
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %deref.tmp, ptr align 1 %call13, i64 ptrtoint (ptr getelementptr (%_Z4SlotI6StringE, ptr null, i32 1) to i64), i1 false)
  call void @_ZN11BuilderListI4SlotI6StringEE3addE4SlotI6StringE(ptr %call17, ptr %deref.tmp)
  br label %while.cond10

while.exit12:                                     ; preds = %while.cond10
  br label %while.cond
}

define linkonce_odr i1 @_ZN14HashSetBuilderI6StringE3addE6String(ptr %0, ptr %1) {
entry:
  %load.struct = load %_Z14HashSetBuilderI6StringE, ptr %0, align 8
  %length = extractvalue %_Z14HashSetBuilderI6StringE %load.struct, 0
  %add = add i64 %length, 1
  %call = call i64 @_ZN7hashing9get_primeEm(i64 %add)
  %load.struct1 = load %_Z14HashSetBuilderI6StringE, ptr %0, align 8
  %slots = extractvalue %_Z14HashSetBuilderI6StringE %load.struct1, 1
  %eq = icmp eq ptr %slots, null
  br i1 %eq, label %lor.end, label %lor.rhs

lor.rhs:                                          ; preds = %entry
  %load.struct2 = load %_Z14HashSetBuilderI6StringE, ptr %0, align 8
  %slots3 = extractvalue %_Z14HashSetBuilderI6StringE %load.struct2, 1
  %deref = load %_Z6VectorI11BuilderListI4SlotI6StringEEE, ptr %slots3, align 8
  %length4 = extractvalue %_Z6VectorI11BuilderListI4SlotI6StringEEE %deref, 0
  %gt = icmp ugt i64 %call, %length4
  br label %lor.end

lor.end:                                          ; preds = %lor.rhs, %entry
  %lor.result = phi i1 [ true, %entry ], [ %gt, %lor.rhs ]
  br i1 %lor.result, label %if.then, label %if.end

if.then:                                          ; preds = %lor.end
  %load.struct5 = load %_Z14HashSetBuilderI6StringE, ptr %0, align 8
  %length6 = extractvalue %_Z14HashSetBuilderI6StringE %load.struct5, 0
  %add7 = add i64 %length6, 1
  call void @_ZN14HashSetBuilderI6StringE10reallocateEm(ptr %0, i64 %add7)
  br label %if.end

if.end:                                           ; preds = %if.then, %lor.end
  %call8 = call i1 @_ZN14HashSetBuilderI6StringE12add_internalE6String(ptr %0, ptr %1)
  ret i1 %call8
}

define linkonce_odr i64 @_ZN6String4hashEv(ptr %0) {
entry:
  %byte = alloca i8, align 1
  %index = alloca i64, align 8
  %bit_count = alloca i64, align 8
  %length = alloca i64, align 8
  %load.struct = load %_Z6String, ptr %0, align 8
  %data = extractvalue %_Z6String %load.struct, 0
  %eq = icmp eq ptr %data, null
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %call = call i64 @_ZN7hashing4hashEPcm(ptr null, i64 0)
  ret i64 %call

if.end:                                           ; preds = %entry
  store i64 0, ptr %length, align 1
  store i64 0, ptr %bit_count, align 1
  store i64 0, ptr %index, align 1
  br label %while.cond

while.cond:                                       ; preds = %if.end15, %if.end
  br i1 true, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %bit_count1 = load i64, ptr %bit_count, align 4
  %eq2 = icmp eq i64 %bit_count1, 63
  br i1 %eq2, label %if.then3, label %if.end4

while.exit:                                       ; preds = %if.then14, %while.cond
  %load.struct19 = load %_Z6String, ptr %0, align 8
  %data20 = extractvalue %_Z6String %load.struct19, 0
  %index21 = load i64, ptr %index, align 4
  %ptr.add22 = getelementptr inbounds i8, ptr %data20, i64 %index21
  %ptr.add23 = getelementptr inbounds i8, ptr %ptr.add22, i64 1
  %length24 = load i64, ptr %length, align 4
  %call25 = call i64 @_ZN7hashing4hashEPcm(ptr %ptr.add23, i64 %length24)
  ret i64 %call25

if.then3:                                         ; preds = %while.body
  call void @exit(i64 11)
  br label %if.end4

if.end4:                                          ; preds = %if.then3, %while.body
  %load.struct5 = load %_Z6String, ptr %0, align 8
  %data6 = extractvalue %_Z6String %load.struct5, 0
  %index7 = load i64, ptr %index, align 4
  %ptr.add = getelementptr inbounds i8, ptr %data6, i64 %index7
  %deref = load i8, ptr %ptr.add, align 1
  store i8 %deref, ptr %byte, align 1
  %length8 = load i64, ptr %length, align 4
  %byte9 = load i8, ptr %byte, align 1
  %and = and i8 %byte9, 127
  %as.zext = zext i8 %and to i64
  %bit_count10 = load i64, ptr %bit_count, align 4
  %shl = shl i64 %as.zext, %bit_count10
  %or = or i64 %length8, %shl
  store i64 %or, ptr %length, align 1
  %byte11 = load i8, ptr %byte, align 1
  %and12 = and i8 %byte11, -128
  %eq13 = icmp eq i8 %and12, 0
  br i1 %eq13, label %if.then14, label %if.end15

if.then14:                                        ; preds = %if.end4
  br label %while.exit

if.end15:                                         ; preds = %if.end4
  %bit_count16 = load i64, ptr %bit_count, align 4
  %add = add i64 %bit_count16, 7
  store i64 %add, ptr %bit_count, align 1
  %index17 = load i64, ptr %index, align 4
  %add18 = add i64 %index17, 1
  store i64 %add18, ptr %index, align 1
  br label %while.cond
}

define linkonce_odr i1 @_ZN14HashSetBuilderI6StringE12add_internalE6String(ptr %0, ptr %1) {
entry:
  %arg.tmp = alloca %_Z4SlotI6StringE, align 8
  %tuple = alloca %_Z4SlotI6StringE, align 8
  %local_page = call ptr @_Z21scaly_alloc_root_pagev()
  %call = call i64 @_ZN6String4hashEv(ptr %1)
  %load.struct = load %_Z14HashSetBuilderI6StringE, ptr %0, align 8
  %slots = extractvalue %_Z14HashSetBuilderI6StringE %load.struct, 1
  %deref = load %_Z6VectorI11BuilderListI4SlotI6StringEEE, ptr %slots, align 8
  %length = extractvalue %_Z6VectorI11BuilderListI4SlotI6StringEEE %deref, 0
  %urem = urem i64 %call, %length
  %field.inplace = getelementptr inbounds %_Z14HashSetBuilderI6StringE, ptr %0, i32 0, i32 1
  %deref.recv = load ptr, ptr %field.inplace, align 8
  %call1 = call ptr @_ZN6VectorI11BuilderListI4SlotI6StringEEE7get_ptrEm(ptr %deref.recv, i64 %urem)
  %sret.result = alloca %_Z19BuilderListIteratorI4SlotI6StringEE, align 8
  call void @_ZN11BuilderListI4SlotI6StringEE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z19BuilderListIteratorI4SlotI6StringEE) %sret.result, ptr %local_page, ptr %call1)
  %iterator = alloca ptr, align 8
  store ptr %sret.result, ptr %iterator, align 1
  br label %while.cond

while.cond:                                       ; preds = %if.end, %entry
  %iterator2 = load ptr, ptr %iterator, align 8
  %call3 = call ptr @_ZN19BuilderListIteratorI4SlotI6StringEE4nextEv(ptr %iterator2)
  %while.tobool = icmp ne ptr %call3, null
  br i1 %while.tobool, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %field.inplace4 = getelementptr inbounds %_Z4SlotI6StringE, ptr %call3, i32 0, i32 0
  %call5 = call i1 @_ZN6String6equalsE6String(ptr %1, ptr %field.inplace4)
  br i1 %call5, label %if.then, label %if.end

while.exit:                                       ; preds = %while.cond
  %load.struct6 = load %_Z14HashSetBuilderI6StringE, ptr %0, align 8
  %slots7 = extractvalue %_Z14HashSetBuilderI6StringE %load.struct6, 1
  %call8 = call ptr @_ZN4Page3getEPv(ptr %slots7)
  %field.load = load %_Z6String, ptr %1, align 8
  %tuple.field = getelementptr inbounds %_Z4SlotI6StringE, ptr %tuple, i32 0, i32 0
  store %_Z6String %field.load, ptr %tuple.field, align 1
  %tuple.field9 = getelementptr inbounds %_Z4SlotI6StringE, ptr %tuple, i32 0, i32 1
  store i64 %call, ptr %tuple.field9, align 1
  %tuple.val = load %_Z4SlotI6StringE, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %arg.tmp, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z4SlotI6StringE, ptr null, i32 1) to i64), i1 false)
  call void @_ZN11BuilderListI4SlotI6StringEE3addE4SlotI6StringE(ptr %call1, ptr %arg.tmp)
  %load.struct10 = load %_Z14HashSetBuilderI6StringE, ptr %0, align 8
  %length11 = extractvalue %_Z14HashSetBuilderI6StringE %load.struct10, 0
  %add = add i64 %length11, 1
  %length12 = getelementptr inbounds %_Z14HashSetBuilderI6StringE, ptr %0, i32 0, i32 0
  store i64 %add, ptr %length12, align 4
  call void @_Z28scaly_release_root_page_fullP4Page(ptr %local_page)
  ret i1 true

if.then:                                          ; preds = %while.body
  call void @_Z28scaly_release_root_page_fullP4Page(ptr %local_page)
  ret i1 false

if.end:                                           ; preds = %while.body
  br label %while.cond
}

define linkonce_odr i1 @_ZN14HashSetBuilderI6StringE8containsE6String(ptr %0, ptr %1) {
entry:
  %iterator = alloca ptr, align 8
  %local_page = call ptr @_Z21scaly_alloc_root_pagev()
  %sret.result = alloca %_Z19BuilderListIteratorI4SlotI6StringEE, align 8
  %load.struct = load %_Z14HashSetBuilderI6StringE, ptr %0, align 8
  %slots = extractvalue %_Z14HashSetBuilderI6StringE %load.struct, 1
  %eq = icmp eq ptr %slots, null
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  call void @_Z23scaly_release_root_pageP4Page(ptr %local_page)
  ret i1 false

if.end:                                           ; preds = %entry
  %call = call i64 @_ZN6String4hashEv(ptr %1)
  %load.struct1 = load %_Z14HashSetBuilderI6StringE, ptr %0, align 8
  %slots2 = extractvalue %_Z14HashSetBuilderI6StringE %load.struct1, 1
  %deref = load %_Z6VectorI11BuilderListI4SlotI6StringEEE, ptr %slots2, align 8
  %length = extractvalue %_Z6VectorI11BuilderListI4SlotI6StringEEE %deref, 0
  %urem = urem i64 %call, %length
  %field.inplace = getelementptr inbounds %_Z14HashSetBuilderI6StringE, ptr %0, i32 0, i32 1
  %deref.recv = load ptr, ptr %field.inplace, align 8
  %call3 = call ptr @_ZN6VectorI11BuilderListI4SlotI6StringEEE7get_ptrEm(ptr %deref.recv, i64 %urem)
  call void @_ZN11BuilderListI4SlotI6StringEE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z19BuilderListIteratorI4SlotI6StringEE) %sret.result, ptr %local_page, ptr %call3)
  store ptr %sret.result, ptr %iterator, align 1
  br label %while.cond

while.cond:                                       ; preds = %if.end9, %if.end
  %iterator4 = load ptr, ptr %iterator, align 8
  %call5 = call ptr @_ZN19BuilderListIteratorI4SlotI6StringEE4nextEv(ptr %iterator4)
  %while.tobool = icmp ne ptr %call5, null
  br i1 %while.tobool, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %field.inplace6 = getelementptr inbounds %_Z4SlotI6StringE, ptr %call5, i32 0, i32 0
  %call7 = call i1 @_ZN6String6equalsE6String(ptr %1, ptr %field.inplace6)
  br i1 %call7, label %if.then8, label %if.end9

while.exit:                                       ; preds = %while.cond
  call void @_Z28scaly_release_root_page_fullP4Page(ptr %local_page)
  ret i1 false

if.then8:                                         ; preds = %while.body
  call void @_Z28scaly_release_root_page_fullP4Page(ptr %local_page)
  ret i1 true

if.end9:                                          ; preds = %while.body
  br label %while.cond
}

define linkonce_odr void @_ZN14HashSetBuilderI6StringEC1E6VectorI6StringE(ptr %0, ptr %1) {
entry:
  %deref.tmp = alloca %_Z6String, align 8
  %vector_iterator = alloca ptr, align 8
  %struct.init = alloca %_Z14VectorIteratorI6StringE, align 8
  %length = getelementptr inbounds %_Z14HashSetBuilderI6StringE, ptr %0, i32 0, i32 0
  store i64 0, ptr %length, align 4
  %slots = getelementptr inbounds %_Z14HashSetBuilderI6StringE, ptr %0, i32 0, i32 1
  store ptr null, ptr %slots, align 8
  %load.struct = load %_Z6VectorI6StringE, ptr %1, align 8
  %length1 = extractvalue %_Z6VectorI6StringE %load.struct, 0
  %gt = icmp ugt i64 %length1, 0
  br i1 %gt, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %field.inplace = getelementptr inbounds %_Z6VectorI6StringE, ptr %1, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 4
  call void @_ZN14HashSetBuilderI6StringE10reallocateEm(ptr %0, i64 %field.val)
  call void @_ZN14VectorIteratorI6StringEC1EP6VectorI6StringE(ptr %struct.init, ptr %1)
  store ptr %struct.init, ptr %vector_iterator, align 1
  br label %while.cond

if.end:                                           ; preds = %while.exit, %entry
  ret void

while.cond:                                       ; preds = %while.body, %if.then
  %vector_iterator2 = load ptr, ptr %vector_iterator, align 8
  %call = call ptr @_ZN14VectorIteratorI6StringE4nextEv(ptr %vector_iterator2)
  %while.tobool = icmp ne ptr %call, null
  br i1 %while.tobool, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %deref.tmp, ptr align 1 %call, i64 ptrtoint (ptr getelementptr (%_Z6String, ptr null, i32 1) to i64), i1 false)
  %call3 = call i1 @_ZN14HashSetBuilderI6StringE12add_internalE6String(ptr %0, ptr %deref.tmp)
  br label %while.cond

while.exit:                                       ; preds = %while.cond
  br label %if.end
}

define linkonce_odr void @_ZN14HashSetBuilderI6StringEC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr ptr @_ZN6VectorI6VectorI6StringEE3getEPN4scaly6memory4PageEm(ptr %0, ptr %1, i64 %2) {
entry:
  %load.struct = load %_Z6VectorI6VectorI6StringEE, ptr %1, align 8
  %length = extractvalue %_Z6VectorI6VectorI6StringEE %load.struct, 0
  %ge = icmp uge i64 %2, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %load.struct1 = load %_Z6VectorI6VectorI6StringEE, ptr %1, align 8
  %data = extractvalue %_Z6VectorI6VectorI6StringEE %load.struct1, 1
  %ptr.add = getelementptr inbounds %_Z6VectorI6StringE, ptr %data, i64 %2
  ret ptr %ptr.add
}

define linkonce_odr ptr @_ZN6VectorI6VectorI6StringEE7get_ptrEm(ptr %0, i64 %1) {
entry:
  %load.struct = load %_Z6VectorI6VectorI6StringEE, ptr %0, align 8
  %length = extractvalue %_Z6VectorI6VectorI6StringEE %load.struct, 0
  %ge = icmp uge i64 %1, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %load.struct1 = load %_Z6VectorI6VectorI6StringEE, ptr %0, align 8
  %data = extractvalue %_Z6VectorI6VectorI6StringEE %load.struct1, 1
  %ptr.add = getelementptr inbounds %_Z6VectorI6StringE, ptr %data, i64 %1
  ret ptr %ptr.add
}

define linkonce_odr void @_ZN6VectorI6VectorI6StringEE3putEm6VectorI6StringE(ptr %0, i64 %1, ptr %2) {
entry:
  %load.struct = load %_Z6VectorI6VectorI6StringEE, ptr %0, align 8
  %length = extractvalue %_Z6VectorI6VectorI6StringEE %load.struct, 0
  %ge = icmp uge i64 %1, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  call void @_Z11scaly_eputsP10const_char(ptr @.str.15)
  call void @_Z12scaly_eputnlv()
  call void @exit(i64 15)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %load.struct1 = load %_Z6VectorI6VectorI6StringEE, ptr %0, align 8
  %data = extractvalue %_Z6VectorI6VectorI6StringEE %load.struct1, 1
  %ptr.add = getelementptr inbounds %_Z6VectorI6StringE, ptr %data, i64 %1
  %store.load = load %_Z6VectorI6StringE, ptr %2, align 8
  store %_Z6VectorI6StringE %store.load, ptr %ptr.add, align 8
  ret void
}

define linkonce_odr ptr @_ZN14VectorIteratorI6VectorI6StringEE4nextEv(ptr %0) {
entry:
  %load.struct = load %_Z14VectorIteratorI6VectorI6StringEE, ptr %0, align 8
  %position = extractvalue %_Z14VectorIteratorI6VectorI6StringEE %load.struct, 1
  %load.struct1 = load %_Z14VectorIteratorI6VectorI6StringEE, ptr %0, align 8
  %position2 = extractvalue %_Z14VectorIteratorI6VectorI6StringEE %load.struct1, 1
  %add = add i64 %position2, 1
  %position3 = getelementptr inbounds %_Z14VectorIteratorI6VectorI6StringEE, ptr %0, i32 0, i32 1
  store i64 %add, ptr %position3, align 4
  ret ptr null
}

define linkonce_odr void @_ZN14VectorIteratorI6VectorI6StringEEC1EP6VectorI6VectorI6StringEE(ptr %0, ptr %1) {
entry:
  %position = getelementptr inbounds %_Z14VectorIteratorI6VectorI6StringEE, ptr %0, i32 0, i32 1
  store i64 0, ptr %position, align 4
  ret void
}

define linkonce_odr void @_ZN14VectorIteratorI6VectorI6StringEEC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr void @_ZN6VectorI6VectorI6StringEE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z14VectorIteratorI6VectorI6StringEE) %0, ptr %1, ptr %2) {
entry:
  %struct.init = alloca %_Z14VectorIteratorI6VectorI6StringEE, align 8
  call void @_ZN14VectorIteratorI6VectorI6StringEEC1EP6VectorI6VectorI6StringEE(ptr %struct.init, ptr %2)
  %sret.body = load %_Z14VectorIteratorI6VectorI6StringEE, ptr %struct.init, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %struct.init, i64 ptrtoint (ptr getelementptr (%_Z14VectorIteratorI6VectorI6StringEE, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN6VectorI6VectorI6StringEE8as_sliceEPN4scaly6memory4PageE(ptr noalias sret(%_Z5Slice) %0, ptr %1, ptr %2) {
entry:
  %load.struct = load %_Z6VectorI6VectorI6StringEE, ptr %2, align 8
  %data = extractvalue %_Z6VectorI6VectorI6StringEE %load.struct, 1
  %load.struct1 = load %_Z6VectorI6VectorI6StringEE, ptr %2, align 8
  %length = extractvalue %_Z6VectorI6VectorI6StringEE %load.struct1, 0
  %tuple = alloca { ptr, i64 }, align 8
  %tuple.field = getelementptr inbounds { ptr, i64 }, ptr %tuple, i32 0, i32 0
  store ptr %data, ptr %tuple.field, align 1
  %tuple.field2 = getelementptr inbounds { ptr, i64 }, ptr %tuple, i32 0, i32 1
  store i64 %length, ptr %tuple.field2, align 1
  %tuple.val = load { ptr, i64 }, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr ({ ptr, i64 }, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN6VectorI6VectorI6StringEEC1Ev(ptr %0) {
entry:
  %length = getelementptr inbounds %_Z6VectorI6VectorI6StringEE, ptr %0, i32 0, i32 0
  store i64 0, ptr %length, align 4
  %data = getelementptr inbounds %_Z6VectorI6VectorI6StringEE, ptr %0, i32 0, i32 1
  store ptr null, ptr %data, align 8
  ret void
}

define linkonce_odr void @_ZN6VectorI6VectorI6StringEEC1EP6VectorI6StringEm(ptr %0, ptr %1, i64 %2) {
entry:
  %length = getelementptr inbounds %_Z6VectorI6VectorI6StringEE, ptr %0, i32 0, i32 0
  store i64 %2, ptr %length, align 4
  %data = getelementptr inbounds %_Z6VectorI6VectorI6StringEE, ptr %0, i32 0, i32 1
  store ptr %1, ptr %data, align 8
  ret void
}

define linkonce_odr void @_ZN6VectorI6VectorI6StringEEC1EPN4scaly6memory4PageEm(ptr %0, ptr %1, i64 %2) {
entry:
  %length = getelementptr inbounds %_Z6VectorI6VectorI6StringEE, ptr %0, i32 0, i32 0
  store i64 %2, ptr %length, align 4
  %gt = icmp ugt i64 %2, 0
  br i1 %gt, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %mul = mul i64 %2, ptrtoint (ptr getelementptr (%_Z6VectorI6StringE, ptr null, i32 1) to i64)
  %call = call ptr @_ZN4Page8allocateEmm(ptr %1, i64 %mul, i64 8)
  %data = getelementptr inbounds %_Z6VectorI6VectorI6StringEE, ptr %0, i32 0, i32 1
  store ptr %call, ptr %data, align 8
  %field.inplace = getelementptr inbounds %_Z6VectorI6VectorI6StringEE, ptr %0, i32 0, i32 1
  %deref.recv = load ptr, ptr %field.inplace, align 8
  %mul1 = mul i64 %2, ptrtoint (ptr getelementptr (%_Z6VectorI6StringE, ptr null, i32 1) to i64)
  %call2 = call ptr @memset(ptr %deref.recv, i64 0, i64 %mul1)
  br label %if.end

if.else:                                          ; preds = %entry
  %data3 = getelementptr inbounds %_Z6VectorI6VectorI6StringEE, ptr %0, i32 0, i32 1
  store ptr null, ptr %data3, align 8
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  %if.value = phi ptr [ %call2, %if.then ], [ null, %if.else ]
  ret void
}

define linkonce_odr void @_ZN6VectorI6VectorI6StringEEC1EPN4scaly6memory4PageEP6VectorI6StringEm(ptr %0, ptr %1, ptr %2, i64 %3) {
entry:
  %length = getelementptr inbounds %_Z6VectorI6VectorI6StringEE, ptr %0, i32 0, i32 0
  store i64 %3, ptr %length, align 4
  %gt = icmp ugt i64 %3, 0
  br i1 %gt, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %mul = mul i64 %3, ptrtoint (ptr getelementptr (%_Z6VectorI6StringE, ptr null, i32 1) to i64)
  %call = call ptr @_ZN4Page8allocateEmm(ptr %1, i64 %mul, i64 8)
  %data = getelementptr inbounds %_Z6VectorI6VectorI6StringEE, ptr %0, i32 0, i32 1
  store ptr %call, ptr %data, align 8
  %field.inplace = getelementptr inbounds %_Z6VectorI6VectorI6StringEE, ptr %0, i32 0, i32 1
  %deref.recv = load ptr, ptr %field.inplace, align 8
  %mul1 = mul i64 %3, ptrtoint (ptr getelementptr (%_Z6VectorI6StringE, ptr null, i32 1) to i64)
  %call2 = call ptr @memcpy(ptr %deref.recv, ptr %2, i64 %mul1)
  br label %if.end

if.else:                                          ; preds = %entry
  %data3 = getelementptr inbounds %_Z6VectorI6VectorI6StringEE, ptr %0, i32 0, i32 1
  store ptr null, ptr %data3, align 8
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  %if.value = phi ptr [ %call2, %if.then ], [ null, %if.else ]
  ret void
}

define linkonce_odr void @_ZN6VectorI6VectorI6StringEEC1EPN4scaly6memory4PageE6VectorI6VectorI6StringEE(ptr %0, ptr %1, ptr %2) {
entry:
  %load.struct = load %_Z6VectorI6VectorI6StringEE, ptr %2, align 8
  %length = extractvalue %_Z6VectorI6VectorI6StringEE %load.struct, 0
  %length1 = getelementptr inbounds %_Z6VectorI6VectorI6StringEE, ptr %0, i32 0, i32 0
  store i64 %length, ptr %length1, align 4
  %load.struct2 = load %_Z6VectorI6VectorI6StringEE, ptr %0, align 8
  %length3 = extractvalue %_Z6VectorI6VectorI6StringEE %load.struct2, 0
  %gt = icmp ugt i64 %length3, 0
  br i1 %gt, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %load.struct4 = load %_Z6VectorI6VectorI6StringEE, ptr %0, align 8
  %length5 = extractvalue %_Z6VectorI6VectorI6StringEE %load.struct4, 0
  %mul = mul i64 %length5, ptrtoint (ptr getelementptr (%_Z6VectorI6StringE, ptr null, i32 1) to i64)
  %call = call ptr @_ZN4Page8allocateEmm(ptr %1, i64 %mul, i64 8)
  %data = getelementptr inbounds %_Z6VectorI6VectorI6StringEE, ptr %0, i32 0, i32 1
  store ptr %call, ptr %data, align 8
  %field.inplace = getelementptr inbounds %_Z6VectorI6VectorI6StringEE, ptr %0, i32 0, i32 1
  %deref.recv = load ptr, ptr %field.inplace, align 8
  %field.inplace6 = getelementptr inbounds %_Z6VectorI6VectorI6StringEE, ptr %2, i32 0, i32 1
  %deref.recv7 = load ptr, ptr %field.inplace6, align 8
  %load.struct8 = load %_Z6VectorI6VectorI6StringEE, ptr %0, align 8
  %length9 = extractvalue %_Z6VectorI6VectorI6StringEE %load.struct8, 0
  %mul10 = mul i64 %length9, ptrtoint (ptr getelementptr (%_Z6VectorI6StringE, ptr null, i32 1) to i64)
  %call11 = call ptr @memcpy(ptr %deref.recv, ptr %deref.recv7, i64 %mul10)
  br label %if.end

if.else:                                          ; preds = %entry
  %data12 = getelementptr inbounds %_Z6VectorI6VectorI6StringEE, ptr %0, i32 0, i32 1
  store ptr null, ptr %data12, align 8
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  %if.value = phi ptr [ %call11, %if.then ], [ null, %if.else ]
  ret void
}

define linkonce_odr ptr @_ZN5ArrayI6VectorI6StringEE10get_bufferEv(ptr %0) {
entry:
  ret ptr null
}

define linkonce_odr i64 @_ZN5ArrayI6VectorI6StringEE10get_lengthEv(ptr %0) {
entry:
  %load.struct = load %_Z5ArrayI6VectorI6StringEE, ptr %0, align 8
  %length = extractvalue %_Z5ArrayI6VectorI6StringEE %load.struct, 0
  ret i64 %length
}

define linkonce_odr i64 @_ZN5ArrayI6VectorI6StringEE12get_capacityEv(ptr %0) {
entry:
  ret i64 0
}

define linkonce_odr void @_ZN5ArrayI6VectorI6StringEE10reallocateEv(ptr %0) {
entry:
  %call = call ptr @_ZN4Page3getEPv(ptr %0)
  %length = alloca i64, align 8
  store i64 0, ptr %length, align 1
  ret void
}

define linkonce_odr void @_ZN5ArrayI6VectorI6StringEE3addE6VectorI6StringE(ptr %0, ptr %1) {
entry:
  br i1 false, label %lor.end, label %lor.rhs

lor.rhs:                                          ; preds = %entry
  %load.struct = load %_Z5ArrayI6VectorI6StringEE, ptr %0, align 8
  %length = extractvalue %_Z5ArrayI6VectorI6StringEE %load.struct, 0
  br label %lor.end

lor.end:                                          ; preds = %lor.rhs, %entry
  %lor.result = phi i1 [ true, %entry ], [ true, %lor.rhs ]
  br i1 %lor.result, label %if.then, label %if.end

if.then:                                          ; preds = %lor.end
  call void @_ZN5ArrayI6VectorI6StringEE10reallocateEv(ptr %0)
  br label %if.end

if.end:                                           ; preds = %if.then, %lor.end
  %load.struct1 = load %_Z5ArrayI6VectorI6StringEE, ptr %0, align 8
  %length2 = extractvalue %_Z5ArrayI6VectorI6StringEE %load.struct1, 0
  %load.struct3 = load %_Z5ArrayI6VectorI6StringEE, ptr %0, align 8
  %length4 = extractvalue %_Z5ArrayI6VectorI6StringEE %load.struct3, 0
  %add = add i64 %length4, 1
  %length5 = getelementptr inbounds %_Z5ArrayI6VectorI6StringEE, ptr %0, i32 0, i32 0
  store i64 %add, ptr %length5, align 4
  ret void
}

define linkonce_odr void @_ZN5ArrayI6VectorI6StringEE3addE6VectorI6VectorI6StringEE(ptr %0, ptr %1) {
entry:
  %load.struct = load %_Z5ArrayI6VectorI6StringEE, ptr %0, align 8
  %length = extractvalue %_Z5ArrayI6VectorI6StringEE %load.struct, 0
  %load.struct1 = load %_Z6VectorI6VectorI6StringEE, ptr %1, align 8
  %length2 = extractvalue %_Z6VectorI6VectorI6StringEE %load.struct1, 0
  %add = add i64 %length, %length2
  %new_length = alloca i64, align 8
  store i64 %add, ptr %new_length, align 1
  %new_length3 = load i64, ptr %new_length, align 4
  %load.struct4 = load %_Z5ArrayI6VectorI6StringEE, ptr %0, align 8
  %length5 = extractvalue %_Z5ArrayI6VectorI6StringEE %load.struct4, 0
  %lt = icmp ult i64 %new_length3, %length5
  br i1 %lt, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  call void @exit(i64 14)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  br i1 false, label %lor.end, label %lor.rhs

lor.rhs:                                          ; preds = %if.end
  %new_length6 = load i64, ptr %new_length, align 4
  br label %lor.end

lor.end:                                          ; preds = %lor.rhs, %if.end
  %lor.result = phi i1 [ true, %if.end ], [ true, %lor.rhs ]
  br i1 %lor.result, label %if.then7, label %if.end8

if.then7:                                         ; preds = %lor.end
  call void @_ZN5ArrayI6VectorI6StringEE10reallocateEv(ptr %0)
  br label %if.end8

if.end8:                                          ; preds = %if.then7, %lor.end
  %new_length9 = load i64, ptr %new_length, align 4
  %load.struct10 = load %_Z6VectorI6VectorI6StringEE, ptr %1, align 8
  %length11 = extractvalue %_Z6VectorI6VectorI6StringEE %load.struct10, 0
  %gt = icmp ugt i64 %length11, 0
  br i1 %gt, label %if.then12, label %if.end13

if.then12:                                        ; preds = %if.end8
  %load.struct14 = load %_Z5ArrayI6VectorI6StringEE, ptr %0, align 8
  %length15 = extractvalue %_Z5ArrayI6VectorI6StringEE %load.struct14, 0
  %mul = mul i64 %length15, ptrtoint (ptr getelementptr (%_Z6VectorI6StringE, ptr null, i32 1) to i64)
  br label %if.end13

if.end13:                                         ; preds = %if.then12, %if.end8
  %load.struct16 = load %_Z5ArrayI6VectorI6StringEE, ptr %0, align 8
  %length17 = extractvalue %_Z5ArrayI6VectorI6StringEE %load.struct16, 0
  %load.struct18 = load %_Z6VectorI6VectorI6StringEE, ptr %1, align 8
  %length19 = extractvalue %_Z6VectorI6VectorI6StringEE %load.struct18, 0
  %add20 = add i64 %length17, %length19
  %length21 = getelementptr inbounds %_Z5ArrayI6VectorI6StringEE, ptr %0, i32 0, i32 0
  store i64 %add20, ptr %length21, align 4
  ret void
}

define linkonce_odr ptr @_ZN5ArrayI6VectorI6StringEE3getEPN4scaly6memory4PageEm(ptr %0, ptr %1, i64 %2) {
entry:
  %load.struct = load %_Z5ArrayI6VectorI6StringEE, ptr %1, align 8
  %length = extractvalue %_Z5ArrayI6VectorI6StringEE %load.struct, 0
  %ge = icmp uge i64 %2, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  ret ptr null
}

define linkonce_odr void @_ZN5ArrayI6VectorI6StringEE3putEm6VectorI6StringE(ptr %0, i64 %1, ptr %2) {
entry:
  %load.struct = load %_Z5ArrayI6VectorI6StringEE, ptr %0, align 8
  %length = extractvalue %_Z5ArrayI6VectorI6StringEE %load.struct, 0
  %ge = icmp uge i64 %1, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  call void @_Z11scaly_eputsP10const_char(ptr @.str.16)
  call void @_Z12scaly_eputnlv()
  call void @exit(i64 15)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  ret void
}

define linkonce_odr ptr @_ZN13ArrayIteratorI6VectorI6StringEE4nextEv(ptr %0) {
entry:
  %load.struct = load %_Z13ArrayIteratorI6VectorI6StringEE, ptr %0, align 8
  %position = extractvalue %_Z13ArrayIteratorI6VectorI6StringEE %load.struct, 1
  %load.struct1 = load %_Z13ArrayIteratorI6VectorI6StringEE, ptr %0, align 8
  %position2 = extractvalue %_Z13ArrayIteratorI6VectorI6StringEE %load.struct1, 1
  %add = add i64 %position2, 1
  %position3 = getelementptr inbounds %_Z13ArrayIteratorI6VectorI6StringEE, ptr %0, i32 0, i32 1
  store i64 %add, ptr %position3, align 4
  %load.struct4 = load %_Z13ArrayIteratorI6VectorI6StringEE, ptr %0, align 8
  %position5 = extractvalue %_Z13ArrayIteratorI6VectorI6StringEE %load.struct4, 1
  %sub = sub i64 %position5, 1
  ret ptr null
}

define linkonce_odr void @_ZN13ArrayIteratorI6VectorI6StringEEC1EP5ArrayI6VectorI6StringEE(ptr %0, ptr %1) {
entry:
  %position = getelementptr inbounds %_Z13ArrayIteratorI6VectorI6StringEE, ptr %0, i32 0, i32 1
  store i64 0, ptr %position, align 4
  ret void
}

define linkonce_odr void @_ZN13ArrayIteratorI6VectorI6StringEEC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr void @_ZN5ArrayI6VectorI6StringEE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z13ArrayIteratorI6VectorI6StringEE) %0, ptr %1, ptr %2) {
entry:
  %struct.init = alloca %_Z13ArrayIteratorI6VectorI6StringEE, align 8
  call void @_ZN13ArrayIteratorI6VectorI6StringEEC1EP5ArrayI6VectorI6StringEE(ptr %struct.init, ptr %2)
  %sret.body = load %_Z13ArrayIteratorI6VectorI6StringEE, ptr %struct.init, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %struct.init, i64 ptrtoint (ptr getelementptr (%_Z13ArrayIteratorI6VectorI6StringEE, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN5ArrayI6VectorI6StringEEC1Ev(ptr %0) {
entry:
  %length = getelementptr inbounds %_Z5ArrayI6VectorI6StringEE, ptr %0, i32 0, i32 0
  store i64 0, ptr %length, align 4
  ret void
}

define linkonce_odr void @_ZN5ArrayI6VectorI6StringEEC1Em(ptr %0, i64 %1) {
entry:
  %length = getelementptr inbounds %_Z5ArrayI6VectorI6StringEE, ptr %0, i32 0, i32 0
  store i64 0, ptr %length, align 4
  %call = call ptr @_ZN4Page3getEPv(ptr %0)
  %call1 = call ptr @_ZN4Page23allocate_exclusive_pageEv(ptr %call)
  %struct.region = call ptr @_ZN4Page8allocateEmm(ptr %call1, i64 ptrtoint (ptr getelementptr (%_Z6VectorI6VectorI6StringEE, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr ({ i1, %_Z6VectorI6VectorI6StringEE }, ptr null, i64 0, i32 1) to i64))
  call void @_ZN6VectorI6VectorI6StringEEC1EPN4scaly6memory4PageEm(ptr %struct.region, ptr %call1, i64 %1)
  ret void
}

define linkonce_odr void @_ZN5ArrayI6VectorI6StringEEC1EPN4scaly6memory4PageE(ptr %0, ptr %1) {
entry:
  %length = getelementptr inbounds %_Z5ArrayI6VectorI6StringEE, ptr %0, i32 0, i32 0
  store i64 0, ptr %length, align 4
  ret void
}

define linkonce_odr void @_ZN5ArrayI6VectorI6StringEEC1EPN4scaly6memory4PageEm(ptr %0, ptr %1, i64 %2) {
entry:
  %length = getelementptr inbounds %_Z5ArrayI6VectorI6StringEE, ptr %0, i32 0, i32 0
  store i64 0, ptr %length, align 4
  %call = call ptr @_ZN4Page23allocate_exclusive_pageEv(ptr %1)
  %struct.region = call ptr @_ZN4Page8allocateEmm(ptr %call, i64 ptrtoint (ptr getelementptr (%_Z6VectorI6VectorI6StringEE, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr ({ i1, %_Z6VectorI6VectorI6StringEE }, ptr null, i64 0, i32 1) to i64))
  call void @_ZN6VectorI6VectorI6StringEEC1EPN4scaly6memory4PageEm(ptr %struct.region, ptr %call, i64 %2)
  ret void
}

define linkonce_odr void @_ZN6VectorI6VectorI6StringEEC1EPN4scaly6memory4PageE5ArrayI6VectorI6StringEE(ptr %0, ptr %1, ptr %2) {
entry:
  %load.struct = load %_Z5ArrayI6VectorI6StringEE, ptr %2, align 8
  %length = extractvalue %_Z5ArrayI6VectorI6StringEE %load.struct, 0
  %length1 = getelementptr inbounds %_Z6VectorI6VectorI6StringEE, ptr %0, i32 0, i32 0
  store i64 %length, ptr %length1, align 4
  %load.struct2 = load %_Z6VectorI6VectorI6StringEE, ptr %0, align 8
  %length3 = extractvalue %_Z6VectorI6VectorI6StringEE %load.struct2, 0
  %gt = icmp ugt i64 %length3, 0
  br i1 %gt, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %load.struct4 = load %_Z6VectorI6VectorI6StringEE, ptr %0, align 8
  %length5 = extractvalue %_Z6VectorI6VectorI6StringEE %load.struct4, 0
  %mul = mul i64 %length5, ptrtoint (ptr getelementptr (%_Z6VectorI6StringE, ptr null, i32 1) to i64)
  %call = call ptr @_ZN4Page8allocateEmm(ptr %1, i64 %mul, i64 8)
  %data = getelementptr inbounds %_Z6VectorI6VectorI6StringEE, ptr %0, i32 0, i32 1
  store ptr %call, ptr %data, align 8
  %load.struct6 = load %_Z5ArrayI6VectorI6StringEE, ptr %2, align 8
  %vector = extractvalue %_Z5ArrayI6VectorI6StringEE %load.struct6, 1
  %field.inplace = getelementptr inbounds %_Z6VectorI6VectorI6StringEE, ptr %0, i32 0, i32 1
  %deref.recv = load ptr, ptr %field.inplace, align 8
  %field.inplace7 = getelementptr inbounds %_Z6VectorI6VectorI6StringEE, ptr %vector, i32 0, i32 1
  %deref.recv8 = load ptr, ptr %field.inplace7, align 8
  %load.struct9 = load %_Z6VectorI6VectorI6StringEE, ptr %0, align 8
  %length10 = extractvalue %_Z6VectorI6VectorI6StringEE %load.struct9, 0
  %mul11 = mul i64 %length10, ptrtoint (ptr getelementptr (%_Z6VectorI6StringE, ptr null, i32 1) to i64)
  %call12 = call ptr @memcpy(ptr %deref.recv, ptr %deref.recv8, i64 %mul11)
  br label %if.end

if.else:                                          ; preds = %entry
  %data13 = getelementptr inbounds %_Z6VectorI6VectorI6StringEE, ptr %0, i32 0, i32 1
  store ptr null, ptr %data13, align 8
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  %if.value = phi ptr [ %call12, %if.then ], [ null, %if.else ]
  ret void
}

define linkonce_odr ptr @_ZN4ListI6VectorI6StringEE8get_headEPN4scaly6memory4PageE(ptr %0, ptr %1) {
entry:
  %load.struct = load %_Z4ListI6VectorI6StringEE, ptr %1, align 8
  %head = extractvalue %_Z4ListI6VectorI6StringEE %load.struct, 0
  %eq = icmp eq ptr %head, null
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %addr.gep = getelementptr inbounds %_Z4ListI6VectorI6StringEE, ptr %1, i32 0, i32 0
  %addr.gep1 = getelementptr inbounds %_Z4NodeI6VectorI6StringEE, ptr %addr.gep, i32 0, i32 0
  ret ptr %addr.gep1
}

define linkonce_odr ptr @_ZN12ListIteratorI6VectorI6StringEE4nextEv(ptr %0) {
entry:
  %old_current = alloca ptr, align 8
  %load.struct = load %_Z12ListIteratorI6VectorI6StringEE, ptr %0, align 8
  %current = extractvalue %_Z12ListIteratorI6VectorI6StringEE %load.struct, 0
  %ne = icmp ne ptr %current, null
  br i1 %ne, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %load.struct1 = load %_Z12ListIteratorI6VectorI6StringEE, ptr %0, align 8
  %current2 = extractvalue %_Z12ListIteratorI6VectorI6StringEE %load.struct1, 0
  store ptr %current2, ptr %old_current, align 1
  %load.struct3 = load %_Z12ListIteratorI6VectorI6StringEE, ptr %0, align 8
  %current4 = extractvalue %_Z12ListIteratorI6VectorI6StringEE %load.struct3, 0
  %deref = load %_Z4NodeI6VectorI6StringEE, ptr %current4, align 8
  %next = extractvalue %_Z4NodeI6VectorI6StringEE %deref, 1
  %current5 = getelementptr inbounds %_Z12ListIteratorI6VectorI6StringEE, ptr %0, i32 0, i32 0
  store ptr %next, ptr %current5, align 8
  %old_current6 = load ptr, ptr %old_current, align 8
  %addr.gep = getelementptr inbounds %_Z4NodeI6VectorI6StringEE, ptr %old_current6, i32 0, i32 0
  ret ptr %addr.gep

if.else:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; No predecessors!
  ret ptr null
}

define linkonce_odr void @_ZN12ListIteratorI6VectorI6StringEEC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr i64 @_ZN4ListI6VectorI6StringEE5countEv(ptr %0) {
entry:
  %local_page = call ptr @_Z21scaly_alloc_root_pagev()
  %sret.result = alloca %_Z12ListIteratorI6VectorI6StringEE, align 8
  call void @_ZN4ListI6VectorI6StringEE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z12ListIteratorI6VectorI6StringEE) %sret.result, ptr %local_page, ptr %0)
  %list_iterator = alloca ptr, align 8
  store ptr %sret.result, ptr %list_iterator, align 1
  %i = alloca i64, align 8
  store i64 0, ptr %i, align 1
  br label %while.cond

while.cond:                                       ; preds = %while.body, %entry
  %list_iterator1 = load ptr, ptr %list_iterator, align 8
  %call = call ptr @_ZN12ListIteratorI6VectorI6StringEE4nextEv(ptr %list_iterator1)
  %ne = icmp ne ptr %call, null
  br i1 %ne, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %i2 = load i64, ptr %i, align 4
  %add = add i64 %i2, 1
  store i64 %add, ptr %i, align 1
  br label %while.cond

while.exit:                                       ; preds = %while.cond
  %i3 = load i64, ptr %i, align 4
  call void @_Z28scaly_release_root_page_fullP4Page(ptr %local_page)
  ret i64 %i3
}

define linkonce_odr i1 @_ZN4ListI6VectorI6StringEE6removeE6VectorI6StringE(ptr %0, ptr %1) {
entry:
  %load.struct = load %_Z4ListI6VectorI6StringEE, ptr %0, align 8
  %head = extractvalue %_Z4ListI6VectorI6StringEE %load.struct, 0
  %node = alloca ptr, align 8
  store ptr %head, ptr %node, align 1
  %previous_node = alloca ptr, align 8
  store ptr null, ptr %previous_node, align 1
  br label %while.cond

while.cond:                                       ; preds = %while.body, %entry
  %node1 = load ptr, ptr %node, align 8
  %ne = icmp ne ptr %node1, null
  br i1 %ne, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %node2 = load ptr, ptr %node, align 8
  %load.struct3 = load %_Z4NodeI6VectorI6StringEE, ptr %node2, align 8
  %element = extractvalue %_Z4NodeI6VectorI6StringEE %load.struct3, 0
  %node4 = load ptr, ptr %node, align 8
  store ptr %node4, ptr %previous_node, align 1
  %node5 = load ptr, ptr %node, align 8
  %load.struct6 = load %_Z4NodeI6VectorI6StringEE, ptr %node5, align 8
  %next = extractvalue %_Z4NodeI6VectorI6StringEE %load.struct6, 1
  store ptr %next, ptr %node, align 1
  br label %while.cond

while.exit:                                       ; preds = %while.cond
  ret i1 false
}

define linkonce_odr void @_ZN4ListI6VectorI6StringEE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z12ListIteratorI6VectorI6StringEE) %0, ptr %1, ptr %2) {
entry:
  %load.struct = load %_Z4ListI6VectorI6StringEE, ptr %2, align 8
  %head = extractvalue %_Z4ListI6VectorI6StringEE %load.struct, 0
  %tuple = alloca %_Z12ListIteratorI6VectorI6StringEE, align 8
  %tuple.field = getelementptr inbounds %_Z12ListIteratorI6VectorI6StringEE, ptr %tuple, i32 0, i32 0
  store ptr %head, ptr %tuple.field, align 1
  %tuple.val = load %_Z12ListIteratorI6VectorI6StringEE, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z12ListIteratorI6VectorI6StringEE, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN4ListI6VectorI6StringEE3addE6VectorI6StringE(ptr %0, ptr %1) {
entry:
  %load.struct = load %_Z4ListI6VectorI6StringEE, ptr %0, align 8
  %head = extractvalue %_Z4ListI6VectorI6StringEE %load.struct, 0
  %own_page = call ptr @_Z3getPv(ptr %0)
  %tuple.region = call ptr @_ZN4Page8allocateEmm(ptr %own_page, i64 ptrtoint (ptr getelementptr (%_Z4NodeI6VectorI6StringEE, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr ({ i1, %_Z4NodeI6VectorI6StringEE }, ptr null, i64 0, i32 1) to i64))
  %field.load = load %_Z6VectorI6StringE, ptr %1, align 8
  %tuple.field = getelementptr inbounds %_Z4NodeI6VectorI6StringEE, ptr %tuple.region, i32 0, i32 0
  store %_Z6VectorI6StringE %field.load, ptr %tuple.field, align 1
  %tuple.field1 = getelementptr inbounds %_Z4NodeI6VectorI6StringEE, ptr %tuple.region, i32 0, i32 1
  store ptr %head, ptr %tuple.field1, align 1
  %head2 = getelementptr inbounds %_Z4ListI6VectorI6StringEE, ptr %0, i32 0, i32 0
  store ptr %tuple.region, ptr %head2, align 8
  ret void
}

define linkonce_odr void @_ZN4ListI6VectorI6StringEEC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr void @_ZN6VectorI6VectorI6StringEEC1EPN4scaly6memory4PageE4ListI6VectorI6StringEE(ptr %0, ptr %1, ptr %2) {
entry:
  %deref.tmp = alloca %_Z6VectorI6StringE, align 8
  %i = alloca i64, align 8
  %list_iterator = alloca ptr, align 8
  %local_page = call ptr @_Z21scaly_alloc_root_pagev()
  %sret.result = alloca %_Z12ListIteratorI6VectorI6StringEE, align 8
  %call = call i64 @_ZN4ListI6VectorI6StringEE5countEv(ptr %2)
  %length = getelementptr inbounds %_Z6VectorI6VectorI6StringEE, ptr %0, i32 0, i32 0
  store i64 %call, ptr %length, align 4
  %load.struct = load %_Z6VectorI6VectorI6StringEE, ptr %0, align 8
  %length1 = extractvalue %_Z6VectorI6VectorI6StringEE %load.struct, 0
  %gt = icmp ugt i64 %length1, 0
  br i1 %gt, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %load.struct2 = load %_Z6VectorI6VectorI6StringEE, ptr %0, align 8
  %length3 = extractvalue %_Z6VectorI6VectorI6StringEE %load.struct2, 0
  %mul = mul i64 %length3, ptrtoint (ptr getelementptr (%_Z6VectorI6StringE, ptr null, i32 1) to i64)
  %call4 = call ptr @_ZN4Page8allocateEmm(ptr %1, i64 %mul, i64 8)
  %data = getelementptr inbounds %_Z6VectorI6VectorI6StringEE, ptr %0, i32 0, i32 1
  store ptr %call4, ptr %data, align 8
  call void @_ZN4ListI6VectorI6StringEE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z12ListIteratorI6VectorI6StringEE) %sret.result, ptr %local_page, ptr %2)
  store ptr %sret.result, ptr %list_iterator, align 1
  %load.struct5 = load %_Z6VectorI6VectorI6StringEE, ptr %0, align 8
  %length6 = extractvalue %_Z6VectorI6VectorI6StringEE %load.struct5, 0
  store i64 %length6, ptr %i, align 1
  br label %while.cond

if.else:                                          ; preds = %entry
  %data13 = getelementptr inbounds %_Z6VectorI6VectorI6StringEE, ptr %0, i32 0, i32 1
  store ptr null, ptr %data13, align 8
  br label %if.end

if.end:                                           ; preds = %if.else, %while.exit
  call void @_Z28scaly_release_root_page_fullP4Page(ptr %local_page)
  ret void

while.cond:                                       ; preds = %while.body, %if.then
  %list_iterator7 = load ptr, ptr %list_iterator, align 8
  %call8 = call ptr @_ZN12ListIteratorI6VectorI6StringEE4nextEv(ptr %list_iterator7)
  %while.tobool = icmp ne ptr %call8, null
  br i1 %while.tobool, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %i9 = load i64, ptr %i, align 4
  %sub = sub i64 %i9, 1
  store i64 %sub, ptr %i, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %deref.tmp, ptr align 1 %call8, i64 ptrtoint (ptr getelementptr (%_Z6VectorI6StringE, ptr null, i32 1) to i64), i1 false)
  %load.struct10 = load %_Z6VectorI6VectorI6StringEE, ptr %0, align 8
  %data11 = extractvalue %_Z6VectorI6VectorI6StringEE %load.struct10, 1
  %i12 = load i64, ptr %i, align 4
  %ptr.add = getelementptr inbounds %_Z6VectorI6StringE, ptr %data11, i64 %i12
  %store.load = load %_Z6VectorI6StringE, ptr %deref.tmp, align 8
  store %_Z6VectorI6StringE %store.load, ptr %ptr.add, align 8
  br label %while.cond

while.exit:                                       ; preds = %while.cond
  br label %if.end
}

define linkonce_odr i1 @_ZN7HashSetI6StringE8containsE6String(ptr %0, ptr %1) {
entry:
  %deref.tmp14 = alloca %_Z6String, align 8
  %i = alloca i64, align 8
  %deref.tmp = alloca %_Z6VectorI6StringE, align 8
  %load.struct = load %_Z7HashSetI6StringE, ptr %0, align 8
  %slots = extractvalue %_Z7HashSetI6StringE %load.struct, 0
  %eq = icmp eq ptr %slots, null
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret i1 false

if.end:                                           ; preds = %entry
  %call = call i64 @_ZN6String4hashEv(ptr %1)
  %load.struct1 = load %_Z7HashSetI6StringE, ptr %0, align 8
  %slots2 = extractvalue %_Z7HashSetI6StringE %load.struct1, 0
  %deref = load %_Z6VectorI6VectorI6StringEE, ptr %slots2, align 8
  %length = extractvalue %_Z6VectorI6VectorI6StringEE %deref, 0
  %urem = urem i64 %call, %length
  %field.inplace = getelementptr inbounds %_Z7HashSetI6StringE, ptr %0, i32 0, i32 0
  %deref.recv = load ptr, ptr %field.inplace, align 8
  %call3 = call ptr @_ZN6VectorI6VectorI6StringEE7get_ptrEm(ptr %deref.recv, i64 %urem)
  %eq4 = icmp eq ptr %call3, null
  br i1 %eq4, label %if.then5, label %if.end6

if.then5:                                         ; preds = %if.end
  ret i1 false

if.end6:                                          ; preds = %if.end
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %deref.tmp, ptr align 1 %call3, i64 ptrtoint (ptr getelementptr (%_Z6VectorI6StringE, ptr null, i32 1) to i64), i1 false)
  %load.struct7 = load %_Z6VectorI6StringE, ptr %deref.tmp, align 8
  %length8 = extractvalue %_Z6VectorI6StringE %load.struct7, 0
  store i64 0, ptr %i, align 1
  br label %while.cond

while.cond:                                       ; preds = %if.end13, %if.end6
  %i9 = load i64, ptr %i, align 4
  %lt = icmp ult i64 %i9, %length8
  br i1 %lt, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %i10 = load i64, ptr %i, align 4
  %call11 = call ptr @_ZN6VectorI6StringE7get_ptrEm(ptr %deref.tmp, i64 %i10)
  %ne = icmp ne ptr %call11, null
  br i1 %ne, label %if.then12, label %if.end13

while.exit:                                       ; preds = %while.cond
  ret i1 false

if.then12:                                        ; preds = %while.body
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %deref.tmp14, ptr align 1 %call11, i64 ptrtoint (ptr getelementptr (%_Z6String, ptr null, i32 1) to i64), i1 false)
  %call15 = call i1 @_ZN6String6equalsE6String(ptr %1, ptr %deref.tmp14)
  br i1 %call15, label %if.then16, label %if.end17

if.end13:                                         ; preds = %if.end17, %while.body
  %i18 = load i64, ptr %i, align 4
  %add = add i64 %i18, 1
  store i64 %add, ptr %i, align 1
  br label %while.cond

if.then16:                                        ; preds = %if.then12
  ret i1 true

if.end17:                                         ; preds = %if.then12
  br label %if.end13
}

define linkonce_odr ptr @_ZN12ListIteratorI4SlotI6StringEE4nextEv(ptr %0) {
entry:
  %old_current = alloca ptr, align 8
  %load.struct = load %_Z12ListIteratorI4SlotI6StringEE, ptr %0, align 8
  %current = extractvalue %_Z12ListIteratorI4SlotI6StringEE %load.struct, 0
  %ne = icmp ne ptr %current, null
  br i1 %ne, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %load.struct1 = load %_Z12ListIteratorI4SlotI6StringEE, ptr %0, align 8
  %current2 = extractvalue %_Z12ListIteratorI4SlotI6StringEE %load.struct1, 0
  store ptr %current2, ptr %old_current, align 1
  %load.struct3 = load %_Z12ListIteratorI4SlotI6StringEE, ptr %0, align 8
  %current4 = extractvalue %_Z12ListIteratorI4SlotI6StringEE %load.struct3, 0
  %deref = load %_Z4NodeI4SlotI6StringEE, ptr %current4, align 8
  %next = extractvalue %_Z4NodeI4SlotI6StringEE %deref, 1
  %current5 = getelementptr inbounds %_Z12ListIteratorI4SlotI6StringEE, ptr %0, i32 0, i32 0
  store ptr %next, ptr %current5, align 8
  %old_current6 = load ptr, ptr %old_current, align 8
  %addr.gep = getelementptr inbounds %_Z4NodeI4SlotI6StringEE, ptr %old_current6, i32 0, i32 0
  ret ptr %addr.gep

if.else:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; No predecessors!
  ret ptr null
}

define linkonce_odr void @_ZN12ListIteratorI4SlotI6StringEEC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr void @_ZN7HashSetI6StringEC1EPN4scaly6memory4PageE14HashSetBuilderI6StringE(ptr %0, ptr %1, ptr %2) {
entry:
  %deref.tmp = alloca %_Z5ArrayI6StringE, align 8
  %list_iterator = alloca %_Z12ListIteratorI4SlotI6StringEE, align 8
  %tuple = alloca %_Z12ListIteratorI4SlotI6StringEE, align 8
  %array = alloca ptr, align 8
  %local_page = call ptr @_Z21scaly_alloc_root_pagev()
  %i = alloca i64, align 8
  %length9 = alloca i64, align 8
  %load.struct = load %_Z14HashSetBuilderI6StringE, ptr %2, align 8
  %length = extractvalue %_Z14HashSetBuilderI6StringE %load.struct, 0
  %eq = icmp eq i64 %length, 0
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %slots = getelementptr inbounds %_Z7HashSetI6StringE, ptr %0, i32 0, i32 0
  store ptr null, ptr %slots, align 8
  call void @_Z23scaly_release_root_pageP4Page(ptr %local_page)
  ret void

if.end:                                           ; preds = %entry
  %struct.region = call ptr @_ZN4Page8allocateEmm(ptr %1, i64 ptrtoint (ptr getelementptr (%_Z6VectorI6VectorI6StringEE, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr ({ i1, %_Z6VectorI6VectorI6StringEE }, ptr null, i64 0, i32 1) to i64))
  %load.struct1 = load %_Z14HashSetBuilderI6StringE, ptr %2, align 8
  %slots2 = extractvalue %_Z14HashSetBuilderI6StringE %load.struct1, 1
  %deref = load %_Z6VectorI11BuilderListI4SlotI6StringEEE, ptr %slots2, align 8
  %length3 = extractvalue %_Z6VectorI11BuilderListI4SlotI6StringEEE %deref, 0
  call void @_ZN6VectorI6VectorI6StringEEC1EPN4scaly6memory4PageEm(ptr %struct.region, ptr %1, i64 %length3)
  %slots4 = getelementptr inbounds %_Z7HashSetI6StringE, ptr %0, i32 0, i32 0
  store ptr %struct.region, ptr %slots4, align 8
  %load.struct5 = load %_Z14HashSetBuilderI6StringE, ptr %2, align 8
  %slots6 = extractvalue %_Z14HashSetBuilderI6StringE %load.struct5, 1
  %deref7 = load %_Z6VectorI11BuilderListI4SlotI6StringEEE, ptr %slots6, align 8
  %length8 = extractvalue %_Z6VectorI11BuilderListI4SlotI6StringEEE %deref7, 0
  store i64 %length8, ptr %length9, align 1
  store i64 0, ptr %i, align 1
  br label %while.cond

while.cond:                                       ; preds = %if.end14, %if.end
  %i10 = load i64, ptr %i, align 4
  %length11 = load i64, ptr %length9, align 4
  %lt = icmp ult i64 %i10, %length11
  br i1 %lt, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %field.inplace = getelementptr inbounds %_Z14HashSetBuilderI6StringE, ptr %2, i32 0, i32 1
  %deref.recv = load ptr, ptr %field.inplace, align 8
  %i12 = load i64, ptr %i, align 4
  %call = call ptr @_ZN6VectorI11BuilderListI4SlotI6StringEEE7get_ptrEm(ptr %deref.recv, i64 %i12)
  %ne = icmp ne ptr %call, null
  br i1 %ne, label %if.then13, label %if.end14

while.exit:                                       ; preds = %while.cond
  call void @_Z28scaly_release_root_page_fullP4Page(ptr %local_page)
  ret void

if.then13:                                        ; preds = %while.body
  %struct.region15 = call ptr @_ZN4Page8allocateEmm(ptr %local_page, i64 ptrtoint (ptr getelementptr (%_Z5ArrayI6StringE, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr ({ i1, %_Z5ArrayI6StringE }, ptr null, i64 0, i32 1) to i64))
  %tuple.field = getelementptr inbounds %_Z5ArrayI6StringE, ptr %struct.region15, i32 0, i32 0
  store i64 0, ptr %tuple.field, align 4
  %tuple.field16 = getelementptr inbounds %_Z5ArrayI6StringE, ptr %struct.region15, i32 0, i32 1
  store ptr null, ptr %tuple.field16, align 8
  store ptr %struct.region15, ptr %array, align 1
  %load.struct17 = load %_Z11BuilderListI4SlotI6StringEE, ptr %call, align 8
  %head = extractvalue %_Z11BuilderListI4SlotI6StringEE %load.struct17, 0
  %tuple.field18 = getelementptr inbounds %_Z12ListIteratorI4SlotI6StringEE, ptr %tuple, i32 0, i32 0
  store ptr %head, ptr %tuple.field18, align 1
  %tuple.val = load %_Z12ListIteratorI4SlotI6StringEE, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %list_iterator, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z12ListIteratorI4SlotI6StringEE, ptr null, i32 1) to i64), i1 false)
  br label %while.cond19

if.end14:                                         ; preds = %if.end29, %while.body
  %i35 = load i64, ptr %i, align 4
  %add = add i64 %i35, 1
  store i64 %add, ptr %i, align 1
  br label %while.cond

while.cond19:                                     ; preds = %while.body20, %if.then13
  %call22 = call ptr @_ZN12ListIteratorI4SlotI6StringEE4nextEv(ptr %list_iterator)
  %while.tobool = icmp ne ptr %call22, null
  br i1 %while.tobool, label %while.body20, label %while.exit21

while.body20:                                     ; preds = %while.cond19
  %array23 = load ptr, ptr %array, align 8
  %field.inplace24 = getelementptr inbounds %_Z4SlotI6StringE, ptr %call22, i32 0, i32 0
  call void @_ZN5ArrayI6StringE3addE6String(ptr %array23, ptr %field.inplace24)
  br label %while.cond19

while.exit21:                                     ; preds = %while.cond19
  %array25 = load ptr, ptr %array, align 8
  %load.struct26 = load %_Z5ArrayI6StringE, ptr %array25, align 8
  %length27 = extractvalue %_Z5ArrayI6StringE %load.struct26, 0
  %gt = icmp ugt i64 %length27, 0
  br i1 %gt, label %if.then28, label %if.end29

if.then28:                                        ; preds = %while.exit21
  %field.inplace30 = getelementptr inbounds %_Z7HashSetI6StringE, ptr %0, i32 0, i32 0
  %deref.recv31 = load ptr, ptr %field.inplace30, align 8
  %i32 = load i64, ptr %i, align 4
  %struct.region33 = call ptr @_ZN4Page8allocateEmm(ptr %1, i64 ptrtoint (ptr getelementptr (%_Z6VectorI6StringE, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr ({ i1, %_Z6VectorI6StringE }, ptr null, i64 0, i32 1) to i64))
  %array34 = load ptr, ptr %array, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %deref.tmp, ptr align 1 %array34, i64 ptrtoint (ptr getelementptr (%_Z5ArrayI6StringE, ptr null, i32 1) to i64), i1 false)
  call void @_ZN6VectorI6StringEC1EPN4scaly6memory4PageE5ArrayI6StringE(ptr %struct.region33, ptr %1, ptr %deref.tmp)
  call void @_ZN6VectorI6VectorI6StringEE3putEm6VectorI6StringE(ptr %deref.recv31, i64 %i32, ptr %struct.region33)
  br label %if.end29

if.end29:                                         ; preds = %if.then28, %while.exit21
  br label %if.end14
}

define linkonce_odr void @_ZN7HashSetI6StringEC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr void @_ZN11BuilderListI1TE3addE1T(ptr %0, ptr %1) {
entry:
  ret void
}

define linkonce_odr i1 @_ZN11BuilderListI1TE6removeE1T(ptr %0, ptr %1) {
entry:
  ret i1 false
}

define linkonce_odr ptr @_ZN11BuilderListI1TE8get_headEPN4scaly6memory4PageE(ptr %0, ptr %1) {
entry:
  ret ptr null
}

define linkonce_odr ptr @_ZN19BuilderListIteratorI1TE4nextEv(ptr %0) {
entry:
  ret ptr null
}

define linkonce_odr void @_ZN19BuilderListIteratorI1TEC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr i64 @_ZN11BuilderListI1TE5countEv(ptr %0) {
entry:
  ret i64 0
}

define linkonce_odr void @_ZN11BuilderListI1TE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z19BuilderListIteratorI1TE) %0, ptr %1, ptr %2) {
entry:
  ret void
}

define linkonce_odr void @_ZN11BuilderListI1TEC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr ptr @_ZN6VectorI11BuilderListI4SlotI1TEEE3getEPN4scaly6memory4PageEm(ptr %0, ptr %1, i64 %2) {
entry:
  ret ptr null
}

define linkonce_odr ptr @_ZN6VectorI11BuilderListI4SlotI1TEEE7get_ptrEm(ptr %0, i64 %1) {
entry:
  ret ptr null
}

define linkonce_odr void @_ZN6VectorI11BuilderListI4SlotI1TEEE3putEm11BuilderListI4SlotI1TEE(ptr %0, i64 %1, ptr %2) {
entry:
  ret void
}

define linkonce_odr void @_ZN11BuilderListI11BuilderListI4SlotI1TEEE3addE11BuilderListI4SlotI1TEE(ptr %0, ptr %1) {
entry:
  ret void
}

define linkonce_odr i1 @_ZN11BuilderListI11BuilderListI4SlotI1TEEE6removeE11BuilderListI4SlotI1TEE(ptr %0, ptr %1) {
entry:
  ret i1 false
}

define linkonce_odr ptr @_ZN11BuilderListI11BuilderListI4SlotI1TEEE8get_headEPN4scaly6memory4PageE(ptr %0, ptr %1) {
entry:
  ret ptr null
}

define linkonce_odr ptr @_ZN19BuilderListIteratorI11BuilderListI4SlotI1TEEE4nextEv(ptr %0) {
entry:
  ret ptr null
}

define linkonce_odr void @_ZN19BuilderListIteratorI11BuilderListI4SlotI1TEEEC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr i64 @_ZN11BuilderListI11BuilderListI4SlotI1TEEE5countEv(ptr %0) {
entry:
  ret i64 0
}

define linkonce_odr void @_ZN11BuilderListI11BuilderListI4SlotI1TEEE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z19BuilderListIteratorI11BuilderListI4SlotI1TEEE) %0, ptr %1, ptr %2) {
entry:
  ret void
}

define linkonce_odr void @_ZN11BuilderListI11BuilderListI4SlotI1TEEEC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr ptr @_ZN14VectorIteratorI11BuilderListI4SlotI1TEEE4nextEv(ptr %0) {
entry:
  ret ptr null
}

define linkonce_odr void @_ZN14VectorIteratorI11BuilderListI4SlotI1TEEEC1EP6VectorI11BuilderListI4SlotI1TEEE(ptr %0, ptr %1) {
entry:
  ret void
}

define linkonce_odr void @_ZN14VectorIteratorI11BuilderListI4SlotI1TEEEC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr void @_ZN6VectorI11BuilderListI4SlotI1TEEE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z14VectorIteratorI11BuilderListI4SlotI1TEEE) %0, ptr %1, ptr %2) {
entry:
  ret void
}

define linkonce_odr void @_ZN6VectorI11BuilderListI4SlotI1TEEE8as_sliceEPN4scaly6memory4PageE(ptr noalias sret(%_Z5Slice) %0, ptr %1, ptr %2) {
entry:
  ret void
}

define linkonce_odr void @_ZN6VectorI11BuilderListI4SlotI1TEEEC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr void @_ZN6VectorI11BuilderListI4SlotI1TEEEC1EP11BuilderListI4SlotI1TEEm(ptr %0, ptr %1, i64 %2) {
entry:
  ret void
}

define linkonce_odr void @_ZN6VectorI11BuilderListI4SlotI1TEEEC1EPN4scaly6memory4PageEm(ptr %0, ptr %1, i64 %2) {
entry:
  ret void
}

define linkonce_odr void @_ZN6VectorI11BuilderListI4SlotI1TEEEC1EPN4scaly6memory4PageEP11BuilderListI4SlotI1TEEm(ptr %0, ptr %1, ptr %2, i64 %3) {
entry:
  ret void
}

define linkonce_odr void @_ZN6VectorI11BuilderListI4SlotI1TEEEC1EPN4scaly6memory4PageE6VectorI11BuilderListI4SlotI1TEEE(ptr %0, ptr %1, ptr %2) {
entry:
  ret void
}

define linkonce_odr ptr @_ZN5ArrayI11BuilderListI4SlotI1TEEE10get_bufferEv(ptr %0) {
entry:
  ret ptr null
}

define linkonce_odr i64 @_ZN5ArrayI11BuilderListI4SlotI1TEEE10get_lengthEv(ptr %0) {
entry:
  ret i64 0
}

define linkonce_odr i64 @_ZN5ArrayI11BuilderListI4SlotI1TEEE12get_capacityEv(ptr %0) {
entry:
  ret i64 0
}

define linkonce_odr void @_ZN5ArrayI11BuilderListI4SlotI1TEEE10reallocateEv(ptr %0) {
entry:
  ret void
}

define linkonce_odr void @_ZN5ArrayI11BuilderListI4SlotI1TEEE3addE11BuilderListI4SlotI1TEE(ptr %0, ptr %1) {
entry:
  ret void
}

define linkonce_odr void @_ZN5ArrayI11BuilderListI4SlotI1TEEE3addE6VectorI11BuilderListI4SlotI1TEEE(ptr %0, ptr %1) {
entry:
  ret void
}

define linkonce_odr ptr @_ZN5ArrayI11BuilderListI4SlotI1TEEE3getEPN4scaly6memory4PageEm(ptr %0, ptr %1, i64 %2) {
entry:
  ret ptr null
}

define linkonce_odr void @_ZN5ArrayI11BuilderListI4SlotI1TEEE3putEm11BuilderListI4SlotI1TEE(ptr %0, i64 %1, ptr %2) {
entry:
  ret void
}

define linkonce_odr ptr @_ZN13ArrayIteratorI11BuilderListI4SlotI1TEEE4nextEv(ptr %0) {
entry:
  ret ptr null
}

define linkonce_odr void @_ZN13ArrayIteratorI11BuilderListI4SlotI1TEEEC1EP5ArrayI11BuilderListI4SlotI1TEEE(ptr %0, ptr %1) {
entry:
  ret void
}

define linkonce_odr void @_ZN13ArrayIteratorI11BuilderListI4SlotI1TEEEC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr void @_ZN5ArrayI11BuilderListI4SlotI1TEEE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z13ArrayIteratorI11BuilderListI4SlotI1TEEE) %0, ptr %1, ptr %2) {
entry:
  ret void
}

define linkonce_odr void @_ZN6VectorI11BuilderListI4SlotI1TEEEC1EPN4scaly6memory4PageE5ArrayI11BuilderListI4SlotI1TEEE(ptr %0, ptr %1, ptr %2) {
entry:
  ret void
}

define linkonce_odr ptr @_ZN4ListI11BuilderListI4SlotI1TEEE8get_headEPN4scaly6memory4PageE(ptr %0, ptr %1) {
entry:
  ret ptr null
}

define linkonce_odr ptr @_ZN12ListIteratorI11BuilderListI4SlotI1TEEE4nextEv(ptr %0) {
entry:
  ret ptr null
}

define linkonce_odr void @_ZN12ListIteratorI11BuilderListI4SlotI1TEEEC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr i64 @_ZN4ListI11BuilderListI4SlotI1TEEE5countEv(ptr %0) {
entry:
  ret i64 0
}

define linkonce_odr i1 @_ZN4ListI11BuilderListI4SlotI1TEEE6removeE11BuilderListI4SlotI1TEE(ptr %0, ptr %1) {
entry:
  ret i1 false
}

define linkonce_odr void @_ZN4ListI11BuilderListI4SlotI1TEEE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z12ListIteratorI11BuilderListI4SlotI1TEEE) %0, ptr %1, ptr %2) {
entry:
  ret void
}

define linkonce_odr void @_ZN4ListI11BuilderListI4SlotI1TEEE3addE11BuilderListI4SlotI1TEE(ptr %0, ptr %1) {
entry:
  ret void
}

define linkonce_odr void @_ZN4ListI11BuilderListI4SlotI1TEEEC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr void @_ZN6VectorI11BuilderListI4SlotI1TEEEC1EPN4scaly6memory4PageE4ListI11BuilderListI4SlotI1TEEE(ptr %0, ptr %1, ptr %2) {
entry:
  ret void
}

define linkonce_odr void @_ZN5ArrayI11BuilderListI4SlotI1TEEEC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr void @_ZN5ArrayI11BuilderListI4SlotI1TEEEC1Em(ptr %0, i64 %1) {
entry:
  ret void
}

define linkonce_odr void @_ZN5ArrayI11BuilderListI4SlotI1TEEEC1EPN4scaly6memory4PageE(ptr %0, ptr %1) {
entry:
  ret void
}

define linkonce_odr void @_ZN5ArrayI11BuilderListI4SlotI1TEEEC1EPN4scaly6memory4PageEm(ptr %0, ptr %1, i64 %2) {
entry:
  ret void
}

define linkonce_odr ptr @_ZN19BuilderListIteratorI4SlotI1TEE4nextEv(ptr %0) {
entry:
  ret ptr null
}

define linkonce_odr void @_ZN19BuilderListIteratorI4SlotI1TEEC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr void @_ZN11BuilderListI4SlotI1TEE3addE4SlotI1TE(ptr %0, ptr %1) {
entry:
  ret void
}

define linkonce_odr i1 @_ZN11BuilderListI4SlotI1TEE6removeE4SlotI1TE(ptr %0, ptr %1) {
entry:
  ret i1 false
}

define linkonce_odr ptr @_ZN11BuilderListI4SlotI1TEE8get_headEPN4scaly6memory4PageE(ptr %0, ptr %1) {
entry:
  ret ptr null
}

define linkonce_odr i64 @_ZN11BuilderListI4SlotI1TEE5countEv(ptr %0) {
entry:
  ret i64 0
}

define linkonce_odr void @_ZN11BuilderListI4SlotI1TEE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z19BuilderListIteratorI4SlotI1TEE) %0, ptr %1, ptr %2) {
entry:
  ret void
}

define linkonce_odr void @_ZN11BuilderListI4SlotI1TEEC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr void @_ZN14HashSetBuilderI1TE10reallocateEm(ptr %0, i64 %1) {
entry:
  ret void
}

define linkonce_odr i1 @_ZN14HashSetBuilderI1TE3addE1T(ptr %0, ptr %1) {
entry:
  ret i1 false
}

define linkonce_odr i1 @_ZN14HashSetBuilderI1TE12add_internalE1T(ptr %0, ptr %1) {
entry:
  ret i1 false
}

define linkonce_odr i1 @_ZN14HashSetBuilderI1TE8containsE1T(ptr %0, ptr %1) {
entry:
  ret i1 false
}

define linkonce_odr void @_ZN14HashSetBuilderI1TEC1E6VectorI1TE(ptr %0, ptr %1) {
entry:
  ret void
}

define linkonce_odr void @_ZN14HashSetBuilderI1TEC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr ptr @_ZN6VectorI12KeyValuePairI6StringiEE3getEPN4scaly6memory4PageEm(ptr %0, ptr %1, i64 %2) {
entry:
  %load.struct = load %_Z6VectorI12KeyValuePairI6StringiEE, ptr %1, align 8
  %length = extractvalue %_Z6VectorI12KeyValuePairI6StringiEE %load.struct, 0
  %ge = icmp uge i64 %2, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %load.struct1 = load %_Z6VectorI12KeyValuePairI6StringiEE, ptr %1, align 8
  %data = extractvalue %_Z6VectorI12KeyValuePairI6StringiEE %load.struct1, 1
  %ptr.add = getelementptr inbounds %_Z12KeyValuePairI6StringiE, ptr %data, i64 %2
  ret ptr %ptr.add
}

define linkonce_odr ptr @_ZN6VectorI12KeyValuePairI6StringiEE7get_ptrEm(ptr %0, i64 %1) {
entry:
  %load.struct = load %_Z6VectorI12KeyValuePairI6StringiEE, ptr %0, align 8
  %length = extractvalue %_Z6VectorI12KeyValuePairI6StringiEE %load.struct, 0
  %ge = icmp uge i64 %1, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %load.struct1 = load %_Z6VectorI12KeyValuePairI6StringiEE, ptr %0, align 8
  %data = extractvalue %_Z6VectorI12KeyValuePairI6StringiEE %load.struct1, 1
  %ptr.add = getelementptr inbounds %_Z12KeyValuePairI6StringiE, ptr %data, i64 %1
  ret ptr %ptr.add
}

define linkonce_odr void @_ZN6VectorI12KeyValuePairI6StringiEE3putEm12KeyValuePairI6StringiE(ptr %0, i64 %1, ptr %2) {
entry:
  %load.struct = load %_Z6VectorI12KeyValuePairI6StringiEE, ptr %0, align 8
  %length = extractvalue %_Z6VectorI12KeyValuePairI6StringiEE %load.struct, 0
  %ge = icmp uge i64 %1, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  call void @_Z11scaly_eputsP10const_char(ptr @.str.17)
  call void @_Z12scaly_eputnlv()
  call void @exit(i64 15)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %load.struct1 = load %_Z6VectorI12KeyValuePairI6StringiEE, ptr %0, align 8
  %data = extractvalue %_Z6VectorI12KeyValuePairI6StringiEE %load.struct1, 1
  %ptr.add = getelementptr inbounds %_Z12KeyValuePairI6StringiE, ptr %data, i64 %1
  %store.load = load %_Z12KeyValuePairI6StringiE, ptr %2, align 8
  store %_Z12KeyValuePairI6StringiE %store.load, ptr %ptr.add, align 8
  ret void
}

define linkonce_odr ptr @_ZN14VectorIteratorI12KeyValuePairI6StringiEE4nextEv(ptr %0) {
entry:
  %load.struct = load %_Z14VectorIteratorI12KeyValuePairI6StringiEE, ptr %0, align 8
  %vector = extractvalue %_Z14VectorIteratorI12KeyValuePairI6StringiEE %load.struct, 0
  %eq = icmp eq ptr %vector, null
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %load.struct1 = load %_Z14VectorIteratorI12KeyValuePairI6StringiEE, ptr %0, align 8
  %position = extractvalue %_Z14VectorIteratorI12KeyValuePairI6StringiEE %load.struct1, 1
  %load.struct2 = load %_Z14VectorIteratorI12KeyValuePairI6StringiEE, ptr %0, align 8
  %vector3 = extractvalue %_Z14VectorIteratorI12KeyValuePairI6StringiEE %load.struct2, 0
  %deref = load %_Z6VectorI12KeyValuePairI6StringiEE, ptr %vector3, align 8
  %length = extractvalue %_Z6VectorI12KeyValuePairI6StringiEE %deref, 0
  %eq4 = icmp eq i64 %position, %length
  br i1 %eq4, label %if.then5, label %if.end6

if.then5:                                         ; preds = %if.end
  ret ptr null

if.end6:                                          ; preds = %if.end
  %load.struct7 = load %_Z14VectorIteratorI12KeyValuePairI6StringiEE, ptr %0, align 8
  %position8 = extractvalue %_Z14VectorIteratorI12KeyValuePairI6StringiEE %load.struct7, 1
  %add = add i64 %position8, 1
  %position9 = getelementptr inbounds %_Z14VectorIteratorI12KeyValuePairI6StringiEE, ptr %0, i32 0, i32 1
  store i64 %add, ptr %position9, align 4
  %field.inplace = getelementptr inbounds %_Z14VectorIteratorI12KeyValuePairI6StringiEE, ptr %0, i32 0, i32 0
  %deref.recv = load ptr, ptr %field.inplace, align 8
  %load.struct10 = load %_Z14VectorIteratorI12KeyValuePairI6StringiEE, ptr %0, align 8
  %position11 = extractvalue %_Z14VectorIteratorI12KeyValuePairI6StringiEE %load.struct10, 1
  %sub = sub i64 %position11, 1
  %call = call ptr @_ZN6VectorI12KeyValuePairI6StringiEE7get_ptrEm(ptr %deref.recv, i64 %sub)
  ret ptr %call
}

define linkonce_odr void @_ZN14VectorIteratorI12KeyValuePairI6StringiEEC1EP6VectorI12KeyValuePairI6StringiEE(ptr %0, ptr %1) {
entry:
  %vector = getelementptr inbounds %_Z14VectorIteratorI12KeyValuePairI6StringiEE, ptr %0, i32 0, i32 0
  store ptr %1, ptr %vector, align 8
  %position = getelementptr inbounds %_Z14VectorIteratorI12KeyValuePairI6StringiEE, ptr %0, i32 0, i32 1
  store i64 0, ptr %position, align 4
  ret void
}

define linkonce_odr void @_ZN14VectorIteratorI12KeyValuePairI6StringiEEC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr void @_ZN6VectorI12KeyValuePairI6StringiEE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z14VectorIteratorI12KeyValuePairI6StringiEE) %0, ptr %1, ptr %2) {
entry:
  %struct.init = alloca %_Z14VectorIteratorI12KeyValuePairI6StringiEE, align 8
  call void @_ZN14VectorIteratorI12KeyValuePairI6StringiEEC1EP6VectorI12KeyValuePairI6StringiEE(ptr %struct.init, ptr %2)
  %sret.body = load %_Z14VectorIteratorI12KeyValuePairI6StringiEE, ptr %struct.init, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %struct.init, i64 ptrtoint (ptr getelementptr (%_Z14VectorIteratorI12KeyValuePairI6StringiEE, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN6VectorI12KeyValuePairI6StringiEE8as_sliceEPN4scaly6memory4PageE(ptr noalias sret(%_Z5Slice) %0, ptr %1, ptr %2) {
entry:
  %load.struct = load %_Z6VectorI12KeyValuePairI6StringiEE, ptr %2, align 8
  %data = extractvalue %_Z6VectorI12KeyValuePairI6StringiEE %load.struct, 1
  %load.struct1 = load %_Z6VectorI12KeyValuePairI6StringiEE, ptr %2, align 8
  %length = extractvalue %_Z6VectorI12KeyValuePairI6StringiEE %load.struct1, 0
  %tuple = alloca { ptr, i64 }, align 8
  %tuple.field = getelementptr inbounds { ptr, i64 }, ptr %tuple, i32 0, i32 0
  store ptr %data, ptr %tuple.field, align 1
  %tuple.field2 = getelementptr inbounds { ptr, i64 }, ptr %tuple, i32 0, i32 1
  store i64 %length, ptr %tuple.field2, align 1
  %tuple.val = load { ptr, i64 }, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr ({ ptr, i64 }, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN6VectorI12KeyValuePairI6StringiEEC1Ev(ptr %0) {
entry:
  %length = getelementptr inbounds %_Z6VectorI12KeyValuePairI6StringiEE, ptr %0, i32 0, i32 0
  store i64 0, ptr %length, align 4
  %data = getelementptr inbounds %_Z6VectorI12KeyValuePairI6StringiEE, ptr %0, i32 0, i32 1
  store ptr null, ptr %data, align 8
  ret void
}

define linkonce_odr void @_ZN6VectorI12KeyValuePairI6StringiEEC1EP12KeyValuePairI6StringiEm(ptr %0, ptr %1, i64 %2) {
entry:
  %length = getelementptr inbounds %_Z6VectorI12KeyValuePairI6StringiEE, ptr %0, i32 0, i32 0
  store i64 %2, ptr %length, align 4
  %data = getelementptr inbounds %_Z6VectorI12KeyValuePairI6StringiEE, ptr %0, i32 0, i32 1
  store ptr %1, ptr %data, align 8
  ret void
}

define linkonce_odr void @_ZN6VectorI12KeyValuePairI6StringiEEC1EPN4scaly6memory4PageEm(ptr %0, ptr %1, i64 %2) {
entry:
  %length = getelementptr inbounds %_Z6VectorI12KeyValuePairI6StringiEE, ptr %0, i32 0, i32 0
  store i64 %2, ptr %length, align 4
  %gt = icmp ugt i64 %2, 0
  br i1 %gt, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %mul = mul i64 %2, ptrtoint (ptr getelementptr (%_Z12KeyValuePairI6StringiE, ptr null, i32 1) to i64)
  %call = call ptr @_ZN4Page8allocateEmm(ptr %1, i64 %mul, i64 8)
  %data = getelementptr inbounds %_Z6VectorI12KeyValuePairI6StringiEE, ptr %0, i32 0, i32 1
  store ptr %call, ptr %data, align 8
  %field.inplace = getelementptr inbounds %_Z6VectorI12KeyValuePairI6StringiEE, ptr %0, i32 0, i32 1
  %deref.recv = load ptr, ptr %field.inplace, align 8
  %mul1 = mul i64 %2, ptrtoint (ptr getelementptr (%_Z12KeyValuePairI6StringiE, ptr null, i32 1) to i64)
  %call2 = call ptr @memset(ptr %deref.recv, i64 0, i64 %mul1)
  br label %if.end

if.else:                                          ; preds = %entry
  %data3 = getelementptr inbounds %_Z6VectorI12KeyValuePairI6StringiEE, ptr %0, i32 0, i32 1
  store ptr null, ptr %data3, align 8
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  %if.value = phi ptr [ %call2, %if.then ], [ null, %if.else ]
  ret void
}

define linkonce_odr void @_ZN6VectorI12KeyValuePairI6StringiEEC1EPN4scaly6memory4PageEP12KeyValuePairI6StringiEm(ptr %0, ptr %1, ptr %2, i64 %3) {
entry:
  %length = getelementptr inbounds %_Z6VectorI12KeyValuePairI6StringiEE, ptr %0, i32 0, i32 0
  store i64 %3, ptr %length, align 4
  %gt = icmp ugt i64 %3, 0
  br i1 %gt, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %mul = mul i64 %3, ptrtoint (ptr getelementptr (%_Z12KeyValuePairI6StringiE, ptr null, i32 1) to i64)
  %call = call ptr @_ZN4Page8allocateEmm(ptr %1, i64 %mul, i64 8)
  %data = getelementptr inbounds %_Z6VectorI12KeyValuePairI6StringiEE, ptr %0, i32 0, i32 1
  store ptr %call, ptr %data, align 8
  %field.inplace = getelementptr inbounds %_Z6VectorI12KeyValuePairI6StringiEE, ptr %0, i32 0, i32 1
  %deref.recv = load ptr, ptr %field.inplace, align 8
  %mul1 = mul i64 %3, ptrtoint (ptr getelementptr (%_Z12KeyValuePairI6StringiE, ptr null, i32 1) to i64)
  %call2 = call ptr @memcpy(ptr %deref.recv, ptr %2, i64 %mul1)
  br label %if.end

if.else:                                          ; preds = %entry
  %data3 = getelementptr inbounds %_Z6VectorI12KeyValuePairI6StringiEE, ptr %0, i32 0, i32 1
  store ptr null, ptr %data3, align 8
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  %if.value = phi ptr [ %call2, %if.then ], [ null, %if.else ]
  ret void
}

define linkonce_odr void @_ZN6VectorI12KeyValuePairI6StringiEEC1EPN4scaly6memory4PageE6VectorI12KeyValuePairI6StringiEE(ptr %0, ptr %1, ptr %2) {
entry:
  %load.struct = load %_Z6VectorI12KeyValuePairI6StringiEE, ptr %2, align 8
  %length = extractvalue %_Z6VectorI12KeyValuePairI6StringiEE %load.struct, 0
  %length1 = getelementptr inbounds %_Z6VectorI12KeyValuePairI6StringiEE, ptr %0, i32 0, i32 0
  store i64 %length, ptr %length1, align 4
  %load.struct2 = load %_Z6VectorI12KeyValuePairI6StringiEE, ptr %0, align 8
  %length3 = extractvalue %_Z6VectorI12KeyValuePairI6StringiEE %load.struct2, 0
  %gt = icmp ugt i64 %length3, 0
  br i1 %gt, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %load.struct4 = load %_Z6VectorI12KeyValuePairI6StringiEE, ptr %0, align 8
  %length5 = extractvalue %_Z6VectorI12KeyValuePairI6StringiEE %load.struct4, 0
  %mul = mul i64 %length5, ptrtoint (ptr getelementptr (%_Z12KeyValuePairI6StringiE, ptr null, i32 1) to i64)
  %call = call ptr @_ZN4Page8allocateEmm(ptr %1, i64 %mul, i64 8)
  %data = getelementptr inbounds %_Z6VectorI12KeyValuePairI6StringiEE, ptr %0, i32 0, i32 1
  store ptr %call, ptr %data, align 8
  %field.inplace = getelementptr inbounds %_Z6VectorI12KeyValuePairI6StringiEE, ptr %0, i32 0, i32 1
  %deref.recv = load ptr, ptr %field.inplace, align 8
  %field.inplace6 = getelementptr inbounds %_Z6VectorI12KeyValuePairI6StringiEE, ptr %2, i32 0, i32 1
  %deref.recv7 = load ptr, ptr %field.inplace6, align 8
  %load.struct8 = load %_Z6VectorI12KeyValuePairI6StringiEE, ptr %0, align 8
  %length9 = extractvalue %_Z6VectorI12KeyValuePairI6StringiEE %load.struct8, 0
  %mul10 = mul i64 %length9, ptrtoint (ptr getelementptr (%_Z12KeyValuePairI6StringiE, ptr null, i32 1) to i64)
  %call11 = call ptr @memcpy(ptr %deref.recv, ptr %deref.recv7, i64 %mul10)
  br label %if.end

if.else:                                          ; preds = %entry
  %data12 = getelementptr inbounds %_Z6VectorI12KeyValuePairI6StringiEE, ptr %0, i32 0, i32 1
  store ptr null, ptr %data12, align 8
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  %if.value = phi ptr [ %call11, %if.then ], [ null, %if.else ]
  ret void
}

define linkonce_odr void @_ZN6VectorI12KeyValuePairI6StringiEEC1EPN4scaly6memory4PageE5ArrayI12KeyValuePairI6StringiEE(ptr %0, ptr %1, ptr %2) {
entry:
  %load.struct = load %_Z5ArrayI12KeyValuePairI6StringiEE, ptr %2, align 8
  %length = extractvalue %_Z5ArrayI12KeyValuePairI6StringiEE %load.struct, 0
  %length1 = getelementptr inbounds %_Z6VectorI12KeyValuePairI6StringiEE, ptr %0, i32 0, i32 0
  store i64 %length, ptr %length1, align 4
  %load.struct2 = load %_Z6VectorI12KeyValuePairI6StringiEE, ptr %0, align 8
  %length3 = extractvalue %_Z6VectorI12KeyValuePairI6StringiEE %load.struct2, 0
  %gt = icmp ugt i64 %length3, 0
  br i1 %gt, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %load.struct4 = load %_Z6VectorI12KeyValuePairI6StringiEE, ptr %0, align 8
  %length5 = extractvalue %_Z6VectorI12KeyValuePairI6StringiEE %load.struct4, 0
  %mul = mul i64 %length5, ptrtoint (ptr getelementptr (%_Z12KeyValuePairI6StringiE, ptr null, i32 1) to i64)
  %call = call ptr @_ZN4Page8allocateEmm(ptr %1, i64 %mul, i64 8)
  %data = getelementptr inbounds %_Z6VectorI12KeyValuePairI6StringiEE, ptr %0, i32 0, i32 1
  store ptr %call, ptr %data, align 8
  %load.struct6 = load %_Z5ArrayI12KeyValuePairI6StringiEE, ptr %2, align 8
  %vector = extractvalue %_Z5ArrayI12KeyValuePairI6StringiEE %load.struct6, 1
  %field.inplace = getelementptr inbounds %_Z6VectorI12KeyValuePairI6StringiEE, ptr %0, i32 0, i32 1
  %deref.recv = load ptr, ptr %field.inplace, align 8
  %field.inplace7 = getelementptr inbounds %_Z6VectorI12KeyValuePairI6StringiEE, ptr %vector, i32 0, i32 1
  %deref.recv8 = load ptr, ptr %field.inplace7, align 8
  %load.struct9 = load %_Z6VectorI12KeyValuePairI6StringiEE, ptr %0, align 8
  %length10 = extractvalue %_Z6VectorI12KeyValuePairI6StringiEE %load.struct9, 0
  %mul11 = mul i64 %length10, ptrtoint (ptr getelementptr (%_Z12KeyValuePairI6StringiE, ptr null, i32 1) to i64)
  %call12 = call ptr @memcpy(ptr %deref.recv, ptr %deref.recv8, i64 %mul11)
  br label %if.end

if.else:                                          ; preds = %entry
  %data13 = getelementptr inbounds %_Z6VectorI12KeyValuePairI6StringiEE, ptr %0, i32 0, i32 1
  store ptr null, ptr %data13, align 8
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  %if.value = phi ptr [ %call12, %if.then ], [ null, %if.else ]
  ret void
}

define linkonce_odr ptr @_ZN4ListI12KeyValuePairI6StringiEE8get_headEPN4scaly6memory4PageE(ptr %0, ptr %1) {
entry:
  %load.struct = load %_Z4ListI12KeyValuePairI6StringiEE, ptr %1, align 8
  %head = extractvalue %_Z4ListI12KeyValuePairI6StringiEE %load.struct, 0
  %eq = icmp eq ptr %head, null
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %addr.gep = getelementptr inbounds %_Z4ListI12KeyValuePairI6StringiEE, ptr %1, i32 0, i32 0
  %addr.gep1 = getelementptr inbounds %_Z4NodeI12KeyValuePairI6StringiEE, ptr %addr.gep, i32 0, i32 0
  ret ptr %addr.gep1
}

define linkonce_odr ptr @_ZN12ListIteratorI12KeyValuePairI6StringiEE4nextEv(ptr %0) {
entry:
  %old_current = alloca ptr, align 8
  %load.struct = load %_Z12ListIteratorI12KeyValuePairI6StringiEE, ptr %0, align 8
  %current = extractvalue %_Z12ListIteratorI12KeyValuePairI6StringiEE %load.struct, 0
  %ne = icmp ne ptr %current, null
  br i1 %ne, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %load.struct1 = load %_Z12ListIteratorI12KeyValuePairI6StringiEE, ptr %0, align 8
  %current2 = extractvalue %_Z12ListIteratorI12KeyValuePairI6StringiEE %load.struct1, 0
  store ptr %current2, ptr %old_current, align 1
  %load.struct3 = load %_Z12ListIteratorI12KeyValuePairI6StringiEE, ptr %0, align 8
  %current4 = extractvalue %_Z12ListIteratorI12KeyValuePairI6StringiEE %load.struct3, 0
  %deref = load %_Z4NodeI12KeyValuePairI6StringiEE, ptr %current4, align 8
  %next = extractvalue %_Z4NodeI12KeyValuePairI6StringiEE %deref, 1
  %current5 = getelementptr inbounds %_Z12ListIteratorI12KeyValuePairI6StringiEE, ptr %0, i32 0, i32 0
  store ptr %next, ptr %current5, align 8
  %old_current6 = load ptr, ptr %old_current, align 8
  %addr.gep = getelementptr inbounds %_Z4NodeI12KeyValuePairI6StringiEE, ptr %old_current6, i32 0, i32 0
  ret ptr %addr.gep

if.else:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; No predecessors!
  ret ptr null
}

define linkonce_odr void @_ZN12ListIteratorI12KeyValuePairI6StringiEEC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr i64 @_ZN4ListI12KeyValuePairI6StringiEE5countEv(ptr %0) {
entry:
  %local_page = call ptr @_Z21scaly_alloc_root_pagev()
  %sret.result = alloca %_Z12ListIteratorI12KeyValuePairI6StringiEE, align 8
  call void @_ZN4ListI12KeyValuePairI6StringiEE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z12ListIteratorI12KeyValuePairI6StringiEE) %sret.result, ptr %local_page, ptr %0)
  %list_iterator = alloca ptr, align 8
  store ptr %sret.result, ptr %list_iterator, align 1
  %i = alloca i64, align 8
  store i64 0, ptr %i, align 1
  br label %while.cond

while.cond:                                       ; preds = %while.body, %entry
  %list_iterator1 = load ptr, ptr %list_iterator, align 8
  %call = call ptr @_ZN12ListIteratorI12KeyValuePairI6StringiEE4nextEv(ptr %list_iterator1)
  %ne = icmp ne ptr %call, null
  br i1 %ne, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %i2 = load i64, ptr %i, align 4
  %add = add i64 %i2, 1
  store i64 %add, ptr %i, align 1
  br label %while.cond

while.exit:                                       ; preds = %while.cond
  %i3 = load i64, ptr %i, align 4
  call void @_Z28scaly_release_root_page_fullP4Page(ptr %local_page)
  ret i64 %i3
}

define linkonce_odr i1 @_ZN4ListI12KeyValuePairI6StringiEE6removeE12KeyValuePairI6StringiE(ptr %0, ptr %1) {
entry:
  %load.struct = load %_Z4ListI12KeyValuePairI6StringiEE, ptr %0, align 8
  %head = extractvalue %_Z4ListI12KeyValuePairI6StringiEE %load.struct, 0
  %node = alloca ptr, align 8
  store ptr %head, ptr %node, align 1
  %previous_node = alloca ptr, align 8
  store ptr null, ptr %previous_node, align 1
  br label %while.cond

while.cond:                                       ; preds = %while.body, %entry
  %node1 = load ptr, ptr %node, align 8
  %ne = icmp ne ptr %node1, null
  br i1 %ne, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %node2 = load ptr, ptr %node, align 8
  %load.struct3 = load %_Z4NodeI12KeyValuePairI6StringiEE, ptr %node2, align 8
  %element = extractvalue %_Z4NodeI12KeyValuePairI6StringiEE %load.struct3, 0
  %node4 = load ptr, ptr %node, align 8
  store ptr %node4, ptr %previous_node, align 1
  %node5 = load ptr, ptr %node, align 8
  %load.struct6 = load %_Z4NodeI12KeyValuePairI6StringiEE, ptr %node5, align 8
  %next = extractvalue %_Z4NodeI12KeyValuePairI6StringiEE %load.struct6, 1
  store ptr %next, ptr %node, align 1
  br label %while.cond

while.exit:                                       ; preds = %while.cond
  ret i1 false
}

define linkonce_odr void @_ZN4ListI12KeyValuePairI6StringiEE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z12ListIteratorI12KeyValuePairI6StringiEE) %0, ptr %1, ptr %2) {
entry:
  %load.struct = load %_Z4ListI12KeyValuePairI6StringiEE, ptr %2, align 8
  %head = extractvalue %_Z4ListI12KeyValuePairI6StringiEE %load.struct, 0
  %tuple = alloca %_Z12ListIteratorI12KeyValuePairI6StringiEE, align 8
  %tuple.field = getelementptr inbounds %_Z12ListIteratorI12KeyValuePairI6StringiEE, ptr %tuple, i32 0, i32 0
  store ptr %head, ptr %tuple.field, align 1
  %tuple.val = load %_Z12ListIteratorI12KeyValuePairI6StringiEE, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z12ListIteratorI12KeyValuePairI6StringiEE, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN4ListI12KeyValuePairI6StringiEE3addE12KeyValuePairI6StringiE(ptr %0, ptr %1) {
entry:
  %load.struct = load %_Z4ListI12KeyValuePairI6StringiEE, ptr %0, align 8
  %head = extractvalue %_Z4ListI12KeyValuePairI6StringiEE %load.struct, 0
  %own_page = call ptr @_Z3getPv(ptr %0)
  %tuple.region = call ptr @_ZN4Page8allocateEmm(ptr %own_page, i64 ptrtoint (ptr getelementptr (%_Z4NodeI12KeyValuePairI6StringiEE, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr ({ i1, %_Z4NodeI12KeyValuePairI6StringiEE }, ptr null, i64 0, i32 1) to i64))
  %field.load = load %_Z12KeyValuePairI6StringiE, ptr %1, align 8
  %tuple.field = getelementptr inbounds %_Z4NodeI12KeyValuePairI6StringiEE, ptr %tuple.region, i32 0, i32 0
  store %_Z12KeyValuePairI6StringiE %field.load, ptr %tuple.field, align 1
  %tuple.field1 = getelementptr inbounds %_Z4NodeI12KeyValuePairI6StringiEE, ptr %tuple.region, i32 0, i32 1
  store ptr %head, ptr %tuple.field1, align 1
  %head2 = getelementptr inbounds %_Z4ListI12KeyValuePairI6StringiEE, ptr %0, i32 0, i32 0
  store ptr %tuple.region, ptr %head2, align 8
  ret void
}

define linkonce_odr void @_ZN4ListI12KeyValuePairI6StringiEEC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr void @_ZN6VectorI12KeyValuePairI6StringiEEC1EPN4scaly6memory4PageE4ListI12KeyValuePairI6StringiEE(ptr %0, ptr %1, ptr %2) {
entry:
  %deref.tmp = alloca %_Z12KeyValuePairI6StringiE, align 8
  %i = alloca i64, align 8
  %list_iterator = alloca ptr, align 8
  %local_page = call ptr @_Z21scaly_alloc_root_pagev()
  %sret.result = alloca %_Z12ListIteratorI12KeyValuePairI6StringiEE, align 8
  %call = call i64 @_ZN4ListI12KeyValuePairI6StringiEE5countEv(ptr %2)
  %length = getelementptr inbounds %_Z6VectorI12KeyValuePairI6StringiEE, ptr %0, i32 0, i32 0
  store i64 %call, ptr %length, align 4
  %load.struct = load %_Z6VectorI12KeyValuePairI6StringiEE, ptr %0, align 8
  %length1 = extractvalue %_Z6VectorI12KeyValuePairI6StringiEE %load.struct, 0
  %gt = icmp ugt i64 %length1, 0
  br i1 %gt, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %load.struct2 = load %_Z6VectorI12KeyValuePairI6StringiEE, ptr %0, align 8
  %length3 = extractvalue %_Z6VectorI12KeyValuePairI6StringiEE %load.struct2, 0
  %mul = mul i64 %length3, ptrtoint (ptr getelementptr (%_Z12KeyValuePairI6StringiE, ptr null, i32 1) to i64)
  %call4 = call ptr @_ZN4Page8allocateEmm(ptr %1, i64 %mul, i64 8)
  %data = getelementptr inbounds %_Z6VectorI12KeyValuePairI6StringiEE, ptr %0, i32 0, i32 1
  store ptr %call4, ptr %data, align 8
  call void @_ZN4ListI12KeyValuePairI6StringiEE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z12ListIteratorI12KeyValuePairI6StringiEE) %sret.result, ptr %local_page, ptr %2)
  store ptr %sret.result, ptr %list_iterator, align 1
  %load.struct5 = load %_Z6VectorI12KeyValuePairI6StringiEE, ptr %0, align 8
  %length6 = extractvalue %_Z6VectorI12KeyValuePairI6StringiEE %load.struct5, 0
  store i64 %length6, ptr %i, align 1
  br label %while.cond

if.else:                                          ; preds = %entry
  %data13 = getelementptr inbounds %_Z6VectorI12KeyValuePairI6StringiEE, ptr %0, i32 0, i32 1
  store ptr null, ptr %data13, align 8
  br label %if.end

if.end:                                           ; preds = %if.else, %while.exit
  call void @_Z28scaly_release_root_page_fullP4Page(ptr %local_page)
  ret void

while.cond:                                       ; preds = %while.body, %if.then
  %list_iterator7 = load ptr, ptr %list_iterator, align 8
  %call8 = call ptr @_ZN12ListIteratorI12KeyValuePairI6StringiEE4nextEv(ptr %list_iterator7)
  %while.tobool = icmp ne ptr %call8, null
  br i1 %while.tobool, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %i9 = load i64, ptr %i, align 4
  %sub = sub i64 %i9, 1
  store i64 %sub, ptr %i, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %deref.tmp, ptr align 1 %call8, i64 ptrtoint (ptr getelementptr (%_Z12KeyValuePairI6StringiE, ptr null, i32 1) to i64), i1 false)
  %load.struct10 = load %_Z6VectorI12KeyValuePairI6StringiEE, ptr %0, align 8
  %data11 = extractvalue %_Z6VectorI12KeyValuePairI6StringiEE %load.struct10, 1
  %i12 = load i64, ptr %i, align 4
  %ptr.add = getelementptr inbounds %_Z12KeyValuePairI6StringiE, ptr %data11, i64 %i12
  %store.load = load %_Z12KeyValuePairI6StringiE, ptr %deref.tmp, align 8
  store %_Z12KeyValuePairI6StringiE %store.load, ptr %ptr.add, align 8
  br label %while.cond

while.exit:                                       ; preds = %while.cond
  br label %if.end
}

define linkonce_odr ptr @_ZN5ArrayI12KeyValuePairI6StringiEE10get_bufferEv(ptr %0) {
entry:
  %load.struct = load %_Z5ArrayI12KeyValuePairI6StringiEE, ptr %0, align 8
  %vector = extractvalue %_Z5ArrayI12KeyValuePairI6StringiEE %load.struct, 1
  %eq = icmp eq ptr %vector, null
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %load.struct1 = load %_Z5ArrayI12KeyValuePairI6StringiEE, ptr %0, align 8
  %vector2 = extractvalue %_Z5ArrayI12KeyValuePairI6StringiEE %load.struct1, 1
  %deref = load %_Z6VectorI12KeyValuePairI6StringiEE, ptr %vector2, align 8
  %data = extractvalue %_Z6VectorI12KeyValuePairI6StringiEE %deref, 1
  ret ptr %data
}

define linkonce_odr i64 @_ZN5ArrayI12KeyValuePairI6StringiEE10get_lengthEv(ptr %0) {
entry:
  %load.struct = load %_Z5ArrayI12KeyValuePairI6StringiEE, ptr %0, align 8
  %length = extractvalue %_Z5ArrayI12KeyValuePairI6StringiEE %load.struct, 0
  ret i64 %length
}

define linkonce_odr i64 @_ZN5ArrayI12KeyValuePairI6StringiEE12get_capacityEv(ptr %0) {
entry:
  %load.struct = load %_Z5ArrayI12KeyValuePairI6StringiEE, ptr %0, align 8
  %vector = extractvalue %_Z5ArrayI12KeyValuePairI6StringiEE %load.struct, 1
  %eq = icmp eq ptr %vector, null
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret i64 0

if.end:                                           ; preds = %entry
  %load.struct1 = load %_Z5ArrayI12KeyValuePairI6StringiEE, ptr %0, align 8
  %vector2 = extractvalue %_Z5ArrayI12KeyValuePairI6StringiEE %load.struct1, 1
  %deref = load %_Z6VectorI12KeyValuePairI6StringiEE, ptr %vector2, align 8
  %length = extractvalue %_Z6VectorI12KeyValuePairI6StringiEE %deref, 0
  ret i64 %length
}

define linkonce_odr void @_ZN5ArrayI12KeyValuePairI6StringiEE10reallocateEv(ptr %0) {
entry:
  %call = call ptr @_ZN4Page3getEPv(ptr %0)
  %length = alloca i64, align 8
  store i64 0, ptr %length, align 1
  %load.struct = load %_Z5ArrayI12KeyValuePairI6StringiEE, ptr %0, align 8
  %vector = extractvalue %_Z5ArrayI12KeyValuePairI6StringiEE %load.struct, 1
  %eq = icmp eq ptr %vector, null
  br i1 %eq, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %call1 = call ptr @_ZN4Page23allocate_exclusive_pageEv(ptr %call)
  %call2 = call i64 @_ZN4Page12get_capacityEm(ptr %call1, i64 8)
  %sub = sub i64 %call2, ptrtoint (ptr getelementptr (%_Z6VectorI12KeyValuePairI6StringiEE, ptr null, i32 1) to i64)
  %udiv = udiv i64 %sub, ptrtoint (ptr getelementptr (%_Z12KeyValuePairI6StringiE, ptr null, i32 1) to i64)
  store i64 %udiv, ptr %length, align 1
  %struct.region = call ptr @_ZN4Page8allocateEmm(ptr %call1, i64 ptrtoint (ptr getelementptr (%_Z6VectorI12KeyValuePairI6StringiEE, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr ({ i1, %_Z6VectorI12KeyValuePairI6StringiEE }, ptr null, i64 0, i32 1) to i64))
  %length3 = load i64, ptr %length, align 4
  call void @_ZN6VectorI12KeyValuePairI6StringiEEC1EPN4scaly6memory4PageEm(ptr %struct.region, ptr %call1, i64 %length3)
  %vector4 = getelementptr inbounds %_Z5ArrayI12KeyValuePairI6StringiEE, ptr %0, i32 0, i32 1
  store ptr %struct.region, ptr %vector4, align 8
  br label %if.end

if.else:                                          ; preds = %entry
  %load.struct5 = load %_Z5ArrayI12KeyValuePairI6StringiEE, ptr %0, align 8
  %vector6 = extractvalue %_Z5ArrayI12KeyValuePairI6StringiEE %load.struct5, 1
  %deref = load %_Z6VectorI12KeyValuePairI6StringiEE, ptr %vector6, align 8
  %length7 = extractvalue %_Z6VectorI12KeyValuePairI6StringiEE %deref, 0
  %mul = mul i64 %length7, 2
  store i64 %mul, ptr %length, align 1
  %call8 = call ptr @_ZN4Page23allocate_exclusive_pageEv(ptr %call)
  %struct.region9 = call ptr @_ZN4Page8allocateEmm(ptr %call8, i64 ptrtoint (ptr getelementptr (%_Z6VectorI12KeyValuePairI6StringiEE, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr ({ i1, %_Z6VectorI12KeyValuePairI6StringiEE }, ptr null, i64 0, i32 1) to i64))
  %length10 = load i64, ptr %length, align 4
  call void @_ZN6VectorI12KeyValuePairI6StringiEEC1EPN4scaly6memory4PageEm(ptr %struct.region9, ptr %call8, i64 %length10)
  %load.struct11 = load %_Z5ArrayI12KeyValuePairI6StringiEE, ptr %0, align 8
  %vector12 = extractvalue %_Z5ArrayI12KeyValuePairI6StringiEE %load.struct11, 1
  %deref13 = load %_Z6VectorI12KeyValuePairI6StringiEE, ptr %vector12, align 8
  %length14 = extractvalue %_Z6VectorI12KeyValuePairI6StringiEE %deref13, 0
  %mul15 = mul i64 %length14, ptrtoint (ptr getelementptr (%_Z12KeyValuePairI6StringiE, ptr null, i32 1) to i64)
  %field.inplace = getelementptr inbounds %_Z6VectorI12KeyValuePairI6StringiEE, ptr %struct.region9, i32 0, i32 1
  %deref.recv = load ptr, ptr %field.inplace, align 8
  %field.inplace16 = getelementptr inbounds %_Z5ArrayI12KeyValuePairI6StringiEE, ptr %0, i32 0, i32 1
  %load.struct17 = load %_Z5ArrayI12KeyValuePairI6StringiEE, ptr %0, align 8
  %vector18 = extractvalue %_Z5ArrayI12KeyValuePairI6StringiEE %load.struct17, 1
  %deref19 = load %_Z6VectorI12KeyValuePairI6StringiEE, ptr %vector18, align 8
  %data = extractvalue %_Z6VectorI12KeyValuePairI6StringiEE %deref19, 1
  %call20 = call ptr @memcpy(ptr %deref.recv, ptr %data, i64 %mul15)
  %load.struct21 = load %_Z5ArrayI12KeyValuePairI6StringiEE, ptr %0, align 8
  %vector22 = extractvalue %_Z5ArrayI12KeyValuePairI6StringiEE %load.struct21, 1
  %call23 = call ptr @_ZN4Page3getEPv(ptr %vector22)
  call void @_ZN4Page25deallocate_exclusive_pageEP4Page(ptr %call, ptr %call23)
  %vector24 = getelementptr inbounds %_Z5ArrayI12KeyValuePairI6StringiEE, ptr %0, i32 0, i32 1
  store ptr %struct.region9, ptr %vector24, align 8
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  ret void
}

define linkonce_odr void @_ZN5ArrayI12KeyValuePairI6StringiEE3addE12KeyValuePairI6StringiE(ptr %0, ptr %1) {
entry:
  %load.struct = load %_Z5ArrayI12KeyValuePairI6StringiEE, ptr %0, align 8
  %vector = extractvalue %_Z5ArrayI12KeyValuePairI6StringiEE %load.struct, 1
  %eq = icmp eq ptr %vector, null
  br i1 %eq, label %lor.end, label %lor.rhs

lor.rhs:                                          ; preds = %entry
  %load.struct1 = load %_Z5ArrayI12KeyValuePairI6StringiEE, ptr %0, align 8
  %length = extractvalue %_Z5ArrayI12KeyValuePairI6StringiEE %load.struct1, 0
  %load.struct2 = load %_Z5ArrayI12KeyValuePairI6StringiEE, ptr %0, align 8
  %vector3 = extractvalue %_Z5ArrayI12KeyValuePairI6StringiEE %load.struct2, 1
  %deref = load %_Z6VectorI12KeyValuePairI6StringiEE, ptr %vector3, align 8
  %length4 = extractvalue %_Z6VectorI12KeyValuePairI6StringiEE %deref, 0
  %eq5 = icmp eq i64 %length, %length4
  br label %lor.end

lor.end:                                          ; preds = %lor.rhs, %entry
  %lor.result = phi i1 [ true, %entry ], [ %eq5, %lor.rhs ]
  br i1 %lor.result, label %if.then, label %if.end

if.then:                                          ; preds = %lor.end
  call void @_ZN5ArrayI12KeyValuePairI6StringiEE10reallocateEv(ptr %0)
  br label %if.end

if.end:                                           ; preds = %if.then, %lor.end
  %load.struct6 = load %_Z5ArrayI12KeyValuePairI6StringiEE, ptr %0, align 8
  %vector7 = extractvalue %_Z5ArrayI12KeyValuePairI6StringiEE %load.struct6, 1
  %deref8 = load %_Z6VectorI12KeyValuePairI6StringiEE, ptr %vector7, align 8
  %data = extractvalue %_Z6VectorI12KeyValuePairI6StringiEE %deref8, 1
  %load.struct9 = load %_Z5ArrayI12KeyValuePairI6StringiEE, ptr %0, align 8
  %length10 = extractvalue %_Z5ArrayI12KeyValuePairI6StringiEE %load.struct9, 0
  %ptr.add = getelementptr inbounds %_Z12KeyValuePairI6StringiE, ptr %data, i64 %length10
  %store.load = load %_Z12KeyValuePairI6StringiE, ptr %1, align 8
  store %_Z12KeyValuePairI6StringiE %store.load, ptr %ptr.add, align 8
  %load.struct11 = load %_Z5ArrayI12KeyValuePairI6StringiEE, ptr %0, align 8
  %length12 = extractvalue %_Z5ArrayI12KeyValuePairI6StringiEE %load.struct11, 0
  %add = add i64 %length12, 1
  %length13 = getelementptr inbounds %_Z5ArrayI12KeyValuePairI6StringiEE, ptr %0, i32 0, i32 0
  store i64 %add, ptr %length13, align 4
  ret void
}

define linkonce_odr void @_ZN5ArrayI12KeyValuePairI6StringiEE3addE6VectorI12KeyValuePairI6StringiEE(ptr %0, ptr %1) {
entry:
  %own_page = alloca ptr, align 8
  %load.struct = load %_Z5ArrayI12KeyValuePairI6StringiEE, ptr %0, align 8
  %length = extractvalue %_Z5ArrayI12KeyValuePairI6StringiEE %load.struct, 0
  %load.struct1 = load %_Z6VectorI12KeyValuePairI6StringiEE, ptr %1, align 8
  %length2 = extractvalue %_Z6VectorI12KeyValuePairI6StringiEE %load.struct1, 0
  %add = add i64 %length, %length2
  %new_length = alloca i64, align 8
  store i64 %add, ptr %new_length, align 1
  %new_length3 = load i64, ptr %new_length, align 4
  %load.struct4 = load %_Z5ArrayI12KeyValuePairI6StringiEE, ptr %0, align 8
  %length5 = extractvalue %_Z5ArrayI12KeyValuePairI6StringiEE %load.struct4, 0
  %lt = icmp ult i64 %new_length3, %length5
  br i1 %lt, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  call void @exit(i64 14)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %load.struct6 = load %_Z5ArrayI12KeyValuePairI6StringiEE, ptr %0, align 8
  %vector = extractvalue %_Z5ArrayI12KeyValuePairI6StringiEE %load.struct6, 1
  %eq = icmp eq ptr %vector, null
  br i1 %eq, label %lor.end, label %lor.rhs

lor.rhs:                                          ; preds = %if.end
  %new_length7 = load i64, ptr %new_length, align 4
  %load.struct8 = load %_Z5ArrayI12KeyValuePairI6StringiEE, ptr %0, align 8
  %vector9 = extractvalue %_Z5ArrayI12KeyValuePairI6StringiEE %load.struct8, 1
  %deref = load %_Z6VectorI12KeyValuePairI6StringiEE, ptr %vector9, align 8
  %length10 = extractvalue %_Z6VectorI12KeyValuePairI6StringiEE %deref, 0
  %gt = icmp ugt i64 %new_length7, %length10
  br label %lor.end

lor.end:                                          ; preds = %lor.rhs, %if.end
  %lor.result = phi i1 [ true, %if.end ], [ %gt, %lor.rhs ]
  br i1 %lor.result, label %if.then11, label %if.end12

if.then11:                                        ; preds = %lor.end
  call void @_ZN5ArrayI12KeyValuePairI6StringiEE10reallocateEv(ptr %0)
  br label %if.end12

if.end12:                                         ; preds = %if.then11, %lor.end
  %new_length13 = load i64, ptr %new_length, align 4
  %load.struct14 = load %_Z5ArrayI12KeyValuePairI6StringiEE, ptr %0, align 8
  %vector15 = extractvalue %_Z5ArrayI12KeyValuePairI6StringiEE %load.struct14, 1
  %deref16 = load %_Z6VectorI12KeyValuePairI6StringiEE, ptr %vector15, align 8
  %length17 = extractvalue %_Z6VectorI12KeyValuePairI6StringiEE %deref16, 0
  %gt18 = icmp ugt i64 %new_length13, %length17
  br i1 %gt18, label %if.then19, label %if.end20

if.then19:                                        ; preds = %if.end12
  %call = call ptr @_ZN4Page3getEPv(ptr %0)
  store ptr %call, ptr %own_page, align 1
  %own_page21 = load ptr, ptr %own_page, align 8
  %new_length22 = load i64, ptr %new_length, align 4
  %mul = mul i64 %new_length22, ptrtoint (ptr getelementptr (%_Z12KeyValuePairI6StringiE, ptr null, i32 1) to i64)
  %call23 = call ptr @_ZN4Page8allocateEmm(ptr %own_page21, i64 %mul, i64 8)
  %load.struct24 = load %_Z5ArrayI12KeyValuePairI6StringiEE, ptr %0, align 8
  %length25 = extractvalue %_Z5ArrayI12KeyValuePairI6StringiEE %load.struct24, 0
  %mul26 = mul i64 %length25, ptrtoint (ptr getelementptr (%_Z12KeyValuePairI6StringiE, ptr null, i32 1) to i64)
  %load.struct27 = load %_Z5ArrayI12KeyValuePairI6StringiEE, ptr %0, align 8
  %length28 = extractvalue %_Z5ArrayI12KeyValuePairI6StringiEE %load.struct27, 0
  %gt29 = icmp ugt i64 %length28, 0
  br i1 %gt29, label %if.then30, label %if.end31

if.end20:                                         ; preds = %if.end31, %if.end12
  %load.struct48 = load %_Z6VectorI12KeyValuePairI6StringiEE, ptr %1, align 8
  %length49 = extractvalue %_Z6VectorI12KeyValuePairI6StringiEE %load.struct48, 0
  %gt50 = icmp ugt i64 %length49, 0
  br i1 %gt50, label %if.then51, label %if.end52

if.then30:                                        ; preds = %if.then19
  %field.inplace = getelementptr inbounds %_Z5ArrayI12KeyValuePairI6StringiEE, ptr %0, i32 0, i32 1
  %load.struct32 = load %_Z5ArrayI12KeyValuePairI6StringiEE, ptr %0, align 8
  %vector33 = extractvalue %_Z5ArrayI12KeyValuePairI6StringiEE %load.struct32, 1
  %deref34 = load %_Z6VectorI12KeyValuePairI6StringiEE, ptr %vector33, align 8
  %data = extractvalue %_Z6VectorI12KeyValuePairI6StringiEE %deref34, 1
  %call35 = call ptr @memcpy(ptr %call23, ptr %data, i64 %mul26)
  br label %if.end31

if.end31:                                         ; preds = %if.then30, %if.then19
  %load.struct36 = load %_Z5ArrayI12KeyValuePairI6StringiEE, ptr %0, align 8
  %vector37 = extractvalue %_Z5ArrayI12KeyValuePairI6StringiEE %load.struct36, 1
  %deref38 = load %_Z6VectorI12KeyValuePairI6StringiEE, ptr %vector37, align 8
  %data39 = extractvalue %_Z6VectorI12KeyValuePairI6StringiEE %deref38, 1
  %call40 = call ptr @_ZN4Page3getEPv(ptr %data39)
  %own_page41 = load ptr, ptr %own_page, align 8
  call void @_ZN4Page25deallocate_exclusive_pageEP4Page(ptr %own_page41, ptr %call40)
  %vector42 = getelementptr inbounds %_Z5ArrayI12KeyValuePairI6StringiEE, ptr %0, i32 0, i32 1
  %field.deref = load ptr, ptr %vector42, align 8
  %data43 = getelementptr inbounds %_Z6VectorI12KeyValuePairI6StringiEE, ptr %field.deref, i32 0, i32 1
  store ptr %call23, ptr %data43, align 8
  %new_length44 = load i64, ptr %new_length, align 4
  %vector45 = getelementptr inbounds %_Z5ArrayI12KeyValuePairI6StringiEE, ptr %0, i32 0, i32 1
  %field.deref46 = load ptr, ptr %vector45, align 8
  %length47 = getelementptr inbounds %_Z6VectorI12KeyValuePairI6StringiEE, ptr %field.deref46, i32 0, i32 0
  store i64 %new_length44, ptr %length47, align 4
  br label %if.end20

if.then51:                                        ; preds = %if.end20
  %load.struct53 = load %_Z5ArrayI12KeyValuePairI6StringiEE, ptr %0, align 8
  %vector54 = extractvalue %_Z5ArrayI12KeyValuePairI6StringiEE %load.struct53, 1
  %deref55 = load %_Z6VectorI12KeyValuePairI6StringiEE, ptr %vector54, align 8
  %data56 = extractvalue %_Z6VectorI12KeyValuePairI6StringiEE %deref55, 1
  %load.struct57 = load %_Z5ArrayI12KeyValuePairI6StringiEE, ptr %0, align 8
  %length58 = extractvalue %_Z5ArrayI12KeyValuePairI6StringiEE %load.struct57, 0
  %mul59 = mul i64 %length58, ptrtoint (ptr getelementptr (%_Z12KeyValuePairI6StringiE, ptr null, i32 1) to i64)
  %ptr.add = getelementptr inbounds %_Z12KeyValuePairI6StringiE, ptr %data56, i64 %mul59
  %field.inplace60 = getelementptr inbounds %_Z6VectorI12KeyValuePairI6StringiEE, ptr %1, i32 0, i32 1
  %deref.recv = load ptr, ptr %field.inplace60, align 8
  %load.struct61 = load %_Z6VectorI12KeyValuePairI6StringiEE, ptr %1, align 8
  %length62 = extractvalue %_Z6VectorI12KeyValuePairI6StringiEE %load.struct61, 0
  %mul63 = mul i64 %length62, ptrtoint (ptr getelementptr (%_Z12KeyValuePairI6StringiE, ptr null, i32 1) to i64)
  %call64 = call ptr @memcpy(ptr %ptr.add, ptr %deref.recv, i64 %mul63)
  br label %if.end52

if.end52:                                         ; preds = %if.then51, %if.end20
  %load.struct65 = load %_Z5ArrayI12KeyValuePairI6StringiEE, ptr %0, align 8
  %length66 = extractvalue %_Z5ArrayI12KeyValuePairI6StringiEE %load.struct65, 0
  %load.struct67 = load %_Z6VectorI12KeyValuePairI6StringiEE, ptr %1, align 8
  %length68 = extractvalue %_Z6VectorI12KeyValuePairI6StringiEE %load.struct67, 0
  %add69 = add i64 %length66, %length68
  %length70 = getelementptr inbounds %_Z5ArrayI12KeyValuePairI6StringiEE, ptr %0, i32 0, i32 0
  store i64 %add69, ptr %length70, align 4
  ret void
}

define linkonce_odr ptr @_ZN5ArrayI12KeyValuePairI6StringiEE3getEPN4scaly6memory4PageEm(ptr %0, ptr %1, i64 %2) {
entry:
  %load.struct = load %_Z5ArrayI12KeyValuePairI6StringiEE, ptr %1, align 8
  %length = extractvalue %_Z5ArrayI12KeyValuePairI6StringiEE %load.struct, 0
  %ge = icmp uge i64 %2, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %load.struct1 = load %_Z5ArrayI12KeyValuePairI6StringiEE, ptr %1, align 8
  %vector = extractvalue %_Z5ArrayI12KeyValuePairI6StringiEE %load.struct1, 1
  %deref = load %_Z6VectorI12KeyValuePairI6StringiEE, ptr %vector, align 8
  %data = extractvalue %_Z6VectorI12KeyValuePairI6StringiEE %deref, 1
  %ptr.add = getelementptr inbounds %_Z12KeyValuePairI6StringiE, ptr %data, i64 %2
  ret ptr %ptr.add
}

define linkonce_odr void @_ZN5ArrayI12KeyValuePairI6StringiEE3putEm12KeyValuePairI6StringiE(ptr %0, i64 %1, ptr %2) {
entry:
  %load.struct = load %_Z5ArrayI12KeyValuePairI6StringiEE, ptr %0, align 8
  %length = extractvalue %_Z5ArrayI12KeyValuePairI6StringiEE %load.struct, 0
  %ge = icmp uge i64 %1, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  call void @_Z11scaly_eputsP10const_char(ptr @.str.18)
  call void @_Z12scaly_eputnlv()
  call void @exit(i64 15)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %load.struct1 = load %_Z5ArrayI12KeyValuePairI6StringiEE, ptr %0, align 8
  %vector = extractvalue %_Z5ArrayI12KeyValuePairI6StringiEE %load.struct1, 1
  %deref = load %_Z6VectorI12KeyValuePairI6StringiEE, ptr %vector, align 8
  %data = extractvalue %_Z6VectorI12KeyValuePairI6StringiEE %deref, 1
  %ptr.add = getelementptr inbounds %_Z12KeyValuePairI6StringiE, ptr %data, i64 %1
  %store.load = load %_Z12KeyValuePairI6StringiE, ptr %2, align 8
  store %_Z12KeyValuePairI6StringiE %store.load, ptr %ptr.add, align 8
  ret void
}

define linkonce_odr ptr @_ZN13ArrayIteratorI12KeyValuePairI6StringiEE4nextEv(ptr %0) {
entry:
  %load.struct = load %_Z13ArrayIteratorI12KeyValuePairI6StringiEE, ptr %0, align 8
  %array = extractvalue %_Z13ArrayIteratorI12KeyValuePairI6StringiEE %load.struct, 0
  %eq = icmp eq ptr %array, null
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %load.struct1 = load %_Z13ArrayIteratorI12KeyValuePairI6StringiEE, ptr %0, align 8
  %position = extractvalue %_Z13ArrayIteratorI12KeyValuePairI6StringiEE %load.struct1, 1
  %load.struct2 = load %_Z13ArrayIteratorI12KeyValuePairI6StringiEE, ptr %0, align 8
  %array3 = extractvalue %_Z13ArrayIteratorI12KeyValuePairI6StringiEE %load.struct2, 0
  %deref = load %_Z5ArrayI12KeyValuePairI6StringiEE, ptr %array3, align 8
  %length = extractvalue %_Z5ArrayI12KeyValuePairI6StringiEE %deref, 0
  %eq4 = icmp eq i64 %position, %length
  br i1 %eq4, label %if.then5, label %if.end6

if.then5:                                         ; preds = %if.end
  ret ptr null

if.end6:                                          ; preds = %if.end
  %load.struct7 = load %_Z13ArrayIteratorI12KeyValuePairI6StringiEE, ptr %0, align 8
  %position8 = extractvalue %_Z13ArrayIteratorI12KeyValuePairI6StringiEE %load.struct7, 1
  %add = add i64 %position8, 1
  %position9 = getelementptr inbounds %_Z13ArrayIteratorI12KeyValuePairI6StringiEE, ptr %0, i32 0, i32 1
  store i64 %add, ptr %position9, align 4
  %field.inplace = getelementptr inbounds %_Z13ArrayIteratorI12KeyValuePairI6StringiEE, ptr %0, i32 0, i32 0
  %deref.recv = load ptr, ptr %field.inplace, align 8
  %call = call ptr @_ZN5ArrayI12KeyValuePairI6StringiEE10get_bufferEv(ptr %deref.recv)
  %load.struct10 = load %_Z13ArrayIteratorI12KeyValuePairI6StringiEE, ptr %0, align 8
  %position11 = extractvalue %_Z13ArrayIteratorI12KeyValuePairI6StringiEE %load.struct10, 1
  %sub = sub i64 %position11, 1
  %ptr.add = getelementptr inbounds %_Z12KeyValuePairI6StringiE, ptr %call, i64 %sub
  ret ptr %ptr.add
}

define linkonce_odr void @_ZN13ArrayIteratorI12KeyValuePairI6StringiEEC1EP5ArrayI12KeyValuePairI6StringiEE(ptr %0, ptr %1) {
entry:
  %array = getelementptr inbounds %_Z13ArrayIteratorI12KeyValuePairI6StringiEE, ptr %0, i32 0, i32 0
  store ptr %1, ptr %array, align 8
  %position = getelementptr inbounds %_Z13ArrayIteratorI12KeyValuePairI6StringiEE, ptr %0, i32 0, i32 1
  store i64 0, ptr %position, align 4
  ret void
}

define linkonce_odr void @_ZN13ArrayIteratorI12KeyValuePairI6StringiEEC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr void @_ZN5ArrayI12KeyValuePairI6StringiEE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z13ArrayIteratorI12KeyValuePairI6StringiEE) %0, ptr %1, ptr %2) {
entry:
  %struct.init = alloca %_Z13ArrayIteratorI12KeyValuePairI6StringiEE, align 8
  call void @_ZN13ArrayIteratorI12KeyValuePairI6StringiEEC1EP5ArrayI12KeyValuePairI6StringiEE(ptr %struct.init, ptr %2)
  %sret.body = load %_Z13ArrayIteratorI12KeyValuePairI6StringiEE, ptr %struct.init, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %struct.init, i64 ptrtoint (ptr getelementptr (%_Z13ArrayIteratorI12KeyValuePairI6StringiEE, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN5ArrayI12KeyValuePairI6StringiEEC1Ev(ptr %0) {
entry:
  %length = getelementptr inbounds %_Z5ArrayI12KeyValuePairI6StringiEE, ptr %0, i32 0, i32 0
  store i64 0, ptr %length, align 4
  %vector = getelementptr inbounds %_Z5ArrayI12KeyValuePairI6StringiEE, ptr %0, i32 0, i32 1
  store ptr null, ptr %vector, align 8
  ret void
}

define linkonce_odr void @_ZN5ArrayI12KeyValuePairI6StringiEEC1Em(ptr %0, i64 %1) {
entry:
  %length = getelementptr inbounds %_Z5ArrayI12KeyValuePairI6StringiEE, ptr %0, i32 0, i32 0
  store i64 0, ptr %length, align 4
  %vector = getelementptr inbounds %_Z5ArrayI12KeyValuePairI6StringiEE, ptr %0, i32 0, i32 1
  store ptr null, ptr %vector, align 8
  %call = call ptr @_ZN4Page3getEPv(ptr %0)
  %call1 = call ptr @_ZN4Page23allocate_exclusive_pageEv(ptr %call)
  %struct.region = call ptr @_ZN4Page8allocateEmm(ptr %call1, i64 ptrtoint (ptr getelementptr (%_Z6VectorI12KeyValuePairI6StringiEE, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr ({ i1, %_Z6VectorI12KeyValuePairI6StringiEE }, ptr null, i64 0, i32 1) to i64))
  call void @_ZN6VectorI12KeyValuePairI6StringiEEC1EPN4scaly6memory4PageEm(ptr %struct.region, ptr %call1, i64 %1)
  %vector2 = getelementptr inbounds %_Z5ArrayI12KeyValuePairI6StringiEE, ptr %0, i32 0, i32 1
  store ptr %struct.region, ptr %vector2, align 8
  ret void
}

define linkonce_odr void @_ZN5ArrayI12KeyValuePairI6StringiEEC1EPN4scaly6memory4PageE(ptr %0, ptr %1) {
entry:
  %length = getelementptr inbounds %_Z5ArrayI12KeyValuePairI6StringiEE, ptr %0, i32 0, i32 0
  store i64 0, ptr %length, align 4
  %vector = getelementptr inbounds %_Z5ArrayI12KeyValuePairI6StringiEE, ptr %0, i32 0, i32 1
  store ptr null, ptr %vector, align 8
  ret void
}

define linkonce_odr void @_ZN5ArrayI12KeyValuePairI6StringiEEC1EPN4scaly6memory4PageEm(ptr %0, ptr %1, i64 %2) {
entry:
  %length = getelementptr inbounds %_Z5ArrayI12KeyValuePairI6StringiEE, ptr %0, i32 0, i32 0
  store i64 0, ptr %length, align 4
  %vector = getelementptr inbounds %_Z5ArrayI12KeyValuePairI6StringiEE, ptr %0, i32 0, i32 1
  store ptr null, ptr %vector, align 8
  %call = call ptr @_ZN4Page23allocate_exclusive_pageEv(ptr %1)
  %struct.region = call ptr @_ZN4Page8allocateEmm(ptr %call, i64 ptrtoint (ptr getelementptr (%_Z6VectorI12KeyValuePairI6StringiEE, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr ({ i1, %_Z6VectorI12KeyValuePairI6StringiEE }, ptr null, i64 0, i32 1) to i64))
  call void @_ZN6VectorI12KeyValuePairI6StringiEEC1EPN4scaly6memory4PageEm(ptr %struct.region, ptr %call, i64 %2)
  %vector1 = getelementptr inbounds %_Z5ArrayI12KeyValuePairI6StringiEE, ptr %0, i32 0, i32 1
  store ptr %struct.region, ptr %vector1, align 8
  ret void
}

define linkonce_odr ptr @_ZN6VectorI12KeyValuePairI1K1VEE3getEPN4scaly6memory4PageEm(ptr %0, ptr %1, i64 %2) {
entry:
  ret ptr null
}

define linkonce_odr ptr @_ZN6VectorI12KeyValuePairI1K1VEE7get_ptrEm(ptr %0, i64 %1) {
entry:
  ret ptr null
}

define linkonce_odr void @_ZN6VectorI12KeyValuePairI1K1VEE3putEm12KeyValuePairI1K1VE(ptr %0, i64 %1, ptr %2) {
entry:
  ret void
}

define linkonce_odr ptr @_ZN14VectorIteratorI12KeyValuePairI1K1VEE4nextEv(ptr %0) {
entry:
  ret ptr null
}

define linkonce_odr void @_ZN14VectorIteratorI12KeyValuePairI1K1VEEC1EP6VectorI12KeyValuePairI1K1VEE(ptr %0, ptr %1) {
entry:
  ret void
}

define linkonce_odr void @_ZN14VectorIteratorI12KeyValuePairI1K1VEEC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr void @_ZN6VectorI12KeyValuePairI1K1VEE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z14VectorIteratorI12KeyValuePairI1K1VEE) %0, ptr %1, ptr %2) {
entry:
  ret void
}

define linkonce_odr void @_ZN6VectorI12KeyValuePairI1K1VEE8as_sliceEPN4scaly6memory4PageE(ptr noalias sret(%_Z5Slice) %0, ptr %1, ptr %2) {
entry:
  ret void
}

define linkonce_odr void @_ZN6VectorI12KeyValuePairI1K1VEEC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr void @_ZN6VectorI12KeyValuePairI1K1VEEC1EP12KeyValuePairI1K1VEm(ptr %0, ptr %1, i64 %2) {
entry:
  ret void
}

define linkonce_odr void @_ZN6VectorI12KeyValuePairI1K1VEEC1EPN4scaly6memory4PageEm(ptr %0, ptr %1, i64 %2) {
entry:
  ret void
}

define linkonce_odr void @_ZN6VectorI12KeyValuePairI1K1VEEC1EPN4scaly6memory4PageEP12KeyValuePairI1K1VEm(ptr %0, ptr %1, ptr %2, i64 %3) {
entry:
  ret void
}

define linkonce_odr void @_ZN6VectorI12KeyValuePairI1K1VEEC1EPN4scaly6memory4PageE6VectorI12KeyValuePairI1K1VEE(ptr %0, ptr %1, ptr %2) {
entry:
  ret void
}

define linkonce_odr ptr @_ZN5ArrayI12KeyValuePairI1K1VEE10get_bufferEv(ptr %0) {
entry:
  ret ptr null
}

define linkonce_odr i64 @_ZN5ArrayI12KeyValuePairI1K1VEE10get_lengthEv(ptr %0) {
entry:
  ret i64 0
}

define linkonce_odr i64 @_ZN5ArrayI12KeyValuePairI1K1VEE12get_capacityEv(ptr %0) {
entry:
  ret i64 0
}

define linkonce_odr void @_ZN5ArrayI12KeyValuePairI1K1VEE10reallocateEv(ptr %0) {
entry:
  ret void
}

define linkonce_odr void @_ZN5ArrayI12KeyValuePairI1K1VEE3addE12KeyValuePairI1K1VE(ptr %0, ptr %1) {
entry:
  ret void
}

define linkonce_odr void @_ZN5ArrayI12KeyValuePairI1K1VEE3addE6VectorI12KeyValuePairI1K1VEE(ptr %0, ptr %1) {
entry:
  ret void
}

define linkonce_odr ptr @_ZN5ArrayI12KeyValuePairI1K1VEE3getEPN4scaly6memory4PageEm(ptr %0, ptr %1, i64 %2) {
entry:
  ret ptr null
}

define linkonce_odr void @_ZN5ArrayI12KeyValuePairI1K1VEE3putEm12KeyValuePairI1K1VE(ptr %0, i64 %1, ptr %2) {
entry:
  ret void
}

define linkonce_odr ptr @_ZN13ArrayIteratorI12KeyValuePairI1K1VEE4nextEv(ptr %0) {
entry:
  ret ptr null
}

define linkonce_odr void @_ZN13ArrayIteratorI12KeyValuePairI1K1VEEC1EP5ArrayI12KeyValuePairI1K1VEE(ptr %0, ptr %1) {
entry:
  ret void
}

define linkonce_odr void @_ZN13ArrayIteratorI12KeyValuePairI1K1VEEC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr void @_ZN5ArrayI12KeyValuePairI1K1VEE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z13ArrayIteratorI12KeyValuePairI1K1VEE) %0, ptr %1, ptr %2) {
entry:
  ret void
}

define linkonce_odr void @_ZN5ArrayI12KeyValuePairI1K1VEEC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr void @_ZN5ArrayI12KeyValuePairI1K1VEEC1Em(ptr %0, i64 %1) {
entry:
  ret void
}

define linkonce_odr void @_ZN5ArrayI12KeyValuePairI1K1VEEC1EPN4scaly6memory4PageE(ptr %0, ptr %1) {
entry:
  ret void
}

define linkonce_odr void @_ZN5ArrayI12KeyValuePairI1K1VEEC1EPN4scaly6memory4PageEm(ptr %0, ptr %1, i64 %2) {
entry:
  ret void
}

define linkonce_odr void @_ZN6VectorI12KeyValuePairI1K1VEEC1EPN4scaly6memory4PageE5ArrayI12KeyValuePairI1K1VEE(ptr %0, ptr %1, ptr %2) {
entry:
  ret void
}

define linkonce_odr ptr @_ZN4ListI12KeyValuePairI1K1VEE8get_headEPN4scaly6memory4PageE(ptr %0, ptr %1) {
entry:
  ret ptr null
}

define linkonce_odr ptr @_ZN12ListIteratorI12KeyValuePairI1K1VEE4nextEv(ptr %0) {
entry:
  ret ptr null
}

define linkonce_odr void @_ZN12ListIteratorI12KeyValuePairI1K1VEEC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr i64 @_ZN4ListI12KeyValuePairI1K1VEE5countEv(ptr %0) {
entry:
  ret i64 0
}

define linkonce_odr i1 @_ZN4ListI12KeyValuePairI1K1VEE6removeE12KeyValuePairI1K1VE(ptr %0, ptr %1) {
entry:
  ret i1 false
}

define linkonce_odr void @_ZN4ListI12KeyValuePairI1K1VEE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z12ListIteratorI12KeyValuePairI1K1VEE) %0, ptr %1, ptr %2) {
entry:
  ret void
}

define linkonce_odr void @_ZN4ListI12KeyValuePairI1K1VEE3addE12KeyValuePairI1K1VE(ptr %0, ptr %1) {
entry:
  ret void
}

define linkonce_odr void @_ZN4ListI12KeyValuePairI1K1VEEC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr void @_ZN6VectorI12KeyValuePairI1K1VEEC1EPN4scaly6memory4PageE4ListI12KeyValuePairI1K1VEE(ptr %0, ptr %1, ptr %2) {
entry:
  ret void
}

declare i1 @_ZN14HashMapBuilderI6StringiE8containsE6String(ptr, ptr)

declare i1 @_ZN7HashMapI6StringiE8containsE6String(ptr, ptr)

declare i1 @_ZN14HashMapBuilderI6StringmE3addE6Stringm(ptr, ptr, i64)

declare i1 @_ZN14HashMapBuilderI6StringmE8containsE6String(ptr, ptr)

declare void @_ZN5SliceIiE8subsliceEPN4scaly6memory4PageEmm(ptr noalias sret(%_Z5Slice), ptr, ptr, i64, i64)

define linkonce_odr void @_ZN6String8as_sliceEPN4scaly6memory4PageE(ptr noalias sret(%_Z5Slice) %0, ptr %1, ptr %2) {
entry:
  %tuple = alloca { ptr, i64 }, align 8
  %byte = alloca i8, align 1
  %index = alloca i64, align 8
  %bit_count = alloca i64, align 8
  %string_length = alloca i64, align 8
  %struct.init = alloca %_Z5Slice, align 8
  %load.struct = load %_Z6String, ptr %2, align 8
  %data = extractvalue %_Z6String %load.struct, 0
  %eq = icmp eq ptr %data, null
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %tuple.field = getelementptr inbounds %_Z5Slice, ptr %struct.init, i32 0, i32 0
  store ptr null, ptr %tuple.field, align 8
  %tuple.field1 = getelementptr inbounds %_Z5Slice, ptr %struct.init, i32 0, i32 1
  store i64 0, ptr %tuple.field1, align 4
  %sret.body = load %_Z5Slice, ptr %struct.init, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %struct.init, i64 ptrtoint (ptr getelementptr (%_Z5Slice, ptr null, i32 1) to i64), i1 false)
  ret void

if.end:                                           ; preds = %entry
  store i64 0, ptr %string_length, align 1
  store i64 0, ptr %bit_count, align 1
  store i64 0, ptr %index, align 1
  br label %while.cond

while.cond:                                       ; preds = %if.end16, %if.end
  br i1 true, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %bit_count2 = load i64, ptr %bit_count, align 4
  %eq3 = icmp eq i64 %bit_count2, 63
  br i1 %eq3, label %if.then4, label %if.end5

while.exit:                                       ; preds = %if.then15, %while.cond
  %load.struct20 = load %_Z6String, ptr %2, align 8
  %data21 = extractvalue %_Z6String %load.struct20, 0
  %index22 = load i64, ptr %index, align 4
  %ptr.add23 = getelementptr inbounds i8, ptr %data21, i64 %index22
  %ptr.add24 = getelementptr inbounds i8, ptr %ptr.add23, i64 1
  %string_length25 = load i64, ptr %string_length, align 4
  %tuple.field26 = getelementptr inbounds { ptr, i64 }, ptr %tuple, i32 0, i32 0
  store ptr %ptr.add24, ptr %tuple.field26, align 1
  %tuple.field27 = getelementptr inbounds { ptr, i64 }, ptr %tuple, i32 0, i32 1
  store i64 %string_length25, ptr %tuple.field27, align 1
  %tuple.val = load { ptr, i64 }, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr ({ ptr, i64 }, ptr null, i32 1) to i64), i1 false)
  ret void

if.then4:                                         ; preds = %while.body
  call void @exit(i64 11)
  br label %if.end5

if.end5:                                          ; preds = %if.then4, %while.body
  %load.struct6 = load %_Z6String, ptr %2, align 8
  %data7 = extractvalue %_Z6String %load.struct6, 0
  %index8 = load i64, ptr %index, align 4
  %ptr.add = getelementptr inbounds i8, ptr %data7, i64 %index8
  %deref = load i8, ptr %ptr.add, align 1
  store i8 %deref, ptr %byte, align 1
  %string_length9 = load i64, ptr %string_length, align 4
  %byte10 = load i8, ptr %byte, align 1
  %and = and i8 %byte10, 127
  %as.zext = zext i8 %and to i64
  %bit_count11 = load i64, ptr %bit_count, align 4
  %shl = shl i64 %as.zext, %bit_count11
  %or = or i64 %string_length9, %shl
  store i64 %or, ptr %string_length, align 1
  %byte12 = load i8, ptr %byte, align 1
  %and13 = and i8 %byte12, -128
  %eq14 = icmp eq i8 %and13, 0
  br i1 %eq14, label %if.then15, label %if.end16

if.then15:                                        ; preds = %if.end5
  br label %while.exit

if.end16:                                         ; preds = %if.end5
  %bit_count17 = load i64, ptr %bit_count, align 4
  %add = add i64 %bit_count17, 7
  store i64 %add, ptr %bit_count, align 1
  %index18 = load i64, ptr %index, align 4
  %add19 = add i64 %index18, 1
  store i64 %add19, ptr %index, align 1
  br label %while.cond
}

define linkonce_odr i1 @_ZN6String8containsE2u8(ptr %0, i8 %1) {
entry:
  %i = alloca i64, align 8
  %call = call ptr @_ZN6String10get_bufferEv(ptr %0)
  %eq = icmp eq ptr %call, null
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret i1 false

if.end:                                           ; preds = %entry
  %call1 = call i64 @_ZN6String10get_lengthEv(ptr %0)
  store i64 0, ptr %i, align 1
  br label %while.cond

while.cond:                                       ; preds = %if.end6, %if.end
  %i2 = load i64, ptr %i, align 4
  %lt = icmp ult i64 %i2, %call1
  br i1 %lt, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %i3 = load i64, ptr %i, align 4
  %ptr.add = getelementptr inbounds i8, ptr %call, i64 %i3
  %deref = load i8, ptr %ptr.add, align 1
  %eq4 = icmp eq i8 %deref, %1
  br i1 %eq4, label %if.then5, label %if.end6

while.exit:                                       ; preds = %while.cond
  ret i1 false

if.then5:                                         ; preds = %while.body
  ret i1 true

if.end6:                                          ; preds = %while.body
  %i7 = load i64, ptr %i, align 4
  %add = add i64 %i7, 1
  store i64 %add, ptr %i, align 1
  br label %while.cond
}

define linkonce_odr i1 @_ZN6String8containsE6String(ptr %0, ptr %1) {
entry:
  %i = alloca i64, align 8
  %call = call i64 @_ZN6String10get_lengthEv(ptr %0)
  %call1 = call i64 @_ZN6String10get_lengthEv(ptr %1)
  %eq = icmp eq i64 %call1, 0
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret i1 true

if.end:                                           ; preds = %entry
  %gt = icmp ugt i64 %call1, %call
  br i1 %gt, label %if.then2, label %if.end3

if.then2:                                         ; preds = %if.end
  ret i1 false

if.end3:                                          ; preds = %if.end
  %call4 = call ptr @_ZN6String10get_bufferEv(ptr %0)
  %call5 = call ptr @_ZN6String10get_bufferEv(ptr %1)
  store i64 0, ptr %i, align 1
  br label %while.cond

while.cond:                                       ; preds = %if.end11, %if.end3
  %i6 = load i64, ptr %i, align 4
  %sub = sub i64 %call, %call1
  %le = icmp ule i64 %i6, %sub
  br i1 %le, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %i7 = load i64, ptr %i, align 4
  %ptr.add = getelementptr inbounds i8, ptr %call4, i64 %i7
  %call8 = call i32 @memcmp(ptr %ptr.add, ptr %call5, i64 %call1)
  %zext = zext i32 %call8 to i64
  %eq9 = icmp eq i64 %zext, 0
  br i1 %eq9, label %if.then10, label %if.end11

while.exit:                                       ; preds = %while.cond
  ret i1 false

if.then10:                                        ; preds = %while.body
  ret i1 true

if.end11:                                         ; preds = %while.body
  %i12 = load i64, ptr %i, align 4
  %add = add i64 %i12, 1
  store i64 %add, ptr %i, align 1
  br label %while.cond
}

define linkonce_odr i1 @_ZN6String11starts_withE6String(ptr %0, ptr %1) {
entry:
  %call = call i64 @_ZN6String10get_lengthEv(ptr %0)
  %call1 = call i64 @_ZN6String10get_lengthEv(ptr %1)
  %gt = icmp ugt i64 %call1, %call
  br i1 %gt, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret i1 false

if.end:                                           ; preds = %entry
  %eq = icmp eq i64 %call1, 0
  br i1 %eq, label %if.then2, label %if.end3

if.then2:                                         ; preds = %if.end
  ret i1 true

if.end3:                                          ; preds = %if.end
  %call4 = call ptr @_ZN6String10get_bufferEv(ptr %0)
  %call5 = call ptr @_ZN6String10get_bufferEv(ptr %1)
  %call6 = call i32 @memcmp(ptr %call4, ptr %call5, i64 %call1)
  %zext = zext i32 %call6 to i64
  %eq7 = icmp eq i64 %zext, 0
  ret i1 %eq7
}

define linkonce_odr i1 @_ZN6String9ends_withE6String(ptr %0, ptr %1) {
entry:
  %call = call i64 @_ZN6String10get_lengthEv(ptr %0)
  %call1 = call i64 @_ZN6String10get_lengthEv(ptr %1)
  %gt = icmp ugt i64 %call1, %call
  br i1 %gt, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret i1 false

if.end:                                           ; preds = %entry
  %eq = icmp eq i64 %call1, 0
  br i1 %eq, label %if.then2, label %if.end3

if.then2:                                         ; preds = %if.end
  ret i1 true

if.end3:                                          ; preds = %if.end
  %call4 = call ptr @_ZN6String10get_bufferEv(ptr %0)
  %call5 = call ptr @_ZN6String10get_bufferEv(ptr %1)
  %ptr.add = getelementptr inbounds i8, ptr %call4, i64 %call
  %neg = sub i64 0, %call1
  %ptr.sub = getelementptr inbounds i8, ptr %ptr.add, i64 %neg
  %call6 = call i32 @memcmp(ptr %ptr.sub, ptr %call5, i64 %call1)
  %zext = zext i32 %call6 to i64
  %eq7 = icmp eq i64 %zext, 0
  ret i1 %eq7
}

define linkonce_odr i64 @_ZN6String8index_ofE2u8(ptr %0, i8 %1) {
entry:
  %i = alloca i64, align 8
  %call = call ptr @_ZN6String10get_bufferEv(ptr %0)
  %eq = icmp eq ptr %call, null
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret i64 -1

if.end:                                           ; preds = %entry
  %call1 = call i64 @_ZN6String10get_lengthEv(ptr %0)
  store i64 0, ptr %i, align 1
  br label %while.cond

while.cond:                                       ; preds = %if.end6, %if.end
  %i2 = load i64, ptr %i, align 4
  %lt = icmp ult i64 %i2, %call1
  br i1 %lt, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %i3 = load i64, ptr %i, align 4
  %ptr.add = getelementptr inbounds i8, ptr %call, i64 %i3
  %deref = load i8, ptr %ptr.add, align 1
  %eq4 = icmp eq i8 %deref, %1
  br i1 %eq4, label %if.then5, label %if.end6

while.exit:                                       ; preds = %while.cond
  ret i64 -1

if.then5:                                         ; preds = %while.body
  %i7 = load i64, ptr %i, align 4
  ret i64 %i7

if.end6:                                          ; preds = %while.body
  %i8 = load i64, ptr %i, align 4
  %add = add i64 %i8, 1
  store i64 %add, ptr %i, align 1
  br label %while.cond
}

define linkonce_odr i64 @_ZN6String8index_ofE2u8m(ptr %0, i8 %1, i64 %2) {
entry:
  %i = alloca i64, align 8
  %call = call ptr @_ZN6String10get_bufferEv(ptr %0)
  %eq = icmp eq ptr %call, null
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret i64 -1

if.end:                                           ; preds = %entry
  %call1 = call i64 @_ZN6String10get_lengthEv(ptr %0)
  %ge = icmp uge i64 %2, %call1
  br i1 %ge, label %if.then2, label %if.end3

if.then2:                                         ; preds = %if.end
  ret i64 -1

if.end3:                                          ; preds = %if.end
  store i64 %2, ptr %i, align 1
  br label %while.cond

while.cond:                                       ; preds = %if.end8, %if.end3
  %i4 = load i64, ptr %i, align 4
  %lt = icmp ult i64 %i4, %call1
  br i1 %lt, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %i5 = load i64, ptr %i, align 4
  %ptr.add = getelementptr inbounds i8, ptr %call, i64 %i5
  %deref = load i8, ptr %ptr.add, align 1
  %eq6 = icmp eq i8 %deref, %1
  br i1 %eq6, label %if.then7, label %if.end8

while.exit:                                       ; preds = %while.cond
  ret i64 -1

if.then7:                                         ; preds = %while.body
  %i9 = load i64, ptr %i, align 4
  ret i64 %i9

if.end8:                                          ; preds = %while.body
  %i10 = load i64, ptr %i, align 4
  %add = add i64 %i10, 1
  store i64 %add, ptr %i, align 1
  br label %while.cond
}

declare i1 @_ZN6OptionIiE7is_someEv(ptr)

declare i1 @_ZN6OptionIiE7is_noneEv(ptr)

declare i64 @_ZN6OptionIiE9unwrap_orEi(ptr, i64)

declare i1 @_ZN6OptionIPiE7is_someEv(ptr)

declare i1 @_ZN6ResultIibE5is_okEv(ptr)

declare i1 @_ZN6ResultIibE8is_errorEv(ptr)

declare i64 @_ZN6ResultIibE6unwrapEv(ptr)

declare i1 @_ZN6ResultIibE12unwrap_errorEv(ptr)

declare i64 @_ZN6ResultIibE9unwrap_orEi(ptr, i64)

define linkonce_odr i1 @_ZN7hashing8is_primeEm(i64 %0) {
entry:
  %divisor = alloca i64, align 8
  %and = and i64 %0, 1
  %ne = icmp ne i64 %and, 0
  br i1 %ne, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  store i64 3, ptr %divisor, align 1
  br label %while.cond

if.end:                                           ; preds = %entry
  %eq7 = icmp eq i64 %0, 2
  ret i1 %eq7

while.cond:                                       ; preds = %if.end5, %if.then
  %divisor1 = load i64, ptr %divisor, align 4
  %divisor2 = load i64, ptr %divisor, align 4
  %mul = mul i64 %divisor1, %divisor2
  %le = icmp ule i64 %mul, %0
  br i1 %le, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %divisor3 = load i64, ptr %divisor, align 4
  %urem = urem i64 %0, %divisor3
  %eq = icmp eq i64 %urem, 0
  br i1 %eq, label %if.then4, label %if.end5

while.exit:                                       ; preds = %while.cond
  ret i1 true

if.then4:                                         ; preds = %while.body
  ret i1 false

if.end5:                                          ; preds = %while.body
  %divisor6 = load i64, ptr %divisor, align 4
  %add = add i64 %divisor6, 2
  store i64 %add, ptr %divisor, align 1
  br label %while.cond
}

define linkonce_odr i64 @_ZN7hashing9get_primeEm(i64 %0) {
entry:
  %or = or i64 %0, 1
  %i = alloca i64, align 8
  store i64 %or, ptr %i, align 1
  br label %while.cond

while.cond:                                       ; preds = %entry
  %i1 = load i64, ptr %i, align 4
  ret i64 %0

while.body:                                       ; No predecessors!
  unreachable

while.exit:                                       ; No predecessors!
  unreachable
}

define linkonce_odr i64 @_ZN7hashing4hashEPcm(ptr %0, i64 %1) {
entry:
  %hash = alloca i64, align 8
  store i64 -3750763034362895579, ptr %hash, align 1
  %prime = alloca i64, align 8
  store i64 1099511628211, ptr %prime, align 1
  %i = alloca i64, align 8
  store i64 0, ptr %i, align 1
  br label %while.cond

while.cond:                                       ; preds = %while.body, %entry
  %i1 = load i64, ptr %i, align 4
  %lt = icmp ult i64 %i1, %1
  br i1 %lt, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %i2 = load i64, ptr %i, align 4
  %ptr.add = getelementptr inbounds i8, ptr %0, i64 %i2
  %deref = load i8, ptr %ptr.add, align 1
  %as.zext = zext i8 %deref to i64
  %hash3 = load i64, ptr %hash, align 4
  %or = or i64 %hash3, %as.zext
  %hash4 = load i64, ptr %hash, align 4
  %and = and i64 %hash4, %as.zext
  %sub = sub i64 %or, %and
  %prime5 = load i64, ptr %prime, align 4
  %mul = mul i64 %sub, %prime5
  store i64 %mul, ptr %hash, align 1
  %i6 = load i64, ptr %i, align 4
  %add = add i64 %i6, 1
  store i64 %add, ptr %i, align 1
  br label %while.cond

while.exit:                                       ; preds = %while.cond
  %hash7 = load i64, ptr %hash, align 4
  ret i64 %hash7
}

define linkonce_odr ptr @_ZN5Slice3getEPN4scaly6memory4PageEm(ptr %0, ptr %1, i64 %2) {
entry:
  %load.struct = load %_Z5Slice, ptr %1, align 8
  %length = extractvalue %_Z5Slice %load.struct, 1
  %ge = icmp uge i64 %2, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %load.struct1 = load %_Z5Slice, ptr %1, align 8
  %data = extractvalue %_Z5Slice %load.struct1, 0
  %ptr.add = getelementptr inbounds ptr, ptr %data, i64 %2
  ret ptr %ptr.add
}

define linkonce_odr i1 @_ZN5Slice8is_emptyEv(ptr %0) {
entry:
  %load.struct = load %_Z5Slice, ptr %0, align 8
  %length = extractvalue %_Z5Slice %load.struct, 1
  %eq = icmp eq i64 %length, 0
  ret i1 %eq
}

define linkonce_odr void @_ZN5Slice8subsliceEPN4scaly6memory4PageEmm(ptr noalias sret(%_Z5Slice) %0, ptr %1, ptr %2, i64 %3, i64 %4) {
entry:
  %tuple = alloca { ptr, i64 }, align 8
  %from = alloca i64, align 8
  store i64 %3, ptr %from, align 1
  %to = alloca i64, align 8
  store i64 %4, ptr %to, align 1
  %from1 = load i64, ptr %from, align 4
  %load.struct = load %_Z5Slice, ptr %2, align 8
  %length = extractvalue %_Z5Slice %load.struct, 1
  %gt = icmp ugt i64 %from1, %length
  br i1 %gt, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %load.struct2 = load %_Z5Slice, ptr %2, align 8
  %length3 = extractvalue %_Z5Slice %load.struct2, 1
  store i64 %length3, ptr %from, align 1
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %to4 = load i64, ptr %to, align 4
  %load.struct5 = load %_Z5Slice, ptr %2, align 8
  %length6 = extractvalue %_Z5Slice %load.struct5, 1
  %gt7 = icmp ugt i64 %to4, %length6
  br i1 %gt7, label %if.then8, label %if.end9

if.then8:                                         ; preds = %if.end
  %load.struct10 = load %_Z5Slice, ptr %2, align 8
  %length11 = extractvalue %_Z5Slice %load.struct10, 1
  store i64 %length11, ptr %to, align 1
  br label %if.end9

if.end9:                                          ; preds = %if.then8, %if.end
  %from12 = load i64, ptr %from, align 4
  %to13 = load i64, ptr %to, align 4
  %gt14 = icmp ugt i64 %from12, %to13
  br i1 %gt14, label %if.then15, label %if.end16

if.then15:                                        ; preds = %if.end9
  %to17 = load i64, ptr %to, align 4
  store i64 %to17, ptr %from, align 1
  br label %if.end16

if.end16:                                         ; preds = %if.then15, %if.end9
  %load.struct18 = load %_Z5Slice, ptr %2, align 8
  %data = extractvalue %_Z5Slice %load.struct18, 0
  %from19 = load i64, ptr %from, align 4
  %ptr.add = getelementptr inbounds ptr, ptr %data, i64 %from19
  %to20 = load i64, ptr %to, align 4
  %from21 = load i64, ptr %from, align 4
  %sub = sub i64 %to20, %from21
  %tuple.field = getelementptr inbounds { ptr, i64 }, ptr %tuple, i32 0, i32 0
  store ptr %ptr.add, ptr %tuple.field, align 1
  %tuple.field22 = getelementptr inbounds { ptr, i64 }, ptr %tuple, i32 0, i32 1
  store i64 %sub, ptr %tuple.field22, align 1
  %tuple.val = load { ptr, i64 }, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr ({ ptr, i64 }, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN5Slice10slice_fromEPN4scaly6memory4PageEm(ptr noalias sret(%_Z5Slice) %0, ptr %1, ptr %2, i64 %3) {
entry:
  %local_page = call ptr @_Z21scaly_alloc_root_pagev()
  %sret.result = alloca %_Z5Slice, align 8
  %field.inplace = getelementptr inbounds %_Z5Slice, ptr %2, i32 0, i32 1
  %field.val = load i64, ptr %field.inplace, align 4
  call void @_ZN5Slice8subsliceEPN4scaly6memory4PageEmm(ptr noalias sret(%_Z5Slice) %sret.result, ptr %local_page, ptr %2, i64 %3, i64 %field.val)
  call void @_Z28scaly_release_root_page_fullP4Page(ptr %local_page)
  %sret.body = load %_Z5Slice, ptr %sret.result, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %sret.result, i64 ptrtoint (ptr getelementptr (%_Z5Slice, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN5Slice8slice_toEPN4scaly6memory4PageEm(ptr noalias sret(%_Z5Slice) %0, ptr %1, ptr %2, i64 %3) {
entry:
  %local_page = call ptr @_Z21scaly_alloc_root_pagev()
  %sret.result = alloca %_Z5Slice, align 8
  call void @_ZN5Slice8subsliceEPN4scaly6memory4PageEmm(ptr noalias sret(%_Z5Slice) %sret.result, ptr %local_page, ptr %2, i64 0, i64 %3)
  call void @_Z28scaly_release_root_page_fullP4Page(ptr %local_page)
  %sret.body = load %_Z5Slice, ptr %sret.result, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %sret.result, i64 ptrtoint (ptr getelementptr (%_Z5Slice, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr i1 @_ZN5Slice6equalsE5SliceI1TE(ptr %0, ptr %1) {
entry:
  ret i1 false
}

define linkonce_odr i1 @_ZN5Slice11starts_withE5SliceI1TE(ptr %0, ptr %1) {
entry:
  ret i1 false
}

define linkonce_odr i1 @_ZN5Slice9ends_withE5SliceI1TE(ptr %0, ptr %1) {
entry:
  ret i1 false
}

define linkonce_odr ptr @_ZN13SliceIteratorI1TE4nextEv(ptr %0) {
entry:
  ret ptr null
}

define linkonce_odr void @_ZN13SliceIteratorI1TEC1E5SliceI1TE(ptr %0, ptr %1) {
entry:
  ret void
}

define linkonce_odr void @_ZN13SliceIteratorI1TEC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr void @_ZN5Slice12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z13SliceIteratorI1TE) %0, ptr %1, ptr %2) {
entry:
  %tuple = alloca %_Z13SliceIteratorI1TE, align 8
  %field.load = load %_Z5Slice, ptr %2, align 8
  %tuple.field = getelementptr inbounds %_Z13SliceIteratorI1TE, ptr %tuple, i32 0, i32 0
  store %_Z5Slice %field.load, ptr %tuple.field, align 1
  %tuple.val = load %_Z13SliceIteratorI1TE, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z13SliceIteratorI1TE, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN5SliceC1Ev(ptr %0) {
entry:
  %data = getelementptr inbounds %_Z5Slice, ptr %0, i32 0, i32 0
  store ptr null, ptr %data, align 8
  %length = getelementptr inbounds %_Z5Slice, ptr %0, i32 0, i32 1
  store i64 0, ptr %length, align 4
  ret void
}

define linkonce_odr ptr @_ZN13SliceIterator4nextEv(ptr %0) {
entry:
  %load.struct = load %_Z13SliceIterator, ptr %0, align 8
  %position = extractvalue %_Z13SliceIterator %load.struct, 1
  %load.struct1 = load %_Z13SliceIterator, ptr %0, align 8
  %slice = extractvalue %_Z13SliceIterator %load.struct1, 0
  %length = extractvalue %_Z5Slice %slice, 1
  %ge = icmp uge i64 %position, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %load.struct2 = load %_Z13SliceIterator, ptr %0, align 8
  %slice3 = extractvalue %_Z13SliceIterator %load.struct2, 0
  %data = extractvalue %_Z5Slice %slice3, 0
  %load.struct4 = load %_Z13SliceIterator, ptr %0, align 8
  %position5 = extractvalue %_Z13SliceIterator %load.struct4, 1
  %ptr.add = getelementptr inbounds ptr, ptr %data, i64 %position5
  %load.struct6 = load %_Z13SliceIterator, ptr %0, align 8
  %position7 = extractvalue %_Z13SliceIterator %load.struct6, 1
  %add = add i64 %position7, 1
  %position8 = getelementptr inbounds %_Z13SliceIterator, ptr %0, i32 0, i32 1
  store i64 %add, ptr %position8, align 4
  ret ptr %ptr.add
}

define linkonce_odr void @_ZN13SliceIteratorC1E5SliceI1TE(ptr %0, ptr %1) {
entry:
  ret void
}

define linkonce_odr void @_ZN13SliceIteratorC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr ptr @_ZN12ListIterator4nextEv(ptr %0) {
entry:
  %old_current = alloca ptr, align 8
  %load.struct = load %_Z12ListIteratorI1TE, ptr %0, align 8
  %current = extractvalue %_Z12ListIteratorI1TE %load.struct, 0
  %ne = icmp ne ptr %current, null
  br i1 %ne, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %load.struct1 = load %_Z12ListIteratorI1TE, ptr %0, align 8
  %current2 = extractvalue %_Z12ListIteratorI1TE %load.struct1, 0
  store ptr %current2, ptr %old_current, align 1
  %load.struct3 = load %_Z12ListIteratorI1TE, ptr %0, align 8
  %current4 = extractvalue %_Z12ListIteratorI1TE %load.struct3, 0
  %deref = load %_Z4NodeI1TE, ptr %current4, align 8
  %next = extractvalue %_Z4NodeI1TE %deref, 1
  %current5 = getelementptr inbounds %_Z12ListIteratorI1TE, ptr %0, i32 0, i32 0
  store ptr %next, ptr %current5, align 8
  %old_current6 = load ptr, ptr %old_current, align 8
  %load.struct7 = load %_Z4NodeI1TE, ptr %old_current6, align 8
  %element = extractvalue %_Z4NodeI1TE %load.struct7, 0
  ret ptr %element

if.else:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; No predecessors!
  ret ptr null
}

define linkonce_odr void @_ZN12ListIteratorC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr ptr @_ZN4List8get_headEPN4scaly6memory4PageE(ptr %0, ptr %1) {
entry:
  %load.struct = load %_Z4ListI1TE, ptr %1, align 8
  %head = extractvalue %_Z4ListI1TE %load.struct, 0
  %eq = icmp eq ptr %head, null
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %load.struct1 = load %_Z4ListI1TE, ptr %1, align 8
  %head2 = extractvalue %_Z4ListI1TE %load.struct1, 0
  %deref = load %_Z4NodeI1TE, ptr %head2, align 8
  %element = extractvalue %_Z4NodeI1TE %deref, 0
  ret ptr %element
}

define linkonce_odr i64 @_ZN4List5countEv(ptr %0) {
entry:
  %local_page = call ptr @_Z21scaly_alloc_root_pagev()
  %sret.result = alloca %_Z12ListIteratorI1TE, align 8
  call void @_ZN4ListI1TE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z12ListIteratorI1TE) %sret.result, ptr %local_page, ptr %0)
  %list_iterator = alloca ptr, align 8
  store ptr %sret.result, ptr %list_iterator, align 1
  %i = alloca i64, align 8
  store i64 0, ptr %i, align 1
  br label %while.cond

while.cond:                                       ; preds = %while.body, %entry
  %list_iterator1 = load ptr, ptr %list_iterator, align 8
  %call = call ptr @_ZN12ListIteratorI1TE4nextEv(ptr %list_iterator1)
  %ne = icmp ne ptr %call, null
  br i1 %ne, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %i2 = load i64, ptr %i, align 4
  %add = add i64 %i2, 1
  store i64 %add, ptr %i, align 1
  br label %while.cond

while.exit:                                       ; preds = %while.cond
  %i3 = load i64, ptr %i, align 4
  call void @_Z28scaly_release_root_page_fullP4Page(ptr %local_page)
  ret i64 %i3
}

define linkonce_odr i1 @_ZN4List6removeE1T(ptr %0, ptr %1) {
entry:
  ret i1 false
}

define linkonce_odr void @_ZN4List12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z12ListIteratorI1TE) %0, ptr %1, ptr %2) {
entry:
  %load.struct = load %_Z4ListI1TE, ptr %2, align 8
  %head = extractvalue %_Z4ListI1TE %load.struct, 0
  %tuple = alloca %_Z12ListIteratorI1TE, align 8
  %tuple.field = getelementptr inbounds %_Z12ListIteratorI1TE, ptr %tuple, i32 0, i32 0
  store ptr %head, ptr %tuple.field, align 1
  %tuple.val = load %_Z12ListIteratorI1TE, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z12ListIteratorI1TE, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN4List3addE1T(ptr %0, ptr %1) {
entry:
  ret void
}

define linkonce_odr void @_ZN4ListC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr ptr @_ZN14VectorIterator4nextEv(ptr %0) {
entry:
  %load.struct = load %_Z14VectorIteratorI1TE, ptr %0, align 8
  %vector = extractvalue %_Z14VectorIteratorI1TE %load.struct, 0
  %eq = icmp eq ptr %vector, null
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %load.struct1 = load %_Z14VectorIteratorI1TE, ptr %0, align 8
  %position = extractvalue %_Z14VectorIteratorI1TE %load.struct1, 1
  %load.struct2 = load %_Z14VectorIteratorI1TE, ptr %0, align 8
  %vector3 = extractvalue %_Z14VectorIteratorI1TE %load.struct2, 0
  %deref = load %_Z6VectorI1TE, ptr %vector3, align 8
  %length = extractvalue %_Z6VectorI1TE %deref, 0
  %eq4 = icmp eq i64 %position, %length
  br i1 %eq4, label %if.then5, label %if.end6

if.then5:                                         ; preds = %if.end
  ret ptr null

if.end6:                                          ; preds = %if.end
  %load.struct7 = load %_Z14VectorIteratorI1TE, ptr %0, align 8
  %position8 = extractvalue %_Z14VectorIteratorI1TE %load.struct7, 1
  %add = add i64 %position8, 1
  %position9 = getelementptr inbounds %_Z14VectorIteratorI1TE, ptr %0, i32 0, i32 1
  store i64 %add, ptr %position9, align 4
  %field.inplace = getelementptr inbounds %_Z14VectorIteratorI1TE, ptr %0, i32 0, i32 0
  %deref.recv = load ptr, ptr %field.inplace, align 8
  %load.struct10 = load %_Z14VectorIteratorI1TE, ptr %0, align 8
  %position11 = extractvalue %_Z14VectorIteratorI1TE %load.struct10, 1
  %sub = sub i64 %position11, 1
  %call = call ptr @_ZN6VectorI1TE7get_ptrEm(ptr %deref.recv, i64 %sub)
  ret ptr %call
}

define linkonce_odr void @_ZN14VectorIteratorC1EP6VectorI1TE(ptr %0, ptr %1) {
entry:
  ret void
}

define linkonce_odr void @_ZN14VectorIteratorC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr ptr @_ZN6Vector3getEPN4scaly6memory4PageEm(ptr %0, ptr %1, i64 %2) {
entry:
  %load.struct = load %_Z6VectorI1TE, ptr %1, align 8
  %length = extractvalue %_Z6VectorI1TE %load.struct, 0
  %ge = icmp uge i64 %2, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %load.struct1 = load %_Z6VectorI1TE, ptr %1, align 8
  %data = extractvalue %_Z6VectorI1TE %load.struct1, 1
  %ptr.add = getelementptr inbounds ptr, ptr %data, i64 %2
  ret ptr %ptr.add
}

define linkonce_odr ptr @_ZN6Vector7get_ptrEm(ptr %0, i64 %1) {
entry:
  %load.struct = load %_Z6VectorI1TE, ptr %0, align 8
  %length = extractvalue %_Z6VectorI1TE %load.struct, 0
  %ge = icmp uge i64 %1, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %load.struct1 = load %_Z6VectorI1TE, ptr %0, align 8
  %data = extractvalue %_Z6VectorI1TE %load.struct1, 1
  %ptr.add = getelementptr inbounds ptr, ptr %data, i64 %1
  ret ptr %ptr.add
}

define linkonce_odr void @_ZN6Vector3putEm1T(ptr %0, i64 %1, ptr %2) {
entry:
  ret void
}

define linkonce_odr void @_ZN6Vector12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z14VectorIteratorI1TE) %0, ptr %1, ptr %2) {
entry:
  %struct.init = alloca %_Z14VectorIteratorI1TE, align 8
  call void @_ZN14VectorIteratorI1TEC1EP6VectorI1TE(ptr %struct.init, ptr %2)
  %sret.body = load %_Z14VectorIteratorI1TE, ptr %struct.init, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %struct.init, i64 ptrtoint (ptr getelementptr (%_Z14VectorIteratorI1TE, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN6Vector8as_sliceEPN4scaly6memory4PageE(ptr noalias sret(%_Z5Slice) %0, ptr %1, ptr %2) {
entry:
  %load.struct = load %_Z6VectorI1TE, ptr %2, align 8
  %data = extractvalue %_Z6VectorI1TE %load.struct, 1
  %load.struct1 = load %_Z6VectorI1TE, ptr %2, align 8
  %length = extractvalue %_Z6VectorI1TE %load.struct1, 0
  %tuple = alloca { ptr, i64 }, align 8
  %tuple.field = getelementptr inbounds { ptr, i64 }, ptr %tuple, i32 0, i32 0
  store ptr %data, ptr %tuple.field, align 1
  %tuple.field2 = getelementptr inbounds { ptr, i64 }, ptr %tuple, i32 0, i32 1
  store i64 %length, ptr %tuple.field2, align 1
  %tuple.val = load { ptr, i64 }, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr ({ ptr, i64 }, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN6VectorC1Ev(ptr %0) {
entry:
  %length = getelementptr inbounds %_Z6Vector, ptr %0, i32 0, i32 0
  store i64 0, ptr %length, align 4
  %data = getelementptr inbounds %_Z6Vector, ptr %0, i32 0, i32 1
  store ptr null, ptr %data, align 8
  ret void
}

define linkonce_odr void @_ZN6VectorC1EP1Tm(ptr %0, ptr %1, i64 %2) {
entry:
  ret void
}

define linkonce_odr void @_ZN6VectorC1EPN4scaly6memory4PageEm(ptr %0, ptr %1, i64 %2) {
entry:
  %length = getelementptr inbounds %_Z6Vector, ptr %0, i32 0, i32 0
  store i64 %2, ptr %length, align 4
  %gt = icmp ugt i64 %2, 0
  br i1 %gt, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %mul = mul i64 %2, ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64)
  %call = call ptr @_ZN4Page8allocateEmm(ptr %1, i64 %mul, i64 8)
  %data = getelementptr inbounds %_Z6Vector, ptr %0, i32 0, i32 1
  store ptr %call, ptr %data, align 8
  %field.inplace = getelementptr inbounds %_Z6Vector, ptr %0, i32 0, i32 1
  %deref.recv = load ptr, ptr %field.inplace, align 8
  %mul1 = mul i64 %2, ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64)
  %call2 = call ptr @memset(ptr %deref.recv, i64 0, i64 %mul1)
  br label %if.end

if.else:                                          ; preds = %entry
  %data3 = getelementptr inbounds %_Z6Vector, ptr %0, i32 0, i32 1
  store ptr null, ptr %data3, align 8
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  %if.value = phi ptr [ %call2, %if.then ], [ null, %if.else ]
  ret void
}

define linkonce_odr void @_ZN6VectorC1EPN4scaly6memory4PageEP1Tm(ptr %0, ptr %1, ptr %2, i64 %3) {
entry:
  ret void
}

define linkonce_odr void @_ZN6VectorC1EPN4scaly6memory4PageE6VectorI1TE(ptr %0, ptr %1, ptr %2) {
entry:
  ret void
}

define linkonce_odr void @_ZN6VectorC1EPN4scaly6memory4PageE5ArrayI1TE(ptr %0, ptr %1, ptr %2) {
entry:
  ret void
}

define linkonce_odr void @_ZN6VectorC1EPN4scaly6memory4PageE4ListI1TE(ptr %0, ptr %1, ptr %2) {
entry:
  ret void
}

define linkonce_odr ptr @_ZN13ArrayIterator4nextEv(ptr %0) {
entry:
  %load.struct = load %_Z13ArrayIteratorI1TE, ptr %0, align 8
  %array = extractvalue %_Z13ArrayIteratorI1TE %load.struct, 0
  %eq = icmp eq ptr %array, null
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %load.struct1 = load %_Z13ArrayIteratorI1TE, ptr %0, align 8
  %position = extractvalue %_Z13ArrayIteratorI1TE %load.struct1, 1
  %load.struct2 = load %_Z13ArrayIteratorI1TE, ptr %0, align 8
  %array3 = extractvalue %_Z13ArrayIteratorI1TE %load.struct2, 0
  %deref = load %_Z5ArrayI1TE, ptr %array3, align 8
  %length = extractvalue %_Z5ArrayI1TE %deref, 0
  %eq4 = icmp eq i64 %position, %length
  br i1 %eq4, label %if.then5, label %if.end6

if.then5:                                         ; preds = %if.end
  ret ptr null

if.end6:                                          ; preds = %if.end
  %load.struct7 = load %_Z13ArrayIteratorI1TE, ptr %0, align 8
  %position8 = extractvalue %_Z13ArrayIteratorI1TE %load.struct7, 1
  %add = add i64 %position8, 1
  %position9 = getelementptr inbounds %_Z13ArrayIteratorI1TE, ptr %0, i32 0, i32 1
  store i64 %add, ptr %position9, align 4
  %field.inplace = getelementptr inbounds %_Z13ArrayIteratorI1TE, ptr %0, i32 0, i32 0
  %deref.recv = load ptr, ptr %field.inplace, align 8
  %call = call ptr @_ZN5ArrayI1TE10get_bufferEv(ptr %deref.recv)
  %load.struct10 = load %_Z13ArrayIteratorI1TE, ptr %0, align 8
  %position11 = extractvalue %_Z13ArrayIteratorI1TE %load.struct10, 1
  %sub = sub i64 %position11, 1
  %ptr.add = getelementptr inbounds ptr, ptr %call, i64 %sub
  ret ptr %ptr.add
}

define linkonce_odr void @_ZN13ArrayIteratorC1EP5ArrayI1TE(ptr %0, ptr %1) {
entry:
  ret void
}

define linkonce_odr void @_ZN13ArrayIteratorC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr ptr @_ZN5Array10get_bufferEv(ptr %0) {
entry:
  %load.struct = load %_Z5ArrayI1TE, ptr %0, align 8
  %vector = extractvalue %_Z5ArrayI1TE %load.struct, 1
  %eq = icmp eq ptr %vector, null
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %load.struct1 = load %_Z5ArrayI1TE, ptr %0, align 8
  %vector2 = extractvalue %_Z5ArrayI1TE %load.struct1, 1
  %deref = load %_Z6VectorI1TE, ptr %vector2, align 8
  %data = extractvalue %_Z6VectorI1TE %deref, 1
  ret ptr %data
}

define linkonce_odr i64 @_ZN5Array10get_lengthEv(ptr %0) {
entry:
  %load.struct = load %_Z5ArrayI1TE, ptr %0, align 8
  %length = extractvalue %_Z5ArrayI1TE %load.struct, 0
  ret i64 %length
}

define linkonce_odr i64 @_ZN5Array12get_capacityEv(ptr %0) {
entry:
  %load.struct = load %_Z5ArrayI1TE, ptr %0, align 8
  %vector = extractvalue %_Z5ArrayI1TE %load.struct, 1
  %eq = icmp eq ptr %vector, null
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret i64 0

if.end:                                           ; preds = %entry
  %load.struct1 = load %_Z5ArrayI1TE, ptr %0, align 8
  %vector2 = extractvalue %_Z5ArrayI1TE %load.struct1, 1
  %deref = load %_Z6VectorI1TE, ptr %vector2, align 8
  %length = extractvalue %_Z6VectorI1TE %deref, 0
  ret i64 %length
}

define linkonce_odr void @_ZN5Array3addE1T(ptr %0, ptr %1) {
entry:
  ret void
}

define linkonce_odr void @_ZN5Array3addE6VectorI1TE(ptr %0, ptr %1) {
entry:
  ret void
}

define linkonce_odr ptr @_ZN5Array3getEPN4scaly6memory4PageEm(ptr %0, ptr %1, i64 %2) {
entry:
  %load.struct = load %_Z5ArrayI1TE, ptr %1, align 8
  %length = extractvalue %_Z5ArrayI1TE %load.struct, 0
  %ge = icmp uge i64 %2, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %load.struct1 = load %_Z5ArrayI1TE, ptr %1, align 8
  %vector = extractvalue %_Z5ArrayI1TE %load.struct1, 1
  %deref = load %_Z6VectorI1TE, ptr %vector, align 8
  %data = extractvalue %_Z6VectorI1TE %deref, 1
  %ptr.add = getelementptr inbounds ptr, ptr %data, i64 %2
  ret ptr %ptr.add
}

define linkonce_odr void @_ZN5Array3putEm1T(ptr %0, i64 %1, ptr %2) {
entry:
  ret void
}

define linkonce_odr void @_ZN5Array12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z13ArrayIteratorI1TE) %0, ptr %1, ptr %2) {
entry:
  %struct.init = alloca %_Z13ArrayIteratorI1TE, align 8
  call void @_ZN13ArrayIteratorI1TEC1EP5ArrayI1TE(ptr %struct.init, ptr %2)
  %sret.body = load %_Z13ArrayIteratorI1TE, ptr %struct.init, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %struct.init, i64 ptrtoint (ptr getelementptr (%_Z13ArrayIteratorI1TE, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN5Array10reallocateEv(ptr %0) {
entry:
  %call = call ptr @_ZN4Page3getEPv(ptr %0)
  %length = alloca i64, align 8
  store i64 0, ptr %length, align 1
  %load.struct = load %_Z5ArrayI1TE, ptr %0, align 8
  %vector = extractvalue %_Z5ArrayI1TE %load.struct, 1
  %eq = icmp eq ptr %vector, null
  br i1 %eq, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %call1 = call ptr @_ZN4Page23allocate_exclusive_pageEv(ptr %call)
  %call2 = call i64 @_ZN4Page12get_capacityEm(ptr %call1, i64 8)
  %sub = sub i64 %call2, ptrtoint (ptr getelementptr (%_Z6VectorI1TE, ptr null, i32 1) to i64)
  %udiv = udiv i64 %sub, ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64)
  store i64 %udiv, ptr %length, align 1
  %struct.region = call ptr @_ZN4Page8allocateEmm(ptr %call1, i64 ptrtoint (ptr getelementptr (%_Z6VectorI1TE, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr ({ i1, %_Z6VectorI1TE }, ptr null, i64 0, i32 1) to i64))
  %length3 = load i64, ptr %length, align 4
  call void @_ZN6VectorI1TEC1EPN4scaly6memory4PageEm(ptr %struct.region, ptr %call1, i64 %length3)
  %vector4 = getelementptr inbounds %_Z5ArrayI1TE, ptr %0, i32 0, i32 1
  store ptr %struct.region, ptr %vector4, align 8
  br label %if.end

if.else:                                          ; preds = %entry
  %load.struct5 = load %_Z5ArrayI1TE, ptr %0, align 8
  %vector6 = extractvalue %_Z5ArrayI1TE %load.struct5, 1
  %deref = load %_Z6VectorI1TE, ptr %vector6, align 8
  %length7 = extractvalue %_Z6VectorI1TE %deref, 0
  %mul = mul i64 %length7, 2
  store i64 %mul, ptr %length, align 1
  %call8 = call ptr @_ZN4Page23allocate_exclusive_pageEv(ptr %call)
  %struct.region9 = call ptr @_ZN4Page8allocateEmm(ptr %call8, i64 ptrtoint (ptr getelementptr (%_Z6VectorI1TE, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr ({ i1, %_Z6VectorI1TE }, ptr null, i64 0, i32 1) to i64))
  %length10 = load i64, ptr %length, align 4
  call void @_ZN6VectorI1TEC1EPN4scaly6memory4PageEm(ptr %struct.region9, ptr %call8, i64 %length10)
  %load.struct11 = load %_Z5ArrayI1TE, ptr %0, align 8
  %vector12 = extractvalue %_Z5ArrayI1TE %load.struct11, 1
  %deref13 = load %_Z6VectorI1TE, ptr %vector12, align 8
  %length14 = extractvalue %_Z6VectorI1TE %deref13, 0
  %mul15 = mul i64 %length14, ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64)
  %field.inplace = getelementptr inbounds %_Z6VectorI1TE, ptr %struct.region9, i32 0, i32 1
  %deref.recv = load ptr, ptr %field.inplace, align 8
  %field.inplace16 = getelementptr inbounds %_Z5ArrayI1TE, ptr %0, i32 0, i32 1
  %load.struct17 = load %_Z5ArrayI1TE, ptr %0, align 8
  %vector18 = extractvalue %_Z5ArrayI1TE %load.struct17, 1
  %deref19 = load %_Z6VectorI1TE, ptr %vector18, align 8
  %data = extractvalue %_Z6VectorI1TE %deref19, 1
  %call20 = call ptr @memcpy(ptr %deref.recv, ptr %data, i64 %mul15)
  %load.struct21 = load %_Z5ArrayI1TE, ptr %0, align 8
  %vector22 = extractvalue %_Z5ArrayI1TE %load.struct21, 1
  %call23 = call ptr @_ZN4Page3getEPv(ptr %vector22)
  call void @_ZN4Page25deallocate_exclusive_pageEP4Page(ptr %call, ptr %call23)
  %vector24 = getelementptr inbounds %_Z5ArrayI1TE, ptr %0, i32 0, i32 1
  store ptr %struct.region9, ptr %vector24, align 8
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  ret void
}

define linkonce_odr void @_ZN5ArrayC1Ev(ptr %0) {
entry:
  %length = getelementptr inbounds %_Z5Array, ptr %0, i32 0, i32 0
  store i64 0, ptr %length, align 4
  %vector = getelementptr inbounds %_Z5Array, ptr %0, i32 0, i32 1
  store ptr null, ptr %vector, align 8
  ret void
}

define linkonce_odr void @_ZN5ArrayC1Em(ptr %0, i64 %1) {
entry:
  %length = getelementptr inbounds %_Z5Array, ptr %0, i32 0, i32 0
  store i64 0, ptr %length, align 4
  %vector = getelementptr inbounds %_Z5Array, ptr %0, i32 0, i32 1
  store ptr null, ptr %vector, align 8
  %call = call ptr @_ZN4Page3getEPv(ptr %0)
  %call1 = call ptr @_ZN4Page23allocate_exclusive_pageEv(ptr %call)
  %struct.region = call ptr @_ZN4Page8allocateEmm(ptr %call1, i64 ptrtoint (ptr getelementptr (%_Z6VectorI1TE, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr ({ i1, %_Z6VectorI1TE }, ptr null, i64 0, i32 1) to i64))
  call void @_ZN6VectorI1TEC1EPN4scaly6memory4PageEm(ptr %struct.region, ptr %call1, i64 %1)
  %vector2 = getelementptr inbounds %_Z5Array, ptr %0, i32 0, i32 1
  store ptr %struct.region, ptr %vector2, align 8
  ret void
}

define linkonce_odr void @_ZN5ArrayC1EPN4scaly6memory4PageE(ptr %0, ptr %1) {
entry:
  %length = getelementptr inbounds %_Z5Array, ptr %0, i32 0, i32 0
  store i64 0, ptr %length, align 4
  %vector = getelementptr inbounds %_Z5Array, ptr %0, i32 0, i32 1
  store ptr null, ptr %vector, align 8
  ret void
}

define linkonce_odr void @_ZN5ArrayC1EPN4scaly6memory4PageEm(ptr %0, ptr %1, i64 %2) {
entry:
  %length = getelementptr inbounds %_Z5Array, ptr %0, i32 0, i32 0
  store i64 0, ptr %length, align 4
  %vector = getelementptr inbounds %_Z5Array, ptr %0, i32 0, i32 1
  store ptr null, ptr %vector, align 8
  %call = call ptr @_ZN4Page23allocate_exclusive_pageEv(ptr %1)
  %struct.region = call ptr @_ZN4Page8allocateEmm(ptr %call, i64 ptrtoint (ptr getelementptr (%_Z6VectorI1TE, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr ({ i1, %_Z6VectorI1TE }, ptr null, i64 0, i32 1) to i64))
  call void @_ZN6VectorI1TEC1EPN4scaly6memory4PageEm(ptr %struct.region, ptr %call, i64 %2)
  %vector1 = getelementptr inbounds %_Z5Array, ptr %0, i32 0, i32 1
  store ptr %struct.region, ptr %vector1, align 8
  ret void
}

define linkonce_odr ptr @_ZN19BuilderListIterator4nextEv(ptr %0) {
entry:
  %load.struct = load %_Z19BuilderListIteratorI1TE, ptr %0, align 8
  %current = extractvalue %_Z19BuilderListIteratorI1TE %load.struct, 0
  %ne = icmp ne ptr %current, null
  br i1 %ne, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %load.struct1 = load %_Z19BuilderListIteratorI1TE, ptr %0, align 8
  %current2 = extractvalue %_Z19BuilderListIteratorI1TE %load.struct1, 0
  %load.struct3 = load %_Z19BuilderListIteratorI1TE, ptr %0, align 8
  %current4 = extractvalue %_Z19BuilderListIteratorI1TE %load.struct3, 0
  %deref = load %_Z4NodeI1TE, ptr %current4, align 8
  %next = extractvalue %_Z4NodeI1TE %deref, 1
  %current5 = getelementptr inbounds %_Z19BuilderListIteratorI1TE, ptr %0, i32 0, i32 0
  store ptr %next, ptr %current5, align 8
  %load.struct6 = load %_Z4NodeI1TE, ptr %current2, align 8
  %element = extractvalue %_Z4NodeI1TE %load.struct6, 0
  ret ptr %element

if.else:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; No predecessors!
  ret ptr null
}

define linkonce_odr void @_ZN19BuilderListIteratorC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr void @_ZN11BuilderList3addE1T(ptr %0, ptr %1) {
entry:
  ret void
}

define linkonce_odr i1 @_ZN11BuilderList6removeE1T(ptr %0, ptr %1) {
entry:
  ret i1 false
}

define linkonce_odr ptr @_ZN11BuilderList8get_headEPN4scaly6memory4PageE(ptr %0, ptr %1) {
entry:
  %load.struct = load %_Z11BuilderListI1TE, ptr %1, align 8
  %head = extractvalue %_Z11BuilderListI1TE %load.struct, 0
  %eq = icmp eq ptr %head, null
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %load.struct1 = load %_Z11BuilderListI1TE, ptr %1, align 8
  %head2 = extractvalue %_Z11BuilderListI1TE %load.struct1, 0
  %deref = load %_Z4NodeI1TE, ptr %head2, align 8
  %element = extractvalue %_Z4NodeI1TE %deref, 0
  ret ptr %element
}

define linkonce_odr i64 @_ZN11BuilderList5countEv(ptr %0) {
entry:
  %local_page = call ptr @_Z21scaly_alloc_root_pagev()
  %sret.result = alloca %_Z19BuilderListIteratorI1TE, align 8
  call void @_ZN11BuilderListI1TE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z19BuilderListIteratorI1TE) %sret.result, ptr %local_page, ptr %0)
  %list_iterator = alloca ptr, align 8
  store ptr %sret.result, ptr %list_iterator, align 1
  %i = alloca i64, align 8
  store i64 0, ptr %i, align 1
  br label %while.cond

while.cond:                                       ; preds = %while.body, %entry
  %list_iterator1 = load ptr, ptr %list_iterator, align 8
  %call = call ptr @_ZN19BuilderListIteratorI1TE4nextEv(ptr %list_iterator1)
  %ne = icmp ne ptr %call, null
  br i1 %ne, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %i2 = load i64, ptr %i, align 4
  %add = add i64 %i2, 1
  store i64 %add, ptr %i, align 1
  br label %while.cond

while.exit:                                       ; preds = %while.cond
  %i3 = load i64, ptr %i, align 4
  call void @_Z28scaly_release_root_page_fullP4Page(ptr %local_page)
  ret i64 %i3
}

define linkonce_odr void @_ZN11BuilderList12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z19BuilderListIteratorI1TE) %0, ptr %1, ptr %2) {
entry:
  %load.struct = load %_Z11BuilderListI1TE, ptr %2, align 8
  %head = extractvalue %_Z11BuilderListI1TE %load.struct, 0
  %tuple = alloca %_Z19BuilderListIteratorI1TE, align 8
  %tuple.field = getelementptr inbounds %_Z19BuilderListIteratorI1TE, ptr %tuple, i32 0, i32 0
  store ptr %head, ptr %tuple.field, align 1
  %tuple.val = load %_Z19BuilderListIteratorI1TE, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z19BuilderListIteratorI1TE, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN11BuilderListC1Ev(ptr %0) {
entry:
  %head = getelementptr inbounds %_Z11BuilderList, ptr %0, i32 0, i32 0
  store ptr null, ptr %head, align 8
  ret void
}

define linkonce_odr void @_ZN14HashSetBuilder10reallocateEm(ptr %0, i64 %1) {
entry:
  %deref.tmp = alloca %_Z4SlotI1TE, align 8
  %list_iterator = alloca %_Z19BuilderListIteratorI4SlotI1TEE, align 8
  %tuple = alloca %_Z19BuilderListIteratorI4SlotI1TEE, align 8
  %vector_iterator = alloca ptr, align 8
  %struct.init = alloca %_Z14VectorIteratorI11BuilderListI4SlotI1TEEE, align 8
  %call = call i64 @_ZN7hashing9get_primeEm(i64 %1)
  %call1 = call ptr @_ZN4Page3getEPv(ptr %0)
  %call2 = call ptr @_ZN4Page23allocate_exclusive_pageEv(ptr %call1)
  %struct.region = call ptr @_ZN4Page8allocateEmm(ptr %call2, i64 ptrtoint (ptr getelementptr (%_Z6VectorI11BuilderListI4SlotI1TEEE, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr ({ i1, %_Z6VectorI11BuilderListI4SlotI1TEEE }, ptr null, i64 0, i32 1) to i64))
  call void @_ZN6VectorI11BuilderListI4SlotI1TEEEC1EPN4scaly6memory4PageEm(ptr %struct.region, ptr %call2, i64 %call)
  %load.struct = load %_Z14HashSetBuilderI1TE, ptr %0, align 8
  %slots = extractvalue %_Z14HashSetBuilderI1TE %load.struct, 1
  %ne = icmp ne ptr %slots, null
  br i1 %ne, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %load.struct3 = load %_Z14HashSetBuilderI1TE, ptr %0, align 8
  %slots4 = extractvalue %_Z14HashSetBuilderI1TE %load.struct3, 1
  call void @_ZN14VectorIteratorI11BuilderListI4SlotI1TEEEC1EP6VectorI11BuilderListI4SlotI1TEEE(ptr %struct.init, ptr %slots4)
  store ptr %struct.init, ptr %vector_iterator, align 1
  br label %while.cond

if.end:                                           ; preds = %while.exit, %entry
  %slots19 = getelementptr inbounds %_Z14HashSetBuilderI1TE, ptr %0, i32 0, i32 1
  store ptr %struct.region, ptr %slots19, align 8
  ret void

while.cond:                                       ; preds = %while.exit10, %if.then
  %vector_iterator5 = load ptr, ptr %vector_iterator, align 8
  %call6 = call ptr @_ZN14VectorIteratorI11BuilderListI4SlotI1TEEE4nextEv(ptr %vector_iterator5)
  %while.tobool = icmp ne ptr %call6, null
  br i1 %while.tobool, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %load.struct7 = load %_Z11BuilderListI4SlotI1TEE, ptr %call6, align 8
  %head = extractvalue %_Z11BuilderListI4SlotI1TEE %load.struct7, 0
  %tuple.field = getelementptr inbounds %_Z19BuilderListIteratorI4SlotI1TEE, ptr %tuple, i32 0, i32 0
  store ptr %head, ptr %tuple.field, align 1
  %tuple.val = load %_Z19BuilderListIteratorI4SlotI1TEE, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %list_iterator, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z19BuilderListIteratorI4SlotI1TEE, ptr null, i32 1) to i64), i1 false)
  br label %while.cond8

while.exit:                                       ; preds = %while.cond
  %load.struct16 = load %_Z14HashSetBuilderI1TE, ptr %0, align 8
  %slots17 = extractvalue %_Z14HashSetBuilderI1TE %load.struct16, 1
  %call18 = call ptr @_ZN4Page3getEPv(ptr %slots17)
  call void @_ZN4Page25deallocate_exclusive_pageEP4Page(ptr %call1, ptr %call18)
  br label %if.end

while.cond8:                                      ; preds = %while.body9, %while.body
  %call11 = call ptr @_ZN19BuilderListIteratorI4SlotI1TEE4nextEv(ptr %list_iterator)
  %while.tobool12 = icmp ne ptr %call11, null
  br i1 %while.tobool12, label %while.body9, label %while.exit10

while.body9:                                      ; preds = %while.cond8
  %load.struct13 = load %_Z4SlotI1TE, ptr %call11, align 8
  %hash_code = extractvalue %_Z4SlotI1TE %load.struct13, 1
  %load.struct14 = load %_Z6VectorI11BuilderListI4SlotI1TEEE, ptr %struct.region, align 8
  %length = extractvalue %_Z6VectorI11BuilderListI4SlotI1TEEE %load.struct14, 0
  %urem = urem i64 %hash_code, %length
  %call15 = call ptr @_ZN6VectorI11BuilderListI4SlotI1TEEE7get_ptrEm(ptr %struct.region, i64 %urem)
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %deref.tmp, ptr align 1 %call11, i64 ptrtoint (ptr getelementptr (%_Z4SlotI1TE, ptr null, i32 1) to i64), i1 false)
  call void @_ZN11BuilderListI4SlotI1TEE3addE4SlotI1TE(ptr %call15, ptr %deref.tmp)
  br label %while.cond8

while.exit10:                                     ; preds = %while.cond8
  br label %while.cond
}

define linkonce_odr i1 @_ZN14HashSetBuilder3addE1T(ptr %0, ptr %1) {
entry:
  ret i1 false
}

define linkonce_odr i1 @_ZN14HashSetBuilder12add_internalE1T(ptr %0, ptr %1) {
entry:
  ret i1 false
}

define linkonce_odr i1 @_ZN14HashSetBuilder8containsE1T(ptr %0, ptr %1) {
entry:
  ret i1 false
}

define linkonce_odr void @_ZN14HashSetBuilderC1E6VectorI1TE(ptr %0, ptr %1) {
entry:
  ret void
}

define linkonce_odr void @_ZN14HashSetBuilderC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr ptr @_ZN6VectorI6VectorI1TEE3getEPN4scaly6memory4PageEm(ptr %0, ptr %1, i64 %2) {
entry:
  ret ptr null
}

define linkonce_odr ptr @_ZN6VectorI6VectorI1TEE7get_ptrEm(ptr %0, i64 %1) {
entry:
  ret ptr null
}

define linkonce_odr void @_ZN6VectorI6VectorI1TEE3putEm6VectorI1TE(ptr %0, i64 %1, ptr %2) {
entry:
  ret void
}

define linkonce_odr ptr @_ZN14VectorIteratorI6VectorI1TEE4nextEv(ptr %0) {
entry:
  ret ptr null
}

define linkonce_odr void @_ZN14VectorIteratorI6VectorI1TEEC1EP6VectorI6VectorI1TEE(ptr %0, ptr %1) {
entry:
  ret void
}

define linkonce_odr void @_ZN14VectorIteratorI6VectorI1TEEC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr void @_ZN6VectorI6VectorI1TEE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z14VectorIteratorI6VectorI1TEE) %0, ptr %1, ptr %2) {
entry:
  ret void
}

define linkonce_odr void @_ZN6VectorI6VectorI1TEE8as_sliceEPN4scaly6memory4PageE(ptr noalias sret(%_Z5Slice) %0, ptr %1, ptr %2) {
entry:
  ret void
}

define linkonce_odr void @_ZN6VectorI6VectorI1TEEC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr void @_ZN6VectorI6VectorI1TEEC1EP6VectorI1TEm(ptr %0, ptr %1, i64 %2) {
entry:
  ret void
}

define linkonce_odr void @_ZN6VectorI6VectorI1TEEC1EPN4scaly6memory4PageEm(ptr %0, ptr %1, i64 %2) {
entry:
  ret void
}

define linkonce_odr void @_ZN6VectorI6VectorI1TEEC1EPN4scaly6memory4PageEP6VectorI1TEm(ptr %0, ptr %1, ptr %2, i64 %3) {
entry:
  ret void
}

define linkonce_odr void @_ZN6VectorI6VectorI1TEEC1EPN4scaly6memory4PageE6VectorI6VectorI1TEE(ptr %0, ptr %1, ptr %2) {
entry:
  ret void
}

define linkonce_odr ptr @_ZN5ArrayI6VectorI1TEE10get_bufferEv(ptr %0) {
entry:
  ret ptr null
}

define linkonce_odr i64 @_ZN5ArrayI6VectorI1TEE10get_lengthEv(ptr %0) {
entry:
  ret i64 0
}

define linkonce_odr i64 @_ZN5ArrayI6VectorI1TEE12get_capacityEv(ptr %0) {
entry:
  ret i64 0
}

define linkonce_odr void @_ZN5ArrayI6VectorI1TEE10reallocateEv(ptr %0) {
entry:
  ret void
}

define linkonce_odr void @_ZN5ArrayI6VectorI1TEE3addE6VectorI1TE(ptr %0, ptr %1) {
entry:
  ret void
}

define linkonce_odr void @_ZN5ArrayI6VectorI1TEE3addE6VectorI6VectorI1TEE(ptr %0, ptr %1) {
entry:
  ret void
}

define linkonce_odr ptr @_ZN5ArrayI6VectorI1TEE3getEPN4scaly6memory4PageEm(ptr %0, ptr %1, i64 %2) {
entry:
  ret ptr null
}

define linkonce_odr void @_ZN5ArrayI6VectorI1TEE3putEm6VectorI1TE(ptr %0, i64 %1, ptr %2) {
entry:
  ret void
}

define linkonce_odr ptr @_ZN13ArrayIteratorI6VectorI1TEE4nextEv(ptr %0) {
entry:
  ret ptr null
}

define linkonce_odr void @_ZN13ArrayIteratorI6VectorI1TEEC1EP5ArrayI6VectorI1TEE(ptr %0, ptr %1) {
entry:
  ret void
}

define linkonce_odr void @_ZN13ArrayIteratorI6VectorI1TEEC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr void @_ZN5ArrayI6VectorI1TEE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z13ArrayIteratorI6VectorI1TEE) %0, ptr %1, ptr %2) {
entry:
  ret void
}

define linkonce_odr void @_ZN6VectorI6VectorI1TEEC1EPN4scaly6memory4PageE5ArrayI6VectorI1TEE(ptr %0, ptr %1, ptr %2) {
entry:
  ret void
}

define linkonce_odr ptr @_ZN4ListI6VectorI1TEE8get_headEPN4scaly6memory4PageE(ptr %0, ptr %1) {
entry:
  ret ptr null
}

define linkonce_odr ptr @_ZN12ListIteratorI6VectorI1TEE4nextEv(ptr %0) {
entry:
  ret ptr null
}

define linkonce_odr void @_ZN12ListIteratorI6VectorI1TEEC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr i64 @_ZN4ListI6VectorI1TEE5countEv(ptr %0) {
entry:
  ret i64 0
}

define linkonce_odr i1 @_ZN4ListI6VectorI1TEE6removeE6VectorI1TE(ptr %0, ptr %1) {
entry:
  ret i1 false
}

define linkonce_odr void @_ZN4ListI6VectorI1TEE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z12ListIteratorI6VectorI1TEE) %0, ptr %1, ptr %2) {
entry:
  ret void
}

define linkonce_odr void @_ZN4ListI6VectorI1TEE3addE6VectorI1TE(ptr %0, ptr %1) {
entry:
  ret void
}

define linkonce_odr void @_ZN4ListI6VectorI1TEEC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr void @_ZN6VectorI6VectorI1TEEC1EPN4scaly6memory4PageE4ListI6VectorI1TEE(ptr %0, ptr %1, ptr %2) {
entry:
  ret void
}

define linkonce_odr void @_ZN5ArrayI6VectorI1TEEC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr void @_ZN5ArrayI6VectorI1TEEC1Em(ptr %0, i64 %1) {
entry:
  ret void
}

define linkonce_odr void @_ZN5ArrayI6VectorI1TEEC1EPN4scaly6memory4PageE(ptr %0, ptr %1) {
entry:
  ret void
}

define linkonce_odr void @_ZN5ArrayI6VectorI1TEEC1EPN4scaly6memory4PageEm(ptr %0, ptr %1, i64 %2) {
entry:
  ret void
}

define linkonce_odr i1 @_ZN7HashSetI1TE8containsE1T(ptr %0, ptr %1) {
entry:
  ret i1 false
}

define linkonce_odr ptr @_ZN12ListIteratorI4SlotI1TEE4nextEv(ptr %0) {
entry:
  ret ptr null
}

define linkonce_odr void @_ZN12ListIteratorI4SlotI1TEEC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr void @_ZN7HashSetI1TEC1EPN4scaly6memory4PageE14HashSetBuilderI1TE(ptr %0, ptr %1, ptr %2) {
entry:
  ret void
}

define linkonce_odr void @_ZN7HashSetI1TEC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr i1 @_ZN7HashSet8containsE1T(ptr %0, ptr %1) {
entry:
  ret i1 false
}

define linkonce_odr void @_ZN7HashSetC1EPN4scaly6memory4PageE14HashSetBuilderI1TE(ptr %0, ptr %1, ptr %2) {
entry:
  ret void
}

define linkonce_odr void @_ZN7HashSetC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr void @_ZN11BuilderListI4SlotI12KeyValuePairI1K1VEEE3addE4SlotI12KeyValuePairI1K1VEE(ptr %0, ptr %1) {
entry:
  ret void
}

define linkonce_odr i1 @_ZN11BuilderListI4SlotI12KeyValuePairI1K1VEEE6removeE4SlotI12KeyValuePairI1K1VEE(ptr %0, ptr %1) {
entry:
  ret i1 false
}

define linkonce_odr ptr @_ZN11BuilderListI4SlotI12KeyValuePairI1K1VEEE8get_headEPN4scaly6memory4PageE(ptr %0, ptr %1) {
entry:
  ret ptr null
}

define linkonce_odr ptr @_ZN19BuilderListIteratorI4SlotI12KeyValuePairI1K1VEEE4nextEv(ptr %0) {
entry:
  ret ptr null
}

define linkonce_odr void @_ZN19BuilderListIteratorI4SlotI12KeyValuePairI1K1VEEEC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr i64 @_ZN11BuilderListI4SlotI12KeyValuePairI1K1VEEE5countEv(ptr %0) {
entry:
  ret i64 0
}

define linkonce_odr void @_ZN11BuilderListI4SlotI12KeyValuePairI1K1VEEE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z19BuilderListIteratorI4SlotI12KeyValuePairI1K1VEEE) %0, ptr %1, ptr %2) {
entry:
  ret void
}

define linkonce_odr void @_ZN11BuilderListI4SlotI12KeyValuePairI1K1VEEEC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr ptr @_ZN6VectorI11BuilderListI4SlotI12KeyValuePairI1K1VEEEE3getEPN4scaly6memory4PageEm(ptr %0, ptr %1, i64 %2) {
entry:
  ret ptr null
}

define linkonce_odr ptr @_ZN6VectorI11BuilderListI4SlotI12KeyValuePairI1K1VEEEE7get_ptrEm(ptr %0, i64 %1) {
entry:
  ret ptr null
}

define linkonce_odr void @_ZN6VectorI11BuilderListI4SlotI12KeyValuePairI1K1VEEEE3putEm11BuilderListI4SlotI12KeyValuePairI1K1VEEE(ptr %0, i64 %1, ptr %2) {
entry:
  ret void
}

define linkonce_odr ptr @_ZN14VectorIteratorI11BuilderListI4SlotI12KeyValuePairI1K1VEEEE4nextEv(ptr %0) {
entry:
  ret ptr null
}

define linkonce_odr void @_ZN14VectorIteratorI11BuilderListI4SlotI12KeyValuePairI1K1VEEEEC1EP6VectorI11BuilderListI4SlotI12KeyValuePairI1K1VEEEE(ptr %0, ptr %1) {
entry:
  ret void
}

define linkonce_odr void @_ZN14VectorIteratorI11BuilderListI4SlotI12KeyValuePairI1K1VEEEEC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr void @_ZN6VectorI11BuilderListI4SlotI12KeyValuePairI1K1VEEEE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z14VectorIteratorI11BuilderListI4SlotI12KeyValuePairI1K1VEEEE) %0, ptr %1, ptr %2) {
entry:
  ret void
}

define linkonce_odr void @_ZN6VectorI11BuilderListI4SlotI12KeyValuePairI1K1VEEEE8as_sliceEPN4scaly6memory4PageE(ptr noalias sret(%_Z5Slice) %0, ptr %1, ptr %2) {
entry:
  ret void
}

define linkonce_odr void @_ZN6VectorI11BuilderListI4SlotI12KeyValuePairI1K1VEEEEC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr void @_ZN6VectorI11BuilderListI4SlotI12KeyValuePairI1K1VEEEEC1EP11BuilderListI4SlotI12KeyValuePairI1K1VEEEm(ptr %0, ptr %1, i64 %2) {
entry:
  ret void
}

define linkonce_odr void @_ZN6VectorI11BuilderListI4SlotI12KeyValuePairI1K1VEEEEC1EPN4scaly6memory4PageEm(ptr %0, ptr %1, i64 %2) {
entry:
  ret void
}

define linkonce_odr void @_ZN6VectorI11BuilderListI4SlotI12KeyValuePairI1K1VEEEEC1EPN4scaly6memory4PageEP11BuilderListI4SlotI12KeyValuePairI1K1VEEEm(ptr %0, ptr %1, ptr %2, i64 %3) {
entry:
  ret void
}

define linkonce_odr void @_ZN6VectorI11BuilderListI4SlotI12KeyValuePairI1K1VEEEEC1EPN4scaly6memory4PageE6VectorI11BuilderListI4SlotI12KeyValuePairI1K1VEEEE(ptr %0, ptr %1, ptr %2) {
entry:
  ret void
}

define linkonce_odr ptr @_ZN5ArrayI11BuilderListI4SlotI12KeyValuePairI1K1VEEEE10get_bufferEv(ptr %0) {
entry:
  ret ptr null
}

define linkonce_odr i64 @_ZN5ArrayI11BuilderListI4SlotI12KeyValuePairI1K1VEEEE10get_lengthEv(ptr %0) {
entry:
  ret i64 0
}

define linkonce_odr i64 @_ZN5ArrayI11BuilderListI4SlotI12KeyValuePairI1K1VEEEE12get_capacityEv(ptr %0) {
entry:
  ret i64 0
}

define linkonce_odr void @_ZN5ArrayI11BuilderListI4SlotI12KeyValuePairI1K1VEEEE10reallocateEv(ptr %0) {
entry:
  ret void
}

define linkonce_odr void @_ZN5ArrayI11BuilderListI4SlotI12KeyValuePairI1K1VEEEE3addE11BuilderListI4SlotI12KeyValuePairI1K1VEEE(ptr %0, ptr %1) {
entry:
  ret void
}

define linkonce_odr void @_ZN5ArrayI11BuilderListI4SlotI12KeyValuePairI1K1VEEEE3addE6VectorI11BuilderListI4SlotI12KeyValuePairI1K1VEEEE(ptr %0, ptr %1) {
entry:
  ret void
}

define linkonce_odr ptr @_ZN5ArrayI11BuilderListI4SlotI12KeyValuePairI1K1VEEEE3getEPN4scaly6memory4PageEm(ptr %0, ptr %1, i64 %2) {
entry:
  ret ptr null
}

define linkonce_odr void @_ZN5ArrayI11BuilderListI4SlotI12KeyValuePairI1K1VEEEE3putEm11BuilderListI4SlotI12KeyValuePairI1K1VEEE(ptr %0, i64 %1, ptr %2) {
entry:
  ret void
}

define linkonce_odr ptr @_ZN13ArrayIteratorI11BuilderListI4SlotI12KeyValuePairI1K1VEEEE4nextEv(ptr %0) {
entry:
  ret ptr null
}

define linkonce_odr void @_ZN13ArrayIteratorI11BuilderListI4SlotI12KeyValuePairI1K1VEEEEC1EP5ArrayI11BuilderListI4SlotI12KeyValuePairI1K1VEEEE(ptr %0, ptr %1) {
entry:
  ret void
}

define linkonce_odr void @_ZN13ArrayIteratorI11BuilderListI4SlotI12KeyValuePairI1K1VEEEEC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr void @_ZN5ArrayI11BuilderListI4SlotI12KeyValuePairI1K1VEEEE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z13ArrayIteratorI11BuilderListI4SlotI12KeyValuePairI1K1VEEEE) %0, ptr %1, ptr %2) {
entry:
  ret void
}

define linkonce_odr void @_ZN5ArrayI11BuilderListI4SlotI12KeyValuePairI1K1VEEEEC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr void @_ZN5ArrayI11BuilderListI4SlotI12KeyValuePairI1K1VEEEEC1Em(ptr %0, i64 %1) {
entry:
  ret void
}

define linkonce_odr void @_ZN5ArrayI11BuilderListI4SlotI12KeyValuePairI1K1VEEEEC1EPN4scaly6memory4PageE(ptr %0, ptr %1) {
entry:
  ret void
}

define linkonce_odr void @_ZN5ArrayI11BuilderListI4SlotI12KeyValuePairI1K1VEEEEC1EPN4scaly6memory4PageEm(ptr %0, ptr %1, i64 %2) {
entry:
  ret void
}

define linkonce_odr void @_ZN6VectorI11BuilderListI4SlotI12KeyValuePairI1K1VEEEEC1EPN4scaly6memory4PageE5ArrayI11BuilderListI4SlotI12KeyValuePairI1K1VEEEE(ptr %0, ptr %1, ptr %2) {
entry:
  ret void
}

define linkonce_odr ptr @_ZN4ListI11BuilderListI4SlotI12KeyValuePairI1K1VEEEE8get_headEPN4scaly6memory4PageE(ptr %0, ptr %1) {
entry:
  ret ptr null
}

define linkonce_odr ptr @_ZN12ListIteratorI11BuilderListI4SlotI12KeyValuePairI1K1VEEEE4nextEv(ptr %0) {
entry:
  ret ptr null
}

define linkonce_odr void @_ZN12ListIteratorI11BuilderListI4SlotI12KeyValuePairI1K1VEEEEC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr i64 @_ZN4ListI11BuilderListI4SlotI12KeyValuePairI1K1VEEEE5countEv(ptr %0) {
entry:
  ret i64 0
}

define linkonce_odr i1 @_ZN4ListI11BuilderListI4SlotI12KeyValuePairI1K1VEEEE6removeE11BuilderListI4SlotI12KeyValuePairI1K1VEEE(ptr %0, ptr %1) {
entry:
  ret i1 false
}

define linkonce_odr void @_ZN4ListI11BuilderListI4SlotI12KeyValuePairI1K1VEEEE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z12ListIteratorI11BuilderListI4SlotI12KeyValuePairI1K1VEEEE) %0, ptr %1, ptr %2) {
entry:
  ret void
}

define linkonce_odr void @_ZN4ListI11BuilderListI4SlotI12KeyValuePairI1K1VEEEE3addE11BuilderListI4SlotI12KeyValuePairI1K1VEEE(ptr %0, ptr %1) {
entry:
  ret void
}

define linkonce_odr void @_ZN4ListI11BuilderListI4SlotI12KeyValuePairI1K1VEEEEC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr void @_ZN6VectorI11BuilderListI4SlotI12KeyValuePairI1K1VEEEEC1EPN4scaly6memory4PageE4ListI11BuilderListI4SlotI12KeyValuePairI1K1VEEEE(ptr %0, ptr %1, ptr %2) {
entry:
  ret void
}

define linkonce_odr void @_ZN14HashMapBuilder10reallocateEm(ptr %0, i64 %1) {
entry:
  %deref.tmp = alloca %_Z4SlotI12KeyValuePairI1K1VEE, align 8
  %list_iterator = alloca %_Z19BuilderListIteratorI4SlotI12KeyValuePairI1K1VEEE, align 8
  %tuple = alloca %_Z19BuilderListIteratorI4SlotI12KeyValuePairI1K1VEEE, align 8
  %vector_iterator = alloca ptr, align 8
  %struct.init = alloca %_Z14VectorIteratorI11BuilderListI4SlotI12KeyValuePairI1K1VEEEE, align 8
  %call = call i64 @_ZN7hashing9get_primeEm(i64 %1)
  %call1 = call ptr @_ZN4Page3getEPv(ptr %0)
  %call2 = call ptr @_ZN4Page23allocate_exclusive_pageEv(ptr %call1)
  %struct.region = call ptr @_ZN4Page8allocateEmm(ptr %call2, i64 ptrtoint (ptr getelementptr (%_Z6VectorI11BuilderListI4SlotI12KeyValuePairI1K1VEEEE, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr ({ i1, %_Z6VectorI11BuilderListI4SlotI12KeyValuePairI1K1VEEEE }, ptr null, i64 0, i32 1) to i64))
  call void @_ZN6VectorI11BuilderListI4SlotI12KeyValuePairI1K1VEEEEC1EPN4scaly6memory4PageEm(ptr %struct.region, ptr %call2, i64 %call)
  %load.struct = load %_Z14HashMapBuilder, ptr %0, align 8
  %slots = extractvalue %_Z14HashMapBuilder %load.struct, 1
  %ne = icmp ne ptr %slots, null
  br i1 %ne, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %load.struct3 = load %_Z14HashMapBuilder, ptr %0, align 8
  %slots4 = extractvalue %_Z14HashMapBuilder %load.struct3, 1
  call void @_ZN14VectorIteratorI11BuilderListI4SlotI12KeyValuePairI1K1VEEEEC1EP6VectorI11BuilderListI4SlotI12KeyValuePairI1K1VEEEE(ptr %struct.init, ptr %slots4)
  store ptr %struct.init, ptr %vector_iterator, align 1
  br label %while.cond

if.end:                                           ; preds = %while.exit, %entry
  %slots19 = getelementptr inbounds %_Z14HashMapBuilder, ptr %0, i32 0, i32 1
  store ptr %struct.region, ptr %slots19, align 8
  ret void

while.cond:                                       ; preds = %while.exit10, %if.then
  %vector_iterator5 = load ptr, ptr %vector_iterator, align 8
  %call6 = call ptr @_ZN14VectorIteratorI11BuilderListI4SlotI12KeyValuePairI1K1VEEEE4nextEv(ptr %vector_iterator5)
  %while.tobool = icmp ne ptr %call6, null
  br i1 %while.tobool, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %load.struct7 = load %_Z11BuilderListI4SlotI12KeyValuePairI1K1VEEE, ptr %call6, align 8
  %head = extractvalue %_Z11BuilderListI4SlotI12KeyValuePairI1K1VEEE %load.struct7, 0
  %tuple.field = getelementptr inbounds %_Z19BuilderListIteratorI4SlotI12KeyValuePairI1K1VEEE, ptr %tuple, i32 0, i32 0
  store ptr %head, ptr %tuple.field, align 1
  %tuple.val = load %_Z19BuilderListIteratorI4SlotI12KeyValuePairI1K1VEEE, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %list_iterator, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z19BuilderListIteratorI4SlotI12KeyValuePairI1K1VEEE, ptr null, i32 1) to i64), i1 false)
  br label %while.cond8

while.exit:                                       ; preds = %while.cond
  %load.struct16 = load %_Z14HashMapBuilder, ptr %0, align 8
  %slots17 = extractvalue %_Z14HashMapBuilder %load.struct16, 1
  %call18 = call ptr @_ZN4Page3getEPv(ptr %slots17)
  call void @_ZN4Page25deallocate_exclusive_pageEP4Page(ptr %call1, ptr %call18)
  br label %if.end

while.cond8:                                      ; preds = %while.body9, %while.body
  %call11 = call ptr @_ZN19BuilderListIteratorI4SlotI12KeyValuePairI1K1VEEE4nextEv(ptr %list_iterator)
  %while.tobool12 = icmp ne ptr %call11, null
  br i1 %while.tobool12, label %while.body9, label %while.exit10

while.body9:                                      ; preds = %while.cond8
  %load.struct13 = load %_Z4SlotI12KeyValuePairI1K1VEE, ptr %call11, align 8
  %hash_code = extractvalue %_Z4SlotI12KeyValuePairI1K1VEE %load.struct13, 1
  %load.struct14 = load %_Z6VectorI11BuilderListI4SlotI12KeyValuePairI1K1VEEEE, ptr %struct.region, align 8
  %length = extractvalue %_Z6VectorI11BuilderListI4SlotI12KeyValuePairI1K1VEEEE %load.struct14, 0
  %urem = urem i64 %hash_code, %length
  %call15 = call ptr @_ZN6VectorI11BuilderListI4SlotI12KeyValuePairI1K1VEEEE7get_ptrEm(ptr %struct.region, i64 %urem)
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %deref.tmp, ptr align 1 %call11, i64 ptrtoint (ptr getelementptr (%_Z4SlotI12KeyValuePairI1K1VEE, ptr null, i32 1) to i64), i1 false)
  call void @_ZN11BuilderListI4SlotI12KeyValuePairI1K1VEEE3addE4SlotI12KeyValuePairI1K1VEE(ptr %call15, ptr %deref.tmp)
  br label %while.cond8

while.exit10:                                     ; preds = %while.cond8
  br label %while.cond
}

define linkonce_odr i1 @_ZN14HashMapBuilder3addE1K1V(ptr %0, ptr %1, ptr %2) {
entry:
  ret i1 false
}

define linkonce_odr i1 @_ZN14HashMapBuilder12add_internalE1K1V(ptr %0, ptr %1, ptr %2) {
entry:
  ret i1 false
}

define linkonce_odr i1 @_ZN14HashMapBuilder8containsE1K(ptr %0, ptr %1) {
entry:
  ret i1 false
}

define linkonce_odr ptr @_ZN14HashMapBuilder3getEPN4scaly6memory4PageE1K(ptr %0, ptr %1, ptr %2) {
entry:
  ret ptr null
}

define linkonce_odr void @_ZN14HashMapBuilderC1E6VectorI12KeyValuePairI1K1VEE(ptr %0, ptr %1) {
entry:
  ret void
}

define linkonce_odr void @_ZN14HashMapBuilderC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr ptr @_ZN6VectorI6VectorI12KeyValuePairI1K1VEEE3getEPN4scaly6memory4PageEm(ptr %0, ptr %1, i64 %2) {
entry:
  ret ptr null
}

define linkonce_odr ptr @_ZN6VectorI6VectorI12KeyValuePairI1K1VEEE7get_ptrEm(ptr %0, i64 %1) {
entry:
  ret ptr null
}

define linkonce_odr void @_ZN6VectorI6VectorI12KeyValuePairI1K1VEEE3putEm6VectorI12KeyValuePairI1K1VEE(ptr %0, i64 %1, ptr %2) {
entry:
  ret void
}

define linkonce_odr void @_ZN6VectorI6VectorI12KeyValuePairI1K1VEEE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z14VectorIteratorI6VectorI12KeyValuePairI1K1VEEE) %0, ptr %1, ptr %2) {
entry:
  ret void
}

define linkonce_odr void @_ZN6VectorI6VectorI12KeyValuePairI1K1VEEE8as_sliceEPN4scaly6memory4PageE(ptr noalias sret(%_Z5Slice) %0, ptr %1, ptr %2) {
entry:
  ret void
}

define linkonce_odr void @_ZN6VectorI6VectorI12KeyValuePairI1K1VEEEC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr void @_ZN6VectorI6VectorI12KeyValuePairI1K1VEEEC1EP6VectorI12KeyValuePairI1K1VEEm(ptr %0, ptr %1, i64 %2) {
entry:
  ret void
}

define linkonce_odr void @_ZN6VectorI6VectorI12KeyValuePairI1K1VEEEC1EPN4scaly6memory4PageEm(ptr %0, ptr %1, i64 %2) {
entry:
  ret void
}

define linkonce_odr void @_ZN6VectorI6VectorI12KeyValuePairI1K1VEEEC1EPN4scaly6memory4PageEP6VectorI12KeyValuePairI1K1VEEm(ptr %0, ptr %1, ptr %2, i64 %3) {
entry:
  ret void
}

define linkonce_odr void @_ZN6VectorI6VectorI12KeyValuePairI1K1VEEEC1EPN4scaly6memory4PageE6VectorI6VectorI12KeyValuePairI1K1VEEE(ptr %0, ptr %1, ptr %2) {
entry:
  ret void
}

define linkonce_odr ptr @_ZN5ArrayI6VectorI12KeyValuePairI1K1VEEE10get_bufferEv(ptr %0) {
entry:
  ret ptr null
}

define linkonce_odr i64 @_ZN5ArrayI6VectorI12KeyValuePairI1K1VEEE10get_lengthEv(ptr %0) {
entry:
  ret i64 0
}

define linkonce_odr i64 @_ZN5ArrayI6VectorI12KeyValuePairI1K1VEEE12get_capacityEv(ptr %0) {
entry:
  ret i64 0
}

define linkonce_odr void @_ZN5ArrayI6VectorI12KeyValuePairI1K1VEEE10reallocateEv(ptr %0) {
entry:
  ret void
}

define linkonce_odr void @_ZN5ArrayI6VectorI12KeyValuePairI1K1VEEE3addE6VectorI12KeyValuePairI1K1VEE(ptr %0, ptr %1) {
entry:
  ret void
}

define linkonce_odr void @_ZN5ArrayI6VectorI12KeyValuePairI1K1VEEE3addE6VectorI6VectorI12KeyValuePairI1K1VEEE(ptr %0, ptr %1) {
entry:
  ret void
}

define linkonce_odr ptr @_ZN5ArrayI6VectorI12KeyValuePairI1K1VEEE3getEPN4scaly6memory4PageEm(ptr %0, ptr %1, i64 %2) {
entry:
  ret ptr null
}

define linkonce_odr void @_ZN5ArrayI6VectorI12KeyValuePairI1K1VEEE3putEm6VectorI12KeyValuePairI1K1VEE(ptr %0, i64 %1, ptr %2) {
entry:
  ret void
}

define linkonce_odr ptr @_ZN13ArrayIteratorI6VectorI12KeyValuePairI1K1VEEE4nextEv(ptr %0) {
entry:
  ret ptr null
}

define linkonce_odr void @_ZN13ArrayIteratorI6VectorI12KeyValuePairI1K1VEEEC1EP5ArrayI6VectorI12KeyValuePairI1K1VEEE(ptr %0, ptr %1) {
entry:
  ret void
}

define linkonce_odr void @_ZN13ArrayIteratorI6VectorI12KeyValuePairI1K1VEEEC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr void @_ZN5ArrayI6VectorI12KeyValuePairI1K1VEEE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z13ArrayIteratorI6VectorI12KeyValuePairI1K1VEEE) %0, ptr %1, ptr %2) {
entry:
  ret void
}

define linkonce_odr void @_ZN5ArrayI6VectorI12KeyValuePairI1K1VEEEC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr void @_ZN5ArrayI6VectorI12KeyValuePairI1K1VEEEC1Em(ptr %0, i64 %1) {
entry:
  ret void
}

define linkonce_odr void @_ZN5ArrayI6VectorI12KeyValuePairI1K1VEEEC1EPN4scaly6memory4PageE(ptr %0, ptr %1) {
entry:
  ret void
}

define linkonce_odr void @_ZN5ArrayI6VectorI12KeyValuePairI1K1VEEEC1EPN4scaly6memory4PageEm(ptr %0, ptr %1, i64 %2) {
entry:
  ret void
}

define linkonce_odr void @_ZN6VectorI6VectorI12KeyValuePairI1K1VEEEC1EPN4scaly6memory4PageE5ArrayI6VectorI12KeyValuePairI1K1VEEE(ptr %0, ptr %1, ptr %2) {
entry:
  ret void
}

define linkonce_odr ptr @_ZN4ListI6VectorI12KeyValuePairI1K1VEEE8get_headEPN4scaly6memory4PageE(ptr %0, ptr %1) {
entry:
  ret ptr null
}

define linkonce_odr ptr @_ZN12ListIteratorI6VectorI12KeyValuePairI1K1VEEE4nextEv(ptr %0) {
entry:
  ret ptr null
}

define linkonce_odr void @_ZN12ListIteratorI6VectorI12KeyValuePairI1K1VEEEC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr i64 @_ZN4ListI6VectorI12KeyValuePairI1K1VEEE5countEv(ptr %0) {
entry:
  ret i64 0
}

define linkonce_odr i1 @_ZN4ListI6VectorI12KeyValuePairI1K1VEEE6removeE6VectorI12KeyValuePairI1K1VEE(ptr %0, ptr %1) {
entry:
  ret i1 false
}

define linkonce_odr void @_ZN4ListI6VectorI12KeyValuePairI1K1VEEE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z12ListIteratorI6VectorI12KeyValuePairI1K1VEEE) %0, ptr %1, ptr %2) {
entry:
  ret void
}

define linkonce_odr void @_ZN4ListI6VectorI12KeyValuePairI1K1VEEE3addE6VectorI12KeyValuePairI1K1VEE(ptr %0, ptr %1) {
entry:
  ret void
}

define linkonce_odr void @_ZN4ListI6VectorI12KeyValuePairI1K1VEEEC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr void @_ZN6VectorI6VectorI12KeyValuePairI1K1VEEEC1EPN4scaly6memory4PageE4ListI6VectorI12KeyValuePairI1K1VEEE(ptr %0, ptr %1, ptr %2) {
entry:
  ret void
}

define linkonce_odr ptr @_ZN14VectorIteratorI6VectorI12KeyValuePairI1K1VEEE4nextEv(ptr %0) {
entry:
  ret ptr null
}

define linkonce_odr void @_ZN14VectorIteratorI6VectorI12KeyValuePairI1K1VEEEC1EP6VectorI6VectorI12KeyValuePairI1K1VEEE(ptr %0, ptr %1) {
entry:
  ret void
}

define linkonce_odr void @_ZN14VectorIteratorI6VectorI12KeyValuePairI1K1VEEEC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr ptr @_ZN15HashMapIteratorI1K1VE4nextEv(ptr %0) {
entry:
  ret ptr null
}

define linkonce_odr void @_ZN15HashMapIteratorI1K1VEC1E7HashMapI1K1VE(ptr %0, ptr %1) {
entry:
  ret void
}

define linkonce_odr void @_ZN15HashMapIteratorI1K1VEC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr ptr @_ZN15HashMapIterator4nextEv(ptr %0) {
entry:
  %struct.init = alloca %_Z14VectorIteratorI12KeyValuePairI1K1VEE, align 8
  br label %while.cond

while.cond:                                       ; preds = %if.end4, %entry
  br i1 true, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %field.inplace = getelementptr inbounds %_Z15HashMapIteratorI1K1VE, ptr %0, i32 0, i32 1
  %call = call ptr @_ZN14VectorIteratorI12KeyValuePairI1K1VEE4nextEv(ptr %field.inplace)
  %ne = icmp ne ptr %call, null
  br i1 %ne, label %if.then, label %if.end

while.exit:                                       ; preds = %while.cond
  ret ptr null

if.then:                                          ; preds = %while.body
  %load.struct = load %_Z12KeyValuePairI1K1VE, ptr %call, align 8
  %value = extractvalue %_Z12KeyValuePairI1K1VE %load.struct, 1
  ret ptr %value

if.end:                                           ; preds = %while.body
  %field.inplace1 = getelementptr inbounds %_Z15HashMapIteratorI1K1VE, ptr %0, i32 0, i32 0
  %call2 = call ptr @_ZN14VectorIteratorI6VectorI12KeyValuePairI1K1VEEE4nextEv(ptr %field.inplace1)
  %eq = icmp eq ptr %call2, null
  br i1 %eq, label %if.then3, label %if.end4

if.then3:                                         ; preds = %if.end
  ret ptr null

if.end4:                                          ; preds = %if.end
  call void @_ZN14VectorIteratorI12KeyValuePairI1K1VEEC1EP6VectorI12KeyValuePairI1K1VEE(ptr %struct.init, ptr %call2)
  %parent.page = call ptr @_ZN4Page3getEPv(ptr %0)
  %ctor.heap = call ptr @_ZN4Page8allocateEmm(ptr %parent.page, i64 ptrtoint (ptr getelementptr (%_Z14VectorIteratorI12KeyValuePairI1K1VEE, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr ({ i1, %_Z14VectorIteratorI12KeyValuePairI1K1VEE }, ptr null, i64 0, i32 1) to i64))
  %1 = call ptr @memcpy(ptr %ctor.heap, ptr %struct.init, i64 ptrtoint (ptr getelementptr (%_Z14VectorIteratorI12KeyValuePairI1K1VEE, ptr null, i32 1) to i64))
  %element_iterator = getelementptr inbounds %_Z15HashMapIteratorI1K1VE, ptr %0, i32 0, i32 1
  %field.load = load %_Z14VectorIteratorI12KeyValuePairI1K1VEE, ptr %ctor.heap, align 8
  store %_Z14VectorIteratorI12KeyValuePairI1K1VEE %field.load, ptr %element_iterator, align 8
  br label %while.cond
}

define linkonce_odr void @_ZN15HashMapIteratorC1E7HashMapI1K1VE(ptr %0, ptr %1) {
entry:
  ret void
}

define linkonce_odr void @_ZN15HashMapIteratorC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr i1 @_ZN7HashMap8containsE1K(ptr %0, ptr %1) {
entry:
  ret i1 false
}

define linkonce_odr ptr @_ZN7HashMap3getEPN4scaly6memory4PageE1K(ptr %0, ptr %1, ptr %2) {
entry:
  ret ptr null
}

define linkonce_odr ptr @_ZN12ListIteratorI4SlotI12KeyValuePairI1K1VEEE4nextEv(ptr %0) {
entry:
  ret ptr null
}

define linkonce_odr void @_ZN12ListIteratorI4SlotI12KeyValuePairI1K1VEEEC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr void @_ZN7HashMapC1EPN4scaly6memory4PageE14HashMapBuilderI1K1VE(ptr %0, ptr %1, ptr %2) {
entry:
  ret void
}

define linkonce_odr void @_ZN7HashMapC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr ptr @_ZN6String10get_bufferEv(ptr %0) {
entry:
  %byte = alloca i8, align 1
  %index = alloca i64, align 8
  %bit_count = alloca i64, align 8
  %length = alloca i64, align 8
  %load.struct = load %_Z6String, ptr %0, align 8
  %data = extractvalue %_Z6String %load.struct, 0
  %eq = icmp eq ptr %data, null
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  store i64 0, ptr %length, align 1
  store i64 0, ptr %bit_count, align 1
  store i64 0, ptr %index, align 1
  br label %while.cond

while.cond:                                       ; preds = %if.end15, %if.end
  br i1 true, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %bit_count1 = load i64, ptr %bit_count, align 4
  %eq2 = icmp eq i64 %bit_count1, 63
  br i1 %eq2, label %if.then3, label %if.end4

while.exit:                                       ; preds = %if.then14, %while.cond
  %load.struct19 = load %_Z6String, ptr %0, align 8
  %data20 = extractvalue %_Z6String %load.struct19, 0
  %index21 = load i64, ptr %index, align 4
  %ptr.add22 = getelementptr inbounds i8, ptr %data20, i64 %index21
  %ptr.add23 = getelementptr inbounds i8, ptr %ptr.add22, i64 1
  ret ptr %ptr.add23

if.then3:                                         ; preds = %while.body
  call void @exit(i64 11)
  br label %if.end4

if.end4:                                          ; preds = %if.then3, %while.body
  %load.struct5 = load %_Z6String, ptr %0, align 8
  %data6 = extractvalue %_Z6String %load.struct5, 0
  %index7 = load i64, ptr %index, align 4
  %ptr.add = getelementptr inbounds i8, ptr %data6, i64 %index7
  %deref = load i8, ptr %ptr.add, align 1
  store i8 %deref, ptr %byte, align 1
  %length8 = load i64, ptr %length, align 4
  %byte9 = load i8, ptr %byte, align 1
  %and = and i8 %byte9, 127
  %as.zext = zext i8 %and to i64
  %bit_count10 = load i64, ptr %bit_count, align 4
  %shl = shl i64 %as.zext, %bit_count10
  %or = or i64 %length8, %shl
  store i64 %or, ptr %length, align 1
  %byte11 = load i8, ptr %byte, align 1
  %and12 = and i8 %byte11, -128
  %eq13 = icmp eq i8 %and12, 0
  br i1 %eq13, label %if.then14, label %if.end15

if.then14:                                        ; preds = %if.end4
  br label %while.exit

if.end15:                                         ; preds = %if.end4
  %bit_count16 = load i64, ptr %bit_count, align 4
  %add = add i64 %bit_count16, 7
  store i64 %add, ptr %bit_count, align 1
  %index17 = load i64, ptr %index, align 4
  %add18 = add i64 %index17, 1
  store i64 %add18, ptr %index, align 1
  br label %while.cond
}

define linkonce_odr ptr @_ZN6String6c_dataEv(ptr %0) {
entry:
  %call = call ptr @_ZN6String10get_bufferEv(ptr %0)
  ret ptr %call
}

define linkonce_odr ptr @_ZN6String11to_c_stringEPN4scaly6memory4PageE(ptr %0, ptr %1) {
entry:
  %dest = alloca ptr, align 8
  %byte = alloca i8, align 1
  %length = alloca i64, align 8
  store i64 0, ptr %length, align 1
  %bit_count = alloca i64, align 8
  store i64 0, ptr %bit_count, align 1
  %index = alloca i64, align 8
  store i64 0, ptr %index, align 1
  %load.struct = load %_Z6String, ptr %1, align 8
  %data = extractvalue %_Z6String %load.struct, 0
  %ne = icmp ne ptr %data, null
  br i1 %ne, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  br label %while.cond

if.end:                                           ; preds = %while.exit, %entry
  %length18 = load i64, ptr %length, align 4
  %add19 = add i64 %length18, 1
  %call = call ptr @_ZN4Page8allocateEmm(ptr %0, i64 %add19, i64 1)
  store ptr %call, ptr %dest, align 1
  %load.struct20 = load %_Z6String, ptr %1, align 8
  %data21 = extractvalue %_Z6String %load.struct20, 0
  %ne22 = icmp ne ptr %data21, null
  br i1 %ne22, label %if.then23, label %if.end24

while.cond:                                       ; preds = %if.end14, %if.then
  br i1 true, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %bit_count1 = load i64, ptr %bit_count, align 4
  %eq = icmp eq i64 %bit_count1, 63
  br i1 %eq, label %if.then2, label %if.end3

while.exit:                                       ; preds = %if.then13, %while.cond
  br label %if.end

if.then2:                                         ; preds = %while.body
  call void @exit(i64 11)
  br label %if.end3

if.end3:                                          ; preds = %if.then2, %while.body
  %load.struct4 = load %_Z6String, ptr %1, align 8
  %data5 = extractvalue %_Z6String %load.struct4, 0
  %index6 = load i64, ptr %index, align 4
  %ptr.add = getelementptr inbounds i8, ptr %data5, i64 %index6
  %deref = load i8, ptr %ptr.add, align 1
  store i8 %deref, ptr %byte, align 1
  %length7 = load i64, ptr %length, align 4
  %byte8 = load i8, ptr %byte, align 1
  %and = and i8 %byte8, 127
  %as.zext = zext i8 %and to i64
  %bit_count9 = load i64, ptr %bit_count, align 4
  %shl = shl i64 %as.zext, %bit_count9
  %or = or i64 %length7, %shl
  store i64 %or, ptr %length, align 1
  %byte10 = load i8, ptr %byte, align 1
  %and11 = and i8 %byte10, -128
  %eq12 = icmp eq i8 %and11, 0
  br i1 %eq12, label %if.then13, label %if.end14

if.then13:                                        ; preds = %if.end3
  br label %while.exit

if.end14:                                         ; preds = %if.end3
  %bit_count15 = load i64, ptr %bit_count, align 4
  %add = add i64 %bit_count15, 7
  store i64 %add, ptr %bit_count, align 1
  %index16 = load i64, ptr %index, align 4
  %add17 = add i64 %index16, 1
  store i64 %add17, ptr %index, align 1
  br label %while.cond

if.then23:                                        ; preds = %if.end
  %dest25 = load ptr, ptr %dest, align 8
  %load.struct26 = load %_Z6String, ptr %1, align 8
  %data27 = extractvalue %_Z6String %load.struct26, 0
  %index28 = load i64, ptr %index, align 4
  %ptr.add29 = getelementptr inbounds i8, ptr %data27, i64 %index28
  %ptr.add30 = getelementptr inbounds i8, ptr %ptr.add29, i64 1
  %length31 = load i64, ptr %length, align 4
  %call32 = call ptr @memcpy(ptr %dest25, ptr %ptr.add30, i64 %length31)
  br label %if.end24

if.end24:                                         ; preds = %if.then23, %if.end
  %dest33 = load ptr, ptr %dest, align 8
  %length34 = load i64, ptr %length, align 4
  %ptr.add35 = getelementptr inbounds i8, ptr %dest33, i64 %length34
  store i8 0, ptr %ptr.add35, align 1
  %dest36 = load ptr, ptr %dest, align 8
  ret ptr %dest36
}

define linkonce_odr i8 @_ZN6String3getEm(ptr %0, i64 %1) {
entry:
  %call = call ptr @_ZN6String10get_bufferEv(ptr %0)
  %eq = icmp eq ptr %call, null
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret i8 0

if.end:                                           ; preds = %entry
  %call1 = call i64 @_ZN6String10get_lengthEv(ptr %0)
  %ge = icmp uge i64 %1, %call1
  br i1 %ge, label %if.then2, label %if.end3

if.then2:                                         ; preds = %if.end
  ret i8 0

if.end3:                                          ; preds = %if.end
  %ptr.add = getelementptr inbounds i8, ptr %call, i64 %1
  %deref = load i8, ptr %ptr.add, align 1
  ret i8 %deref
}

define linkonce_odr i64 @_ZN6String6lengthEv(ptr %0) {
entry:
  %call = call i64 @_ZN6String10get_lengthEv(ptr %0)
  ret i64 %call
}

define linkonce_odr void @_ZN6String9substringEPN4scaly6memory4PageEP4Pagemm(ptr noalias sret(%_Z6String) %0, ptr %1, ptr %2, ptr %3, i64 %4, i64 %5) {
entry:
  %struct.region = call ptr @_ZN4Page8allocateEmm(ptr %3, i64 ptrtoint (ptr getelementptr (%_Z6String, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr ({ i1, %_Z6String }, ptr null, i64 0, i32 1) to i64))
  %call = call ptr @_ZN6String10get_bufferEv(ptr %2)
  %ptr.add = getelementptr inbounds i8, ptr %call, i64 %4
  call void @_ZN6StringC1EPN4scaly6memory4PageEP10const_charm(ptr %struct.region, ptr %3, ptr %ptr.add, i64 %5)
  %sret.body = load %_Z6String, ptr %struct.region, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %struct.region, i64 ptrtoint (ptr getelementptr (%_Z6String, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr i1 @_ZN6String6equalsEP10const_char(ptr %0, ptr %1) {
entry:
  %byte = alloca i8, align 1
  %bit_count = alloca i64, align 8
  %length = alloca i64, align 8
  store i64 0, ptr %length, align 1
  %index = alloca i64, align 8
  store i64 0, ptr %index, align 1
  %load.struct = load %_Z6String, ptr %0, align 8
  %data = extractvalue %_Z6String %load.struct, 0
  %ne = icmp ne ptr %data, null
  br i1 %ne, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  store i64 0, ptr %bit_count, align 1
  br label %while.cond

if.end:                                           ; preds = %while.exit, %entry
  %length18 = load i64, ptr %length, align 4
  %call = call i64 @strlen(ptr %1)
  %ne19 = icmp ne i64 %length18, %call
  br i1 %ne19, label %if.then20, label %if.end21

while.cond:                                       ; preds = %if.end14, %if.then
  br i1 true, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %bit_count1 = load i64, ptr %bit_count, align 4
  %eq = icmp eq i64 %bit_count1, 63
  br i1 %eq, label %if.then2, label %if.end3

while.exit:                                       ; preds = %if.then13, %while.cond
  br label %if.end

if.then2:                                         ; preds = %while.body
  call void @exit(i64 13)
  br label %if.end3

if.end3:                                          ; preds = %if.then2, %while.body
  %load.struct4 = load %_Z6String, ptr %0, align 8
  %data5 = extractvalue %_Z6String %load.struct4, 0
  %index6 = load i64, ptr %index, align 4
  %ptr.add = getelementptr inbounds i8, ptr %data5, i64 %index6
  %deref = load i8, ptr %ptr.add, align 1
  store i8 %deref, ptr %byte, align 1
  %length7 = load i64, ptr %length, align 4
  %byte8 = load i8, ptr %byte, align 1
  %and = and i8 %byte8, 127
  %as.zext = zext i8 %and to i64
  %bit_count9 = load i64, ptr %bit_count, align 4
  %shl = shl i64 %as.zext, %bit_count9
  %or = or i64 %length7, %shl
  store i64 %or, ptr %length, align 1
  %byte10 = load i8, ptr %byte, align 1
  %and11 = and i8 %byte10, -128
  %eq12 = icmp eq i8 %and11, 0
  br i1 %eq12, label %if.then13, label %if.end14

if.then13:                                        ; preds = %if.end3
  br label %while.exit

if.end14:                                         ; preds = %if.end3
  %bit_count15 = load i64, ptr %bit_count, align 4
  %add = add i64 %bit_count15, 7
  store i64 %add, ptr %bit_count, align 1
  %index16 = load i64, ptr %index, align 4
  %add17 = add i64 %index16, 1
  store i64 %add17, ptr %index, align 1
  br label %while.cond

if.then20:                                        ; preds = %if.end
  ret i1 false

if.end21:                                         ; preds = %if.end
  %load.struct22 = load %_Z6String, ptr %0, align 8
  %data23 = extractvalue %_Z6String %load.struct22, 0
  %eq24 = icmp eq ptr %data23, null
  br i1 %eq24, label %if.then25, label %if.end26

if.then25:                                        ; preds = %if.end21
  ret i1 true

if.end26:                                         ; preds = %if.end21
  %load.struct27 = load %_Z6String, ptr %0, align 8
  %data28 = extractvalue %_Z6String %load.struct27, 0
  %index29 = load i64, ptr %index, align 4
  %ptr.add30 = getelementptr inbounds i8, ptr %data28, i64 %index29
  %ptr.add31 = getelementptr inbounds i8, ptr %ptr.add30, i64 1
  %length32 = load i64, ptr %length, align 4
  %call33 = call i32 @memcmp(ptr %ptr.add31, ptr %1, i64 %length32)
  %zext = zext i32 %call33 to i64
  %eq34 = icmp eq i64 %zext, 0
  ret i1 %eq34
}

define linkonce_odr ptr @_ZN6VectorIcE3getEPN4scaly6memory4PageEm(ptr %0, ptr %1, i64 %2) {
entry:
  %load.struct = load %_Z6VectorIcE, ptr %1, align 8
  %length = extractvalue %_Z6VectorIcE %load.struct, 0
  %ge = icmp uge i64 %2, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %load.struct1 = load %_Z6VectorIcE, ptr %1, align 8
  %data = extractvalue %_Z6VectorIcE %load.struct1, 1
  %ptr.add = getelementptr inbounds i8, ptr %data, i64 %2
  ret ptr %ptr.add
}

define linkonce_odr ptr @_ZN6VectorIcE7get_ptrEm(ptr %0, i64 %1) {
entry:
  %load.struct = load %_Z6VectorIcE, ptr %0, align 8
  %length = extractvalue %_Z6VectorIcE %load.struct, 0
  %ge = icmp uge i64 %1, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %load.struct1 = load %_Z6VectorIcE, ptr %0, align 8
  %data = extractvalue %_Z6VectorIcE %load.struct1, 1
  %ptr.add = getelementptr inbounds i8, ptr %data, i64 %1
  ret ptr %ptr.add
}

define linkonce_odr void @_ZN6VectorIcE3putEmc(ptr %0, i64 %1, i8 %2) {
entry:
  %load.struct = load %_Z6VectorIcE, ptr %0, align 8
  %length = extractvalue %_Z6VectorIcE %load.struct, 0
  %ge = icmp uge i64 %1, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  call void @_Z11scaly_eputsP10const_char(ptr @.str.19)
  call void @_Z12scaly_eputnlv()
  call void @exit(i64 15)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %load.struct1 = load %_Z6VectorIcE, ptr %0, align 8
  %data = extractvalue %_Z6VectorIcE %load.struct1, 1
  %ptr.add = getelementptr inbounds i8, ptr %data, i64 %1
  store i8 %2, ptr %ptr.add, align 1
  ret void
}

define linkonce_odr ptr @_ZN14VectorIteratorIcE4nextEv(ptr %0) {
entry:
  %load.struct = load %_Z14VectorIteratorIcE, ptr %0, align 8
  %vector = extractvalue %_Z14VectorIteratorIcE %load.struct, 0
  %eq = icmp eq ptr %vector, null
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %load.struct1 = load %_Z14VectorIteratorIcE, ptr %0, align 8
  %position = extractvalue %_Z14VectorIteratorIcE %load.struct1, 1
  %load.struct2 = load %_Z14VectorIteratorIcE, ptr %0, align 8
  %vector3 = extractvalue %_Z14VectorIteratorIcE %load.struct2, 0
  %deref = load %_Z6VectorIcE, ptr %vector3, align 8
  %length = extractvalue %_Z6VectorIcE %deref, 0
  %eq4 = icmp eq i64 %position, %length
  br i1 %eq4, label %if.then5, label %if.end6

if.then5:                                         ; preds = %if.end
  ret ptr null

if.end6:                                          ; preds = %if.end
  %load.struct7 = load %_Z14VectorIteratorIcE, ptr %0, align 8
  %position8 = extractvalue %_Z14VectorIteratorIcE %load.struct7, 1
  %add = add i64 %position8, 1
  %position9 = getelementptr inbounds %_Z14VectorIteratorIcE, ptr %0, i32 0, i32 1
  store i64 %add, ptr %position9, align 4
  %field.inplace = getelementptr inbounds %_Z14VectorIteratorIcE, ptr %0, i32 0, i32 0
  %deref.recv = load ptr, ptr %field.inplace, align 8
  %load.struct10 = load %_Z14VectorIteratorIcE, ptr %0, align 8
  %position11 = extractvalue %_Z14VectorIteratorIcE %load.struct10, 1
  %sub = sub i64 %position11, 1
  %call = call ptr @_ZN6VectorIcE7get_ptrEm(ptr %deref.recv, i64 %sub)
  ret ptr %call
}

define linkonce_odr void @_ZN14VectorIteratorIcEC1EP6VectorIcE(ptr %0, ptr %1) {
entry:
  %vector = getelementptr inbounds %_Z14VectorIteratorIcE, ptr %0, i32 0, i32 0
  store ptr %1, ptr %vector, align 8
  %position = getelementptr inbounds %_Z14VectorIteratorIcE, ptr %0, i32 0, i32 1
  store i64 0, ptr %position, align 4
  ret void
}

define linkonce_odr void @_ZN14VectorIteratorIcEC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr void @_ZN6VectorIcE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z14VectorIteratorIcE) %0, ptr %1, ptr %2) {
entry:
  %struct.init = alloca %_Z14VectorIteratorIcE, align 8
  call void @_ZN14VectorIteratorIcEC1EP6VectorIcE(ptr %struct.init, ptr %2)
  %sret.body = load %_Z14VectorIteratorIcE, ptr %struct.init, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %struct.init, i64 ptrtoint (ptr getelementptr (%_Z14VectorIteratorIcE, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN6VectorIcE8as_sliceEPN4scaly6memory4PageE(ptr noalias sret(%_Z5Slice) %0, ptr %1, ptr %2) {
entry:
  %load.struct = load %_Z6VectorIcE, ptr %2, align 8
  %data = extractvalue %_Z6VectorIcE %load.struct, 1
  %load.struct1 = load %_Z6VectorIcE, ptr %2, align 8
  %length = extractvalue %_Z6VectorIcE %load.struct1, 0
  %tuple = alloca { ptr, i64 }, align 8
  %tuple.field = getelementptr inbounds { ptr, i64 }, ptr %tuple, i32 0, i32 0
  store ptr %data, ptr %tuple.field, align 1
  %tuple.field2 = getelementptr inbounds { ptr, i64 }, ptr %tuple, i32 0, i32 1
  store i64 %length, ptr %tuple.field2, align 1
  %tuple.val = load { ptr, i64 }, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr ({ ptr, i64 }, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN6VectorIcEC1Ev(ptr %0) {
entry:
  %length = getelementptr inbounds %_Z6VectorIcE, ptr %0, i32 0, i32 0
  store i64 0, ptr %length, align 4
  %data = getelementptr inbounds %_Z6VectorIcE, ptr %0, i32 0, i32 1
  store ptr null, ptr %data, align 8
  ret void
}

define linkonce_odr void @_ZN6VectorIcEC1EPcm(ptr %0, ptr %1, i64 %2) {
entry:
  %length = getelementptr inbounds %_Z6VectorIcE, ptr %0, i32 0, i32 0
  store i64 %2, ptr %length, align 4
  %data = getelementptr inbounds %_Z6VectorIcE, ptr %0, i32 0, i32 1
  store ptr %1, ptr %data, align 8
  ret void
}

define linkonce_odr void @_ZN6VectorIcEC1EPN4scaly6memory4PageEm(ptr %0, ptr %1, i64 %2) {
entry:
  %length = getelementptr inbounds %_Z6VectorIcE, ptr %0, i32 0, i32 0
  store i64 %2, ptr %length, align 4
  %gt = icmp ugt i64 %2, 0
  br i1 %gt, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %mul = mul i64 %2, ptrtoint (ptr getelementptr (i8, ptr null, i32 1) to i64)
  %call = call ptr @_ZN4Page8allocateEmm(ptr %1, i64 %mul, i64 1)
  %data = getelementptr inbounds %_Z6VectorIcE, ptr %0, i32 0, i32 1
  store ptr %call, ptr %data, align 8
  %field.inplace = getelementptr inbounds %_Z6VectorIcE, ptr %0, i32 0, i32 1
  %deref.recv = load ptr, ptr %field.inplace, align 8
  %mul1 = mul i64 %2, ptrtoint (ptr getelementptr (i8, ptr null, i32 1) to i64)
  %call2 = call ptr @memset(ptr %deref.recv, i64 0, i64 %mul1)
  br label %if.end

if.else:                                          ; preds = %entry
  %data3 = getelementptr inbounds %_Z6VectorIcE, ptr %0, i32 0, i32 1
  store ptr null, ptr %data3, align 8
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  %if.value = phi ptr [ %call2, %if.then ], [ null, %if.else ]
  ret void
}

define linkonce_odr void @_ZN6VectorIcEC1EPN4scaly6memory4PageEPcm(ptr %0, ptr %1, ptr %2, i64 %3) {
entry:
  %length = getelementptr inbounds %_Z6VectorIcE, ptr %0, i32 0, i32 0
  store i64 %3, ptr %length, align 4
  %gt = icmp ugt i64 %3, 0
  br i1 %gt, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %mul = mul i64 %3, ptrtoint (ptr getelementptr (i8, ptr null, i32 1) to i64)
  %call = call ptr @_ZN4Page8allocateEmm(ptr %1, i64 %mul, i64 1)
  %data = getelementptr inbounds %_Z6VectorIcE, ptr %0, i32 0, i32 1
  store ptr %call, ptr %data, align 8
  %field.inplace = getelementptr inbounds %_Z6VectorIcE, ptr %0, i32 0, i32 1
  %deref.recv = load ptr, ptr %field.inplace, align 8
  %mul1 = mul i64 %3, ptrtoint (ptr getelementptr (i8, ptr null, i32 1) to i64)
  %call2 = call ptr @memcpy(ptr %deref.recv, ptr %2, i64 %mul1)
  br label %if.end

if.else:                                          ; preds = %entry
  %data3 = getelementptr inbounds %_Z6VectorIcE, ptr %0, i32 0, i32 1
  store ptr null, ptr %data3, align 8
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  %if.value = phi ptr [ %call2, %if.then ], [ null, %if.else ]
  ret void
}

define linkonce_odr void @_ZN6VectorIcEC1EPN4scaly6memory4PageE6VectorIcE(ptr %0, ptr %1, ptr %2) {
entry:
  %load.struct = load %_Z6VectorIcE, ptr %2, align 8
  %length = extractvalue %_Z6VectorIcE %load.struct, 0
  %length1 = getelementptr inbounds %_Z6VectorIcE, ptr %0, i32 0, i32 0
  store i64 %length, ptr %length1, align 4
  %load.struct2 = load %_Z6VectorIcE, ptr %0, align 8
  %length3 = extractvalue %_Z6VectorIcE %load.struct2, 0
  %gt = icmp ugt i64 %length3, 0
  br i1 %gt, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %load.struct4 = load %_Z6VectorIcE, ptr %0, align 8
  %length5 = extractvalue %_Z6VectorIcE %load.struct4, 0
  %mul = mul i64 %length5, ptrtoint (ptr getelementptr (i8, ptr null, i32 1) to i64)
  %call = call ptr @_ZN4Page8allocateEmm(ptr %1, i64 %mul, i64 1)
  %data = getelementptr inbounds %_Z6VectorIcE, ptr %0, i32 0, i32 1
  store ptr %call, ptr %data, align 8
  %field.inplace = getelementptr inbounds %_Z6VectorIcE, ptr %0, i32 0, i32 1
  %deref.recv = load ptr, ptr %field.inplace, align 8
  %field.inplace6 = getelementptr inbounds %_Z6VectorIcE, ptr %2, i32 0, i32 1
  %deref.recv7 = load ptr, ptr %field.inplace6, align 8
  %load.struct8 = load %_Z6VectorIcE, ptr %0, align 8
  %length9 = extractvalue %_Z6VectorIcE %load.struct8, 0
  %mul10 = mul i64 %length9, ptrtoint (ptr getelementptr (i8, ptr null, i32 1) to i64)
  %call11 = call ptr @memcpy(ptr %deref.recv, ptr %deref.recv7, i64 %mul10)
  br label %if.end

if.else:                                          ; preds = %entry
  %data12 = getelementptr inbounds %_Z6VectorIcE, ptr %0, i32 0, i32 1
  store ptr null, ptr %data12, align 8
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  %if.value = phi ptr [ %call11, %if.then ], [ null, %if.else ]
  ret void
}

define linkonce_odr ptr @_ZN5ArrayIcE10get_bufferEv(ptr %0) {
entry:
  %load.struct = load %_Z5ArrayIcE, ptr %0, align 8
  %vector = extractvalue %_Z5ArrayIcE %load.struct, 1
  %eq = icmp eq ptr %vector, null
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %load.struct1 = load %_Z5ArrayIcE, ptr %0, align 8
  %vector2 = extractvalue %_Z5ArrayIcE %load.struct1, 1
  %deref = load %_Z6VectorIcE, ptr %vector2, align 8
  %data = extractvalue %_Z6VectorIcE %deref, 1
  ret ptr %data
}

define linkonce_odr i64 @_ZN5ArrayIcE10get_lengthEv(ptr %0) {
entry:
  %load.struct = load %_Z5ArrayIcE, ptr %0, align 8
  %length = extractvalue %_Z5ArrayIcE %load.struct, 0
  ret i64 %length
}

define linkonce_odr i64 @_ZN5ArrayIcE12get_capacityEv(ptr %0) {
entry:
  %load.struct = load %_Z5ArrayIcE, ptr %0, align 8
  %vector = extractvalue %_Z5ArrayIcE %load.struct, 1
  %eq = icmp eq ptr %vector, null
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret i64 0

if.end:                                           ; preds = %entry
  %load.struct1 = load %_Z5ArrayIcE, ptr %0, align 8
  %vector2 = extractvalue %_Z5ArrayIcE %load.struct1, 1
  %deref = load %_Z6VectorIcE, ptr %vector2, align 8
  %length = extractvalue %_Z6VectorIcE %deref, 0
  ret i64 %length
}

define linkonce_odr void @_ZN5ArrayIcE10reallocateEv(ptr %0) {
entry:
  %call = call ptr @_ZN4Page3getEPv(ptr %0)
  %length = alloca i64, align 8
  store i64 0, ptr %length, align 1
  %load.struct = load %_Z5ArrayIcE, ptr %0, align 8
  %vector = extractvalue %_Z5ArrayIcE %load.struct, 1
  %eq = icmp eq ptr %vector, null
  br i1 %eq, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %call1 = call ptr @_ZN4Page23allocate_exclusive_pageEv(ptr %call)
  %call2 = call i64 @_ZN4Page12get_capacityEm(ptr %call1, i64 1)
  %sub = sub i64 %call2, ptrtoint (ptr getelementptr (%_Z6VectorIcE, ptr null, i32 1) to i64)
  %udiv = udiv i64 %sub, ptrtoint (ptr getelementptr (i8, ptr null, i32 1) to i64)
  store i64 %udiv, ptr %length, align 1
  %struct.region = call ptr @_ZN4Page8allocateEmm(ptr %call1, i64 ptrtoint (ptr getelementptr (%_Z6VectorIcE, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr ({ i1, %_Z6VectorIcE }, ptr null, i64 0, i32 1) to i64))
  %length3 = load i64, ptr %length, align 4
  call void @_ZN6VectorIcEC1EPN4scaly6memory4PageEm(ptr %struct.region, ptr %call1, i64 %length3)
  %vector4 = getelementptr inbounds %_Z5ArrayIcE, ptr %0, i32 0, i32 1
  store ptr %struct.region, ptr %vector4, align 8
  br label %if.end

if.else:                                          ; preds = %entry
  %load.struct5 = load %_Z5ArrayIcE, ptr %0, align 8
  %vector6 = extractvalue %_Z5ArrayIcE %load.struct5, 1
  %deref = load %_Z6VectorIcE, ptr %vector6, align 8
  %length7 = extractvalue %_Z6VectorIcE %deref, 0
  %mul = mul i64 %length7, 2
  store i64 %mul, ptr %length, align 1
  %call8 = call ptr @_ZN4Page23allocate_exclusive_pageEv(ptr %call)
  %struct.region9 = call ptr @_ZN4Page8allocateEmm(ptr %call8, i64 ptrtoint (ptr getelementptr (%_Z6VectorIcE, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr ({ i1, %_Z6VectorIcE }, ptr null, i64 0, i32 1) to i64))
  %length10 = load i64, ptr %length, align 4
  call void @_ZN6VectorIcEC1EPN4scaly6memory4PageEm(ptr %struct.region9, ptr %call8, i64 %length10)
  %load.struct11 = load %_Z5ArrayIcE, ptr %0, align 8
  %vector12 = extractvalue %_Z5ArrayIcE %load.struct11, 1
  %deref13 = load %_Z6VectorIcE, ptr %vector12, align 8
  %length14 = extractvalue %_Z6VectorIcE %deref13, 0
  %mul15 = mul i64 %length14, ptrtoint (ptr getelementptr (i8, ptr null, i32 1) to i64)
  %field.inplace = getelementptr inbounds %_Z6VectorIcE, ptr %struct.region9, i32 0, i32 1
  %deref.recv = load ptr, ptr %field.inplace, align 8
  %field.inplace16 = getelementptr inbounds %_Z5ArrayIcE, ptr %0, i32 0, i32 1
  %load.struct17 = load %_Z5ArrayIcE, ptr %0, align 8
  %vector18 = extractvalue %_Z5ArrayIcE %load.struct17, 1
  %deref19 = load %_Z6VectorIcE, ptr %vector18, align 8
  %data = extractvalue %_Z6VectorIcE %deref19, 1
  %call20 = call ptr @memcpy(ptr %deref.recv, ptr %data, i64 %mul15)
  %load.struct21 = load %_Z5ArrayIcE, ptr %0, align 8
  %vector22 = extractvalue %_Z5ArrayIcE %load.struct21, 1
  %call23 = call ptr @_ZN4Page3getEPv(ptr %vector22)
  call void @_ZN4Page25deallocate_exclusive_pageEP4Page(ptr %call, ptr %call23)
  %vector24 = getelementptr inbounds %_Z5ArrayIcE, ptr %0, i32 0, i32 1
  store ptr %struct.region9, ptr %vector24, align 8
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  ret void
}

define linkonce_odr void @_ZN5ArrayIcE3addEc(ptr %0, i8 %1) {
entry:
  %load.struct = load %_Z5ArrayIcE, ptr %0, align 8
  %vector = extractvalue %_Z5ArrayIcE %load.struct, 1
  %eq = icmp eq ptr %vector, null
  br i1 %eq, label %lor.end, label %lor.rhs

lor.rhs:                                          ; preds = %entry
  %load.struct1 = load %_Z5ArrayIcE, ptr %0, align 8
  %length = extractvalue %_Z5ArrayIcE %load.struct1, 0
  %load.struct2 = load %_Z5ArrayIcE, ptr %0, align 8
  %vector3 = extractvalue %_Z5ArrayIcE %load.struct2, 1
  %deref = load %_Z6VectorIcE, ptr %vector3, align 8
  %length4 = extractvalue %_Z6VectorIcE %deref, 0
  %eq5 = icmp eq i64 %length, %length4
  br label %lor.end

lor.end:                                          ; preds = %lor.rhs, %entry
  %lor.result = phi i1 [ true, %entry ], [ %eq5, %lor.rhs ]
  br i1 %lor.result, label %if.then, label %if.end

if.then:                                          ; preds = %lor.end
  call void @_ZN5ArrayIcE10reallocateEv(ptr %0)
  br label %if.end

if.end:                                           ; preds = %if.then, %lor.end
  %load.struct6 = load %_Z5ArrayIcE, ptr %0, align 8
  %vector7 = extractvalue %_Z5ArrayIcE %load.struct6, 1
  %deref8 = load %_Z6VectorIcE, ptr %vector7, align 8
  %data = extractvalue %_Z6VectorIcE %deref8, 1
  %load.struct9 = load %_Z5ArrayIcE, ptr %0, align 8
  %length10 = extractvalue %_Z5ArrayIcE %load.struct9, 0
  %ptr.add = getelementptr inbounds i8, ptr %data, i64 %length10
  store i8 %1, ptr %ptr.add, align 1
  %load.struct11 = load %_Z5ArrayIcE, ptr %0, align 8
  %length12 = extractvalue %_Z5ArrayIcE %load.struct11, 0
  %add = add i64 %length12, 1
  %length13 = getelementptr inbounds %_Z5ArrayIcE, ptr %0, i32 0, i32 0
  store i64 %add, ptr %length13, align 4
  ret void
}

define linkonce_odr void @_ZN5ArrayIcE3addE6VectorIcE(ptr %0, ptr %1) {
entry:
  %own_page = alloca ptr, align 8
  %load.struct = load %_Z5ArrayIcE, ptr %0, align 8
  %length = extractvalue %_Z5ArrayIcE %load.struct, 0
  %load.struct1 = load %_Z6VectorIcE, ptr %1, align 8
  %length2 = extractvalue %_Z6VectorIcE %load.struct1, 0
  %add = add i64 %length, %length2
  %new_length = alloca i64, align 8
  store i64 %add, ptr %new_length, align 1
  %new_length3 = load i64, ptr %new_length, align 4
  %load.struct4 = load %_Z5ArrayIcE, ptr %0, align 8
  %length5 = extractvalue %_Z5ArrayIcE %load.struct4, 0
  %lt = icmp ult i64 %new_length3, %length5
  br i1 %lt, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  call void @exit(i64 14)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %load.struct6 = load %_Z5ArrayIcE, ptr %0, align 8
  %vector = extractvalue %_Z5ArrayIcE %load.struct6, 1
  %eq = icmp eq ptr %vector, null
  br i1 %eq, label %lor.end, label %lor.rhs

lor.rhs:                                          ; preds = %if.end
  %new_length7 = load i64, ptr %new_length, align 4
  %load.struct8 = load %_Z5ArrayIcE, ptr %0, align 8
  %vector9 = extractvalue %_Z5ArrayIcE %load.struct8, 1
  %deref = load %_Z6VectorIcE, ptr %vector9, align 8
  %length10 = extractvalue %_Z6VectorIcE %deref, 0
  %gt = icmp ugt i64 %new_length7, %length10
  br label %lor.end

lor.end:                                          ; preds = %lor.rhs, %if.end
  %lor.result = phi i1 [ true, %if.end ], [ %gt, %lor.rhs ]
  br i1 %lor.result, label %if.then11, label %if.end12

if.then11:                                        ; preds = %lor.end
  call void @_ZN5ArrayIcE10reallocateEv(ptr %0)
  br label %if.end12

if.end12:                                         ; preds = %if.then11, %lor.end
  %new_length13 = load i64, ptr %new_length, align 4
  %load.struct14 = load %_Z5ArrayIcE, ptr %0, align 8
  %vector15 = extractvalue %_Z5ArrayIcE %load.struct14, 1
  %deref16 = load %_Z6VectorIcE, ptr %vector15, align 8
  %length17 = extractvalue %_Z6VectorIcE %deref16, 0
  %gt18 = icmp ugt i64 %new_length13, %length17
  br i1 %gt18, label %if.then19, label %if.end20

if.then19:                                        ; preds = %if.end12
  %call = call ptr @_ZN4Page3getEPv(ptr %0)
  store ptr %call, ptr %own_page, align 1
  %own_page21 = load ptr, ptr %own_page, align 8
  %new_length22 = load i64, ptr %new_length, align 4
  %mul = mul i64 %new_length22, ptrtoint (ptr getelementptr (i8, ptr null, i32 1) to i64)
  %call23 = call ptr @_ZN4Page8allocateEmm(ptr %own_page21, i64 %mul, i64 1)
  %load.struct24 = load %_Z5ArrayIcE, ptr %0, align 8
  %length25 = extractvalue %_Z5ArrayIcE %load.struct24, 0
  %mul26 = mul i64 %length25, ptrtoint (ptr getelementptr (i8, ptr null, i32 1) to i64)
  %load.struct27 = load %_Z5ArrayIcE, ptr %0, align 8
  %length28 = extractvalue %_Z5ArrayIcE %load.struct27, 0
  %gt29 = icmp ugt i64 %length28, 0
  br i1 %gt29, label %if.then30, label %if.end31

if.end20:                                         ; preds = %if.end31, %if.end12
  %load.struct48 = load %_Z6VectorIcE, ptr %1, align 8
  %length49 = extractvalue %_Z6VectorIcE %load.struct48, 0
  %gt50 = icmp ugt i64 %length49, 0
  br i1 %gt50, label %if.then51, label %if.end52

if.then30:                                        ; preds = %if.then19
  %field.inplace = getelementptr inbounds %_Z5ArrayIcE, ptr %0, i32 0, i32 1
  %load.struct32 = load %_Z5ArrayIcE, ptr %0, align 8
  %vector33 = extractvalue %_Z5ArrayIcE %load.struct32, 1
  %deref34 = load %_Z6VectorIcE, ptr %vector33, align 8
  %data = extractvalue %_Z6VectorIcE %deref34, 1
  %call35 = call ptr @memcpy(ptr %call23, ptr %data, i64 %mul26)
  br label %if.end31

if.end31:                                         ; preds = %if.then30, %if.then19
  %load.struct36 = load %_Z5ArrayIcE, ptr %0, align 8
  %vector37 = extractvalue %_Z5ArrayIcE %load.struct36, 1
  %deref38 = load %_Z6VectorIcE, ptr %vector37, align 8
  %data39 = extractvalue %_Z6VectorIcE %deref38, 1
  %call40 = call ptr @_ZN4Page3getEPv(ptr %data39)
  %own_page41 = load ptr, ptr %own_page, align 8
  call void @_ZN4Page25deallocate_exclusive_pageEP4Page(ptr %own_page41, ptr %call40)
  %vector42 = getelementptr inbounds %_Z5ArrayIcE, ptr %0, i32 0, i32 1
  %field.deref = load ptr, ptr %vector42, align 8
  %data43 = getelementptr inbounds %_Z6VectorIcE, ptr %field.deref, i32 0, i32 1
  store ptr %call23, ptr %data43, align 8
  %new_length44 = load i64, ptr %new_length, align 4
  %vector45 = getelementptr inbounds %_Z5ArrayIcE, ptr %0, i32 0, i32 1
  %field.deref46 = load ptr, ptr %vector45, align 8
  %length47 = getelementptr inbounds %_Z6VectorIcE, ptr %field.deref46, i32 0, i32 0
  store i64 %new_length44, ptr %length47, align 4
  br label %if.end20

if.then51:                                        ; preds = %if.end20
  %load.struct53 = load %_Z5ArrayIcE, ptr %0, align 8
  %vector54 = extractvalue %_Z5ArrayIcE %load.struct53, 1
  %deref55 = load %_Z6VectorIcE, ptr %vector54, align 8
  %data56 = extractvalue %_Z6VectorIcE %deref55, 1
  %load.struct57 = load %_Z5ArrayIcE, ptr %0, align 8
  %length58 = extractvalue %_Z5ArrayIcE %load.struct57, 0
  %mul59 = mul i64 %length58, ptrtoint (ptr getelementptr (i8, ptr null, i32 1) to i64)
  %ptr.add = getelementptr inbounds i8, ptr %data56, i64 %mul59
  %field.inplace60 = getelementptr inbounds %_Z6VectorIcE, ptr %1, i32 0, i32 1
  %deref.recv = load ptr, ptr %field.inplace60, align 8
  %load.struct61 = load %_Z6VectorIcE, ptr %1, align 8
  %length62 = extractvalue %_Z6VectorIcE %load.struct61, 0
  %mul63 = mul i64 %length62, ptrtoint (ptr getelementptr (i8, ptr null, i32 1) to i64)
  %call64 = call ptr @memcpy(ptr %ptr.add, ptr %deref.recv, i64 %mul63)
  br label %if.end52

if.end52:                                         ; preds = %if.then51, %if.end20
  %load.struct65 = load %_Z5ArrayIcE, ptr %0, align 8
  %length66 = extractvalue %_Z5ArrayIcE %load.struct65, 0
  %load.struct67 = load %_Z6VectorIcE, ptr %1, align 8
  %length68 = extractvalue %_Z6VectorIcE %load.struct67, 0
  %add69 = add i64 %length66, %length68
  %length70 = getelementptr inbounds %_Z5ArrayIcE, ptr %0, i32 0, i32 0
  store i64 %add69, ptr %length70, align 4
  ret void
}

define linkonce_odr ptr @_ZN5ArrayIcE3getEPN4scaly6memory4PageEm(ptr %0, ptr %1, i64 %2) {
entry:
  %load.struct = load %_Z5ArrayIcE, ptr %1, align 8
  %length = extractvalue %_Z5ArrayIcE %load.struct, 0
  %ge = icmp uge i64 %2, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %load.struct1 = load %_Z5ArrayIcE, ptr %1, align 8
  %vector = extractvalue %_Z5ArrayIcE %load.struct1, 1
  %deref = load %_Z6VectorIcE, ptr %vector, align 8
  %data = extractvalue %_Z6VectorIcE %deref, 1
  %ptr.add = getelementptr inbounds i8, ptr %data, i64 %2
  ret ptr %ptr.add
}

define linkonce_odr void @_ZN5ArrayIcE3putEmc(ptr %0, i64 %1, i8 %2) {
entry:
  %load.struct = load %_Z5ArrayIcE, ptr %0, align 8
  %length = extractvalue %_Z5ArrayIcE %load.struct, 0
  %ge = icmp uge i64 %1, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  call void @_Z11scaly_eputsP10const_char(ptr @.str.20)
  call void @_Z12scaly_eputnlv()
  call void @exit(i64 15)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %load.struct1 = load %_Z5ArrayIcE, ptr %0, align 8
  %vector = extractvalue %_Z5ArrayIcE %load.struct1, 1
  %deref = load %_Z6VectorIcE, ptr %vector, align 8
  %data = extractvalue %_Z6VectorIcE %deref, 1
  %ptr.add = getelementptr inbounds i8, ptr %data, i64 %1
  store i8 %2, ptr %ptr.add, align 1
  ret void
}

define linkonce_odr ptr @_ZN13ArrayIteratorIcE4nextEv(ptr %0) {
entry:
  %load.struct = load %_Z13ArrayIteratorIcE, ptr %0, align 8
  %array = extractvalue %_Z13ArrayIteratorIcE %load.struct, 0
  %eq = icmp eq ptr %array, null
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %load.struct1 = load %_Z13ArrayIteratorIcE, ptr %0, align 8
  %position = extractvalue %_Z13ArrayIteratorIcE %load.struct1, 1
  %load.struct2 = load %_Z13ArrayIteratorIcE, ptr %0, align 8
  %array3 = extractvalue %_Z13ArrayIteratorIcE %load.struct2, 0
  %deref = load %_Z5ArrayIcE, ptr %array3, align 8
  %length = extractvalue %_Z5ArrayIcE %deref, 0
  %eq4 = icmp eq i64 %position, %length
  br i1 %eq4, label %if.then5, label %if.end6

if.then5:                                         ; preds = %if.end
  ret ptr null

if.end6:                                          ; preds = %if.end
  %load.struct7 = load %_Z13ArrayIteratorIcE, ptr %0, align 8
  %position8 = extractvalue %_Z13ArrayIteratorIcE %load.struct7, 1
  %add = add i64 %position8, 1
  %position9 = getelementptr inbounds %_Z13ArrayIteratorIcE, ptr %0, i32 0, i32 1
  store i64 %add, ptr %position9, align 4
  %field.inplace = getelementptr inbounds %_Z13ArrayIteratorIcE, ptr %0, i32 0, i32 0
  %deref.recv = load ptr, ptr %field.inplace, align 8
  %call = call ptr @_ZN5ArrayIcE10get_bufferEv(ptr %deref.recv)
  %load.struct10 = load %_Z13ArrayIteratorIcE, ptr %0, align 8
  %position11 = extractvalue %_Z13ArrayIteratorIcE %load.struct10, 1
  %sub = sub i64 %position11, 1
  %ptr.add = getelementptr inbounds i8, ptr %call, i64 %sub
  ret ptr %ptr.add
}

define linkonce_odr void @_ZN13ArrayIteratorIcEC1EP5ArrayIcE(ptr %0, ptr %1) {
entry:
  %array = getelementptr inbounds %_Z13ArrayIteratorIcE, ptr %0, i32 0, i32 0
  store ptr %1, ptr %array, align 8
  %position = getelementptr inbounds %_Z13ArrayIteratorIcE, ptr %0, i32 0, i32 1
  store i64 0, ptr %position, align 4
  ret void
}

define linkonce_odr void @_ZN13ArrayIteratorIcEC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr void @_ZN5ArrayIcE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z13ArrayIteratorIcE) %0, ptr %1, ptr %2) {
entry:
  %struct.init = alloca %_Z13ArrayIteratorIcE, align 8
  call void @_ZN13ArrayIteratorIcEC1EP5ArrayIcE(ptr %struct.init, ptr %2)
  %sret.body = load %_Z13ArrayIteratorIcE, ptr %struct.init, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %struct.init, i64 ptrtoint (ptr getelementptr (%_Z13ArrayIteratorIcE, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN5ArrayIcEC1Ev(ptr %0) {
entry:
  %length = getelementptr inbounds %_Z5ArrayIcE, ptr %0, i32 0, i32 0
  store i64 0, ptr %length, align 4
  %vector = getelementptr inbounds %_Z5ArrayIcE, ptr %0, i32 0, i32 1
  store ptr null, ptr %vector, align 8
  ret void
}

define linkonce_odr void @_ZN5ArrayIcEC1Em(ptr %0, i64 %1) {
entry:
  %length = getelementptr inbounds %_Z5ArrayIcE, ptr %0, i32 0, i32 0
  store i64 0, ptr %length, align 4
  %vector = getelementptr inbounds %_Z5ArrayIcE, ptr %0, i32 0, i32 1
  store ptr null, ptr %vector, align 8
  %call = call ptr @_ZN4Page3getEPv(ptr %0)
  %call1 = call ptr @_ZN4Page23allocate_exclusive_pageEv(ptr %call)
  %struct.region = call ptr @_ZN4Page8allocateEmm(ptr %call1, i64 ptrtoint (ptr getelementptr (%_Z6VectorIcE, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr ({ i1, %_Z6VectorIcE }, ptr null, i64 0, i32 1) to i64))
  call void @_ZN6VectorIcEC1EPN4scaly6memory4PageEm(ptr %struct.region, ptr %call1, i64 %1)
  %vector2 = getelementptr inbounds %_Z5ArrayIcE, ptr %0, i32 0, i32 1
  store ptr %struct.region, ptr %vector2, align 8
  ret void
}

define linkonce_odr void @_ZN5ArrayIcEC1EPN4scaly6memory4PageE(ptr %0, ptr %1) {
entry:
  %length = getelementptr inbounds %_Z5ArrayIcE, ptr %0, i32 0, i32 0
  store i64 0, ptr %length, align 4
  %vector = getelementptr inbounds %_Z5ArrayIcE, ptr %0, i32 0, i32 1
  store ptr null, ptr %vector, align 8
  ret void
}

define linkonce_odr void @_ZN5ArrayIcEC1EPN4scaly6memory4PageEm(ptr %0, ptr %1, i64 %2) {
entry:
  %length = getelementptr inbounds %_Z5ArrayIcE, ptr %0, i32 0, i32 0
  store i64 0, ptr %length, align 4
  %vector = getelementptr inbounds %_Z5ArrayIcE, ptr %0, i32 0, i32 1
  store ptr null, ptr %vector, align 8
  %call = call ptr @_ZN4Page23allocate_exclusive_pageEv(ptr %1)
  %struct.region = call ptr @_ZN4Page8allocateEmm(ptr %call, i64 ptrtoint (ptr getelementptr (%_Z6VectorIcE, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr ({ i1, %_Z6VectorIcE }, ptr null, i64 0, i32 1) to i64))
  call void @_ZN6VectorIcEC1EPN4scaly6memory4PageEm(ptr %struct.region, ptr %call, i64 %2)
  %vector1 = getelementptr inbounds %_Z5ArrayIcE, ptr %0, i32 0, i32 1
  store ptr %struct.region, ptr %vector1, align 8
  ret void
}

define linkonce_odr void @_ZN6VectorIcEC1EPN4scaly6memory4PageE5ArrayIcE(ptr %0, ptr %1, ptr %2) {
entry:
  %load.struct = load %_Z5ArrayIcE, ptr %2, align 8
  %length = extractvalue %_Z5ArrayIcE %load.struct, 0
  %length1 = getelementptr inbounds %_Z6VectorIcE, ptr %0, i32 0, i32 0
  store i64 %length, ptr %length1, align 4
  %load.struct2 = load %_Z6VectorIcE, ptr %0, align 8
  %length3 = extractvalue %_Z6VectorIcE %load.struct2, 0
  %gt = icmp ugt i64 %length3, 0
  br i1 %gt, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %load.struct4 = load %_Z6VectorIcE, ptr %0, align 8
  %length5 = extractvalue %_Z6VectorIcE %load.struct4, 0
  %mul = mul i64 %length5, ptrtoint (ptr getelementptr (i8, ptr null, i32 1) to i64)
  %call = call ptr @_ZN4Page8allocateEmm(ptr %1, i64 %mul, i64 1)
  %data = getelementptr inbounds %_Z6VectorIcE, ptr %0, i32 0, i32 1
  store ptr %call, ptr %data, align 8
  %load.struct6 = load %_Z5ArrayIcE, ptr %2, align 8
  %vector = extractvalue %_Z5ArrayIcE %load.struct6, 1
  %field.inplace = getelementptr inbounds %_Z6VectorIcE, ptr %0, i32 0, i32 1
  %deref.recv = load ptr, ptr %field.inplace, align 8
  %field.inplace7 = getelementptr inbounds %_Z6VectorIcE, ptr %vector, i32 0, i32 1
  %deref.recv8 = load ptr, ptr %field.inplace7, align 8
  %load.struct9 = load %_Z6VectorIcE, ptr %0, align 8
  %length10 = extractvalue %_Z6VectorIcE %load.struct9, 0
  %mul11 = mul i64 %length10, ptrtoint (ptr getelementptr (i8, ptr null, i32 1) to i64)
  %call12 = call ptr @memcpy(ptr %deref.recv, ptr %deref.recv8, i64 %mul11)
  br label %if.end

if.else:                                          ; preds = %entry
  %data13 = getelementptr inbounds %_Z6VectorIcE, ptr %0, i32 0, i32 1
  store ptr null, ptr %data13, align 8
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  %if.value = phi ptr [ %call12, %if.then ], [ null, %if.else ]
  ret void
}

define linkonce_odr ptr @_ZN4ListIcE8get_headEPN4scaly6memory4PageE(ptr %0, ptr %1) {
entry:
  %load.struct = load %_Z4ListIcE, ptr %1, align 8
  %head = extractvalue %_Z4ListIcE %load.struct, 0
  %eq = icmp eq ptr %head, null
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %addr.gep = getelementptr inbounds %_Z4ListIcE, ptr %1, i32 0, i32 0
  %addr.gep1 = getelementptr inbounds %_Z4NodeIcE, ptr %addr.gep, i32 0, i32 0
  ret ptr %addr.gep1
}

define linkonce_odr ptr @_ZN12ListIteratorIcE4nextEv(ptr %0) {
entry:
  %old_current = alloca ptr, align 8
  %load.struct = load %_Z12ListIteratorIcE, ptr %0, align 8
  %current = extractvalue %_Z12ListIteratorIcE %load.struct, 0
  %ne = icmp ne ptr %current, null
  br i1 %ne, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %load.struct1 = load %_Z12ListIteratorIcE, ptr %0, align 8
  %current2 = extractvalue %_Z12ListIteratorIcE %load.struct1, 0
  store ptr %current2, ptr %old_current, align 1
  %load.struct3 = load %_Z12ListIteratorIcE, ptr %0, align 8
  %current4 = extractvalue %_Z12ListIteratorIcE %load.struct3, 0
  %deref = load %_Z4NodeIcE, ptr %current4, align 8
  %next = extractvalue %_Z4NodeIcE %deref, 1
  %current5 = getelementptr inbounds %_Z12ListIteratorIcE, ptr %0, i32 0, i32 0
  store ptr %next, ptr %current5, align 8
  %old_current6 = load ptr, ptr %old_current, align 8
  %addr.gep = getelementptr inbounds %_Z4NodeIcE, ptr %old_current6, i32 0, i32 0
  ret ptr %addr.gep

if.else:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; No predecessors!
  ret ptr null
}

define linkonce_odr void @_ZN12ListIteratorIcEC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr i64 @_ZN4ListIcE5countEv(ptr %0) {
entry:
  %local_page = call ptr @_Z21scaly_alloc_root_pagev()
  %sret.result = alloca %_Z12ListIteratorIcE, align 8
  call void @_ZN4ListIcE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z12ListIteratorIcE) %sret.result, ptr %local_page, ptr %0)
  %list_iterator = alloca ptr, align 8
  store ptr %sret.result, ptr %list_iterator, align 1
  %i = alloca i64, align 8
  store i64 0, ptr %i, align 1
  br label %while.cond

while.cond:                                       ; preds = %while.body, %entry
  %list_iterator1 = load ptr, ptr %list_iterator, align 8
  %call = call ptr @_ZN12ListIteratorIcE4nextEv(ptr %list_iterator1)
  %ne = icmp ne ptr %call, null
  br i1 %ne, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %i2 = load i64, ptr %i, align 4
  %add = add i64 %i2, 1
  store i64 %add, ptr %i, align 1
  br label %while.cond

while.exit:                                       ; preds = %while.cond
  %i3 = load i64, ptr %i, align 4
  call void @_Z28scaly_release_root_page_fullP4Page(ptr %local_page)
  ret i64 %i3
}

define linkonce_odr i1 @_ZN4ListIcE6removeEc(ptr %0, i8 %1) {
entry:
  %load.struct = load %_Z4ListIcE, ptr %0, align 8
  %head = extractvalue %_Z4ListIcE %load.struct, 0
  %node = alloca ptr, align 8
  store ptr %head, ptr %node, align 1
  %previous_node = alloca ptr, align 8
  store ptr null, ptr %previous_node, align 1
  br label %while.cond

while.cond:                                       ; preds = %if.end, %entry
  %node1 = load ptr, ptr %node, align 8
  %ne = icmp ne ptr %node1, null
  br i1 %ne, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %node2 = load ptr, ptr %node, align 8
  %load.struct3 = load %_Z4NodeIcE, ptr %node2, align 8
  %element = extractvalue %_Z4NodeIcE %load.struct3, 0
  %eq = icmp eq i8 %element, %1
  br i1 %eq, label %if.then, label %if.end

while.exit:                                       ; preds = %while.cond
  ret i1 false

if.then:                                          ; preds = %while.body
  %previous_node4 = load ptr, ptr %previous_node, align 8
  %ne5 = icmp ne ptr %previous_node4, null
  br i1 %ne5, label %if.then6, label %if.end7

if.end:                                           ; preds = %while.body
  %node18 = load ptr, ptr %node, align 8
  store ptr %node18, ptr %previous_node, align 1
  %node19 = load ptr, ptr %node, align 8
  %load.struct20 = load %_Z4NodeIcE, ptr %node19, align 8
  %next21 = extractvalue %_Z4NodeIcE %load.struct20, 1
  store ptr %next21, ptr %node, align 1
  br label %while.cond

if.then6:                                         ; preds = %if.then
  %node8 = load ptr, ptr %node, align 8
  %load.struct9 = load %_Z4NodeIcE, ptr %node8, align 8
  %next = extractvalue %_Z4NodeIcE %load.struct9, 1
  %ptr.load = load ptr, ptr %previous_node, align 8
  %next10 = getelementptr inbounds %_Z4NodeIcE, ptr %ptr.load, i32 0, i32 1
  store ptr %next, ptr %next10, align 8
  br label %if.end7

if.end7:                                          ; preds = %if.then6, %if.then
  %node11 = load ptr, ptr %node, align 8
  %load.struct12 = load %_Z4ListIcE, ptr %0, align 8
  %head13 = extractvalue %_Z4ListIcE %load.struct12, 0
  %eq14 = icmp eq ptr %node11, %head13
  br i1 %eq14, label %if.then15, label %if.end16

if.then15:                                        ; preds = %if.end7
  %head17 = getelementptr inbounds %_Z4ListIcE, ptr %0, i32 0, i32 0
  store ptr null, ptr %head17, align 8
  br label %if.end16

if.end16:                                         ; preds = %if.then15, %if.end7
  ret i1 true
}

define linkonce_odr void @_ZN4ListIcE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z12ListIteratorIcE) %0, ptr %1, ptr %2) {
entry:
  %load.struct = load %_Z4ListIcE, ptr %2, align 8
  %head = extractvalue %_Z4ListIcE %load.struct, 0
  %tuple = alloca %_Z12ListIteratorIcE, align 8
  %tuple.field = getelementptr inbounds %_Z12ListIteratorIcE, ptr %tuple, i32 0, i32 0
  store ptr %head, ptr %tuple.field, align 1
  %tuple.val = load %_Z12ListIteratorIcE, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z12ListIteratorIcE, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN4ListIcE3addEc(ptr %0, i8 %1) {
entry:
  %load.struct = load %_Z4ListIcE, ptr %0, align 8
  %head = extractvalue %_Z4ListIcE %load.struct, 0
  %own_page = call ptr @_Z3getPv(ptr %0)
  %tuple.region = call ptr @_ZN4Page8allocateEmm(ptr %own_page, i64 ptrtoint (ptr getelementptr (%_Z4NodeIcE, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr ({ i1, %_Z4NodeIcE }, ptr null, i64 0, i32 1) to i64))
  %tuple.field = getelementptr inbounds %_Z4NodeIcE, ptr %tuple.region, i32 0, i32 0
  store i8 %1, ptr %tuple.field, align 1
  %tuple.field1 = getelementptr inbounds %_Z4NodeIcE, ptr %tuple.region, i32 0, i32 1
  store ptr %head, ptr %tuple.field1, align 1
  %head2 = getelementptr inbounds %_Z4ListIcE, ptr %0, i32 0, i32 0
  store ptr %tuple.region, ptr %head2, align 8
  ret void
}

define linkonce_odr void @_ZN4ListIcEC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr void @_ZN6VectorIcEC1EPN4scaly6memory4PageE4ListIcE(ptr %0, ptr %1, ptr %2) {
entry:
  %i = alloca i64, align 8
  %list_iterator = alloca ptr, align 8
  %local_page = call ptr @_Z21scaly_alloc_root_pagev()
  %sret.result = alloca %_Z12ListIteratorIcE, align 8
  %call = call i64 @_ZN4ListIcE5countEv(ptr %2)
  %length = getelementptr inbounds %_Z6VectorIcE, ptr %0, i32 0, i32 0
  store i64 %call, ptr %length, align 4
  %load.struct = load %_Z6VectorIcE, ptr %0, align 8
  %length1 = extractvalue %_Z6VectorIcE %load.struct, 0
  %gt = icmp ugt i64 %length1, 0
  br i1 %gt, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %load.struct2 = load %_Z6VectorIcE, ptr %0, align 8
  %length3 = extractvalue %_Z6VectorIcE %load.struct2, 0
  %mul = mul i64 %length3, ptrtoint (ptr getelementptr (i8, ptr null, i32 1) to i64)
  %call4 = call ptr @_ZN4Page8allocateEmm(ptr %1, i64 %mul, i64 1)
  %data = getelementptr inbounds %_Z6VectorIcE, ptr %0, i32 0, i32 1
  store ptr %call4, ptr %data, align 8
  call void @_ZN4ListIcE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z12ListIteratorIcE) %sret.result, ptr %local_page, ptr %2)
  store ptr %sret.result, ptr %list_iterator, align 1
  %load.struct5 = load %_Z6VectorIcE, ptr %0, align 8
  %length6 = extractvalue %_Z6VectorIcE %load.struct5, 0
  store i64 %length6, ptr %i, align 1
  br label %while.cond

if.else:                                          ; preds = %entry
  %data13 = getelementptr inbounds %_Z6VectorIcE, ptr %0, i32 0, i32 1
  store ptr null, ptr %data13, align 8
  br label %if.end

if.end:                                           ; preds = %if.else, %while.exit
  call void @_Z28scaly_release_root_page_fullP4Page(ptr %local_page)
  ret void

while.cond:                                       ; preds = %while.body, %if.then
  %list_iterator7 = load ptr, ptr %list_iterator, align 8
  %call8 = call ptr @_ZN12ListIteratorIcE4nextEv(ptr %list_iterator7)
  %while.tobool = icmp ne ptr %call8, null
  br i1 %while.tobool, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %i9 = load i64, ptr %i, align 4
  %sub = sub i64 %i9, 1
  store i64 %sub, ptr %i, align 1
  %deref = load i8, ptr %call8, align 1
  %load.struct10 = load %_Z6VectorIcE, ptr %0, align 8
  %data11 = extractvalue %_Z6VectorIcE %load.struct10, 1
  %i12 = load i64, ptr %i, align 4
  %ptr.add = getelementptr inbounds i8, ptr %data11, i64 %i12
  store i8 %deref, ptr %ptr.add, align 1
  br label %while.cond

while.exit:                                       ; preds = %while.cond
  br label %if.end
}

define linkonce_odr i1 @_ZN6String6equalsE6VectorIcE(ptr %0, ptr %1) {
entry:
  %byte = alloca i8, align 1
  %length = alloca i64, align 8
  store i64 0, ptr %length, align 1
  %bit_count = alloca i64, align 8
  store i64 0, ptr %bit_count, align 1
  %index = alloca i64, align 8
  store i64 0, ptr %index, align 1
  br label %while.cond

while.cond:                                       ; preds = %if.end10, %entry
  br i1 true, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %bit_count1 = load i64, ptr %bit_count, align 4
  %eq = icmp eq i64 %bit_count1, 63
  br i1 %eq, label %if.then, label %if.end

while.exit:                                       ; preds = %if.then9, %while.cond
  %length14 = load i64, ptr %length, align 4
  %load.struct15 = load %_Z6VectorIcE, ptr %1, align 8
  %length16 = extractvalue %_Z6VectorIcE %load.struct15, 0
  %ne = icmp ne i64 %length14, %length16
  br i1 %ne, label %if.then17, label %if.end18

if.then:                                          ; preds = %while.body
  call void @exit(i64 11)
  br label %if.end

if.end:                                           ; preds = %if.then, %while.body
  %load.struct = load %_Z6String, ptr %0, align 8
  %data = extractvalue %_Z6String %load.struct, 0
  %index2 = load i64, ptr %index, align 4
  %ptr.add = getelementptr inbounds i8, ptr %data, i64 %index2
  %deref = load i8, ptr %ptr.add, align 1
  store i8 %deref, ptr %byte, align 1
  %length3 = load i64, ptr %length, align 4
  %byte4 = load i8, ptr %byte, align 1
  %and = and i8 %byte4, 127
  %as.zext = zext i8 %and to i64
  %bit_count5 = load i64, ptr %bit_count, align 4
  %shl = shl i64 %as.zext, %bit_count5
  %or = or i64 %length3, %shl
  store i64 %or, ptr %length, align 1
  %byte6 = load i8, ptr %byte, align 1
  %and7 = and i8 %byte6, -128
  %eq8 = icmp eq i8 %and7, 0
  br i1 %eq8, label %if.then9, label %if.end10

if.then9:                                         ; preds = %if.end
  br label %while.exit

if.end10:                                         ; preds = %if.end
  %bit_count11 = load i64, ptr %bit_count, align 4
  %add = add i64 %bit_count11, 7
  store i64 %add, ptr %bit_count, align 1
  %index12 = load i64, ptr %index, align 4
  %add13 = add i64 %index12, 1
  store i64 %add13, ptr %index, align 1
  br label %while.cond

if.then17:                                        ; preds = %while.exit
  ret i1 false

if.end18:                                         ; preds = %while.exit
  %load.struct19 = load %_Z6String, ptr %0, align 8
  %data20 = extractvalue %_Z6String %load.struct19, 0
  %index21 = load i64, ptr %index, align 4
  %ptr.add22 = getelementptr inbounds i8, ptr %data20, i64 %index21
  %ptr.add23 = getelementptr inbounds i8, ptr %ptr.add22, i64 1
  %field.inplace = getelementptr inbounds %_Z6VectorIcE, ptr %1, i32 0, i32 1
  %deref.recv = load ptr, ptr %field.inplace, align 8
  %length24 = load i64, ptr %length, align 4
  %call = call i32 @memcmp(ptr %ptr.add23, ptr %deref.recv, i64 %length24)
  %zext = zext i32 %call to i64
  %eq25 = icmp eq i64 %zext, 0
  ret i1 %eq25
}

define linkonce_odr void @_ZN6String12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z14StringIterator) %0, ptr %1, ptr %2) {
entry:
  %struct.init = alloca %_Z14StringIterator, align 8
  call void @_ZN14StringIteratorC1E6String(ptr %struct.init, ptr %2)
  %sret.body = load %_Z14StringIterator, ptr %struct.init, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %struct.init, i64 ptrtoint (ptr getelementptr (%_Z14StringIterator, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr i1 @_ZN6String11starts_withEP10const_char(ptr %0, ptr %1) {
entry:
  %call = call i64 @_ZN6String10get_lengthEv(ptr %0)
  %call1 = call i64 @strlen(ptr %1)
  %gt = icmp ugt i64 %call1, %call
  br i1 %gt, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret i1 false

if.end:                                           ; preds = %entry
  %eq = icmp eq i64 %call1, 0
  br i1 %eq, label %if.then2, label %if.end3

if.then2:                                         ; preds = %if.end
  ret i1 true

if.end3:                                          ; preds = %if.end
  %call4 = call ptr @_ZN6String10get_bufferEv(ptr %0)
  %call5 = call i32 @memcmp(ptr %call4, ptr %1, i64 %call1)
  %zext = zext i32 %call5 to i64
  %eq6 = icmp eq i64 %zext, 0
  ret i1 %eq6
}

define linkonce_odr i1 @_ZN6String9ends_withEP10const_char(ptr %0, ptr %1) {
entry:
  %call = call i64 @_ZN6String10get_lengthEv(ptr %0)
  %call1 = call i64 @strlen(ptr %1)
  %gt = icmp ugt i64 %call1, %call
  br i1 %gt, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret i1 false

if.end:                                           ; preds = %entry
  %eq = icmp eq i64 %call1, 0
  br i1 %eq, label %if.then2, label %if.end3

if.then2:                                         ; preds = %if.end
  ret i1 true

if.end3:                                          ; preds = %if.end
  %call4 = call ptr @_ZN6String10get_bufferEv(ptr %0)
  %ptr.add = getelementptr inbounds i8, ptr %call4, i64 %call
  %neg = sub i64 0, %call1
  %ptr.sub = getelementptr inbounds i8, ptr %ptr.add, i64 %neg
  %call5 = call i32 @memcmp(ptr %ptr.sub, ptr %1, i64 %call1)
  %zext = zext i32 %call5 to i64
  %eq6 = icmp eq i64 %zext, 0
  ret i1 %eq6
}

define linkonce_odr i64 @_ZN6String13last_index_ofE2u8(ptr %0, i8 %1) {
entry:
  %i = alloca i64, align 8
  %call = call ptr @_ZN6String10get_bufferEv(ptr %0)
  %eq = icmp eq ptr %call, null
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret i64 -1

if.end:                                           ; preds = %entry
  %call1 = call i64 @_ZN6String10get_lengthEv(ptr %0)
  %eq2 = icmp eq i64 %call1, 0
  br i1 %eq2, label %if.then3, label %if.end4

if.then3:                                         ; preds = %if.end
  ret i64 -1

if.end4:                                          ; preds = %if.end
  %sub = sub i64 %call1, 1
  store i64 %sub, ptr %i, align 1
  br label %while.cond

while.cond:                                       ; preds = %if.end14, %if.end4
  %i5 = load i64, ptr %i, align 4
  %ge = icmp uge i64 %i5, 0
  br i1 %ge, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %i6 = load i64, ptr %i, align 4
  %ptr.add = getelementptr inbounds i8, ptr %call, i64 %i6
  %deref = load i8, ptr %ptr.add, align 1
  %eq7 = icmp eq i8 %deref, %1
  br i1 %eq7, label %if.then8, label %if.end9

while.exit:                                       ; preds = %if.then13, %while.cond
  ret i64 -1

if.then8:                                         ; preds = %while.body
  %i10 = load i64, ptr %i, align 4
  ret i64 %i10

if.end9:                                          ; preds = %while.body
  %i11 = load i64, ptr %i, align 4
  %eq12 = icmp eq i64 %i11, 0
  br i1 %eq12, label %if.then13, label %if.end14

if.then13:                                        ; preds = %if.end9
  br label %while.exit

if.end14:                                         ; preds = %if.end9
  %i15 = load i64, ptr %i, align 4
  %sub16 = sub i64 %i15, 1
  store i64 %sub16, ptr %i, align 1
  br label %while.cond
}

define linkonce_odr void @_ZN6StringC1Ev(ptr %0) {
entry:
  %data = getelementptr inbounds %_Z6String, ptr %0, i32 0, i32 0
  store ptr null, ptr %data, align 8
  ret void
}

define linkonce_odr void @_ZN6StringC1EPN4scaly6memory4PageEm(ptr %0, ptr %1, i64 %2) {
entry:
  %wi = alloca i64, align 8
  %wrest = alloca i64, align 8
  %rest = alloca i64, align 8
  store i64 %2, ptr %rest, align 1
  %counter = alloca i64, align 8
  store i64 0, ptr %counter, align 1
  br label %while.cond

while.cond:                                       ; preds = %while.body, %entry
  %rest1 = load i64, ptr %rest, align 4
  %ge = icmp uge i64 %rest1, 128
  br i1 %ge, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %rest2 = load i64, ptr %rest, align 4
  %lshr = lshr i64 %rest2, 7
  store i64 %lshr, ptr %rest, align 1
  %counter3 = load i64, ptr %counter, align 4
  %add = add i64 %counter3, 1
  store i64 %add, ptr %counter, align 1
  br label %while.cond

while.exit:                                       ; preds = %while.cond
  %counter4 = load i64, ptr %counter, align 4
  %add5 = add i64 %counter4, 1
  %add6 = add i64 %add5, %2
  %add7 = add i64 %add6, 1
  %call = call ptr @_ZN4Page8allocateEmm(ptr %1, i64 %add7, i64 1)
  %data = getelementptr inbounds %_Z6String, ptr %0, i32 0, i32 0
  store ptr %call, ptr %data, align 8
  store i64 %2, ptr %wrest, align 1
  store i64 0, ptr %wi, align 1
  br label %while.cond8

while.cond8:                                      ; preds = %while.body9, %while.exit
  %wrest11 = load i64, ptr %wrest, align 4
  %ge12 = icmp uge i64 %wrest11, 128
  br i1 %ge12, label %while.body9, label %while.exit10

while.body9:                                      ; preds = %while.cond8
  %wrest13 = load i64, ptr %wrest, align 4
  %as.trunc = trunc i64 %wrest13 to i8
  %or = or i8 %as.trunc, -128
  %load.struct = load %_Z6String, ptr %0, align 8
  %data14 = extractvalue %_Z6String %load.struct, 0
  %wi15 = load i64, ptr %wi, align 4
  %ptr.add = getelementptr inbounds i8, ptr %data14, i64 %wi15
  store i8 %or, ptr %ptr.add, align 1
  %wrest16 = load i64, ptr %wrest, align 4
  %lshr17 = lshr i64 %wrest16, 7
  store i64 %lshr17, ptr %wrest, align 1
  %wi18 = load i64, ptr %wi, align 4
  %add19 = add i64 %wi18, 1
  store i64 %add19, ptr %wi, align 1
  br label %while.cond8

while.exit10:                                     ; preds = %while.cond8
  %wrest20 = load i64, ptr %wrest, align 4
  %as.trunc21 = trunc i64 %wrest20 to i8
  %load.struct22 = load %_Z6String, ptr %0, align 8
  %data23 = extractvalue %_Z6String %load.struct22, 0
  %wi24 = load i64, ptr %wi, align 4
  %ptr.add25 = getelementptr inbounds i8, ptr %data23, i64 %wi24
  store i8 %as.trunc21, ptr %ptr.add25, align 1
  %load.struct26 = load %_Z6String, ptr %0, align 8
  %data27 = extractvalue %_Z6String %load.struct26, 0
  %ptr.add28 = getelementptr inbounds i8, ptr %data27, i64 %add6
  store i8 0, ptr %ptr.add28, align 1
  ret void
}

define linkonce_odr void @_ZN6StringC1EPN4scaly6memory4PageEP10const_charm(ptr %0, ptr %1, ptr %2, i64 %3) {
entry:
  %wi = alloca i64, align 8
  %wrest = alloca i64, align 8
  %counter = alloca i64, align 8
  %rest = alloca i64, align 8
  %eq = icmp eq i64 %3, 0
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %data = getelementptr inbounds %_Z6String, ptr %0, i32 0, i32 0
  store ptr null, ptr %data, align 8
  ret void

if.end:                                           ; preds = %entry
  store i64 %3, ptr %rest, align 1
  store i64 0, ptr %counter, align 1
  br label %while.cond

while.cond:                                       ; preds = %while.body, %if.end
  %rest1 = load i64, ptr %rest, align 4
  %ge = icmp uge i64 %rest1, 128
  br i1 %ge, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %rest2 = load i64, ptr %rest, align 4
  %lshr = lshr i64 %rest2, 7
  store i64 %lshr, ptr %rest, align 1
  %counter3 = load i64, ptr %counter, align 4
  %add = add i64 %counter3, 1
  store i64 %add, ptr %counter, align 1
  br label %while.cond

while.exit:                                       ; preds = %while.cond
  %counter4 = load i64, ptr %counter, align 4
  %add5 = add i64 %counter4, 1
  %add6 = add i64 %add5, %3
  %add7 = add i64 %add6, 1
  %call = call ptr @_ZN4Page8allocateEmm(ptr %1, i64 %add7, i64 1)
  %data8 = getelementptr inbounds %_Z6String, ptr %0, i32 0, i32 0
  store ptr %call, ptr %data8, align 8
  store i64 %3, ptr %wrest, align 1
  store i64 0, ptr %wi, align 1
  br label %while.cond9

while.cond9:                                      ; preds = %while.body10, %while.exit
  %wrest12 = load i64, ptr %wrest, align 4
  %ge13 = icmp uge i64 %wrest12, 128
  br i1 %ge13, label %while.body10, label %while.exit11

while.body10:                                     ; preds = %while.cond9
  %wrest14 = load i64, ptr %wrest, align 4
  %as.trunc = trunc i64 %wrest14 to i8
  %or = or i8 %as.trunc, -128
  %load.struct = load %_Z6String, ptr %0, align 8
  %data15 = extractvalue %_Z6String %load.struct, 0
  %wi16 = load i64, ptr %wi, align 4
  %ptr.add = getelementptr inbounds i8, ptr %data15, i64 %wi16
  store i8 %or, ptr %ptr.add, align 1
  %wrest17 = load i64, ptr %wrest, align 4
  %lshr18 = lshr i64 %wrest17, 7
  store i64 %lshr18, ptr %wrest, align 1
  %wi19 = load i64, ptr %wi, align 4
  %add20 = add i64 %wi19, 1
  store i64 %add20, ptr %wi, align 1
  br label %while.cond9

while.exit11:                                     ; preds = %while.cond9
  %wrest21 = load i64, ptr %wrest, align 4
  %as.trunc22 = trunc i64 %wrest21 to i8
  %load.struct23 = load %_Z6String, ptr %0, align 8
  %data24 = extractvalue %_Z6String %load.struct23, 0
  %wi25 = load i64, ptr %wi, align 4
  %ptr.add26 = getelementptr inbounds i8, ptr %data24, i64 %wi25
  store i8 %as.trunc22, ptr %ptr.add26, align 1
  %load.struct27 = load %_Z6String, ptr %0, align 8
  %data28 = extractvalue %_Z6String %load.struct27, 0
  %counter29 = load i64, ptr %counter, align 4
  %ptr.add30 = getelementptr inbounds i8, ptr %data28, i64 %counter29
  %ptr.add31 = getelementptr inbounds i8, ptr %ptr.add30, i64 1
  %call32 = call ptr @memcpy(ptr %ptr.add31, ptr %2, i64 %3)
  %load.struct33 = load %_Z6String, ptr %0, align 8
  %data34 = extractvalue %_Z6String %load.struct33, 0
  %ptr.add35 = getelementptr inbounds i8, ptr %data34, i64 %add6
  store i8 0, ptr %ptr.add35, align 1
  ret void
}

define linkonce_odr void @_ZN6StringC1EPN4scaly6memory4PageE6VectorIcE(ptr %0, ptr %1, ptr %2) {
entry:
  %wi = alloca i64, align 8
  %wrest = alloca i64, align 8
  %load.struct = load %_Z6VectorIcE, ptr %2, align 8
  %length = extractvalue %_Z6VectorIcE %load.struct, 0
  %rest = alloca i64, align 8
  store i64 %length, ptr %rest, align 1
  %counter = alloca i64, align 8
  store i64 0, ptr %counter, align 1
  br label %while.cond

while.cond:                                       ; preds = %while.body, %entry
  %rest1 = load i64, ptr %rest, align 4
  %ge = icmp uge i64 %rest1, 128
  br i1 %ge, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %rest2 = load i64, ptr %rest, align 4
  %lshr = lshr i64 %rest2, 7
  store i64 %lshr, ptr %rest, align 1
  %counter3 = load i64, ptr %counter, align 4
  %add = add i64 %counter3, 1
  store i64 %add, ptr %counter, align 1
  br label %while.cond

while.exit:                                       ; preds = %while.cond
  %counter4 = load i64, ptr %counter, align 4
  %add5 = add i64 %counter4, 1
  %load.struct6 = load %_Z6VectorIcE, ptr %2, align 8
  %length7 = extractvalue %_Z6VectorIcE %load.struct6, 0
  %add8 = add i64 %add5, %length7
  %add9 = add i64 %add8, 1
  %call = call ptr @_ZN4Page8allocateEmm(ptr %1, i64 %add9, i64 1)
  %data = getelementptr inbounds %_Z6String, ptr %0, i32 0, i32 0
  store ptr %call, ptr %data, align 8
  %load.struct10 = load %_Z6VectorIcE, ptr %2, align 8
  %length11 = extractvalue %_Z6VectorIcE %load.struct10, 0
  store i64 %length11, ptr %wrest, align 1
  store i64 0, ptr %wi, align 1
  br label %while.cond12

while.cond12:                                     ; preds = %while.body13, %while.exit
  %wrest15 = load i64, ptr %wrest, align 4
  %ge16 = icmp uge i64 %wrest15, 128
  br i1 %ge16, label %while.body13, label %while.exit14

while.body13:                                     ; preds = %while.cond12
  %wrest17 = load i64, ptr %wrest, align 4
  %as.trunc = trunc i64 %wrest17 to i8
  %or = or i8 %as.trunc, -128
  %load.struct18 = load %_Z6String, ptr %0, align 8
  %data19 = extractvalue %_Z6String %load.struct18, 0
  %wi20 = load i64, ptr %wi, align 4
  %ptr.add = getelementptr inbounds i8, ptr %data19, i64 %wi20
  store i8 %or, ptr %ptr.add, align 1
  %wrest21 = load i64, ptr %wrest, align 4
  %lshr22 = lshr i64 %wrest21, 7
  store i64 %lshr22, ptr %wrest, align 1
  %wi23 = load i64, ptr %wi, align 4
  %add24 = add i64 %wi23, 1
  store i64 %add24, ptr %wi, align 1
  br label %while.cond12

while.exit14:                                     ; preds = %while.cond12
  %wrest25 = load i64, ptr %wrest, align 4
  %as.trunc26 = trunc i64 %wrest25 to i8
  %load.struct27 = load %_Z6String, ptr %0, align 8
  %data28 = extractvalue %_Z6String %load.struct27, 0
  %wi29 = load i64, ptr %wi, align 4
  %ptr.add30 = getelementptr inbounds i8, ptr %data28, i64 %wi29
  store i8 %as.trunc26, ptr %ptr.add30, align 1
  %load.struct31 = load %_Z6VectorIcE, ptr %2, align 8
  %length32 = extractvalue %_Z6VectorIcE %load.struct31, 0
  %gt = icmp ugt i64 %length32, 0
  br i1 %gt, label %if.then, label %if.end

if.then:                                          ; preds = %while.exit14
  %load.struct33 = load %_Z6String, ptr %0, align 8
  %data34 = extractvalue %_Z6String %load.struct33, 0
  %counter35 = load i64, ptr %counter, align 4
  %ptr.add36 = getelementptr inbounds i8, ptr %data34, i64 %counter35
  %ptr.add37 = getelementptr inbounds i8, ptr %ptr.add36, i64 1
  %field.inplace = getelementptr inbounds %_Z6VectorIcE, ptr %2, i32 0, i32 1
  %deref.recv = load ptr, ptr %field.inplace, align 8
  %field.inplace38 = getelementptr inbounds %_Z6VectorIcE, ptr %2, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace38, align 4
  %call39 = call ptr @memcpy(ptr %ptr.add37, ptr %deref.recv, i64 %field.val)
  br label %if.end

if.end:                                           ; preds = %if.then, %while.exit14
  %load.struct40 = load %_Z6String, ptr %0, align 8
  %data41 = extractvalue %_Z6String %load.struct40, 0
  %ptr.add42 = getelementptr inbounds i8, ptr %data41, i64 %add8
  store i8 0, ptr %ptr.add42, align 1
  ret void
}

define linkonce_odr void @_ZN6StringC1EPN4scaly6memory4PageEP10const_char(ptr %0, ptr %1, ptr %2) {
entry:
  %wi = alloca i64, align 8
  %wrest = alloca i64, align 8
  %counter = alloca i64, align 8
  %rest = alloca i64, align 8
  %call = call i64 @strlen(ptr %2)
  %eq = icmp eq i64 %call, 0
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %data = getelementptr inbounds %_Z6String, ptr %0, i32 0, i32 0
  store ptr null, ptr %data, align 8
  ret void

if.end:                                           ; preds = %entry
  store i64 %call, ptr %rest, align 1
  store i64 0, ptr %counter, align 1
  br label %while.cond

while.cond:                                       ; preds = %while.body, %if.end
  %rest1 = load i64, ptr %rest, align 4
  %ge = icmp uge i64 %rest1, 128
  br i1 %ge, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %rest2 = load i64, ptr %rest, align 4
  %lshr = lshr i64 %rest2, 7
  store i64 %lshr, ptr %rest, align 1
  %counter3 = load i64, ptr %counter, align 4
  %add = add i64 %counter3, 1
  store i64 %add, ptr %counter, align 1
  br label %while.cond

while.exit:                                       ; preds = %while.cond
  %counter4 = load i64, ptr %counter, align 4
  %add5 = add i64 %counter4, 1
  %add6 = add i64 %add5, %call
  %add7 = add i64 %add6, 1
  %call8 = call ptr @_ZN4Page8allocateEmm(ptr %1, i64 %add7, i64 1)
  %data9 = getelementptr inbounds %_Z6String, ptr %0, i32 0, i32 0
  store ptr %call8, ptr %data9, align 8
  store i64 %call, ptr %wrest, align 1
  store i64 0, ptr %wi, align 1
  br label %while.cond10

while.cond10:                                     ; preds = %while.body11, %while.exit
  %wrest13 = load i64, ptr %wrest, align 4
  %ge14 = icmp uge i64 %wrest13, 128
  br i1 %ge14, label %while.body11, label %while.exit12

while.body11:                                     ; preds = %while.cond10
  %wrest15 = load i64, ptr %wrest, align 4
  %as.trunc = trunc i64 %wrest15 to i8
  %or = or i8 %as.trunc, -128
  %load.struct = load %_Z6String, ptr %0, align 8
  %data16 = extractvalue %_Z6String %load.struct, 0
  %wi17 = load i64, ptr %wi, align 4
  %ptr.add = getelementptr inbounds i8, ptr %data16, i64 %wi17
  store i8 %or, ptr %ptr.add, align 1
  %wrest18 = load i64, ptr %wrest, align 4
  %lshr19 = lshr i64 %wrest18, 7
  store i64 %lshr19, ptr %wrest, align 1
  %wi20 = load i64, ptr %wi, align 4
  %add21 = add i64 %wi20, 1
  store i64 %add21, ptr %wi, align 1
  br label %while.cond10

while.exit12:                                     ; preds = %while.cond10
  %wrest22 = load i64, ptr %wrest, align 4
  %as.trunc23 = trunc i64 %wrest22 to i8
  %load.struct24 = load %_Z6String, ptr %0, align 8
  %data25 = extractvalue %_Z6String %load.struct24, 0
  %wi26 = load i64, ptr %wi, align 4
  %ptr.add27 = getelementptr inbounds i8, ptr %data25, i64 %wi26
  store i8 %as.trunc23, ptr %ptr.add27, align 1
  %load.struct28 = load %_Z6String, ptr %0, align 8
  %data29 = extractvalue %_Z6String %load.struct28, 0
  %counter30 = load i64, ptr %counter, align 4
  %ptr.add31 = getelementptr inbounds i8, ptr %data29, i64 %counter30
  %ptr.add32 = getelementptr inbounds i8, ptr %ptr.add31, i64 1
  %call33 = call ptr @memcpy(ptr %ptr.add32, ptr %2, i64 %call)
  %load.struct34 = load %_Z6String, ptr %0, align 8
  %data35 = extractvalue %_Z6String %load.struct34, 0
  %ptr.add36 = getelementptr inbounds i8, ptr %data35, i64 %add6
  store i8 0, ptr %ptr.add36, align 1
  ret void
}

define linkonce_odr void @_ZN6StringC1EPN4scaly6memory4PageE6String(ptr %0, ptr %1, ptr %2) {
entry:
  %overall_length = alloca i64, align 8
  %index = alloca i64, align 8
  %bit_count = alloca i64, align 8
  %length = alloca i64, align 8
  %load.struct = load %_Z6String, ptr %2, align 8
  %data = extractvalue %_Z6String %load.struct, 0
  %eq = icmp eq ptr %data, null
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %data1 = getelementptr inbounds %_Z6String, ptr %0, i32 0, i32 0
  store ptr null, ptr %data1, align 8
  ret void

if.end:                                           ; preds = %entry
  store i64 0, ptr %length, align 1
  store i64 0, ptr %bit_count, align 1
  store i64 0, ptr %index, align 1
  br label %while.cond

while.cond:                                       ; preds = %if.end14, %if.end
  br i1 true, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %bit_count2 = load i64, ptr %bit_count, align 4
  %eq3 = icmp eq i64 %bit_count2, 63
  br i1 %eq3, label %if.then4, label %if.end5

while.exit:                                       ; preds = %if.then13, %while.cond
  %index18 = load i64, ptr %index, align 4
  %add19 = add i64 %index18, 1
  %length20 = load i64, ptr %length, align 4
  %add21 = add i64 %add19, %length20
  store i64 %add21, ptr %overall_length, align 1
  %overall_length22 = load i64, ptr %overall_length, align 4
  %add23 = add i64 %overall_length22, 1
  %call = call ptr @_ZN4Page8allocateEmm(ptr %1, i64 %add23, i64 1)
  %data24 = getelementptr inbounds %_Z6String, ptr %0, i32 0, i32 0
  store ptr %call, ptr %data24, align 8
  %field.inplace = getelementptr inbounds %_Z6String, ptr %0, i32 0, i32 0
  %deref.recv = load ptr, ptr %field.inplace, align 8
  %field.inplace25 = getelementptr inbounds %_Z6String, ptr %2, i32 0, i32 0
  %deref.recv26 = load ptr, ptr %field.inplace25, align 8
  %overall_length27 = load i64, ptr %overall_length, align 4
  %call28 = call ptr @memcpy(ptr %deref.recv, ptr %deref.recv26, i64 %overall_length27)
  %load.struct29 = load %_Z6String, ptr %0, align 8
  %data30 = extractvalue %_Z6String %load.struct29, 0
  %overall_length31 = load i64, ptr %overall_length, align 4
  %ptr.add32 = getelementptr inbounds i8, ptr %data30, i64 %overall_length31
  store i8 0, ptr %ptr.add32, align 1
  ret void

if.then4:                                         ; preds = %while.body
  call void @exit(i64 12)
  br label %if.end5

if.end5:                                          ; preds = %if.then4, %while.body
  %load.struct6 = load %_Z6String, ptr %2, align 8
  %data7 = extractvalue %_Z6String %load.struct6, 0
  %index8 = load i64, ptr %index, align 4
  %ptr.add = getelementptr inbounds i8, ptr %data7, i64 %index8
  %deref = load i8, ptr %ptr.add, align 1
  %length9 = load i64, ptr %length, align 4
  %and = and i8 %deref, 127
  %as.zext = zext i8 %and to i64
  %bit_count10 = load i64, ptr %bit_count, align 4
  %shl = shl i64 %as.zext, %bit_count10
  %or = or i64 %length9, %shl
  store i64 %or, ptr %length, align 1
  %and11 = and i8 %deref, -128
  %eq12 = icmp eq i8 %and11, 0
  br i1 %eq12, label %if.then13, label %if.end14

if.then13:                                        ; preds = %if.end5
  br label %while.exit

if.end14:                                         ; preds = %if.end5
  %bit_count15 = load i64, ptr %bit_count, align 4
  %add = add i64 %bit_count15, 7
  store i64 %add, ptr %bit_count, align 1
  %index16 = load i64, ptr %index, align 4
  %add17 = add i64 %index16, 1
  store i64 %add17, ptr %index, align 1
  br label %while.cond
}

define linkonce_odr void @_ZN6StringC1EPN4scaly6memory4PageE2u8(ptr %0, ptr %1, i8 %2) {
entry:
  %call = call ptr @_ZN4Page8allocateEmm(ptr %1, i64 3, i64 1)
  %data = getelementptr inbounds %_Z6String, ptr %0, i32 0, i32 0
  store ptr %call, ptr %data, align 8
  %load.struct = load %_Z6String, ptr %0, align 8
  %data1 = extractvalue %_Z6String %load.struct, 0
  store i64 1, ptr %data1, align 4
  %load.struct2 = load %_Z6String, ptr %0, align 8
  %data3 = extractvalue %_Z6String %load.struct2, 0
  %ptr.add = getelementptr inbounds i8, ptr %data3, i64 1
  store i8 %2, ptr %ptr.add, align 1
  %load.struct4 = load %_Z6String, ptr %0, align 8
  %data5 = extractvalue %_Z6String %load.struct4, 0
  %ptr.add6 = getelementptr inbounds i8, ptr %data5, i64 2
  store i8 0, ptr %ptr.add6, align 1
  ret void
}

define linkonce_odr ptr @_ZN14StringIterator4nextEv(ptr %0) {
entry:
  %load.struct = load %_Z14StringIterator, ptr %0, align 8
  %current = extractvalue %_Z14StringIterator %load.struct, 0
  %load.struct1 = load %_Z14StringIterator, ptr %0, align 8
  %last = extractvalue %_Z14StringIterator %load.struct1, 1
  %eq = icmp eq ptr %current, %last
  br i1 %eq, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  ret ptr null

if.else:                                          ; preds = %entry
  %load.struct2 = load %_Z14StringIterator, ptr %0, align 8
  %current3 = extractvalue %_Z14StringIterator %load.struct2, 0
  %load.struct4 = load %_Z14StringIterator, ptr %0, align 8
  %current5 = extractvalue %_Z14StringIterator %load.struct4, 0
  %ptr.add = getelementptr inbounds i8, ptr %current5, i64 1
  %current6 = getelementptr inbounds %_Z14StringIterator, ptr %0, i32 0, i32 0
  store ptr %ptr.add, ptr %current6, align 8
  ret ptr %current3

if.end:                                           ; No predecessors!
  ret ptr null
}

define linkonce_odr void @_ZN14StringIteratorC1E6String(ptr %0, ptr %1) {
entry:
  %call = call ptr @_ZN6String10get_bufferEv(ptr %1)
  %current = getelementptr inbounds %_Z14StringIterator, ptr %0, i32 0, i32 0
  store ptr %call, ptr %current, align 8
  %call1 = call i64 @_ZN6String10get_lengthEv(ptr %1)
  %ptr.add = getelementptr inbounds i8, ptr %call, i64 %call1
  %last = getelementptr inbounds %_Z14StringIterator, ptr %0, i32 0, i32 1
  store ptr %ptr.add, ptr %last, align 8
  ret void
}

define linkonce_odr void @_ZN14StringIteratorC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr void @_ZN13StringBuilder6appendEP10const_char(ptr %0, ptr %1) {
entry:
  %struct.init = alloca %_Z6VectorIcE, align 8
  %call = call i64 @strlen(ptr %1)
  %eq = icmp eq i64 %call, 0
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret void

if.end:                                           ; preds = %entry
  %field.inplace = getelementptr inbounds %_Z13StringBuilder, ptr %0, i32 0, i32 0
  call void @_ZN6VectorIcEC1EPcm(ptr %struct.init, ptr %1, i64 %call)
  call void @_ZN5ArrayIcE3addE6VectorIcE(ptr %field.inplace, ptr %struct.init)
  ret void
}

define linkonce_odr void @_ZN13StringBuilder6appendEPcm(ptr %0, ptr %1, i64 %2) {
entry:
  %field.inplace = getelementptr inbounds %_Z13StringBuilder, ptr %0, i32 0, i32 0
  %struct.init = alloca %_Z6VectorIcE, align 8
  call void @_ZN6VectorIcEC1EPcm(ptr %struct.init, ptr %1, i64 %2)
  call void @_ZN5ArrayIcE3addE6VectorIcE(ptr %field.inplace, ptr %struct.init)
  ret void
}

define linkonce_odr void @_ZN13StringBuilderC1EPN4scaly6memory4PageE(ptr %0, ptr %1) {
entry:
  %struct.region = call ptr @_ZN4Page8allocateEmm(ptr %1, i64 ptrtoint (ptr getelementptr (%_Z5ArrayIcE, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr ({ i1, %_Z5ArrayIcE }, ptr null, i64 0, i32 1) to i64))
  %tuple.field = getelementptr inbounds %_Z5ArrayIcE, ptr %struct.region, i32 0, i32 0
  store i64 0, ptr %tuple.field, align 4
  %tuple.field1 = getelementptr inbounds %_Z5ArrayIcE, ptr %struct.region, i32 0, i32 1
  store ptr null, ptr %tuple.field1, align 8
  %buffer = getelementptr inbounds %_Z13StringBuilder, ptr %0, i32 0, i32 0
  %field.load = load %_Z5ArrayIcE, ptr %struct.region, align 8
  store %_Z5ArrayIcE %field.load, ptr %buffer, align 8
  ret void
}

define linkonce_odr void @_ZN13StringBuilderC1EPN4scaly6memory4PageEm(ptr %0, ptr %1, i64 %2) {
entry:
  %struct.region = call ptr @_ZN4Page8allocateEmm(ptr %1, i64 ptrtoint (ptr getelementptr (%_Z5ArrayIcE, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr ({ i1, %_Z5ArrayIcE }, ptr null, i64 0, i32 1) to i64))
  call void @_ZN5ArrayIcEC1Em(ptr %struct.region, i64 %2)
  %buffer = getelementptr inbounds %_Z13StringBuilder, ptr %0, i32 0, i32 0
  %field.load = load %_Z5ArrayIcE, ptr %struct.region, align 8
  store %_Z5ArrayIcE %field.load, ptr %buffer, align 8
  ret void
}

define linkonce_odr void @_ZN13StringBuilderC1EPN4scaly6memory4PageEc(ptr %0, ptr %1, i8 %2) {
entry:
  %struct.region = call ptr @_ZN4Page8allocateEmm(ptr %1, i64 ptrtoint (ptr getelementptr (%_Z5ArrayIcE, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr ({ i1, %_Z5ArrayIcE }, ptr null, i64 0, i32 1) to i64))
  %tuple.field = getelementptr inbounds %_Z5ArrayIcE, ptr %struct.region, i32 0, i32 0
  store i64 0, ptr %tuple.field, align 4
  %tuple.field1 = getelementptr inbounds %_Z5ArrayIcE, ptr %struct.region, i32 0, i32 1
  store ptr null, ptr %tuple.field1, align 8
  %buffer = getelementptr inbounds %_Z13StringBuilder, ptr %0, i32 0, i32 0
  %field.load = load %_Z5ArrayIcE, ptr %struct.region, align 8
  store %_Z5ArrayIcE %field.load, ptr %buffer, align 8
  call void @_ZN13StringBuilder6appendEc(ptr %0, i8 %2)
  ret void
}

define linkonce_odr void @_ZN13StringBuilderC1EPN4scaly6memory4PageE6String(ptr %0, ptr %1, ptr %2) {
entry:
  %struct.region = call ptr @_ZN4Page8allocateEmm(ptr %1, i64 ptrtoint (ptr getelementptr (%_Z5ArrayIcE, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr ({ i1, %_Z5ArrayIcE }, ptr null, i64 0, i32 1) to i64))
  %tuple.field = getelementptr inbounds %_Z5ArrayIcE, ptr %struct.region, i32 0, i32 0
  store i64 0, ptr %tuple.field, align 4
  %tuple.field1 = getelementptr inbounds %_Z5ArrayIcE, ptr %struct.region, i32 0, i32 1
  store ptr null, ptr %tuple.field1, align 8
  %buffer = getelementptr inbounds %_Z13StringBuilder, ptr %0, i32 0, i32 0
  %field.load = load %_Z5ArrayIcE, ptr %struct.region, align 8
  store %_Z5ArrayIcE %field.load, ptr %buffer, align 8
  call void @_ZN13StringBuilder6appendE6String(ptr %0, ptr %2)
  ret void
}

define linkonce_odr void @_ZN13StringBuilderC1EPN4scaly6memory4PageEP10const_char(ptr %0, ptr %1, ptr %2) {
entry:
  %struct.region = call ptr @_ZN4Page8allocateEmm(ptr %1, i64 ptrtoint (ptr getelementptr (%_Z5ArrayIcE, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr ({ i1, %_Z5ArrayIcE }, ptr null, i64 0, i32 1) to i64))
  %tuple.field = getelementptr inbounds %_Z5ArrayIcE, ptr %struct.region, i32 0, i32 0
  store i64 0, ptr %tuple.field, align 4
  %tuple.field1 = getelementptr inbounds %_Z5ArrayIcE, ptr %struct.region, i32 0, i32 1
  store ptr null, ptr %tuple.field1, align 8
  %buffer = getelementptr inbounds %_Z13StringBuilder, ptr %0, i32 0, i32 0
  %field.load = load %_Z5ArrayIcE, ptr %struct.region, align 8
  store %_Z5ArrayIcE %field.load, ptr %buffer, align 8
  call void @_ZN13StringBuilder6appendEP10const_char(ptr %0, ptr %2)
  ret void
}

define linkonce_odr void @_ZN13StringBuilderC1Ev(ptr %0) {
entry:
  ret void
}

define linkonce_odr i1 @_ZN6Result5is_okEv(ptr %0) {
entry:
  %tag.ptr = getelementptr inbounds %_Z6Result, ptr %0, i32 0, i32 0
  %tag = load i8, ptr %tag.ptr, align 1
  switch i8 %tag, label %choose.else [
    i8 0, label %choose.when
  ]

choose.end:                                       ; No predecessors!
  ret i1 false

choose.else:                                      ; preds = %entry
  ret i1 false

choose.when:                                      ; preds = %entry
  %"variant.c_data().ptr" = getelementptr inbounds %_Z6Result, ptr %0, i32 0, i32 1
  %variant.val = load ptr, ptr %"variant.c_data().ptr", align 8
  ret i1 true
}

define linkonce_odr i1 @_ZN6Result8is_errorEv(ptr %0) {
entry:
  %tag.ptr = getelementptr inbounds %_Z6Result, ptr %0, i32 0, i32 0
  %tag = load i8, ptr %tag.ptr, align 1
  switch i8 %tag, label %choose.else [
    i8 1, label %choose.when
  ]

choose.end:                                       ; No predecessors!
  ret i1 false

choose.else:                                      ; preds = %entry
  ret i1 false

choose.when:                                      ; preds = %entry
  %"variant.c_data().ptr" = getelementptr inbounds %_Z6Result, ptr %0, i32 0, i32 1
  %variant.val = load ptr, ptr %"variant.c_data().ptr", align 8
  ret i1 true
}

define linkonce_odr void @_ZN6Result6unwrapEv(ptr noalias sret({ ptr }) %0, ptr %1) {
entry:
  %tag.ptr = getelementptr inbounds %_Z6Result, ptr %1, i32 0, i32 0
  %tag = load i8, ptr %tag.ptr, align 1
  switch i8 %tag, label %choose.else [
    i8 0, label %choose.when
  ]

choose.end:                                       ; preds = %choose.else
  ret void

choose.else:                                      ; preds = %entry
  br label %choose.end

choose.when:                                      ; preds = %entry
  %"variant.c_data().ptr" = getelementptr inbounds %_Z6Result, ptr %1, i32 0, i32 1
  %variant.val = load ptr, ptr %"variant.c_data().ptr", align 8
  %sret.body = load { ptr }, ptr %variant.val, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %variant.val, i64 ptrtoint (ptr getelementptr ({ ptr }, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN6Result12unwrap_errorEv(ptr noalias sret({ ptr }) %0, ptr %1) {
entry:
  %tag.ptr = getelementptr inbounds %_Z6Result, ptr %1, i32 0, i32 0
  %tag = load i8, ptr %tag.ptr, align 1
  switch i8 %tag, label %choose.else [
    i8 1, label %choose.when
  ]

choose.end:                                       ; preds = %choose.else
  ret void

choose.else:                                      ; preds = %entry
  br label %choose.end

choose.when:                                      ; preds = %entry
  %"variant.c_data().ptr" = getelementptr inbounds %_Z6Result, ptr %1, i32 0, i32 1
  %variant.val = load ptr, ptr %"variant.c_data().ptr", align 8
  %sret.body = load { ptr }, ptr %variant.val, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %variant.val, i64 ptrtoint (ptr getelementptr ({ ptr }, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN6Result9unwrap_orE1T(ptr noalias sret({ ptr }) %0, ptr %1, ptr %2) {
entry:
  ret void
}

define linkonce_odr i1 @_ZN6Option7is_someEv(ptr %0) {
entry:
  %tag.ptr = getelementptr inbounds %_Z6Option, ptr %0, i32 0, i32 0
  %tag = load i8, ptr %tag.ptr, align 1
  switch i8 %tag, label %choose.else [
    i8 0, label %choose.when
  ]

choose.end:                                       ; preds = %choose.else, %choose.when
  %choose.value = phi i1 [ true, %choose.when ], [ false, %choose.else ]
  ret i1 %choose.value

choose.else:                                      ; preds = %entry
  br label %choose.end

choose.when:                                      ; preds = %entry
  %"variant.c_data().ptr" = getelementptr inbounds %_Z6Option, ptr %0, i32 0, i32 1
  %variant.val = load ptr, ptr %"variant.c_data().ptr", align 8
  br label %choose.end
}

define linkonce_odr i1 @_ZN6Option7is_noneEv(ptr %0) {
entry:
  %tag.ptr = getelementptr inbounds %_Z6Option, ptr %0, i32 0, i32 0
  %tag = load i8, ptr %tag.ptr, align 1
  switch i8 %tag, label %choose.else [
    i8 0, label %choose.when
  ]

choose.end:                                       ; preds = %choose.else, %choose.when
  %choose.value = phi i1 [ false, %choose.when ], [ true, %choose.else ]
  ret i1 %choose.value

choose.else:                                      ; preds = %entry
  br label %choose.end

choose.when:                                      ; preds = %entry
  %"variant.c_data().ptr" = getelementptr inbounds %_Z6Option, ptr %0, i32 0, i32 1
  %variant.val = load ptr, ptr %"variant.c_data().ptr", align 8
  br label %choose.end
}

declare void @_ZN6Option6unwrapEv(ptr noalias sret({ ptr }), ptr)

define linkonce_odr void @_ZN6Option9unwrap_orE1T(ptr noalias sret({ ptr }) %0, ptr %1, ptr %2) {
entry:
  ret void
}

define linkonce_odr void @_ZN7Console5printEP10const_char(ptr %0) {
entry:
  %call = call i64 @strlen(ptr %0)
  %len = alloca i64, align 8
  store i64 %call, ptr %len, align 1
  %len1 = load i64, ptr %len, align 4
  %call2 = call i64 @write(i64 1, ptr %0, i64 %len1)
  ret void
}

define linkonce_odr void @_ZN7Console7printlnEP10const_char(ptr %0) {
entry:
  call void @_ZN7Console5printEP10const_char(ptr %0)
  %nl = alloca i8, align 1
  store i8 10, ptr %nl, align 1
  %call = call i64 @write(i64 1, ptr %nl, i64 1)
  ret void
}

define linkonce_odr i1 @_ZN4File6existsE6String(ptr %0) {
entry:
  %local_page = call ptr @_Z21scaly_alloc_root_pagev()
  %call = call ptr @_ZN6String11to_c_stringEPN4scaly6memory4PageE(ptr %local_page, ptr %0)
  %call1 = call i64 @access(ptr %call, i64 0)
  %eq = icmp eq i64 %call1, 0
  call void @_Z28scaly_release_root_page_fullP4Page(ptr %local_page)
  ret i1 %eq
}

define linkonce_odr void @_ZN4File14read_to_stringEPN4scaly6memory4PageE6String(ptr noalias sret(%_Z6String) %0, ptr %1, ptr %2) {
entry:
  %ret = alloca ptr, align 8
  %struct.init = alloca %_Z6String, align 8
  %call = call ptr @_ZN6String11to_c_stringEPN4scaly6memory4PageE(ptr %1, ptr %2)
  %call1 = call ptr @fopen(ptr %call, ptr @.str.21)
  %file = alloca ptr, align 8
  store ptr %call1, ptr %file, align 1
  %file2 = load ptr, ptr %file, align 8
  %eq = icmp eq ptr %file2, null
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %tuple.field = getelementptr inbounds %_Z6String, ptr %struct.init, i32 0, i32 0
  store ptr null, ptr %tuple.field, align 8
  %sret.body = load %_Z6String, ptr %struct.init, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %struct.init, i64 ptrtoint (ptr getelementptr (%_Z6String, ptr null, i32 1) to i64), i1 false)
  ret void

if.end:                                           ; preds = %entry
  %file3 = load ptr, ptr %file, align 8
  %call4 = call i64 @fseek(ptr %file3, i64 0, i64 2)
  %file5 = load ptr, ptr %file, align 8
  %call6 = call i64 @ftell(ptr %file5)
  %file7 = load ptr, ptr %file, align 8
  call void @rewind(ptr %file7)
  %struct.region = call ptr @_ZN4Page8allocateEmm(ptr %1, i64 ptrtoint (ptr getelementptr (%_Z6String, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr ({ i1, %_Z6String }, ptr null, i64 0, i32 1) to i64))
  call void @_ZN6StringC1EPN4scaly6memory4PageEm(ptr %struct.region, ptr %1, i64 %call6)
  store ptr %struct.region, ptr %ret, align 1
  %ret8 = load ptr, ptr %ret, align 8
  %call9 = call ptr @_ZN6String10get_bufferEv(ptr %ret8)
  %file10 = load ptr, ptr %file, align 8
  %call11 = call i64 @fread(ptr %call9, i64 1, i64 %call6, ptr %file10)
  %file12 = load ptr, ptr %file, align 8
  %call13 = call i64 @fclose(ptr %file12)
  %ret14 = load ptr, ptr %ret, align 8
  %sret.body15 = load %_Z6String, ptr %ret14, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %ret14, i64 ptrtoint (ptr getelementptr (%_Z6String, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr i1 @_ZN4File17write_from_stringE6String6String(ptr %0, ptr %1) {
entry:
  %local_page = call ptr @_Z21scaly_alloc_root_pagev()
  %call = call ptr @_ZN6String11to_c_stringEPN4scaly6memory4PageE(ptr %local_page, ptr %0)
  %call1 = call ptr @fopen(ptr %call, ptr @.str.22)
  %file = alloca ptr, align 8
  store ptr %call1, ptr %file, align 1
  %file2 = load ptr, ptr %file, align 8
  %eq = icmp eq ptr %file2, null
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  call void @_Z28scaly_release_root_page_fullP4Page(ptr %local_page)
  ret i1 false

if.end:                                           ; preds = %entry
  %call3 = call ptr @_ZN6String10get_bufferEv(ptr %1)
  %call4 = call i64 @_ZN6String10get_lengthEv(ptr %1)
  %file5 = load ptr, ptr %file, align 8
  %call6 = call i64 @fwrite(ptr %call3, i64 1, i64 %call4, ptr %file5)
  %file7 = load ptr, ptr %file, align 8
  %call8 = call i64 @fclose(ptr %file7)
  call void @_Z28scaly_release_root_page_fullP4Page(ptr %local_page)
  ret i1 true
}

define linkonce_odr i1 @_ZN9Directory6existsE6String(ptr %0) {
entry:
  %local_page = call ptr @_Z21scaly_alloc_root_pagev()
  %call = call ptr @_ZN6String11to_c_stringEPN4scaly6memory4PageE(ptr %local_page, ptr %0)
  %call1 = call i64 @access(ptr %call, i64 0)
  %eq = icmp eq i64 %call1, 0
  call void @_Z28scaly_release_root_page_fullP4Page(ptr %local_page)
  ret i1 %eq
}

define linkonce_odr i1 @_ZN9Directory12is_directoryE6String(ptr %0) {
entry:
  %local_page = call ptr @_Z21scaly_alloc_root_pagev()
  %call = call ptr @_ZN6String11to_c_stringEPN4scaly6memory4PageE(ptr %local_page, ptr %0)
  %call1 = call ptr @opendir(ptr %call)
  %dir = alloca ptr, align 8
  store ptr %call1, ptr %dir, align 1
  %dir2 = load ptr, ptr %dir, align 8
  %eq = icmp eq ptr %dir2, null
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  call void @_Z28scaly_release_root_page_fullP4Page(ptr %local_page)
  ret i1 false

if.end:                                           ; preds = %entry
  %dir3 = load ptr, ptr %dir, align 8
  %call4 = call i64 @closedir(ptr %dir3)
  call void @_Z28scaly_release_root_page_fullP4Page(ptr %local_page)
  ret i1 true
}

define linkonce_odr i1 @_ZN9Directory6createE6String(ptr %0) {
entry:
  %local_page = call ptr @_Z21scaly_alloc_root_pagev()
  %call = call ptr @_ZN6String11to_c_stringEPN4scaly6memory4PageE(ptr %local_page, ptr %0)
  %call1 = call i64 @mkdir(ptr %call, i64 493)
  %eq = icmp eq i64 %call1, 0
  call void @_Z28scaly_release_root_page_fullP4Page(ptr %local_page)
  ret i1 %eq
}

define linkonce_odr i1 @_ZN9Directory6removeE6String(ptr %0) {
entry:
  %local_page = call ptr @_Z21scaly_alloc_root_pagev()
  %call = call ptr @_ZN6String11to_c_stringEPN4scaly6memory4PageE(ptr %local_page, ptr %0)
  %call1 = call i64 @rmdir(ptr %call)
  %eq = icmp eq i64 %call1, 0
  call void @_Z28scaly_release_root_page_fullP4Page(ptr %local_page)
  ret i1 %eq
}

define linkonce_odr void @_ZN4Path18get_directory_nameEPN4scaly6memory4PageE6String(ptr noalias sret(%_Z6String) %0, ptr %1, ptr %2) {
entry:
  %call = call ptr @_ZN6String11to_c_stringEPN4scaly6memory4PageE(ptr %1, ptr %2)
  %call1 = call ptr @dirname(ptr %call)
  %call2 = call i64 @strcmp(ptr %call1, ptr @.str.23)
  %eq = icmp eq i64 %call2, 0
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  store { ptr } zeroinitializer, ptr %0, align 1
  ret void

if.end:                                           ; preds = %entry
  %struct.region = call ptr @_ZN4Page8allocateEmm(ptr %1, i64 ptrtoint (ptr getelementptr (%_Z6String, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr ({ i1, %_Z6String }, ptr null, i64 0, i32 1) to i64))
  call void @_ZN6StringC1EPN4scaly6memory4PageEP10const_char(ptr %struct.region, ptr %1, ptr %call1)
  %sret.body = load %_Z6String, ptr %struct.region, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %struct.region, i64 ptrtoint (ptr getelementptr (%_Z6String, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN4Path13get_file_nameEPN4scaly6memory4PageE6String(ptr noalias sret(%_Z6String) %0, ptr %1, ptr %2) {
entry:
  %call = call ptr @_ZN6String11to_c_stringEPN4scaly6memory4PageE(ptr %1, ptr %2)
  %call1 = call ptr @basename(ptr %call)
  %struct.region = call ptr @_ZN4Page8allocateEmm(ptr %1, i64 ptrtoint (ptr getelementptr (%_Z6String, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr ({ i1, %_Z6String }, ptr null, i64 0, i32 1) to i64))
  call void @_ZN6StringC1EPN4scaly6memory4PageEP10const_char(ptr %struct.region, ptr %1, ptr %call1)
  %sret.body = load %_Z6String, ptr %struct.region, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %struct.region, i64 ptrtoint (ptr getelementptr (%_Z6String, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN4Path4joinEPN4scaly6memory4PageE6String6String(ptr noalias sret(%_Z6String) %0, ptr %1, ptr %2, ptr %3) {
entry:
  %sret.result = alloca %_Z6String, align 8
  %path = alloca ptr, align 8
  %local_page = call ptr @_Z21scaly_alloc_root_pagev()
  %call = call i64 @_ZN6String10get_lengthEv(ptr %2)
  %eq = icmp eq i64 %call, 0
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %struct.region = call ptr @_ZN4Page8allocateEmm(ptr %1, i64 ptrtoint (ptr getelementptr (%_Z6String, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr ({ i1, %_Z6String }, ptr null, i64 0, i32 1) to i64))
  call void @_ZN6StringC1EPN4scaly6memory4PageE6String(ptr %struct.region, ptr %1, ptr %3)
  %sret.body = load %_Z6String, ptr %struct.region, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %struct.region, i64 ptrtoint (ptr getelementptr (%_Z6String, ptr null, i32 1) to i64), i1 false)
  call void @_Z23scaly_release_root_pageP4Page(ptr %local_page)
  ret void

if.end:                                           ; preds = %entry
  %struct.region1 = call ptr @_ZN4Page8allocateEmm(ptr %local_page, i64 ptrtoint (ptr getelementptr (%_Z13StringBuilder, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr ({ i1, %_Z13StringBuilder }, ptr null, i64 0, i32 1) to i64))
  call void @_ZN13StringBuilderC1EPN4scaly6memory4PageE(ptr %struct.region1, ptr %local_page)
  store ptr %struct.region1, ptr %path, align 1
  %path2 = load ptr, ptr %path, align 8
  call void @_ZN13StringBuilder6appendE6String(ptr %path2, ptr %2)
  %call3 = call i64 @_ZN6String10get_lengthEv(ptr %3)
  %eq4 = icmp eq i64 %call3, 0
  br i1 %eq4, label %if.then5, label %if.end6

if.then5:                                         ; preds = %if.end
  %path7 = load ptr, ptr %path, align 8
  call void @_ZN13StringBuilder9to_stringEPN4scaly6memory4PageE(ptr noalias sret(%_Z6String) %sret.result, ptr %1, ptr %path7)
  call void @_Z28scaly_release_root_page_fullP4Page(ptr %local_page)
  %sret.body8 = load %_Z6String, ptr %sret.result, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %sret.result, i64 ptrtoint (ptr getelementptr (%_Z6String, ptr null, i32 1) to i64), i1 false)
  ret void

if.end6:                                          ; preds = %if.end
  %path9 = load ptr, ptr %path, align 8
  call void @_ZN13StringBuilder6appendEc(ptr %path9, i8 47)
  %path10 = load ptr, ptr %path, align 8
  call void @_ZN13StringBuilder6appendE6String(ptr %path10, ptr %3)
  %path11 = load ptr, ptr %path, align 8
  call void @_ZN13StringBuilder9to_stringEPN4scaly6memory4PageE(ptr noalias sret(%_Z6String) %sret.result, ptr %1, ptr %path11)
  call void @_Z28scaly_release_root_page_fullP4Page(ptr %local_page)
  %sret.body12 = load %_Z6String, ptr %sret.result, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %sret.result, i64 ptrtoint (ptr getelementptr (%_Z6String, ptr null, i32 1) to i64), i1 false)
  ret void
}

; Function Attrs: nocallback nofree nounwind willreturn memory(argmem: readwrite)
declare void @llvm.memcpy.p0.p0.i64(ptr noalias nocapture writeonly, ptr noalias nocapture readonly, i64, i1 immarg) #0

define linkonce_odr ptr @_Z3getPv(ptr %0) {
entry:
  %page = call ptr @_ZN4Page3getEPv(ptr %0)
  ret ptr %page
}

attributes #0 = { nocallback nofree nounwind willreturn memory(argmem: readwrite) }
