; ModuleID = 'json'
source_filename = "json"
target datalayout = "e-m:o-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-n32:64-S128-Fn32"

%_Z5SliceI2u8E = type { i64, ptr }
%_Z13SliceIteratorI2u8E = type { %_Z5SliceI2u8E, i64 }
%_Z10JsonStream = type { i64, i64, i64, i64, i64, i64 }
%_Z10JsonReader = type { %_Z5SliceI2u8E, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i1, i1, i64, i64 }
%_Z9JsonToken = type { i8, [0 x i8] }
%_Z8JsonScan = type { i64, i64, i1 }
%_Z5SliceI1TE = type { i64, ptr }
%_Z13SliceIteratorI1TE = type { %_Z5SliceI1TE, i64 }
%_Z5SliceIcE = type { i64, ptr }
%_Z13SliceIteratorIcE = type { %_Z5SliceIcE, i64 }
%_Z5SliceI10JsonMemberE = type { i64, ptr }
%_Z10JsonMember = type { %_Z5SliceI2u8E, %_Z9JsonValue, i1 }
%_Z9JsonValue = type { i8, <{ [3 x i64], [0 x i8] }> }
%_Z13SliceIteratorI10JsonMemberE = type { %_Z5SliceI10JsonMemberE, i64 }
%_Z5SliceI9JsonValueE = type { i64, ptr }
%_Z13SliceIteratorI9JsonValueE = type { %_Z5SliceI9JsonValueE, i64 }
%_Z9JsonArena = type { %_Z5SliceI9JsonValueE, i64, %_Z5SliceI10JsonMemberE, i64 }
%_Z10JsonNumber = type { %_Z5SliceI2u8E, i1 }
%_Z10JsonString = type { %_Z5SliceI2u8E, i1 }
%_Z9JsonArray = type { %_Z5SliceI9JsonValueE }
%_Z10JsonObject = type { %_Z5SliceI10JsonMemberE }
%_Z6VectorI2u8E = type { i64, ptr }
%_Z14VectorIteratorI2u8E = type { ptr, i64 }
%_Z5ArrayI2u8E = type { i64, i64, ptr }
%_Z13ArrayIteratorI2u8E = type { ptr, i64 }
%_Z4ListI2u8E = type { ptr, ptr }
%_Z4NodeI2u8E = type { i8, ptr }
%_Z12ListIteratorI2u8E = type { ptr }
%_Z5ArrayI9JsonValueE = type { i64, i64, ptr }
%_Z6VectorI9JsonValueE = type { i64, ptr }
%_Z14VectorIteratorI9JsonValueE = type { ptr, i64 }
%_Z4ListI9JsonValueE = type { ptr, ptr }
%_Z4NodeI9JsonValueE = type { %_Z9JsonValue, ptr }
%_Z12ListIteratorI9JsonValueE = type { ptr }
%_Z13ArrayIteratorI9JsonValueE = type { ptr, i64 }
%_Z5ArrayI10JsonMemberE = type { i64, i64, ptr }
%_Z6VectorI10JsonMemberE = type { i64, ptr }
%_Z14VectorIteratorI10JsonMemberE = type { ptr, i64 }
%_Z4ListI10JsonMemberE = type { ptr, ptr }
%_Z4NodeI10JsonMemberE = type { %_Z10JsonMember, ptr }
%_Z12ListIteratorI10JsonMemberE = type { ptr }
%_Z13ArrayIteratorI10JsonMemberE = type { ptr, i64 }
%_Z5ArrayImE = type { i64, i64, ptr }
%_Z6VectorImE = type { i64, ptr }
%_Z14VectorIteratorImE = type { ptr, i64 }
%_Z5SliceImE = type { i64, ptr }
%_Z13SliceIteratorImE = type { %_Z5SliceImE, i64 }
%_Z4ListImE = type { ptr, ptr }
%_Z4NodeImE = type { i64, ptr }
%_Z12ListIteratorImE = type { ptr }
%_Z13ArrayIteratorImE = type { ptr, i64 }
%_Z10JsonParsed = type { %_Z9JsonValue, i64, i64 }
%_Z10JsonWriter = type { %_Z5ArrayI2u8E, i1, i1 }
%_Z5ArrayI14JsonWriteFrameE = type { i64, i64, ptr }
%_Z14JsonWriteFrame = type { %_Z5SliceI9JsonValueE, %_Z5SliceI10JsonMemberE, i64 }
%_Z6VectorI14JsonWriteFrameE = type { i64, ptr }
%_Z14VectorIteratorI14JsonWriteFrameE = type { ptr, i64 }
%_Z5SliceI14JsonWriteFrameE = type { i64, ptr }
%_Z13SliceIteratorI14JsonWriteFrameE = type { %_Z5SliceI14JsonWriteFrameE, i64 }
%_Z4ListI14JsonWriteFrameE = type { ptr, ptr }
%_Z4NodeI14JsonWriteFrameE = type { %_Z14JsonWriteFrame, ptr }
%_Z12ListIteratorI14JsonWriteFrameE = type { ptr }
%_Z13ArrayIteratorI14JsonWriteFrameE = type { ptr, i64 }

@"9MAX_DEPTH" = internal constant i64 256
@"12EXPECT_VALUE" = internal constant i64 0
@"19EXPECT_VALUE_OR_END" = internal constant i64 1
@"10EXPECT_KEY" = internal constant i64 2
@"17EXPECT_KEY_OR_END" = internal constant i64 3
@"16EXPECT_SEPARATOR" = internal constant i64 4
@"10EXPECT_EOF" = internal constant i64 5
@"11READER_DONE" = internal constant i64 6
@"7JSON_OK" = internal constant i64 0
@"8JSON_END" = internal constant i64 1
@"13JSON_TRAILING" = internal constant i64 2
@"10JSON_VALUE" = internal constant i64 3
@"8JSON_KEY" = internal constant i64 4
@"10JSON_COLON" = internal constant i64 5
@"14JSON_SEPARATOR" = internal constant i64 6
@"11JSON_NUMBER" = internal constant i64 7
@"12JSON_CONTROL" = internal constant i64 8
@"11JSON_ESCAPE" = internal constant i64 9
@"10JSON_DEPTH" = internal constant i64 10
@"11STAGE_VALUE" = internal constant i64 0
@"9STAGE_KEY" = internal constant i64 1
@"14STAGE_COMPLETE" = internal constant i64 2
@"12JSON_PADDING" = internal constant i64 16
@"10JSON_CHUNK" = internal constant i64 128
@"10JSON_BLOCK" = internal constant i64 4096
@.str = private unnamed_addr constant [9 x i8] c"Slice.at\00", align 1
@.str.1 = private unnamed_addr constant [10 x i8] c"Slice.put\00", align 1
@.str.2 = private unnamed_addr constant [8 x i8] c"Slice[]\00", align 1
@.simd.mem = private unnamed_addr constant [10 x i8] c"SIMD load\00", align 1
@.simd.mem.3 = private unnamed_addr constant [10 x i8] c"SIMD load\00", align 1
@.str.4 = private unnamed_addr constant [9 x i8] c"Slice.at\00", align 1
@.str.5 = private unnamed_addr constant [10 x i8] c"Slice.put\00", align 1
@.str.6 = private unnamed_addr constant [9 x i8] c"no error\00", align 1
@.str.7 = private unnamed_addr constant [31 x i8] c"unexpected end of the document\00", align 1
@.str.8 = private unnamed_addr constant [38 x i8] c"characters after the document's value\00", align 1
@.str.9 = private unnamed_addr constant [21 x i8] c"a value was expected\00", align 1
@.str.10 = private unnamed_addr constant [38 x i8] c"a member name (a string) was expected\00", align 1
@.str.11 = private unnamed_addr constant [39 x i8] c"':' was expected after the member name\00", align 1
@.str.12 = private unnamed_addr constant [40 x i8] c"',' or the closing bracket was expected\00", align 1
@.str.13 = private unnamed_addr constant [17 x i8] c"malformed number\00", align 1
@.str.14 = private unnamed_addr constant [34 x i8] c"raw control character in a string\00", align 1
@.str.15 = private unnamed_addr constant [27 x i8] c"invalid escape in a string\00", align 1
@.str.16 = private unnamed_addr constant [17 x i8] c"nesting too deep\00", align 1
@.str.17 = private unnamed_addr constant [14 x i8] c"unknown error\00", align 1
@.str.18 = private unnamed_addr constant [5 x i8] c"true\00", align 1
@.str.19 = private unnamed_addr constant [6 x i8] c"false\00", align 1
@.str.20 = private unnamed_addr constant [5 x i8] c"null\00", align 1
@.simd.mem.21 = private unnamed_addr constant [10 x i8] c"SIMD load\00", align 1
@.simd.mem.22 = private unnamed_addr constant [10 x i8] c"SIMD load\00", align 1
@.str.23 = private unnamed_addr constant [9 x i8] c"Slice.at\00", align 1
@.str.24 = private unnamed_addr constant [10 x i8] c"Slice.put\00", align 1
@.str.25 = private unnamed_addr constant [9 x i8] c"Slice.at\00", align 1
@.str.26 = private unnamed_addr constant [10 x i8] c"Slice.put\00", align 1
@.str.27 = private unnamed_addr constant [8 x i8] c"Slice[]\00", align 1
@.str.28 = private unnamed_addr constant [8 x i8] c"Slice[]\00", align 1
@.str.29 = private unnamed_addr constant [10 x i8] c"Vector.at\00", align 1
@.str.30 = private unnamed_addr constant [11 x i8] c"Vector.put\00", align 1
@.str.31 = private unnamed_addr constant [10 x i8] c"Array.add\00", align 1
@.str.32 = private unnamed_addr constant [13 x i8] c"Array.extend\00", align 1
@.str.33 = private unnamed_addr constant [9 x i8] c"Array.at\00", align 1
@.str.34 = private unnamed_addr constant [10 x i8] c"Array.put\00", align 1
@.str.35 = private unnamed_addr constant [10 x i8] c"Vector.at\00", align 1
@.str.36 = private unnamed_addr constant [11 x i8] c"Vector.put\00", align 1
@.str.37 = private unnamed_addr constant [10 x i8] c"Array.add\00", align 1
@.str.38 = private unnamed_addr constant [13 x i8] c"Array.extend\00", align 1
@.str.39 = private unnamed_addr constant [9 x i8] c"Array.at\00", align 1
@.str.40 = private unnamed_addr constant [10 x i8] c"Array.put\00", align 1
@.str.41 = private unnamed_addr constant [10 x i8] c"Vector.at\00", align 1
@.str.42 = private unnamed_addr constant [11 x i8] c"Vector.put\00", align 1
@.str.43 = private unnamed_addr constant [10 x i8] c"Array.add\00", align 1
@.str.44 = private unnamed_addr constant [13 x i8] c"Array.extend\00", align 1
@.str.45 = private unnamed_addr constant [9 x i8] c"Array.at\00", align 1
@.str.46 = private unnamed_addr constant [10 x i8] c"Array.put\00", align 1
@.str.47 = private unnamed_addr constant [10 x i8] c"Vector.at\00", align 1
@.str.48 = private unnamed_addr constant [11 x i8] c"Vector.put\00", align 1
@.str.49 = private unnamed_addr constant [9 x i8] c"Slice.at\00", align 1
@.str.50 = private unnamed_addr constant [10 x i8] c"Slice.put\00", align 1
@.str.51 = private unnamed_addr constant [10 x i8] c"Array.add\00", align 1
@.str.52 = private unnamed_addr constant [13 x i8] c"Array.extend\00", align 1
@.str.53 = private unnamed_addr constant [9 x i8] c"Array.at\00", align 1
@.str.54 = private unnamed_addr constant [10 x i8] c"Array.put\00", align 1
@.str.55 = private unnamed_addr constant [8 x i8] c"Array[]\00", align 1
@.str.56 = private unnamed_addr constant [8 x i8] c"Array[]\00", align 1
@.str.57 = private unnamed_addr constant [5 x i8] c"true\00", align 1
@.str.58 = private unnamed_addr constant [6 x i8] c"false\00", align 1
@.str.59 = private unnamed_addr constant [5 x i8] c"null\00", align 1
@.str.60 = private unnamed_addr constant [5 x i8] c"true\00", align 1
@.str.61 = private unnamed_addr constant [6 x i8] c"false\00", align 1
@.str.62 = private unnamed_addr constant [5 x i8] c"null\00", align 1
@.str.63 = private unnamed_addr constant [10 x i8] c"Vector.at\00", align 1
@.str.64 = private unnamed_addr constant [11 x i8] c"Vector.put\00", align 1
@.str.65 = private unnamed_addr constant [9 x i8] c"Slice.at\00", align 1
@.str.66 = private unnamed_addr constant [10 x i8] c"Slice.put\00", align 1
@.str.67 = private unnamed_addr constant [10 x i8] c"Array.add\00", align 1
@.str.68 = private unnamed_addr constant [13 x i8] c"Array.extend\00", align 1
@.str.69 = private unnamed_addr constant [9 x i8] c"Array.at\00", align 1
@.str.70 = private unnamed_addr constant [10 x i8] c"Array.put\00", align 1
@.str.71 = private unnamed_addr constant [8 x i8] c"Array[]\00", align 1
@.simd.mem.72 = private unnamed_addr constant [10 x i8] c"SIMD load\00", align 1
@.simd.mem.73 = private unnamed_addr constant [11 x i8] c"SIMD store\00", align 1
@.simd.mem.74 = private unnamed_addr constant [10 x i8] c"SIMD load\00", align 1
@.simd.mem.75 = private unnamed_addr constant [11 x i8] c"SIMD store\00", align 1
@.simd.mem.76 = private unnamed_addr constant [10 x i8] c"SIMD load\00", align 1
@.simd.mem.77 = private unnamed_addr constant [11 x i8] c"SIMD store\00", align 1
@.simd.mem.78 = private unnamed_addr constant [10 x i8] c"SIMD load\00", align 1
@.simd.mem.79 = private unnamed_addr constant [11 x i8] c"SIMD store\00", align 1
@.str.80 = private unnamed_addr constant [4 x i8] c"u00\00", align 1
@.sconst = private constant [5 x i8] c"\03u00\00"
@.sconst.81 = private constant [6 x i8] c"\04null\00"
@.sconst.82 = private constant [6 x i8] c"\04true\00"
@.sconst.83 = private constant [7 x i8] c"\05false\00"

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

define linkonce_odr ptr @_ZN5SliceI2u8E3getEm(ptr %0, i64 %1) {
entry:
  %load.struct = load %_Z5SliceI2u8E, ptr %0, align 8
  %length = extractvalue %_Z5SliceI2u8E %load.struct, 0
  %ge = icmp uge i64 %1, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %load.struct1 = load %_Z5SliceI2u8E, ptr %0, align 8
  %data = extractvalue %_Z5SliceI2u8E %load.struct1, 1
  %ptr.add = getelementptr inbounds i8, ptr %data, i64 %1
  ret ptr %ptr.add
}

define linkonce_odr ptr @_ZN5SliceI2u8E2atEm(ptr %0, i64 %1) {
entry:
  %load.struct = load %_Z5SliceI2u8E, ptr %0, align 8
  %length = extractvalue %_Z5SliceI2u8E %load.struct, 0
  %ge = icmp uge i64 %1, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %field.inplace = getelementptr inbounds nuw %_Z5SliceI2u8E, ptr %0, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  call void @_Z15scaly_panic_oobP10const_charmm(ptr @.str, i64 %1, i64 %field.val)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %load.struct1 = load %_Z5SliceI2u8E, ptr %0, align 8
  %data = extractvalue %_Z5SliceI2u8E %load.struct1, 1
  %ptr.add = getelementptr inbounds i8, ptr %data, i64 %1
  ret ptr %ptr.add
}

define linkonce_odr void @_ZN5SliceI2u8E3putEm2u8(ptr %0, i64 %1, i8 %2) {
entry:
  %load.struct = load %_Z5SliceI2u8E, ptr %0, align 8
  %length = extractvalue %_Z5SliceI2u8E %load.struct, 0
  %ge = icmp uge i64 %1, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %field.inplace = getelementptr inbounds nuw %_Z5SliceI2u8E, ptr %0, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  call void @_Z15scaly_panic_oobP10const_charmm(ptr @.str.1, i64 %1, i64 %field.val)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %load.struct1 = load %_Z5SliceI2u8E, ptr %0, align 8
  %data = extractvalue %_Z5SliceI2u8E %load.struct1, 1
  %ptr.add = getelementptr inbounds i8, ptr %data, i64 %1
  store i8 %2, ptr %ptr.add, align 1
  ret void
}

define linkonce_odr i1 @_ZN5SliceI2u8E8is_emptyEv(ptr %0) {
entry:
  %load.struct = load %_Z5SliceI2u8E, ptr %0, align 8
  %length = extractvalue %_Z5SliceI2u8E %load.struct, 0
  %eq = icmp eq i64 %length, 0
  ret i1 %eq
}

define linkonce_odr void @_ZN5SliceI2u8E8subsliceEmm(ptr noalias sret(%_Z5SliceI2u8E) %0, ptr %1, i64 %2, i64 %3) {
entry:
  %tuple = alloca %_Z5SliceI2u8E, align 8
  %from = alloca i64, align 8
  store i64 %2, ptr %from, align 1
  %to = alloca i64, align 8
  store i64 %3, ptr %to, align 1
  %from1 = load i64, ptr %from, align 8
  %load.struct = load %_Z5SliceI2u8E, ptr %1, align 8
  %length = extractvalue %_Z5SliceI2u8E %load.struct, 0
  %gt = icmp ugt i64 %from1, %length
  br i1 %gt, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %load.struct2 = load %_Z5SliceI2u8E, ptr %1, align 8
  %length3 = extractvalue %_Z5SliceI2u8E %load.struct2, 0
  store i64 %length3, ptr %from, align 1
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %to4 = load i64, ptr %to, align 8
  %load.struct5 = load %_Z5SliceI2u8E, ptr %1, align 8
  %length6 = extractvalue %_Z5SliceI2u8E %load.struct5, 0
  %gt7 = icmp ugt i64 %to4, %length6
  br i1 %gt7, label %if.then8, label %if.end9

if.then8:                                         ; preds = %if.end
  %load.struct10 = load %_Z5SliceI2u8E, ptr %1, align 8
  %length11 = extractvalue %_Z5SliceI2u8E %load.struct10, 0
  store i64 %length11, ptr %to, align 1
  br label %if.end9

if.end9:                                          ; preds = %if.then8, %if.end
  %from12 = load i64, ptr %from, align 8
  %to13 = load i64, ptr %to, align 8
  %gt14 = icmp ugt i64 %from12, %to13
  br i1 %gt14, label %if.then15, label %if.end16

if.then15:                                        ; preds = %if.end9
  %to17 = load i64, ptr %to, align 8
  store i64 %to17, ptr %from, align 1
  br label %if.end16

if.end16:                                         ; preds = %if.then15, %if.end9
  %to18 = load i64, ptr %to, align 8
  %from19 = load i64, ptr %from, align 8
  %sub = sub i64 %to18, %from19
  %load.struct20 = load %_Z5SliceI2u8E, ptr %1, align 8
  %data = extractvalue %_Z5SliceI2u8E %load.struct20, 1
  %from21 = load i64, ptr %from, align 8
  %ptr.add = getelementptr inbounds i8, ptr %data, i64 %from21
  %tuple.field = getelementptr inbounds nuw %_Z5SliceI2u8E, ptr %tuple, i32 0, i32 0
  store i64 %sub, ptr %tuple.field, align 1
  %tuple.field22 = getelementptr inbounds nuw %_Z5SliceI2u8E, ptr %tuple, i32 0, i32 1
  store ptr %ptr.add, ptr %tuple.field22, align 1
  %tuple.val = load %_Z5SliceI2u8E, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z5SliceI2u8E, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN5SliceI2u8E10slice_fromEPN4scaly6memory4PageEm(ptr noalias sret(%_Z5SliceI2u8E) %0, ptr %1, ptr %2, i64 %3) {
entry:
  %sret.result = alloca %_Z5SliceI2u8E, align 8
  %field.inplace = getelementptr inbounds nuw %_Z5SliceI2u8E, ptr %2, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  call void @_ZN5SliceI2u8E8subsliceEmm(ptr noalias sret(%_Z5SliceI2u8E) %sret.result, ptr %2, i64 %3, i64 %field.val)
  %sret.body = load %_Z5SliceI2u8E, ptr %sret.result, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %sret.result, i64 ptrtoint (ptr getelementptr (%_Z5SliceI2u8E, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN5SliceI2u8E8slice_toEPN4scaly6memory4PageEm(ptr noalias sret(%_Z5SliceI2u8E) %0, ptr %1, ptr %2, i64 %3) {
entry:
  %sret.result = alloca %_Z5SliceI2u8E, align 8
  call void @_ZN5SliceI2u8E8subsliceEmm(ptr noalias sret(%_Z5SliceI2u8E) %sret.result, ptr %2, i64 0, i64 %3)
  %sret.body = load %_Z5SliceI2u8E, ptr %sret.result, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %sret.result, i64 ptrtoint (ptr getelementptr (%_Z5SliceI2u8E, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr i1 @_ZN5SliceI2u8E6equalsE5SliceI2u8E(ptr %0, ptr %1) {
entry:
  %load.struct = load %_Z5SliceI2u8E, ptr %0, align 8
  %length = extractvalue %_Z5SliceI2u8E %load.struct, 0
  %load.struct1 = load %_Z5SliceI2u8E, ptr %1, align 8
  %length2 = extractvalue %_Z5SliceI2u8E %load.struct1, 0
  %ne = icmp ne i64 %length, %length2
  br i1 %ne, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret i1 false

if.end:                                           ; preds = %entry
  %load.struct3 = load %_Z5SliceI2u8E, ptr %0, align 8
  %length4 = extractvalue %_Z5SliceI2u8E %load.struct3, 0
  %eq = icmp eq i64 %length4, 0
  br i1 %eq, label %if.then5, label %if.end6

if.then5:                                         ; preds = %if.end
  ret i1 true

if.end6:                                          ; preds = %if.end
  %load.struct7 = load %_Z5SliceI2u8E, ptr %0, align 8
  %data = extractvalue %_Z5SliceI2u8E %load.struct7, 1
  %load.struct8 = load %_Z5SliceI2u8E, ptr %1, align 8
  %data9 = extractvalue %_Z5SliceI2u8E %load.struct8, 1
  %load.struct10 = load %_Z5SliceI2u8E, ptr %0, align 8
  %length11 = extractvalue %_Z5SliceI2u8E %load.struct10, 0
  %mul = mul i64 %length11, ptrtoint (ptr getelementptr (i8, ptr null, i32 1) to i64)
  %call = call i32 @memcmp(ptr %data, ptr %data9, i64 %mul)
  %eq12 = icmp eq i32 %call, 0
  ret i1 %eq12
}

define linkonce_odr i1 @_ZN5SliceI2u8E11starts_withE5SliceI2u8E(ptr %0, ptr %1) {
entry:
  %sret.result = alloca %_Z5SliceI2u8E, align 8
  %load.struct = load %_Z5SliceI2u8E, ptr %1, align 8
  %length = extractvalue %_Z5SliceI2u8E %load.struct, 0
  %load.struct1 = load %_Z5SliceI2u8E, ptr %0, align 8
  %length2 = extractvalue %_Z5SliceI2u8E %load.struct1, 0
  %gt = icmp ugt i64 %length, %length2
  br i1 %gt, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret i1 false

if.end:                                           ; preds = %entry
  %field.inplace = getelementptr inbounds nuw %_Z5SliceI2u8E, ptr %1, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  call void @_ZN5SliceI2u8E8subsliceEmm(ptr noalias sret(%_Z5SliceI2u8E) %sret.result, ptr %0, i64 0, i64 %field.val)
  %call = call i1 @_ZN5SliceI2u8E6equalsE5SliceI2u8E(ptr %sret.result, ptr %1)
  ret i1 %call
}

define linkonce_odr i1 @_ZN5SliceI2u8E9ends_withE5SliceI2u8E(ptr %0, ptr %1) {
entry:
  %sret.result = alloca %_Z5SliceI2u8E, align 8
  %load.struct = load %_Z5SliceI2u8E, ptr %1, align 8
  %length = extractvalue %_Z5SliceI2u8E %load.struct, 0
  %load.struct1 = load %_Z5SliceI2u8E, ptr %0, align 8
  %length2 = extractvalue %_Z5SliceI2u8E %load.struct1, 0
  %gt = icmp ugt i64 %length, %length2
  br i1 %gt, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret i1 false

if.end:                                           ; preds = %entry
  %load.struct3 = load %_Z5SliceI2u8E, ptr %0, align 8
  %length4 = extractvalue %_Z5SliceI2u8E %load.struct3, 0
  %load.struct5 = load %_Z5SliceI2u8E, ptr %1, align 8
  %length6 = extractvalue %_Z5SliceI2u8E %load.struct5, 0
  %sub = sub i64 %length4, %length6
  %field.inplace = getelementptr inbounds nuw %_Z5SliceI2u8E, ptr %0, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  call void @_ZN5SliceI2u8E8subsliceEmm(ptr noalias sret(%_Z5SliceI2u8E) %sret.result, ptr %0, i64 %sub, i64 %field.val)
  %call = call i1 @_ZN5SliceI2u8E6equalsE5SliceI2u8E(ptr %sret.result, ptr %1)
  ret i1 %call
}

define linkonce_odr ptr @_ZN13SliceIteratorI2u8E4nextEv(ptr %0) {
entry:
  %load.struct = load %_Z13SliceIteratorI2u8E, ptr %0, align 8
  %position = extractvalue %_Z13SliceIteratorI2u8E %load.struct, 1
  %load.struct1 = load %_Z13SliceIteratorI2u8E, ptr %0, align 8
  %slice = extractvalue %_Z13SliceIteratorI2u8E %load.struct1, 0
  %length = extractvalue %_Z5SliceI2u8E %slice, 0
  %ge = icmp uge i64 %position, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %load.struct2 = load %_Z13SliceIteratorI2u8E, ptr %0, align 8
  %slice3 = extractvalue %_Z13SliceIteratorI2u8E %load.struct2, 0
  %data = extractvalue %_Z5SliceI2u8E %slice3, 1
  %load.struct4 = load %_Z13SliceIteratorI2u8E, ptr %0, align 8
  %position5 = extractvalue %_Z13SliceIteratorI2u8E %load.struct4, 1
  %ptr.add = getelementptr inbounds i8, ptr %data, i64 %position5
  %load.struct6 = load %_Z13SliceIteratorI2u8E, ptr %0, align 8
  %position7 = extractvalue %_Z13SliceIteratorI2u8E %load.struct6, 1
  %add = add i64 %position7, 1
  %position8 = getelementptr inbounds nuw %_Z13SliceIteratorI2u8E, ptr %0, i32 0, i32 1
  store i64 %add, ptr %position8, align 8
  ret ptr %ptr.add
}

define linkonce_odr void @_ZN13SliceIteratorI2u8EC1E5SliceI2u8E(ptr %0, ptr %1) {
entry:
  %slice = getelementptr inbounds nuw %_Z13SliceIteratorI2u8E, ptr %0, i32 0, i32 0
  %field.load = load %_Z5SliceI2u8E, ptr %1, align 8
  store %_Z5SliceI2u8E %field.load, ptr %slice, align 8
  %position = getelementptr inbounds nuw %_Z13SliceIteratorI2u8E, ptr %0, i32 0, i32 1
  store i64 0, ptr %position, align 8
  ret void
}

define linkonce_odr void @_ZN5SliceI2u8E12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z13SliceIteratorI2u8E) %0, ptr %1, ptr %2) {
entry:
  %struct.init = alloca %_Z13SliceIteratorI2u8E, align 8
  call void @_ZN13SliceIteratorI2u8EC1E5SliceI2u8E(ptr %struct.init, ptr %2)
  %sret.body = load %_Z13SliceIteratorI2u8E, ptr %struct.init, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %struct.init, i64 ptrtoint (ptr getelementptr (%_Z13SliceIteratorI2u8E, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN5SliceI2u8EC1Ev(ptr %0) {
entry:
  %data = getelementptr inbounds nuw %_Z5SliceI2u8E, ptr %0, i32 0, i32 1
  store ptr null, ptr %data, align 8
  %length = getelementptr inbounds nuw %_Z5SliceI2u8E, ptr %0, i32 0, i32 0
  store i64 0, ptr %length, align 8
  ret void
}

define linkonce_odr void @_ZN10JsonStreamB4json8classifyEPN4scaly6memory4PageE5SliceI2u8E10JsonStreamB4json(ptr noalias sret(%_Z10JsonStream) %0, ptr %1, ptr %2, ptr %3) {
entry:
  %tuple = alloca %_Z10JsonStream, align 8
  %b = alloca i64, align 8
  %escape_carry80 = alloca i64, align 8
  %i = alloca i64, align 8
  %load.struct = load %_Z10JsonStream, ptr %3, align 8
  %to = extractvalue %_Z10JsonStream %load.struct, 2
  %quote = alloca i64, align 8
  store i64 0, ptr %quote, align 1
  %back = alloca i64, align 8
  store i64 0, ptr %back, align 1
  %nonscalar = alloca i64, align 8
  store i64 0, ptr %nonscalar, align 1
  %op = alloca i64, align 8
  store i64 0, ptr %op, align 1
  %special = alloca i64, align 8
  store i64 0, ptr %special, align 1
  %add = add i64 %to, 64
  %load.struct1 = load %_Z5SliceI2u8E, ptr %2, align 8
  %length = extractvalue %_Z5SliceI2u8E %load.struct1, 0
  %le = icmp ule i64 %add, %length
  br i1 %le, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %simd.cont = getelementptr inbounds nuw %_Z5SliceI2u8E, ptr %2, i32 0, i32 0
  %simd.len = load i64, ptr %simd.cont, align 8
  %simd.cont2 = getelementptr inbounds nuw %_Z5SliceI2u8E, ptr %2, i32 0, i32 1
  %simd.data = load ptr, ptr %simd.cont2, align 8
  %simd.fits = icmp uge i64 %simd.len, 32
  %simd.room = sub i64 %simd.len, 32
  %simd.within = icmp ule i64 %to, %simd.room
  %simd.inrange = and i1 %simd.fits, %simd.within
  br i1 %simd.inrange, label %simd.mem.ok, label %simd.mem.oob

if.else:                                          ; preds = %entry
  store i64 0, ptr %i, align 1
  br label %while.cond

if.end:                                           ; preds = %while.exit70, %simd.mem.ok13
  %load.struct79 = load %_Z10JsonStream, ptr %3, align 8
  %escape_carry = extractvalue %_Z10JsonStream %load.struct79, 3
  store i64 %escape_carry, ptr %i, align 1
  store i64 0, ptr %escape_carry80, align 1
  %back81 = load i64, ptr %back, align 8
  %escaped = load i64, ptr %i, align 8
  %xor = xor i64 %escaped, -1
  %and = and i64 %back81, %xor
  store i64 %and, ptr %b, align 1
  br label %while.cond82

simd.mem.oob:                                     ; preds = %if.then
  call void @_Z15scaly_panic_oobP10const_charmm(ptr @.simd.mem, i64 %to, i64 %simd.len)
  unreachable

simd.mem.ok:                                      ; preds = %if.then
  %simd.addr = getelementptr inbounds i8, ptr %simd.data, i64 %to
  %simd.load = load <32 x i8>, ptr %simd.addr, align 1
  %call = call <32 x i8> @_ZN10JsonStreamB4json7classesE5u8x32(<32 x i8> %simd.load)
  %add3 = add i64 %to, 32
  %simd.cont4 = getelementptr inbounds nuw %_Z5SliceI2u8E, ptr %2, i32 0, i32 0
  %simd.len5 = load i64, ptr %simd.cont4, align 8
  %simd.cont6 = getelementptr inbounds nuw %_Z5SliceI2u8E, ptr %2, i32 0, i32 1
  %simd.data7 = load ptr, ptr %simd.cont6, align 8
  %simd.fits8 = icmp uge i64 %simd.len5, 32
  %simd.room9 = sub i64 %simd.len5, 32
  %simd.within10 = icmp ule i64 %add3, %simd.room9
  %simd.inrange11 = and i1 %simd.fits8, %simd.within10
  br i1 %simd.inrange11, label %simd.mem.ok13, label %simd.mem.oob12

simd.mem.oob12:                                   ; preds = %simd.mem.ok
  call void @_Z15scaly_panic_oobP10const_charmm(ptr @.simd.mem.3, i64 %add3, i64 %simd.len5)
  unreachable

simd.mem.ok13:                                    ; preds = %simd.mem.ok
  %simd.addr14 = getelementptr inbounds i8, ptr %simd.data7, i64 %add3
  %simd.load15 = load <32 x i8>, ptr %simd.addr14, align 1
  %call16 = call <32 x i8> @_ZN10JsonStreamB4json7classesE5u8x32(<32 x i8> %simd.load15)
  %call17 = call i64 @_ZN10JsonStreamB4json10class_bitsE5u8x325u8x322u8(<32 x i8> %call, <32 x i8> %call16, i8 -128)
  store i64 %call17, ptr %quote, align 1
  %call18 = call i64 @_ZN10JsonStreamB4json10class_bitsE5u8x325u8x322u8(<32 x i8> %call, <32 x i8> %call16, i8 64)
  store i64 %call18, ptr %back, align 1
  %call19 = call i64 @_ZN10JsonStreamB4json10class_bitsE5u8x325u8x322u8(<32 x i8> %call, <32 x i8> %call16, i8 7)
  store i64 %call19, ptr %op, align 1
  %call20 = call i64 @_ZN10JsonStreamB4json10class_bitsE5u8x325u8x322u8(<32 x i8> %call, <32 x i8> %call16, i8 -97)
  store i64 %call20, ptr %nonscalar, align 1
  %call21 = call i64 @_ZN10JsonStreamB4json10class_bitsE5u8x325u8x322u8(<32 x i8> %call, <32 x i8> %call16, i8 96)
  store i64 %call21, ptr %special, align 1
  br label %if.end

while.cond:                                       ; preds = %if.end63, %if.else
  %i22 = load i64, ptr %i, align 8
  %add23 = add i64 %to, %i22
  %load.struct24 = load %_Z5SliceI2u8E, ptr %2, align 8
  %length25 = extractvalue %_Z5SliceI2u8E %load.struct24, 0
  %lt = icmp ult i64 %add23, %length25
  br i1 %lt, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %i26 = load i64, ptr %i, align 8
  %add27 = add i64 %to, %i26
  %call28 = call i8 @_ZN5SliceI2u8EixEm(ptr %2, i64 %add27)
  %i29 = load i64, ptr %i, align 8
  %shl = shl i64 1, %i29
  %match.cmp = icmp eq i8 %call28, 34
  br i1 %match.cmp, label %match.case, label %match.next

while.exit:                                       ; preds = %while.cond
  br label %while.cond68

match.end:                                        ; preds = %if.end58, %match.case40, %match.case33, %match.case
  %lt61 = icmp ult i8 %call28, 32
  br i1 %lt61, label %if.then62, label %if.end63

match.case:                                       ; preds = %while.body
  %quote30 = load i64, ptr %quote, align 8
  %or = or i64 %quote30, %shl
  store i64 %or, ptr %quote, align 1
  %nonscalar31 = load i64, ptr %nonscalar, align 8
  %or32 = or i64 %nonscalar31, %shl
  store i64 %or32, ptr %nonscalar, align 1
  br label %match.end

match.next:                                       ; preds = %while.body
  %match.cmp35 = icmp eq i8 %call28, 92
  br i1 %match.cmp35, label %match.case33, label %match.next34

match.case33:                                     ; preds = %match.next
  %back36 = load i64, ptr %back, align 8
  %or37 = or i64 %back36, %shl
  store i64 %or37, ptr %back, align 1
  %special38 = load i64, ptr %special, align 8
  %or39 = or i64 %special38, %shl
  store i64 %or39, ptr %special, align 1
  br label %match.end

match.next34:                                     ; preds = %match.next
  %match.cmp42 = icmp eq i8 %call28, 123
  br i1 %match.cmp42, label %match.case40, label %match.alt

match.case40:                                     ; preds = %match.alt50, %match.alt48, %match.alt46, %match.alt44, %match.alt, %match.next34
  %op52 = load i64, ptr %op, align 8
  %or53 = or i64 %op52, %shl
  store i64 %or53, ptr %op, align 1
  %nonscalar54 = load i64, ptr %nonscalar, align 8
  %or55 = or i64 %nonscalar54, %shl
  store i64 %or55, ptr %nonscalar, align 1
  br label %match.end

match.next41:                                     ; preds = %match.alt50
  %call56 = call i1 @_ZN10JsonReaderB4json5is_wsE2u8(i8 %call28)
  br i1 %call56, label %if.then57, label %if.end58

match.alt:                                        ; preds = %match.next34
  %match.cmp43 = icmp eq i8 %call28, 125
  br i1 %match.cmp43, label %match.case40, label %match.alt44

match.alt44:                                      ; preds = %match.alt
  %match.cmp45 = icmp eq i8 %call28, 91
  br i1 %match.cmp45, label %match.case40, label %match.alt46

match.alt46:                                      ; preds = %match.alt44
  %match.cmp47 = icmp eq i8 %call28, 93
  br i1 %match.cmp47, label %match.case40, label %match.alt48

match.alt48:                                      ; preds = %match.alt46
  %match.cmp49 = icmp eq i8 %call28, 58
  br i1 %match.cmp49, label %match.case40, label %match.alt50

match.alt50:                                      ; preds = %match.alt48
  %match.cmp51 = icmp eq i8 %call28, 44
  br i1 %match.cmp51, label %match.case40, label %match.next41

if.then57:                                        ; preds = %match.next41
  %nonscalar59 = load i64, ptr %nonscalar, align 8
  %or60 = or i64 %nonscalar59, %shl
  store i64 %or60, ptr %nonscalar, align 1
  br label %if.end58

if.end58:                                         ; preds = %if.then57, %match.next41
  br label %match.end

if.then62:                                        ; preds = %match.end
  %special64 = load i64, ptr %special, align 8
  %or65 = or i64 %special64, %shl
  store i64 %or65, ptr %special, align 1
  br label %if.end63

if.end63:                                         ; preds = %if.then62, %match.end
  %i66 = load i64, ptr %i, align 8
  %add67 = add i64 %i66, 1
  store i64 %add67, ptr %i, align 1
  br label %while.cond

while.cond68:                                     ; preds = %while.body69, %while.exit
  %i71 = load i64, ptr %i, align 8
  %lt72 = icmp ult i64 %i71, 64
  br i1 %lt72, label %while.body69, label %while.exit70

while.body69:                                     ; preds = %while.cond68
  %nonscalar73 = load i64, ptr %nonscalar, align 8
  %i74 = load i64, ptr %i, align 8
  %shl75 = shl i64 1, %i74
  %or76 = or i64 %nonscalar73, %shl75
  store i64 %or76, ptr %nonscalar, align 1
  %i77 = load i64, ptr %i, align 8
  %add78 = add i64 %i77, 1
  store i64 %add78, ptr %i, align 1
  br label %while.cond68

while.exit70:                                     ; preds = %while.cond68
  br label %if.end

while.cond82:                                     ; preds = %if.end89, %if.end
  %b85 = load i64, ptr %b, align 8
  %ne = icmp ne i64 %b85, 0
  br i1 %ne, label %while.body83, label %while.exit84

while.body83:                                     ; preds = %while.cond82
  %b86 = load i64, ptr %b, align 8
  %call87 = call i64 @_Z14trailing_zeros3u64(i64 %b86)
  %eq = icmp eq i64 %call87, 63
  br i1 %eq, label %if.then88, label %if.end89

while.exit84:                                     ; preds = %if.then88, %while.cond82
  %quote100 = load i64, ptr %quote, align 8
  %escaped101 = load i64, ptr %i, align 8
  %xor102 = xor i64 %escaped101, -1
  %and103 = and i64 %quote100, %xor102
  %call104 = call i64 @_ZN10JsonStreamB4json10prefix_xorE3u64(i64 %and103)
  %load.struct105 = load %_Z10JsonStream, ptr %3, align 8
  %string_carry = extractvalue %_Z10JsonStream %load.struct105, 4
  %xor106 = xor i64 %call104, %string_carry
  %xor107 = xor i64 %xor106, -1
  %nonscalar108 = load i64, ptr %nonscalar, align 8
  %or109 = or i64 %nonscalar108, %xor106
  %xor110 = xor i64 %or109, -1
  %shl111 = shl i64 %xor110, 1
  %load.struct112 = load %_Z10JsonStream, ptr %3, align 8
  %scalar_carry = extractvalue %_Z10JsonStream %load.struct112, 5
  %or113 = or i64 %shl111, %scalar_carry
  %xor114 = xor i64 %or113, -1
  %and115 = and i64 %xor110, %xor114
  %op116 = load i64, ptr %op, align 8
  %and117 = and i64 %op116, %xor107
  %or118 = or i64 %and103, %and117
  %or119 = or i64 %or118, %and115
  %special120 = load i64, ptr %special, align 8
  %and121 = and i64 %special120, %xor106
  %or122 = or i64 %or119, %and121
  %add123 = add i64 %to, 64
  %escape_carry124 = load i64, ptr %escape_carry80, align 8
  %lshr = lshr i64 %xor106, 63
  %sub125 = sub i64 0, %lshr
  %lshr126 = lshr i64 %xor110, 63
  %tuple.field = getelementptr inbounds nuw %_Z10JsonStream, ptr %tuple, i32 0, i32 0
  store i64 %or122, ptr %tuple.field, align 1
  %tuple.field127 = getelementptr inbounds nuw %_Z10JsonStream, ptr %tuple, i32 0, i32 1
  store i64 %to, ptr %tuple.field127, align 1
  %tuple.field128 = getelementptr inbounds nuw %_Z10JsonStream, ptr %tuple, i32 0, i32 2
  store i64 %add123, ptr %tuple.field128, align 1
  %tuple.field129 = getelementptr inbounds nuw %_Z10JsonStream, ptr %tuple, i32 0, i32 3
  store i64 %escape_carry124, ptr %tuple.field129, align 1
  %tuple.field130 = getelementptr inbounds nuw %_Z10JsonStream, ptr %tuple, i32 0, i32 4
  store i64 %sub125, ptr %tuple.field130, align 1
  %tuple.field131 = getelementptr inbounds nuw %_Z10JsonStream, ptr %tuple, i32 0, i32 5
  store i64 %lshr126, ptr %tuple.field131, align 1
  %tuple.val = load %_Z10JsonStream, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z10JsonStream, ptr null, i32 1) to i64), i1 false)
  ret void

if.then88:                                        ; preds = %while.body83
  store i64 1, ptr %escape_carry80, align 1
  br label %while.exit84

if.end89:                                         ; preds = %while.body83
  %add90 = add i64 %call87, 1
  %shl91 = shl i64 1, %add90
  %escaped92 = load i64, ptr %i, align 8
  %or93 = or i64 %escaped92, %shl91
  store i64 %or93, ptr %i, align 1
  %b94 = load i64, ptr %b, align 8
  %xor95 = xor i64 %shl91, -1
  %and96 = and i64 %b94, %xor95
  store i64 %and96, ptr %b, align 1
  %b97 = load i64, ptr %b, align 8
  %b98 = load i64, ptr %b, align 8
  %sub = sub i64 %b98, 1
  %and99 = and i64 %b97, %sub
  store i64 %and99, ptr %b, align 1
  br label %while.cond82
}

define linkonce_odr i64 @_ZN10JsonStreamB4json4takeE5SliceI2u8E(ptr %0, ptr %1) {
entry:
  %tuple = alloca %_Z10JsonStream, align 8
  %sret.result = alloca %_Z10JsonStream, align 8
  br label %while.cond

while.cond:                                       ; preds = %if.end, %entry
  %load.struct = load %_Z10JsonStream, ptr %0, align 8
  %bits = extractvalue %_Z10JsonStream %load.struct, 0
  %eq = icmp eq i64 %bits, 0
  br i1 %eq, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %load.struct1 = load %_Z10JsonStream, ptr %0, align 8
  %to = extractvalue %_Z10JsonStream %load.struct1, 2
  %load.struct2 = load %_Z5SliceI2u8E, ptr %1, align 8
  %length = extractvalue %_Z5SliceI2u8E %load.struct2, 0
  %ge = icmp uge i64 %to, %length
  br i1 %ge, label %if.then, label %if.end

while.exit:                                       ; preds = %while.cond
  %load.struct36 = load %_Z10JsonStream, ptr %0, align 8
  %base37 = extractvalue %_Z10JsonStream %load.struct36, 1
  %field.inplace = getelementptr inbounds nuw %_Z10JsonStream, ptr %0, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  %call = call i64 @_Z14trailing_zeros3u64(i64 %field.val)
  %add = add i64 %base37, %call
  %load.struct38 = load %_Z10JsonStream, ptr %0, align 8
  %bits39 = extractvalue %_Z10JsonStream %load.struct38, 0
  %load.struct40 = load %_Z10JsonStream, ptr %0, align 8
  %bits41 = extractvalue %_Z10JsonStream %load.struct40, 0
  %sub = sub i64 %bits41, 1
  %and = and i64 %bits39, %sub
  %bits42 = getelementptr inbounds nuw %_Z10JsonStream, ptr %0, i32 0, i32 0
  store i64 %and, ptr %bits42, align 8
  ret i64 %add

if.then:                                          ; preds = %while.body
  %load.struct3 = load %_Z5SliceI2u8E, ptr %1, align 8
  %length4 = extractvalue %_Z5SliceI2u8E %load.struct3, 0
  ret i64 %length4

if.end:                                           ; preds = %while.body
  %load.struct5 = load %_Z10JsonStream, ptr %0, align 8
  %bits6 = extractvalue %_Z10JsonStream %load.struct5, 0
  %load.struct7 = load %_Z10JsonStream, ptr %0, align 8
  %base = extractvalue %_Z10JsonStream %load.struct7, 1
  %load.struct8 = load %_Z10JsonStream, ptr %0, align 8
  %to9 = extractvalue %_Z10JsonStream %load.struct8, 2
  %load.struct10 = load %_Z10JsonStream, ptr %0, align 8
  %escape_carry = extractvalue %_Z10JsonStream %load.struct10, 3
  %load.struct11 = load %_Z10JsonStream, ptr %0, align 8
  %string_carry = extractvalue %_Z10JsonStream %load.struct11, 4
  %load.struct12 = load %_Z10JsonStream, ptr %0, align 8
  %scalar_carry = extractvalue %_Z10JsonStream %load.struct12, 5
  %tuple.field = getelementptr inbounds nuw %_Z10JsonStream, ptr %tuple, i32 0, i32 0
  store i64 %bits6, ptr %tuple.field, align 1
  %tuple.field13 = getelementptr inbounds nuw %_Z10JsonStream, ptr %tuple, i32 0, i32 1
  store i64 %base, ptr %tuple.field13, align 1
  %tuple.field14 = getelementptr inbounds nuw %_Z10JsonStream, ptr %tuple, i32 0, i32 2
  store i64 %to9, ptr %tuple.field14, align 1
  %tuple.field15 = getelementptr inbounds nuw %_Z10JsonStream, ptr %tuple, i32 0, i32 3
  store i64 %escape_carry, ptr %tuple.field15, align 1
  %tuple.field16 = getelementptr inbounds nuw %_Z10JsonStream, ptr %tuple, i32 0, i32 4
  store i64 %string_carry, ptr %tuple.field16, align 1
  %tuple.field17 = getelementptr inbounds nuw %_Z10JsonStream, ptr %tuple, i32 0, i32 5
  store i64 %scalar_carry, ptr %tuple.field17, align 1
  call void @_ZN10JsonStreamB4json8classifyEPN4scaly6memory4PageE5SliceI2u8E10JsonStreamB4json(ptr noalias sret(%_Z10JsonStream) %sret.result, ptr null, ptr %1, ptr %tuple)
  %load.struct18 = load %_Z10JsonStream, ptr %sret.result, align 8
  %bits19 = extractvalue %_Z10JsonStream %load.struct18, 0
  %bits20 = getelementptr inbounds nuw %_Z10JsonStream, ptr %0, i32 0, i32 0
  store i64 %bits19, ptr %bits20, align 8
  %load.struct21 = load %_Z10JsonStream, ptr %sret.result, align 8
  %base22 = extractvalue %_Z10JsonStream %load.struct21, 1
  %base23 = getelementptr inbounds nuw %_Z10JsonStream, ptr %0, i32 0, i32 1
  store i64 %base22, ptr %base23, align 8
  %load.struct24 = load %_Z10JsonStream, ptr %sret.result, align 8
  %to25 = extractvalue %_Z10JsonStream %load.struct24, 2
  %to26 = getelementptr inbounds nuw %_Z10JsonStream, ptr %0, i32 0, i32 2
  store i64 %to25, ptr %to26, align 8
  %load.struct27 = load %_Z10JsonStream, ptr %sret.result, align 8
  %escape_carry28 = extractvalue %_Z10JsonStream %load.struct27, 3
  %escape_carry29 = getelementptr inbounds nuw %_Z10JsonStream, ptr %0, i32 0, i32 3
  store i64 %escape_carry28, ptr %escape_carry29, align 8
  %load.struct30 = load %_Z10JsonStream, ptr %sret.result, align 8
  %string_carry31 = extractvalue %_Z10JsonStream %load.struct30, 4
  %string_carry32 = getelementptr inbounds nuw %_Z10JsonStream, ptr %0, i32 0, i32 4
  store i64 %string_carry31, ptr %string_carry32, align 8
  %load.struct33 = load %_Z10JsonStream, ptr %sret.result, align 8
  %scalar_carry34 = extractvalue %_Z10JsonStream %load.struct33, 5
  %scalar_carry35 = getelementptr inbounds nuw %_Z10JsonStream, ptr %0, i32 0, i32 5
  store i64 %scalar_carry34, ptr %scalar_carry35, align 8
  br label %while.cond
}

define linkonce_odr <32 x i8> @_ZN10JsonStreamB4json7classesE5u8x32(<32 x i8> %0) {
entry:
  %simd.and = and <32 x i8> %0, splat (i8 15)
  %simd.shuffle = shufflevector <32 x i8> %simd.and, <32 x i8> %simd.and, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7, i32 8, i32 9, i32 10, i32 11, i32 12, i32 13, i32 14, i32 15>
  %simd.lookup.hits = icmp eq <16 x i8> %simd.shuffle, zeroinitializer
  %simd.lookup = select <16 x i1> %simd.lookup.hits, <16 x i8> splat (i8 48), <16 x i8> zeroinitializer
  %simd.lookup.hits1 = icmp eq <16 x i8> %simd.shuffle, splat (i8 1)
  %simd.lookup2 = select <16 x i1> %simd.lookup.hits1, <16 x i8> splat (i8 32), <16 x i8> %simd.lookup
  %simd.lookup.hits3 = icmp eq <16 x i8> %simd.shuffle, splat (i8 2)
  %simd.lookup4 = select <16 x i1> %simd.lookup.hits3, <16 x i8> splat (i8 -96), <16 x i8> %simd.lookup2
  %simd.lookup.hits5 = icmp eq <16 x i8> %simd.shuffle, splat (i8 3)
  %simd.lookup6 = select <16 x i1> %simd.lookup.hits5, <16 x i8> splat (i8 32), <16 x i8> %simd.lookup4
  %simd.lookup.hits7 = icmp eq <16 x i8> %simd.shuffle, splat (i8 4)
  %simd.lookup8 = select <16 x i1> %simd.lookup.hits7, <16 x i8> splat (i8 32), <16 x i8> %simd.lookup6
  %simd.lookup.hits9 = icmp eq <16 x i8> %simd.shuffle, splat (i8 5)
  %simd.lookup10 = select <16 x i1> %simd.lookup.hits9, <16 x i8> splat (i8 32), <16 x i8> %simd.lookup8
  %simd.lookup.hits11 = icmp eq <16 x i8> %simd.shuffle, splat (i8 6)
  %simd.lookup12 = select <16 x i1> %simd.lookup.hits11, <16 x i8> splat (i8 32), <16 x i8> %simd.lookup10
  %simd.lookup.hits13 = icmp eq <16 x i8> %simd.shuffle, splat (i8 7)
  %simd.lookup14 = select <16 x i1> %simd.lookup.hits13, <16 x i8> splat (i8 32), <16 x i8> %simd.lookup12
  %simd.lookup.hits15 = icmp eq <16 x i8> %simd.shuffle, splat (i8 8)
  %simd.lookup16 = select <16 x i1> %simd.lookup.hits15, <16 x i8> splat (i8 32), <16 x i8> %simd.lookup14
  %simd.lookup.hits17 = icmp eq <16 x i8> %simd.shuffle, splat (i8 9)
  %simd.lookup18 = select <16 x i1> %simd.lookup.hits17, <16 x i8> splat (i8 40), <16 x i8> %simd.lookup16
  %simd.lookup.hits19 = icmp eq <16 x i8> %simd.shuffle, splat (i8 10)
  %simd.lookup20 = select <16 x i1> %simd.lookup.hits19, <16 x i8> splat (i8 44), <16 x i8> %simd.lookup18
  %simd.lookup.hits21 = icmp eq <16 x i8> %simd.shuffle, splat (i8 11)
  %simd.lookup22 = select <16 x i1> %simd.lookup.hits21, <16 x i8> splat (i8 33), <16 x i8> %simd.lookup20
  %simd.lookup.hits23 = icmp eq <16 x i8> %simd.shuffle, splat (i8 12)
  %simd.lookup24 = select <16 x i1> %simd.lookup.hits23, <16 x i8> splat (i8 98), <16 x i8> %simd.lookup22
  %simd.lookup.hits25 = icmp eq <16 x i8> %simd.shuffle, splat (i8 13)
  %simd.lookup26 = select <16 x i1> %simd.lookup.hits25, <16 x i8> splat (i8 41), <16 x i8> %simd.lookup24
  %simd.lookup.hits27 = icmp eq <16 x i8> %simd.shuffle, splat (i8 14)
  %simd.lookup28 = select <16 x i1> %simd.lookup.hits27, <16 x i8> splat (i8 32), <16 x i8> %simd.lookup26
  %simd.lookup.hits29 = icmp eq <16 x i8> %simd.shuffle, splat (i8 15)
  %simd.lookup30 = select <16 x i1> %simd.lookup.hits29, <16 x i8> splat (i8 32), <16 x i8> %simd.lookup28
  %simd.shuffle31 = shufflevector <32 x i8> %simd.and, <32 x i8> %simd.and, <16 x i32> <i32 16, i32 17, i32 18, i32 19, i32 20, i32 21, i32 22, i32 23, i32 24, i32 25, i32 26, i32 27, i32 28, i32 29, i32 30, i32 31>
  %simd.lookup.hits32 = icmp eq <16 x i8> %simd.shuffle31, zeroinitializer
  %simd.lookup33 = select <16 x i1> %simd.lookup.hits32, <16 x i8> splat (i8 48), <16 x i8> zeroinitializer
  %simd.lookup.hits34 = icmp eq <16 x i8> %simd.shuffle31, splat (i8 1)
  %simd.lookup35 = select <16 x i1> %simd.lookup.hits34, <16 x i8> splat (i8 32), <16 x i8> %simd.lookup33
  %simd.lookup.hits36 = icmp eq <16 x i8> %simd.shuffle31, splat (i8 2)
  %simd.lookup37 = select <16 x i1> %simd.lookup.hits36, <16 x i8> splat (i8 -96), <16 x i8> %simd.lookup35
  %simd.lookup.hits38 = icmp eq <16 x i8> %simd.shuffle31, splat (i8 3)
  %simd.lookup39 = select <16 x i1> %simd.lookup.hits38, <16 x i8> splat (i8 32), <16 x i8> %simd.lookup37
  %simd.lookup.hits40 = icmp eq <16 x i8> %simd.shuffle31, splat (i8 4)
  %simd.lookup41 = select <16 x i1> %simd.lookup.hits40, <16 x i8> splat (i8 32), <16 x i8> %simd.lookup39
  %simd.lookup.hits42 = icmp eq <16 x i8> %simd.shuffle31, splat (i8 5)
  %simd.lookup43 = select <16 x i1> %simd.lookup.hits42, <16 x i8> splat (i8 32), <16 x i8> %simd.lookup41
  %simd.lookup.hits44 = icmp eq <16 x i8> %simd.shuffle31, splat (i8 6)
  %simd.lookup45 = select <16 x i1> %simd.lookup.hits44, <16 x i8> splat (i8 32), <16 x i8> %simd.lookup43
  %simd.lookup.hits46 = icmp eq <16 x i8> %simd.shuffle31, splat (i8 7)
  %simd.lookup47 = select <16 x i1> %simd.lookup.hits46, <16 x i8> splat (i8 32), <16 x i8> %simd.lookup45
  %simd.lookup.hits48 = icmp eq <16 x i8> %simd.shuffle31, splat (i8 8)
  %simd.lookup49 = select <16 x i1> %simd.lookup.hits48, <16 x i8> splat (i8 32), <16 x i8> %simd.lookup47
  %simd.lookup.hits50 = icmp eq <16 x i8> %simd.shuffle31, splat (i8 9)
  %simd.lookup51 = select <16 x i1> %simd.lookup.hits50, <16 x i8> splat (i8 40), <16 x i8> %simd.lookup49
  %simd.lookup.hits52 = icmp eq <16 x i8> %simd.shuffle31, splat (i8 10)
  %simd.lookup53 = select <16 x i1> %simd.lookup.hits52, <16 x i8> splat (i8 44), <16 x i8> %simd.lookup51
  %simd.lookup.hits54 = icmp eq <16 x i8> %simd.shuffle31, splat (i8 11)
  %simd.lookup55 = select <16 x i1> %simd.lookup.hits54, <16 x i8> splat (i8 33), <16 x i8> %simd.lookup53
  %simd.lookup.hits56 = icmp eq <16 x i8> %simd.shuffle31, splat (i8 12)
  %simd.lookup57 = select <16 x i1> %simd.lookup.hits56, <16 x i8> splat (i8 98), <16 x i8> %simd.lookup55
  %simd.lookup.hits58 = icmp eq <16 x i8> %simd.shuffle31, splat (i8 13)
  %simd.lookup59 = select <16 x i1> %simd.lookup.hits58, <16 x i8> splat (i8 41), <16 x i8> %simd.lookup57
  %simd.lookup.hits60 = icmp eq <16 x i8> %simd.shuffle31, splat (i8 14)
  %simd.lookup61 = select <16 x i1> %simd.lookup.hits60, <16 x i8> splat (i8 32), <16 x i8> %simd.lookup59
  %simd.lookup.hits62 = icmp eq <16 x i8> %simd.shuffle31, splat (i8 15)
  %simd.lookup63 = select <16 x i1> %simd.lookup.hits62, <16 x i8> splat (i8 32), <16 x i8> %simd.lookup61
  %simd.shuffle64 = shufflevector <16 x i8> %simd.lookup30, <16 x i8> %simd.lookup63, <32 x i32> <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7, i32 8, i32 9, i32 10, i32 11, i32 12, i32 13, i32 14, i32 15, i32 16, i32 17, i32 18, i32 19, i32 20, i32 21, i32 22, i32 23, i32 24, i32 25, i32 26, i32 27, i32 28, i32 29, i32 30, i32 31>
  %simd.shr = lshr <32 x i8> %0, splat (i8 4)
  %simd.shuffle65 = shufflevector <32 x i8> %simd.shr, <32 x i8> %simd.shr, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7, i32 8, i32 9, i32 10, i32 11, i32 12, i32 13, i32 14, i32 15>
  %simd.lookup.hits66 = icmp eq <16 x i8> %simd.shuffle65, zeroinitializer
  %simd.lookup67 = select <16 x i1> %simd.lookup.hits66, <16 x i8> splat (i8 40), <16 x i8> zeroinitializer
  %simd.lookup.hits68 = icmp eq <16 x i8> %simd.shuffle65, splat (i8 1)
  %simd.lookup69 = select <16 x i1> %simd.lookup.hits68, <16 x i8> splat (i8 32), <16 x i8> %simd.lookup67
  %simd.lookup.hits70 = icmp eq <16 x i8> %simd.shuffle65, splat (i8 2)
  %simd.lookup71 = select <16 x i1> %simd.lookup.hits70, <16 x i8> splat (i8 -110), <16 x i8> %simd.lookup69
  %simd.lookup.hits72 = icmp eq <16 x i8> %simd.shuffle65, splat (i8 3)
  %simd.lookup73 = select <16 x i1> %simd.lookup.hits72, <16 x i8> splat (i8 4), <16 x i8> %simd.lookup71
  %simd.lookup.hits74 = icmp eq <16 x i8> %simd.shuffle65, splat (i8 4)
  %simd.lookup75 = select <16 x i1> %simd.lookup.hits74, <16 x i8> zeroinitializer, <16 x i8> %simd.lookup73
  %simd.lookup.hits76 = icmp eq <16 x i8> %simd.shuffle65, splat (i8 5)
  %simd.lookup77 = select <16 x i1> %simd.lookup.hits76, <16 x i8> splat (i8 65), <16 x i8> %simd.lookup75
  %simd.lookup.hits78 = icmp eq <16 x i8> %simd.shuffle65, splat (i8 6)
  %simd.lookup79 = select <16 x i1> %simd.lookup.hits78, <16 x i8> zeroinitializer, <16 x i8> %simd.lookup77
  %simd.lookup.hits80 = icmp eq <16 x i8> %simd.shuffle65, splat (i8 7)
  %simd.lookup81 = select <16 x i1> %simd.lookup.hits80, <16 x i8> splat (i8 1), <16 x i8> %simd.lookup79
  %simd.lookup.hits82 = icmp eq <16 x i8> %simd.shuffle65, splat (i8 8)
  %simd.lookup83 = select <16 x i1> %simd.lookup.hits82, <16 x i8> zeroinitializer, <16 x i8> %simd.lookup81
  %simd.lookup.hits84 = icmp eq <16 x i8> %simd.shuffle65, splat (i8 9)
  %simd.lookup85 = select <16 x i1> %simd.lookup.hits84, <16 x i8> zeroinitializer, <16 x i8> %simd.lookup83
  %simd.lookup.hits86 = icmp eq <16 x i8> %simd.shuffle65, splat (i8 10)
  %simd.lookup87 = select <16 x i1> %simd.lookup.hits86, <16 x i8> zeroinitializer, <16 x i8> %simd.lookup85
  %simd.lookup.hits88 = icmp eq <16 x i8> %simd.shuffle65, splat (i8 11)
  %simd.lookup89 = select <16 x i1> %simd.lookup.hits88, <16 x i8> zeroinitializer, <16 x i8> %simd.lookup87
  %simd.lookup.hits90 = icmp eq <16 x i8> %simd.shuffle65, splat (i8 12)
  %simd.lookup91 = select <16 x i1> %simd.lookup.hits90, <16 x i8> zeroinitializer, <16 x i8> %simd.lookup89
  %simd.lookup.hits92 = icmp eq <16 x i8> %simd.shuffle65, splat (i8 13)
  %simd.lookup93 = select <16 x i1> %simd.lookup.hits92, <16 x i8> zeroinitializer, <16 x i8> %simd.lookup91
  %simd.lookup.hits94 = icmp eq <16 x i8> %simd.shuffle65, splat (i8 14)
  %simd.lookup95 = select <16 x i1> %simd.lookup.hits94, <16 x i8> zeroinitializer, <16 x i8> %simd.lookup93
  %simd.lookup.hits96 = icmp eq <16 x i8> %simd.shuffle65, splat (i8 15)
  %simd.lookup97 = select <16 x i1> %simd.lookup.hits96, <16 x i8> zeroinitializer, <16 x i8> %simd.lookup95
  %simd.shuffle98 = shufflevector <32 x i8> %simd.shr, <32 x i8> %simd.shr, <16 x i32> <i32 16, i32 17, i32 18, i32 19, i32 20, i32 21, i32 22, i32 23, i32 24, i32 25, i32 26, i32 27, i32 28, i32 29, i32 30, i32 31>
  %simd.lookup.hits99 = icmp eq <16 x i8> %simd.shuffle98, zeroinitializer
  %simd.lookup100 = select <16 x i1> %simd.lookup.hits99, <16 x i8> splat (i8 40), <16 x i8> zeroinitializer
  %simd.lookup.hits101 = icmp eq <16 x i8> %simd.shuffle98, splat (i8 1)
  %simd.lookup102 = select <16 x i1> %simd.lookup.hits101, <16 x i8> splat (i8 32), <16 x i8> %simd.lookup100
  %simd.lookup.hits103 = icmp eq <16 x i8> %simd.shuffle98, splat (i8 2)
  %simd.lookup104 = select <16 x i1> %simd.lookup.hits103, <16 x i8> splat (i8 -110), <16 x i8> %simd.lookup102
  %simd.lookup.hits105 = icmp eq <16 x i8> %simd.shuffle98, splat (i8 3)
  %simd.lookup106 = select <16 x i1> %simd.lookup.hits105, <16 x i8> splat (i8 4), <16 x i8> %simd.lookup104
  %simd.lookup.hits107 = icmp eq <16 x i8> %simd.shuffle98, splat (i8 4)
  %simd.lookup108 = select <16 x i1> %simd.lookup.hits107, <16 x i8> zeroinitializer, <16 x i8> %simd.lookup106
  %simd.lookup.hits109 = icmp eq <16 x i8> %simd.shuffle98, splat (i8 5)
  %simd.lookup110 = select <16 x i1> %simd.lookup.hits109, <16 x i8> splat (i8 65), <16 x i8> %simd.lookup108
  %simd.lookup.hits111 = icmp eq <16 x i8> %simd.shuffle98, splat (i8 6)
  %simd.lookup112 = select <16 x i1> %simd.lookup.hits111, <16 x i8> zeroinitializer, <16 x i8> %simd.lookup110
  %simd.lookup.hits113 = icmp eq <16 x i8> %simd.shuffle98, splat (i8 7)
  %simd.lookup114 = select <16 x i1> %simd.lookup.hits113, <16 x i8> splat (i8 1), <16 x i8> %simd.lookup112
  %simd.lookup.hits115 = icmp eq <16 x i8> %simd.shuffle98, splat (i8 8)
  %simd.lookup116 = select <16 x i1> %simd.lookup.hits115, <16 x i8> zeroinitializer, <16 x i8> %simd.lookup114
  %simd.lookup.hits117 = icmp eq <16 x i8> %simd.shuffle98, splat (i8 9)
  %simd.lookup118 = select <16 x i1> %simd.lookup.hits117, <16 x i8> zeroinitializer, <16 x i8> %simd.lookup116
  %simd.lookup.hits119 = icmp eq <16 x i8> %simd.shuffle98, splat (i8 10)
  %simd.lookup120 = select <16 x i1> %simd.lookup.hits119, <16 x i8> zeroinitializer, <16 x i8> %simd.lookup118
  %simd.lookup.hits121 = icmp eq <16 x i8> %simd.shuffle98, splat (i8 11)
  %simd.lookup122 = select <16 x i1> %simd.lookup.hits121, <16 x i8> zeroinitializer, <16 x i8> %simd.lookup120
  %simd.lookup.hits123 = icmp eq <16 x i8> %simd.shuffle98, splat (i8 12)
  %simd.lookup124 = select <16 x i1> %simd.lookup.hits123, <16 x i8> zeroinitializer, <16 x i8> %simd.lookup122
  %simd.lookup.hits125 = icmp eq <16 x i8> %simd.shuffle98, splat (i8 13)
  %simd.lookup126 = select <16 x i1> %simd.lookup.hits125, <16 x i8> zeroinitializer, <16 x i8> %simd.lookup124
  %simd.lookup.hits127 = icmp eq <16 x i8> %simd.shuffle98, splat (i8 14)
  %simd.lookup128 = select <16 x i1> %simd.lookup.hits127, <16 x i8> zeroinitializer, <16 x i8> %simd.lookup126
  %simd.lookup.hits129 = icmp eq <16 x i8> %simd.shuffle98, splat (i8 15)
  %simd.lookup130 = select <16 x i1> %simd.lookup.hits129, <16 x i8> zeroinitializer, <16 x i8> %simd.lookup128
  %simd.shuffle131 = shufflevector <16 x i8> %simd.lookup97, <16 x i8> %simd.lookup130, <32 x i32> <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7, i32 8, i32 9, i32 10, i32 11, i32 12, i32 13, i32 14, i32 15, i32 16, i32 17, i32 18, i32 19, i32 20, i32 21, i32 22, i32 23, i32 24, i32 25, i32 26, i32 27, i32 28, i32 29, i32 30, i32 31>
  %simd.and132 = and <32 x i8> %simd.shuffle64, %simd.shuffle131
  ret <32 x i8> %simd.and132
}

define linkonce_odr i64 @_ZN10JsonStreamB4json10class_bitsE5u8x325u8x322u8(<32 x i8> %0, <32 x i8> %1, i8 %2) {
entry:
  %simd.splat = insertelement <32 x i8> undef, i8 %2, i64 0
  %simd.splat1 = insertelement <32 x i8> %simd.splat, i8 %2, i64 1
  %simd.splat2 = insertelement <32 x i8> %simd.splat1, i8 %2, i64 2
  %simd.splat3 = insertelement <32 x i8> %simd.splat2, i8 %2, i64 3
  %simd.splat4 = insertelement <32 x i8> %simd.splat3, i8 %2, i64 4
  %simd.splat5 = insertelement <32 x i8> %simd.splat4, i8 %2, i64 5
  %simd.splat6 = insertelement <32 x i8> %simd.splat5, i8 %2, i64 6
  %simd.splat7 = insertelement <32 x i8> %simd.splat6, i8 %2, i64 7
  %simd.splat8 = insertelement <32 x i8> %simd.splat7, i8 %2, i64 8
  %simd.splat9 = insertelement <32 x i8> %simd.splat8, i8 %2, i64 9
  %simd.splat10 = insertelement <32 x i8> %simd.splat9, i8 %2, i64 10
  %simd.splat11 = insertelement <32 x i8> %simd.splat10, i8 %2, i64 11
  %simd.splat12 = insertelement <32 x i8> %simd.splat11, i8 %2, i64 12
  %simd.splat13 = insertelement <32 x i8> %simd.splat12, i8 %2, i64 13
  %simd.splat14 = insertelement <32 x i8> %simd.splat13, i8 %2, i64 14
  %simd.splat15 = insertelement <32 x i8> %simd.splat14, i8 %2, i64 15
  %simd.splat16 = insertelement <32 x i8> %simd.splat15, i8 %2, i64 16
  %simd.splat17 = insertelement <32 x i8> %simd.splat16, i8 %2, i64 17
  %simd.splat18 = insertelement <32 x i8> %simd.splat17, i8 %2, i64 18
  %simd.splat19 = insertelement <32 x i8> %simd.splat18, i8 %2, i64 19
  %simd.splat20 = insertelement <32 x i8> %simd.splat19, i8 %2, i64 20
  %simd.splat21 = insertelement <32 x i8> %simd.splat20, i8 %2, i64 21
  %simd.splat22 = insertelement <32 x i8> %simd.splat21, i8 %2, i64 22
  %simd.splat23 = insertelement <32 x i8> %simd.splat22, i8 %2, i64 23
  %simd.splat24 = insertelement <32 x i8> %simd.splat23, i8 %2, i64 24
  %simd.splat25 = insertelement <32 x i8> %simd.splat24, i8 %2, i64 25
  %simd.splat26 = insertelement <32 x i8> %simd.splat25, i8 %2, i64 26
  %simd.splat27 = insertelement <32 x i8> %simd.splat26, i8 %2, i64 27
  %simd.splat28 = insertelement <32 x i8> %simd.splat27, i8 %2, i64 28
  %simd.splat29 = insertelement <32 x i8> %simd.splat28, i8 %2, i64 29
  %simd.splat30 = insertelement <32 x i8> %simd.splat29, i8 %2, i64 30
  %simd.splat31 = insertelement <32 x i8> %simd.splat30, i8 %2, i64 31
  %simd.and = and <32 x i8> %0, %simd.splat31
  %simd.cmp = icmp ne <32 x i8> %simd.and, zeroinitializer
  %simd.and32 = and <32 x i8> %1, %simd.splat31
  %simd.cmp33 = icmp ne <32 x i8> %simd.and32, zeroinitializer
  %simd.join.bits = bitcast <32 x i1> %simd.cmp to i32
  %simd.join.wide = zext i32 %simd.join.bits to i64
  %simd.join.bits34 = bitcast <32 x i1> %simd.cmp33 to i32
  %simd.join.wide35 = zext i32 %simd.join.bits34 to i64
  %simd.join.at = shl i64 %simd.join.wide35, 32
  %simd.join = or i64 %simd.join.wide, %simd.join.at
  ret i64 %simd.join
}

define linkonce_odr i8 @_ZN5SliceI2u8EixEm(ptr %0, i64 %1) {
entry:
  %load.struct = load %_Z5SliceI2u8E, ptr %0, align 8
  %length = extractvalue %_Z5SliceI2u8E %load.struct, 0
  %ge = icmp uge i64 %1, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %field.inplace = getelementptr inbounds nuw %_Z5SliceI2u8E, ptr %0, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  call void @_Z15scaly_panic_oobP10const_charmm(ptr @.str.2, i64 %1, i64 %field.val)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %load.struct1 = load %_Z5SliceI2u8E, ptr %0, align 8
  %data = extractvalue %_Z5SliceI2u8E %load.struct1, 1
  %ptr.add = getelementptr inbounds i8, ptr %data, i64 %1
  %deref = load i8, ptr %ptr.add, align 1
  ret i8 %deref
}

define linkonce_odr i1 @_ZN10JsonReaderB4json5is_wsE2u8(i8 %0) {
entry:
  %eq = icmp eq i8 %0, 32
  br i1 %eq, label %lor.end, label %lor.rhs

lor.rhs:                                          ; preds = %entry
  %eq1 = icmp eq i8 %0, 10
  br label %lor.end

lor.end:                                          ; preds = %lor.rhs, %entry
  %lor.result = phi i1 [ true, %entry ], [ %eq1, %lor.rhs ]
  br i1 %lor.result, label %lor.end3, label %lor.rhs2

lor.rhs2:                                         ; preds = %lor.end
  %eq4 = icmp eq i8 %0, 13
  br label %lor.end3

lor.end3:                                         ; preds = %lor.rhs2, %lor.end
  %lor.result5 = phi i1 [ true, %lor.end ], [ %eq4, %lor.rhs2 ]
  br i1 %lor.result5, label %lor.end7, label %lor.rhs6

lor.rhs6:                                         ; preds = %lor.end3
  %eq8 = icmp eq i8 %0, 9
  br label %lor.end7

lor.end7:                                         ; preds = %lor.rhs6, %lor.end3
  %lor.result9 = phi i1 [ true, %lor.end3 ], [ %eq8, %lor.rhs6 ]
  ret i1 %lor.result9
}

define linkonce_odr i64 @_ZN10JsonStreamB4json10prefix_xorE3u64(i64 %0) {
entry:
  %y = alloca i64, align 8
  store i64 %0, ptr %y, align 1
  %y1 = load i64, ptr %y, align 8
  %y2 = load i64, ptr %y, align 8
  %shl = shl i64 %y2, 1
  %xor = xor i64 %y1, %shl
  store i64 %xor, ptr %y, align 1
  %y3 = load i64, ptr %y, align 8
  %y4 = load i64, ptr %y, align 8
  %shl5 = shl i64 %y4, 2
  %xor6 = xor i64 %y3, %shl5
  store i64 %xor6, ptr %y, align 1
  %y7 = load i64, ptr %y, align 8
  %y8 = load i64, ptr %y, align 8
  %shl9 = shl i64 %y8, 4
  %xor10 = xor i64 %y7, %shl9
  store i64 %xor10, ptr %y, align 1
  %y11 = load i64, ptr %y, align 8
  %y12 = load i64, ptr %y, align 8
  %shl13 = shl i64 %y12, 8
  %xor14 = xor i64 %y11, %shl13
  store i64 %xor14, ptr %y, align 1
  %y15 = load i64, ptr %y, align 8
  %y16 = load i64, ptr %y, align 8
  %shl17 = shl i64 %y16, 16
  %xor18 = xor i64 %y15, %shl17
  store i64 %xor18, ptr %y, align 1
  %y19 = load i64, ptr %y, align 8
  %y20 = load i64, ptr %y, align 8
  %shl21 = shl i64 %y20, 32
  %xor22 = xor i64 %y19, %shl21
  ret i64 %xor22
}

define linkonce_odr void @_ZN10JsonReaderB4json6createEPN4scaly6memory4PageE5SliceI2u8E(ptr noalias sret(%_Z10JsonReader) %0, ptr %1, ptr %2) {
entry:
  %tuple = alloca %_Z10JsonReader, align 8
  %field.load = load %_Z5SliceI2u8E, ptr %2, align 8
  %tuple.field = getelementptr inbounds nuw %_Z10JsonReader, ptr %tuple, i32 0, i32 0
  store %_Z5SliceI2u8E %field.load, ptr %tuple.field, align 1
  %tuple.field1 = getelementptr inbounds nuw %_Z10JsonReader, ptr %tuple, i32 0, i32 1
  store i64 256, ptr %tuple.field1, align 1
  %tuple.field2 = getelementptr inbounds nuw %_Z10JsonReader, ptr %tuple, i32 0, i32 2
  store i64 0, ptr %tuple.field2, align 1
  %tuple.field3 = getelementptr inbounds nuw %_Z10JsonReader, ptr %tuple, i32 0, i32 3
  store i64 0, ptr %tuple.field3, align 1
  %tuple.field4 = getelementptr inbounds nuw %_Z10JsonReader, ptr %tuple, i32 0, i32 4
  store i64 0, ptr %tuple.field4, align 1
  %tuple.field5 = getelementptr inbounds nuw %_Z10JsonReader, ptr %tuple, i32 0, i32 5
  store i64 0, ptr %tuple.field5, align 1
  %tuple.field6 = getelementptr inbounds nuw %_Z10JsonReader, ptr %tuple, i32 0, i32 6
  store i64 0, ptr %tuple.field6, align 1
  %tuple.field7 = getelementptr inbounds nuw %_Z10JsonReader, ptr %tuple, i32 0, i32 7
  store i64 0, ptr %tuple.field7, align 1
  %tuple.field8 = getelementptr inbounds nuw %_Z10JsonReader, ptr %tuple, i32 0, i32 8
  store i64 0, ptr %tuple.field8, align 1
  %tuple.field9 = getelementptr inbounds nuw %_Z10JsonReader, ptr %tuple, i32 0, i32 9
  store i64 0, ptr %tuple.field9, align 1
  %tuple.field10 = getelementptr inbounds nuw %_Z10JsonReader, ptr %tuple, i32 0, i32 10
  store i64 0, ptr %tuple.field10, align 1
  %tuple.field11 = getelementptr inbounds nuw %_Z10JsonReader, ptr %tuple, i32 0, i32 11
  store i1 false, ptr %tuple.field11, align 1
  %tuple.field12 = getelementptr inbounds nuw %_Z10JsonReader, ptr %tuple, i32 0, i32 12
  store i1 false, ptr %tuple.field12, align 1
  %tuple.field13 = getelementptr inbounds nuw %_Z10JsonReader, ptr %tuple, i32 0, i32 13
  store i64 0, ptr %tuple.field13, align 1
  %tuple.field14 = getelementptr inbounds nuw %_Z10JsonReader, ptr %tuple, i32 0, i32 14
  store i64 0, ptr %tuple.field14, align 1
  %tuple.val = load %_Z10JsonReader, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z10JsonReader, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN10JsonReaderB4json14create_limitedEPN4scaly6memory4PageE5SliceI2u8Em(ptr noalias sret(%_Z10JsonReader) %0, ptr %1, ptr %2, i64 %3) {
entry:
  %tuple = alloca %_Z10JsonReader, align 8
  %limit = alloca i64, align 8
  store i64 %3, ptr %limit, align 1
  %limit1 = load i64, ptr %limit, align 8
  %gt = icmp ugt i64 %limit1, 256
  br i1 %gt, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  store i64 256, ptr %limit, align 1
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %limit2 = load i64, ptr %limit, align 8
  %field.load = load %_Z5SliceI2u8E, ptr %2, align 8
  %tuple.field = getelementptr inbounds nuw %_Z10JsonReader, ptr %tuple, i32 0, i32 0
  store %_Z5SliceI2u8E %field.load, ptr %tuple.field, align 1
  %tuple.field3 = getelementptr inbounds nuw %_Z10JsonReader, ptr %tuple, i32 0, i32 1
  store i64 %limit2, ptr %tuple.field3, align 1
  %tuple.field4 = getelementptr inbounds nuw %_Z10JsonReader, ptr %tuple, i32 0, i32 2
  store i64 0, ptr %tuple.field4, align 1
  %tuple.field5 = getelementptr inbounds nuw %_Z10JsonReader, ptr %tuple, i32 0, i32 3
  store i64 0, ptr %tuple.field5, align 1
  %tuple.field6 = getelementptr inbounds nuw %_Z10JsonReader, ptr %tuple, i32 0, i32 4
  store i64 0, ptr %tuple.field6, align 1
  %tuple.field7 = getelementptr inbounds nuw %_Z10JsonReader, ptr %tuple, i32 0, i32 5
  store i64 0, ptr %tuple.field7, align 1
  %tuple.field8 = getelementptr inbounds nuw %_Z10JsonReader, ptr %tuple, i32 0, i32 6
  store i64 0, ptr %tuple.field8, align 1
  %tuple.field9 = getelementptr inbounds nuw %_Z10JsonReader, ptr %tuple, i32 0, i32 7
  store i64 0, ptr %tuple.field9, align 1
  %tuple.field10 = getelementptr inbounds nuw %_Z10JsonReader, ptr %tuple, i32 0, i32 8
  store i64 0, ptr %tuple.field10, align 1
  %tuple.field11 = getelementptr inbounds nuw %_Z10JsonReader, ptr %tuple, i32 0, i32 9
  store i64 0, ptr %tuple.field11, align 1
  %tuple.field12 = getelementptr inbounds nuw %_Z10JsonReader, ptr %tuple, i32 0, i32 10
  store i64 0, ptr %tuple.field12, align 1
  %tuple.field13 = getelementptr inbounds nuw %_Z10JsonReader, ptr %tuple, i32 0, i32 11
  store i1 false, ptr %tuple.field13, align 1
  %tuple.field14 = getelementptr inbounds nuw %_Z10JsonReader, ptr %tuple, i32 0, i32 12
  store i1 false, ptr %tuple.field14, align 1
  %tuple.field15 = getelementptr inbounds nuw %_Z10JsonReader, ptr %tuple, i32 0, i32 13
  store i64 0, ptr %tuple.field15, align 1
  %tuple.field16 = getelementptr inbounds nuw %_Z10JsonReader, ptr %tuple, i32 0, i32 14
  store i64 0, ptr %tuple.field16, align 1
  %tuple.val = load %_Z10JsonReader, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z10JsonReader, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN10JsonReaderB4json7skip_wsEv(ptr %0) {
entry:
  %arg.tmp = alloca %_Z5SliceI2u8E, align 8
  %load.struct = load %_Z10JsonReader, ptr %0, align 8
  %source = extractvalue %_Z10JsonReader %load.struct, 0
  %load.struct1 = load %_Z10JsonReader, ptr %0, align 8
  %pos = extractvalue %_Z10JsonReader %load.struct1, 7
  %p = alloca i64, align 8
  store i64 %pos, ptr %p, align 1
  br label %while.cond

while.cond:                                       ; preds = %if.end, %entry
  %p2 = load i64, ptr %p, align 8
  %length = extractvalue %_Z5SliceI2u8E %source, 0
  %lt = icmp ult i64 %p2, %length
  br i1 %lt, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  store %_Z5SliceI2u8E %source, ptr %arg.tmp, align 1
  %p3 = load i64, ptr %p, align 8
  %call = call i8 @_ZN5SliceI2u8EixEm(ptr %arg.tmp, i64 %p3)
  %ne = icmp ne i8 %call, 32
  br i1 %ne, label %land.rhs5, label %if.end

while.exit:                                       ; preds = %if.then, %while.cond
  %p10 = load i64, ptr %p, align 8
  %pos11 = getelementptr inbounds nuw %_Z10JsonReader, ptr %0, i32 0, i32 7
  store i64 %p10, ptr %pos11, align 8
  ret void

if.then:                                          ; preds = %land.rhs
  br label %while.exit

if.end:                                           ; preds = %land.rhs, %land.rhs4, %land.rhs5, %while.body
  %p9 = load i64, ptr %p, align 8
  %add = add i64 %p9, 1
  store i64 %add, ptr %p, align 1
  br label %while.cond

land.rhs:                                         ; preds = %land.rhs4
  %ne8 = icmp ne i8 %call, 9
  br i1 %ne8, label %if.then, label %if.end

land.rhs4:                                        ; preds = %land.rhs5
  %ne7 = icmp ne i8 %call, 13
  br i1 %ne7, label %land.rhs, label %if.end

land.rhs5:                                        ; preds = %while.body
  %ne6 = icmp ne i8 %call, 10
  br i1 %ne6, label %land.rhs4, label %if.end
}

define linkonce_odr void @_ZN10JsonReaderB4json4failEPN4scaly6memory4PageEi(ptr noalias sret(%_Z9JsonToken) %0, ptr %1, ptr %2, i64 %3) {
entry:
  %error = getelementptr inbounds nuw %_Z10JsonReader, ptr %2, i32 0, i32 13
  store i64 %3, ptr %error, align 8
  %load.struct = load %_Z10JsonReader, ptr %2, align 8
  %pos = extractvalue %_Z10JsonReader %load.struct, 7
  %error_at = getelementptr inbounds nuw %_Z10JsonReader, ptr %2, i32 0, i32 14
  store i64 %pos, ptr %error_at, align 8
  %state = getelementptr inbounds nuw %_Z10JsonReader, ptr %2, i32 0, i32 8
  store i64 6, ptr %state, align 8
  %variant.ptr = alloca %_Z9JsonToken, align 8
  %variant.tag.ptr = getelementptr inbounds nuw %_Z9JsonToken, ptr %variant.ptr, i32 0, i32 0
  store i8 1, ptr %variant.tag.ptr, align 1
  %variant.val = load %_Z9JsonToken, ptr %variant.ptr, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %variant.ptr, i64 ptrtoint (ptr getelementptr (%_Z9JsonToken, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr i1 @_ZN10JsonReaderB4json9in_objectEv(ptr %0) {
entry:
  %load.struct = load %_Z10JsonReader, ptr %0, align 8
  %depth = extractvalue %_Z10JsonReader %load.struct, 2
  %sub = sub i64 %depth, 1
  %and = and i64 %sub, 63
  %shl = shl i64 1, %and
  %lshr = lshr i64 %sub, 6
  %eq = icmp eq i64 %lshr, 0
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %load.struct1 = load %_Z10JsonReader, ptr %0, align 8
  %levels0 = extractvalue %_Z10JsonReader %load.struct1, 3
  %and2 = and i64 %levels0, %shl
  %ne = icmp ne i64 %and2, 0
  ret i1 %ne

if.end:                                           ; preds = %entry
  %eq3 = icmp eq i64 %lshr, 1
  br i1 %eq3, label %if.then4, label %if.end5

if.then4:                                         ; preds = %if.end
  %load.struct6 = load %_Z10JsonReader, ptr %0, align 8
  %levels1 = extractvalue %_Z10JsonReader %load.struct6, 4
  %and7 = and i64 %levels1, %shl
  %ne8 = icmp ne i64 %and7, 0
  ret i1 %ne8

if.end5:                                          ; preds = %if.end
  %eq9 = icmp eq i64 %lshr, 2
  br i1 %eq9, label %if.then10, label %if.end11

if.then10:                                        ; preds = %if.end5
  %load.struct12 = load %_Z10JsonReader, ptr %0, align 8
  %levels2 = extractvalue %_Z10JsonReader %load.struct12, 5
  %and13 = and i64 %levels2, %shl
  %ne14 = icmp ne i64 %and13, 0
  ret i1 %ne14

if.end11:                                         ; preds = %if.end5
  %load.struct15 = load %_Z10JsonReader, ptr %0, align 8
  %levels3 = extractvalue %_Z10JsonReader %load.struct15, 6
  %and16 = and i64 %levels3, %shl
  %ne17 = icmp ne i64 %and16, 0
  ret i1 %ne17
}

define linkonce_odr void @_ZN10JsonReaderB4json5closeEPN4scaly6memory4PageE9JsonTokenB4json(ptr noalias sret(%_Z9JsonToken) %0, ptr %1, ptr %2, ptr %3) {
entry:
  %load.struct = load %_Z10JsonReader, ptr %2, align 8
  %depth = extractvalue %_Z10JsonReader %load.struct, 2
  %sub = sub i64 %depth, 1
  %depth1 = getelementptr inbounds nuw %_Z10JsonReader, ptr %2, i32 0, i32 2
  store i64 %sub, ptr %depth1, align 8
  %load.struct2 = load %_Z10JsonReader, ptr %2, align 8
  %pos = extractvalue %_Z10JsonReader %load.struct2, 7
  %add = add i64 %pos, 1
  %pos3 = getelementptr inbounds nuw %_Z10JsonReader, ptr %2, i32 0, i32 7
  store i64 %add, ptr %pos3, align 8
  call void @_ZN10JsonReaderB4json11after_valueEv(ptr %2)
  %sret.body = load %_Z9JsonToken, ptr %3, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %3, i64 ptrtoint (ptr getelementptr (%_Z9JsonToken, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr i1 @_ZN10JsonReaderB4json11scan_stringEv(ptr %0) {
entry:
  %sret.result3 = alloca %_Z9JsonToken, align 8
  %sret.result = alloca %_Z8JsonScan, align 8
  %field.inplace = getelementptr inbounds nuw %_Z10JsonReader, ptr %0, i32 0, i32 0
  %field.inplace1 = getelementptr inbounds nuw %_Z10JsonReader, ptr %0, i32 0, i32 7
  %field.val = load i64, ptr %field.inplace1, align 8
  call void @_ZN10JsonReaderB4json12string_closeEPN4scaly6memory4PageE5SliceI2u8Em(ptr noalias sret(%_Z8JsonScan) %sret.result, ptr null, ptr %field.inplace, i64 %field.val)
  %load.struct = load %_Z8JsonScan, ptr %sret.result, align 8
  %code = extractvalue %_Z8JsonScan %load.struct, 0
  %ne = icmp ne i64 %code, 0
  br i1 %ne, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %load.struct2 = load %_Z8JsonScan, ptr %sret.result, align 8
  %at = extractvalue %_Z8JsonScan %load.struct2, 1
  %pos = getelementptr inbounds nuw %_Z10JsonReader, ptr %0, i32 0, i32 7
  store i64 %at, ptr %pos, align 8
  %field.inplace4 = getelementptr inbounds nuw %_Z8JsonScan, ptr %sret.result, i32 0, i32 0
  %field.val5 = load i64, ptr %field.inplace4, align 8
  call void @_ZN10JsonReaderB4json4failEPN4scaly6memory4PageEi(ptr noalias sret(%_Z9JsonToken) %sret.result3, ptr null, ptr %0, i64 %field.val5)
  ret i1 false

if.end:                                           ; preds = %entry
  %load.struct6 = load %_Z10JsonReader, ptr %0, align 8
  %pos7 = extractvalue %_Z10JsonReader %load.struct6, 7
  %add = add i64 %pos7, 1
  %start = getelementptr inbounds nuw %_Z10JsonReader, ptr %0, i32 0, i32 9
  store i64 %add, ptr %start, align 8
  %load.struct8 = load %_Z8JsonScan, ptr %sret.result, align 8
  %at9 = extractvalue %_Z8JsonScan %load.struct8, 1
  %end = getelementptr inbounds nuw %_Z10JsonReader, ptr %0, i32 0, i32 10
  store i64 %at9, ptr %end, align 8
  %load.struct10 = load %_Z8JsonScan, ptr %sret.result, align 8
  %flag = extractvalue %_Z8JsonScan %load.struct10, 2
  %escaped = getelementptr inbounds nuw %_Z10JsonReader, ptr %0, i32 0, i32 11
  store i1 %flag, ptr %escaped, align 1
  %load.struct11 = load %_Z8JsonScan, ptr %sret.result, align 8
  %at12 = extractvalue %_Z8JsonScan %load.struct11, 1
  %add13 = add i64 %at12, 1
  %pos14 = getelementptr inbounds nuw %_Z10JsonReader, ptr %0, i32 0, i32 7
  store i64 %add13, ptr %pos14, align 8
  ret i1 true
}

define linkonce_odr void @_ZN10JsonReaderB4json5valueEPN4scaly6memory4PageE2u8(ptr noalias sret(%_Z9JsonToken) %0, ptr %1, ptr %2, i8 %3) {
entry:
  %arg.tmp65 = alloca %_Z5SliceI2u8E, align 8
  %tuple61 = alloca %_Z5SliceI2u8E, align 8
  %arg.tmp55 = alloca %_Z5SliceI2u8E, align 8
  %tuple51 = alloca %_Z5SliceI2u8E, align 8
  %variant.ptr48 = alloca %_Z9JsonToken, align 8
  %arg.tmp = alloca %_Z5SliceI2u8E, align 8
  %tuple = alloca %_Z5SliceI2u8E, align 8
  %variant.ptr39 = alloca %_Z9JsonToken, align 8
  %variant.ptr26 = alloca %_Z9JsonToken, align 8
  %variant.ptr13 = alloca %_Z9JsonToken, align 8
  %variant.ptr = alloca %_Z9JsonToken, align 8
  %eq = icmp eq i8 %3, 123
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %call = call i1 @_ZN10JsonReaderB4json4openEb(ptr %2, i1 true)
  %eq1 = icmp eq i1 %call, false
  br i1 %eq1, label %if.then2, label %if.end3

if.end:                                           ; preds = %entry
  %eq6 = icmp eq i8 %3, 91
  br i1 %eq6, label %if.then7, label %if.end8

if.then2:                                         ; preds = %if.then
  %variant.tag.ptr = getelementptr inbounds nuw %_Z9JsonToken, ptr %variant.ptr, i32 0, i32 0
  store i8 1, ptr %variant.tag.ptr, align 1
  %variant.val = load %_Z9JsonToken, ptr %variant.ptr, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %variant.ptr, i64 ptrtoint (ptr getelementptr (%_Z9JsonToken, ptr null, i32 1) to i64), i1 false)
  ret void

if.end3:                                          ; preds = %if.then
  %state = getelementptr inbounds nuw %_Z10JsonReader, ptr %2, i32 0, i32 8
  store i64 3, ptr %state, align 8
  %variant.tag.ptr4 = getelementptr inbounds nuw %_Z9JsonToken, ptr %variant.ptr, i32 0, i32 0
  store i8 2, ptr %variant.tag.ptr4, align 1
  %variant.val5 = load %_Z9JsonToken, ptr %variant.ptr, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %variant.ptr, i64 ptrtoint (ptr getelementptr (%_Z9JsonToken, ptr null, i32 1) to i64), i1 false)
  ret void

if.then7:                                         ; preds = %if.end
  %call9 = call i1 @_ZN10JsonReaderB4json4openEb(ptr %2, i1 false)
  %eq10 = icmp eq i1 %call9, false
  br i1 %eq10, label %if.then11, label %if.end12

if.end8:                                          ; preds = %if.end
  %eq19 = icmp eq i8 %3, 34
  br i1 %eq19, label %if.then20, label %if.end21

if.then11:                                        ; preds = %if.then7
  %variant.tag.ptr14 = getelementptr inbounds nuw %_Z9JsonToken, ptr %variant.ptr13, i32 0, i32 0
  store i8 1, ptr %variant.tag.ptr14, align 1
  %variant.val15 = load %_Z9JsonToken, ptr %variant.ptr13, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %variant.ptr13, i64 ptrtoint (ptr getelementptr (%_Z9JsonToken, ptr null, i32 1) to i64), i1 false)
  ret void

if.end12:                                         ; preds = %if.then7
  %state16 = getelementptr inbounds nuw %_Z10JsonReader, ptr %2, i32 0, i32 8
  store i64 1, ptr %state16, align 8
  %variant.tag.ptr17 = getelementptr inbounds nuw %_Z9JsonToken, ptr %variant.ptr13, i32 0, i32 0
  store i8 4, ptr %variant.tag.ptr17, align 1
  %variant.val18 = load %_Z9JsonToken, ptr %variant.ptr13, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %variant.ptr13, i64 ptrtoint (ptr getelementptr (%_Z9JsonToken, ptr null, i32 1) to i64), i1 false)
  ret void

if.then20:                                        ; preds = %if.end8
  %call22 = call i1 @_ZN10JsonReaderB4json11scan_stringEv(ptr %2)
  %eq23 = icmp eq i1 %call22, false
  br i1 %eq23, label %if.then24, label %if.end25

if.end21:                                         ; preds = %if.end8
  %eq33 = icmp eq i8 %3, 45
  br i1 %eq33, label %if.then31, label %lor.rhs

if.then24:                                        ; preds = %if.then20
  %variant.tag.ptr27 = getelementptr inbounds nuw %_Z9JsonToken, ptr %variant.ptr26, i32 0, i32 0
  store i8 1, ptr %variant.tag.ptr27, align 1
  %variant.val28 = load %_Z9JsonToken, ptr %variant.ptr26, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %variant.ptr26, i64 ptrtoint (ptr getelementptr (%_Z9JsonToken, ptr null, i32 1) to i64), i1 false)
  ret void

if.end25:                                         ; preds = %if.then20
  call void @_ZN10JsonReaderB4json11after_valueEv(ptr %2)
  %variant.tag.ptr29 = getelementptr inbounds nuw %_Z9JsonToken, ptr %variant.ptr26, i32 0, i32 0
  store i8 7, ptr %variant.tag.ptr29, align 1
  %variant.val30 = load %_Z9JsonToken, ptr %variant.ptr26, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %variant.ptr26, i64 ptrtoint (ptr getelementptr (%_Z9JsonToken, ptr null, i32 1) to i64), i1 false)
  ret void

if.then31:                                        ; preds = %lor.end, %if.end21
  %call35 = call i1 @_ZN10JsonReaderB4json11scan_numberEv(ptr %2)
  %eq36 = icmp eq i1 %call35, false
  br i1 %eq36, label %if.then37, label %if.end38

if.end32:                                         ; preds = %lor.end
  %tuple.field = getelementptr inbounds nuw %_Z5SliceI2u8E, ptr %tuple, i32 0, i32 0
  store i64 4, ptr %tuple.field, align 1
  %tuple.field44 = getelementptr inbounds nuw %_Z5SliceI2u8E, ptr %tuple, i32 0, i32 1
  store ptr @.str.18, ptr %tuple.field44, align 1
  %tuple.val = load %_Z5SliceI2u8E, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %arg.tmp, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z5SliceI2u8E, ptr null, i32 1) to i64), i1 false)
  %call45 = call i1 @_ZN10JsonReaderB4json7literalE5SliceI2u8E(ptr %2, ptr %arg.tmp)
  br i1 %call45, label %if.then46, label %if.end47

lor.rhs:                                          ; preds = %if.end21
  %ge = icmp uge i8 %3, 48
  br i1 %ge, label %lor.rhs34, label %lor.end

lor.rhs34:                                        ; preds = %lor.rhs
  %le = icmp ule i8 %3, 57
  br label %lor.end

lor.end:                                          ; preds = %lor.rhs34, %lor.rhs
  %lor.result = phi i1 [ false, %lor.rhs ], [ %le, %lor.rhs34 ]
  br i1 %lor.result, label %if.then31, label %if.end32

if.then37:                                        ; preds = %if.then31
  %variant.tag.ptr40 = getelementptr inbounds nuw %_Z9JsonToken, ptr %variant.ptr39, i32 0, i32 0
  store i8 1, ptr %variant.tag.ptr40, align 1
  %variant.val41 = load %_Z9JsonToken, ptr %variant.ptr39, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %variant.ptr39, i64 ptrtoint (ptr getelementptr (%_Z9JsonToken, ptr null, i32 1) to i64), i1 false)
  ret void

if.end38:                                         ; preds = %if.then31
  call void @_ZN10JsonReaderB4json11after_valueEv(ptr %2)
  %variant.tag.ptr42 = getelementptr inbounds nuw %_Z9JsonToken, ptr %variant.ptr39, i32 0, i32 0
  store i8 8, ptr %variant.tag.ptr42, align 1
  %variant.val43 = load %_Z9JsonToken, ptr %variant.ptr39, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %variant.ptr39, i64 ptrtoint (ptr getelementptr (%_Z9JsonToken, ptr null, i32 1) to i64), i1 false)
  ret void

if.then46:                                        ; preds = %if.end32
  %variant.tag.ptr49 = getelementptr inbounds nuw %_Z9JsonToken, ptr %variant.ptr48, i32 0, i32 0
  store i8 9, ptr %variant.tag.ptr49, align 1
  %variant.val50 = load %_Z9JsonToken, ptr %variant.ptr48, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %variant.ptr48, i64 ptrtoint (ptr getelementptr (%_Z9JsonToken, ptr null, i32 1) to i64), i1 false)
  ret void

if.end47:                                         ; preds = %if.end32
  %tuple.field52 = getelementptr inbounds nuw %_Z5SliceI2u8E, ptr %tuple51, i32 0, i32 0
  store i64 5, ptr %tuple.field52, align 1
  %tuple.field53 = getelementptr inbounds nuw %_Z5SliceI2u8E, ptr %tuple51, i32 0, i32 1
  store ptr @.str.19, ptr %tuple.field53, align 1
  %tuple.val54 = load %_Z5SliceI2u8E, ptr %tuple51, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %arg.tmp55, ptr align 1 %tuple51, i64 ptrtoint (ptr getelementptr (%_Z5SliceI2u8E, ptr null, i32 1) to i64), i1 false)
  %call56 = call i1 @_ZN10JsonReaderB4json7literalE5SliceI2u8E(ptr %2, ptr %arg.tmp55)
  br i1 %call56, label %if.then57, label %if.end58

if.then57:                                        ; preds = %if.end47
  %variant.tag.ptr59 = getelementptr inbounds nuw %_Z9JsonToken, ptr %variant.ptr48, i32 0, i32 0
  store i8 10, ptr %variant.tag.ptr59, align 1
  %variant.val60 = load %_Z9JsonToken, ptr %variant.ptr48, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %variant.ptr48, i64 ptrtoint (ptr getelementptr (%_Z9JsonToken, ptr null, i32 1) to i64), i1 false)
  ret void

if.end58:                                         ; preds = %if.end47
  %tuple.field62 = getelementptr inbounds nuw %_Z5SliceI2u8E, ptr %tuple61, i32 0, i32 0
  store i64 4, ptr %tuple.field62, align 1
  %tuple.field63 = getelementptr inbounds nuw %_Z5SliceI2u8E, ptr %tuple61, i32 0, i32 1
  store ptr @.str.20, ptr %tuple.field63, align 1
  %tuple.val64 = load %_Z5SliceI2u8E, ptr %tuple61, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %arg.tmp65, ptr align 1 %tuple61, i64 ptrtoint (ptr getelementptr (%_Z5SliceI2u8E, ptr null, i32 1) to i64), i1 false)
  %call66 = call i1 @_ZN10JsonReaderB4json7literalE5SliceI2u8E(ptr %2, ptr %arg.tmp65)
  br i1 %call66, label %if.then67, label %if.end68

if.then67:                                        ; preds = %if.end58
  %variant.tag.ptr69 = getelementptr inbounds nuw %_Z9JsonToken, ptr %variant.ptr48, i32 0, i32 0
  store i8 11, ptr %variant.tag.ptr69, align 1
  %variant.val70 = load %_Z9JsonToken, ptr %variant.ptr48, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %variant.ptr48, i64 ptrtoint (ptr getelementptr (%_Z9JsonToken, ptr null, i32 1) to i64), i1 false)
  ret void

if.end68:                                         ; preds = %if.end58
  call void @_ZN10JsonReaderB4json4failEPN4scaly6memory4PageEi(ptr noalias sret(%_Z9JsonToken) %variant.ptr48, ptr null, ptr %2, i64 3)
  %sret.body = load %_Z9JsonToken, ptr %variant.ptr48, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %variant.ptr48, i64 ptrtoint (ptr getelementptr (%_Z9JsonToken, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN10JsonReaderB4json4nextEPN4scaly6memory4PageE(ptr noalias sret(%_Z9JsonToken) %0, ptr %1, ptr %2) {
entry:
  %arg.tmp68 = alloca %_Z9JsonToken, align 8
  %arg.tmp = alloca %_Z9JsonToken, align 8
  %variant.ptr46 = alloca %_Z9JsonToken, align 8
  %sret.result25 = alloca %_Z9JsonToken, align 8
  %sret.result = alloca %_Z9JsonToken, align 8
  %variant.ptr16 = alloca %_Z9JsonToken, align 8
  %variant.ptr = alloca %_Z9JsonToken, align 8
  br label %repeat.body

repeat.body:                                      ; preds = %if.end40, %entry
  %load.struct = load %_Z10JsonReader, ptr %2, align 8
  %state = extractvalue %_Z10JsonReader %load.struct, 8
  %eq = icmp eq i64 %state, 6
  br i1 %eq, label %if.then, label %if.end

repeat.exit:                                      ; No predecessors!
  ret void

if.then:                                          ; preds = %repeat.body
  %load.struct1 = load %_Z10JsonReader, ptr %2, align 8
  %error = extractvalue %_Z10JsonReader %load.struct1, 13
  %ne = icmp ne i64 %error, 0
  br i1 %ne, label %if.then2, label %if.end3

if.end:                                           ; preds = %repeat.body
  call void @_ZN10JsonReaderB4json7skip_wsEv(ptr %2)
  %load.struct6 = load %_Z10JsonReader, ptr %2, align 8
  %pos = extractvalue %_Z10JsonReader %load.struct6, 7
  %load.struct7 = load %_Z10JsonReader, ptr %2, align 8
  %source = extractvalue %_Z10JsonReader %load.struct7, 0
  %length = extractvalue %_Z5SliceI2u8E %source, 0
  %ge = icmp uge i64 %pos, %length
  br i1 %ge, label %if.then8, label %if.end9

if.then2:                                         ; preds = %if.then
  %variant.tag.ptr = getelementptr inbounds nuw %_Z9JsonToken, ptr %variant.ptr, i32 0, i32 0
  store i8 1, ptr %variant.tag.ptr, align 1
  %variant.val = load %_Z9JsonToken, ptr %variant.ptr, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %variant.ptr, i64 ptrtoint (ptr getelementptr (%_Z9JsonToken, ptr null, i32 1) to i64), i1 false)
  ret void

if.end3:                                          ; preds = %if.then
  %variant.tag.ptr4 = getelementptr inbounds nuw %_Z9JsonToken, ptr %variant.ptr, i32 0, i32 0
  store i8 0, ptr %variant.tag.ptr4, align 1
  %variant.val5 = load %_Z9JsonToken, ptr %variant.ptr, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %variant.ptr, i64 ptrtoint (ptr getelementptr (%_Z9JsonToken, ptr null, i32 1) to i64), i1 false)
  ret void

if.then8:                                         ; preds = %if.end
  %load.struct10 = load %_Z10JsonReader, ptr %2, align 8
  %state11 = extractvalue %_Z10JsonReader %load.struct10, 8
  %eq12 = icmp eq i64 %state11, 5
  br i1 %eq12, label %if.then13, label %if.end14

if.end9:                                          ; preds = %if.end
  %field.inplace = getelementptr inbounds nuw %_Z10JsonReader, ptr %2, i32 0, i32 0
  %field.inplace19 = getelementptr inbounds nuw %_Z10JsonReader, ptr %2, i32 0, i32 7
  %field.val = load i64, ptr %field.inplace19, align 8
  %call = call i8 @_ZN5SliceI2u8EixEm(ptr %field.inplace, i64 %field.val)
  %load.struct20 = load %_Z10JsonReader, ptr %2, align 8
  %state21 = extractvalue %_Z10JsonReader %load.struct20, 8
  %eq22 = icmp eq i64 %state21, 5
  br i1 %eq22, label %if.then23, label %if.end24

if.then13:                                        ; preds = %if.then8
  %state15 = getelementptr inbounds nuw %_Z10JsonReader, ptr %2, i32 0, i32 8
  store i64 6, ptr %state15, align 8
  %variant.tag.ptr17 = getelementptr inbounds nuw %_Z9JsonToken, ptr %variant.ptr16, i32 0, i32 0
  store i8 0, ptr %variant.tag.ptr17, align 1
  %variant.val18 = load %_Z9JsonToken, ptr %variant.ptr16, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %variant.ptr16, i64 ptrtoint (ptr getelementptr (%_Z9JsonToken, ptr null, i32 1) to i64), i1 false)
  ret void

if.end14:                                         ; preds = %if.then8
  call void @_ZN10JsonReaderB4json4failEPN4scaly6memory4PageEi(ptr noalias sret(%_Z9JsonToken) %sret.result, ptr null, ptr %2, i64 1)
  %sret.body = load %_Z9JsonToken, ptr %sret.result, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %sret.result, i64 ptrtoint (ptr getelementptr (%_Z9JsonToken, ptr null, i32 1) to i64), i1 false)
  ret void

if.then23:                                        ; preds = %if.end9
  call void @_ZN10JsonReaderB4json4failEPN4scaly6memory4PageEi(ptr noalias sret(%_Z9JsonToken) %sret.result25, ptr null, ptr %2, i64 2)
  %sret.body26 = load %_Z9JsonToken, ptr %sret.result25, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %sret.result25, i64 ptrtoint (ptr getelementptr (%_Z9JsonToken, ptr null, i32 1) to i64), i1 false)
  ret void

if.end24:                                         ; preds = %if.end9
  %load.struct27 = load %_Z10JsonReader, ptr %2, align 8
  %state28 = extractvalue %_Z10JsonReader %load.struct27, 8
  %eq29 = icmp eq i64 %state28, 4
  br i1 %eq29, label %if.then30, label %if.end31

if.then30:                                        ; preds = %if.end24
  %call32 = call i1 @_ZN10JsonReaderB4json9in_objectEv(ptr %2)
  %eq33 = icmp eq i8 %call, 44
  br i1 %eq33, label %if.then34, label %if.end35

if.end31:                                         ; preds = %if.end24
  %load.struct62 = load %_Z10JsonReader, ptr %2, align 8
  %state63 = extractvalue %_Z10JsonReader %load.struct62, 8
  %eq64 = icmp eq i64 %state63, 3
  br i1 %eq64, label %land.rhs61, label %if.end60

if.then34:                                        ; preds = %if.then30
  %load.struct36 = load %_Z10JsonReader, ptr %2, align 8
  %pos37 = extractvalue %_Z10JsonReader %load.struct36, 7
  %add = add i64 %pos37, 1
  %pos38 = getelementptr inbounds nuw %_Z10JsonReader, ptr %2, i32 0, i32 7
  store i64 %add, ptr %pos38, align 8
  br i1 %call32, label %if.then39, label %if.else

if.end35:                                         ; preds = %if.then30
  br i1 %call32, label %land.rhs, label %if.end44

if.then39:                                        ; preds = %if.then34
  %state41 = getelementptr inbounds nuw %_Z10JsonReader, ptr %2, i32 0, i32 8
  store i64 2, ptr %state41, align 8
  br label %if.end40

if.else:                                          ; preds = %if.then34
  %state42 = getelementptr inbounds nuw %_Z10JsonReader, ptr %2, i32 0, i32 8
  store i64 0, ptr %state42, align 8
  br label %if.end40

if.end40:                                         ; preds = %if.else, %if.then39
  %if.value = phi i64 [ 2, %if.then39 ], [ 0, %if.else ]
  br label %repeat.body

if.then43:                                        ; preds = %land.rhs
  %variant.tag.ptr47 = getelementptr inbounds nuw %_Z9JsonToken, ptr %variant.ptr46, i32 0, i32 0
  store i8 3, ptr %variant.tag.ptr47, align 1
  %variant.val48 = load %_Z9JsonToken, ptr %variant.ptr46, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %arg.tmp, ptr align 1 %variant.ptr46, i64 ptrtoint (ptr getelementptr (%_Z9JsonToken, ptr null, i32 1) to i64), i1 false)
  call void @_ZN10JsonReaderB4json5closeEPN4scaly6memory4PageE9JsonTokenB4json(ptr noalias sret(%_Z9JsonToken) %sret.result25, ptr null, ptr %2, ptr %arg.tmp)
  %sret.body49 = load %_Z9JsonToken, ptr %sret.result25, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %sret.result25, i64 ptrtoint (ptr getelementptr (%_Z9JsonToken, ptr null, i32 1) to i64), i1 false)
  ret void

if.end44:                                         ; preds = %land.rhs, %if.end35
  %eq53 = icmp eq i1 %call32, false
  br i1 %eq53, label %land.rhs52, label %if.end51

land.rhs:                                         ; preds = %if.end35
  %eq45 = icmp eq i8 %call, 125
  br i1 %eq45, label %if.then43, label %if.end44

if.then50:                                        ; preds = %land.rhs52
  %variant.tag.ptr55 = getelementptr inbounds nuw %_Z9JsonToken, ptr %variant.ptr46, i32 0, i32 0
  store i8 5, ptr %variant.tag.ptr55, align 1
  %variant.val56 = load %_Z9JsonToken, ptr %variant.ptr46, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %arg.tmp, ptr align 1 %variant.ptr46, i64 ptrtoint (ptr getelementptr (%_Z9JsonToken, ptr null, i32 1) to i64), i1 false)
  call void @_ZN10JsonReaderB4json5closeEPN4scaly6memory4PageE9JsonTokenB4json(ptr noalias sret(%_Z9JsonToken) %sret.result25, ptr null, ptr %2, ptr %arg.tmp)
  %sret.body57 = load %_Z9JsonToken, ptr %sret.result25, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %sret.result25, i64 ptrtoint (ptr getelementptr (%_Z9JsonToken, ptr null, i32 1) to i64), i1 false)
  ret void

if.end51:                                         ; preds = %land.rhs52, %if.end44
  call void @_ZN10JsonReaderB4json4failEPN4scaly6memory4PageEi(ptr noalias sret(%_Z9JsonToken) %sret.result25, ptr null, ptr %2, i64 6)
  %sret.body58 = load %_Z9JsonToken, ptr %sret.result25, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %sret.result25, i64 ptrtoint (ptr getelementptr (%_Z9JsonToken, ptr null, i32 1) to i64), i1 false)
  ret void

land.rhs52:                                       ; preds = %if.end44
  %eq54 = icmp eq i8 %call, 93
  br i1 %eq54, label %if.then50, label %if.end51

if.then59:                                        ; preds = %land.rhs61
  %variant.tag.ptr66 = getelementptr inbounds nuw %_Z9JsonToken, ptr %arg.tmp, i32 0, i32 0
  store i8 3, ptr %variant.tag.ptr66, align 1
  %variant.val67 = load %_Z9JsonToken, ptr %arg.tmp, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %arg.tmp68, ptr align 1 %arg.tmp, i64 ptrtoint (ptr getelementptr (%_Z9JsonToken, ptr null, i32 1) to i64), i1 false)
  call void @_ZN10JsonReaderB4json5closeEPN4scaly6memory4PageE9JsonTokenB4json(ptr noalias sret(%_Z9JsonToken) %variant.ptr46, ptr null, ptr %2, ptr %arg.tmp68)
  %sret.body69 = load %_Z9JsonToken, ptr %variant.ptr46, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %variant.ptr46, i64 ptrtoint (ptr getelementptr (%_Z9JsonToken, ptr null, i32 1) to i64), i1 false)
  ret void

if.end60:                                         ; preds = %land.rhs61, %if.end31
  %load.struct73 = load %_Z10JsonReader, ptr %2, align 8
  %state74 = extractvalue %_Z10JsonReader %load.struct73, 8
  %eq75 = icmp eq i64 %state74, 1
  br i1 %eq75, label %land.rhs72, label %if.end71

land.rhs61:                                       ; preds = %if.end31
  %eq65 = icmp eq i8 %call, 125
  br i1 %eq65, label %if.then59, label %if.end60

if.then70:                                        ; preds = %land.rhs72
  %variant.tag.ptr77 = getelementptr inbounds nuw %_Z9JsonToken, ptr %arg.tmp, i32 0, i32 0
  store i8 5, ptr %variant.tag.ptr77, align 1
  %variant.val78 = load %_Z9JsonToken, ptr %arg.tmp, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %arg.tmp68, ptr align 1 %arg.tmp, i64 ptrtoint (ptr getelementptr (%_Z9JsonToken, ptr null, i32 1) to i64), i1 false)
  call void @_ZN10JsonReaderB4json5closeEPN4scaly6memory4PageE9JsonTokenB4json(ptr noalias sret(%_Z9JsonToken) %variant.ptr46, ptr null, ptr %2, ptr %arg.tmp68)
  %sret.body79 = load %_Z9JsonToken, ptr %variant.ptr46, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %variant.ptr46, i64 ptrtoint (ptr getelementptr (%_Z9JsonToken, ptr null, i32 1) to i64), i1 false)
  ret void

if.end71:                                         ; preds = %land.rhs72, %if.end60
  %load.struct82 = load %_Z10JsonReader, ptr %2, align 8
  %state83 = extractvalue %_Z10JsonReader %load.struct82, 8
  %eq84 = icmp eq i64 %state83, 2
  br i1 %eq84, label %if.then80, label %lor.rhs

land.rhs72:                                       ; preds = %if.end60
  %eq76 = icmp eq i8 %call, 93
  br i1 %eq76, label %if.then70, label %if.end71

if.then80:                                        ; preds = %lor.rhs, %if.end71
  %ne88 = icmp ne i8 %call, 34
  br i1 %ne88, label %if.then89, label %if.end90

if.end81:                                         ; preds = %lor.rhs
  call void @_ZN10JsonReaderB4json5valueEPN4scaly6memory4PageE2u8(ptr noalias sret(%_Z9JsonToken) %arg.tmp, ptr %1, ptr %2, i8 %call)
  %sret.body120 = load %_Z9JsonToken, ptr %arg.tmp, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %arg.tmp, i64 ptrtoint (ptr getelementptr (%_Z9JsonToken, ptr null, i32 1) to i64), i1 false)
  ret void

lor.rhs:                                          ; preds = %if.end71
  %load.struct85 = load %_Z10JsonReader, ptr %2, align 8
  %state86 = extractvalue %_Z10JsonReader %load.struct85, 8
  %eq87 = icmp eq i64 %state86, 3
  br i1 %eq87, label %if.then80, label %if.end81

if.then89:                                        ; preds = %if.then80
  call void @_ZN10JsonReaderB4json4failEPN4scaly6memory4PageEi(ptr noalias sret(%_Z9JsonToken) %variant.ptr46, ptr null, ptr %2, i64 4)
  %sret.body91 = load %_Z9JsonToken, ptr %variant.ptr46, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %variant.ptr46, i64 ptrtoint (ptr getelementptr (%_Z9JsonToken, ptr null, i32 1) to i64), i1 false)
  ret void

if.end90:                                         ; preds = %if.then80
  %call92 = call i1 @_ZN10JsonReaderB4json11scan_stringEv(ptr %2)
  %eq93 = icmp eq i1 %call92, false
  br i1 %eq93, label %if.then94, label %if.end95

if.then94:                                        ; preds = %if.end90
  %variant.tag.ptr96 = getelementptr inbounds nuw %_Z9JsonToken, ptr %variant.ptr46, i32 0, i32 0
  store i8 1, ptr %variant.tag.ptr96, align 1
  %variant.val97 = load %_Z9JsonToken, ptr %variant.ptr46, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %variant.ptr46, i64 ptrtoint (ptr getelementptr (%_Z9JsonToken, ptr null, i32 1) to i64), i1 false)
  ret void

if.end95:                                         ; preds = %if.end90
  call void @_ZN10JsonReaderB4json7skip_wsEv(ptr %2)
  %load.struct101 = load %_Z10JsonReader, ptr %2, align 8
  %pos102 = extractvalue %_Z10JsonReader %load.struct101, 7
  %load.struct103 = load %_Z10JsonReader, ptr %2, align 8
  %source104 = extractvalue %_Z10JsonReader %load.struct103, 0
  %length105 = extractvalue %_Z5SliceI2u8E %source104, 0
  %ge106 = icmp uge i64 %pos102, %length105
  br i1 %ge106, label %if.then98, label %lor.rhs100

if.then98:                                        ; preds = %lor.rhs100, %if.end95
  call void @_ZN10JsonReaderB4json4failEPN4scaly6memory4PageEi(ptr noalias sret(%_Z9JsonToken) %variant.ptr46, ptr null, ptr %2, i64 5)
  %sret.body112 = load %_Z9JsonToken, ptr %variant.ptr46, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %variant.ptr46, i64 ptrtoint (ptr getelementptr (%_Z9JsonToken, ptr null, i32 1) to i64), i1 false)
  ret void

if.end99:                                         ; preds = %lor.rhs100
  %load.struct113 = load %_Z10JsonReader, ptr %2, align 8
  %pos114 = extractvalue %_Z10JsonReader %load.struct113, 7
  %add115 = add i64 %pos114, 1
  %pos116 = getelementptr inbounds nuw %_Z10JsonReader, ptr %2, i32 0, i32 7
  store i64 %add115, ptr %pos116, align 8
  %state117 = getelementptr inbounds nuw %_Z10JsonReader, ptr %2, i32 0, i32 8
  store i64 0, ptr %state117, align 8
  %variant.tag.ptr118 = getelementptr inbounds nuw %_Z9JsonToken, ptr %variant.ptr46, i32 0, i32 0
  store i8 6, ptr %variant.tag.ptr118, align 1
  %variant.val119 = load %_Z9JsonToken, ptr %variant.ptr46, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %variant.ptr46, i64 ptrtoint (ptr getelementptr (%_Z9JsonToken, ptr null, i32 1) to i64), i1 false)
  ret void

lor.rhs100:                                       ; preds = %if.end95
  %field.inplace107 = getelementptr inbounds nuw %_Z10JsonReader, ptr %2, i32 0, i32 0
  %field.inplace108 = getelementptr inbounds nuw %_Z10JsonReader, ptr %2, i32 0, i32 7
  %field.val109 = load i64, ptr %field.inplace108, align 8
  %call110 = call i8 @_ZN5SliceI2u8EixEm(ptr %field.inplace107, i64 %field.val109)
  %ne111 = icmp ne i8 %call110, 58
  br i1 %ne111, label %if.then98, label %if.end99
}

define linkonce_odr i1 @_ZN10JsonReaderB4json4skipEv(ptr %0) {
entry:
  %frame = alloca { ptr, ptr }, align 8
  store ptr null, ptr %frame, align 8
  %frame.parent = getelementptr inbounds nuw { ptr, ptr }, ptr %frame, i32 0, i32 1
  store ptr null, ptr %frame.parent, align 8
  %sret.result = alloca %_Z9JsonToken, align 8
  %load.struct = load %_Z10JsonReader, ptr %0, align 8
  %state = extractvalue %_Z10JsonReader %load.struct, 8
  %ne = icmp ne i64 %state, 3
  br i1 %ne, label %land.rhs, label %if.end

if.then:                                          ; preds = %land.rhs
  %load.struct4 = load %_Z10JsonReader, ptr %0, align 8
  %error = extractvalue %_Z10JsonReader %load.struct4, 13
  %eq = icmp eq i64 %error, 0
  call void @_Z19scaly_release_frameP5Frame(ptr %frame)
  ret i1 %eq

if.end:                                           ; preds = %land.rhs, %entry
  %load.struct5 = load %_Z10JsonReader, ptr %0, align 8
  %depth = extractvalue %_Z10JsonReader %load.struct5, 2
  br label %repeat.body

land.rhs:                                         ; preds = %entry
  %load.struct1 = load %_Z10JsonReader, ptr %0, align 8
  %state2 = extractvalue %_Z10JsonReader %load.struct1, 8
  %ne3 = icmp ne i64 %state2, 1
  br i1 %ne3, label %if.then, label %if.end

repeat.body:                                      ; preds = %choose.end, %if.end
  call void @_ZN10JsonReaderB4json4nextEPN4scaly6memory4PageE(ptr noalias sret(%_Z9JsonToken) %sret.result, ptr %frame, ptr %0)
  %tag.ptr = getelementptr inbounds nuw %_Z9JsonToken, ptr %sret.result, i32 0, i32 0
  %tag = load i8, ptr %tag.ptr, align 1
  switch i8 %tag, label %choose.else [
    i8 1, label %choose.when
    i8 0, label %choose.when6
  ]

repeat.exit:                                      ; No predecessors!
  call void @_Z19scaly_release_frameP5Frame(ptr %frame)
  ret i1 false

choose.end:                                       ; preds = %if.end11
  br label %repeat.body

choose.else:                                      ; preds = %repeat.body
  %load.struct8 = load %_Z10JsonReader, ptr %0, align 8
  %depth9 = extractvalue %_Z10JsonReader %load.struct8, 2
  %lt = icmp ult i64 %depth9, %depth
  br i1 %lt, label %if.then10, label %if.end11

choose.when:                                      ; preds = %repeat.body
  %"variant.c_data().ptr" = getelementptr inbounds nuw %_Z9JsonToken, ptr %sret.result, i32 0, i32 1
  call void @_Z19scaly_release_frameP5Frame(ptr %frame)
  ret i1 false

choose.when6:                                     ; preds = %repeat.body
  %"variant.c_data().ptr7" = getelementptr inbounds nuw %_Z9JsonToken, ptr %sret.result, i32 0, i32 1
  call void @_Z19scaly_release_frameP5Frame(ptr %frame)
  ret i1 true

if.then10:                                        ; preds = %choose.else
  call void @_Z19scaly_release_frameP5Frame(ptr %frame)
  ret i1 true

if.end11:                                         ; preds = %choose.else
  br label %choose.end
}

define linkonce_odr ptr @_ZN5SliceI1TE3getEm(ptr %0, i64 %1) {
stub.entry:
  ret ptr null
}

define linkonce_odr ptr @_ZN5SliceI1TE2atEm(ptr %0, i64 %1) {
stub.entry:
  ret ptr null
}

define linkonce_odr void @_ZN5SliceI1TE3putEm1T(ptr %0, i64 %1, ptr %2) {
stub.entry:
  ret void
}

define linkonce_odr i1 @_ZN5SliceI1TE8is_emptyEv(ptr %0) {
stub.entry:
  ret i1 false
}

define linkonce_odr void @_ZN5SliceI1TE8subsliceEmm(ptr noalias sret(%_Z5SliceI1TE) %0, ptr %1, i64 %2, i64 %3) {
stub.entry:
  ret void
}

define linkonce_odr void @_ZN5SliceI1TE10slice_fromEPN4scaly6memory4PageEm(ptr noalias sret(%_Z5SliceI1TE) %0, ptr %1, ptr %2, i64 %3) {
stub.entry:
  ret void
}

define linkonce_odr void @_ZN5SliceI1TE8slice_toEPN4scaly6memory4PageEm(ptr noalias sret(%_Z5SliceI1TE) %0, ptr %1, ptr %2, i64 %3) {
stub.entry:
  ret void
}

define linkonce_odr i1 @_ZN5SliceI1TE6equalsE5SliceI1TE(ptr %0, ptr %1) {
stub.entry:
  ret i1 false
}

define linkonce_odr i1 @_ZN5SliceI1TE11starts_withE5SliceI1TE(ptr %0, ptr %1) {
stub.entry:
  ret i1 false
}

define linkonce_odr i1 @_ZN5SliceI1TE9ends_withE5SliceI1TE(ptr %0, ptr %1) {
stub.entry:
  ret i1 false
}

define linkonce_odr ptr @_ZN13SliceIteratorI1TE4nextEv(ptr %0) {
stub.entry:
  ret ptr null
}

define linkonce_odr void @_ZN13SliceIteratorI1TEC1E5SliceI1TE(ptr %0, ptr %1) {
stub.entry:
  ret void
}

define linkonce_odr void @_ZN5SliceI1TE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z13SliceIteratorI1TE) %0, ptr %1, ptr %2) {
stub.entry:
  ret void
}

define linkonce_odr void @_ZN5SliceI1TEC1Ev(ptr %0) {
stub.entry:
  ret void
}

define linkonce_odr void @_ZN10JsonReaderB4json4textEv(ptr noalias sret(%_Z5SliceI2u8E) %0, ptr %1) {
entry:
  %sret.result = alloca %_Z5SliceI2u8E, align 8
  %field.inplace = getelementptr inbounds nuw %_Z10JsonReader, ptr %1, i32 0, i32 0
  %field.inplace1 = getelementptr inbounds nuw %_Z10JsonReader, ptr %1, i32 0, i32 9
  %field.val = load i64, ptr %field.inplace1, align 8
  %field.inplace2 = getelementptr inbounds nuw %_Z10JsonReader, ptr %1, i32 0, i32 10
  %field.val3 = load i64, ptr %field.inplace2, align 8
  call void @_ZN5SliceI2u8E8subsliceEmm(ptr noalias sret(%_Z5SliceI2u8E) %sret.result, ptr %field.inplace, i64 %field.val, i64 %field.val3)
  %sret.body = load %_Z5SliceI2u8E, ptr %sret.result, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %sret.result, i64 ptrtoint (ptr getelementptr (%_Z5SliceI2u8E, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN10JsonReaderB4json6decodeEPN4scaly6memory4PageE5SliceI2u8Eb(ptr noalias sret({ ptr }) %0, ptr %1, ptr %2, i1 %3) {
entry:
  %sret.result101 = alloca { ptr }, align 8
  %cp = alloca i64, align 8
  %sret.result = alloca %_Z5SliceI2u8E, align 8
  %j = alloca i64, align 8
  %i = alloca i64, align 8
  %sb = alloca ptr, align 8
  %eq = icmp eq i1 %3, false
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %frame.page = load ptr, ptr %1, align 8
  %frame.has_page = icmp ne ptr %frame.page, null
  br i1 %frame.has_page, label %frame.forced, label %frame.force

if.end:                                           ; preds = %entry
  %frame.page3 = load ptr, ptr %1, align 8
  %frame.has_page4 = icmp ne ptr %frame.page3, null
  br i1 %frame.has_page4, label %frame.forced6, label %frame.force5

frame.force:                                      ; preds = %if.then
  %forced_page = call ptr @_Z17scaly_force_frameP5Frame(ptr %1)
  br label %frame.forced

frame.forced:                                     ; preds = %frame.force, %if.then
  %forced_page1 = phi ptr [ %frame.page, %if.then ], [ %forced_page, %frame.force ]
  %struct.region = call ptr @_ZN4Page8allocateEmm(ptr %forced_page1, i64 ptrtoint (ptr getelementptr ({ ptr }, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr ({ i1, { ptr } }, ptr null, i64 0, i32 1) to i64))
  %load.struct = load %_Z5SliceI2u8E, ptr %2, align 8
  %data = extractvalue %_Z5SliceI2u8E %load.struct, 1
  %load.struct2 = load %_Z5SliceI2u8E, ptr %2, align 8
  %length = extractvalue %_Z5SliceI2u8E %load.struct2, 0
  call void @_ZN6StringC1EP10const_charm(ptr %struct.region, ptr %data, i64 %length)
  %sret.body = load { ptr }, ptr %struct.region, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %struct.region, i64 ptrtoint (ptr getelementptr ({ ptr }, ptr null, i32 1) to i64), i1 false)
  ret void

frame.force5:                                     ; preds = %if.end
  %forced_page7 = call ptr @_Z17scaly_force_frameP5Frame(ptr %1)
  br label %frame.forced6

frame.forced6:                                    ; preds = %frame.force5, %if.end
  %forced_page8 = phi ptr [ %frame.page3, %if.end ], [ %forced_page7, %frame.force5 ]
  %struct.region9 = call ptr @_ZN4Page8allocateEmm(ptr %forced_page8, i64 64, i64 ptrtoint (ptr getelementptr ({ i1, ptr }, ptr null, i64 0, i32 1) to i64))
  call void @_ZN13StringBuilderC1Ev(ptr %struct.region9)
  store ptr %struct.region9, ptr %sb, align 1
  store i64 0, ptr %i, align 1
  br label %while.cond

while.cond:                                       ; preds = %if.end46, %frame.forced6
  %i10 = load i64, ptr %i, align 8
  %load.struct11 = load %_Z5SliceI2u8E, ptr %2, align 8
  %length12 = extractvalue %_Z5SliceI2u8E %load.struct11, 0
  %lt = icmp ult i64 %i10, %length12
  br i1 %lt, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %i13 = load i64, ptr %i, align 8
  store i64 %i13, ptr %j, align 1
  br label %while.cond14

while.exit:                                       ; preds = %if.then37, %while.cond
  %sb102 = load ptr, ptr %sb, align 8
  call void @_ZN13StringBuilder9to_stringEPN4scaly6memory4PageE(ptr noalias sret({ ptr }) %sret.result101, ptr %1, ptr %sb102)
  %sret.body103 = load { ptr }, ptr %sret.result101, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %sret.result101, i64 ptrtoint (ptr getelementptr ({ ptr }, ptr null, i32 1) to i64), i1 false)
  ret void

while.cond14:                                     ; preds = %while.body15, %while.body
  %j17 = load i64, ptr %j, align 8
  %load.struct18 = load %_Z5SliceI2u8E, ptr %2, align 8
  %length19 = extractvalue %_Z5SliceI2u8E %load.struct18, 0
  %lt20 = icmp ult i64 %j17, %length19
  br i1 %lt20, label %lor.rhs, label %lor.end

while.body15:                                     ; preds = %lor.end
  %j22 = load i64, ptr %j, align 8
  %add = add i64 %j22, 1
  store i64 %add, ptr %j, align 1
  br label %while.cond14

while.exit16:                                     ; preds = %lor.end
  %j23 = load i64, ptr %j, align 8
  %i24 = load i64, ptr %i, align 8
  %gt = icmp ugt i64 %j23, %i24
  br i1 %gt, label %if.then25, label %if.end26

lor.rhs:                                          ; preds = %while.cond14
  %j21 = load i64, ptr %j, align 8
  %call = call i8 @_ZN5SliceI2u8EixEm(ptr %2, i64 %j21)
  %ne = icmp ne i8 %call, 92
  br label %lor.end

lor.end:                                          ; preds = %lor.rhs, %while.cond14
  %lor.result = phi i1 [ false, %while.cond14 ], [ %ne, %lor.rhs ]
  br i1 %lor.result, label %while.body15, label %while.exit16

if.then25:                                        ; preds = %while.exit16
  %sb27 = load ptr, ptr %sb, align 8
  %i28 = load i64, ptr %i, align 8
  %j29 = load i64, ptr %j, align 8
  call void @_ZN5SliceI2u8E8subsliceEmm(ptr noalias sret(%_Z5SliceI2u8E) %sret.result, ptr %2, i64 %i28, i64 %j29)
  %load.struct30 = load %_Z5SliceI2u8E, ptr %sret.result, align 8
  %data31 = extractvalue %_Z5SliceI2u8E %load.struct30, 1
  %j32 = load i64, ptr %j, align 8
  %i33 = load i64, ptr %i, align 8
  %sub = sub i64 %j32, %i33
  call void @_ZN13StringBuilder6appendEPcm(ptr %sb27, ptr %data31, i64 %sub)
  br label %if.end26

if.end26:                                         ; preds = %if.then25, %while.exit16
  %j34 = load i64, ptr %j, align 8
  %load.struct35 = load %_Z5SliceI2u8E, ptr %2, align 8
  %length36 = extractvalue %_Z5SliceI2u8E %load.struct35, 0
  %ge = icmp uge i64 %j34, %length36
  br i1 %ge, label %if.then37, label %if.end38

if.then37:                                        ; preds = %if.end26
  br label %while.exit

if.end38:                                         ; preds = %if.end26
  %j39 = load i64, ptr %j, align 8
  %add40 = add i64 %j39, 1
  %call41 = call i8 @_ZN5SliceI2u8EixEm(ptr %2, i64 %add40)
  %j42 = load i64, ptr %j, align 8
  %add43 = add i64 %j42, 2
  store i64 %add43, ptr %i, align 1
  %eq44 = icmp eq i8 %call41, 117
  br i1 %eq44, label %if.then45, label %if.else

if.then45:                                        ; preds = %if.end38
  %i47 = load i64, ptr %i, align 8
  %call48 = call i64 @_ZN10JsonReaderB4json4hex4E5SliceI2u8Em(ptr %2, i64 %i47)
  store i64 %call48, ptr %cp, align 1
  %i49 = load i64, ptr %i, align 8
  %add50 = add i64 %i49, 4
  store i64 %add50, ptr %i, align 1
  %cp54 = load i64, ptr %cp, align 8
  %ge55 = icmp sge i64 %cp54, 55296
  br i1 %ge55, label %land.rhs, label %if.else52

if.else:                                          ; preds = %if.end38
  %sb99 = load ptr, ptr %sb, align 8
  %call100 = call i8 @_ZN10JsonReaderB4json8unescapeE2u8(i8 %call41)
  call void @_ZN13StringBuilder6appendEc(ptr %sb99, i8 %call100)
  br label %if.end46

if.end46:                                         ; preds = %if.else, %if.end53
  br label %while.cond

if.then51:                                        ; preds = %land.rhs
  %i62 = load i64, ptr %i, align 8
  %add63 = add i64 %i62, 6
  %load.struct64 = load %_Z5SliceI2u8E, ptr %2, align 8
  %length65 = extractvalue %_Z5SliceI2u8E %load.struct64, 0
  %le66 = icmp ule i64 %add63, %length65
  br i1 %le66, label %land.rhs61, label %if.else58

if.else52:                                        ; preds = %land.rhs, %if.then45
  %cp93 = load i64, ptr %cp, align 8
  %ge94 = icmp sge i64 %cp93, 56320
  br i1 %ge94, label %land.rhs92, label %if.end91

if.end53:                                         ; preds = %if.end91, %if.end59
  %sb97 = load ptr, ptr %sb, align 8
  %cp98 = load i64, ptr %cp, align 8
  call void @_ZN10JsonReaderB4json11append_utf8ER13StringBuilder3i64(ptr %sb97, i64 %cp98)
  br label %if.end46

land.rhs:                                         ; preds = %if.then45
  %cp56 = load i64, ptr %cp, align 8
  %le = icmp sle i64 %cp56, 56319
  br i1 %le, label %if.then51, label %if.else52

if.then57:                                        ; preds = %land.rhs60
  %i74 = load i64, ptr %i, align 8
  %add75 = add i64 %i74, 2
  %call76 = call i64 @_ZN10JsonReaderB4json4hex4E5SliceI2u8Em(ptr %2, i64 %add75)
  %ge81 = icmp sge i64 %call76, 56320
  br i1 %ge81, label %land.rhs80, label %if.else78

if.else58:                                        ; preds = %land.rhs60, %land.rhs61, %if.then51
  store i64 65533, ptr %cp, align 1
  br label %if.end59

if.end59:                                         ; preds = %if.else58, %if.end79
  br label %if.end53

land.rhs60:                                       ; preds = %land.rhs61
  %i70 = load i64, ptr %i, align 8
  %add71 = add i64 %i70, 1
  %call72 = call i8 @_ZN5SliceI2u8EixEm(ptr %2, i64 %add71)
  %eq73 = icmp eq i8 %call72, 117
  br i1 %eq73, label %if.then57, label %if.else58

land.rhs61:                                       ; preds = %if.then51
  %i67 = load i64, ptr %i, align 8
  %call68 = call i8 @_ZN5SliceI2u8EixEm(ptr %2, i64 %i67)
  %eq69 = icmp eq i8 %call68, 92
  br i1 %eq69, label %land.rhs60, label %if.else58

if.then77:                                        ; preds = %land.rhs80
  %cp83 = load i64, ptr %cp, align 8
  %sub84 = sub i64 %cp83, 55296
  %shl = shl i64 %sub84, 10
  %add85 = add i64 65536, %shl
  %sub86 = sub i64 %call76, 56320
  %add87 = add i64 %add85, %sub86
  store i64 %add87, ptr %cp, align 1
  %i88 = load i64, ptr %i, align 8
  %add89 = add i64 %i88, 6
  store i64 %add89, ptr %i, align 1
  br label %if.end79

if.else78:                                        ; preds = %land.rhs80, %if.then57
  store i64 65533, ptr %cp, align 1
  br label %if.end79

if.end79:                                         ; preds = %if.else78, %if.then77
  br label %if.end59

land.rhs80:                                       ; preds = %if.then57
  %le82 = icmp sle i64 %call76, 57343
  br i1 %le82, label %if.then77, label %if.else78

if.then90:                                        ; preds = %land.rhs92
  store i64 65533, ptr %cp, align 1
  br label %if.end91

if.end91:                                         ; preds = %if.then90, %land.rhs92, %if.else52
  br label %if.end53

land.rhs92:                                       ; preds = %if.else52
  %cp95 = load i64, ptr %cp, align 8
  %le96 = icmp sle i64 %cp95, 57343
  br i1 %le96, label %if.then90, label %if.end91
}

define linkonce_odr void @_ZN10JsonReaderB4json6stringEPN4scaly6memory4PageE(ptr noalias sret({ ptr }) %0, ptr %1, ptr %2) {
entry:
  %sret.result = alloca { ptr }, align 8
  %sret.result1 = alloca %_Z5SliceI2u8E, align 8
  %field.inplace = getelementptr inbounds nuw %_Z10JsonReader, ptr %2, i32 0, i32 0
  %field.inplace2 = getelementptr inbounds nuw %_Z10JsonReader, ptr %2, i32 0, i32 9
  %field.val = load i64, ptr %field.inplace2, align 8
  %field.inplace3 = getelementptr inbounds nuw %_Z10JsonReader, ptr %2, i32 0, i32 10
  %field.val4 = load i64, ptr %field.inplace3, align 8
  call void @_ZN5SliceI2u8E8subsliceEmm(ptr noalias sret(%_Z5SliceI2u8E) %sret.result1, ptr %field.inplace, i64 %field.val, i64 %field.val4)
  %field.inplace5 = getelementptr inbounds nuw %_Z10JsonReader, ptr %2, i32 0, i32 11
  %field.val6 = load i1, ptr %field.inplace5, align 1
  call void @_ZN10JsonReaderB4json6decodeEPN4scaly6memory4PageE5SliceI2u8Eb(ptr noalias sret({ ptr }) %sret.result, ptr %1, ptr %sret.result1, i1 %field.val6)
  %sret.body = load { ptr }, ptr %sret.result, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %sret.result, i64 ptrtoint (ptr getelementptr ({ ptr }, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr i1 @_ZN10JsonReaderB4json11is_integralEv(ptr %0) {
entry:
  %load.struct = load %_Z10JsonReader, ptr %0, align 8
  %integral = extractvalue %_Z10JsonReader %load.struct, 12
  ret i1 %integral
}

define linkonce_odr i1 @_ZN10JsonReaderB4json6failedEv(ptr %0) {
entry:
  %load.struct = load %_Z10JsonReader, ptr %0, align 8
  %error = extractvalue %_Z10JsonReader %load.struct, 13
  %ne = icmp ne i64 %error, 0
  ret i1 %ne
}

define linkonce_odr i64 @_ZN10JsonReaderB4json12error_offsetEv(ptr %0) {
entry:
  %load.struct = load %_Z10JsonReader, ptr %0, align 8
  %error_at = extractvalue %_Z10JsonReader %load.struct, 14
  ret i64 %error_at
}

define linkonce_odr ptr @_ZN5SliceIcE3getEm(ptr %0, i64 %1) {
entry:
  %load.struct = load %_Z5SliceIcE, ptr %0, align 8
  %length = extractvalue %_Z5SliceIcE %load.struct, 0
  %ge = icmp uge i64 %1, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %load.struct1 = load %_Z5SliceIcE, ptr %0, align 8
  %data = extractvalue %_Z5SliceIcE %load.struct1, 1
  %ptr.add = getelementptr inbounds i8, ptr %data, i64 %1
  ret ptr %ptr.add
}

define linkonce_odr ptr @_ZN5SliceIcE2atEm(ptr %0, i64 %1) {
entry:
  %load.struct = load %_Z5SliceIcE, ptr %0, align 8
  %length = extractvalue %_Z5SliceIcE %load.struct, 0
  %ge = icmp uge i64 %1, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %field.inplace = getelementptr inbounds nuw %_Z5SliceIcE, ptr %0, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  call void @_Z15scaly_panic_oobP10const_charmm(ptr @.str.4, i64 %1, i64 %field.val)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %load.struct1 = load %_Z5SliceIcE, ptr %0, align 8
  %data = extractvalue %_Z5SliceIcE %load.struct1, 1
  %ptr.add = getelementptr inbounds i8, ptr %data, i64 %1
  ret ptr %ptr.add
}

define linkonce_odr void @_ZN5SliceIcE3putEmc(ptr %0, i64 %1, i8 %2) {
entry:
  %load.struct = load %_Z5SliceIcE, ptr %0, align 8
  %length = extractvalue %_Z5SliceIcE %load.struct, 0
  %ge = icmp uge i64 %1, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %field.inplace = getelementptr inbounds nuw %_Z5SliceIcE, ptr %0, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  call void @_Z15scaly_panic_oobP10const_charmm(ptr @.str.5, i64 %1, i64 %field.val)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %load.struct1 = load %_Z5SliceIcE, ptr %0, align 8
  %data = extractvalue %_Z5SliceIcE %load.struct1, 1
  %ptr.add = getelementptr inbounds i8, ptr %data, i64 %1
  store i8 %2, ptr %ptr.add, align 1
  ret void
}

define linkonce_odr i1 @_ZN5SliceIcE8is_emptyEv(ptr %0) {
entry:
  %load.struct = load %_Z5SliceIcE, ptr %0, align 8
  %length = extractvalue %_Z5SliceIcE %load.struct, 0
  %eq = icmp eq i64 %length, 0
  ret i1 %eq
}

define linkonce_odr void @_ZN5SliceIcE8subsliceEmm(ptr noalias sret(%_Z5SliceIcE) %0, ptr %1, i64 %2, i64 %3) {
entry:
  %tuple = alloca %_Z5SliceIcE, align 8
  %from = alloca i64, align 8
  store i64 %2, ptr %from, align 1
  %to = alloca i64, align 8
  store i64 %3, ptr %to, align 1
  %from1 = load i64, ptr %from, align 8
  %load.struct = load %_Z5SliceIcE, ptr %1, align 8
  %length = extractvalue %_Z5SliceIcE %load.struct, 0
  %gt = icmp ugt i64 %from1, %length
  br i1 %gt, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %load.struct2 = load %_Z5SliceIcE, ptr %1, align 8
  %length3 = extractvalue %_Z5SliceIcE %load.struct2, 0
  store i64 %length3, ptr %from, align 1
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %to4 = load i64, ptr %to, align 8
  %load.struct5 = load %_Z5SliceIcE, ptr %1, align 8
  %length6 = extractvalue %_Z5SliceIcE %load.struct5, 0
  %gt7 = icmp ugt i64 %to4, %length6
  br i1 %gt7, label %if.then8, label %if.end9

if.then8:                                         ; preds = %if.end
  %load.struct10 = load %_Z5SliceIcE, ptr %1, align 8
  %length11 = extractvalue %_Z5SliceIcE %load.struct10, 0
  store i64 %length11, ptr %to, align 1
  br label %if.end9

if.end9:                                          ; preds = %if.then8, %if.end
  %from12 = load i64, ptr %from, align 8
  %to13 = load i64, ptr %to, align 8
  %gt14 = icmp ugt i64 %from12, %to13
  br i1 %gt14, label %if.then15, label %if.end16

if.then15:                                        ; preds = %if.end9
  %to17 = load i64, ptr %to, align 8
  store i64 %to17, ptr %from, align 1
  br label %if.end16

if.end16:                                         ; preds = %if.then15, %if.end9
  %to18 = load i64, ptr %to, align 8
  %from19 = load i64, ptr %from, align 8
  %sub = sub i64 %to18, %from19
  %load.struct20 = load %_Z5SliceIcE, ptr %1, align 8
  %data = extractvalue %_Z5SliceIcE %load.struct20, 1
  %from21 = load i64, ptr %from, align 8
  %ptr.add = getelementptr inbounds i8, ptr %data, i64 %from21
  %tuple.field = getelementptr inbounds nuw %_Z5SliceIcE, ptr %tuple, i32 0, i32 0
  store i64 %sub, ptr %tuple.field, align 1
  %tuple.field22 = getelementptr inbounds nuw %_Z5SliceIcE, ptr %tuple, i32 0, i32 1
  store ptr %ptr.add, ptr %tuple.field22, align 1
  %tuple.val = load %_Z5SliceIcE, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z5SliceIcE, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN5SliceIcE10slice_fromEPN4scaly6memory4PageEm(ptr noalias sret(%_Z5SliceIcE) %0, ptr %1, ptr %2, i64 %3) {
entry:
  %sret.result = alloca %_Z5SliceIcE, align 8
  %field.inplace = getelementptr inbounds nuw %_Z5SliceIcE, ptr %2, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  call void @_ZN5SliceIcE8subsliceEmm(ptr noalias sret(%_Z5SliceIcE) %sret.result, ptr %2, i64 %3, i64 %field.val)
  %sret.body = load %_Z5SliceIcE, ptr %sret.result, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %sret.result, i64 ptrtoint (ptr getelementptr (%_Z5SliceIcE, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN5SliceIcE8slice_toEPN4scaly6memory4PageEm(ptr noalias sret(%_Z5SliceIcE) %0, ptr %1, ptr %2, i64 %3) {
entry:
  %sret.result = alloca %_Z5SliceIcE, align 8
  call void @_ZN5SliceIcE8subsliceEmm(ptr noalias sret(%_Z5SliceIcE) %sret.result, ptr %2, i64 0, i64 %3)
  %sret.body = load %_Z5SliceIcE, ptr %sret.result, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %sret.result, i64 ptrtoint (ptr getelementptr (%_Z5SliceIcE, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr i1 @_ZN5SliceIcE6equalsE5SliceIcE(ptr %0, ptr %1) {
entry:
  %load.struct = load %_Z5SliceIcE, ptr %0, align 8
  %length = extractvalue %_Z5SliceIcE %load.struct, 0
  %load.struct1 = load %_Z5SliceIcE, ptr %1, align 8
  %length2 = extractvalue %_Z5SliceIcE %load.struct1, 0
  %ne = icmp ne i64 %length, %length2
  br i1 %ne, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret i1 false

if.end:                                           ; preds = %entry
  %load.struct3 = load %_Z5SliceIcE, ptr %0, align 8
  %length4 = extractvalue %_Z5SliceIcE %load.struct3, 0
  %eq = icmp eq i64 %length4, 0
  br i1 %eq, label %if.then5, label %if.end6

if.then5:                                         ; preds = %if.end
  ret i1 true

if.end6:                                          ; preds = %if.end
  %load.struct7 = load %_Z5SliceIcE, ptr %0, align 8
  %data = extractvalue %_Z5SliceIcE %load.struct7, 1
  %load.struct8 = load %_Z5SliceIcE, ptr %1, align 8
  %data9 = extractvalue %_Z5SliceIcE %load.struct8, 1
  %load.struct10 = load %_Z5SliceIcE, ptr %0, align 8
  %length11 = extractvalue %_Z5SliceIcE %load.struct10, 0
  %mul = mul i64 %length11, ptrtoint (ptr getelementptr (i8, ptr null, i32 1) to i64)
  %call = call i32 @memcmp(ptr %data, ptr %data9, i64 %mul)
  %eq12 = icmp eq i32 %call, 0
  ret i1 %eq12
}

define linkonce_odr i1 @_ZN5SliceIcE11starts_withE5SliceIcE(ptr %0, ptr %1) {
entry:
  %sret.result = alloca %_Z5SliceIcE, align 8
  %load.struct = load %_Z5SliceIcE, ptr %1, align 8
  %length = extractvalue %_Z5SliceIcE %load.struct, 0
  %load.struct1 = load %_Z5SliceIcE, ptr %0, align 8
  %length2 = extractvalue %_Z5SliceIcE %load.struct1, 0
  %gt = icmp ugt i64 %length, %length2
  br i1 %gt, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret i1 false

if.end:                                           ; preds = %entry
  %field.inplace = getelementptr inbounds nuw %_Z5SliceIcE, ptr %1, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  call void @_ZN5SliceIcE8subsliceEmm(ptr noalias sret(%_Z5SliceIcE) %sret.result, ptr %0, i64 0, i64 %field.val)
  %call = call i1 @_ZN5SliceIcE6equalsE5SliceIcE(ptr %sret.result, ptr %1)
  ret i1 %call
}

define linkonce_odr i1 @_ZN5SliceIcE9ends_withE5SliceIcE(ptr %0, ptr %1) {
entry:
  %sret.result = alloca %_Z5SliceIcE, align 8
  %load.struct = load %_Z5SliceIcE, ptr %1, align 8
  %length = extractvalue %_Z5SliceIcE %load.struct, 0
  %load.struct1 = load %_Z5SliceIcE, ptr %0, align 8
  %length2 = extractvalue %_Z5SliceIcE %load.struct1, 0
  %gt = icmp ugt i64 %length, %length2
  br i1 %gt, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret i1 false

if.end:                                           ; preds = %entry
  %load.struct3 = load %_Z5SliceIcE, ptr %0, align 8
  %length4 = extractvalue %_Z5SliceIcE %load.struct3, 0
  %load.struct5 = load %_Z5SliceIcE, ptr %1, align 8
  %length6 = extractvalue %_Z5SliceIcE %load.struct5, 0
  %sub = sub i64 %length4, %length6
  %field.inplace = getelementptr inbounds nuw %_Z5SliceIcE, ptr %0, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  call void @_ZN5SliceIcE8subsliceEmm(ptr noalias sret(%_Z5SliceIcE) %sret.result, ptr %0, i64 %sub, i64 %field.val)
  %call = call i1 @_ZN5SliceIcE6equalsE5SliceIcE(ptr %sret.result, ptr %1)
  ret i1 %call
}

define linkonce_odr ptr @_ZN13SliceIteratorIcE4nextEv(ptr %0) {
entry:
  %load.struct = load %_Z13SliceIteratorIcE, ptr %0, align 8
  %position = extractvalue %_Z13SliceIteratorIcE %load.struct, 1
  %load.struct1 = load %_Z13SliceIteratorIcE, ptr %0, align 8
  %slice = extractvalue %_Z13SliceIteratorIcE %load.struct1, 0
  %length = extractvalue %_Z5SliceIcE %slice, 0
  %ge = icmp uge i64 %position, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %load.struct2 = load %_Z13SliceIteratorIcE, ptr %0, align 8
  %slice3 = extractvalue %_Z13SliceIteratorIcE %load.struct2, 0
  %data = extractvalue %_Z5SliceIcE %slice3, 1
  %load.struct4 = load %_Z13SliceIteratorIcE, ptr %0, align 8
  %position5 = extractvalue %_Z13SliceIteratorIcE %load.struct4, 1
  %ptr.add = getelementptr inbounds i8, ptr %data, i64 %position5
  %load.struct6 = load %_Z13SliceIteratorIcE, ptr %0, align 8
  %position7 = extractvalue %_Z13SliceIteratorIcE %load.struct6, 1
  %add = add i64 %position7, 1
  %position8 = getelementptr inbounds nuw %_Z13SliceIteratorIcE, ptr %0, i32 0, i32 1
  store i64 %add, ptr %position8, align 8
  ret ptr %ptr.add
}

define linkonce_odr void @_ZN13SliceIteratorIcEC1E5SliceIcE(ptr %0, ptr %1) {
entry:
  %slice = getelementptr inbounds nuw %_Z13SliceIteratorIcE, ptr %0, i32 0, i32 0
  %field.load = load %_Z5SliceIcE, ptr %1, align 8
  store %_Z5SliceIcE %field.load, ptr %slice, align 8
  %position = getelementptr inbounds nuw %_Z13SliceIteratorIcE, ptr %0, i32 0, i32 1
  store i64 0, ptr %position, align 8
  ret void
}

define linkonce_odr void @_ZN5SliceIcE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z13SliceIteratorIcE) %0, ptr %1, ptr %2) {
entry:
  %struct.init = alloca %_Z13SliceIteratorIcE, align 8
  call void @_ZN13SliceIteratorIcEC1E5SliceIcE(ptr %struct.init, ptr %2)
  %sret.body = load %_Z13SliceIteratorIcE, ptr %struct.init, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %struct.init, i64 ptrtoint (ptr getelementptr (%_Z13SliceIteratorIcE, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN5SliceIcEC1Ev(ptr %0) {
entry:
  %data = getelementptr inbounds nuw %_Z5SliceIcE, ptr %0, i32 0, i32 1
  store ptr null, ptr %data, align 8
  %length = getelementptr inbounds nuw %_Z5SliceIcE, ptr %0, i32 0, i32 0
  store i64 0, ptr %length, align 8
  ret void
}

define linkonce_odr void @_ZN10JsonReaderB4json7messageEi(ptr noalias sret(%_Z5SliceIcE) %0, i64 %1) {
entry:
  %tuple = alloca %_Z5SliceIcE, align 8
  %match.cmp = icmp eq i64 %1, 0
  br i1 %match.cmp, label %match.case, label %match.next

match.end:                                        ; No predecessors!
  ret void

match.case:                                       ; preds = %entry
  %tuple.field = getelementptr inbounds nuw %_Z5SliceIcE, ptr %tuple, i32 0, i32 0
  store i64 8, ptr %tuple.field, align 1
  %tuple.field1 = getelementptr inbounds nuw %_Z5SliceIcE, ptr %tuple, i32 0, i32 1
  store ptr @.str.6, ptr %tuple.field1, align 1
  %tuple.val = load %_Z5SliceIcE, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z5SliceIcE, ptr null, i32 1) to i64), i1 false)
  ret void

match.next:                                       ; preds = %entry
  %match.cmp4 = icmp eq i64 %1, 1
  br i1 %match.cmp4, label %match.case2, label %match.next3

match.case2:                                      ; preds = %match.next
  %tuple.field5 = getelementptr inbounds nuw %_Z5SliceIcE, ptr %tuple, i32 0, i32 0
  store i64 30, ptr %tuple.field5, align 1
  %tuple.field6 = getelementptr inbounds nuw %_Z5SliceIcE, ptr %tuple, i32 0, i32 1
  store ptr @.str.7, ptr %tuple.field6, align 1
  %tuple.val7 = load %_Z5SliceIcE, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z5SliceIcE, ptr null, i32 1) to i64), i1 false)
  ret void

match.next3:                                      ; preds = %match.next
  %match.cmp10 = icmp eq i64 %1, 2
  br i1 %match.cmp10, label %match.case8, label %match.next9

match.case8:                                      ; preds = %match.next3
  %tuple.field11 = getelementptr inbounds nuw %_Z5SliceIcE, ptr %tuple, i32 0, i32 0
  store i64 37, ptr %tuple.field11, align 1
  %tuple.field12 = getelementptr inbounds nuw %_Z5SliceIcE, ptr %tuple, i32 0, i32 1
  store ptr @.str.8, ptr %tuple.field12, align 1
  %tuple.val13 = load %_Z5SliceIcE, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z5SliceIcE, ptr null, i32 1) to i64), i1 false)
  ret void

match.next9:                                      ; preds = %match.next3
  %match.cmp16 = icmp eq i64 %1, 3
  br i1 %match.cmp16, label %match.case14, label %match.next15

match.case14:                                     ; preds = %match.next9
  %tuple.field17 = getelementptr inbounds nuw %_Z5SliceIcE, ptr %tuple, i32 0, i32 0
  store i64 20, ptr %tuple.field17, align 1
  %tuple.field18 = getelementptr inbounds nuw %_Z5SliceIcE, ptr %tuple, i32 0, i32 1
  store ptr @.str.9, ptr %tuple.field18, align 1
  %tuple.val19 = load %_Z5SliceIcE, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z5SliceIcE, ptr null, i32 1) to i64), i1 false)
  ret void

match.next15:                                     ; preds = %match.next9
  %match.cmp22 = icmp eq i64 %1, 4
  br i1 %match.cmp22, label %match.case20, label %match.next21

match.case20:                                     ; preds = %match.next15
  %tuple.field23 = getelementptr inbounds nuw %_Z5SliceIcE, ptr %tuple, i32 0, i32 0
  store i64 37, ptr %tuple.field23, align 1
  %tuple.field24 = getelementptr inbounds nuw %_Z5SliceIcE, ptr %tuple, i32 0, i32 1
  store ptr @.str.10, ptr %tuple.field24, align 1
  %tuple.val25 = load %_Z5SliceIcE, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z5SliceIcE, ptr null, i32 1) to i64), i1 false)
  ret void

match.next21:                                     ; preds = %match.next15
  %match.cmp28 = icmp eq i64 %1, 5
  br i1 %match.cmp28, label %match.case26, label %match.next27

match.case26:                                     ; preds = %match.next21
  %tuple.field29 = getelementptr inbounds nuw %_Z5SliceIcE, ptr %tuple, i32 0, i32 0
  store i64 38, ptr %tuple.field29, align 1
  %tuple.field30 = getelementptr inbounds nuw %_Z5SliceIcE, ptr %tuple, i32 0, i32 1
  store ptr @.str.11, ptr %tuple.field30, align 1
  %tuple.val31 = load %_Z5SliceIcE, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z5SliceIcE, ptr null, i32 1) to i64), i1 false)
  ret void

match.next27:                                     ; preds = %match.next21
  %match.cmp34 = icmp eq i64 %1, 6
  br i1 %match.cmp34, label %match.case32, label %match.next33

match.case32:                                     ; preds = %match.next27
  %tuple.field35 = getelementptr inbounds nuw %_Z5SliceIcE, ptr %tuple, i32 0, i32 0
  store i64 39, ptr %tuple.field35, align 1
  %tuple.field36 = getelementptr inbounds nuw %_Z5SliceIcE, ptr %tuple, i32 0, i32 1
  store ptr @.str.12, ptr %tuple.field36, align 1
  %tuple.val37 = load %_Z5SliceIcE, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z5SliceIcE, ptr null, i32 1) to i64), i1 false)
  ret void

match.next33:                                     ; preds = %match.next27
  %match.cmp40 = icmp eq i64 %1, 7
  br i1 %match.cmp40, label %match.case38, label %match.next39

match.case38:                                     ; preds = %match.next33
  %tuple.field41 = getelementptr inbounds nuw %_Z5SliceIcE, ptr %tuple, i32 0, i32 0
  store i64 16, ptr %tuple.field41, align 1
  %tuple.field42 = getelementptr inbounds nuw %_Z5SliceIcE, ptr %tuple, i32 0, i32 1
  store ptr @.str.13, ptr %tuple.field42, align 1
  %tuple.val43 = load %_Z5SliceIcE, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z5SliceIcE, ptr null, i32 1) to i64), i1 false)
  ret void

match.next39:                                     ; preds = %match.next33
  %match.cmp46 = icmp eq i64 %1, 8
  br i1 %match.cmp46, label %match.case44, label %match.next45

match.case44:                                     ; preds = %match.next39
  %tuple.field47 = getelementptr inbounds nuw %_Z5SliceIcE, ptr %tuple, i32 0, i32 0
  store i64 33, ptr %tuple.field47, align 1
  %tuple.field48 = getelementptr inbounds nuw %_Z5SliceIcE, ptr %tuple, i32 0, i32 1
  store ptr @.str.14, ptr %tuple.field48, align 1
  %tuple.val49 = load %_Z5SliceIcE, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z5SliceIcE, ptr null, i32 1) to i64), i1 false)
  ret void

match.next45:                                     ; preds = %match.next39
  %match.cmp52 = icmp eq i64 %1, 9
  br i1 %match.cmp52, label %match.case50, label %match.next51

match.case50:                                     ; preds = %match.next45
  %tuple.field53 = getelementptr inbounds nuw %_Z5SliceIcE, ptr %tuple, i32 0, i32 0
  store i64 26, ptr %tuple.field53, align 1
  %tuple.field54 = getelementptr inbounds nuw %_Z5SliceIcE, ptr %tuple, i32 0, i32 1
  store ptr @.str.15, ptr %tuple.field54, align 1
  %tuple.val55 = load %_Z5SliceIcE, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z5SliceIcE, ptr null, i32 1) to i64), i1 false)
  ret void

match.next51:                                     ; preds = %match.next45
  %match.cmp58 = icmp eq i64 %1, 10
  br i1 %match.cmp58, label %match.case56, label %match.next57

match.case56:                                     ; preds = %match.next51
  %tuple.field59 = getelementptr inbounds nuw %_Z5SliceIcE, ptr %tuple, i32 0, i32 0
  store i64 16, ptr %tuple.field59, align 1
  %tuple.field60 = getelementptr inbounds nuw %_Z5SliceIcE, ptr %tuple, i32 0, i32 1
  store ptr @.str.16, ptr %tuple.field60, align 1
  %tuple.val61 = load %_Z5SliceIcE, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z5SliceIcE, ptr null, i32 1) to i64), i1 false)
  ret void

match.next57:                                     ; preds = %match.next51
  %tuple.field62 = getelementptr inbounds nuw %_Z5SliceIcE, ptr %tuple, i32 0, i32 0
  store i64 13, ptr %tuple.field62, align 1
  %tuple.field63 = getelementptr inbounds nuw %_Z5SliceIcE, ptr %tuple, i32 0, i32 1
  store ptr @.str.17, ptr %tuple.field63, align 1
  %tuple.val64 = load %_Z5SliceIcE, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z5SliceIcE, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN10JsonReaderB4json13error_messageEv(ptr noalias sret(%_Z5SliceIcE) %0, ptr %1) {
entry:
  %sret.result = alloca %_Z5SliceIcE, align 8
  %field.inplace = getelementptr inbounds nuw %_Z10JsonReader, ptr %1, i32 0, i32 13
  %field.val = load i64, ptr %field.inplace, align 8
  call void @_ZN10JsonReaderB4json7messageEi(ptr noalias sret(%_Z5SliceIcE) %sret.result, i64 %field.val)
  %sret.body = load %_Z5SliceIcE, ptr %sret.result, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %sret.result, i64 ptrtoint (ptr getelementptr (%_Z5SliceIcE, ptr null, i32 1) to i64), i1 false)
  ret void
}

declare void @_ZN13StringBuilder6appendEPcm(ptr, ptr, i64)

define linkonce_odr i64 @_ZN10JsonReaderB4json4hex4E5SliceI2u8Em(ptr %0, i64 %1) {
entry:
  %v = alloca i64, align 8
  store i64 0, ptr %v, align 1
  %k = alloca i64, align 8
  store i64 0, ptr %k, align 1
  br label %while.cond

while.cond:                                       ; preds = %while.body, %entry
  %k1 = load i64, ptr %k, align 8
  %lt = icmp ult i64 %k1, 4
  br i1 %lt, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %v2 = load i64, ptr %v, align 8
  %shl = shl i64 %v2, 4
  %k3 = load i64, ptr %k, align 8
  %add = add i64 %1, %k3
  %call = call i8 @_ZN5SliceI2u8EixEm(ptr %0, i64 %add)
  %call4 = call i64 @_ZN10JsonReaderB4json9hex_digitE2u8(i8 %call)
  %add5 = add i64 %shl, %call4
  store i64 %add5, ptr %v, align 1
  %k6 = load i64, ptr %k, align 8
  %add7 = add i64 %k6, 1
  store i64 %add7, ptr %k, align 1
  br label %while.cond

while.exit:                                       ; preds = %while.cond
  %v8 = load i64, ptr %v, align 8
  ret i64 %v8
}

define linkonce_odr void @_ZN10JsonReaderB4json11append_utf8ER13StringBuilder3i64(ptr %0, i64 %1) {
entry:
  %lt = icmp slt i64 %1, 128
  br i1 %lt, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %as.trunc = trunc i64 %1 to i8
  call void @_ZN13StringBuilder6appendEc(ptr %0, i8 %as.trunc)
  ret void

if.end:                                           ; preds = %entry
  %lt1 = icmp slt i64 %1, 2048
  br i1 %lt1, label %if.then2, label %if.end3

if.then2:                                         ; preds = %if.end
  %ashr = ashr i64 %1, 6
  %or = or i64 192, %ashr
  %as.trunc4 = trunc i64 %or to i8
  call void @_ZN13StringBuilder6appendEc(ptr %0, i8 %as.trunc4)
  %and = and i64 %1, 63
  %or5 = or i64 128, %and
  %as.trunc6 = trunc i64 %or5 to i8
  call void @_ZN13StringBuilder6appendEc(ptr %0, i8 %as.trunc6)
  ret void

if.end3:                                          ; preds = %if.end
  %lt7 = icmp slt i64 %1, 65536
  br i1 %lt7, label %if.then8, label %if.end9

if.then8:                                         ; preds = %if.end3
  %ashr10 = ashr i64 %1, 12
  %or11 = or i64 224, %ashr10
  %as.trunc12 = trunc i64 %or11 to i8
  call void @_ZN13StringBuilder6appendEc(ptr %0, i8 %as.trunc12)
  %ashr13 = ashr i64 %1, 6
  %and14 = and i64 %ashr13, 63
  %or15 = or i64 128, %and14
  %as.trunc16 = trunc i64 %or15 to i8
  call void @_ZN13StringBuilder6appendEc(ptr %0, i8 %as.trunc16)
  %and17 = and i64 %1, 63
  %or18 = or i64 128, %and17
  %as.trunc19 = trunc i64 %or18 to i8
  call void @_ZN13StringBuilder6appendEc(ptr %0, i8 %as.trunc19)
  ret void

if.end9:                                          ; preds = %if.end3
  %ashr20 = ashr i64 %1, 18
  %or21 = or i64 240, %ashr20
  %as.trunc22 = trunc i64 %or21 to i8
  call void @_ZN13StringBuilder6appendEc(ptr %0, i8 %as.trunc22)
  %ashr23 = ashr i64 %1, 12
  %and24 = and i64 %ashr23, 63
  %or25 = or i64 128, %and24
  %as.trunc26 = trunc i64 %or25 to i8
  call void @_ZN13StringBuilder6appendEc(ptr %0, i8 %as.trunc26)
  %ashr27 = ashr i64 %1, 6
  %and28 = and i64 %ashr27, 63
  %or29 = or i64 128, %and28
  %as.trunc30 = trunc i64 %or29 to i8
  call void @_ZN13StringBuilder6appendEc(ptr %0, i8 %as.trunc30)
  %and31 = and i64 %1, 63
  %or32 = or i64 128, %and31
  %as.trunc33 = trunc i64 %or32 to i8
  call void @_ZN13StringBuilder6appendEc(ptr %0, i8 %as.trunc33)
  ret void
}

define linkonce_odr i8 @_ZN10JsonReaderB4json8unescapeE2u8(i8 %0) {
entry:
  %match.cmp = icmp eq i8 %0, 98
  br i1 %match.cmp, label %match.case, label %match.next

match.end:                                        ; No predecessors!
  ret i8 0

match.case:                                       ; preds = %entry
  ret i8 8

match.next:                                       ; preds = %entry
  %match.cmp3 = icmp eq i8 %0, 102
  br i1 %match.cmp3, label %match.case1, label %match.next2

match.case1:                                      ; preds = %match.next
  ret i8 12

match.next2:                                      ; preds = %match.next
  %match.cmp6 = icmp eq i8 %0, 110
  br i1 %match.cmp6, label %match.case4, label %match.next5

match.case4:                                      ; preds = %match.next2
  ret i8 10

match.next5:                                      ; preds = %match.next2
  %match.cmp9 = icmp eq i8 %0, 114
  br i1 %match.cmp9, label %match.case7, label %match.next8

match.case7:                                      ; preds = %match.next5
  ret i8 13

match.next8:                                      ; preds = %match.next5
  %match.cmp12 = icmp eq i8 %0, 116
  br i1 %match.cmp12, label %match.case10, label %match.next11

match.case10:                                     ; preds = %match.next8
  ret i8 9

match.next11:                                     ; preds = %match.next8
  ret i8 %0
}

declare void @_ZN13StringBuilder6appendEc(ptr, i8)

declare void @_ZN13StringBuilder9to_stringEPN4scaly6memory4PageE(ptr noalias sret({ ptr }), ptr, ptr)

define linkonce_odr i64 @_ZN10JsonReaderB4json9hex_digitE2u8(i8 %0) {
entry:
  %ge = icmp uge i8 %0, 48
  br i1 %ge, label %land.rhs, label %if.end

if.then:                                          ; preds = %land.rhs
  %sub = sub i8 %0, 48
  %as.zext = zext i8 %sub to i64
  ret i64 %as.zext

if.end:                                           ; preds = %land.rhs, %entry
  %ge4 = icmp uge i8 %0, 97
  br i1 %ge4, label %land.rhs3, label %if.end2

land.rhs:                                         ; preds = %entry
  %le = icmp ule i8 %0, 57
  br i1 %le, label %if.then, label %if.end

if.then1:                                         ; preds = %land.rhs3
  %sub6 = sub i8 %0, 97
  %add = add i8 %sub6, 10
  %as.zext7 = zext i8 %add to i64
  ret i64 %as.zext7

if.end2:                                          ; preds = %land.rhs3, %if.end
  %ge11 = icmp uge i8 %0, 65
  br i1 %ge11, label %land.rhs10, label %if.end9

land.rhs3:                                        ; preds = %if.end
  %le5 = icmp ule i8 %0, 102
  br i1 %le5, label %if.then1, label %if.end2

if.then8:                                         ; preds = %land.rhs10
  %sub13 = sub i8 %0, 65
  %add14 = add i8 %sub13, 10
  %as.zext15 = zext i8 %add14 to i64
  ret i64 %as.zext15

if.end9:                                          ; preds = %land.rhs10, %if.end2
  ret i64 -1

land.rhs10:                                       ; preds = %if.end2
  %le12 = icmp ule i8 %0, 70
  br i1 %le12, label %if.then8, label %if.end9
}

define linkonce_odr i1 @_ZN10JsonReaderB4json4openEb(ptr %0, i1 %1) {
entry:
  %sret.result = alloca %_Z9JsonToken, align 8
  %load.struct = load %_Z10JsonReader, ptr %0, align 8
  %depth = extractvalue %_Z10JsonReader %load.struct, 2
  %load.struct1 = load %_Z10JsonReader, ptr %0, align 8
  %max_depth = extractvalue %_Z10JsonReader %load.struct1, 1
  %ge = icmp uge i64 %depth, %max_depth
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  call void @_ZN10JsonReaderB4json4failEPN4scaly6memory4PageEi(ptr noalias sret(%_Z9JsonToken) %sret.result, ptr null, ptr %0, i64 10)
  ret i1 false

if.end:                                           ; preds = %entry
  %load.struct2 = load %_Z10JsonReader, ptr %0, align 8
  %depth3 = extractvalue %_Z10JsonReader %load.struct2, 2
  %and = and i64 %depth3, 63
  %shl = shl i64 1, %and
  %load.struct4 = load %_Z10JsonReader, ptr %0, align 8
  %depth5 = extractvalue %_Z10JsonReader %load.struct4, 2
  %lshr = lshr i64 %depth5, 6
  %eq = icmp eq i64 %lshr, 0
  br i1 %eq, label %if.then6, label %if.else

if.then6:                                         ; preds = %if.end
  br i1 %1, label %if.then8, label %if.else9

if.else:                                          ; preds = %if.end
  %eq17 = icmp eq i64 %lshr, 1
  br i1 %eq17, label %if.then18, label %if.else19

if.end7:                                          ; preds = %if.end20, %if.end10
  %if.value63 = phi i64 [ %if.value, %if.end10 ], [ %if.value62, %if.end20 ]
  %load.struct64 = load %_Z10JsonReader, ptr %0, align 8
  %depth65 = extractvalue %_Z10JsonReader %load.struct64, 2
  %add = add i64 %depth65, 1
  %depth66 = getelementptr inbounds nuw %_Z10JsonReader, ptr %0, i32 0, i32 2
  store i64 %add, ptr %depth66, align 8
  %load.struct67 = load %_Z10JsonReader, ptr %0, align 8
  %pos = extractvalue %_Z10JsonReader %load.struct67, 7
  %add68 = add i64 %pos, 1
  %pos69 = getelementptr inbounds nuw %_Z10JsonReader, ptr %0, i32 0, i32 7
  store i64 %add68, ptr %pos69, align 8
  ret i1 true

if.then8:                                         ; preds = %if.then6
  %load.struct11 = load %_Z10JsonReader, ptr %0, align 8
  %levels0 = extractvalue %_Z10JsonReader %load.struct11, 3
  %or = or i64 %levels0, %shl
  %levels012 = getelementptr inbounds nuw %_Z10JsonReader, ptr %0, i32 0, i32 3
  store i64 %or, ptr %levels012, align 8
  br label %if.end10

if.else9:                                         ; preds = %if.then6
  %load.struct13 = load %_Z10JsonReader, ptr %0, align 8
  %levels014 = extractvalue %_Z10JsonReader %load.struct13, 3
  %xor = xor i64 %shl, -1
  %and15 = and i64 %levels014, %xor
  %levels016 = getelementptr inbounds nuw %_Z10JsonReader, ptr %0, i32 0, i32 3
  store i64 %and15, ptr %levels016, align 8
  br label %if.end10

if.end10:                                         ; preds = %if.else9, %if.then8
  %if.value = phi i64 [ %or, %if.then8 ], [ %and15, %if.else9 ]
  br label %if.end7

if.then18:                                        ; preds = %if.else
  br i1 %1, label %if.then21, label %if.else22

if.else19:                                        ; preds = %if.else
  %eq33 = icmp eq i64 %lshr, 2
  br i1 %eq33, label %if.then34, label %if.else35

if.end20:                                         ; preds = %if.end36, %if.end23
  %if.value62 = phi i64 [ %if.value32, %if.end23 ], [ %if.value61, %if.end36 ]
  br label %if.end7

if.then21:                                        ; preds = %if.then18
  %load.struct24 = load %_Z10JsonReader, ptr %0, align 8
  %levels1 = extractvalue %_Z10JsonReader %load.struct24, 4
  %or25 = or i64 %levels1, %shl
  %levels126 = getelementptr inbounds nuw %_Z10JsonReader, ptr %0, i32 0, i32 4
  store i64 %or25, ptr %levels126, align 8
  br label %if.end23

if.else22:                                        ; preds = %if.then18
  %load.struct27 = load %_Z10JsonReader, ptr %0, align 8
  %levels128 = extractvalue %_Z10JsonReader %load.struct27, 4
  %xor29 = xor i64 %shl, -1
  %and30 = and i64 %levels128, %xor29
  %levels131 = getelementptr inbounds nuw %_Z10JsonReader, ptr %0, i32 0, i32 4
  store i64 %and30, ptr %levels131, align 8
  br label %if.end23

if.end23:                                         ; preds = %if.else22, %if.then21
  %if.value32 = phi i64 [ %or25, %if.then21 ], [ %and30, %if.else22 ]
  br label %if.end20

if.then34:                                        ; preds = %if.else19
  br i1 %1, label %if.then37, label %if.else38

if.else35:                                        ; preds = %if.else19
  br i1 %1, label %if.then49, label %if.else50

if.end36:                                         ; preds = %if.end51, %if.end39
  %if.value61 = phi i64 [ %if.value48, %if.end39 ], [ %if.value60, %if.end51 ]
  br label %if.end20

if.then37:                                        ; preds = %if.then34
  %load.struct40 = load %_Z10JsonReader, ptr %0, align 8
  %levels2 = extractvalue %_Z10JsonReader %load.struct40, 5
  %or41 = or i64 %levels2, %shl
  %levels242 = getelementptr inbounds nuw %_Z10JsonReader, ptr %0, i32 0, i32 5
  store i64 %or41, ptr %levels242, align 8
  br label %if.end39

if.else38:                                        ; preds = %if.then34
  %load.struct43 = load %_Z10JsonReader, ptr %0, align 8
  %levels244 = extractvalue %_Z10JsonReader %load.struct43, 5
  %xor45 = xor i64 %shl, -1
  %and46 = and i64 %levels244, %xor45
  %levels247 = getelementptr inbounds nuw %_Z10JsonReader, ptr %0, i32 0, i32 5
  store i64 %and46, ptr %levels247, align 8
  br label %if.end39

if.end39:                                         ; preds = %if.else38, %if.then37
  %if.value48 = phi i64 [ %or41, %if.then37 ], [ %and46, %if.else38 ]
  br label %if.end36

if.then49:                                        ; preds = %if.else35
  %load.struct52 = load %_Z10JsonReader, ptr %0, align 8
  %levels3 = extractvalue %_Z10JsonReader %load.struct52, 6
  %or53 = or i64 %levels3, %shl
  %levels354 = getelementptr inbounds nuw %_Z10JsonReader, ptr %0, i32 0, i32 6
  store i64 %or53, ptr %levels354, align 8
  br label %if.end51

if.else50:                                        ; preds = %if.else35
  %load.struct55 = load %_Z10JsonReader, ptr %0, align 8
  %levels356 = extractvalue %_Z10JsonReader %load.struct55, 6
  %xor57 = xor i64 %shl, -1
  %and58 = and i64 %levels356, %xor57
  %levels359 = getelementptr inbounds nuw %_Z10JsonReader, ptr %0, i32 0, i32 6
  store i64 %and58, ptr %levels359, align 8
  br label %if.end51

if.end51:                                         ; preds = %if.else50, %if.then49
  %if.value60 = phi i64 [ %or53, %if.then49 ], [ %and58, %if.else50 ]
  br label %if.end36
}

define linkonce_odr void @_ZN10JsonReaderB4json11after_valueEv(ptr %0) {
entry:
  %load.struct = load %_Z10JsonReader, ptr %0, align 8
  %depth = extractvalue %_Z10JsonReader %load.struct, 2
  %eq = icmp eq i64 %depth, 0
  br i1 %eq, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %state = getelementptr inbounds nuw %_Z10JsonReader, ptr %0, i32 0, i32 8
  store i64 5, ptr %state, align 8
  br label %if.end

if.else:                                          ; preds = %entry
  %state1 = getelementptr inbounds nuw %_Z10JsonReader, ptr %0, i32 0, i32 8
  store i64 4, ptr %state1, align 8
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  %if.value = phi i64 [ 5, %if.then ], [ 4, %if.else ]
  ret void
}

define linkonce_odr i1 @_ZN10JsonReaderB4json11scan_numberEv(ptr %0) {
entry:
  %sret.result3 = alloca %_Z9JsonToken, align 8
  %sret.result = alloca %_Z8JsonScan, align 8
  %field.inplace = getelementptr inbounds nuw %_Z10JsonReader, ptr %0, i32 0, i32 0
  %field.inplace1 = getelementptr inbounds nuw %_Z10JsonReader, ptr %0, i32 0, i32 7
  %field.val = load i64, ptr %field.inplace1, align 8
  call void @_ZN10JsonReaderB4json10number_endEPN4scaly6memory4PageE5SliceI2u8Em(ptr noalias sret(%_Z8JsonScan) %sret.result, ptr null, ptr %field.inplace, i64 %field.val)
  %load.struct = load %_Z8JsonScan, ptr %sret.result, align 8
  %code = extractvalue %_Z8JsonScan %load.struct, 0
  %ne = icmp ne i64 %code, 0
  br i1 %ne, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %load.struct2 = load %_Z8JsonScan, ptr %sret.result, align 8
  %at = extractvalue %_Z8JsonScan %load.struct2, 1
  %pos = getelementptr inbounds nuw %_Z10JsonReader, ptr %0, i32 0, i32 7
  store i64 %at, ptr %pos, align 8
  %field.inplace4 = getelementptr inbounds nuw %_Z8JsonScan, ptr %sret.result, i32 0, i32 0
  %field.val5 = load i64, ptr %field.inplace4, align 8
  call void @_ZN10JsonReaderB4json4failEPN4scaly6memory4PageEi(ptr noalias sret(%_Z9JsonToken) %sret.result3, ptr null, ptr %0, i64 %field.val5)
  ret i1 false

if.end:                                           ; preds = %entry
  %load.struct6 = load %_Z10JsonReader, ptr %0, align 8
  %pos7 = extractvalue %_Z10JsonReader %load.struct6, 7
  %start = getelementptr inbounds nuw %_Z10JsonReader, ptr %0, i32 0, i32 9
  store i64 %pos7, ptr %start, align 8
  %load.struct8 = load %_Z8JsonScan, ptr %sret.result, align 8
  %at9 = extractvalue %_Z8JsonScan %load.struct8, 1
  %end = getelementptr inbounds nuw %_Z10JsonReader, ptr %0, i32 0, i32 10
  store i64 %at9, ptr %end, align 8
  %load.struct10 = load %_Z8JsonScan, ptr %sret.result, align 8
  %flag = extractvalue %_Z8JsonScan %load.struct10, 2
  %integral = getelementptr inbounds nuw %_Z10JsonReader, ptr %0, i32 0, i32 12
  store i1 %flag, ptr %integral, align 1
  %load.struct11 = load %_Z8JsonScan, ptr %sret.result, align 8
  %at12 = extractvalue %_Z8JsonScan %load.struct11, 1
  %pos13 = getelementptr inbounds nuw %_Z10JsonReader, ptr %0, i32 0, i32 7
  store i64 %at12, ptr %pos13, align 8
  ret i1 true
}

define linkonce_odr i1 @_ZN10JsonReaderB4json7literalE5SliceI2u8E(ptr %0, ptr %1) {
entry:
  %sret.result = alloca %_Z5SliceI2u8E, align 8
  %load.struct = load %_Z10JsonReader, ptr %0, align 8
  %pos = extractvalue %_Z10JsonReader %load.struct, 7
  %load.struct1 = load %_Z5SliceI2u8E, ptr %1, align 8
  %length = extractvalue %_Z5SliceI2u8E %load.struct1, 0
  %add = add i64 %pos, %length
  %load.struct2 = load %_Z10JsonReader, ptr %0, align 8
  %source = extractvalue %_Z10JsonReader %load.struct2, 0
  %length3 = extractvalue %_Z5SliceI2u8E %source, 0
  %gt = icmp ugt i64 %add, %length3
  br i1 %gt, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret i1 false

if.end:                                           ; preds = %entry
  %field.inplace = getelementptr inbounds nuw %_Z10JsonReader, ptr %0, i32 0, i32 0
  %field.inplace4 = getelementptr inbounds nuw %_Z10JsonReader, ptr %0, i32 0, i32 7
  %field.val = load i64, ptr %field.inplace4, align 8
  %load.struct5 = load %_Z10JsonReader, ptr %0, align 8
  %pos6 = extractvalue %_Z10JsonReader %load.struct5, 7
  %load.struct7 = load %_Z5SliceI2u8E, ptr %1, align 8
  %length8 = extractvalue %_Z5SliceI2u8E %load.struct7, 0
  %add9 = add i64 %pos6, %length8
  call void @_ZN5SliceI2u8E8subsliceEmm(ptr noalias sret(%_Z5SliceI2u8E) %sret.result, ptr %field.inplace, i64 %field.val, i64 %add9)
  %call = call i1 @_ZN5SliceI2u8E6equalsE5SliceI2u8E(ptr %sret.result, ptr %1)
  %eq = icmp eq i1 %call, false
  br i1 %eq, label %if.then10, label %if.end11

if.then10:                                        ; preds = %if.end
  ret i1 false

if.end11:                                         ; preds = %if.end
  %load.struct12 = load %_Z10JsonReader, ptr %0, align 8
  %pos13 = extractvalue %_Z10JsonReader %load.struct12, 7
  %load.struct14 = load %_Z5SliceI2u8E, ptr %1, align 8
  %length15 = extractvalue %_Z5SliceI2u8E %load.struct14, 0
  %add16 = add i64 %pos13, %length15
  %pos17 = getelementptr inbounds nuw %_Z10JsonReader, ptr %0, i32 0, i32 7
  store i64 %add16, ptr %pos17, align 8
  call void @_ZN10JsonReaderB4json11after_valueEv(ptr %0)
  ret i1 true
}

define linkonce_odr void @_ZN10JsonReaderB4json12string_closeEPN4scaly6memory4PageE5SliceI2u8Em(ptr noalias sret(%_Z8JsonScan) %0, ptr %1, ptr %2, i64 %3) {
entry:
  %k = alloca i64, align 8
  %tuple = alloca %_Z8JsonScan, align 8
  %add = add i64 %3, 1
  %p = alloca i64, align 8
  store i64 %add, ptr %p, align 1
  %escaped = alloca i1, align 1
  store i1 false, ptr %escaped, align 1
  %p1 = load i64, ptr %p, align 8
  %add2 = add i64 %p1, 16
  br label %while.cond

while.cond:                                       ; preds = %if.end71, %if.then29, %entry
  %p3 = load i64, ptr %p, align 8
  %load.struct = load %_Z5SliceI2u8E, ptr %2, align 8
  %length = extractvalue %_Z5SliceI2u8E %load.struct, 0
  %lt = icmp ult i64 %p3, %length
  br i1 %lt, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  br label %while.cond4

while.exit:                                       ; preds = %if.then25, %while.cond
  %p132 = load i64, ptr %p, align 8
  %escaped133 = load i1, ptr %escaped, align 1
  %tuple.field134 = getelementptr inbounds nuw %_Z8JsonScan, ptr %tuple, i32 0, i32 0
  store i64 1, ptr %tuple.field134, align 1
  %tuple.field135 = getelementptr inbounds nuw %_Z8JsonScan, ptr %tuple, i32 0, i32 1
  store i64 %p132, ptr %tuple.field135, align 1
  %tuple.field136 = getelementptr inbounds nuw %_Z8JsonScan, ptr %tuple, i32 0, i32 2
  store i1 %escaped133, ptr %tuple.field136, align 1
  %tuple.val137 = load %_Z8JsonScan, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z8JsonScan, ptr null, i32 1) to i64), i1 false)
  ret void

while.cond4:                                      ; preds = %if.end, %while.body
  %p7 = load i64, ptr %p, align 8
  %ge = icmp uge i64 %p7, %add2
  br i1 %ge, label %lor.rhs, label %lor.end

while.body5:                                      ; preds = %lor.end
  %p12 = load i64, ptr %p, align 8
  %simd.cont = getelementptr inbounds nuw %_Z5SliceI2u8E, ptr %2, i32 0, i32 0
  %simd.len = load i64, ptr %simd.cont, align 8
  %simd.cont13 = getelementptr inbounds nuw %_Z5SliceI2u8E, ptr %2, i32 0, i32 1
  %simd.data = load ptr, ptr %simd.cont13, align 8
  %simd.fits = icmp uge i64 %simd.len, 16
  %simd.room = sub i64 %simd.len, 16
  %simd.within = icmp ule i64 %p12, %simd.room
  %simd.inrange = and i1 %simd.fits, %simd.within
  br i1 %simd.inrange, label %simd.mem.ok, label %simd.mem.oob

while.exit6:                                      ; preds = %if.then, %lor.end
  %p21 = load i64, ptr %p, align 8
  %load.struct22 = load %_Z5SliceI2u8E, ptr %2, align 8
  %length23 = extractvalue %_Z5SliceI2u8E %load.struct22, 0
  %ge24 = icmp uge i64 %p21, %length23
  br i1 %ge24, label %if.then25, label %if.end26

lor.rhs:                                          ; preds = %while.cond4
  %p8 = load i64, ptr %p, align 8
  %add9 = add i64 %p8, 16
  %load.struct10 = load %_Z5SliceI2u8E, ptr %2, align 8
  %length11 = extractvalue %_Z5SliceI2u8E %load.struct10, 0
  %le = icmp ule i64 %add9, %length11
  br label %lor.end

lor.end:                                          ; preds = %lor.rhs, %while.cond4
  %lor.result = phi i1 [ false, %while.cond4 ], [ %le, %lor.rhs ]
  br i1 %lor.result, label %while.body5, label %while.exit6

simd.mem.oob:                                     ; preds = %while.body5
  call void @_Z15scaly_panic_oobP10const_charmm(ptr @.simd.mem.21, i64 %p12, i64 %simd.len)
  unreachable

simd.mem.ok:                                      ; preds = %while.body5
  %simd.addr = getelementptr inbounds i8, ptr %simd.data, i64 %p12
  %simd.load = load <16 x i8>, ptr %simd.addr, align 1
  %simd.cmp = icmp eq <16 x i8> %simd.load, splat (i8 34)
  %simd.cmp14 = icmp eq <16 x i8> %simd.load, splat (i8 92)
  %simd.or = or <16 x i1> %simd.cmp, %simd.cmp14
  %simd.cmp15 = icmp ult <16 x i8> %simd.load, splat (i8 32)
  %simd.or16 = or <16 x i1> %simd.or, %simd.cmp15
  %simd.bits = bitcast <16 x i1> %simd.or16 to i16
  %simd.bits64 = zext i16 %simd.bits to i64
  %ne = icmp ne i64 %simd.bits64, 0
  br i1 %ne, label %if.then, label %if.end

if.then:                                          ; preds = %simd.mem.ok
  %p17 = load i64, ptr %p, align 8
  %call = call i64 @_Z14trailing_zeros3u64(i64 %simd.bits64)
  %add18 = add i64 %p17, %call
  store i64 %add18, ptr %p, align 1
  br label %while.exit6

if.end:                                           ; preds = %simd.mem.ok
  %p19 = load i64, ptr %p, align 8
  %add20 = add i64 %p19, 16
  store i64 %add20, ptr %p, align 1
  br label %while.cond4

if.then25:                                        ; preds = %while.exit6
  br label %while.exit

if.end26:                                         ; preds = %while.exit6
  %p27 = load i64, ptr %p, align 8
  %call28 = call i8 @_ZN5SliceI2u8EixEm(ptr %2, i64 %p27)
  %ge32 = icmp uge i8 %call28, 32
  br i1 %ge32, label %land.rhs31, label %if.end30

if.then29:                                        ; preds = %land.rhs
  %p35 = load i64, ptr %p, align 8
  %add36 = add i64 %p35, 1
  store i64 %add36, ptr %p, align 1
  br label %while.cond

if.end30:                                         ; preds = %land.rhs, %land.rhs31, %if.end26
  %eq = icmp eq i8 %call28, 34
  br i1 %eq, label %if.then37, label %if.end38

land.rhs:                                         ; preds = %land.rhs31
  %ne34 = icmp ne i8 %call28, 92
  br i1 %ne34, label %if.then29, label %if.end30

land.rhs31:                                       ; preds = %if.end26
  %ne33 = icmp ne i8 %call28, 34
  br i1 %ne33, label %land.rhs, label %if.end30

if.then37:                                        ; preds = %if.end30
  %p39 = load i64, ptr %p, align 8
  %escaped40 = load i1, ptr %escaped, align 1
  %tuple.field = getelementptr inbounds nuw %_Z8JsonScan, ptr %tuple, i32 0, i32 0
  store i64 0, ptr %tuple.field, align 1
  %tuple.field41 = getelementptr inbounds nuw %_Z8JsonScan, ptr %tuple, i32 0, i32 1
  store i64 %p39, ptr %tuple.field41, align 1
  %tuple.field42 = getelementptr inbounds nuw %_Z8JsonScan, ptr %tuple, i32 0, i32 2
  store i1 %escaped40, ptr %tuple.field42, align 1
  %tuple.val = load %_Z8JsonScan, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z8JsonScan, ptr null, i32 1) to i64), i1 false)
  ret void

if.end38:                                         ; preds = %if.end30
  %lt43 = icmp ult i8 %call28, 32
  br i1 %lt43, label %if.then44, label %if.end45

if.then44:                                        ; preds = %if.end38
  %p46 = load i64, ptr %p, align 8
  %escaped47 = load i1, ptr %escaped, align 1
  %tuple.field48 = getelementptr inbounds nuw %_Z8JsonScan, ptr %tuple, i32 0, i32 0
  store i64 8, ptr %tuple.field48, align 1
  %tuple.field49 = getelementptr inbounds nuw %_Z8JsonScan, ptr %tuple, i32 0, i32 1
  store i64 %p46, ptr %tuple.field49, align 1
  %tuple.field50 = getelementptr inbounds nuw %_Z8JsonScan, ptr %tuple, i32 0, i32 2
  store i1 %escaped47, ptr %tuple.field50, align 1
  %tuple.val51 = load %_Z8JsonScan, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z8JsonScan, ptr null, i32 1) to i64), i1 false)
  ret void

if.end45:                                         ; preds = %if.end38
  store i1 true, ptr %escaped, align 1
  %p52 = load i64, ptr %p, align 8
  %add53 = add i64 %p52, 1
  %load.struct54 = load %_Z5SliceI2u8E, ptr %2, align 8
  %length55 = extractvalue %_Z5SliceI2u8E %load.struct54, 0
  %ge56 = icmp uge i64 %add53, %length55
  br i1 %ge56, label %if.then57, label %if.end58

if.then57:                                        ; preds = %if.end45
  %p59 = load i64, ptr %p, align 8
  %add60 = add i64 %p59, 1
  %escaped61 = load i1, ptr %escaped, align 1
  %tuple.field62 = getelementptr inbounds nuw %_Z8JsonScan, ptr %tuple, i32 0, i32 0
  store i64 1, ptr %tuple.field62, align 1
  %tuple.field63 = getelementptr inbounds nuw %_Z8JsonScan, ptr %tuple, i32 0, i32 1
  store i64 %add60, ptr %tuple.field63, align 1
  %tuple.field64 = getelementptr inbounds nuw %_Z8JsonScan, ptr %tuple, i32 0, i32 2
  store i1 %escaped61, ptr %tuple.field64, align 1
  %tuple.val65 = load %_Z8JsonScan, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z8JsonScan, ptr null, i32 1) to i64), i1 false)
  ret void

if.end58:                                         ; preds = %if.end45
  %p66 = load i64, ptr %p, align 8
  %add67 = add i64 %p66, 1
  %call68 = call i8 @_ZN5SliceI2u8EixEm(ptr %2, i64 %add67)
  %eq69 = icmp eq i8 %call68, 117
  br i1 %eq69, label %if.then70, label %if.else

if.then70:                                        ; preds = %if.end58
  %p72 = load i64, ptr %p, align 8
  %add73 = add i64 %p72, 6
  %load.struct74 = load %_Z5SliceI2u8E, ptr %2, align 8
  %length75 = extractvalue %_Z5SliceI2u8E %load.struct74, 0
  %gt = icmp ugt i64 %add73, %length75
  br i1 %gt, label %if.then76, label %if.end77

if.else:                                          ; preds = %if.end58
  %match.cmp = icmp eq i8 %call68, 34
  br i1 %match.cmp, label %match.case, label %match.alt

if.end71:                                         ; preds = %match.end, %while.exit87
  br label %while.cond

if.then76:                                        ; preds = %if.then70
  %load.struct78 = load %_Z5SliceI2u8E, ptr %2, align 8
  %length79 = extractvalue %_Z5SliceI2u8E %load.struct78, 0
  %escaped80 = load i1, ptr %escaped, align 1
  %tuple.field81 = getelementptr inbounds nuw %_Z8JsonScan, ptr %tuple, i32 0, i32 0
  store i64 1, ptr %tuple.field81, align 1
  %tuple.field82 = getelementptr inbounds nuw %_Z8JsonScan, ptr %tuple, i32 0, i32 1
  store i64 %length79, ptr %tuple.field82, align 1
  %tuple.field83 = getelementptr inbounds nuw %_Z8JsonScan, ptr %tuple, i32 0, i32 2
  store i1 %escaped80, ptr %tuple.field83, align 1
  %tuple.val84 = load %_Z8JsonScan, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z8JsonScan, ptr null, i32 1) to i64), i1 false)
  ret void

if.end77:                                         ; preds = %if.then70
  store i64 2, ptr %k, align 1
  br label %while.cond85

while.cond85:                                     ; preds = %if.end97, %if.end77
  %k88 = load i64, ptr %k, align 8
  %lt89 = icmp ult i64 %k88, 6
  br i1 %lt89, label %while.body86, label %while.exit87

while.body86:                                     ; preds = %while.cond85
  %p90 = load i64, ptr %p, align 8
  %k91 = load i64, ptr %k, align 8
  %add92 = add i64 %p90, %k91
  %call93 = call i8 @_ZN5SliceI2u8EixEm(ptr %2, i64 %add92)
  %call94 = call i64 @_ZN10JsonReaderB4json9hex_digitE2u8(i8 %call93)
  %lt95 = icmp slt i64 %call94, 0
  br i1 %lt95, label %if.then96, label %if.end97

while.exit87:                                     ; preds = %while.cond85
  %p108 = load i64, ptr %p, align 8
  %add109 = add i64 %p108, 6
  store i64 %add109, ptr %p, align 1
  br label %if.end71

if.then96:                                        ; preds = %while.body86
  %p98 = load i64, ptr %p, align 8
  %k99 = load i64, ptr %k, align 8
  %add100 = add i64 %p98, %k99
  %escaped101 = load i1, ptr %escaped, align 1
  %tuple.field102 = getelementptr inbounds nuw %_Z8JsonScan, ptr %tuple, i32 0, i32 0
  store i64 9, ptr %tuple.field102, align 1
  %tuple.field103 = getelementptr inbounds nuw %_Z8JsonScan, ptr %tuple, i32 0, i32 1
  store i64 %add100, ptr %tuple.field103, align 1
  %tuple.field104 = getelementptr inbounds nuw %_Z8JsonScan, ptr %tuple, i32 0, i32 2
  store i1 %escaped101, ptr %tuple.field104, align 1
  %tuple.val105 = load %_Z8JsonScan, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z8JsonScan, ptr null, i32 1) to i64), i1 false)
  ret void

if.end97:                                         ; preds = %while.body86
  %k106 = load i64, ptr %k, align 8
  %add107 = add i64 %k106, 1
  store i64 %add107, ptr %k, align 1
  br label %while.cond85

match.end:                                        ; preds = %match.case
  %match.value = phi i64 [ %add124, %match.case ]
  br label %if.end71

match.case:                                       ; preds = %match.alt121, %match.alt119, %match.alt117, %match.alt115, %match.alt113, %match.alt111, %match.alt, %if.else
  %p123 = load i64, ptr %p, align 8
  %add124 = add i64 %p123, 2
  store i64 %add124, ptr %p, align 1
  br label %match.end

match.next:                                       ; preds = %match.alt121
  %p125 = load i64, ptr %p, align 8
  %add126 = add i64 %p125, 1
  %escaped127 = load i1, ptr %escaped, align 1
  %tuple.field128 = getelementptr inbounds nuw %_Z8JsonScan, ptr %tuple, i32 0, i32 0
  store i64 9, ptr %tuple.field128, align 1
  %tuple.field129 = getelementptr inbounds nuw %_Z8JsonScan, ptr %tuple, i32 0, i32 1
  store i64 %add126, ptr %tuple.field129, align 1
  %tuple.field130 = getelementptr inbounds nuw %_Z8JsonScan, ptr %tuple, i32 0, i32 2
  store i1 %escaped127, ptr %tuple.field130, align 1
  %tuple.val131 = load %_Z8JsonScan, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z8JsonScan, ptr null, i32 1) to i64), i1 false)
  ret void

match.alt:                                        ; preds = %if.else
  %match.cmp110 = icmp eq i8 %call68, 92
  br i1 %match.cmp110, label %match.case, label %match.alt111

match.alt111:                                     ; preds = %match.alt
  %match.cmp112 = icmp eq i8 %call68, 47
  br i1 %match.cmp112, label %match.case, label %match.alt113

match.alt113:                                     ; preds = %match.alt111
  %match.cmp114 = icmp eq i8 %call68, 98
  br i1 %match.cmp114, label %match.case, label %match.alt115

match.alt115:                                     ; preds = %match.alt113
  %match.cmp116 = icmp eq i8 %call68, 102
  br i1 %match.cmp116, label %match.case, label %match.alt117

match.alt117:                                     ; preds = %match.alt115
  %match.cmp118 = icmp eq i8 %call68, 110
  br i1 %match.cmp118, label %match.case, label %match.alt119

match.alt119:                                     ; preds = %match.alt117
  %match.cmp120 = icmp eq i8 %call68, 114
  br i1 %match.cmp120, label %match.case, label %match.alt121

match.alt121:                                     ; preds = %match.alt119
  %match.cmp122 = icmp eq i8 %call68, 116
  br i1 %match.cmp122, label %match.case, label %match.next
}

define linkonce_odr void @_ZN10JsonReaderB4json10number_endEPN4scaly6memory4PageE5SliceI2u8Em(ptr noalias sret(%_Z8JsonScan) %0, ptr %1, ptr %2, i64 %3) {
entry:
  %tuple = alloca %_Z8JsonScan, align 8
  %p = alloca i64, align 8
  store i64 %3, ptr %p, align 1
  %integral = alloca i1, align 1
  store i1 true, ptr %integral, align 1
  %p1 = load i64, ptr %p, align 8
  %call = call i8 @_ZN5SliceI2u8EixEm(ptr %2, i64 %p1)
  %eq = icmp eq i8 %call, 45
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %p2 = load i64, ptr %p, align 8
  %add = add i64 %p2, 1
  store i64 %add, ptr %p, align 1
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %p3 = load i64, ptr %p, align 8
  %call4 = call i1 @_ZN10JsonReaderB4json8digit_atE5SliceI2u8Em(ptr %2, i64 %p3)
  %eq5 = icmp eq i1 %call4, false
  br i1 %eq5, label %if.then6, label %if.end7

if.then6:                                         ; preds = %if.end
  %p8 = load i64, ptr %p, align 8
  %tuple.field = getelementptr inbounds nuw %_Z8JsonScan, ptr %tuple, i32 0, i32 0
  store i64 7, ptr %tuple.field, align 1
  %tuple.field9 = getelementptr inbounds nuw %_Z8JsonScan, ptr %tuple, i32 0, i32 1
  store i64 %p8, ptr %tuple.field9, align 1
  %tuple.field10 = getelementptr inbounds nuw %_Z8JsonScan, ptr %tuple, i32 0, i32 2
  store i1 false, ptr %tuple.field10, align 1
  %tuple.val = load %_Z8JsonScan, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z8JsonScan, ptr null, i32 1) to i64), i1 false)
  ret void

if.end7:                                          ; preds = %if.end
  %p11 = load i64, ptr %p, align 8
  %call12 = call i8 @_ZN5SliceI2u8EixEm(ptr %2, i64 %p11)
  %eq13 = icmp eq i8 %call12, 48
  br i1 %eq13, label %if.then14, label %if.else

if.then14:                                        ; preds = %if.end7
  %p16 = load i64, ptr %p, align 8
  %add17 = add i64 %p16, 1
  store i64 %add17, ptr %p, align 1
  %p18 = load i64, ptr %p, align 8
  %call19 = call i1 @_ZN10JsonReaderB4json8digit_atE5SliceI2u8Em(ptr %2, i64 %p18)
  br i1 %call19, label %if.then20, label %if.end21

if.else:                                          ; preds = %if.end7
  %p27 = load i64, ptr %p, align 8
  %call28 = call i64 @_ZN10JsonReaderB4json10digits_endE5SliceI2u8Em(ptr %2, i64 %p27)
  store i64 %call28, ptr %p, align 1
  br label %if.end15

if.end15:                                         ; preds = %if.else, %if.end21
  %p31 = load i64, ptr %p, align 8
  %load.struct = load %_Z5SliceI2u8E, ptr %2, align 8
  %length = extractvalue %_Z5SliceI2u8E %load.struct, 0
  %lt = icmp ult i64 %p31, %length
  br i1 %lt, label %land.rhs, label %if.end30

if.then20:                                        ; preds = %if.then14
  %p22 = load i64, ptr %p, align 8
  %tuple.field23 = getelementptr inbounds nuw %_Z8JsonScan, ptr %tuple, i32 0, i32 0
  store i64 7, ptr %tuple.field23, align 1
  %tuple.field24 = getelementptr inbounds nuw %_Z8JsonScan, ptr %tuple, i32 0, i32 1
  store i64 %p22, ptr %tuple.field24, align 1
  %tuple.field25 = getelementptr inbounds nuw %_Z8JsonScan, ptr %tuple, i32 0, i32 2
  store i1 false, ptr %tuple.field25, align 1
  %tuple.val26 = load %_Z8JsonScan, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z8JsonScan, ptr null, i32 1) to i64), i1 false)
  ret void

if.end21:                                         ; preds = %if.then14
  br label %if.end15

if.then29:                                        ; preds = %land.rhs
  store i1 false, ptr %integral, align 1
  %p35 = load i64, ptr %p, align 8
  %add36 = add i64 %p35, 1
  store i64 %add36, ptr %p, align 1
  %p37 = load i64, ptr %p, align 8
  %call38 = call i1 @_ZN10JsonReaderB4json8digit_atE5SliceI2u8Em(ptr %2, i64 %p37)
  %eq39 = icmp eq i1 %call38, false
  br i1 %eq39, label %if.then40, label %if.end41

if.end30:                                         ; preds = %if.end41, %land.rhs, %if.end15
  %p52 = load i64, ptr %p, align 8
  %load.struct53 = load %_Z5SliceI2u8E, ptr %2, align 8
  %length54 = extractvalue %_Z5SliceI2u8E %load.struct53, 0
  %lt55 = icmp ult i64 %p52, %length54
  br i1 %lt55, label %land.rhs51, label %if.end50

land.rhs:                                         ; preds = %if.end15
  %p32 = load i64, ptr %p, align 8
  %call33 = call i8 @_ZN5SliceI2u8EixEm(ptr %2, i64 %p32)
  %eq34 = icmp eq i8 %call33, 46
  br i1 %eq34, label %if.then29, label %if.end30

if.then40:                                        ; preds = %if.then29
  %p42 = load i64, ptr %p, align 8
  %tuple.field43 = getelementptr inbounds nuw %_Z8JsonScan, ptr %tuple, i32 0, i32 0
  store i64 7, ptr %tuple.field43, align 1
  %tuple.field44 = getelementptr inbounds nuw %_Z8JsonScan, ptr %tuple, i32 0, i32 1
  store i64 %p42, ptr %tuple.field44, align 1
  %tuple.field45 = getelementptr inbounds nuw %_Z8JsonScan, ptr %tuple, i32 0, i32 2
  store i1 false, ptr %tuple.field45, align 1
  %tuple.val46 = load %_Z8JsonScan, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z8JsonScan, ptr null, i32 1) to i64), i1 false)
  ret void

if.end41:                                         ; preds = %if.then29
  %p47 = load i64, ptr %p, align 8
  %call48 = call i64 @_ZN10JsonReaderB4json10digits_endE5SliceI2u8Em(ptr %2, i64 %p47)
  store i64 %call48, ptr %p, align 1
  br label %if.end30

if.then49:                                        ; preds = %lor.end
  store i1 false, ptr %integral, align 1
  %p62 = load i64, ptr %p, align 8
  %add63 = add i64 %p62, 1
  store i64 %add63, ptr %p, align 1
  %p67 = load i64, ptr %p, align 8
  %load.struct68 = load %_Z5SliceI2u8E, ptr %2, align 8
  %length69 = extractvalue %_Z5SliceI2u8E %load.struct68, 0
  %lt70 = icmp ult i64 %p67, %length69
  br i1 %lt70, label %land.rhs66, label %if.end65

if.end50:                                         ; preds = %if.end86, %lor.end, %if.end30
  %p94 = load i64, ptr %p, align 8
  %integral95 = load i1, ptr %integral, align 1
  %tuple.field96 = getelementptr inbounds nuw %_Z8JsonScan, ptr %tuple, i32 0, i32 0
  store i64 0, ptr %tuple.field96, align 1
  %tuple.field97 = getelementptr inbounds nuw %_Z8JsonScan, ptr %tuple, i32 0, i32 1
  store i64 %p94, ptr %tuple.field97, align 1
  %tuple.field98 = getelementptr inbounds nuw %_Z8JsonScan, ptr %tuple, i32 0, i32 2
  store i1 %integral95, ptr %tuple.field98, align 1
  %tuple.val99 = load %_Z8JsonScan, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z8JsonScan, ptr null, i32 1) to i64), i1 false)
  ret void

land.rhs51:                                       ; preds = %if.end30
  %p56 = load i64, ptr %p, align 8
  %call57 = call i8 @_ZN5SliceI2u8EixEm(ptr %2, i64 %p56)
  %eq58 = icmp eq i8 %call57, 101
  br i1 %eq58, label %lor.end, label %lor.rhs

lor.rhs:                                          ; preds = %land.rhs51
  %p59 = load i64, ptr %p, align 8
  %call60 = call i8 @_ZN5SliceI2u8EixEm(ptr %2, i64 %p59)
  %eq61 = icmp eq i8 %call60, 69
  br label %lor.end

lor.end:                                          ; preds = %lor.rhs, %land.rhs51
  %lor.result = phi i1 [ true, %land.rhs51 ], [ %eq61, %lor.rhs ]
  br i1 %lor.result, label %if.then49, label %if.end50

if.then64:                                        ; preds = %lor.end75
  %p80 = load i64, ptr %p, align 8
  %add81 = add i64 %p80, 1
  store i64 %add81, ptr %p, align 1
  br label %if.end65

if.end65:                                         ; preds = %if.then64, %lor.end75, %if.then49
  %p82 = load i64, ptr %p, align 8
  %call83 = call i1 @_ZN10JsonReaderB4json8digit_atE5SliceI2u8Em(ptr %2, i64 %p82)
  %eq84 = icmp eq i1 %call83, false
  br i1 %eq84, label %if.then85, label %if.end86

land.rhs66:                                       ; preds = %if.then49
  %p71 = load i64, ptr %p, align 8
  %call72 = call i8 @_ZN5SliceI2u8EixEm(ptr %2, i64 %p71)
  %eq73 = icmp eq i8 %call72, 43
  br i1 %eq73, label %lor.end75, label %lor.rhs74

lor.rhs74:                                        ; preds = %land.rhs66
  %p76 = load i64, ptr %p, align 8
  %call77 = call i8 @_ZN5SliceI2u8EixEm(ptr %2, i64 %p76)
  %eq78 = icmp eq i8 %call77, 45
  br label %lor.end75

lor.end75:                                        ; preds = %lor.rhs74, %land.rhs66
  %lor.result79 = phi i1 [ true, %land.rhs66 ], [ %eq78, %lor.rhs74 ]
  br i1 %lor.result79, label %if.then64, label %if.end65

if.then85:                                        ; preds = %if.end65
  %p87 = load i64, ptr %p, align 8
  %tuple.field88 = getelementptr inbounds nuw %_Z8JsonScan, ptr %tuple, i32 0, i32 0
  store i64 7, ptr %tuple.field88, align 1
  %tuple.field89 = getelementptr inbounds nuw %_Z8JsonScan, ptr %tuple, i32 0, i32 1
  store i64 %p87, ptr %tuple.field89, align 1
  %tuple.field90 = getelementptr inbounds nuw %_Z8JsonScan, ptr %tuple, i32 0, i32 2
  store i1 false, ptr %tuple.field90, align 1
  %tuple.val91 = load %_Z8JsonScan, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z8JsonScan, ptr null, i32 1) to i64), i1 false)
  ret void

if.end86:                                         ; preds = %if.end65
  %p92 = load i64, ptr %p, align 8
  %call93 = call i64 @_ZN10JsonReaderB4json10digits_endE5SliceI2u8Em(ptr %2, i64 %p92)
  store i64 %call93, ptr %p, align 1
  br label %if.end50
}

define linkonce_odr i1 @_ZN10JsonReaderB4json8digit_atE5SliceI2u8Em(ptr %0, i64 %1) {
entry:
  %load.struct = load %_Z5SliceI2u8E, ptr %0, align 8
  %length = extractvalue %_Z5SliceI2u8E %load.struct, 0
  %lt = icmp ult i64 %1, %length
  br i1 %lt, label %lor.rhs, label %lor.end

lor.rhs:                                          ; preds = %entry
  %call = call i8 @_ZN5SliceI2u8EixEm(ptr %0, i64 %1)
  %ge = icmp uge i8 %call, 48
  br label %lor.end

lor.end:                                          ; preds = %lor.rhs, %entry
  %lor.result = phi i1 [ false, %entry ], [ %ge, %lor.rhs ]
  br i1 %lor.result, label %lor.rhs1, label %lor.end2

lor.rhs1:                                         ; preds = %lor.end
  %call3 = call i8 @_ZN5SliceI2u8EixEm(ptr %0, i64 %1)
  %le = icmp ule i8 %call3, 57
  br label %lor.end2

lor.end2:                                         ; preds = %lor.rhs1, %lor.end
  %lor.result4 = phi i1 [ false, %lor.end ], [ %le, %lor.rhs1 ]
  ret i1 %lor.result4
}

define linkonce_odr i64 @_ZN10JsonReaderB4json10digits_endE5SliceI2u8Em(ptr %0, i64 %1) {
entry:
  %q = alloca i64, align 8
  store i64 %1, ptr %q, align 1
  br label %while.cond

while.cond:                                       ; preds = %if.end, %entry
  %q1 = load i64, ptr %q, align 8
  %add = add i64 %q1, 16
  %load.struct = load %_Z5SliceI2u8E, ptr %0, align 8
  %length = extractvalue %_Z5SliceI2u8E %load.struct, 0
  %le = icmp ule i64 %add, %length
  br i1 %le, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %q2 = load i64, ptr %q, align 8
  %simd.cont = getelementptr inbounds nuw %_Z5SliceI2u8E, ptr %0, i32 0, i32 0
  %simd.len = load i64, ptr %simd.cont, align 8
  %simd.cont3 = getelementptr inbounds nuw %_Z5SliceI2u8E, ptr %0, i32 0, i32 1
  %simd.data = load ptr, ptr %simd.cont3, align 8
  %simd.fits = icmp uge i64 %simd.len, 16
  %simd.room = sub i64 %simd.len, 16
  %simd.within = icmp ule i64 %q2, %simd.room
  %simd.inrange = and i1 %simd.fits, %simd.within
  br i1 %simd.inrange, label %simd.mem.ok, label %simd.mem.oob

while.exit:                                       ; preds = %while.cond
  br label %while.cond8

simd.mem.oob:                                     ; preds = %while.body
  call void @_Z15scaly_panic_oobP10const_charmm(ptr @.simd.mem.22, i64 %q2, i64 %simd.len)
  unreachable

simd.mem.ok:                                      ; preds = %while.body
  %simd.addr = getelementptr inbounds i8, ptr %simd.data, i64 %q2
  %simd.load = load <16 x i8>, ptr %simd.addr, align 1
  %simd.sub = sub <16 x i8> %simd.load, splat (i8 48)
  %simd.cmp = icmp ugt <16 x i8> %simd.sub, splat (i8 9)
  %simd.bits = bitcast <16 x i1> %simd.cmp to i16
  %simd.bits64 = zext i16 %simd.bits to i64
  %ne = icmp ne i64 %simd.bits64, 0
  br i1 %ne, label %if.then, label %if.end

if.then:                                          ; preds = %simd.mem.ok
  %q4 = load i64, ptr %q, align 8
  %call = call i64 @_Z14trailing_zeros3u64(i64 %simd.bits64)
  %add5 = add i64 %q4, %call
  ret i64 %add5

if.end:                                           ; preds = %simd.mem.ok
  %q6 = load i64, ptr %q, align 8
  %add7 = add i64 %q6, 16
  store i64 %add7, ptr %q, align 1
  br label %while.cond

while.cond8:                                      ; preds = %while.body9, %while.exit
  %q11 = load i64, ptr %q, align 8
  %load.struct12 = load %_Z5SliceI2u8E, ptr %0, align 8
  %length13 = extractvalue %_Z5SliceI2u8E %load.struct12, 0
  %lt = icmp ult i64 %q11, %length13
  br i1 %lt, label %lor.rhs, label %lor.end

while.body9:                                      ; preds = %lor.end17
  %q22 = load i64, ptr %q, align 8
  %add23 = add i64 %q22, 1
  store i64 %add23, ptr %q, align 1
  br label %while.cond8

while.exit10:                                     ; preds = %lor.end17
  %q24 = load i64, ptr %q, align 8
  ret i64 %q24

lor.rhs:                                          ; preds = %while.cond8
  %q14 = load i64, ptr %q, align 8
  %call15 = call i8 @_ZN5SliceI2u8EixEm(ptr %0, i64 %q14)
  %ge = icmp uge i8 %call15, 48
  br label %lor.end

lor.end:                                          ; preds = %lor.rhs, %while.cond8
  %lor.result = phi i1 [ false, %while.cond8 ], [ %ge, %lor.rhs ]
  br i1 %lor.result, label %lor.rhs16, label %lor.end17

lor.rhs16:                                        ; preds = %lor.end
  %q18 = load i64, ptr %q, align 8
  %call19 = call i8 @_ZN5SliceI2u8EixEm(ptr %0, i64 %q18)
  %le20 = icmp ule i8 %call19, 57
  br label %lor.end17

lor.end17:                                        ; preds = %lor.rhs16, %lor.end
  %lor.result21 = phi i1 [ false, %lor.end ], [ %le20, %lor.rhs16 ]
  br i1 %lor.result21, label %while.body9, label %while.exit10
}

define linkonce_odr i64 @_ZN10JsonReaderB4json8put_utf8E5SliceI2u8Em3i64(ptr %0, i64 %1, i64 %2) {
entry:
  %lt = icmp slt i64 %2, 128
  br i1 %lt, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %as.trunc = trunc i64 %2 to i8
  call void @_ZN5SliceI2u8E3putEm2u8(ptr %0, i64 %1, i8 %as.trunc)
  %add = add i64 %1, 1
  ret i64 %add

if.end:                                           ; preds = %entry
  %lt1 = icmp slt i64 %2, 2048
  br i1 %lt1, label %if.then2, label %if.end3

if.then2:                                         ; preds = %if.end
  %ashr = ashr i64 %2, 6
  %or = or i64 192, %ashr
  %as.trunc4 = trunc i64 %or to i8
  call void @_ZN5SliceI2u8E3putEm2u8(ptr %0, i64 %1, i8 %as.trunc4)
  %add5 = add i64 %1, 1
  %and = and i64 %2, 63
  %or6 = or i64 128, %and
  %as.trunc7 = trunc i64 %or6 to i8
  call void @_ZN5SliceI2u8E3putEm2u8(ptr %0, i64 %add5, i8 %as.trunc7)
  %add8 = add i64 %1, 2
  ret i64 %add8

if.end3:                                          ; preds = %if.end
  %lt9 = icmp slt i64 %2, 65536
  br i1 %lt9, label %if.then10, label %if.end11

if.then10:                                        ; preds = %if.end3
  %ashr12 = ashr i64 %2, 12
  %or13 = or i64 224, %ashr12
  %as.trunc14 = trunc i64 %or13 to i8
  call void @_ZN5SliceI2u8E3putEm2u8(ptr %0, i64 %1, i8 %as.trunc14)
  %add15 = add i64 %1, 1
  %ashr16 = ashr i64 %2, 6
  %and17 = and i64 %ashr16, 63
  %or18 = or i64 128, %and17
  %as.trunc19 = trunc i64 %or18 to i8
  call void @_ZN5SliceI2u8E3putEm2u8(ptr %0, i64 %add15, i8 %as.trunc19)
  %add20 = add i64 %1, 2
  %and21 = and i64 %2, 63
  %or22 = or i64 128, %and21
  %as.trunc23 = trunc i64 %or22 to i8
  call void @_ZN5SliceI2u8E3putEm2u8(ptr %0, i64 %add20, i8 %as.trunc23)
  %add24 = add i64 %1, 3
  ret i64 %add24

if.end11:                                         ; preds = %if.end3
  %ashr25 = ashr i64 %2, 18
  %or26 = or i64 240, %ashr25
  %as.trunc27 = trunc i64 %or26 to i8
  call void @_ZN5SliceI2u8E3putEm2u8(ptr %0, i64 %1, i8 %as.trunc27)
  %add28 = add i64 %1, 1
  %ashr29 = ashr i64 %2, 12
  %and30 = and i64 %ashr29, 63
  %or31 = or i64 128, %and30
  %as.trunc32 = trunc i64 %or31 to i8
  call void @_ZN5SliceI2u8E3putEm2u8(ptr %0, i64 %add28, i8 %as.trunc32)
  %add33 = add i64 %1, 2
  %ashr34 = ashr i64 %2, 6
  %and35 = and i64 %ashr34, 63
  %or36 = or i64 128, %and35
  %as.trunc37 = trunc i64 %or36 to i8
  call void @_ZN5SliceI2u8E3putEm2u8(ptr %0, i64 %add33, i8 %as.trunc37)
  %add38 = add i64 %1, 3
  %and39 = and i64 %2, 63
  %or40 = or i64 128, %and39
  %as.trunc41 = trunc i64 %or40 to i8
  call void @_ZN5SliceI2u8E3putEm2u8(ptr %0, i64 %add38, i8 %as.trunc41)
  %add42 = add i64 %1, 4
  ret i64 %add42
}

define linkonce_odr i64 @_ZN10JsonReaderB4json15decode_in_placeE5SliceI2u8Emm(ptr %0, i64 %1, i64 %2) {
entry:
  %cp = alloca i64, align 8
  %r = alloca i64, align 8
  store i64 %1, ptr %r, align 1
  %w = alloca i64, align 8
  store i64 %1, ptr %w, align 1
  br label %while.cond

while.cond:                                       ; preds = %if.end25, %if.then11, %if.then, %entry
  %r1 = load i64, ptr %r, align 8
  %lt = icmp ult i64 %r1, %2
  br i1 %lt, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %r2 = load i64, ptr %r, align 8
  %call = call i8 @_ZN5SliceI2u8EixEm(ptr %0, i64 %r2)
  %ne = icmp ne i8 %call, 92
  br i1 %ne, label %if.then, label %if.end

while.exit:                                       ; preds = %while.cond
  %w67 = load i64, ptr %w, align 8
  %sub68 = sub i64 %w67, %1
  ret i64 %sub68

if.then:                                          ; preds = %while.body
  %w3 = load i64, ptr %w, align 8
  call void @_ZN5SliceI2u8E3putEm2u8(ptr %0, i64 %w3, i8 %call)
  %w4 = load i64, ptr %w, align 8
  %add = add i64 %w4, 1
  store i64 %add, ptr %w, align 1
  %r5 = load i64, ptr %r, align 8
  %add6 = add i64 %r5, 1
  store i64 %add6, ptr %r, align 1
  br label %while.cond

if.end:                                           ; preds = %while.body
  %r7 = load i64, ptr %r, align 8
  %add8 = add i64 %r7, 1
  %call9 = call i8 @_ZN5SliceI2u8EixEm(ptr %0, i64 %add8)
  %ne10 = icmp ne i8 %call9, 117
  br i1 %ne10, label %if.then11, label %if.end12

if.then11:                                        ; preds = %if.end
  %w13 = load i64, ptr %w, align 8
  %call14 = call i8 @_ZN10JsonReaderB4json8unescapeE2u8(i8 %call9)
  call void @_ZN5SliceI2u8E3putEm2u8(ptr %0, i64 %w13, i8 %call14)
  %w15 = load i64, ptr %w, align 8
  %add16 = add i64 %w15, 1
  store i64 %add16, ptr %w, align 1
  %r17 = load i64, ptr %r, align 8
  %add18 = add i64 %r17, 2
  store i64 %add18, ptr %r, align 1
  br label %while.cond

if.end12:                                         ; preds = %if.end
  %r19 = load i64, ptr %r, align 8
  %add20 = add i64 %r19, 2
  %call21 = call i64 @_ZN10JsonReaderB4json4hex4E5SliceI2u8Em(ptr %0, i64 %add20)
  store i64 %call21, ptr %cp, align 1
  %r22 = load i64, ptr %r, align 8
  %add23 = add i64 %r22, 6
  store i64 %add23, ptr %r, align 1
  %cp26 = load i64, ptr %cp, align 8
  %ge = icmp sge i64 %cp26, 55296
  br i1 %ge, label %land.rhs, label %if.else

if.then24:                                        ; preds = %land.rhs
  %r33 = load i64, ptr %r, align 8
  %add34 = add i64 %r33, 6
  %le35 = icmp ule i64 %add34, %2
  br i1 %le35, label %land.rhs32, label %if.else29

if.else:                                          ; preds = %land.rhs, %if.end12
  %cp60 = load i64, ptr %cp, align 8
  %ge61 = icmp sge i64 %cp60, 56320
  br i1 %ge61, label %land.rhs59, label %if.end58

if.end25:                                         ; preds = %if.end58, %if.end30
  %w64 = load i64, ptr %w, align 8
  %cp65 = load i64, ptr %cp, align 8
  %call66 = call i64 @_ZN10JsonReaderB4json8put_utf8E5SliceI2u8Em3i64(ptr %0, i64 %w64, i64 %cp65)
  store i64 %call66, ptr %w, align 1
  br label %while.cond

land.rhs:                                         ; preds = %if.end12
  %cp27 = load i64, ptr %cp, align 8
  %le = icmp sle i64 %cp27, 56319
  br i1 %le, label %if.then24, label %if.else

if.then28:                                        ; preds = %land.rhs31
  %r42 = load i64, ptr %r, align 8
  %add43 = add i64 %r42, 2
  %call44 = call i64 @_ZN10JsonReaderB4json4hex4E5SliceI2u8Em(ptr %0, i64 %add43)
  %ge49 = icmp sge i64 %call44, 56320
  br i1 %ge49, label %land.rhs48, label %if.else46

if.else29:                                        ; preds = %land.rhs31, %land.rhs32, %if.then24
  store i64 65533, ptr %cp, align 1
  br label %if.end30

if.end30:                                         ; preds = %if.else29, %if.end47
  br label %if.end25

land.rhs31:                                       ; preds = %land.rhs32
  %r38 = load i64, ptr %r, align 8
  %add39 = add i64 %r38, 1
  %call40 = call i8 @_ZN5SliceI2u8EixEm(ptr %0, i64 %add39)
  %eq41 = icmp eq i8 %call40, 117
  br i1 %eq41, label %if.then28, label %if.else29

land.rhs32:                                       ; preds = %if.then24
  %r36 = load i64, ptr %r, align 8
  %call37 = call i8 @_ZN5SliceI2u8EixEm(ptr %0, i64 %r36)
  %eq = icmp eq i8 %call37, 92
  br i1 %eq, label %land.rhs31, label %if.else29

if.then45:                                        ; preds = %land.rhs48
  %cp51 = load i64, ptr %cp, align 8
  %sub = sub i64 %cp51, 55296
  %shl = shl i64 %sub, 10
  %add52 = add i64 65536, %shl
  %sub53 = sub i64 %call44, 56320
  %add54 = add i64 %add52, %sub53
  store i64 %add54, ptr %cp, align 1
  %r55 = load i64, ptr %r, align 8
  %add56 = add i64 %r55, 6
  store i64 %add56, ptr %r, align 1
  br label %if.end47

if.else46:                                        ; preds = %land.rhs48, %if.then28
  store i64 65533, ptr %cp, align 1
  br label %if.end47

if.end47:                                         ; preds = %if.else46, %if.then45
  br label %if.end30

land.rhs48:                                       ; preds = %if.then28
  %le50 = icmp sle i64 %call44, 57343
  br i1 %le50, label %if.then45, label %if.else46

if.then57:                                        ; preds = %land.rhs59
  store i64 65533, ptr %cp, align 1
  br label %if.end58

if.end58:                                         ; preds = %if.then57, %land.rhs59, %if.else
  br label %if.end25

land.rhs59:                                       ; preds = %if.else
  %cp62 = load i64, ptr %cp, align 8
  %le63 = icmp sle i64 %cp62, 57343
  br i1 %le63, label %if.then57, label %if.end58
}

declare double @strtod(ptr, ptr)

define linkonce_odr ptr @_ZN5SliceI10JsonMemberB4jsonE3getEm(ptr %0, i64 %1) {
entry:
  %load.struct = load %_Z5SliceI10JsonMemberE, ptr %0, align 8
  %length = extractvalue %_Z5SliceI10JsonMemberE %load.struct, 0
  %ge = icmp uge i64 %1, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %load.struct1 = load %_Z5SliceI10JsonMemberE, ptr %0, align 8
  %data = extractvalue %_Z5SliceI10JsonMemberE %load.struct1, 1
  %ptr.add = getelementptr inbounds %_Z10JsonMember, ptr %data, i64 %1
  ret ptr %ptr.add
}

define linkonce_odr ptr @_ZN5SliceI10JsonMemberB4jsonE2atEm(ptr %0, i64 %1) {
entry:
  %load.struct = load %_Z5SliceI10JsonMemberE, ptr %0, align 8
  %length = extractvalue %_Z5SliceI10JsonMemberE %load.struct, 0
  %ge = icmp uge i64 %1, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %field.inplace = getelementptr inbounds nuw %_Z5SliceI10JsonMemberE, ptr %0, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  call void @_Z15scaly_panic_oobP10const_charmm(ptr @.str.23, i64 %1, i64 %field.val)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %load.struct1 = load %_Z5SliceI10JsonMemberE, ptr %0, align 8
  %data = extractvalue %_Z5SliceI10JsonMemberE %load.struct1, 1
  %ptr.add = getelementptr inbounds %_Z10JsonMember, ptr %data, i64 %1
  ret ptr %ptr.add
}

define linkonce_odr void @_ZN5SliceI10JsonMemberB4jsonE3putEm10JsonMemberB4json(ptr %0, i64 %1, ptr %2) {
entry:
  %load.struct = load %_Z5SliceI10JsonMemberE, ptr %0, align 8
  %length = extractvalue %_Z5SliceI10JsonMemberE %load.struct, 0
  %ge = icmp uge i64 %1, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %field.inplace = getelementptr inbounds nuw %_Z5SliceI10JsonMemberE, ptr %0, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  call void @_Z15scaly_panic_oobP10const_charmm(ptr @.str.24, i64 %1, i64 %field.val)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %load.struct1 = load %_Z5SliceI10JsonMemberE, ptr %0, align 8
  %data = extractvalue %_Z5SliceI10JsonMemberE %load.struct1, 1
  %ptr.add = getelementptr inbounds %_Z10JsonMember, ptr %data, i64 %1
  %store.load = load %_Z10JsonMember, ptr %2, align 8
  store %_Z10JsonMember %store.load, ptr %ptr.add, align 8
  ret void
}

define linkonce_odr i1 @_ZN5SliceI10JsonMemberB4jsonE8is_emptyEv(ptr %0) {
entry:
  %load.struct = load %_Z5SliceI10JsonMemberE, ptr %0, align 8
  %length = extractvalue %_Z5SliceI10JsonMemberE %load.struct, 0
  %eq = icmp eq i64 %length, 0
  ret i1 %eq
}

define linkonce_odr void @_ZN5SliceI10JsonMemberB4jsonE8subsliceEmm(ptr noalias sret(%_Z5SliceI10JsonMemberE) %0, ptr %1, i64 %2, i64 %3) {
entry:
  %tuple = alloca %_Z5SliceI10JsonMemberE, align 8
  %from = alloca i64, align 8
  store i64 %2, ptr %from, align 1
  %to = alloca i64, align 8
  store i64 %3, ptr %to, align 1
  %from1 = load i64, ptr %from, align 8
  %load.struct = load %_Z5SliceI10JsonMemberE, ptr %1, align 8
  %length = extractvalue %_Z5SliceI10JsonMemberE %load.struct, 0
  %gt = icmp ugt i64 %from1, %length
  br i1 %gt, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %load.struct2 = load %_Z5SliceI10JsonMemberE, ptr %1, align 8
  %length3 = extractvalue %_Z5SliceI10JsonMemberE %load.struct2, 0
  store i64 %length3, ptr %from, align 1
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %to4 = load i64, ptr %to, align 8
  %load.struct5 = load %_Z5SliceI10JsonMemberE, ptr %1, align 8
  %length6 = extractvalue %_Z5SliceI10JsonMemberE %load.struct5, 0
  %gt7 = icmp ugt i64 %to4, %length6
  br i1 %gt7, label %if.then8, label %if.end9

if.then8:                                         ; preds = %if.end
  %load.struct10 = load %_Z5SliceI10JsonMemberE, ptr %1, align 8
  %length11 = extractvalue %_Z5SliceI10JsonMemberE %load.struct10, 0
  store i64 %length11, ptr %to, align 1
  br label %if.end9

if.end9:                                          ; preds = %if.then8, %if.end
  %from12 = load i64, ptr %from, align 8
  %to13 = load i64, ptr %to, align 8
  %gt14 = icmp ugt i64 %from12, %to13
  br i1 %gt14, label %if.then15, label %if.end16

if.then15:                                        ; preds = %if.end9
  %to17 = load i64, ptr %to, align 8
  store i64 %to17, ptr %from, align 1
  br label %if.end16

if.end16:                                         ; preds = %if.then15, %if.end9
  %to18 = load i64, ptr %to, align 8
  %from19 = load i64, ptr %from, align 8
  %sub = sub i64 %to18, %from19
  %load.struct20 = load %_Z5SliceI10JsonMemberE, ptr %1, align 8
  %data = extractvalue %_Z5SliceI10JsonMemberE %load.struct20, 1
  %from21 = load i64, ptr %from, align 8
  %ptr.add = getelementptr inbounds %_Z10JsonMember, ptr %data, i64 %from21
  %tuple.field = getelementptr inbounds nuw %_Z5SliceI10JsonMemberE, ptr %tuple, i32 0, i32 0
  store i64 %sub, ptr %tuple.field, align 1
  %tuple.field22 = getelementptr inbounds nuw %_Z5SliceI10JsonMemberE, ptr %tuple, i32 0, i32 1
  store ptr %ptr.add, ptr %tuple.field22, align 1
  %tuple.val = load %_Z5SliceI10JsonMemberE, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z5SliceI10JsonMemberE, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN5SliceI10JsonMemberB4jsonE10slice_fromEPN4scaly6memory4PageEm(ptr noalias sret(%_Z5SliceI10JsonMemberE) %0, ptr %1, ptr %2, i64 %3) {
entry:
  %sret.result = alloca %_Z5SliceI10JsonMemberE, align 8
  %field.inplace = getelementptr inbounds nuw %_Z5SliceI10JsonMemberE, ptr %2, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  call void @_ZN5SliceI10JsonMemberB4jsonE8subsliceEmm(ptr noalias sret(%_Z5SliceI10JsonMemberE) %sret.result, ptr %2, i64 %3, i64 %field.val)
  %sret.body = load %_Z5SliceI10JsonMemberE, ptr %sret.result, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %sret.result, i64 ptrtoint (ptr getelementptr (%_Z5SliceI10JsonMemberE, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN5SliceI10JsonMemberB4jsonE8slice_toEPN4scaly6memory4PageEm(ptr noalias sret(%_Z5SliceI10JsonMemberE) %0, ptr %1, ptr %2, i64 %3) {
entry:
  %sret.result = alloca %_Z5SliceI10JsonMemberE, align 8
  call void @_ZN5SliceI10JsonMemberB4jsonE8subsliceEmm(ptr noalias sret(%_Z5SliceI10JsonMemberE) %sret.result, ptr %2, i64 0, i64 %3)
  %sret.body = load %_Z5SliceI10JsonMemberE, ptr %sret.result, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %sret.result, i64 ptrtoint (ptr getelementptr (%_Z5SliceI10JsonMemberE, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr i1 @_ZN5SliceI10JsonMemberB4jsonE6equalsE5SliceI10JsonMemberB4jsonE(ptr %0, ptr %1) {
entry:
  %load.struct = load %_Z5SliceI10JsonMemberE, ptr %0, align 8
  %length = extractvalue %_Z5SliceI10JsonMemberE %load.struct, 0
  %load.struct1 = load %_Z5SliceI10JsonMemberE, ptr %1, align 8
  %length2 = extractvalue %_Z5SliceI10JsonMemberE %load.struct1, 0
  %ne = icmp ne i64 %length, %length2
  br i1 %ne, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret i1 false

if.end:                                           ; preds = %entry
  %load.struct3 = load %_Z5SliceI10JsonMemberE, ptr %0, align 8
  %length4 = extractvalue %_Z5SliceI10JsonMemberE %load.struct3, 0
  %eq = icmp eq i64 %length4, 0
  br i1 %eq, label %if.then5, label %if.end6

if.then5:                                         ; preds = %if.end
  ret i1 true

if.end6:                                          ; preds = %if.end
  %load.struct7 = load %_Z5SliceI10JsonMemberE, ptr %0, align 8
  %data = extractvalue %_Z5SliceI10JsonMemberE %load.struct7, 1
  %load.struct8 = load %_Z5SliceI10JsonMemberE, ptr %1, align 8
  %data9 = extractvalue %_Z5SliceI10JsonMemberE %load.struct8, 1
  %load.struct10 = load %_Z5SliceI10JsonMemberE, ptr %0, align 8
  %length11 = extractvalue %_Z5SliceI10JsonMemberE %load.struct10, 0
  %mul = mul i64 %length11, ptrtoint (ptr getelementptr (%_Z10JsonMember, ptr null, i32 1) to i64)
  %call = call i32 @memcmp(ptr %data, ptr %data9, i64 %mul)
  %eq12 = icmp eq i32 %call, 0
  ret i1 %eq12
}

define linkonce_odr i1 @_ZN5SliceI10JsonMemberB4jsonE11starts_withE5SliceI10JsonMemberB4jsonE(ptr %0, ptr %1) {
entry:
  %sret.result = alloca %_Z5SliceI10JsonMemberE, align 8
  %load.struct = load %_Z5SliceI10JsonMemberE, ptr %1, align 8
  %length = extractvalue %_Z5SliceI10JsonMemberE %load.struct, 0
  %load.struct1 = load %_Z5SliceI10JsonMemberE, ptr %0, align 8
  %length2 = extractvalue %_Z5SliceI10JsonMemberE %load.struct1, 0
  %gt = icmp ugt i64 %length, %length2
  br i1 %gt, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret i1 false

if.end:                                           ; preds = %entry
  %field.inplace = getelementptr inbounds nuw %_Z5SliceI10JsonMemberE, ptr %1, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  call void @_ZN5SliceI10JsonMemberB4jsonE8subsliceEmm(ptr noalias sret(%_Z5SliceI10JsonMemberE) %sret.result, ptr %0, i64 0, i64 %field.val)
  %call = call i1 @_ZN5SliceI10JsonMemberB4jsonE6equalsE5SliceI10JsonMemberB4jsonE(ptr %sret.result, ptr %1)
  ret i1 %call
}

define linkonce_odr i1 @_ZN5SliceI10JsonMemberB4jsonE9ends_withE5SliceI10JsonMemberB4jsonE(ptr %0, ptr %1) {
entry:
  %sret.result = alloca %_Z5SliceI10JsonMemberE, align 8
  %load.struct = load %_Z5SliceI10JsonMemberE, ptr %1, align 8
  %length = extractvalue %_Z5SliceI10JsonMemberE %load.struct, 0
  %load.struct1 = load %_Z5SliceI10JsonMemberE, ptr %0, align 8
  %length2 = extractvalue %_Z5SliceI10JsonMemberE %load.struct1, 0
  %gt = icmp ugt i64 %length, %length2
  br i1 %gt, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret i1 false

if.end:                                           ; preds = %entry
  %load.struct3 = load %_Z5SliceI10JsonMemberE, ptr %0, align 8
  %length4 = extractvalue %_Z5SliceI10JsonMemberE %load.struct3, 0
  %load.struct5 = load %_Z5SliceI10JsonMemberE, ptr %1, align 8
  %length6 = extractvalue %_Z5SliceI10JsonMemberE %load.struct5, 0
  %sub = sub i64 %length4, %length6
  %field.inplace = getelementptr inbounds nuw %_Z5SliceI10JsonMemberE, ptr %0, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  call void @_ZN5SliceI10JsonMemberB4jsonE8subsliceEmm(ptr noalias sret(%_Z5SliceI10JsonMemberE) %sret.result, ptr %0, i64 %sub, i64 %field.val)
  %call = call i1 @_ZN5SliceI10JsonMemberB4jsonE6equalsE5SliceI10JsonMemberB4jsonE(ptr %sret.result, ptr %1)
  ret i1 %call
}

define linkonce_odr ptr @_ZN13SliceIteratorI10JsonMemberB4jsonE4nextEv(ptr %0) {
entry:
  %load.struct = load %_Z13SliceIteratorI10JsonMemberE, ptr %0, align 8
  %position = extractvalue %_Z13SliceIteratorI10JsonMemberE %load.struct, 1
  %load.struct1 = load %_Z13SliceIteratorI10JsonMemberE, ptr %0, align 8
  %slice = extractvalue %_Z13SliceIteratorI10JsonMemberE %load.struct1, 0
  %length = extractvalue %_Z5SliceI10JsonMemberE %slice, 0
  %ge = icmp uge i64 %position, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %load.struct2 = load %_Z13SliceIteratorI10JsonMemberE, ptr %0, align 8
  %slice3 = extractvalue %_Z13SliceIteratorI10JsonMemberE %load.struct2, 0
  %data = extractvalue %_Z5SliceI10JsonMemberE %slice3, 1
  %load.struct4 = load %_Z13SliceIteratorI10JsonMemberE, ptr %0, align 8
  %position5 = extractvalue %_Z13SliceIteratorI10JsonMemberE %load.struct4, 1
  %ptr.add = getelementptr inbounds %_Z10JsonMember, ptr %data, i64 %position5
  %load.struct6 = load %_Z13SliceIteratorI10JsonMemberE, ptr %0, align 8
  %position7 = extractvalue %_Z13SliceIteratorI10JsonMemberE %load.struct6, 1
  %add = add i64 %position7, 1
  %position8 = getelementptr inbounds nuw %_Z13SliceIteratorI10JsonMemberE, ptr %0, i32 0, i32 1
  store i64 %add, ptr %position8, align 8
  ret ptr %ptr.add
}

define linkonce_odr void @_ZN13SliceIteratorI10JsonMemberB4jsonEC1E5SliceI10JsonMemberB4jsonE(ptr %0, ptr %1) {
entry:
  %slice = getelementptr inbounds nuw %_Z13SliceIteratorI10JsonMemberE, ptr %0, i32 0, i32 0
  %field.load = load %_Z5SliceI10JsonMemberE, ptr %1, align 8
  store %_Z5SliceI10JsonMemberE %field.load, ptr %slice, align 8
  %position = getelementptr inbounds nuw %_Z13SliceIteratorI10JsonMemberE, ptr %0, i32 0, i32 1
  store i64 0, ptr %position, align 8
  ret void
}

define linkonce_odr void @_ZN5SliceI10JsonMemberB4jsonE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z13SliceIteratorI10JsonMemberE) %0, ptr %1, ptr %2) {
entry:
  %struct.init = alloca %_Z13SliceIteratorI10JsonMemberE, align 8
  call void @_ZN13SliceIteratorI10JsonMemberB4jsonEC1E5SliceI10JsonMemberB4jsonE(ptr %struct.init, ptr %2)
  %sret.body = load %_Z13SliceIteratorI10JsonMemberE, ptr %struct.init, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %struct.init, i64 ptrtoint (ptr getelementptr (%_Z13SliceIteratorI10JsonMemberE, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN5SliceI10JsonMemberB4jsonEC1Ev(ptr %0) {
entry:
  %data = getelementptr inbounds nuw %_Z5SliceI10JsonMemberE, ptr %0, i32 0, i32 1
  store ptr null, ptr %data, align 8
  %length = getelementptr inbounds nuw %_Z5SliceI10JsonMemberE, ptr %0, i32 0, i32 0
  store i64 0, ptr %length, align 8
  ret void
}

define linkonce_odr ptr @_ZN5SliceI9JsonValueB4jsonE3getEm(ptr %0, i64 %1) {
entry:
  %load.struct = load %_Z5SliceI9JsonValueE, ptr %0, align 8
  %length = extractvalue %_Z5SliceI9JsonValueE %load.struct, 0
  %ge = icmp uge i64 %1, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %load.struct1 = load %_Z5SliceI9JsonValueE, ptr %0, align 8
  %data = extractvalue %_Z5SliceI9JsonValueE %load.struct1, 1
  %ptr.add = getelementptr inbounds %_Z9JsonValue, ptr %data, i64 %1
  ret ptr %ptr.add
}

define linkonce_odr ptr @_ZN5SliceI9JsonValueB4jsonE2atEm(ptr %0, i64 %1) {
entry:
  %load.struct = load %_Z5SliceI9JsonValueE, ptr %0, align 8
  %length = extractvalue %_Z5SliceI9JsonValueE %load.struct, 0
  %ge = icmp uge i64 %1, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %field.inplace = getelementptr inbounds nuw %_Z5SliceI9JsonValueE, ptr %0, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  call void @_Z15scaly_panic_oobP10const_charmm(ptr @.str.25, i64 %1, i64 %field.val)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %load.struct1 = load %_Z5SliceI9JsonValueE, ptr %0, align 8
  %data = extractvalue %_Z5SliceI9JsonValueE %load.struct1, 1
  %ptr.add = getelementptr inbounds %_Z9JsonValue, ptr %data, i64 %1
  ret ptr %ptr.add
}

define linkonce_odr void @_ZN5SliceI9JsonValueB4jsonE3putEm9JsonValueB4json(ptr %0, i64 %1, ptr %2) {
entry:
  %load.struct = load %_Z5SliceI9JsonValueE, ptr %0, align 8
  %length = extractvalue %_Z5SliceI9JsonValueE %load.struct, 0
  %ge = icmp uge i64 %1, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %field.inplace = getelementptr inbounds nuw %_Z5SliceI9JsonValueE, ptr %0, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  call void @_Z15scaly_panic_oobP10const_charmm(ptr @.str.26, i64 %1, i64 %field.val)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %load.struct1 = load %_Z5SliceI9JsonValueE, ptr %0, align 8
  %data = extractvalue %_Z5SliceI9JsonValueE %load.struct1, 1
  %ptr.add = getelementptr inbounds %_Z9JsonValue, ptr %data, i64 %1
  %store.load = load %_Z9JsonValue, ptr %2, align 1
  store %_Z9JsonValue %store.load, ptr %ptr.add, align 1
  ret void
}

define linkonce_odr i1 @_ZN5SliceI9JsonValueB4jsonE8is_emptyEv(ptr %0) {
entry:
  %load.struct = load %_Z5SliceI9JsonValueE, ptr %0, align 8
  %length = extractvalue %_Z5SliceI9JsonValueE %load.struct, 0
  %eq = icmp eq i64 %length, 0
  ret i1 %eq
}

define linkonce_odr void @_ZN5SliceI9JsonValueB4jsonE8subsliceEmm(ptr noalias sret(%_Z5SliceI9JsonValueE) %0, ptr %1, i64 %2, i64 %3) {
entry:
  %tuple = alloca %_Z5SliceI9JsonValueE, align 8
  %from = alloca i64, align 8
  store i64 %2, ptr %from, align 1
  %to = alloca i64, align 8
  store i64 %3, ptr %to, align 1
  %from1 = load i64, ptr %from, align 8
  %load.struct = load %_Z5SliceI9JsonValueE, ptr %1, align 8
  %length = extractvalue %_Z5SliceI9JsonValueE %load.struct, 0
  %gt = icmp ugt i64 %from1, %length
  br i1 %gt, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %load.struct2 = load %_Z5SliceI9JsonValueE, ptr %1, align 8
  %length3 = extractvalue %_Z5SliceI9JsonValueE %load.struct2, 0
  store i64 %length3, ptr %from, align 1
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %to4 = load i64, ptr %to, align 8
  %load.struct5 = load %_Z5SliceI9JsonValueE, ptr %1, align 8
  %length6 = extractvalue %_Z5SliceI9JsonValueE %load.struct5, 0
  %gt7 = icmp ugt i64 %to4, %length6
  br i1 %gt7, label %if.then8, label %if.end9

if.then8:                                         ; preds = %if.end
  %load.struct10 = load %_Z5SliceI9JsonValueE, ptr %1, align 8
  %length11 = extractvalue %_Z5SliceI9JsonValueE %load.struct10, 0
  store i64 %length11, ptr %to, align 1
  br label %if.end9

if.end9:                                          ; preds = %if.then8, %if.end
  %from12 = load i64, ptr %from, align 8
  %to13 = load i64, ptr %to, align 8
  %gt14 = icmp ugt i64 %from12, %to13
  br i1 %gt14, label %if.then15, label %if.end16

if.then15:                                        ; preds = %if.end9
  %to17 = load i64, ptr %to, align 8
  store i64 %to17, ptr %from, align 1
  br label %if.end16

if.end16:                                         ; preds = %if.then15, %if.end9
  %to18 = load i64, ptr %to, align 8
  %from19 = load i64, ptr %from, align 8
  %sub = sub i64 %to18, %from19
  %load.struct20 = load %_Z5SliceI9JsonValueE, ptr %1, align 8
  %data = extractvalue %_Z5SliceI9JsonValueE %load.struct20, 1
  %from21 = load i64, ptr %from, align 8
  %ptr.add = getelementptr inbounds %_Z9JsonValue, ptr %data, i64 %from21
  %tuple.field = getelementptr inbounds nuw %_Z5SliceI9JsonValueE, ptr %tuple, i32 0, i32 0
  store i64 %sub, ptr %tuple.field, align 1
  %tuple.field22 = getelementptr inbounds nuw %_Z5SliceI9JsonValueE, ptr %tuple, i32 0, i32 1
  store ptr %ptr.add, ptr %tuple.field22, align 1
  %tuple.val = load %_Z5SliceI9JsonValueE, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z5SliceI9JsonValueE, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN5SliceI9JsonValueB4jsonE10slice_fromEPN4scaly6memory4PageEm(ptr noalias sret(%_Z5SliceI9JsonValueE) %0, ptr %1, ptr %2, i64 %3) {
entry:
  %sret.result = alloca %_Z5SliceI9JsonValueE, align 8
  %field.inplace = getelementptr inbounds nuw %_Z5SliceI9JsonValueE, ptr %2, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  call void @_ZN5SliceI9JsonValueB4jsonE8subsliceEmm(ptr noalias sret(%_Z5SliceI9JsonValueE) %sret.result, ptr %2, i64 %3, i64 %field.val)
  %sret.body = load %_Z5SliceI9JsonValueE, ptr %sret.result, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %sret.result, i64 ptrtoint (ptr getelementptr (%_Z5SliceI9JsonValueE, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN5SliceI9JsonValueB4jsonE8slice_toEPN4scaly6memory4PageEm(ptr noalias sret(%_Z5SliceI9JsonValueE) %0, ptr %1, ptr %2, i64 %3) {
entry:
  %sret.result = alloca %_Z5SliceI9JsonValueE, align 8
  call void @_ZN5SliceI9JsonValueB4jsonE8subsliceEmm(ptr noalias sret(%_Z5SliceI9JsonValueE) %sret.result, ptr %2, i64 0, i64 %3)
  %sret.body = load %_Z5SliceI9JsonValueE, ptr %sret.result, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %sret.result, i64 ptrtoint (ptr getelementptr (%_Z5SliceI9JsonValueE, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr i1 @_ZN5SliceI9JsonValueB4jsonE6equalsE5SliceI9JsonValueB4jsonE(ptr %0, ptr %1) {
entry:
  %load.struct = load %_Z5SliceI9JsonValueE, ptr %0, align 8
  %length = extractvalue %_Z5SliceI9JsonValueE %load.struct, 0
  %load.struct1 = load %_Z5SliceI9JsonValueE, ptr %1, align 8
  %length2 = extractvalue %_Z5SliceI9JsonValueE %load.struct1, 0
  %ne = icmp ne i64 %length, %length2
  br i1 %ne, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret i1 false

if.end:                                           ; preds = %entry
  %load.struct3 = load %_Z5SliceI9JsonValueE, ptr %0, align 8
  %length4 = extractvalue %_Z5SliceI9JsonValueE %load.struct3, 0
  %eq = icmp eq i64 %length4, 0
  br i1 %eq, label %if.then5, label %if.end6

if.then5:                                         ; preds = %if.end
  ret i1 true

if.end6:                                          ; preds = %if.end
  %load.struct7 = load %_Z5SliceI9JsonValueE, ptr %0, align 8
  %data = extractvalue %_Z5SliceI9JsonValueE %load.struct7, 1
  %load.struct8 = load %_Z5SliceI9JsonValueE, ptr %1, align 8
  %data9 = extractvalue %_Z5SliceI9JsonValueE %load.struct8, 1
  %load.struct10 = load %_Z5SliceI9JsonValueE, ptr %0, align 8
  %length11 = extractvalue %_Z5SliceI9JsonValueE %load.struct10, 0
  %mul = mul i64 %length11, ptrtoint (ptr getelementptr (%_Z9JsonValue, ptr null, i32 1) to i64)
  %call = call i32 @memcmp(ptr %data, ptr %data9, i64 %mul)
  %eq12 = icmp eq i32 %call, 0
  ret i1 %eq12
}

define linkonce_odr i1 @_ZN5SliceI9JsonValueB4jsonE11starts_withE5SliceI9JsonValueB4jsonE(ptr %0, ptr %1) {
entry:
  %sret.result = alloca %_Z5SliceI9JsonValueE, align 8
  %load.struct = load %_Z5SliceI9JsonValueE, ptr %1, align 8
  %length = extractvalue %_Z5SliceI9JsonValueE %load.struct, 0
  %load.struct1 = load %_Z5SliceI9JsonValueE, ptr %0, align 8
  %length2 = extractvalue %_Z5SliceI9JsonValueE %load.struct1, 0
  %gt = icmp ugt i64 %length, %length2
  br i1 %gt, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret i1 false

if.end:                                           ; preds = %entry
  %field.inplace = getelementptr inbounds nuw %_Z5SliceI9JsonValueE, ptr %1, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  call void @_ZN5SliceI9JsonValueB4jsonE8subsliceEmm(ptr noalias sret(%_Z5SliceI9JsonValueE) %sret.result, ptr %0, i64 0, i64 %field.val)
  %call = call i1 @_ZN5SliceI9JsonValueB4jsonE6equalsE5SliceI9JsonValueB4jsonE(ptr %sret.result, ptr %1)
  ret i1 %call
}

define linkonce_odr i1 @_ZN5SliceI9JsonValueB4jsonE9ends_withE5SliceI9JsonValueB4jsonE(ptr %0, ptr %1) {
entry:
  %sret.result = alloca %_Z5SliceI9JsonValueE, align 8
  %load.struct = load %_Z5SliceI9JsonValueE, ptr %1, align 8
  %length = extractvalue %_Z5SliceI9JsonValueE %load.struct, 0
  %load.struct1 = load %_Z5SliceI9JsonValueE, ptr %0, align 8
  %length2 = extractvalue %_Z5SliceI9JsonValueE %load.struct1, 0
  %gt = icmp ugt i64 %length, %length2
  br i1 %gt, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret i1 false

if.end:                                           ; preds = %entry
  %load.struct3 = load %_Z5SliceI9JsonValueE, ptr %0, align 8
  %length4 = extractvalue %_Z5SliceI9JsonValueE %load.struct3, 0
  %load.struct5 = load %_Z5SliceI9JsonValueE, ptr %1, align 8
  %length6 = extractvalue %_Z5SliceI9JsonValueE %load.struct5, 0
  %sub = sub i64 %length4, %length6
  %field.inplace = getelementptr inbounds nuw %_Z5SliceI9JsonValueE, ptr %0, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  call void @_ZN5SliceI9JsonValueB4jsonE8subsliceEmm(ptr noalias sret(%_Z5SliceI9JsonValueE) %sret.result, ptr %0, i64 %sub, i64 %field.val)
  %call = call i1 @_ZN5SliceI9JsonValueB4jsonE6equalsE5SliceI9JsonValueB4jsonE(ptr %sret.result, ptr %1)
  ret i1 %call
}

define linkonce_odr ptr @_ZN13SliceIteratorI9JsonValueB4jsonE4nextEv(ptr %0) {
entry:
  %load.struct = load %_Z13SliceIteratorI9JsonValueE, ptr %0, align 8
  %position = extractvalue %_Z13SliceIteratorI9JsonValueE %load.struct, 1
  %load.struct1 = load %_Z13SliceIteratorI9JsonValueE, ptr %0, align 8
  %slice = extractvalue %_Z13SliceIteratorI9JsonValueE %load.struct1, 0
  %length = extractvalue %_Z5SliceI9JsonValueE %slice, 0
  %ge = icmp uge i64 %position, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %load.struct2 = load %_Z13SliceIteratorI9JsonValueE, ptr %0, align 8
  %slice3 = extractvalue %_Z13SliceIteratorI9JsonValueE %load.struct2, 0
  %data = extractvalue %_Z5SliceI9JsonValueE %slice3, 1
  %load.struct4 = load %_Z13SliceIteratorI9JsonValueE, ptr %0, align 8
  %position5 = extractvalue %_Z13SliceIteratorI9JsonValueE %load.struct4, 1
  %ptr.add = getelementptr inbounds %_Z9JsonValue, ptr %data, i64 %position5
  %load.struct6 = load %_Z13SliceIteratorI9JsonValueE, ptr %0, align 8
  %position7 = extractvalue %_Z13SliceIteratorI9JsonValueE %load.struct6, 1
  %add = add i64 %position7, 1
  %position8 = getelementptr inbounds nuw %_Z13SliceIteratorI9JsonValueE, ptr %0, i32 0, i32 1
  store i64 %add, ptr %position8, align 8
  ret ptr %ptr.add
}

define linkonce_odr void @_ZN13SliceIteratorI9JsonValueB4jsonEC1E5SliceI9JsonValueB4jsonE(ptr %0, ptr %1) {
entry:
  %slice = getelementptr inbounds nuw %_Z13SliceIteratorI9JsonValueE, ptr %0, i32 0, i32 0
  %field.load = load %_Z5SliceI9JsonValueE, ptr %1, align 8
  store %_Z5SliceI9JsonValueE %field.load, ptr %slice, align 8
  %position = getelementptr inbounds nuw %_Z13SliceIteratorI9JsonValueE, ptr %0, i32 0, i32 1
  store i64 0, ptr %position, align 8
  ret void
}

define linkonce_odr void @_ZN5SliceI9JsonValueB4jsonE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z13SliceIteratorI9JsonValueE) %0, ptr %1, ptr %2) {
entry:
  %struct.init = alloca %_Z13SliceIteratorI9JsonValueE, align 8
  call void @_ZN13SliceIteratorI9JsonValueB4jsonEC1E5SliceI9JsonValueB4jsonE(ptr %struct.init, ptr %2)
  %sret.body = load %_Z13SliceIteratorI9JsonValueE, ptr %struct.init, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %struct.init, i64 ptrtoint (ptr getelementptr (%_Z13SliceIteratorI9JsonValueE, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN5SliceI9JsonValueB4jsonEC1Ev(ptr %0) {
entry:
  %data = getelementptr inbounds nuw %_Z5SliceI9JsonValueE, ptr %0, i32 0, i32 1
  store ptr null, ptr %data, align 8
  %length = getelementptr inbounds nuw %_Z5SliceI9JsonValueE, ptr %0, i32 0, i32 0
  store i64 0, ptr %length, align 8
  ret void
}

declare ptr @_ZN4Page3getEPv(ptr)

declare ptr @_ZN4Page8allocateEmm(ptr, i64, i64)

define linkonce_odr %_Z5SliceI2u8E @_Z17allocate_slice2u8R4Pagem(ptr %0, i64 %1) {
entry:
  %mul = mul i64 %1, ptrtoint (ptr getelementptr (i8, ptr null, i32 1) to i64)
  %call = call ptr @_ZN4Page8allocateEmm(ptr %0, i64 %mul, i64 1)
  %tuple = alloca %_Z5SliceI2u8E, align 8
  %tuple.field = getelementptr inbounds nuw %_Z5SliceI2u8E, ptr %tuple, i32 0, i32 0
  store i64 %1, ptr %tuple.field, align 1
  %tuple.field1 = getelementptr inbounds nuw %_Z5SliceI2u8E, ptr %tuple, i32 0, i32 1
  store ptr %call, ptr %tuple.field1, align 1
  %tuple.val = load %_Z5SliceI2u8E, ptr %tuple, align 8
  ret %_Z5SliceI2u8E %tuple.val
}

define linkonce_odr void @_ZN9JsonArenaB4json13keep_documentEPN4scaly6memory4PageE5SliceI2u8E(ptr noalias sret(%_Z5SliceI2u8E) %0, ptr %1, ptr %2, ptr %3) {
entry:
  %arg.tmp14 = alloca %_Z5SliceI2u8E, align 8
  %sret.result = alloca %_Z5SliceI2u8E, align 8
  %arg.tmp = alloca %_Z5SliceI2u8E, align 8
  %i = alloca i64, align 8
  %call = call ptr @_ZN4Page3getEPv(ptr %2)
  %load.struct = load %_Z5SliceI2u8E, ptr %3, align 8
  %length = extractvalue %_Z5SliceI2u8E %load.struct, 0
  %add = add i64 %length, 16
  %call1 = call %_Z5SliceI2u8E @_Z17allocate_slice2u8R4Pagem(ptr %call, i64 %add)
  %load.struct2 = load %_Z5SliceI2u8E, ptr %3, align 8
  %length3 = extractvalue %_Z5SliceI2u8E %load.struct2, 0
  %gt = icmp ugt i64 %length3, 0
  br i1 %gt, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %data = extractvalue %_Z5SliceI2u8E %call1, 1
  %load.struct4 = load %_Z5SliceI2u8E, ptr %3, align 8
  %data5 = extractvalue %_Z5SliceI2u8E %load.struct4, 1
  %field.inplace = getelementptr inbounds nuw %_Z5SliceI2u8E, ptr %3, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  %call6 = call ptr @memcpy(ptr %data, ptr %data5, i64 %field.val)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %load.struct7 = load %_Z5SliceI2u8E, ptr %3, align 8
  %length8 = extractvalue %_Z5SliceI2u8E %load.struct7, 0
  store i64 %length8, ptr %i, align 1
  br label %while.cond

while.cond:                                       ; preds = %while.body, %if.end
  %i9 = load i64, ptr %i, align 8
  %length10 = extractvalue %_Z5SliceI2u8E %call1, 0
  %lt = icmp ult i64 %i9, %length10
  br i1 %lt, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  store %_Z5SliceI2u8E %call1, ptr %arg.tmp, align 1
  %i11 = load i64, ptr %i, align 8
  call void @_ZN5SliceI2u8E3putEm2u8(ptr %arg.tmp, i64 %i11, i8 0)
  %i12 = load i64, ptr %i, align 8
  %add13 = add i64 %i12, 1
  store i64 %add13, ptr %i, align 1
  br label %while.cond

while.exit:                                       ; preds = %while.cond
  store %_Z5SliceI2u8E %call1, ptr %arg.tmp14, align 1
  %field.inplace15 = getelementptr inbounds nuw %_Z5SliceI2u8E, ptr %3, i32 0, i32 0
  %field.val16 = load i64, ptr %field.inplace15, align 8
  call void @_ZN5SliceI2u8E8subsliceEmm(ptr noalias sret(%_Z5SliceI2u8E) %sret.result, ptr %arg.tmp14, i64 0, i64 %field.val16)
  %sret.body = load %_Z5SliceI2u8E, ptr %sret.result, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %sret.result, i64 ptrtoint (ptr getelementptr (%_Z5SliceI2u8E, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr %_Z5SliceI9JsonValueE @_Z24allocate_slice9JsonValueR4Pagem(ptr %0, i64 %1) {
entry:
  %mul = mul i64 %1, ptrtoint (ptr getelementptr (%_Z9JsonValue, ptr null, i32 1) to i64)
  %call = call ptr @_ZN4Page8allocateEmm(ptr %0, i64 %mul, i64 8)
  %tuple = alloca %_Z5SliceI9JsonValueE, align 8
  %tuple.field = getelementptr inbounds nuw %_Z5SliceI9JsonValueE, ptr %tuple, i32 0, i32 0
  store i64 %1, ptr %tuple.field, align 1
  %tuple.field1 = getelementptr inbounds nuw %_Z5SliceI9JsonValueE, ptr %tuple, i32 0, i32 1
  store ptr %call, ptr %tuple.field1, align 1
  %tuple.val = load %_Z5SliceI9JsonValueE, ptr %tuple, align 8
  ret %_Z5SliceI9JsonValueE %tuple.val
}

define linkonce_odr void @_ZN5SliceI9JsonValueB4jsonEixEm(ptr noalias sret(%_Z9JsonValue) %0, ptr %1, i64 %2) {
entry:
  %deref.tmp = alloca %_Z9JsonValue, align 8
  %load.struct = load %_Z5SliceI9JsonValueE, ptr %1, align 8
  %length = extractvalue %_Z5SliceI9JsonValueE %load.struct, 0
  %ge = icmp uge i64 %2, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %field.inplace = getelementptr inbounds nuw %_Z5SliceI9JsonValueE, ptr %1, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  call void @_Z15scaly_panic_oobP10const_charmm(ptr @.str.27, i64 %2, i64 %field.val)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %load.struct1 = load %_Z5SliceI9JsonValueE, ptr %1, align 8
  %data = extractvalue %_Z5SliceI9JsonValueE %load.struct1, 1
  %ptr.add = getelementptr inbounds %_Z9JsonValue, ptr %data, i64 %2
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %deref.tmp, ptr align 1 %ptr.add, i64 ptrtoint (ptr getelementptr (%_Z9JsonValue, ptr null, i32 1) to i64), i1 false)
  %sret.body = load %_Z9JsonValue, ptr %deref.tmp, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %deref.tmp, i64 ptrtoint (ptr getelementptr (%_Z9JsonValue, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN9JsonArenaB4json11keep_valuesEPN4scaly6memory4PageE5SliceI9JsonValueB4jsonE(ptr noalias sret(%_Z5SliceI9JsonValueE) %0, ptr %1, ptr %2, ptr %3) {
entry:
  %sret.result25 = alloca %_Z9JsonValue, align 8
  %i = alloca i64, align 8
  %sret.result = alloca %_Z5SliceI9JsonValueE, align 8
  %load.struct = load %_Z5SliceI9JsonValueE, ptr %3, align 8
  %length = extractvalue %_Z5SliceI9JsonValueE %load.struct, 0
  %gt = icmp ugt i64 %length, 128
  br i1 %gt, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %call = call ptr @_ZN4Page3getEPv(ptr %2)
  %call1 = call %_Z5SliceI9JsonValueE @_Z24allocate_slice9JsonValueR4Pagem(ptr %call, i64 %length)
  %data = extractvalue %_Z5SliceI9JsonValueE %call1, 1
  %load.struct2 = load %_Z5SliceI9JsonValueE, ptr %3, align 8
  %data3 = extractvalue %_Z5SliceI9JsonValueE %load.struct2, 1
  %mul = mul i64 %length, ptrtoint (ptr getelementptr (%_Z9JsonValue, ptr null, i32 1) to i64)
  %call4 = call ptr @memcpy(ptr %data, ptr %data3, i64 %mul)
  store %_Z5SliceI9JsonValueE %call1, ptr %0, align 1
  ret void

if.end:                                           ; preds = %entry
  %load.struct5 = load %_Z9JsonArena, ptr %2, align 8
  %value_used = extractvalue %_Z9JsonArena %load.struct5, 1
  %add = add i64 %value_used, %length
  %load.struct6 = load %_Z9JsonArena, ptr %2, align 8
  %value_room = extractvalue %_Z9JsonArena %load.struct6, 0
  %length7 = extractvalue %_Z5SliceI9JsonValueE %value_room, 0
  %gt8 = icmp ugt i64 %add, %length7
  br i1 %gt8, label %if.then9, label %if.end10

if.then9:                                         ; preds = %if.end
  %call11 = call ptr @_ZN4Page3getEPv(ptr %2)
  %call12 = call %_Z5SliceI9JsonValueE @_Z24allocate_slice9JsonValueR4Pagem(ptr %call11, i64 128)
  %value_room13 = getelementptr inbounds nuw %_Z9JsonArena, ptr %2, i32 0, i32 0
  store %_Z5SliceI9JsonValueE %call12, ptr %value_room13, align 8
  %value_used14 = getelementptr inbounds nuw %_Z9JsonArena, ptr %2, i32 0, i32 1
  store i64 0, ptr %value_used14, align 8
  br label %if.end10

if.end10:                                         ; preds = %if.then9, %if.end
  %field.inplace = getelementptr inbounds nuw %_Z9JsonArena, ptr %2, i32 0, i32 0
  %field.inplace15 = getelementptr inbounds nuw %_Z9JsonArena, ptr %2, i32 0, i32 1
  %field.val = load i64, ptr %field.inplace15, align 8
  %load.struct16 = load %_Z9JsonArena, ptr %2, align 8
  %value_used17 = extractvalue %_Z9JsonArena %load.struct16, 1
  %add18 = add i64 %value_used17, %length
  call void @_ZN5SliceI9JsonValueB4jsonE8subsliceEmm(ptr noalias sret(%_Z5SliceI9JsonValueE) %sret.result, ptr %field.inplace, i64 %field.val, i64 %add18)
  %load.struct19 = load %_Z9JsonArena, ptr %2, align 8
  %value_used20 = extractvalue %_Z9JsonArena %load.struct19, 1
  %add21 = add i64 %value_used20, %length
  %value_used22 = getelementptr inbounds nuw %_Z9JsonArena, ptr %2, i32 0, i32 1
  store i64 %add21, ptr %value_used22, align 8
  store i64 0, ptr %i, align 1
  br label %while.cond

while.cond:                                       ; preds = %while.body, %if.end10
  %i23 = load i64, ptr %i, align 8
  %lt = icmp ult i64 %i23, %length
  br i1 %lt, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %i24 = load i64, ptr %i, align 8
  %i26 = load i64, ptr %i, align 8
  call void @_ZN5SliceI9JsonValueB4jsonEixEm(ptr noalias sret(%_Z9JsonValue) %sret.result25, ptr %3, i64 %i26)
  call void @_ZN5SliceI9JsonValueB4jsonE3putEm9JsonValueB4json(ptr %sret.result, i64 %i24, ptr %sret.result25)
  %i27 = load i64, ptr %i, align 8
  %add28 = add i64 %i27, 1
  store i64 %add28, ptr %i, align 1
  br label %while.cond

while.exit:                                       ; preds = %while.cond
  %sret.body = load %_Z5SliceI9JsonValueE, ptr %sret.result, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %sret.result, i64 ptrtoint (ptr getelementptr (%_Z5SliceI9JsonValueE, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr %_Z5SliceI10JsonMemberE @_Z26allocate_slice10JsonMemberR4Pagem(ptr %0, i64 %1) {
entry:
  %mul = mul i64 %1, ptrtoint (ptr getelementptr (%_Z10JsonMember, ptr null, i32 1) to i64)
  %call = call ptr @_ZN4Page8allocateEmm(ptr %0, i64 %mul, i64 8)
  %tuple = alloca %_Z5SliceI10JsonMemberE, align 8
  %tuple.field = getelementptr inbounds nuw %_Z5SliceI10JsonMemberE, ptr %tuple, i32 0, i32 0
  store i64 %1, ptr %tuple.field, align 1
  %tuple.field1 = getelementptr inbounds nuw %_Z5SliceI10JsonMemberE, ptr %tuple, i32 0, i32 1
  store ptr %call, ptr %tuple.field1, align 1
  %tuple.val = load %_Z5SliceI10JsonMemberE, ptr %tuple, align 8
  ret %_Z5SliceI10JsonMemberE %tuple.val
}

define linkonce_odr void @_ZN5SliceI10JsonMemberB4jsonEixEm(ptr noalias sret(%_Z10JsonMember) %0, ptr %1, i64 %2) {
entry:
  %deref.tmp = alloca %_Z10JsonMember, align 8
  %load.struct = load %_Z5SliceI10JsonMemberE, ptr %1, align 8
  %length = extractvalue %_Z5SliceI10JsonMemberE %load.struct, 0
  %ge = icmp uge i64 %2, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %field.inplace = getelementptr inbounds nuw %_Z5SliceI10JsonMemberE, ptr %1, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  call void @_Z15scaly_panic_oobP10const_charmm(ptr @.str.28, i64 %2, i64 %field.val)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %load.struct1 = load %_Z5SliceI10JsonMemberE, ptr %1, align 8
  %data = extractvalue %_Z5SliceI10JsonMemberE %load.struct1, 1
  %ptr.add = getelementptr inbounds %_Z10JsonMember, ptr %data, i64 %2
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %deref.tmp, ptr align 1 %ptr.add, i64 ptrtoint (ptr getelementptr (%_Z10JsonMember, ptr null, i32 1) to i64), i1 false)
  %sret.body = load %_Z10JsonMember, ptr %deref.tmp, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %deref.tmp, i64 ptrtoint (ptr getelementptr (%_Z10JsonMember, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN9JsonArenaB4json12keep_membersEPN4scaly6memory4PageE5SliceI10JsonMemberB4jsonE(ptr noalias sret(%_Z5SliceI10JsonMemberE) %0, ptr %1, ptr %2, ptr %3) {
entry:
  %sret.result25 = alloca %_Z10JsonMember, align 8
  %i = alloca i64, align 8
  %sret.result = alloca %_Z5SliceI10JsonMemberE, align 8
  %load.struct = load %_Z5SliceI10JsonMemberE, ptr %3, align 8
  %length = extractvalue %_Z5SliceI10JsonMemberE %load.struct, 0
  %gt = icmp ugt i64 %length, 128
  br i1 %gt, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %call = call ptr @_ZN4Page3getEPv(ptr %2)
  %call1 = call %_Z5SliceI10JsonMemberE @_Z26allocate_slice10JsonMemberR4Pagem(ptr %call, i64 %length)
  %data = extractvalue %_Z5SliceI10JsonMemberE %call1, 1
  %load.struct2 = load %_Z5SliceI10JsonMemberE, ptr %3, align 8
  %data3 = extractvalue %_Z5SliceI10JsonMemberE %load.struct2, 1
  %mul = mul i64 %length, ptrtoint (ptr getelementptr (%_Z10JsonMember, ptr null, i32 1) to i64)
  %call4 = call ptr @memcpy(ptr %data, ptr %data3, i64 %mul)
  store %_Z5SliceI10JsonMemberE %call1, ptr %0, align 1
  ret void

if.end:                                           ; preds = %entry
  %load.struct5 = load %_Z9JsonArena, ptr %2, align 8
  %member_used = extractvalue %_Z9JsonArena %load.struct5, 3
  %add = add i64 %member_used, %length
  %load.struct6 = load %_Z9JsonArena, ptr %2, align 8
  %member_room = extractvalue %_Z9JsonArena %load.struct6, 2
  %length7 = extractvalue %_Z5SliceI10JsonMemberE %member_room, 0
  %gt8 = icmp ugt i64 %add, %length7
  br i1 %gt8, label %if.then9, label %if.end10

if.then9:                                         ; preds = %if.end
  %call11 = call ptr @_ZN4Page3getEPv(ptr %2)
  %call12 = call %_Z5SliceI10JsonMemberE @_Z26allocate_slice10JsonMemberR4Pagem(ptr %call11, i64 128)
  %member_room13 = getelementptr inbounds nuw %_Z9JsonArena, ptr %2, i32 0, i32 2
  store %_Z5SliceI10JsonMemberE %call12, ptr %member_room13, align 8
  %member_used14 = getelementptr inbounds nuw %_Z9JsonArena, ptr %2, i32 0, i32 3
  store i64 0, ptr %member_used14, align 8
  br label %if.end10

if.end10:                                         ; preds = %if.then9, %if.end
  %field.inplace = getelementptr inbounds nuw %_Z9JsonArena, ptr %2, i32 0, i32 2
  %field.inplace15 = getelementptr inbounds nuw %_Z9JsonArena, ptr %2, i32 0, i32 3
  %field.val = load i64, ptr %field.inplace15, align 8
  %load.struct16 = load %_Z9JsonArena, ptr %2, align 8
  %member_used17 = extractvalue %_Z9JsonArena %load.struct16, 3
  %add18 = add i64 %member_used17, %length
  call void @_ZN5SliceI10JsonMemberB4jsonE8subsliceEmm(ptr noalias sret(%_Z5SliceI10JsonMemberE) %sret.result, ptr %field.inplace, i64 %field.val, i64 %add18)
  %load.struct19 = load %_Z9JsonArena, ptr %2, align 8
  %member_used20 = extractvalue %_Z9JsonArena %load.struct19, 3
  %add21 = add i64 %member_used20, %length
  %member_used22 = getelementptr inbounds nuw %_Z9JsonArena, ptr %2, i32 0, i32 3
  store i64 %add21, ptr %member_used22, align 8
  store i64 0, ptr %i, align 1
  br label %while.cond

while.cond:                                       ; preds = %while.body, %if.end10
  %i23 = load i64, ptr %i, align 8
  %lt = icmp ult i64 %i23, %length
  br i1 %lt, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %i24 = load i64, ptr %i, align 8
  %i26 = load i64, ptr %i, align 8
  call void @_ZN5SliceI10JsonMemberB4jsonEixEm(ptr noalias sret(%_Z10JsonMember) %sret.result25, ptr %3, i64 %i26)
  call void @_ZN5SliceI10JsonMemberB4jsonE3putEm10JsonMemberB4json(ptr %sret.result, i64 %i24, ptr %sret.result25)
  %i27 = load i64, ptr %i, align 8
  %add28 = add i64 %i27, 1
  store i64 %add28, ptr %i, align 1
  br label %while.cond

while.exit:                                       ; preds = %while.cond
  %sret.body = load %_Z5SliceI10JsonMemberE, ptr %sret.result, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %sret.result, i64 ptrtoint (ptr getelementptr (%_Z5SliceI10JsonMemberE, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN9JsonArenaB4jsonC1Ev(ptr %0) {
entry:
  %call = call ptr @_ZN4Page3getEPv(ptr %0)
  %value_room = getelementptr inbounds nuw %_Z9JsonArena, ptr %0, i32 0, i32 0
  %data = getelementptr inbounds nuw %_Z5SliceI9JsonValueE, ptr %value_room, i32 0, i32 1
  store ptr null, ptr %data, align 8
  %value_room1 = getelementptr inbounds nuw %_Z9JsonArena, ptr %0, i32 0, i32 0
  %length = getelementptr inbounds nuw %_Z5SliceI9JsonValueE, ptr %value_room1, i32 0, i32 0
  store i64 0, ptr %length, align 8
  %value_used = getelementptr inbounds nuw %_Z9JsonArena, ptr %0, i32 0, i32 1
  store i64 0, ptr %value_used, align 8
  %member_room = getelementptr inbounds nuw %_Z9JsonArena, ptr %0, i32 0, i32 2
  %data2 = getelementptr inbounds nuw %_Z5SliceI10JsonMemberE, ptr %member_room, i32 0, i32 1
  store ptr null, ptr %data2, align 8
  %member_room3 = getelementptr inbounds nuw %_Z9JsonArena, ptr %0, i32 0, i32 2
  %length4 = getelementptr inbounds nuw %_Z5SliceI10JsonMemberE, ptr %member_room3, i32 0, i32 0
  store i64 0, ptr %length4, align 8
  %member_used = getelementptr inbounds nuw %_Z9JsonArena, ptr %0, i32 0, i32 3
  store i64 0, ptr %member_used, align 8
  ret void
}

define linkonce_odr i1 @_ZN9JsonValueB4json7is_nullEv(ptr %0) {
entry:
  %tag.ptr = getelementptr inbounds nuw %_Z9JsonValue, ptr %0, i32 0, i32 0
  %tag = load i8, ptr %tag.ptr, align 1
  switch i8 %tag, label %choose.else [
    i8 0, label %choose.when
  ]

choose.end:                                       ; No predecessors!
  ret i1 false

choose.else:                                      ; preds = %entry
  ret i1 false

choose.when:                                      ; preds = %entry
  %"variant.c_data().ptr" = getelementptr inbounds nuw %_Z9JsonValue, ptr %0, i32 0, i32 1
  ret i1 true
}

define linkonce_odr i1 @_ZN9JsonValueB4json7is_boolEv(ptr %0) {
entry:
  %tag.ptr = getelementptr inbounds nuw %_Z9JsonValue, ptr %0, i32 0, i32 0
  %tag = load i8, ptr %tag.ptr, align 1
  switch i8 %tag, label %choose.else [
    i8 1, label %choose.when
  ]

choose.end:                                       ; No predecessors!
  ret i1 false

choose.else:                                      ; preds = %entry
  ret i1 false

choose.when:                                      ; preds = %entry
  %"variant.c_data().ptr" = getelementptr inbounds nuw %_Z9JsonValue, ptr %0, i32 0, i32 1
  %variant.val = load i1, ptr %"variant.c_data().ptr", align 1
  ret i1 true
}

define linkonce_odr i1 @_ZN9JsonValueB4json9is_numberEv(ptr %0) {
entry:
  %tag.ptr = getelementptr inbounds nuw %_Z9JsonValue, ptr %0, i32 0, i32 0
  %tag = load i8, ptr %tag.ptr, align 1
  switch i8 %tag, label %choose.else [
    i8 2, label %choose.when
  ]

choose.end:                                       ; No predecessors!
  ret i1 false

choose.else:                                      ; preds = %entry
  ret i1 false

choose.when:                                      ; preds = %entry
  %"variant.c_data().ptr" = getelementptr inbounds nuw %_Z9JsonValue, ptr %0, i32 0, i32 1
  %variant.val = load %_Z10JsonNumber, ptr %"variant.c_data().ptr", align 8
  ret i1 true
}

define linkonce_odr i1 @_ZN9JsonValueB4json7is_textEv(ptr %0) {
entry:
  %tag.ptr = getelementptr inbounds nuw %_Z9JsonValue, ptr %0, i32 0, i32 0
  %tag = load i8, ptr %tag.ptr, align 1
  switch i8 %tag, label %choose.else [
    i8 3, label %choose.when
  ]

choose.end:                                       ; No predecessors!
  ret i1 false

choose.else:                                      ; preds = %entry
  ret i1 false

choose.when:                                      ; preds = %entry
  %"variant.c_data().ptr" = getelementptr inbounds nuw %_Z9JsonValue, ptr %0, i32 0, i32 1
  %variant.val = load %_Z10JsonString, ptr %"variant.c_data().ptr", align 8
  ret i1 true
}

define linkonce_odr i1 @_ZN9JsonValueB4json8is_arrayEv(ptr %0) {
entry:
  %tag.ptr = getelementptr inbounds nuw %_Z9JsonValue, ptr %0, i32 0, i32 0
  %tag = load i8, ptr %tag.ptr, align 1
  switch i8 %tag, label %choose.else [
    i8 4, label %choose.when
  ]

choose.end:                                       ; No predecessors!
  ret i1 false

choose.else:                                      ; preds = %entry
  ret i1 false

choose.when:                                      ; preds = %entry
  %"variant.c_data().ptr" = getelementptr inbounds nuw %_Z9JsonValue, ptr %0, i32 0, i32 1
  %variant.val = load %_Z9JsonArray, ptr %"variant.c_data().ptr", align 8
  ret i1 true
}

define linkonce_odr i1 @_ZN9JsonValueB4json9is_objectEv(ptr %0) {
entry:
  %tag.ptr = getelementptr inbounds nuw %_Z9JsonValue, ptr %0, i32 0, i32 0
  %tag = load i8, ptr %tag.ptr, align 1
  switch i8 %tag, label %choose.else [
    i8 5, label %choose.when
  ]

choose.end:                                       ; No predecessors!
  ret i1 false

choose.else:                                      ; preds = %entry
  ret i1 false

choose.when:                                      ; preds = %entry
  %"variant.c_data().ptr" = getelementptr inbounds nuw %_Z9JsonValue, ptr %0, i32 0, i32 1
  %variant.val = load %_Z10JsonObject, ptr %"variant.c_data().ptr", align 8
  ret i1 true
}

define linkonce_odr i1 @_ZN9JsonValueB4json7as_boolEv(ptr %0) {
entry:
  %tag.ptr = getelementptr inbounds nuw %_Z9JsonValue, ptr %0, i32 0, i32 0
  %tag = load i8, ptr %tag.ptr, align 1
  switch i8 %tag, label %choose.else [
    i8 1, label %choose.when
  ]

choose.end:                                       ; No predecessors!
  ret i1 false

choose.else:                                      ; preds = %entry
  ret i1 false

choose.when:                                      ; preds = %entry
  %"variant.c_data().ptr" = getelementptr inbounds nuw %_Z9JsonValue, ptr %0, i32 0, i32 1
  %variant.val = load i1, ptr %"variant.c_data().ptr", align 1
  ret i1 %variant.val
}

define linkonce_odr void @_ZN9JsonValueB4json9as_stringEPN4scaly6memory4PageE(ptr noalias sret({ ptr }) %0, ptr %1, ptr %2) {
entry:
  %tag.ptr = getelementptr inbounds nuw %_Z9JsonValue, ptr %2, i32 0, i32 0
  %tag = load i8, ptr %tag.ptr, align 1
  switch i8 %tag, label %choose.else [
    i8 3, label %choose.when
  ]

choose.end:                                       ; No predecessors!
  ret void

choose.else:                                      ; preds = %entry
  %frame.page3 = load ptr, ptr %1, align 8
  %frame.has_page4 = icmp ne ptr %frame.page3, null
  br i1 %frame.has_page4, label %frame.forced6, label %frame.force5

choose.when:                                      ; preds = %entry
  %"variant.c_data().ptr" = getelementptr inbounds nuw %_Z9JsonValue, ptr %2, i32 0, i32 1
  %variant.val = load %_Z10JsonString, ptr %"variant.c_data().ptr", align 8
  %frame.page = load ptr, ptr %1, align 8
  %frame.has_page = icmp ne ptr %frame.page, null
  br i1 %frame.has_page, label %frame.forced, label %frame.force

frame.force:                                      ; preds = %choose.when
  %forced_page = call ptr @_Z17scaly_force_frameP5Frame(ptr %1)
  br label %frame.forced

frame.forced:                                     ; preds = %frame.force, %choose.when
  %forced_page1 = phi ptr [ %frame.page, %choose.when ], [ %forced_page, %frame.force ]
  %struct.region = call ptr @_ZN4Page8allocateEmm(ptr %forced_page1, i64 ptrtoint (ptr getelementptr ({ ptr }, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr ({ i1, { ptr } }, ptr null, i64 0, i32 1) to i64))
  %bytes = extractvalue %_Z10JsonString %variant.val, 0
  %data = extractvalue %_Z5SliceI2u8E %bytes, 1
  %bytes2 = extractvalue %_Z10JsonString %variant.val, 0
  %length = extractvalue %_Z5SliceI2u8E %bytes2, 0
  call void @_ZN6StringC1EP10const_charm(ptr %struct.region, ptr %data, i64 %length)
  %sret.body = load { ptr }, ptr %struct.region, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %struct.region, i64 ptrtoint (ptr getelementptr ({ ptr }, ptr null, i32 1) to i64), i1 false)
  ret void

frame.force5:                                     ; preds = %choose.else
  %forced_page7 = call ptr @_Z17scaly_force_frameP5Frame(ptr %1)
  br label %frame.forced6

frame.forced6:                                    ; preds = %frame.force5, %choose.else
  %forced_page8 = phi ptr [ %frame.page3, %choose.else ], [ %forced_page7, %frame.force5 ]
  %struct.region9 = call ptr @_ZN4Page8allocateEmm(ptr %forced_page8, i64 ptrtoint (ptr getelementptr ({ ptr }, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr ({ i1, { ptr } }, ptr null, i64 0, i32 1) to i64))
  call void @_ZN6StringC1Ev(ptr %struct.region9)
  %sret.body10 = load { ptr }, ptr %struct.region9, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %struct.region9, i64 ptrtoint (ptr getelementptr ({ ptr }, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN9JsonValueB4json4textEv(ptr noalias sret(%_Z5SliceI2u8E) %0, ptr %1) {
entry:
  %struct.init = alloca %_Z5SliceI2u8E, align 8
  %tag.ptr = getelementptr inbounds nuw %_Z9JsonValue, ptr %1, i32 0, i32 0
  %tag = load i8, ptr %tag.ptr, align 1
  switch i8 %tag, label %choose.else [
    i8 3, label %choose.when
  ]

choose.end:                                       ; No predecessors!
  ret void

choose.else:                                      ; preds = %entry
  %tuple.field = getelementptr inbounds nuw %_Z5SliceI2u8E, ptr %struct.init, i32 0, i32 0
  store i64 0, ptr %tuple.field, align 8
  %tuple.field1 = getelementptr inbounds nuw %_Z5SliceI2u8E, ptr %struct.init, i32 0, i32 1
  store ptr null, ptr %tuple.field1, align 8
  %sret.body = load %_Z5SliceI2u8E, ptr %struct.init, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %struct.init, i64 ptrtoint (ptr getelementptr (%_Z5SliceI2u8E, ptr null, i32 1) to i64), i1 false)
  ret void

choose.when:                                      ; preds = %entry
  %"variant.c_data().ptr" = getelementptr inbounds nuw %_Z9JsonValue, ptr %1, i32 0, i32 1
  %variant.val = load %_Z10JsonString, ptr %"variant.c_data().ptr", align 8
  %bytes = extractvalue %_Z10JsonString %variant.val, 0
  store %_Z5SliceI2u8E %bytes, ptr %0, align 1
  ret void
}

define linkonce_odr void @_ZN9JsonValueB4json11number_textEv(ptr noalias sret(%_Z5SliceI2u8E) %0, ptr %1) {
entry:
  %struct.init = alloca %_Z5SliceI2u8E, align 8
  %tag.ptr = getelementptr inbounds nuw %_Z9JsonValue, ptr %1, i32 0, i32 0
  %tag = load i8, ptr %tag.ptr, align 1
  switch i8 %tag, label %choose.else [
    i8 2, label %choose.when
  ]

choose.end:                                       ; No predecessors!
  ret void

choose.else:                                      ; preds = %entry
  %tuple.field = getelementptr inbounds nuw %_Z5SliceI2u8E, ptr %struct.init, i32 0, i32 0
  store i64 0, ptr %tuple.field, align 8
  %tuple.field1 = getelementptr inbounds nuw %_Z5SliceI2u8E, ptr %struct.init, i32 0, i32 1
  store ptr null, ptr %tuple.field1, align 8
  %sret.body = load %_Z5SliceI2u8E, ptr %struct.init, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %struct.init, i64 ptrtoint (ptr getelementptr (%_Z5SliceI2u8E, ptr null, i32 1) to i64), i1 false)
  ret void

choose.when:                                      ; preds = %entry
  %"variant.c_data().ptr" = getelementptr inbounds nuw %_Z9JsonValue, ptr %1, i32 0, i32 1
  %variant.val = load %_Z10JsonNumber, ptr %"variant.c_data().ptr", align 8
  %text = extractvalue %_Z10JsonNumber %variant.val, 0
  store %_Z5SliceI2u8E %text, ptr %0, align 1
  ret void
}

define linkonce_odr i1 @_ZN9JsonValueB4json12integer_fitsE5SliceI2u8E(ptr %0) {
entry:
  %v = alloca i64, align 8
  %limit = alloca i64, align 8
  %i = alloca i64, align 8
  %load.struct = load %_Z5SliceI2u8E, ptr %0, align 8
  %length = extractvalue %_Z5SliceI2u8E %load.struct, 0
  %eq = icmp eq i64 %length, 0
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret i1 false

if.end:                                           ; preds = %entry
  store i64 0, ptr %i, align 1
  store i64 9223372036854775807, ptr %limit, align 1
  %call = call i8 @_ZN5SliceI2u8EixEm(ptr %0, i64 0)
  %eq1 = icmp eq i8 %call, 45
  br i1 %eq1, label %if.then2, label %if.end3

if.then2:                                         ; preds = %if.end
  store i64 1, ptr %i, align 1
  store i64 -9223372036854775808, ptr %limit, align 1
  br label %if.end3

if.end3:                                          ; preds = %if.then2, %if.end
  %i4 = load i64, ptr %i, align 8
  %load.struct5 = load %_Z5SliceI2u8E, ptr %0, align 8
  %length6 = extractvalue %_Z5SliceI2u8E %load.struct5, 0
  %ge = icmp uge i64 %i4, %length6
  br i1 %ge, label %if.then7, label %if.end8

if.then7:                                         ; preds = %if.end3
  ret i1 false

if.end8:                                          ; preds = %if.end3
  store i64 0, ptr %v, align 1
  br label %while.cond

while.cond:                                       ; preds = %if.end22, %if.end8
  %i9 = load i64, ptr %i, align 8
  %load.struct10 = load %_Z5SliceI2u8E, ptr %0, align 8
  %length11 = extractvalue %_Z5SliceI2u8E %load.struct10, 0
  %lt = icmp ult i64 %i9, %length11
  br i1 %lt, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %i12 = load i64, ptr %i, align 8
  %call13 = call i8 @_ZN5SliceI2u8EixEm(ptr %0, i64 %i12)
  %lt16 = icmp ult i8 %call13, 48
  br i1 %lt16, label %if.then14, label %lor.rhs

while.exit:                                       ; preds = %while.cond
  ret i1 true

if.then14:                                        ; preds = %lor.rhs, %while.body
  ret i1 false

if.end15:                                         ; preds = %lor.rhs
  %sub = sub i8 %call13, 48
  %as.zext = zext i8 %sub to i64
  %v17 = load i64, ptr %v, align 8
  %limit18 = load i64, ptr %limit, align 8
  %sub19 = sub i64 %limit18, %as.zext
  %udiv = udiv i64 %sub19, 10
  %gt20 = icmp ugt i64 %v17, %udiv
  br i1 %gt20, label %if.then21, label %if.end22

lor.rhs:                                          ; preds = %while.body
  %gt = icmp ugt i8 %call13, 57
  br i1 %gt, label %if.then14, label %if.end15

if.then21:                                        ; preds = %if.end15
  ret i1 false

if.end22:                                         ; preds = %if.end15
  %v23 = load i64, ptr %v, align 8
  %mul = mul i64 %v23, 10
  %add = add i64 %mul, %as.zext
  store i64 %add, ptr %v, align 1
  %i24 = load i64, ptr %i, align 8
  %add25 = add i64 %i24, 1
  store i64 %add25, ptr %i, align 1
  br label %while.cond
}

define linkonce_odr i1 @_ZN9JsonValueB4json10is_integerEv(ptr %0) {
entry:
  %arg.tmp = alloca %_Z5SliceI2u8E, align 8
  %tag.ptr = getelementptr inbounds nuw %_Z9JsonValue, ptr %0, i32 0, i32 0
  %tag = load i8, ptr %tag.ptr, align 1
  switch i8 %tag, label %choose.else [
    i8 2, label %choose.when
  ]

choose.end:                                       ; No predecessors!
  ret i1 false

choose.else:                                      ; preds = %entry
  ret i1 false

choose.when:                                      ; preds = %entry
  %"variant.c_data().ptr" = getelementptr inbounds nuw %_Z9JsonValue, ptr %0, i32 0, i32 1
  %variant.val = load %_Z10JsonNumber, ptr %"variant.c_data().ptr", align 8
  %text = extractvalue %_Z10JsonNumber %variant.val, 0
  store %_Z5SliceI2u8E %text, ptr %arg.tmp, align 1
  %call = call i1 @_ZN9JsonValueB4json12integer_fitsE5SliceI2u8E(ptr %arg.tmp)
  ret i1 %call
}

define linkonce_odr i64 @_ZN9JsonValueB4json13integer_valueE5SliceI2u8E(ptr %0) {
entry:
  %v = alloca i64, align 8
  %i = alloca i64, align 8
  store i64 0, ptr %i, align 1
  %negative = alloca i1, align 1
  store i1 false, ptr %negative, align 1
  %call = call i8 @_ZN5SliceI2u8EixEm(ptr %0, i64 0)
  %eq = icmp eq i8 %call, 45
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  store i64 1, ptr %i, align 1
  store i1 true, ptr %negative, align 1
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  store i64 0, ptr %v, align 1
  br label %while.cond

while.cond:                                       ; preds = %while.body, %if.end
  %i1 = load i64, ptr %i, align 8
  %load.struct = load %_Z5SliceI2u8E, ptr %0, align 8
  %length = extractvalue %_Z5SliceI2u8E %load.struct, 0
  %lt = icmp ult i64 %i1, %length
  br i1 %lt, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %v2 = load i64, ptr %v, align 8
  %mul = mul i64 %v2, 10
  %i3 = load i64, ptr %i, align 8
  %call4 = call i8 @_ZN5SliceI2u8EixEm(ptr %0, i64 %i3)
  %sub = sub i8 %call4, 48
  %as.zext = zext i8 %sub to i64
  %add = add i64 %mul, %as.zext
  store i64 %add, ptr %v, align 1
  %i5 = load i64, ptr %i, align 8
  %add6 = add i64 %i5, 1
  store i64 %add6, ptr %i, align 1
  br label %while.cond

while.exit:                                       ; preds = %while.cond
  %negative7 = load i1, ptr %negative, align 1
  br i1 %negative7, label %if.then8, label %if.end9

if.then8:                                         ; preds = %while.exit
  %v10 = load i64, ptr %v, align 8
  %sub11 = sub i64 0, %v10
  ret i64 %sub11

if.end9:                                          ; preds = %while.exit
  %v12 = load i64, ptr %v, align 8
  ret i64 %v12
}

define linkonce_odr i64 @_ZN9JsonValueB4json6as_i64Ev(ptr %0) {
entry:
  %arg.tmp2 = alloca %_Z5SliceI2u8E, align 8
  %arg.tmp = alloca %_Z5SliceI2u8E, align 8
  %tag.ptr = getelementptr inbounds nuw %_Z9JsonValue, ptr %0, i32 0, i32 0
  %tag = load i8, ptr %tag.ptr, align 1
  switch i8 %tag, label %choose.else [
    i8 2, label %choose.when
  ]

choose.end:                                       ; No predecessors!
  ret i64 0

choose.else:                                      ; preds = %entry
  ret i64 0

choose.when:                                      ; preds = %entry
  %"variant.c_data().ptr" = getelementptr inbounds nuw %_Z9JsonValue, ptr %0, i32 0, i32 1
  %variant.val = load %_Z10JsonNumber, ptr %"variant.c_data().ptr", align 8
  %text = extractvalue %_Z10JsonNumber %variant.val, 0
  store %_Z5SliceI2u8E %text, ptr %arg.tmp, align 1
  %call = call i1 @_ZN9JsonValueB4json12integer_fitsE5SliceI2u8E(ptr %arg.tmp)
  br i1 %call, label %if.then, label %if.end

if.then:                                          ; preds = %choose.when
  %text1 = extractvalue %_Z10JsonNumber %variant.val, 0
  store %_Z5SliceI2u8E %text1, ptr %arg.tmp2, align 1
  %call3 = call i64 @_ZN9JsonValueB4json13integer_valueE5SliceI2u8E(ptr %arg.tmp2)
  ret i64 %call3

if.end:                                           ; preds = %choose.when
  ret i64 0
}

define linkonce_odr ptr @_ZN6VectorI2u8E3getEm(ptr noalias %0, i64 %1) {
entry:
  %load.struct = load %_Z6VectorI2u8E, ptr %0, align 8
  %length = extractvalue %_Z6VectorI2u8E %load.struct, 0
  %ge = icmp uge i64 %1, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %load.struct1 = load %_Z6VectorI2u8E, ptr %0, align 8
  %data = extractvalue %_Z6VectorI2u8E %load.struct1, 1
  %ptr.add = getelementptr inbounds i8, ptr %data, i64 %1
  ret ptr %ptr.add
}

define linkonce_odr ptr @_ZN6VectorI2u8E2atEm(ptr noalias %0, i64 %1) {
entry:
  %load.struct = load %_Z6VectorI2u8E, ptr %0, align 8
  %length = extractvalue %_Z6VectorI2u8E %load.struct, 0
  %ge = icmp uge i64 %1, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %field.inplace = getelementptr inbounds nuw %_Z6VectorI2u8E, ptr %0, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  call void @_Z15scaly_panic_oobP10const_charmm(ptr @.str.29, i64 %1, i64 %field.val)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %load.struct1 = load %_Z6VectorI2u8E, ptr %0, align 8
  %data = extractvalue %_Z6VectorI2u8E %load.struct1, 1
  %ptr.add = getelementptr inbounds i8, ptr %data, i64 %1
  ret ptr %ptr.add
}

define linkonce_odr ptr @_ZN6VectorI2u8E7get_ptrEm(ptr noalias %0, i64 %1) {
entry:
  %load.struct = load %_Z6VectorI2u8E, ptr %0, align 8
  %length = extractvalue %_Z6VectorI2u8E %load.struct, 0
  %ge = icmp uge i64 %1, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %load.struct1 = load %_Z6VectorI2u8E, ptr %0, align 8
  %data = extractvalue %_Z6VectorI2u8E %load.struct1, 1
  %ptr.add = getelementptr inbounds i8, ptr %data, i64 %1
  ret ptr %ptr.add
}

define linkonce_odr void @_ZN6VectorI2u8E3putEm2u8(ptr noalias %0, i64 %1, i8 %2) {
entry:
  %load.struct = load %_Z6VectorI2u8E, ptr %0, align 8
  %length = extractvalue %_Z6VectorI2u8E %load.struct, 0
  %ge = icmp uge i64 %1, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %field.inplace = getelementptr inbounds nuw %_Z6VectorI2u8E, ptr %0, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  call void @_Z15scaly_panic_oobP10const_charmm(ptr @.str.30, i64 %1, i64 %field.val)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %load.struct1 = load %_Z6VectorI2u8E, ptr %0, align 8
  %data = extractvalue %_Z6VectorI2u8E %load.struct1, 1
  %ptr.add = getelementptr inbounds i8, ptr %data, i64 %1
  store i8 %2, ptr %ptr.add, align 1
  ret void
}

define linkonce_odr ptr @_ZN14VectorIteratorI2u8E4nextEv(ptr %0) {
entry:
  %load.struct = load %_Z14VectorIteratorI2u8E, ptr %0, align 8
  %vector = extractvalue %_Z14VectorIteratorI2u8E %load.struct, 0
  %eq = icmp eq ptr %vector, null
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %load.struct1 = load %_Z14VectorIteratorI2u8E, ptr %0, align 8
  %position = extractvalue %_Z14VectorIteratorI2u8E %load.struct1, 1
  %load.struct2 = load %_Z14VectorIteratorI2u8E, ptr %0, align 8
  %vector3 = extractvalue %_Z14VectorIteratorI2u8E %load.struct2, 0
  %deref = load %_Z6VectorI2u8E, ptr %vector3, align 8
  %length = extractvalue %_Z6VectorI2u8E %deref, 0
  %eq4 = icmp eq i64 %position, %length
  br i1 %eq4, label %if.then5, label %if.end6

if.then5:                                         ; preds = %if.end
  ret ptr null

if.end6:                                          ; preds = %if.end
  %load.struct7 = load %_Z14VectorIteratorI2u8E, ptr %0, align 8
  %position8 = extractvalue %_Z14VectorIteratorI2u8E %load.struct7, 1
  %add = add i64 %position8, 1
  %position9 = getelementptr inbounds nuw %_Z14VectorIteratorI2u8E, ptr %0, i32 0, i32 1
  store i64 %add, ptr %position9, align 8
  %field.inplace = getelementptr inbounds nuw %_Z14VectorIteratorI2u8E, ptr %0, i32 0, i32 0
  %deref.recv = load ptr, ptr %field.inplace, align 8
  %load.struct10 = load %_Z14VectorIteratorI2u8E, ptr %0, align 8
  %position11 = extractvalue %_Z14VectorIteratorI2u8E %load.struct10, 1
  %sub = sub i64 %position11, 1
  %call = call ptr @_ZN6VectorI2u8E7get_ptrEm(ptr %deref.recv, i64 %sub)
  ret ptr %call
}

define linkonce_odr void @_ZN14VectorIteratorI2u8EC1E6OptionIR6VectorI2u8EE(ptr %0, ptr %1) {
entry:
  %vector = getelementptr inbounds nuw %_Z14VectorIteratorI2u8E, ptr %0, i32 0, i32 0
  store ptr %1, ptr %vector, align 8
  %position = getelementptr inbounds nuw %_Z14VectorIteratorI2u8E, ptr %0, i32 0, i32 1
  store i64 0, ptr %position, align 8
  ret void
}

define linkonce_odr void @_ZN6VectorI2u8E12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z14VectorIteratorI2u8E) %0, ptr %1, ptr noalias %2) {
entry:
  %struct.init = alloca %_Z14VectorIteratorI2u8E, align 8
  call void @_ZN14VectorIteratorI2u8EC1E6OptionIR6VectorI2u8EE(ptr %struct.init, ptr %2)
  %sret.body = load %_Z14VectorIteratorI2u8E, ptr %struct.init, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %struct.init, i64 ptrtoint (ptr getelementptr (%_Z14VectorIteratorI2u8E, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN6VectorI2u8E8as_sliceEv(ptr noalias sret(%_Z5SliceI2u8E) %0, ptr noalias %1) {
entry:
  %load.struct = load %_Z6VectorI2u8E, ptr %1, align 8
  %length = extractvalue %_Z6VectorI2u8E %load.struct, 0
  %load.struct1 = load %_Z6VectorI2u8E, ptr %1, align 8
  %data = extractvalue %_Z6VectorI2u8E %load.struct1, 1
  %tuple = alloca %_Z5SliceI2u8E, align 8
  %tuple.field = getelementptr inbounds nuw %_Z5SliceI2u8E, ptr %tuple, i32 0, i32 0
  store i64 %length, ptr %tuple.field, align 1
  %tuple.field2 = getelementptr inbounds nuw %_Z5SliceI2u8E, ptr %tuple, i32 0, i32 1
  store ptr %data, ptr %tuple.field2, align 1
  %tuple.val = load %_Z5SliceI2u8E, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z5SliceI2u8E, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN6VectorI2u8EC1Ev(ptr noalias %0) {
entry:
  %length = getelementptr inbounds nuw %_Z6VectorI2u8E, ptr %0, i32 0, i32 0
  store i64 0, ptr %length, align 8
  %data = getelementptr inbounds nuw %_Z6VectorI2u8E, ptr %0, i32 0, i32 1
  store ptr null, ptr %data, align 8
  ret void
}

define linkonce_odr void @_ZN6VectorI2u8EC1Em(ptr %0, i64 %1) {
entry:
  %call = call ptr @_ZN4Page3getEPv(ptr %0)
  %length = getelementptr inbounds nuw %_Z6VectorI2u8E, ptr %0, i32 0, i32 0
  store i64 %1, ptr %length, align 8
  %gt = icmp ugt i64 %1, 0
  br i1 %gt, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %mul = mul i64 %1, ptrtoint (ptr getelementptr (i8, ptr null, i32 1) to i64)
  %call1 = call ptr @_ZN4Page8allocateEmm(ptr %call, i64 %mul, i64 1)
  %data = getelementptr inbounds nuw %_Z6VectorI2u8E, ptr %0, i32 0, i32 1
  store ptr %call1, ptr %data, align 8
  %field.inplace = getelementptr inbounds nuw %_Z6VectorI2u8E, ptr %0, i32 0, i32 1
  %deref.recv = load ptr, ptr %field.inplace, align 8
  %mul2 = mul i64 %1, ptrtoint (ptr getelementptr (i8, ptr null, i32 1) to i64)
  %call3 = call ptr @memset(ptr %deref.recv, i32 0, i64 %mul2)
  br label %if.end

if.else:                                          ; preds = %entry
  %data4 = getelementptr inbounds nuw %_Z6VectorI2u8E, ptr %0, i32 0, i32 1
  store ptr null, ptr %data4, align 8
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  %if.value = phi ptr [ %call3, %if.then ], [ null, %if.else ]
  ret void
}

define linkonce_odr void @_ZN6VectorI2u8EC1E6VectorI2u8E(ptr %0, ptr %1) {
entry:
  %call = call ptr @_ZN4Page3getEPv(ptr %0)
  %load.struct = load %_Z6VectorI2u8E, ptr %1, align 8
  %length = extractvalue %_Z6VectorI2u8E %load.struct, 0
  %length1 = getelementptr inbounds nuw %_Z6VectorI2u8E, ptr %0, i32 0, i32 0
  store i64 %length, ptr %length1, align 8
  %load.struct2 = load %_Z6VectorI2u8E, ptr %0, align 8
  %length3 = extractvalue %_Z6VectorI2u8E %load.struct2, 0
  %gt = icmp ugt i64 %length3, 0
  br i1 %gt, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %load.struct4 = load %_Z6VectorI2u8E, ptr %0, align 8
  %length5 = extractvalue %_Z6VectorI2u8E %load.struct4, 0
  %mul = mul i64 %length5, ptrtoint (ptr getelementptr (i8, ptr null, i32 1) to i64)
  %call6 = call ptr @_ZN4Page8allocateEmm(ptr %call, i64 %mul, i64 1)
  %data = getelementptr inbounds nuw %_Z6VectorI2u8E, ptr %0, i32 0, i32 1
  store ptr %call6, ptr %data, align 8
  %field.inplace = getelementptr inbounds nuw %_Z6VectorI2u8E, ptr %0, i32 0, i32 1
  %deref.recv = load ptr, ptr %field.inplace, align 8
  %field.inplace7 = getelementptr inbounds nuw %_Z6VectorI2u8E, ptr %1, i32 0, i32 1
  %deref.recv8 = load ptr, ptr %field.inplace7, align 8
  %load.struct9 = load %_Z6VectorI2u8E, ptr %0, align 8
  %length10 = extractvalue %_Z6VectorI2u8E %load.struct9, 0
  %mul11 = mul i64 %length10, ptrtoint (ptr getelementptr (i8, ptr null, i32 1) to i64)
  %call12 = call ptr @memcpy(ptr %deref.recv, ptr %deref.recv8, i64 %mul11)
  br label %if.end

if.else:                                          ; preds = %entry
  %data13 = getelementptr inbounds nuw %_Z6VectorI2u8E, ptr %0, i32 0, i32 1
  store ptr null, ptr %data13, align 8
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  %if.value = phi ptr [ %call12, %if.then ], [ null, %if.else ]
  ret void
}

define linkonce_odr ptr @_ZN5ArrayI2u8E10get_bufferEv(ptr %0) {
entry:
  %load.struct = load %_Z5ArrayI2u8E, ptr %0, align 8
  %buffer = extractvalue %_Z5ArrayI2u8E %load.struct, 2
  ret ptr %buffer
}

define linkonce_odr i64 @_ZN5ArrayI2u8E10get_lengthEv(ptr %0) {
entry:
  %load.struct = load %_Z5ArrayI2u8E, ptr %0, align 8
  %length = extractvalue %_Z5ArrayI2u8E %load.struct, 0
  ret i64 %length
}

define linkonce_odr i64 @_ZN5ArrayI2u8E12get_capacityEv(ptr %0) {
entry:
  %load.struct = load %_Z5ArrayI2u8E, ptr %0, align 8
  %capacity = extractvalue %_Z5ArrayI2u8E %load.struct, 1
  ret i64 %capacity
}

define linkonce_odr void @_ZN5ArrayI2u8E10reallocateEv(ptr %0) {
entry:
  %first_cap = alloca i64, align 8
  %call = call ptr @_ZN4Page3getEPv(ptr %0)
  %load.struct = load %_Z5ArrayI2u8E, ptr %0, align 8
  %buffer = extractvalue %_Z5ArrayI2u8E %load.struct, 2
  %eq = icmp eq ptr %buffer, null
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %udiv = udiv i64 32, ptrtoint (ptr getelementptr (i8, ptr null, i32 1) to i64)
  store i64 %udiv, ptr %first_cap, align 1
  %first_cap1 = load i64, ptr %first_cap, align 8
  %lt = icmp ult i64 %first_cap1, 1
  br i1 %lt, label %if.then2, label %if.end3

if.end:                                           ; preds = %entry
  %load.struct18 = load %_Z5ArrayI2u8E, ptr %0, align 8
  %capacity19 = extractvalue %_Z5ArrayI2u8E %load.struct18, 1
  %mul20 = mul i64 %capacity19, ptrtoint (ptr getelementptr (i8, ptr null, i32 1) to i64)
  %le21 = icmp ule i64 %mul20, 1024
  %load.struct22 = load %_Z5ArrayI2u8E, ptr %0, align 8
  %buffer23 = extractvalue %_Z5ArrayI2u8E %load.struct22, 2
  %load.struct24 = load %_Z5ArrayI2u8E, ptr %0, align 8
  %capacity25 = extractvalue %_Z5ArrayI2u8E %load.struct24, 1
  %load.struct26 = load %_Z5ArrayI2u8E, ptr %0, align 8
  %capacity27 = extractvalue %_Z5ArrayI2u8E %load.struct26, 1
  %mul28 = mul i64 %capacity27, 2
  store i64 %mul28, ptr %first_cap, align 1
  %new_capacity = load i64, ptr %first_cap, align 8
  %mul29 = mul i64 %new_capacity, ptrtoint (ptr getelementptr (i8, ptr null, i32 1) to i64)
  %le30 = icmp ule i64 %mul29, 1024
  br i1 %le30, label %if.then31, label %if.end32

if.then2:                                         ; preds = %if.then
  store i64 1, ptr %first_cap, align 1
  br label %if.end3

if.end3:                                          ; preds = %if.then2, %if.then
  %first_cap4 = load i64, ptr %first_cap, align 8
  %mul = mul i64 %first_cap4, ptrtoint (ptr getelementptr (i8, ptr null, i32 1) to i64)
  %le = icmp ule i64 %mul, 1024
  br i1 %le, label %if.then5, label %if.end6

if.then5:                                         ; preds = %if.end3
  %first_cap7 = load i64, ptr %first_cap, align 8
  %mul8 = mul i64 %first_cap7, ptrtoint (ptr getelementptr (i8, ptr null, i32 1) to i64)
  %call9 = call ptr @_ZN4Page8allocateEmm(ptr %call, i64 %mul8, i64 1)
  %buffer10 = getelementptr inbounds nuw %_Z5ArrayI2u8E, ptr %0, i32 0, i32 2
  store ptr %call9, ptr %buffer10, align 8
  %first_cap11 = load i64, ptr %first_cap, align 8
  %capacity = getelementptr inbounds nuw %_Z5ArrayI2u8E, ptr %0, i32 0, i32 1
  store i64 %first_cap11, ptr %capacity, align 8
  ret void

if.end6:                                          ; preds = %if.end3
  %first_cap12 = load i64, ptr %first_cap, align 8
  %mul13 = mul i64 %first_cap12, ptrtoint (ptr getelementptr (i8, ptr null, i32 1) to i64)
  %call14 = call ptr @_ZN4Page25allocate_exclusive_bufferEmm(ptr %call, i64 %mul13, i64 1)
  %buffer15 = getelementptr inbounds nuw %_Z5ArrayI2u8E, ptr %0, i32 0, i32 2
  store ptr %call14, ptr %buffer15, align 8
  %first_cap16 = load i64, ptr %first_cap, align 8
  %capacity17 = getelementptr inbounds nuw %_Z5ArrayI2u8E, ptr %0, i32 0, i32 1
  store i64 %first_cap16, ptr %capacity17, align 8
  ret void

if.then31:                                        ; preds = %if.end
  %new_capacity33 = load i64, ptr %first_cap, align 8
  %mul34 = mul i64 %new_capacity33, ptrtoint (ptr getelementptr (i8, ptr null, i32 1) to i64)
  %call35 = call ptr @_ZN4Page8allocateEmm(ptr %call, i64 %mul34, i64 1)
  %buffer36 = getelementptr inbounds nuw %_Z5ArrayI2u8E, ptr %0, i32 0, i32 2
  store ptr %call35, ptr %buffer36, align 8
  %new_capacity37 = load i64, ptr %first_cap, align 8
  %capacity38 = getelementptr inbounds nuw %_Z5ArrayI2u8E, ptr %0, i32 0, i32 1
  store i64 %new_capacity37, ptr %capacity38, align 8
  %field.inplace = getelementptr inbounds nuw %_Z5ArrayI2u8E, ptr %0, i32 0, i32 2
  %deref.recv = load ptr, ptr %field.inplace, align 8
  %mul39 = mul i64 %capacity25, ptrtoint (ptr getelementptr (i8, ptr null, i32 1) to i64)
  %call40 = call ptr @memcpy(ptr %deref.recv, ptr %buffer23, i64 %mul39)
  ret void

if.end32:                                         ; preds = %if.end
  %call41 = call ptr @_ZN4Page23allocate_exclusive_pageEv(ptr %call)
  %call42 = call i64 @_ZN4Page12get_capacityEm(ptr %call41, i64 1)
  call void @_ZN4Page25deallocate_exclusive_pageER4Page(ptr %call, ptr %call41)
  %new_capacity43 = load i64, ptr %first_cap, align 8
  %udiv44 = udiv i64 %call42, ptrtoint (ptr getelementptr (i8, ptr null, i32 1) to i64)
  %lt45 = icmp ult i64 %new_capacity43, %udiv44
  br i1 %lt45, label %if.then46, label %if.end47

if.then46:                                        ; preds = %if.end32
  %udiv48 = udiv i64 %call42, ptrtoint (ptr getelementptr (i8, ptr null, i32 1) to i64)
  store i64 %udiv48, ptr %first_cap, align 1
  br label %if.end47

if.end47:                                         ; preds = %if.then46, %if.end32
  %new_capacity49 = load i64, ptr %first_cap, align 8
  %mul50 = mul i64 %new_capacity49, ptrtoint (ptr getelementptr (i8, ptr null, i32 1) to i64)
  %call51 = call ptr @_ZN4Page25allocate_exclusive_bufferEmm(ptr %call, i64 %mul50, i64 1)
  %buffer52 = getelementptr inbounds nuw %_Z5ArrayI2u8E, ptr %0, i32 0, i32 2
  store ptr %call51, ptr %buffer52, align 8
  %new_capacity53 = load i64, ptr %first_cap, align 8
  %capacity54 = getelementptr inbounds nuw %_Z5ArrayI2u8E, ptr %0, i32 0, i32 1
  store i64 %new_capacity53, ptr %capacity54, align 8
  %field.inplace55 = getelementptr inbounds nuw %_Z5ArrayI2u8E, ptr %0, i32 0, i32 2
  %deref.recv56 = load ptr, ptr %field.inplace55, align 8
  %mul57 = mul i64 %capacity25, ptrtoint (ptr getelementptr (i8, ptr null, i32 1) to i64)
  %call58 = call ptr @memcpy(ptr %deref.recv56, ptr %buffer23, i64 %mul57)
  %eq59 = icmp eq i1 %le21, false
  br i1 %eq59, label %if.then60, label %if.end61

if.then60:                                        ; preds = %if.end47
  %call62 = call ptr @_ZN4Page3getEPv(ptr %buffer23)
  call void @_ZN4Page25deallocate_exclusive_pageER4Page(ptr %call, ptr %call62)
  br label %if.end61

if.end61:                                         ; preds = %if.then60, %if.end47
  ret void
}

define linkonce_odr void @_ZN5ArrayI2u8E3addE2u8(ptr %0, i8 %1) {
entry:
  %load.struct = load %_Z5ArrayI2u8E, ptr %0, align 8
  %buffer = extractvalue %_Z5ArrayI2u8E %load.struct, 2
  %eq = icmp eq ptr %buffer, null
  br i1 %eq, label %if.then, label %lor.rhs

if.then:                                          ; preds = %lor.rhs, %entry
  call void @_ZN5ArrayI2u8E10reallocateEv(ptr %0)
  br label %if.end

if.end:                                           ; preds = %if.then, %lor.rhs
  %load.struct4 = load %_Z5ArrayI2u8E, ptr %0, align 8
  %buffer5 = extractvalue %_Z5ArrayI2u8E %load.struct4, 2
  %load.struct6 = load %_Z5ArrayI2u8E, ptr %0, align 8
  %length7 = extractvalue %_Z5ArrayI2u8E %load.struct6, 0
  %ptr.add = getelementptr inbounds i8, ptr %buffer5, i64 %length7
  store i8 %1, ptr %ptr.add, align 1
  %load.struct8 = load %_Z5ArrayI2u8E, ptr %0, align 8
  %length9 = extractvalue %_Z5ArrayI2u8E %load.struct8, 0
  %add = add i64 %length9, 1
  %length10 = getelementptr inbounds nuw %_Z5ArrayI2u8E, ptr %0, i32 0, i32 0
  store i64 %add, ptr %length10, align 8
  ret void

lor.rhs:                                          ; preds = %entry
  %load.struct1 = load %_Z5ArrayI2u8E, ptr %0, align 8
  %length = extractvalue %_Z5ArrayI2u8E %load.struct1, 0
  %load.struct2 = load %_Z5ArrayI2u8E, ptr %0, align 8
  %capacity = extractvalue %_Z5ArrayI2u8E %load.struct2, 1
  %eq3 = icmp eq i64 %length, %capacity
  br i1 %eq3, label %if.then, label %if.end
}

define linkonce_odr void @_ZN5ArrayI2u8E7add_runEmP2u8(ptr %0, i64 %1, ptr %2) {
entry:
  %load.struct = load %_Z5ArrayI2u8E, ptr %0, align 8
  %length = extractvalue %_Z5ArrayI2u8E %load.struct, 0
  %add = add i64 %length, %1
  %new_length = alloca i64, align 8
  store i64 %add, ptr %new_length, align 1
  %new_length1 = load i64, ptr %new_length, align 8
  %load.struct2 = load %_Z5ArrayI2u8E, ptr %0, align 8
  %length3 = extractvalue %_Z5ArrayI2u8E %load.struct2, 0
  %lt = icmp ult i64 %new_length1, %length3
  br i1 %lt, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %field.inplace = getelementptr inbounds nuw %_Z5ArrayI2u8E, ptr %0, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  call void @_Z16scaly_panic_sizeP10const_charmm(ptr @.str.31, i64 %1, i64 %field.val)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %load.struct6 = load %_Z5ArrayI2u8E, ptr %0, align 8
  %buffer = extractvalue %_Z5ArrayI2u8E %load.struct6, 2
  %eq = icmp eq ptr %buffer, null
  br i1 %eq, label %if.then4, label %lor.rhs

if.then4:                                         ; preds = %lor.rhs, %if.end
  %new_length9 = load i64, ptr %new_length, align 8
  call void @_ZN5ArrayI2u8E7grow_toEm(ptr %0, i64 %new_length9)
  br label %if.end5

if.end5:                                          ; preds = %if.then4, %lor.rhs
  %gt10 = icmp ugt i64 %1, 0
  br i1 %gt10, label %if.then11, label %if.end12

lor.rhs:                                          ; preds = %if.end
  %new_length7 = load i64, ptr %new_length, align 8
  %load.struct8 = load %_Z5ArrayI2u8E, ptr %0, align 8
  %capacity = extractvalue %_Z5ArrayI2u8E %load.struct8, 1
  %gt = icmp ugt i64 %new_length7, %capacity
  br i1 %gt, label %if.then4, label %if.end5

if.then11:                                        ; preds = %if.end5
  %load.struct13 = load %_Z5ArrayI2u8E, ptr %0, align 8
  %buffer14 = extractvalue %_Z5ArrayI2u8E %load.struct13, 2
  %load.struct15 = load %_Z5ArrayI2u8E, ptr %0, align 8
  %length16 = extractvalue %_Z5ArrayI2u8E %load.struct15, 0
  %ptr.add = getelementptr inbounds i8, ptr %buffer14, i64 %length16
  %mul = mul i64 %1, ptrtoint (ptr getelementptr (i8, ptr null, i32 1) to i64)
  %call = call ptr @memcpy(ptr %ptr.add, ptr %2, i64 %mul)
  br label %if.end12

if.end12:                                         ; preds = %if.then11, %if.end5
  %load.struct17 = load %_Z5ArrayI2u8E, ptr %0, align 8
  %length18 = extractvalue %_Z5ArrayI2u8E %load.struct17, 0
  %add19 = add i64 %length18, %1
  %length20 = getelementptr inbounds nuw %_Z5ArrayI2u8E, ptr %0, i32 0, i32 0
  store i64 %add19, ptr %length20, align 8
  ret void
}

define linkonce_odr void @_ZN5ArrayI2u8E3addE6VectorI2u8E(ptr %0, ptr %1) {
entry:
  %field.inplace = getelementptr inbounds nuw %_Z6VectorI2u8E, ptr %1, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  %field.inplace1 = getelementptr inbounds nuw %_Z6VectorI2u8E, ptr %1, i32 0, i32 1
  %deref.recv = load ptr, ptr %field.inplace1, align 8
  call void @_ZN5ArrayI2u8E7add_runEmP2u8(ptr %0, i64 %field.val, ptr %deref.recv)
  ret void
}

define linkonce_odr void @_ZN5ArrayI2u8E3addE5SliceI2u8E(ptr %0, ptr %1) {
entry:
  %field.inplace = getelementptr inbounds nuw %_Z5SliceI2u8E, ptr %1, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  %field.inplace1 = getelementptr inbounds nuw %_Z5SliceI2u8E, ptr %1, i32 0, i32 1
  %deref.recv = load ptr, ptr %field.inplace1, align 8
  call void @_ZN5ArrayI2u8E7add_runEmP2u8(ptr %0, i64 %field.val, ptr %deref.recv)
  ret void
}

define linkonce_odr void @_ZN5ArrayI2u8E7grow_toEm(ptr %0, i64 %1) {
entry:
  call void @_ZN5ArrayI2u8E10reallocateEv(ptr %0)
  %load.struct = load %_Z5ArrayI2u8E, ptr %0, align 8
  %capacity = extractvalue %_Z5ArrayI2u8E %load.struct, 1
  %gt = icmp ugt i64 %1, %capacity
  br i1 %gt, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %call = call ptr @_ZN4Page3getEPv(ptr %0)
  %load.struct1 = load %_Z5ArrayI2u8E, ptr %0, align 8
  %buffer = extractvalue %_Z5ArrayI2u8E %load.struct1, 2
  %load.struct2 = load %_Z5ArrayI2u8E, ptr %0, align 8
  %capacity3 = extractvalue %_Z5ArrayI2u8E %load.struct2, 1
  %mul = mul i64 %capacity3, ptrtoint (ptr getelementptr (i8, ptr null, i32 1) to i64)
  %le = icmp ule i64 %mul, 1024
  %mul4 = mul i64 %1, ptrtoint (ptr getelementptr (i8, ptr null, i32 1) to i64)
  %le5 = icmp ule i64 %mul4, 1024
  br i1 %le5, label %if.then6, label %if.else

if.end:                                           ; preds = %if.end24, %entry
  ret void

if.then6:                                         ; preds = %if.then
  %mul8 = mul i64 %1, ptrtoint (ptr getelementptr (i8, ptr null, i32 1) to i64)
  %call9 = call ptr @_ZN4Page8allocateEmm(ptr %call, i64 %mul8, i64 1)
  %buffer10 = getelementptr inbounds nuw %_Z5ArrayI2u8E, ptr %0, i32 0, i32 2
  store ptr %call9, ptr %buffer10, align 8
  br label %if.end7

if.else:                                          ; preds = %if.then
  %mul11 = mul i64 %1, ptrtoint (ptr getelementptr (i8, ptr null, i32 1) to i64)
  %call12 = call ptr @_ZN4Page25allocate_exclusive_bufferEmm(ptr %call, i64 %mul11, i64 1)
  %buffer13 = getelementptr inbounds nuw %_Z5ArrayI2u8E, ptr %0, i32 0, i32 2
  store ptr %call12, ptr %buffer13, align 8
  br label %if.end7

if.end7:                                          ; preds = %if.else, %if.then6
  %if.value = phi ptr [ %call9, %if.then6 ], [ %call12, %if.else ]
  %capacity14 = getelementptr inbounds nuw %_Z5ArrayI2u8E, ptr %0, i32 0, i32 1
  store i64 %1, ptr %capacity14, align 8
  %load.struct15 = load %_Z5ArrayI2u8E, ptr %0, align 8
  %length = extractvalue %_Z5ArrayI2u8E %load.struct15, 0
  %gt16 = icmp ugt i64 %length, 0
  br i1 %gt16, label %if.then17, label %if.end18

if.then17:                                        ; preds = %if.end7
  %field.inplace = getelementptr inbounds nuw %_Z5ArrayI2u8E, ptr %0, i32 0, i32 2
  %deref.recv = load ptr, ptr %field.inplace, align 8
  %load.struct19 = load %_Z5ArrayI2u8E, ptr %0, align 8
  %length20 = extractvalue %_Z5ArrayI2u8E %load.struct19, 0
  %mul21 = mul i64 %length20, ptrtoint (ptr getelementptr (i8, ptr null, i32 1) to i64)
  %call22 = call ptr @memcpy(ptr %deref.recv, ptr %buffer, i64 %mul21)
  br label %if.end18

if.end18:                                         ; preds = %if.then17, %if.end7
  %eq = icmp eq i1 %le, false
  br i1 %eq, label %if.then23, label %if.end24

if.then23:                                        ; preds = %if.end18
  %call25 = call ptr @_ZN4Page3getEPv(ptr %buffer)
  call void @_ZN4Page25deallocate_exclusive_pageER4Page(ptr %call, ptr %call25)
  br label %if.end24

if.end24:                                         ; preds = %if.then23, %if.end18
  br label %if.end
}

define linkonce_odr void @_ZN5ArrayI2u8E6extendEm(ptr noalias sret(%_Z5SliceI2u8E) %0, ptr %1, i64 %2) {
entry:
  %tuple = alloca %_Z5SliceI2u8E, align 8
  %load.struct = load %_Z5ArrayI2u8E, ptr %1, align 8
  %length = extractvalue %_Z5ArrayI2u8E %load.struct, 0
  %add = add i64 %length, %2
  %lt = icmp ult i64 %add, %length
  br i1 %lt, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  call void @_Z16scaly_panic_sizeP10const_charmm(ptr @.str.32, i64 %2, i64 %length)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %load.struct3 = load %_Z5ArrayI2u8E, ptr %1, align 8
  %buffer = extractvalue %_Z5ArrayI2u8E %load.struct3, 2
  %eq = icmp eq ptr %buffer, null
  br i1 %eq, label %if.then1, label %lor.rhs

if.then1:                                         ; preds = %lor.rhs, %if.end
  call void @_ZN5ArrayI2u8E7grow_toEm(ptr %1, i64 %add)
  br label %if.end2

if.end2:                                          ; preds = %if.then1, %lor.rhs
  %length5 = getelementptr inbounds nuw %_Z5ArrayI2u8E, ptr %1, i32 0, i32 0
  store i64 %add, ptr %length5, align 8
  %load.struct6 = load %_Z5ArrayI2u8E, ptr %1, align 8
  %buffer7 = extractvalue %_Z5ArrayI2u8E %load.struct6, 2
  %ptr.add = getelementptr inbounds i8, ptr %buffer7, i64 %length
  %tuple.field = getelementptr inbounds nuw %_Z5SliceI2u8E, ptr %tuple, i32 0, i32 0
  store i64 %2, ptr %tuple.field, align 1
  %tuple.field8 = getelementptr inbounds nuw %_Z5SliceI2u8E, ptr %tuple, i32 0, i32 1
  store ptr %ptr.add, ptr %tuple.field8, align 1
  %tuple.val = load %_Z5SliceI2u8E, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z5SliceI2u8E, ptr null, i32 1) to i64), i1 false)
  ret void

lor.rhs:                                          ; preds = %if.end
  %load.struct4 = load %_Z5ArrayI2u8E, ptr %1, align 8
  %capacity = extractvalue %_Z5ArrayI2u8E %load.struct4, 1
  %gt = icmp ugt i64 %add, %capacity
  br i1 %gt, label %if.then1, label %if.end2
}

declare ptr @_ZN4Page25allocate_exclusive_bufferEmm(ptr, i64, i64)

declare void @_ZN4Page25deallocate_exclusive_pageER4Page(ptr, ptr)

define linkonce_odr ptr @_ZN5ArrayI2u8E3getEm(ptr %0, i64 %1) {
entry:
  %load.struct = load %_Z5ArrayI2u8E, ptr %0, align 8
  %length = extractvalue %_Z5ArrayI2u8E %load.struct, 0
  %ge = icmp uge i64 %1, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %load.struct1 = load %_Z5ArrayI2u8E, ptr %0, align 8
  %buffer = extractvalue %_Z5ArrayI2u8E %load.struct1, 2
  %ptr.add = getelementptr inbounds i8, ptr %buffer, i64 %1
  ret ptr %ptr.add
}

define linkonce_odr ptr @_ZN5ArrayI2u8E2atEm(ptr %0, i64 %1) {
entry:
  %load.struct = load %_Z5ArrayI2u8E, ptr %0, align 8
  %length = extractvalue %_Z5ArrayI2u8E %load.struct, 0
  %ge = icmp uge i64 %1, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %field.inplace = getelementptr inbounds nuw %_Z5ArrayI2u8E, ptr %0, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  call void @_Z15scaly_panic_oobP10const_charmm(ptr @.str.33, i64 %1, i64 %field.val)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %load.struct1 = load %_Z5ArrayI2u8E, ptr %0, align 8
  %buffer = extractvalue %_Z5ArrayI2u8E %load.struct1, 2
  %ptr.add = getelementptr inbounds i8, ptr %buffer, i64 %1
  ret ptr %ptr.add
}

define linkonce_odr void @_ZN5ArrayI2u8E5clearEv(ptr %0) {
entry:
  %length = getelementptr inbounds nuw %_Z5ArrayI2u8E, ptr %0, i32 0, i32 0
  store i64 0, ptr %length, align 8
  ret void
}

define linkonce_odr void @_ZN5ArrayI2u8E3putEm2u8(ptr %0, i64 %1, i8 %2) {
entry:
  %load.struct = load %_Z5ArrayI2u8E, ptr %0, align 8
  %length = extractvalue %_Z5ArrayI2u8E %load.struct, 0
  %ge = icmp uge i64 %1, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %field.inplace = getelementptr inbounds nuw %_Z5ArrayI2u8E, ptr %0, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  call void @_Z15scaly_panic_oobP10const_charmm(ptr @.str.34, i64 %1, i64 %field.val)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %load.struct1 = load %_Z5ArrayI2u8E, ptr %0, align 8
  %buffer = extractvalue %_Z5ArrayI2u8E %load.struct1, 2
  %ptr.add = getelementptr inbounds i8, ptr %buffer, i64 %1
  store i8 %2, ptr %ptr.add, align 1
  ret void
}

define linkonce_odr ptr @_ZN13ArrayIteratorI2u8E4nextEv(ptr %0) {
entry:
  %load.struct = load %_Z13ArrayIteratorI2u8E, ptr %0, align 8
  %array = extractvalue %_Z13ArrayIteratorI2u8E %load.struct, 0
  %eq = icmp eq ptr %array, null
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %load.struct1 = load %_Z13ArrayIteratorI2u8E, ptr %0, align 8
  %position = extractvalue %_Z13ArrayIteratorI2u8E %load.struct1, 1
  %load.struct2 = load %_Z13ArrayIteratorI2u8E, ptr %0, align 8
  %array3 = extractvalue %_Z13ArrayIteratorI2u8E %load.struct2, 0
  %deref = load %_Z5ArrayI2u8E, ptr %array3, align 8
  %length = extractvalue %_Z5ArrayI2u8E %deref, 0
  %eq4 = icmp eq i64 %position, %length
  br i1 %eq4, label %if.then5, label %if.end6

if.then5:                                         ; preds = %if.end
  ret ptr null

if.end6:                                          ; preds = %if.end
  %load.struct7 = load %_Z13ArrayIteratorI2u8E, ptr %0, align 8
  %position8 = extractvalue %_Z13ArrayIteratorI2u8E %load.struct7, 1
  %add = add i64 %position8, 1
  %position9 = getelementptr inbounds nuw %_Z13ArrayIteratorI2u8E, ptr %0, i32 0, i32 1
  store i64 %add, ptr %position9, align 8
  %field.inplace = getelementptr inbounds nuw %_Z13ArrayIteratorI2u8E, ptr %0, i32 0, i32 0
  %deref.recv = load ptr, ptr %field.inplace, align 8
  %call = call ptr @_ZN5ArrayI2u8E10get_bufferEv(ptr %deref.recv)
  %load.struct10 = load %_Z13ArrayIteratorI2u8E, ptr %0, align 8
  %position11 = extractvalue %_Z13ArrayIteratorI2u8E %load.struct10, 1
  %sub = sub i64 %position11, 1
  %ptr.add = getelementptr inbounds i8, ptr %call, i64 %sub
  ret ptr %ptr.add
}

define linkonce_odr void @_ZN13ArrayIteratorI2u8EC1E6OptionIR5ArrayI2u8EE(ptr %0, ptr %1) {
entry:
  %array = getelementptr inbounds nuw %_Z13ArrayIteratorI2u8E, ptr %0, i32 0, i32 0
  store ptr %1, ptr %array, align 8
  %position = getelementptr inbounds nuw %_Z13ArrayIteratorI2u8E, ptr %0, i32 0, i32 1
  store i64 0, ptr %position, align 8
  ret void
}

define linkonce_odr void @_ZN5ArrayI2u8E12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z13ArrayIteratorI2u8E) %0, ptr %1, ptr %2) {
entry:
  %struct.init = alloca %_Z13ArrayIteratorI2u8E, align 8
  call void @_ZN13ArrayIteratorI2u8EC1E6OptionIR5ArrayI2u8EE(ptr %struct.init, ptr %2)
  %sret.body = load %_Z13ArrayIteratorI2u8E, ptr %struct.init, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %struct.init, i64 ptrtoint (ptr getelementptr (%_Z13ArrayIteratorI2u8E, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN5ArrayI2u8E8as_sliceEv(ptr noalias sret(%_Z5SliceI2u8E) %0, ptr %1) {
entry:
  %load.struct = load %_Z5ArrayI2u8E, ptr %1, align 8
  %length = extractvalue %_Z5ArrayI2u8E %load.struct, 0
  %call = call ptr @_ZN5ArrayI2u8E10get_bufferEv(ptr %1)
  %tuple = alloca %_Z5SliceI2u8E, align 8
  %tuple.field = getelementptr inbounds nuw %_Z5SliceI2u8E, ptr %tuple, i32 0, i32 0
  store i64 %length, ptr %tuple.field, align 1
  %tuple.field1 = getelementptr inbounds nuw %_Z5SliceI2u8E, ptr %tuple, i32 0, i32 1
  store ptr %call, ptr %tuple.field1, align 1
  %tuple.val = load %_Z5SliceI2u8E, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z5SliceI2u8E, ptr null, i32 1) to i64), i1 false)
  ret void
}

declare ptr @_ZN4Page23allocate_exclusive_pageEv(ptr)

declare i64 @_ZN4Page12get_capacityEm(ptr, i64)

define linkonce_odr void @_ZN5ArrayI2u8EC1Ev(ptr %0) {
entry:
  %length = getelementptr inbounds nuw %_Z5ArrayI2u8E, ptr %0, i32 0, i32 0
  store i64 0, ptr %length, align 8
  %capacity = getelementptr inbounds nuw %_Z5ArrayI2u8E, ptr %0, i32 0, i32 1
  store i64 0, ptr %capacity, align 8
  %buffer = getelementptr inbounds nuw %_Z5ArrayI2u8E, ptr %0, i32 0, i32 2
  store ptr null, ptr %buffer, align 8
  ret void
}

define linkonce_odr void @_ZN5ArrayI2u8EC1Em(ptr %0, i64 %1) {
entry:
  %length = getelementptr inbounds nuw %_Z5ArrayI2u8E, ptr %0, i32 0, i32 0
  store i64 0, ptr %length, align 8
  %capacity = getelementptr inbounds nuw %_Z5ArrayI2u8E, ptr %0, i32 0, i32 1
  store i64 0, ptr %capacity, align 8
  %buffer = getelementptr inbounds nuw %_Z5ArrayI2u8E, ptr %0, i32 0, i32 2
  store ptr null, ptr %buffer, align 8
  %gt = icmp ugt i64 %1, 0
  br i1 %gt, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %call = call ptr @_ZN4Page3getEPv(ptr %0)
  %capacity1 = getelementptr inbounds nuw %_Z5ArrayI2u8E, ptr %0, i32 0, i32 1
  store i64 %1, ptr %capacity1, align 8
  %mul = mul i64 %1, ptrtoint (ptr getelementptr (i8, ptr null, i32 1) to i64)
  %le = icmp ule i64 %mul, 1024
  br i1 %le, label %if.then2, label %if.else

if.end:                                           ; preds = %if.end3, %entry
  ret void

if.then2:                                         ; preds = %if.then
  %mul4 = mul i64 %1, ptrtoint (ptr getelementptr (i8, ptr null, i32 1) to i64)
  %call5 = call ptr @_ZN4Page8allocateEmm(ptr %call, i64 %mul4, i64 1)
  %buffer6 = getelementptr inbounds nuw %_Z5ArrayI2u8E, ptr %0, i32 0, i32 2
  store ptr %call5, ptr %buffer6, align 8
  br label %if.end3

if.else:                                          ; preds = %if.then
  %mul7 = mul i64 %1, ptrtoint (ptr getelementptr (i8, ptr null, i32 1) to i64)
  %call8 = call ptr @_ZN4Page25allocate_exclusive_bufferEmm(ptr %call, i64 %mul7, i64 1)
  %buffer9 = getelementptr inbounds nuw %_Z5ArrayI2u8E, ptr %0, i32 0, i32 2
  store ptr %call8, ptr %buffer9, align 8
  br label %if.end3

if.end3:                                          ; preds = %if.else, %if.then2
  %if.value = phi ptr [ %call5, %if.then2 ], [ %call8, %if.else ]
  br label %if.end
}

define linkonce_odr void @_ZN6VectorI2u8EC1E5ArrayI2u8E(ptr %0, ptr %1) {
entry:
  %call = call ptr @_ZN4Page3getEPv(ptr %0)
  %load.struct = load %_Z5ArrayI2u8E, ptr %1, align 8
  %length = extractvalue %_Z5ArrayI2u8E %load.struct, 0
  %length1 = getelementptr inbounds nuw %_Z6VectorI2u8E, ptr %0, i32 0, i32 0
  store i64 %length, ptr %length1, align 8
  %load.struct2 = load %_Z6VectorI2u8E, ptr %0, align 8
  %length3 = extractvalue %_Z6VectorI2u8E %load.struct2, 0
  %gt = icmp ugt i64 %length3, 0
  br i1 %gt, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %load.struct4 = load %_Z6VectorI2u8E, ptr %0, align 8
  %length5 = extractvalue %_Z6VectorI2u8E %load.struct4, 0
  %mul = mul i64 %length5, ptrtoint (ptr getelementptr (i8, ptr null, i32 1) to i64)
  %call6 = call ptr @_ZN4Page8allocateEmm(ptr %call, i64 %mul, i64 1)
  %data = getelementptr inbounds nuw %_Z6VectorI2u8E, ptr %0, i32 0, i32 1
  store ptr %call6, ptr %data, align 8
  %field.inplace = getelementptr inbounds nuw %_Z6VectorI2u8E, ptr %0, i32 0, i32 1
  %deref.recv = load ptr, ptr %field.inplace, align 8
  %call7 = call ptr @_ZN5ArrayI2u8E10get_bufferEv(ptr %1)
  %load.struct8 = load %_Z6VectorI2u8E, ptr %0, align 8
  %length9 = extractvalue %_Z6VectorI2u8E %load.struct8, 0
  %mul10 = mul i64 %length9, ptrtoint (ptr getelementptr (i8, ptr null, i32 1) to i64)
  %call11 = call ptr @memcpy(ptr %deref.recv, ptr %call7, i64 %mul10)
  br label %if.end

if.else:                                          ; preds = %entry
  %data12 = getelementptr inbounds nuw %_Z6VectorI2u8E, ptr %0, i32 0, i32 1
  store ptr null, ptr %data12, align 8
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  %if.value = phi ptr [ %call11, %if.then ], [ null, %if.else ]
  ret void
}

define linkonce_odr ptr @_ZN4ListI2u8E8get_headEv(ptr %0) {
entry:
  %load.struct = load %_Z4ListI2u8E, ptr %0, align 8
  %head = extractvalue %_Z4ListI2u8E %load.struct, 0
  %eq = icmp eq ptr %head, null
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %addr.gep = getelementptr inbounds nuw %_Z4ListI2u8E, ptr %0, i32 0, i32 0
  %addr.hop = load ptr, ptr %addr.gep, align 8
  %addr.gep1 = getelementptr inbounds nuw %_Z4NodeI2u8E, ptr %addr.hop, i32 0, i32 0
  ret ptr %addr.gep1
}

define linkonce_odr ptr @_ZN12ListIteratorI2u8E4nextEv(ptr %0) {
entry:
  %old_current = alloca ptr, align 8
  %load.struct = load %_Z12ListIteratorI2u8E, ptr %0, align 8
  %current = extractvalue %_Z12ListIteratorI2u8E %load.struct, 0
  %ne = icmp ne ptr %current, null
  br i1 %ne, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %load.struct1 = load %_Z12ListIteratorI2u8E, ptr %0, align 8
  %current2 = extractvalue %_Z12ListIteratorI2u8E %load.struct1, 0
  store ptr %current2, ptr %old_current, align 1
  %load.struct3 = load %_Z12ListIteratorI2u8E, ptr %0, align 8
  %current4 = extractvalue %_Z12ListIteratorI2u8E %load.struct3, 0
  %deref = load %_Z4NodeI2u8E, ptr %current4, align 8
  %next = extractvalue %_Z4NodeI2u8E %deref, 1
  %current5 = getelementptr inbounds nuw %_Z12ListIteratorI2u8E, ptr %0, i32 0, i32 0
  store ptr %next, ptr %current5, align 8
  %old_current6 = load ptr, ptr %old_current, align 8
  %addr.gep = getelementptr inbounds nuw %_Z4NodeI2u8E, ptr %old_current6, i32 0, i32 0
  ret ptr %addr.gep

if.else:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; No predecessors!
  ret ptr null
}

define linkonce_odr i64 @_ZN4ListI2u8E5countEv(ptr %0) {
entry:
  %sret.result = alloca %_Z12ListIteratorI2u8E, align 8
  call void @_ZN4ListI2u8E12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z12ListIteratorI2u8E) %sret.result, ptr null, ptr %0)
  %list_iterator = alloca ptr, align 8
  store ptr %sret.result, ptr %list_iterator, align 1
  %i = alloca i64, align 8
  store i64 0, ptr %i, align 1
  br label %while.cond

while.cond:                                       ; preds = %while.body, %entry
  %list_iterator1 = load ptr, ptr %list_iterator, align 8
  %call = call ptr @_ZN12ListIteratorI2u8E4nextEv(ptr %list_iterator1)
  %ne = icmp ne ptr %call, null
  br i1 %ne, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %i2 = load i64, ptr %i, align 8
  %add = add i64 %i2, 1
  store i64 %add, ptr %i, align 1
  br label %while.cond

while.exit:                                       ; preds = %while.cond
  %i3 = load i64, ptr %i, align 8
  ret i64 %i3
}

define linkonce_odr void @_ZN4ListI2u8E12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z12ListIteratorI2u8E) %0, ptr %1, ptr %2) {
entry:
  %load.struct = load %_Z4ListI2u8E, ptr %2, align 8
  %head = extractvalue %_Z4ListI2u8E %load.struct, 0
  %tuple = alloca %_Z12ListIteratorI2u8E, align 8
  %tuple.field = getelementptr inbounds nuw %_Z12ListIteratorI2u8E, ptr %tuple, i32 0, i32 0
  store ptr %head, ptr %tuple.field, align 1
  %tuple.val = load %_Z12ListIteratorI2u8E, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z12ListIteratorI2u8E, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN4ListI2u8E4linkEP4NodeI2u8E(ptr %0, ptr %1) {
entry:
  %load.struct = load %_Z4ListI2u8E, ptr %0, align 8
  %tail = extractvalue %_Z4ListI2u8E %load.struct, 1
  %eq = icmp eq ptr %tail, null
  br i1 %eq, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %head = getelementptr inbounds nuw %_Z4ListI2u8E, ptr %0, i32 0, i32 0
  store ptr %1, ptr %head, align 8
  br label %if.end

if.else:                                          ; preds = %entry
  %tail1 = getelementptr inbounds nuw %_Z4ListI2u8E, ptr %0, i32 0, i32 1
  %field.deref = load ptr, ptr %tail1, align 8
  %next = getelementptr inbounds nuw %_Z4NodeI2u8E, ptr %field.deref, i32 0, i32 1
  store ptr %1, ptr %next, align 8
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  %if.value = phi ptr [ %1, %if.then ], [ %1, %if.else ]
  %tail2 = getelementptr inbounds nuw %_Z4ListI2u8E, ptr %0, i32 0, i32 1
  store ptr %1, ptr %tail2, align 8
  ret void
}

define linkonce_odr void @_ZN4ListI2u8E3addE2u8(ptr %0, i8 %1) {
entry:
  %own_page = call ptr @_Z3getPv(ptr %0)
  %tuple.region = call ptr @_ZN4Page8allocateEmm(ptr %own_page, i64 ptrtoint (ptr getelementptr (%_Z4NodeI2u8E, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr ({ i1, %_Z4NodeI2u8E }, ptr null, i64 0, i32 1) to i64))
  %tuple.field = getelementptr inbounds nuw %_Z4NodeI2u8E, ptr %tuple.region, i32 0, i32 0
  store i8 %1, ptr %tuple.field, align 1
  %tuple.field1 = getelementptr inbounds nuw %_Z4NodeI2u8E, ptr %tuple.region, i32 0, i32 1
  store ptr null, ptr %tuple.field1, align 1
  call void @_ZN4ListI2u8E4linkEP4NodeI2u8E(ptr %0, ptr %tuple.region)
  ret void
}

define linkonce_odr void @_ZN4ListI2u8E6add_onER4Page2u8(ptr %0, ptr %1, i8 %2) {
entry:
  %tuple.region = call ptr @_ZN4Page8allocateEmm(ptr %1, i64 ptrtoint (ptr getelementptr (%_Z4NodeI2u8E, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr ({ i1, %_Z4NodeI2u8E }, ptr null, i64 0, i32 1) to i64))
  %tuple.field = getelementptr inbounds nuw %_Z4NodeI2u8E, ptr %tuple.region, i32 0, i32 0
  store i8 %2, ptr %tuple.field, align 1
  %tuple.field1 = getelementptr inbounds nuw %_Z4NodeI2u8E, ptr %tuple.region, i32 0, i32 1
  store ptr null, ptr %tuple.field1, align 1
  call void @_ZN4ListI2u8E4linkEP4NodeI2u8E(ptr %0, ptr %tuple.region)
  ret void
}

define linkonce_odr void @_ZN6VectorI2u8EC1E4ListI2u8E(ptr %0, ptr %1) {
entry:
  %i = alloca i64, align 8
  %list_iterator = alloca ptr, align 8
  %sret.result = alloca %_Z12ListIteratorI2u8E, align 8
  %call = call ptr @_ZN4Page3getEPv(ptr %0)
  %call1 = call i64 @_ZN4ListI2u8E5countEv(ptr %1)
  %length = getelementptr inbounds nuw %_Z6VectorI2u8E, ptr %0, i32 0, i32 0
  store i64 %call1, ptr %length, align 8
  %load.struct = load %_Z6VectorI2u8E, ptr %0, align 8
  %length2 = extractvalue %_Z6VectorI2u8E %load.struct, 0
  %gt = icmp ugt i64 %length2, 0
  br i1 %gt, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %load.struct3 = load %_Z6VectorI2u8E, ptr %0, align 8
  %length4 = extractvalue %_Z6VectorI2u8E %load.struct3, 0
  %mul = mul i64 %length4, ptrtoint (ptr getelementptr (i8, ptr null, i32 1) to i64)
  %call5 = call ptr @_ZN4Page8allocateEmm(ptr %call, i64 %mul, i64 1)
  %data = getelementptr inbounds nuw %_Z6VectorI2u8E, ptr %0, i32 0, i32 1
  store ptr %call5, ptr %data, align 8
  call void @_ZN4ListI2u8E12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z12ListIteratorI2u8E) %sret.result, ptr null, ptr %1)
  store ptr %sret.result, ptr %list_iterator, align 1
  store i64 0, ptr %i, align 1
  br label %while.cond

if.else:                                          ; preds = %entry
  %data12 = getelementptr inbounds nuw %_Z6VectorI2u8E, ptr %0, i32 0, i32 1
  store ptr null, ptr %data12, align 8
  br label %if.end

if.end:                                           ; preds = %if.else, %while.exit
  ret void

while.cond:                                       ; preds = %while.body, %if.then
  %list_iterator6 = load ptr, ptr %list_iterator, align 8
  %call7 = call ptr @_ZN12ListIteratorI2u8E4nextEv(ptr %list_iterator6)
  %while.tobool = icmp ne ptr %call7, null
  br i1 %while.tobool, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %deref = load i8, ptr %call7, align 1
  %load.struct8 = load %_Z6VectorI2u8E, ptr %0, align 8
  %data9 = extractvalue %_Z6VectorI2u8E %load.struct8, 1
  %i10 = load i64, ptr %i, align 8
  %ptr.add = getelementptr inbounds i8, ptr %data9, i64 %i10
  store i8 %deref, ptr %ptr.add, align 1
  %i11 = load i64, ptr %i, align 8
  %add = add i64 %i11, 1
  store i64 %add, ptr %i, align 1
  br label %while.cond

while.exit:                                       ; preds = %while.cond
  br label %if.end
}

define linkonce_odr double @_ZN9JsonValueB4json6as_f64Ev(ptr %0) {
entry:
  %arg.tmp = alloca %_Z5SliceI2u8E, align 8
  %i = alloca i64, align 8
  %buf = alloca ptr, align 8
  %frame = alloca { ptr, ptr }, align 8
  store ptr null, ptr %frame, align 8
  %frame.parent = getelementptr inbounds nuw { ptr, ptr }, ptr %frame, i32 0, i32 1
  store ptr null, ptr %frame.parent, align 8
  %tag.ptr = getelementptr inbounds nuw %_Z9JsonValue, ptr %0, i32 0, i32 0
  %tag = load i8, ptr %tag.ptr, align 1
  switch i8 %tag, label %choose.else [
    i8 2, label %choose.when
  ]

choose.end:                                       ; No predecessors!
  call void @_Z19scaly_release_frameP5Frame(ptr %frame)
  ret double 0.000000e+00

choose.else:                                      ; preds = %entry
  call void @_Z19scaly_release_frameP5Frame(ptr %frame)
  ret double 0.000000e+00

choose.when:                                      ; preds = %entry
  %"variant.c_data().ptr" = getelementptr inbounds nuw %_Z9JsonValue, ptr %0, i32 0, i32 1
  %variant.val = load %_Z10JsonNumber, ptr %"variant.c_data().ptr", align 8
  %text = extractvalue %_Z10JsonNumber %variant.val, 0
  %frame.page = load ptr, ptr %frame, align 8
  %frame.has_page = icmp ne ptr %frame.page, null
  br i1 %frame.has_page, label %frame.forced, label %frame.force

frame.force:                                      ; preds = %choose.when
  %forced_page = call ptr @_Z17scaly_force_frameP5Frame(ptr %frame)
  br label %frame.forced

frame.forced:                                     ; preds = %frame.force, %choose.when
  %forced_page1 = phi ptr [ %frame.page, %choose.when ], [ %forced_page, %frame.force ]
  %struct.region = call ptr @_ZN4Page8allocateEmm(ptr %forced_page1, i64 ptrtoint (ptr getelementptr (%_Z6VectorI2u8E, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr ({ i1, %_Z6VectorI2u8E }, ptr null, i64 0, i32 1) to i64))
  %length = extractvalue %_Z5SliceI2u8E %text, 0
  %add = add i64 %length, 1
  call void @_ZN6VectorI2u8EC1Em(ptr %struct.region, i64 %add)
  store ptr %struct.region, ptr %buf, align 1
  store i64 0, ptr %i, align 1
  br label %while.cond

while.cond:                                       ; preds = %while.body, %frame.forced
  %i2 = load i64, ptr %i, align 8
  %length3 = extractvalue %_Z5SliceI2u8E %text, 0
  %lt = icmp ult i64 %i2, %length3
  br i1 %lt, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %buf4 = load ptr, ptr %buf, align 8
  %i5 = load i64, ptr %i, align 8
  store %_Z5SliceI2u8E %text, ptr %arg.tmp, align 1
  %i6 = load i64, ptr %i, align 8
  %call = call i8 @_ZN5SliceI2u8EixEm(ptr %arg.tmp, i64 %i6)
  call void @_ZN6VectorI2u8E3putEm2u8(ptr %buf4, i64 %i5, i8 %call)
  %i7 = load i64, ptr %i, align 8
  %add8 = add i64 %i7, 1
  store i64 %add8, ptr %i, align 1
  br label %while.cond

while.exit:                                       ; preds = %while.cond
  %buf9 = load ptr, ptr %buf, align 8
  %length10 = extractvalue %_Z5SliceI2u8E %text, 0
  call void @_ZN6VectorI2u8E3putEm2u8(ptr %buf9, i64 %length10, i8 0)
  %buf11 = load ptr, ptr %buf, align 8
  %load.struct = load %_Z6VectorI2u8E, ptr %buf11, align 8
  %data = extractvalue %_Z6VectorI2u8E %load.struct, 1
  %call12 = call double @strtod(ptr %data, ptr null)
  call void @_Z19scaly_release_frameP5Frame(ptr %frame)
  ret double %call12
}

define linkonce_odr i64 @_ZN9JsonValueB4json6lengthEv(ptr %0) {
entry:
  %tag.ptr = getelementptr inbounds nuw %_Z9JsonValue, ptr %0, i32 0, i32 0
  %tag = load i8, ptr %tag.ptr, align 1
  switch i8 %tag, label %choose.else [
    i8 4, label %choose.when
    i8 5, label %choose.when1
  ]

choose.end:                                       ; No predecessors!
  ret i64 0

choose.else:                                      ; preds = %entry
  ret i64 0

choose.when:                                      ; preds = %entry
  %"variant.c_data().ptr" = getelementptr inbounds nuw %_Z9JsonValue, ptr %0, i32 0, i32 1
  %variant.val = load %_Z9JsonArray, ptr %"variant.c_data().ptr", align 8
  %items = extractvalue %_Z9JsonArray %variant.val, 0
  %length = extractvalue %_Z5SliceI9JsonValueE %items, 0
  ret i64 %length

choose.when1:                                     ; preds = %entry
  %"variant.c_data().ptr2" = getelementptr inbounds nuw %_Z9JsonValue, ptr %0, i32 0, i32 1
  %variant.val3 = load %_Z10JsonObject, ptr %"variant.c_data().ptr2", align 8
  %members = extractvalue %_Z10JsonObject %variant.val3, 0
  %length4 = extractvalue %_Z5SliceI10JsonMemberE %members, 0
  ret i64 %length4
}

define linkonce_odr void @_ZN9JsonValueB4json4itemEPN4scaly6memory4PageEm(ptr noalias sret(%_Z9JsonValue) %0, ptr %1, ptr %2, i64 %3) {
entry:
  %variant.ptr = alloca %_Z9JsonValue, align 8
  %arg.tmp = alloca %_Z5SliceI9JsonValueE, align 8
  %sret.result = alloca %_Z9JsonValue, align 8
  %tag.ptr = getelementptr inbounds nuw %_Z9JsonValue, ptr %2, i32 0, i32 0
  %tag = load i8, ptr %tag.ptr, align 1
  switch i8 %tag, label %choose.else [
    i8 4, label %choose.when
  ]

choose.end:                                       ; No predecessors!
  ret void

choose.else:                                      ; preds = %entry
  %variant.tag.ptr3 = getelementptr inbounds nuw %_Z9JsonValue, ptr %variant.ptr, i32 0, i32 0
  store i8 0, ptr %variant.tag.ptr3, align 1
  %variant.val4 = load %_Z9JsonValue, ptr %variant.ptr, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %variant.ptr, i64 ptrtoint (ptr getelementptr (%_Z9JsonValue, ptr null, i32 1) to i64), i1 false)
  ret void

choose.when:                                      ; preds = %entry
  %"variant.c_data().ptr" = getelementptr inbounds nuw %_Z9JsonValue, ptr %2, i32 0, i32 1
  %variant.val = load %_Z9JsonArray, ptr %"variant.c_data().ptr", align 8
  %items = extractvalue %_Z9JsonArray %variant.val, 0
  %length = extractvalue %_Z5SliceI9JsonValueE %items, 0
  %lt = icmp ult i64 %3, %length
  br i1 %lt, label %if.then, label %if.end

if.then:                                          ; preds = %choose.when
  %items1 = extractvalue %_Z9JsonArray %variant.val, 0
  store %_Z5SliceI9JsonValueE %items1, ptr %arg.tmp, align 1
  call void @_ZN5SliceI9JsonValueB4jsonEixEm(ptr noalias sret(%_Z9JsonValue) %sret.result, ptr %arg.tmp, i64 %3)
  %sret.body = load %_Z9JsonValue, ptr %sret.result, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %sret.result, i64 ptrtoint (ptr getelementptr (%_Z9JsonValue, ptr null, i32 1) to i64), i1 false)
  ret void

if.end:                                           ; preds = %choose.when
  %variant.tag.ptr = getelementptr inbounds nuw %_Z9JsonValue, ptr %sret.result, i32 0, i32 0
  store i8 0, ptr %variant.tag.ptr, align 1
  %variant.val2 = load %_Z9JsonValue, ptr %sret.result, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %sret.result, i64 ptrtoint (ptr getelementptr (%_Z9JsonValue, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN9JsonValueB4json3getEPN4scaly6memory4PageE5SliceI2u8E(ptr noalias sret(%_Z9JsonValue) %0, ptr %1, ptr %2, ptr %3) {
entry:
  %variant.ptr6 = alloca %_Z9JsonValue, align 8
  %variant.ptr = alloca %_Z9JsonValue, align 8
  %arg.tmp = alloca %_Z5SliceI10JsonMemberE, align 8
  %sret.result = alloca %_Z10JsonMember, align 8
  %i = alloca i64, align 8
  %tag.ptr = getelementptr inbounds nuw %_Z9JsonValue, ptr %2, i32 0, i32 0
  %tag = load i8, ptr %tag.ptr, align 1
  switch i8 %tag, label %choose.else [
    i8 5, label %choose.when
  ]

choose.end:                                       ; No predecessors!
  ret void

choose.else:                                      ; preds = %entry
  %variant.tag.ptr7 = getelementptr inbounds nuw %_Z9JsonValue, ptr %variant.ptr6, i32 0, i32 0
  store i8 0, ptr %variant.tag.ptr7, align 1
  %variant.val8 = load %_Z9JsonValue, ptr %variant.ptr6, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %variant.ptr6, i64 ptrtoint (ptr getelementptr (%_Z9JsonValue, ptr null, i32 1) to i64), i1 false)
  ret void

choose.when:                                      ; preds = %entry
  %"variant.c_data().ptr" = getelementptr inbounds nuw %_Z9JsonValue, ptr %2, i32 0, i32 1
  %variant.val = load %_Z10JsonObject, ptr %"variant.c_data().ptr", align 8
  store i64 0, ptr %i, align 1
  br label %while.cond

while.cond:                                       ; preds = %if.end, %choose.when
  %i1 = load i64, ptr %i, align 8
  %members = extractvalue %_Z10JsonObject %variant.val, 0
  %length = extractvalue %_Z5SliceI10JsonMemberE %members, 0
  %lt = icmp ult i64 %i1, %length
  br i1 %lt, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %members2 = extractvalue %_Z10JsonObject %variant.val, 0
  store %_Z5SliceI10JsonMemberE %members2, ptr %arg.tmp, align 1
  %i3 = load i64, ptr %i, align 8
  call void @_ZN5SliceI10JsonMemberB4jsonEixEm(ptr noalias sret(%_Z10JsonMember) %sret.result, ptr %arg.tmp, i64 %i3)
  %field.inplace = getelementptr inbounds nuw %_Z10JsonMember, ptr %sret.result, i32 0, i32 0
  %call = call i1 @_ZN5SliceI2u8E6equalsE5SliceI2u8E(ptr %field.inplace, ptr %3)
  br i1 %call, label %if.then, label %if.end

while.exit:                                       ; preds = %while.cond
  %variant.tag.ptr = getelementptr inbounds nuw %_Z9JsonValue, ptr %variant.ptr, i32 0, i32 0
  store i8 0, ptr %variant.tag.ptr, align 1
  %variant.val5 = load %_Z9JsonValue, ptr %variant.ptr, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %variant.ptr, i64 ptrtoint (ptr getelementptr (%_Z9JsonValue, ptr null, i32 1) to i64), i1 false)
  ret void

if.then:                                          ; preds = %while.body
  %load.struct = load %_Z10JsonMember, ptr %sret.result, align 8
  %value = extractvalue %_Z10JsonMember %load.struct, 1
  store %_Z9JsonValue %value, ptr %0, align 1
  ret void

if.end:                                           ; preds = %while.body
  %i4 = load i64, ptr %i, align 8
  %add = add i64 %i4, 1
  store i64 %add, ptr %i, align 1
  br label %while.cond
}

define linkonce_odr i1 @_ZN9JsonValueB4json3hasE5SliceI2u8E(ptr %0, ptr %1) {
entry:
  %arg.tmp4 = alloca %_Z5SliceI2u8E, align 8
  %arg.tmp = alloca %_Z5SliceI10JsonMemberE, align 8
  %sret.result = alloca %_Z10JsonMember, align 8
  %i = alloca i64, align 8
  %tag.ptr = getelementptr inbounds nuw %_Z9JsonValue, ptr %0, i32 0, i32 0
  %tag = load i8, ptr %tag.ptr, align 1
  switch i8 %tag, label %choose.else [
    i8 5, label %choose.when
  ]

choose.end:                                       ; No predecessors!
  ret i1 false

choose.else:                                      ; preds = %entry
  ret i1 false

choose.when:                                      ; preds = %entry
  %"variant.c_data().ptr" = getelementptr inbounds nuw %_Z9JsonValue, ptr %0, i32 0, i32 1
  %variant.val = load %_Z10JsonObject, ptr %"variant.c_data().ptr", align 8
  store i64 0, ptr %i, align 1
  br label %while.cond

while.cond:                                       ; preds = %if.end, %choose.when
  %i1 = load i64, ptr %i, align 8
  %members = extractvalue %_Z10JsonObject %variant.val, 0
  %length = extractvalue %_Z5SliceI10JsonMemberE %members, 0
  %lt = icmp ult i64 %i1, %length
  br i1 %lt, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %members2 = extractvalue %_Z10JsonObject %variant.val, 0
  store %_Z5SliceI10JsonMemberE %members2, ptr %arg.tmp, align 1
  %i3 = load i64, ptr %i, align 8
  call void @_ZN5SliceI10JsonMemberB4jsonEixEm(ptr noalias sret(%_Z10JsonMember) %sret.result, ptr %arg.tmp, i64 %i3)
  %load.struct = load %_Z10JsonMember, ptr %sret.result, align 8
  %key = extractvalue %_Z10JsonMember %load.struct, 0
  store %_Z5SliceI2u8E %key, ptr %arg.tmp4, align 1
  %call = call i1 @_ZN5SliceI2u8E6equalsE5SliceI2u8E(ptr %arg.tmp4, ptr %1)
  br i1 %call, label %if.then, label %if.end

while.exit:                                       ; preds = %while.cond
  ret i1 false

if.then:                                          ; preds = %while.body
  ret i1 true

if.end:                                           ; preds = %while.body
  %i5 = load i64, ptr %i, align 8
  %add = add i64 %i5, 1
  store i64 %add, ptr %i, align 1
  br label %while.cond
}

define linkonce_odr void @_ZN9JsonValueB4json6key_atEm(ptr noalias sret(%_Z5SliceI2u8E) %0, ptr %1, i64 %2) {
entry:
  %struct.init3 = alloca %_Z5SliceI2u8E, align 8
  %struct.init = alloca %_Z5SliceI2u8E, align 8
  %arg.tmp = alloca %_Z5SliceI10JsonMemberE, align 8
  %sret.result = alloca %_Z10JsonMember, align 8
  %tag.ptr = getelementptr inbounds nuw %_Z9JsonValue, ptr %1, i32 0, i32 0
  %tag = load i8, ptr %tag.ptr, align 1
  switch i8 %tag, label %choose.else [
    i8 5, label %choose.when
  ]

choose.end:                                       ; No predecessors!
  ret void

choose.else:                                      ; preds = %entry
  %tuple.field4 = getelementptr inbounds nuw %_Z5SliceI2u8E, ptr %struct.init3, i32 0, i32 0
  store i64 0, ptr %tuple.field4, align 8
  %tuple.field5 = getelementptr inbounds nuw %_Z5SliceI2u8E, ptr %struct.init3, i32 0, i32 1
  store ptr null, ptr %tuple.field5, align 8
  %sret.body6 = load %_Z5SliceI2u8E, ptr %struct.init3, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %struct.init3, i64 ptrtoint (ptr getelementptr (%_Z5SliceI2u8E, ptr null, i32 1) to i64), i1 false)
  ret void

choose.when:                                      ; preds = %entry
  %"variant.c_data().ptr" = getelementptr inbounds nuw %_Z9JsonValue, ptr %1, i32 0, i32 1
  %variant.val = load %_Z10JsonObject, ptr %"variant.c_data().ptr", align 8
  %members = extractvalue %_Z10JsonObject %variant.val, 0
  %length = extractvalue %_Z5SliceI10JsonMemberE %members, 0
  %lt = icmp ult i64 %2, %length
  br i1 %lt, label %if.then, label %if.end

if.then:                                          ; preds = %choose.when
  %members1 = extractvalue %_Z10JsonObject %variant.val, 0
  store %_Z5SliceI10JsonMemberE %members1, ptr %arg.tmp, align 1
  call void @_ZN5SliceI10JsonMemberB4jsonEixEm(ptr noalias sret(%_Z10JsonMember) %sret.result, ptr %arg.tmp, i64 %2)
  %load.struct = load %_Z10JsonMember, ptr %sret.result, align 8
  %key = extractvalue %_Z10JsonMember %load.struct, 0
  store %_Z5SliceI2u8E %key, ptr %0, align 1
  ret void

if.end:                                           ; preds = %choose.when
  %tuple.field = getelementptr inbounds nuw %_Z5SliceI2u8E, ptr %struct.init, i32 0, i32 0
  store i64 0, ptr %tuple.field, align 8
  %tuple.field2 = getelementptr inbounds nuw %_Z5SliceI2u8E, ptr %struct.init, i32 0, i32 1
  store ptr null, ptr %tuple.field2, align 8
  %sret.body = load %_Z5SliceI2u8E, ptr %struct.init, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %struct.init, i64 ptrtoint (ptr getelementptr (%_Z5SliceI2u8E, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN9JsonValueB4json8value_atEPN4scaly6memory4PageEm(ptr noalias sret(%_Z9JsonValue) %0, ptr %1, ptr %2, i64 %3) {
entry:
  %variant.ptr3 = alloca %_Z9JsonValue, align 8
  %variant.ptr = alloca %_Z9JsonValue, align 8
  %arg.tmp = alloca %_Z5SliceI10JsonMemberE, align 8
  %sret.result = alloca %_Z10JsonMember, align 8
  %tag.ptr = getelementptr inbounds nuw %_Z9JsonValue, ptr %2, i32 0, i32 0
  %tag = load i8, ptr %tag.ptr, align 1
  switch i8 %tag, label %choose.else [
    i8 5, label %choose.when
  ]

choose.end:                                       ; No predecessors!
  ret void

choose.else:                                      ; preds = %entry
  %variant.tag.ptr4 = getelementptr inbounds nuw %_Z9JsonValue, ptr %variant.ptr3, i32 0, i32 0
  store i8 0, ptr %variant.tag.ptr4, align 1
  %variant.val5 = load %_Z9JsonValue, ptr %variant.ptr3, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %variant.ptr3, i64 ptrtoint (ptr getelementptr (%_Z9JsonValue, ptr null, i32 1) to i64), i1 false)
  ret void

choose.when:                                      ; preds = %entry
  %"variant.c_data().ptr" = getelementptr inbounds nuw %_Z9JsonValue, ptr %2, i32 0, i32 1
  %variant.val = load %_Z10JsonObject, ptr %"variant.c_data().ptr", align 8
  %members = extractvalue %_Z10JsonObject %variant.val, 0
  %length = extractvalue %_Z5SliceI10JsonMemberE %members, 0
  %lt = icmp ult i64 %3, %length
  br i1 %lt, label %if.then, label %if.end

if.then:                                          ; preds = %choose.when
  %members1 = extractvalue %_Z10JsonObject %variant.val, 0
  store %_Z5SliceI10JsonMemberE %members1, ptr %arg.tmp, align 1
  call void @_ZN5SliceI10JsonMemberB4jsonEixEm(ptr noalias sret(%_Z10JsonMember) %sret.result, ptr %arg.tmp, i64 %3)
  %load.struct = load %_Z10JsonMember, ptr %sret.result, align 8
  %value = extractvalue %_Z10JsonMember %load.struct, 1
  store %_Z9JsonValue %value, ptr %0, align 1
  ret void

if.end:                                           ; preds = %choose.when
  %variant.tag.ptr = getelementptr inbounds nuw %_Z9JsonValue, ptr %variant.ptr, i32 0, i32 0
  store i8 0, ptr %variant.tag.ptr, align 1
  %variant.val2 = load %_Z9JsonValue, ptr %variant.ptr, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %variant.ptr, i64 ptrtoint (ptr getelementptr (%_Z9JsonValue, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr ptr @_ZN5ArrayI9JsonValueB4jsonE10get_bufferEv(ptr %0) {
entry:
  %load.struct = load %_Z5ArrayI9JsonValueE, ptr %0, align 8
  %buffer = extractvalue %_Z5ArrayI9JsonValueE %load.struct, 2
  ret ptr %buffer
}

define linkonce_odr i64 @_ZN5ArrayI9JsonValueB4jsonE10get_lengthEv(ptr %0) {
entry:
  %load.struct = load %_Z5ArrayI9JsonValueE, ptr %0, align 8
  %length = extractvalue %_Z5ArrayI9JsonValueE %load.struct, 0
  ret i64 %length
}

define linkonce_odr i64 @_ZN5ArrayI9JsonValueB4jsonE12get_capacityEv(ptr %0) {
entry:
  %load.struct = load %_Z5ArrayI9JsonValueE, ptr %0, align 8
  %capacity = extractvalue %_Z5ArrayI9JsonValueE %load.struct, 1
  ret i64 %capacity
}

define linkonce_odr void @_ZN5ArrayI9JsonValueB4jsonE10reallocateEv(ptr %0) {
entry:
  %first_cap = alloca i64, align 8
  %call = call ptr @_ZN4Page3getEPv(ptr %0)
  %load.struct = load %_Z5ArrayI9JsonValueE, ptr %0, align 8
  %buffer = extractvalue %_Z5ArrayI9JsonValueE %load.struct, 2
  %eq = icmp eq ptr %buffer, null
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %udiv = udiv i64 32, ptrtoint (ptr getelementptr (%_Z9JsonValue, ptr null, i32 1) to i64)
  store i64 %udiv, ptr %first_cap, align 1
  %first_cap1 = load i64, ptr %first_cap, align 8
  %lt = icmp ult i64 %first_cap1, 1
  br i1 %lt, label %if.then2, label %if.end3

if.end:                                           ; preds = %entry
  %load.struct18 = load %_Z5ArrayI9JsonValueE, ptr %0, align 8
  %capacity19 = extractvalue %_Z5ArrayI9JsonValueE %load.struct18, 1
  %mul20 = mul i64 %capacity19, ptrtoint (ptr getelementptr (%_Z9JsonValue, ptr null, i32 1) to i64)
  %le21 = icmp ule i64 %mul20, 1024
  %load.struct22 = load %_Z5ArrayI9JsonValueE, ptr %0, align 8
  %buffer23 = extractvalue %_Z5ArrayI9JsonValueE %load.struct22, 2
  %load.struct24 = load %_Z5ArrayI9JsonValueE, ptr %0, align 8
  %capacity25 = extractvalue %_Z5ArrayI9JsonValueE %load.struct24, 1
  %load.struct26 = load %_Z5ArrayI9JsonValueE, ptr %0, align 8
  %capacity27 = extractvalue %_Z5ArrayI9JsonValueE %load.struct26, 1
  %mul28 = mul i64 %capacity27, 2
  store i64 %mul28, ptr %first_cap, align 1
  %new_capacity = load i64, ptr %first_cap, align 8
  %mul29 = mul i64 %new_capacity, ptrtoint (ptr getelementptr (%_Z9JsonValue, ptr null, i32 1) to i64)
  %le30 = icmp ule i64 %mul29, 1024
  br i1 %le30, label %if.then31, label %if.end32

if.then2:                                         ; preds = %if.then
  store i64 1, ptr %first_cap, align 1
  br label %if.end3

if.end3:                                          ; preds = %if.then2, %if.then
  %first_cap4 = load i64, ptr %first_cap, align 8
  %mul = mul i64 %first_cap4, ptrtoint (ptr getelementptr (%_Z9JsonValue, ptr null, i32 1) to i64)
  %le = icmp ule i64 %mul, 1024
  br i1 %le, label %if.then5, label %if.end6

if.then5:                                         ; preds = %if.end3
  %first_cap7 = load i64, ptr %first_cap, align 8
  %mul8 = mul i64 %first_cap7, ptrtoint (ptr getelementptr (%_Z9JsonValue, ptr null, i32 1) to i64)
  %call9 = call ptr @_ZN4Page8allocateEmm(ptr %call, i64 %mul8, i64 8)
  %buffer10 = getelementptr inbounds nuw %_Z5ArrayI9JsonValueE, ptr %0, i32 0, i32 2
  store ptr %call9, ptr %buffer10, align 8
  %first_cap11 = load i64, ptr %first_cap, align 8
  %capacity = getelementptr inbounds nuw %_Z5ArrayI9JsonValueE, ptr %0, i32 0, i32 1
  store i64 %first_cap11, ptr %capacity, align 8
  ret void

if.end6:                                          ; preds = %if.end3
  %first_cap12 = load i64, ptr %first_cap, align 8
  %mul13 = mul i64 %first_cap12, ptrtoint (ptr getelementptr (%_Z9JsonValue, ptr null, i32 1) to i64)
  %call14 = call ptr @_ZN4Page25allocate_exclusive_bufferEmm(ptr %call, i64 %mul13, i64 8)
  %buffer15 = getelementptr inbounds nuw %_Z5ArrayI9JsonValueE, ptr %0, i32 0, i32 2
  store ptr %call14, ptr %buffer15, align 8
  %first_cap16 = load i64, ptr %first_cap, align 8
  %capacity17 = getelementptr inbounds nuw %_Z5ArrayI9JsonValueE, ptr %0, i32 0, i32 1
  store i64 %first_cap16, ptr %capacity17, align 8
  ret void

if.then31:                                        ; preds = %if.end
  %new_capacity33 = load i64, ptr %first_cap, align 8
  %mul34 = mul i64 %new_capacity33, ptrtoint (ptr getelementptr (%_Z9JsonValue, ptr null, i32 1) to i64)
  %call35 = call ptr @_ZN4Page8allocateEmm(ptr %call, i64 %mul34, i64 8)
  %buffer36 = getelementptr inbounds nuw %_Z5ArrayI9JsonValueE, ptr %0, i32 0, i32 2
  store ptr %call35, ptr %buffer36, align 8
  %new_capacity37 = load i64, ptr %first_cap, align 8
  %capacity38 = getelementptr inbounds nuw %_Z5ArrayI9JsonValueE, ptr %0, i32 0, i32 1
  store i64 %new_capacity37, ptr %capacity38, align 8
  %field.inplace = getelementptr inbounds nuw %_Z5ArrayI9JsonValueE, ptr %0, i32 0, i32 2
  %deref.recv = load ptr, ptr %field.inplace, align 8
  %mul39 = mul i64 %capacity25, ptrtoint (ptr getelementptr (%_Z9JsonValue, ptr null, i32 1) to i64)
  %call40 = call ptr @memcpy(ptr %deref.recv, ptr %buffer23, i64 %mul39)
  ret void

if.end32:                                         ; preds = %if.end
  %call41 = call ptr @_ZN4Page23allocate_exclusive_pageEv(ptr %call)
  %call42 = call i64 @_ZN4Page12get_capacityEm(ptr %call41, i64 8)
  call void @_ZN4Page25deallocate_exclusive_pageER4Page(ptr %call, ptr %call41)
  %new_capacity43 = load i64, ptr %first_cap, align 8
  %udiv44 = udiv i64 %call42, ptrtoint (ptr getelementptr (%_Z9JsonValue, ptr null, i32 1) to i64)
  %lt45 = icmp ult i64 %new_capacity43, %udiv44
  br i1 %lt45, label %if.then46, label %if.end47

if.then46:                                        ; preds = %if.end32
  %udiv48 = udiv i64 %call42, ptrtoint (ptr getelementptr (%_Z9JsonValue, ptr null, i32 1) to i64)
  store i64 %udiv48, ptr %first_cap, align 1
  br label %if.end47

if.end47:                                         ; preds = %if.then46, %if.end32
  %new_capacity49 = load i64, ptr %first_cap, align 8
  %mul50 = mul i64 %new_capacity49, ptrtoint (ptr getelementptr (%_Z9JsonValue, ptr null, i32 1) to i64)
  %call51 = call ptr @_ZN4Page25allocate_exclusive_bufferEmm(ptr %call, i64 %mul50, i64 8)
  %buffer52 = getelementptr inbounds nuw %_Z5ArrayI9JsonValueE, ptr %0, i32 0, i32 2
  store ptr %call51, ptr %buffer52, align 8
  %new_capacity53 = load i64, ptr %first_cap, align 8
  %capacity54 = getelementptr inbounds nuw %_Z5ArrayI9JsonValueE, ptr %0, i32 0, i32 1
  store i64 %new_capacity53, ptr %capacity54, align 8
  %field.inplace55 = getelementptr inbounds nuw %_Z5ArrayI9JsonValueE, ptr %0, i32 0, i32 2
  %deref.recv56 = load ptr, ptr %field.inplace55, align 8
  %mul57 = mul i64 %capacity25, ptrtoint (ptr getelementptr (%_Z9JsonValue, ptr null, i32 1) to i64)
  %call58 = call ptr @memcpy(ptr %deref.recv56, ptr %buffer23, i64 %mul57)
  %eq59 = icmp eq i1 %le21, false
  br i1 %eq59, label %if.then60, label %if.end61

if.then60:                                        ; preds = %if.end47
  %call62 = call ptr @_ZN4Page3getEPv(ptr %buffer23)
  call void @_ZN4Page25deallocate_exclusive_pageER4Page(ptr %call, ptr %call62)
  br label %if.end61

if.end61:                                         ; preds = %if.then60, %if.end47
  ret void
}

define linkonce_odr void @_ZN5ArrayI9JsonValueB4jsonE3addE9JsonValueB4json(ptr %0, ptr %1) {
entry:
  %load.struct = load %_Z5ArrayI9JsonValueE, ptr %0, align 8
  %buffer = extractvalue %_Z5ArrayI9JsonValueE %load.struct, 2
  %eq = icmp eq ptr %buffer, null
  br i1 %eq, label %if.then, label %lor.rhs

if.then:                                          ; preds = %lor.rhs, %entry
  call void @_ZN5ArrayI9JsonValueB4jsonE10reallocateEv(ptr %0)
  br label %if.end

if.end:                                           ; preds = %if.then, %lor.rhs
  %load.struct4 = load %_Z5ArrayI9JsonValueE, ptr %0, align 8
  %buffer5 = extractvalue %_Z5ArrayI9JsonValueE %load.struct4, 2
  %load.struct6 = load %_Z5ArrayI9JsonValueE, ptr %0, align 8
  %length7 = extractvalue %_Z5ArrayI9JsonValueE %load.struct6, 0
  %ptr.add = getelementptr inbounds %_Z9JsonValue, ptr %buffer5, i64 %length7
  %store.load = load %_Z9JsonValue, ptr %1, align 1
  store %_Z9JsonValue %store.load, ptr %ptr.add, align 1
  %load.struct8 = load %_Z5ArrayI9JsonValueE, ptr %0, align 8
  %length9 = extractvalue %_Z5ArrayI9JsonValueE %load.struct8, 0
  %add = add i64 %length9, 1
  %length10 = getelementptr inbounds nuw %_Z5ArrayI9JsonValueE, ptr %0, i32 0, i32 0
  store i64 %add, ptr %length10, align 8
  ret void

lor.rhs:                                          ; preds = %entry
  %load.struct1 = load %_Z5ArrayI9JsonValueE, ptr %0, align 8
  %length = extractvalue %_Z5ArrayI9JsonValueE %load.struct1, 0
  %load.struct2 = load %_Z5ArrayI9JsonValueE, ptr %0, align 8
  %capacity = extractvalue %_Z5ArrayI9JsonValueE %load.struct2, 1
  %eq3 = icmp eq i64 %length, %capacity
  br i1 %eq3, label %if.then, label %if.end
}

define linkonce_odr ptr @_ZN6VectorI9JsonValueB4jsonE3getEm(ptr noalias %0, i64 %1) {
entry:
  %load.struct = load %_Z6VectorI9JsonValueE, ptr %0, align 8
  %length = extractvalue %_Z6VectorI9JsonValueE %load.struct, 0
  %ge = icmp uge i64 %1, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %load.struct1 = load %_Z6VectorI9JsonValueE, ptr %0, align 8
  %data = extractvalue %_Z6VectorI9JsonValueE %load.struct1, 1
  %ptr.add = getelementptr inbounds %_Z9JsonValue, ptr %data, i64 %1
  ret ptr %ptr.add
}

define linkonce_odr ptr @_ZN6VectorI9JsonValueB4jsonE2atEm(ptr noalias %0, i64 %1) {
entry:
  %load.struct = load %_Z6VectorI9JsonValueE, ptr %0, align 8
  %length = extractvalue %_Z6VectorI9JsonValueE %load.struct, 0
  %ge = icmp uge i64 %1, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %field.inplace = getelementptr inbounds nuw %_Z6VectorI9JsonValueE, ptr %0, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  call void @_Z15scaly_panic_oobP10const_charmm(ptr @.str.35, i64 %1, i64 %field.val)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %load.struct1 = load %_Z6VectorI9JsonValueE, ptr %0, align 8
  %data = extractvalue %_Z6VectorI9JsonValueE %load.struct1, 1
  %ptr.add = getelementptr inbounds %_Z9JsonValue, ptr %data, i64 %1
  ret ptr %ptr.add
}

define linkonce_odr ptr @_ZN6VectorI9JsonValueB4jsonE7get_ptrEm(ptr noalias %0, i64 %1) {
entry:
  %load.struct = load %_Z6VectorI9JsonValueE, ptr %0, align 8
  %length = extractvalue %_Z6VectorI9JsonValueE %load.struct, 0
  %ge = icmp uge i64 %1, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %load.struct1 = load %_Z6VectorI9JsonValueE, ptr %0, align 8
  %data = extractvalue %_Z6VectorI9JsonValueE %load.struct1, 1
  %ptr.add = getelementptr inbounds %_Z9JsonValue, ptr %data, i64 %1
  ret ptr %ptr.add
}

define linkonce_odr void @_ZN6VectorI9JsonValueB4jsonE3putEm9JsonValueB4json(ptr noalias %0, i64 %1, ptr %2) {
entry:
  %load.struct = load %_Z6VectorI9JsonValueE, ptr %0, align 8
  %length = extractvalue %_Z6VectorI9JsonValueE %load.struct, 0
  %ge = icmp uge i64 %1, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %field.inplace = getelementptr inbounds nuw %_Z6VectorI9JsonValueE, ptr %0, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  call void @_Z15scaly_panic_oobP10const_charmm(ptr @.str.36, i64 %1, i64 %field.val)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %load.struct1 = load %_Z6VectorI9JsonValueE, ptr %0, align 8
  %data = extractvalue %_Z6VectorI9JsonValueE %load.struct1, 1
  %ptr.add = getelementptr inbounds %_Z9JsonValue, ptr %data, i64 %1
  %store.load = load %_Z9JsonValue, ptr %2, align 1
  store %_Z9JsonValue %store.load, ptr %ptr.add, align 1
  ret void
}

define linkonce_odr ptr @_ZN14VectorIteratorI9JsonValueB4jsonE4nextEv(ptr %0) {
entry:
  %load.struct = load %_Z14VectorIteratorI9JsonValueE, ptr %0, align 8
  %vector = extractvalue %_Z14VectorIteratorI9JsonValueE %load.struct, 0
  %eq = icmp eq ptr %vector, null
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %load.struct1 = load %_Z14VectorIteratorI9JsonValueE, ptr %0, align 8
  %position = extractvalue %_Z14VectorIteratorI9JsonValueE %load.struct1, 1
  %load.struct2 = load %_Z14VectorIteratorI9JsonValueE, ptr %0, align 8
  %vector3 = extractvalue %_Z14VectorIteratorI9JsonValueE %load.struct2, 0
  %deref = load %_Z6VectorI9JsonValueE, ptr %vector3, align 8
  %length = extractvalue %_Z6VectorI9JsonValueE %deref, 0
  %eq4 = icmp eq i64 %position, %length
  br i1 %eq4, label %if.then5, label %if.end6

if.then5:                                         ; preds = %if.end
  ret ptr null

if.end6:                                          ; preds = %if.end
  %load.struct7 = load %_Z14VectorIteratorI9JsonValueE, ptr %0, align 8
  %position8 = extractvalue %_Z14VectorIteratorI9JsonValueE %load.struct7, 1
  %add = add i64 %position8, 1
  %position9 = getelementptr inbounds nuw %_Z14VectorIteratorI9JsonValueE, ptr %0, i32 0, i32 1
  store i64 %add, ptr %position9, align 8
  %field.inplace = getelementptr inbounds nuw %_Z14VectorIteratorI9JsonValueE, ptr %0, i32 0, i32 0
  %deref.recv = load ptr, ptr %field.inplace, align 8
  %load.struct10 = load %_Z14VectorIteratorI9JsonValueE, ptr %0, align 8
  %position11 = extractvalue %_Z14VectorIteratorI9JsonValueE %load.struct10, 1
  %sub = sub i64 %position11, 1
  %call = call ptr @_ZN6VectorI9JsonValueB4jsonE7get_ptrEm(ptr %deref.recv, i64 %sub)
  ret ptr %call
}

define linkonce_odr void @_ZN14VectorIteratorI9JsonValueB4jsonEC1E6OptionIR6VectorI9JsonValueB4jsonEE(ptr %0, ptr %1) {
entry:
  %vector = getelementptr inbounds nuw %_Z14VectorIteratorI9JsonValueE, ptr %0, i32 0, i32 0
  store ptr %1, ptr %vector, align 8
  %position = getelementptr inbounds nuw %_Z14VectorIteratorI9JsonValueE, ptr %0, i32 0, i32 1
  store i64 0, ptr %position, align 8
  ret void
}

define linkonce_odr void @_ZN6VectorI9JsonValueB4jsonE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z14VectorIteratorI9JsonValueE) %0, ptr %1, ptr noalias %2) {
entry:
  %struct.init = alloca %_Z14VectorIteratorI9JsonValueE, align 8
  call void @_ZN14VectorIteratorI9JsonValueB4jsonEC1E6OptionIR6VectorI9JsonValueB4jsonEE(ptr %struct.init, ptr %2)
  %sret.body = load %_Z14VectorIteratorI9JsonValueE, ptr %struct.init, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %struct.init, i64 ptrtoint (ptr getelementptr (%_Z14VectorIteratorI9JsonValueE, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN6VectorI9JsonValueB4jsonE8as_sliceEv(ptr noalias sret(%_Z5SliceI9JsonValueE) %0, ptr noalias %1) {
entry:
  %load.struct = load %_Z6VectorI9JsonValueE, ptr %1, align 8
  %length = extractvalue %_Z6VectorI9JsonValueE %load.struct, 0
  %load.struct1 = load %_Z6VectorI9JsonValueE, ptr %1, align 8
  %data = extractvalue %_Z6VectorI9JsonValueE %load.struct1, 1
  %tuple = alloca %_Z5SliceI9JsonValueE, align 8
  %tuple.field = getelementptr inbounds nuw %_Z5SliceI9JsonValueE, ptr %tuple, i32 0, i32 0
  store i64 %length, ptr %tuple.field, align 1
  %tuple.field2 = getelementptr inbounds nuw %_Z5SliceI9JsonValueE, ptr %tuple, i32 0, i32 1
  store ptr %data, ptr %tuple.field2, align 1
  %tuple.val = load %_Z5SliceI9JsonValueE, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z5SliceI9JsonValueE, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN6VectorI9JsonValueB4jsonEC1Ev(ptr noalias %0) {
entry:
  %length = getelementptr inbounds nuw %_Z6VectorI9JsonValueE, ptr %0, i32 0, i32 0
  store i64 0, ptr %length, align 8
  %data = getelementptr inbounds nuw %_Z6VectorI9JsonValueE, ptr %0, i32 0, i32 1
  store ptr null, ptr %data, align 8
  ret void
}

define linkonce_odr void @_ZN6VectorI9JsonValueB4jsonEC1Em(ptr %0, i64 %1) {
entry:
  %call = call ptr @_ZN4Page3getEPv(ptr %0)
  %length = getelementptr inbounds nuw %_Z6VectorI9JsonValueE, ptr %0, i32 0, i32 0
  store i64 %1, ptr %length, align 8
  %gt = icmp ugt i64 %1, 0
  br i1 %gt, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %mul = mul i64 %1, ptrtoint (ptr getelementptr (%_Z9JsonValue, ptr null, i32 1) to i64)
  %call1 = call ptr @_ZN4Page8allocateEmm(ptr %call, i64 %mul, i64 8)
  %data = getelementptr inbounds nuw %_Z6VectorI9JsonValueE, ptr %0, i32 0, i32 1
  store ptr %call1, ptr %data, align 8
  %field.inplace = getelementptr inbounds nuw %_Z6VectorI9JsonValueE, ptr %0, i32 0, i32 1
  %deref.recv = load ptr, ptr %field.inplace, align 8
  %mul2 = mul i64 %1, ptrtoint (ptr getelementptr (%_Z9JsonValue, ptr null, i32 1) to i64)
  %call3 = call ptr @memset(ptr %deref.recv, i32 0, i64 %mul2)
  br label %if.end

if.else:                                          ; preds = %entry
  %data4 = getelementptr inbounds nuw %_Z6VectorI9JsonValueE, ptr %0, i32 0, i32 1
  store ptr null, ptr %data4, align 8
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  %if.value = phi ptr [ %call3, %if.then ], [ null, %if.else ]
  ret void
}

define linkonce_odr void @_ZN6VectorI9JsonValueB4jsonEC1E6VectorI9JsonValueB4jsonE(ptr %0, ptr %1) {
entry:
  %call = call ptr @_ZN4Page3getEPv(ptr %0)
  %load.struct = load %_Z6VectorI9JsonValueE, ptr %1, align 8
  %length = extractvalue %_Z6VectorI9JsonValueE %load.struct, 0
  %length1 = getelementptr inbounds nuw %_Z6VectorI9JsonValueE, ptr %0, i32 0, i32 0
  store i64 %length, ptr %length1, align 8
  %load.struct2 = load %_Z6VectorI9JsonValueE, ptr %0, align 8
  %length3 = extractvalue %_Z6VectorI9JsonValueE %load.struct2, 0
  %gt = icmp ugt i64 %length3, 0
  br i1 %gt, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %load.struct4 = load %_Z6VectorI9JsonValueE, ptr %0, align 8
  %length5 = extractvalue %_Z6VectorI9JsonValueE %load.struct4, 0
  %mul = mul i64 %length5, ptrtoint (ptr getelementptr (%_Z9JsonValue, ptr null, i32 1) to i64)
  %call6 = call ptr @_ZN4Page8allocateEmm(ptr %call, i64 %mul, i64 8)
  %data = getelementptr inbounds nuw %_Z6VectorI9JsonValueE, ptr %0, i32 0, i32 1
  store ptr %call6, ptr %data, align 8
  %field.inplace = getelementptr inbounds nuw %_Z6VectorI9JsonValueE, ptr %0, i32 0, i32 1
  %deref.recv = load ptr, ptr %field.inplace, align 8
  %field.inplace7 = getelementptr inbounds nuw %_Z6VectorI9JsonValueE, ptr %1, i32 0, i32 1
  %deref.recv8 = load ptr, ptr %field.inplace7, align 8
  %load.struct9 = load %_Z6VectorI9JsonValueE, ptr %0, align 8
  %length10 = extractvalue %_Z6VectorI9JsonValueE %load.struct9, 0
  %mul11 = mul i64 %length10, ptrtoint (ptr getelementptr (%_Z9JsonValue, ptr null, i32 1) to i64)
  %call12 = call ptr @memcpy(ptr %deref.recv, ptr %deref.recv8, i64 %mul11)
  br label %if.end

if.else:                                          ; preds = %entry
  %data13 = getelementptr inbounds nuw %_Z6VectorI9JsonValueE, ptr %0, i32 0, i32 1
  store ptr null, ptr %data13, align 8
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  %if.value = phi ptr [ %call12, %if.then ], [ null, %if.else ]
  ret void
}

define linkonce_odr void @_ZN6VectorI9JsonValueB4jsonEC1E5ArrayI9JsonValueB4jsonE(ptr %0, ptr %1) {
entry:
  %call = call ptr @_ZN4Page3getEPv(ptr %0)
  %load.struct = load %_Z5ArrayI9JsonValueE, ptr %1, align 8
  %length = extractvalue %_Z5ArrayI9JsonValueE %load.struct, 0
  %length1 = getelementptr inbounds nuw %_Z6VectorI9JsonValueE, ptr %0, i32 0, i32 0
  store i64 %length, ptr %length1, align 8
  %load.struct2 = load %_Z6VectorI9JsonValueE, ptr %0, align 8
  %length3 = extractvalue %_Z6VectorI9JsonValueE %load.struct2, 0
  %gt = icmp ugt i64 %length3, 0
  br i1 %gt, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %load.struct4 = load %_Z6VectorI9JsonValueE, ptr %0, align 8
  %length5 = extractvalue %_Z6VectorI9JsonValueE %load.struct4, 0
  %mul = mul i64 %length5, ptrtoint (ptr getelementptr (%_Z9JsonValue, ptr null, i32 1) to i64)
  %call6 = call ptr @_ZN4Page8allocateEmm(ptr %call, i64 %mul, i64 8)
  %data = getelementptr inbounds nuw %_Z6VectorI9JsonValueE, ptr %0, i32 0, i32 1
  store ptr %call6, ptr %data, align 8
  %field.inplace = getelementptr inbounds nuw %_Z6VectorI9JsonValueE, ptr %0, i32 0, i32 1
  %deref.recv = load ptr, ptr %field.inplace, align 8
  %call7 = call ptr @_ZN5ArrayI9JsonValueB4jsonE10get_bufferEv(ptr %1)
  %load.struct8 = load %_Z6VectorI9JsonValueE, ptr %0, align 8
  %length9 = extractvalue %_Z6VectorI9JsonValueE %load.struct8, 0
  %mul10 = mul i64 %length9, ptrtoint (ptr getelementptr (%_Z9JsonValue, ptr null, i32 1) to i64)
  %call11 = call ptr @memcpy(ptr %deref.recv, ptr %call7, i64 %mul10)
  br label %if.end

if.else:                                          ; preds = %entry
  %data12 = getelementptr inbounds nuw %_Z6VectorI9JsonValueE, ptr %0, i32 0, i32 1
  store ptr null, ptr %data12, align 8
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  %if.value = phi ptr [ %call11, %if.then ], [ null, %if.else ]
  ret void
}

define linkonce_odr ptr @_ZN4ListI9JsonValueB4jsonE8get_headEv(ptr %0) {
entry:
  %load.struct = load %_Z4ListI9JsonValueE, ptr %0, align 8
  %head = extractvalue %_Z4ListI9JsonValueE %load.struct, 0
  %eq = icmp eq ptr %head, null
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %addr.gep = getelementptr inbounds nuw %_Z4ListI9JsonValueE, ptr %0, i32 0, i32 0
  %addr.hop = load ptr, ptr %addr.gep, align 8
  %addr.gep1 = getelementptr inbounds nuw %_Z4NodeI9JsonValueE, ptr %addr.hop, i32 0, i32 0
  ret ptr %addr.gep1
}

define linkonce_odr ptr @_ZN12ListIteratorI9JsonValueB4jsonE4nextEv(ptr %0) {
entry:
  %old_current = alloca ptr, align 8
  %load.struct = load %_Z12ListIteratorI9JsonValueE, ptr %0, align 8
  %current = extractvalue %_Z12ListIteratorI9JsonValueE %load.struct, 0
  %ne = icmp ne ptr %current, null
  br i1 %ne, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %load.struct1 = load %_Z12ListIteratorI9JsonValueE, ptr %0, align 8
  %current2 = extractvalue %_Z12ListIteratorI9JsonValueE %load.struct1, 0
  store ptr %current2, ptr %old_current, align 1
  %load.struct3 = load %_Z12ListIteratorI9JsonValueE, ptr %0, align 8
  %current4 = extractvalue %_Z12ListIteratorI9JsonValueE %load.struct3, 0
  %deref = load %_Z4NodeI9JsonValueE, ptr %current4, align 8
  %next = extractvalue %_Z4NodeI9JsonValueE %deref, 1
  %current5 = getelementptr inbounds nuw %_Z12ListIteratorI9JsonValueE, ptr %0, i32 0, i32 0
  store ptr %next, ptr %current5, align 8
  %old_current6 = load ptr, ptr %old_current, align 8
  %addr.gep = getelementptr inbounds nuw %_Z4NodeI9JsonValueE, ptr %old_current6, i32 0, i32 0
  ret ptr %addr.gep

if.else:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; No predecessors!
  ret ptr null
}

define linkonce_odr i64 @_ZN4ListI9JsonValueB4jsonE5countEv(ptr %0) {
entry:
  %sret.result = alloca %_Z12ListIteratorI9JsonValueE, align 8
  call void @_ZN4ListI9JsonValueB4jsonE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z12ListIteratorI9JsonValueE) %sret.result, ptr null, ptr %0)
  %list_iterator = alloca ptr, align 8
  store ptr %sret.result, ptr %list_iterator, align 1
  %i = alloca i64, align 8
  store i64 0, ptr %i, align 1
  br label %while.cond

while.cond:                                       ; preds = %while.body, %entry
  %list_iterator1 = load ptr, ptr %list_iterator, align 8
  %call = call ptr @_ZN12ListIteratorI9JsonValueB4jsonE4nextEv(ptr %list_iterator1)
  %ne = icmp ne ptr %call, null
  br i1 %ne, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %i2 = load i64, ptr %i, align 8
  %add = add i64 %i2, 1
  store i64 %add, ptr %i, align 1
  br label %while.cond

while.exit:                                       ; preds = %while.cond
  %i3 = load i64, ptr %i, align 8
  ret i64 %i3
}

define linkonce_odr void @_ZN4ListI9JsonValueB4jsonE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z12ListIteratorI9JsonValueE) %0, ptr %1, ptr %2) {
entry:
  %load.struct = load %_Z4ListI9JsonValueE, ptr %2, align 8
  %head = extractvalue %_Z4ListI9JsonValueE %load.struct, 0
  %tuple = alloca %_Z12ListIteratorI9JsonValueE, align 8
  %tuple.field = getelementptr inbounds nuw %_Z12ListIteratorI9JsonValueE, ptr %tuple, i32 0, i32 0
  store ptr %head, ptr %tuple.field, align 1
  %tuple.val = load %_Z12ListIteratorI9JsonValueE, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z12ListIteratorI9JsonValueE, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN4ListI9JsonValueB4jsonE4linkEP4NodeI9JsonValueB4jsonE(ptr %0, ptr %1) {
entry:
  %load.struct = load %_Z4ListI9JsonValueE, ptr %0, align 8
  %tail = extractvalue %_Z4ListI9JsonValueE %load.struct, 1
  %eq = icmp eq ptr %tail, null
  br i1 %eq, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %head = getelementptr inbounds nuw %_Z4ListI9JsonValueE, ptr %0, i32 0, i32 0
  store ptr %1, ptr %head, align 8
  br label %if.end

if.else:                                          ; preds = %entry
  %tail1 = getelementptr inbounds nuw %_Z4ListI9JsonValueE, ptr %0, i32 0, i32 1
  %field.deref = load ptr, ptr %tail1, align 8
  %next = getelementptr inbounds nuw %_Z4NodeI9JsonValueE, ptr %field.deref, i32 0, i32 1
  store ptr %1, ptr %next, align 8
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  %if.value = phi ptr [ %1, %if.then ], [ %1, %if.else ]
  %tail2 = getelementptr inbounds nuw %_Z4ListI9JsonValueE, ptr %0, i32 0, i32 1
  store ptr %1, ptr %tail2, align 8
  ret void
}

define linkonce_odr void @_ZN4ListI9JsonValueB4jsonE3addE9JsonValueB4json(ptr %0, ptr %1) {
entry:
  %own_page = call ptr @_Z3getPv(ptr %0)
  %tuple.region = call ptr @_ZN4Page8allocateEmm(ptr %own_page, i64 ptrtoint (ptr getelementptr (%_Z4NodeI9JsonValueE, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr ({ i1, %_Z4NodeI9JsonValueE }, ptr null, i64 0, i32 1) to i64))
  %field.load = load %_Z9JsonValue, ptr %1, align 1
  %tuple.field = getelementptr inbounds nuw %_Z4NodeI9JsonValueE, ptr %tuple.region, i32 0, i32 0
  store %_Z9JsonValue %field.load, ptr %tuple.field, align 1
  %tuple.field1 = getelementptr inbounds nuw %_Z4NodeI9JsonValueE, ptr %tuple.region, i32 0, i32 1
  store ptr null, ptr %tuple.field1, align 1
  call void @_ZN4ListI9JsonValueB4jsonE4linkEP4NodeI9JsonValueB4jsonE(ptr %0, ptr %tuple.region)
  ret void
}

define linkonce_odr void @_ZN4ListI9JsonValueB4jsonE6add_onER4Page9JsonValueB4json(ptr %0, ptr %1, ptr %2) {
entry:
  %tuple.region = call ptr @_ZN4Page8allocateEmm(ptr %1, i64 ptrtoint (ptr getelementptr (%_Z4NodeI9JsonValueE, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr ({ i1, %_Z4NodeI9JsonValueE }, ptr null, i64 0, i32 1) to i64))
  %field.load = load %_Z9JsonValue, ptr %2, align 1
  %tuple.field = getelementptr inbounds nuw %_Z4NodeI9JsonValueE, ptr %tuple.region, i32 0, i32 0
  store %_Z9JsonValue %field.load, ptr %tuple.field, align 1
  %tuple.field1 = getelementptr inbounds nuw %_Z4NodeI9JsonValueE, ptr %tuple.region, i32 0, i32 1
  store ptr null, ptr %tuple.field1, align 1
  call void @_ZN4ListI9JsonValueB4jsonE4linkEP4NodeI9JsonValueB4jsonE(ptr %0, ptr %tuple.region)
  ret void
}

define linkonce_odr void @_ZN6VectorI9JsonValueB4jsonEC1E4ListI9JsonValueB4jsonE(ptr %0, ptr %1) {
entry:
  %deref.tmp = alloca %_Z9JsonValue, align 8
  %i = alloca i64, align 8
  %list_iterator = alloca ptr, align 8
  %sret.result = alloca %_Z12ListIteratorI9JsonValueE, align 8
  %call = call ptr @_ZN4Page3getEPv(ptr %0)
  %call1 = call i64 @_ZN4ListI9JsonValueB4jsonE5countEv(ptr %1)
  %length = getelementptr inbounds nuw %_Z6VectorI9JsonValueE, ptr %0, i32 0, i32 0
  store i64 %call1, ptr %length, align 8
  %load.struct = load %_Z6VectorI9JsonValueE, ptr %0, align 8
  %length2 = extractvalue %_Z6VectorI9JsonValueE %load.struct, 0
  %gt = icmp ugt i64 %length2, 0
  br i1 %gt, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %load.struct3 = load %_Z6VectorI9JsonValueE, ptr %0, align 8
  %length4 = extractvalue %_Z6VectorI9JsonValueE %load.struct3, 0
  %mul = mul i64 %length4, ptrtoint (ptr getelementptr (%_Z9JsonValue, ptr null, i32 1) to i64)
  %call5 = call ptr @_ZN4Page8allocateEmm(ptr %call, i64 %mul, i64 8)
  %data = getelementptr inbounds nuw %_Z6VectorI9JsonValueE, ptr %0, i32 0, i32 1
  store ptr %call5, ptr %data, align 8
  call void @_ZN4ListI9JsonValueB4jsonE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z12ListIteratorI9JsonValueE) %sret.result, ptr null, ptr %1)
  store ptr %sret.result, ptr %list_iterator, align 1
  store i64 0, ptr %i, align 1
  br label %while.cond

if.else:                                          ; preds = %entry
  %data12 = getelementptr inbounds nuw %_Z6VectorI9JsonValueE, ptr %0, i32 0, i32 1
  store ptr null, ptr %data12, align 8
  br label %if.end

if.end:                                           ; preds = %if.else, %while.exit
  ret void

while.cond:                                       ; preds = %while.body, %if.then
  %list_iterator6 = load ptr, ptr %list_iterator, align 8
  %call7 = call ptr @_ZN12ListIteratorI9JsonValueB4jsonE4nextEv(ptr %list_iterator6)
  %while.tobool = icmp ne ptr %call7, null
  br i1 %while.tobool, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %deref.tmp, ptr align 1 %call7, i64 ptrtoint (ptr getelementptr (%_Z9JsonValue, ptr null, i32 1) to i64), i1 false)
  %load.struct8 = load %_Z6VectorI9JsonValueE, ptr %0, align 8
  %data9 = extractvalue %_Z6VectorI9JsonValueE %load.struct8, 1
  %i10 = load i64, ptr %i, align 8
  %ptr.add = getelementptr inbounds %_Z9JsonValue, ptr %data9, i64 %i10
  %store.load = load %_Z9JsonValue, ptr %deref.tmp, align 1
  store %_Z9JsonValue %store.load, ptr %ptr.add, align 1
  %i11 = load i64, ptr %i, align 8
  %add = add i64 %i11, 1
  store i64 %add, ptr %i, align 1
  br label %while.cond

while.exit:                                       ; preds = %while.cond
  br label %if.end
}

define linkonce_odr void @_ZN5ArrayI9JsonValueB4jsonE7add_runEmP9JsonValueB4json(ptr %0, i64 %1, ptr %2) {
entry:
  %load.struct = load %_Z5ArrayI9JsonValueE, ptr %0, align 8
  %length = extractvalue %_Z5ArrayI9JsonValueE %load.struct, 0
  %add = add i64 %length, %1
  %new_length = alloca i64, align 8
  store i64 %add, ptr %new_length, align 1
  %new_length1 = load i64, ptr %new_length, align 8
  %load.struct2 = load %_Z5ArrayI9JsonValueE, ptr %0, align 8
  %length3 = extractvalue %_Z5ArrayI9JsonValueE %load.struct2, 0
  %lt = icmp ult i64 %new_length1, %length3
  br i1 %lt, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %field.inplace = getelementptr inbounds nuw %_Z5ArrayI9JsonValueE, ptr %0, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  call void @_Z16scaly_panic_sizeP10const_charmm(ptr @.str.37, i64 %1, i64 %field.val)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %load.struct6 = load %_Z5ArrayI9JsonValueE, ptr %0, align 8
  %buffer = extractvalue %_Z5ArrayI9JsonValueE %load.struct6, 2
  %eq = icmp eq ptr %buffer, null
  br i1 %eq, label %if.then4, label %lor.rhs

if.then4:                                         ; preds = %lor.rhs, %if.end
  %new_length9 = load i64, ptr %new_length, align 8
  call void @_ZN5ArrayI9JsonValueB4jsonE7grow_toEm(ptr %0, i64 %new_length9)
  br label %if.end5

if.end5:                                          ; preds = %if.then4, %lor.rhs
  %gt10 = icmp ugt i64 %1, 0
  br i1 %gt10, label %if.then11, label %if.end12

lor.rhs:                                          ; preds = %if.end
  %new_length7 = load i64, ptr %new_length, align 8
  %load.struct8 = load %_Z5ArrayI9JsonValueE, ptr %0, align 8
  %capacity = extractvalue %_Z5ArrayI9JsonValueE %load.struct8, 1
  %gt = icmp ugt i64 %new_length7, %capacity
  br i1 %gt, label %if.then4, label %if.end5

if.then11:                                        ; preds = %if.end5
  %load.struct13 = load %_Z5ArrayI9JsonValueE, ptr %0, align 8
  %buffer14 = extractvalue %_Z5ArrayI9JsonValueE %load.struct13, 2
  %load.struct15 = load %_Z5ArrayI9JsonValueE, ptr %0, align 8
  %length16 = extractvalue %_Z5ArrayI9JsonValueE %load.struct15, 0
  %ptr.add = getelementptr inbounds %_Z9JsonValue, ptr %buffer14, i64 %length16
  %mul = mul i64 %1, ptrtoint (ptr getelementptr (%_Z9JsonValue, ptr null, i32 1) to i64)
  %call = call ptr @memcpy(ptr %ptr.add, ptr %2, i64 %mul)
  br label %if.end12

if.end12:                                         ; preds = %if.then11, %if.end5
  %load.struct17 = load %_Z5ArrayI9JsonValueE, ptr %0, align 8
  %length18 = extractvalue %_Z5ArrayI9JsonValueE %load.struct17, 0
  %add19 = add i64 %length18, %1
  %length20 = getelementptr inbounds nuw %_Z5ArrayI9JsonValueE, ptr %0, i32 0, i32 0
  store i64 %add19, ptr %length20, align 8
  ret void
}

define linkonce_odr void @_ZN5ArrayI9JsonValueB4jsonE3addE6VectorI9JsonValueB4jsonE(ptr %0, ptr %1) {
entry:
  %field.inplace = getelementptr inbounds nuw %_Z6VectorI9JsonValueE, ptr %1, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  %field.inplace1 = getelementptr inbounds nuw %_Z6VectorI9JsonValueE, ptr %1, i32 0, i32 1
  %deref.recv = load ptr, ptr %field.inplace1, align 8
  call void @_ZN5ArrayI9JsonValueB4jsonE7add_runEmP9JsonValueB4json(ptr %0, i64 %field.val, ptr %deref.recv)
  ret void
}

define linkonce_odr void @_ZN5ArrayI9JsonValueB4jsonE3addE5SliceI9JsonValueB4jsonE(ptr %0, ptr %1) {
entry:
  %field.inplace = getelementptr inbounds nuw %_Z5SliceI9JsonValueE, ptr %1, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  %field.inplace1 = getelementptr inbounds nuw %_Z5SliceI9JsonValueE, ptr %1, i32 0, i32 1
  %deref.recv = load ptr, ptr %field.inplace1, align 8
  call void @_ZN5ArrayI9JsonValueB4jsonE7add_runEmP9JsonValueB4json(ptr %0, i64 %field.val, ptr %deref.recv)
  ret void
}

define linkonce_odr void @_ZN5ArrayI9JsonValueB4jsonE7grow_toEm(ptr %0, i64 %1) {
entry:
  call void @_ZN5ArrayI9JsonValueB4jsonE10reallocateEv(ptr %0)
  %load.struct = load %_Z5ArrayI9JsonValueE, ptr %0, align 8
  %capacity = extractvalue %_Z5ArrayI9JsonValueE %load.struct, 1
  %gt = icmp ugt i64 %1, %capacity
  br i1 %gt, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %call = call ptr @_ZN4Page3getEPv(ptr %0)
  %load.struct1 = load %_Z5ArrayI9JsonValueE, ptr %0, align 8
  %buffer = extractvalue %_Z5ArrayI9JsonValueE %load.struct1, 2
  %load.struct2 = load %_Z5ArrayI9JsonValueE, ptr %0, align 8
  %capacity3 = extractvalue %_Z5ArrayI9JsonValueE %load.struct2, 1
  %mul = mul i64 %capacity3, ptrtoint (ptr getelementptr (%_Z9JsonValue, ptr null, i32 1) to i64)
  %le = icmp ule i64 %mul, 1024
  %mul4 = mul i64 %1, ptrtoint (ptr getelementptr (%_Z9JsonValue, ptr null, i32 1) to i64)
  %le5 = icmp ule i64 %mul4, 1024
  br i1 %le5, label %if.then6, label %if.else

if.end:                                           ; preds = %if.end24, %entry
  ret void

if.then6:                                         ; preds = %if.then
  %mul8 = mul i64 %1, ptrtoint (ptr getelementptr (%_Z9JsonValue, ptr null, i32 1) to i64)
  %call9 = call ptr @_ZN4Page8allocateEmm(ptr %call, i64 %mul8, i64 8)
  %buffer10 = getelementptr inbounds nuw %_Z5ArrayI9JsonValueE, ptr %0, i32 0, i32 2
  store ptr %call9, ptr %buffer10, align 8
  br label %if.end7

if.else:                                          ; preds = %if.then
  %mul11 = mul i64 %1, ptrtoint (ptr getelementptr (%_Z9JsonValue, ptr null, i32 1) to i64)
  %call12 = call ptr @_ZN4Page25allocate_exclusive_bufferEmm(ptr %call, i64 %mul11, i64 8)
  %buffer13 = getelementptr inbounds nuw %_Z5ArrayI9JsonValueE, ptr %0, i32 0, i32 2
  store ptr %call12, ptr %buffer13, align 8
  br label %if.end7

if.end7:                                          ; preds = %if.else, %if.then6
  %if.value = phi ptr [ %call9, %if.then6 ], [ %call12, %if.else ]
  %capacity14 = getelementptr inbounds nuw %_Z5ArrayI9JsonValueE, ptr %0, i32 0, i32 1
  store i64 %1, ptr %capacity14, align 8
  %load.struct15 = load %_Z5ArrayI9JsonValueE, ptr %0, align 8
  %length = extractvalue %_Z5ArrayI9JsonValueE %load.struct15, 0
  %gt16 = icmp ugt i64 %length, 0
  br i1 %gt16, label %if.then17, label %if.end18

if.then17:                                        ; preds = %if.end7
  %field.inplace = getelementptr inbounds nuw %_Z5ArrayI9JsonValueE, ptr %0, i32 0, i32 2
  %deref.recv = load ptr, ptr %field.inplace, align 8
  %load.struct19 = load %_Z5ArrayI9JsonValueE, ptr %0, align 8
  %length20 = extractvalue %_Z5ArrayI9JsonValueE %load.struct19, 0
  %mul21 = mul i64 %length20, ptrtoint (ptr getelementptr (%_Z9JsonValue, ptr null, i32 1) to i64)
  %call22 = call ptr @memcpy(ptr %deref.recv, ptr %buffer, i64 %mul21)
  br label %if.end18

if.end18:                                         ; preds = %if.then17, %if.end7
  %eq = icmp eq i1 %le, false
  br i1 %eq, label %if.then23, label %if.end24

if.then23:                                        ; preds = %if.end18
  %call25 = call ptr @_ZN4Page3getEPv(ptr %buffer)
  call void @_ZN4Page25deallocate_exclusive_pageER4Page(ptr %call, ptr %call25)
  br label %if.end24

if.end24:                                         ; preds = %if.then23, %if.end18
  br label %if.end
}

define linkonce_odr void @_ZN5ArrayI9JsonValueB4jsonE6extendEm(ptr noalias sret(%_Z5SliceI9JsonValueE) %0, ptr %1, i64 %2) {
entry:
  %tuple = alloca %_Z5SliceI9JsonValueE, align 8
  %load.struct = load %_Z5ArrayI9JsonValueE, ptr %1, align 8
  %length = extractvalue %_Z5ArrayI9JsonValueE %load.struct, 0
  %add = add i64 %length, %2
  %lt = icmp ult i64 %add, %length
  br i1 %lt, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  call void @_Z16scaly_panic_sizeP10const_charmm(ptr @.str.38, i64 %2, i64 %length)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %load.struct3 = load %_Z5ArrayI9JsonValueE, ptr %1, align 8
  %buffer = extractvalue %_Z5ArrayI9JsonValueE %load.struct3, 2
  %eq = icmp eq ptr %buffer, null
  br i1 %eq, label %if.then1, label %lor.rhs

if.then1:                                         ; preds = %lor.rhs, %if.end
  call void @_ZN5ArrayI9JsonValueB4jsonE7grow_toEm(ptr %1, i64 %add)
  br label %if.end2

if.end2:                                          ; preds = %if.then1, %lor.rhs
  %length5 = getelementptr inbounds nuw %_Z5ArrayI9JsonValueE, ptr %1, i32 0, i32 0
  store i64 %add, ptr %length5, align 8
  %load.struct6 = load %_Z5ArrayI9JsonValueE, ptr %1, align 8
  %buffer7 = extractvalue %_Z5ArrayI9JsonValueE %load.struct6, 2
  %ptr.add = getelementptr inbounds %_Z9JsonValue, ptr %buffer7, i64 %length
  %tuple.field = getelementptr inbounds nuw %_Z5SliceI9JsonValueE, ptr %tuple, i32 0, i32 0
  store i64 %2, ptr %tuple.field, align 1
  %tuple.field8 = getelementptr inbounds nuw %_Z5SliceI9JsonValueE, ptr %tuple, i32 0, i32 1
  store ptr %ptr.add, ptr %tuple.field8, align 1
  %tuple.val = load %_Z5SliceI9JsonValueE, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z5SliceI9JsonValueE, ptr null, i32 1) to i64), i1 false)
  ret void

lor.rhs:                                          ; preds = %if.end
  %load.struct4 = load %_Z5ArrayI9JsonValueE, ptr %1, align 8
  %capacity = extractvalue %_Z5ArrayI9JsonValueE %load.struct4, 1
  %gt = icmp ugt i64 %add, %capacity
  br i1 %gt, label %if.then1, label %if.end2
}

define linkonce_odr ptr @_ZN5ArrayI9JsonValueB4jsonE3getEm(ptr %0, i64 %1) {
entry:
  %load.struct = load %_Z5ArrayI9JsonValueE, ptr %0, align 8
  %length = extractvalue %_Z5ArrayI9JsonValueE %load.struct, 0
  %ge = icmp uge i64 %1, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %load.struct1 = load %_Z5ArrayI9JsonValueE, ptr %0, align 8
  %buffer = extractvalue %_Z5ArrayI9JsonValueE %load.struct1, 2
  %ptr.add = getelementptr inbounds %_Z9JsonValue, ptr %buffer, i64 %1
  ret ptr %ptr.add
}

define linkonce_odr ptr @_ZN5ArrayI9JsonValueB4jsonE2atEm(ptr %0, i64 %1) {
entry:
  %load.struct = load %_Z5ArrayI9JsonValueE, ptr %0, align 8
  %length = extractvalue %_Z5ArrayI9JsonValueE %load.struct, 0
  %ge = icmp uge i64 %1, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %field.inplace = getelementptr inbounds nuw %_Z5ArrayI9JsonValueE, ptr %0, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  call void @_Z15scaly_panic_oobP10const_charmm(ptr @.str.39, i64 %1, i64 %field.val)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %load.struct1 = load %_Z5ArrayI9JsonValueE, ptr %0, align 8
  %buffer = extractvalue %_Z5ArrayI9JsonValueE %load.struct1, 2
  %ptr.add = getelementptr inbounds %_Z9JsonValue, ptr %buffer, i64 %1
  ret ptr %ptr.add
}

define linkonce_odr void @_ZN5ArrayI9JsonValueB4jsonE5clearEv(ptr %0) {
entry:
  %length = getelementptr inbounds nuw %_Z5ArrayI9JsonValueE, ptr %0, i32 0, i32 0
  store i64 0, ptr %length, align 8
  ret void
}

define linkonce_odr void @_ZN5ArrayI9JsonValueB4jsonE3putEm9JsonValueB4json(ptr %0, i64 %1, ptr %2) {
entry:
  %load.struct = load %_Z5ArrayI9JsonValueE, ptr %0, align 8
  %length = extractvalue %_Z5ArrayI9JsonValueE %load.struct, 0
  %ge = icmp uge i64 %1, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %field.inplace = getelementptr inbounds nuw %_Z5ArrayI9JsonValueE, ptr %0, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  call void @_Z15scaly_panic_oobP10const_charmm(ptr @.str.40, i64 %1, i64 %field.val)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %load.struct1 = load %_Z5ArrayI9JsonValueE, ptr %0, align 8
  %buffer = extractvalue %_Z5ArrayI9JsonValueE %load.struct1, 2
  %ptr.add = getelementptr inbounds %_Z9JsonValue, ptr %buffer, i64 %1
  %store.load = load %_Z9JsonValue, ptr %2, align 1
  store %_Z9JsonValue %store.load, ptr %ptr.add, align 1
  ret void
}

define linkonce_odr ptr @_ZN13ArrayIteratorI9JsonValueB4jsonE4nextEv(ptr %0) {
entry:
  %load.struct = load %_Z13ArrayIteratorI9JsonValueE, ptr %0, align 8
  %array = extractvalue %_Z13ArrayIteratorI9JsonValueE %load.struct, 0
  %eq = icmp eq ptr %array, null
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %load.struct1 = load %_Z13ArrayIteratorI9JsonValueE, ptr %0, align 8
  %position = extractvalue %_Z13ArrayIteratorI9JsonValueE %load.struct1, 1
  %load.struct2 = load %_Z13ArrayIteratorI9JsonValueE, ptr %0, align 8
  %array3 = extractvalue %_Z13ArrayIteratorI9JsonValueE %load.struct2, 0
  %deref = load %_Z5ArrayI9JsonValueE, ptr %array3, align 8
  %length = extractvalue %_Z5ArrayI9JsonValueE %deref, 0
  %eq4 = icmp eq i64 %position, %length
  br i1 %eq4, label %if.then5, label %if.end6

if.then5:                                         ; preds = %if.end
  ret ptr null

if.end6:                                          ; preds = %if.end
  %load.struct7 = load %_Z13ArrayIteratorI9JsonValueE, ptr %0, align 8
  %position8 = extractvalue %_Z13ArrayIteratorI9JsonValueE %load.struct7, 1
  %add = add i64 %position8, 1
  %position9 = getelementptr inbounds nuw %_Z13ArrayIteratorI9JsonValueE, ptr %0, i32 0, i32 1
  store i64 %add, ptr %position9, align 8
  %field.inplace = getelementptr inbounds nuw %_Z13ArrayIteratorI9JsonValueE, ptr %0, i32 0, i32 0
  %deref.recv = load ptr, ptr %field.inplace, align 8
  %call = call ptr @_ZN5ArrayI9JsonValueB4jsonE10get_bufferEv(ptr %deref.recv)
  %load.struct10 = load %_Z13ArrayIteratorI9JsonValueE, ptr %0, align 8
  %position11 = extractvalue %_Z13ArrayIteratorI9JsonValueE %load.struct10, 1
  %sub = sub i64 %position11, 1
  %ptr.add = getelementptr inbounds %_Z9JsonValue, ptr %call, i64 %sub
  ret ptr %ptr.add
}

define linkonce_odr void @_ZN13ArrayIteratorI9JsonValueB4jsonEC1E6OptionIR5ArrayI9JsonValueB4jsonEE(ptr %0, ptr %1) {
entry:
  %array = getelementptr inbounds nuw %_Z13ArrayIteratorI9JsonValueE, ptr %0, i32 0, i32 0
  store ptr %1, ptr %array, align 8
  %position = getelementptr inbounds nuw %_Z13ArrayIteratorI9JsonValueE, ptr %0, i32 0, i32 1
  store i64 0, ptr %position, align 8
  ret void
}

define linkonce_odr void @_ZN5ArrayI9JsonValueB4jsonE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z13ArrayIteratorI9JsonValueE) %0, ptr %1, ptr %2) {
entry:
  %struct.init = alloca %_Z13ArrayIteratorI9JsonValueE, align 8
  call void @_ZN13ArrayIteratorI9JsonValueB4jsonEC1E6OptionIR5ArrayI9JsonValueB4jsonEE(ptr %struct.init, ptr %2)
  %sret.body = load %_Z13ArrayIteratorI9JsonValueE, ptr %struct.init, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %struct.init, i64 ptrtoint (ptr getelementptr (%_Z13ArrayIteratorI9JsonValueE, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN5ArrayI9JsonValueB4jsonE8as_sliceEv(ptr noalias sret(%_Z5SliceI9JsonValueE) %0, ptr %1) {
entry:
  %load.struct = load %_Z5ArrayI9JsonValueE, ptr %1, align 8
  %length = extractvalue %_Z5ArrayI9JsonValueE %load.struct, 0
  %call = call ptr @_ZN5ArrayI9JsonValueB4jsonE10get_bufferEv(ptr %1)
  %tuple = alloca %_Z5SliceI9JsonValueE, align 8
  %tuple.field = getelementptr inbounds nuw %_Z5SliceI9JsonValueE, ptr %tuple, i32 0, i32 0
  store i64 %length, ptr %tuple.field, align 1
  %tuple.field1 = getelementptr inbounds nuw %_Z5SliceI9JsonValueE, ptr %tuple, i32 0, i32 1
  store ptr %call, ptr %tuple.field1, align 1
  %tuple.val = load %_Z5SliceI9JsonValueE, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z5SliceI9JsonValueE, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN5ArrayI9JsonValueB4jsonEC1Ev(ptr %0) {
entry:
  %length = getelementptr inbounds nuw %_Z5ArrayI9JsonValueE, ptr %0, i32 0, i32 0
  store i64 0, ptr %length, align 8
  %capacity = getelementptr inbounds nuw %_Z5ArrayI9JsonValueE, ptr %0, i32 0, i32 1
  store i64 0, ptr %capacity, align 8
  %buffer = getelementptr inbounds nuw %_Z5ArrayI9JsonValueE, ptr %0, i32 0, i32 2
  store ptr null, ptr %buffer, align 8
  ret void
}

define linkonce_odr void @_ZN5ArrayI9JsonValueB4jsonEC1Em(ptr %0, i64 %1) {
entry:
  %length = getelementptr inbounds nuw %_Z5ArrayI9JsonValueE, ptr %0, i32 0, i32 0
  store i64 0, ptr %length, align 8
  %capacity = getelementptr inbounds nuw %_Z5ArrayI9JsonValueE, ptr %0, i32 0, i32 1
  store i64 0, ptr %capacity, align 8
  %buffer = getelementptr inbounds nuw %_Z5ArrayI9JsonValueE, ptr %0, i32 0, i32 2
  store ptr null, ptr %buffer, align 8
  %gt = icmp ugt i64 %1, 0
  br i1 %gt, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %call = call ptr @_ZN4Page3getEPv(ptr %0)
  %capacity1 = getelementptr inbounds nuw %_Z5ArrayI9JsonValueE, ptr %0, i32 0, i32 1
  store i64 %1, ptr %capacity1, align 8
  %mul = mul i64 %1, ptrtoint (ptr getelementptr (%_Z9JsonValue, ptr null, i32 1) to i64)
  %le = icmp ule i64 %mul, 1024
  br i1 %le, label %if.then2, label %if.else

if.end:                                           ; preds = %if.end3, %entry
  ret void

if.then2:                                         ; preds = %if.then
  %mul4 = mul i64 %1, ptrtoint (ptr getelementptr (%_Z9JsonValue, ptr null, i32 1) to i64)
  %call5 = call ptr @_ZN4Page8allocateEmm(ptr %call, i64 %mul4, i64 8)
  %buffer6 = getelementptr inbounds nuw %_Z5ArrayI9JsonValueE, ptr %0, i32 0, i32 2
  store ptr %call5, ptr %buffer6, align 8
  br label %if.end3

if.else:                                          ; preds = %if.then
  %mul7 = mul i64 %1, ptrtoint (ptr getelementptr (%_Z9JsonValue, ptr null, i32 1) to i64)
  %call8 = call ptr @_ZN4Page25allocate_exclusive_bufferEmm(ptr %call, i64 %mul7, i64 8)
  %buffer9 = getelementptr inbounds nuw %_Z5ArrayI9JsonValueE, ptr %0, i32 0, i32 2
  store ptr %call8, ptr %buffer9, align 8
  br label %if.end3

if.end3:                                          ; preds = %if.else, %if.then2
  %if.value = phi ptr [ %call5, %if.then2 ], [ %call8, %if.else ]
  br label %if.end
}

define linkonce_odr ptr @_ZN5ArrayI10JsonMemberB4jsonE10get_bufferEv(ptr %0) {
entry:
  %load.struct = load %_Z5ArrayI10JsonMemberE, ptr %0, align 8
  %buffer = extractvalue %_Z5ArrayI10JsonMemberE %load.struct, 2
  ret ptr %buffer
}

define linkonce_odr i64 @_ZN5ArrayI10JsonMemberB4jsonE10get_lengthEv(ptr %0) {
entry:
  %load.struct = load %_Z5ArrayI10JsonMemberE, ptr %0, align 8
  %length = extractvalue %_Z5ArrayI10JsonMemberE %load.struct, 0
  ret i64 %length
}

define linkonce_odr i64 @_ZN5ArrayI10JsonMemberB4jsonE12get_capacityEv(ptr %0) {
entry:
  %load.struct = load %_Z5ArrayI10JsonMemberE, ptr %0, align 8
  %capacity = extractvalue %_Z5ArrayI10JsonMemberE %load.struct, 1
  ret i64 %capacity
}

define linkonce_odr void @_ZN5ArrayI10JsonMemberB4jsonE10reallocateEv(ptr %0) {
entry:
  %first_cap = alloca i64, align 8
  %call = call ptr @_ZN4Page3getEPv(ptr %0)
  %load.struct = load %_Z5ArrayI10JsonMemberE, ptr %0, align 8
  %buffer = extractvalue %_Z5ArrayI10JsonMemberE %load.struct, 2
  %eq = icmp eq ptr %buffer, null
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %udiv = udiv i64 32, ptrtoint (ptr getelementptr (%_Z10JsonMember, ptr null, i32 1) to i64)
  store i64 %udiv, ptr %first_cap, align 1
  %first_cap1 = load i64, ptr %first_cap, align 8
  %lt = icmp ult i64 %first_cap1, 1
  br i1 %lt, label %if.then2, label %if.end3

if.end:                                           ; preds = %entry
  %load.struct18 = load %_Z5ArrayI10JsonMemberE, ptr %0, align 8
  %capacity19 = extractvalue %_Z5ArrayI10JsonMemberE %load.struct18, 1
  %mul20 = mul i64 %capacity19, ptrtoint (ptr getelementptr (%_Z10JsonMember, ptr null, i32 1) to i64)
  %le21 = icmp ule i64 %mul20, 1024
  %load.struct22 = load %_Z5ArrayI10JsonMemberE, ptr %0, align 8
  %buffer23 = extractvalue %_Z5ArrayI10JsonMemberE %load.struct22, 2
  %load.struct24 = load %_Z5ArrayI10JsonMemberE, ptr %0, align 8
  %capacity25 = extractvalue %_Z5ArrayI10JsonMemberE %load.struct24, 1
  %load.struct26 = load %_Z5ArrayI10JsonMemberE, ptr %0, align 8
  %capacity27 = extractvalue %_Z5ArrayI10JsonMemberE %load.struct26, 1
  %mul28 = mul i64 %capacity27, 2
  store i64 %mul28, ptr %first_cap, align 1
  %new_capacity = load i64, ptr %first_cap, align 8
  %mul29 = mul i64 %new_capacity, ptrtoint (ptr getelementptr (%_Z10JsonMember, ptr null, i32 1) to i64)
  %le30 = icmp ule i64 %mul29, 1024
  br i1 %le30, label %if.then31, label %if.end32

if.then2:                                         ; preds = %if.then
  store i64 1, ptr %first_cap, align 1
  br label %if.end3

if.end3:                                          ; preds = %if.then2, %if.then
  %first_cap4 = load i64, ptr %first_cap, align 8
  %mul = mul i64 %first_cap4, ptrtoint (ptr getelementptr (%_Z10JsonMember, ptr null, i32 1) to i64)
  %le = icmp ule i64 %mul, 1024
  br i1 %le, label %if.then5, label %if.end6

if.then5:                                         ; preds = %if.end3
  %first_cap7 = load i64, ptr %first_cap, align 8
  %mul8 = mul i64 %first_cap7, ptrtoint (ptr getelementptr (%_Z10JsonMember, ptr null, i32 1) to i64)
  %call9 = call ptr @_ZN4Page8allocateEmm(ptr %call, i64 %mul8, i64 8)
  %buffer10 = getelementptr inbounds nuw %_Z5ArrayI10JsonMemberE, ptr %0, i32 0, i32 2
  store ptr %call9, ptr %buffer10, align 8
  %first_cap11 = load i64, ptr %first_cap, align 8
  %capacity = getelementptr inbounds nuw %_Z5ArrayI10JsonMemberE, ptr %0, i32 0, i32 1
  store i64 %first_cap11, ptr %capacity, align 8
  ret void

if.end6:                                          ; preds = %if.end3
  %first_cap12 = load i64, ptr %first_cap, align 8
  %mul13 = mul i64 %first_cap12, ptrtoint (ptr getelementptr (%_Z10JsonMember, ptr null, i32 1) to i64)
  %call14 = call ptr @_ZN4Page25allocate_exclusive_bufferEmm(ptr %call, i64 %mul13, i64 8)
  %buffer15 = getelementptr inbounds nuw %_Z5ArrayI10JsonMemberE, ptr %0, i32 0, i32 2
  store ptr %call14, ptr %buffer15, align 8
  %first_cap16 = load i64, ptr %first_cap, align 8
  %capacity17 = getelementptr inbounds nuw %_Z5ArrayI10JsonMemberE, ptr %0, i32 0, i32 1
  store i64 %first_cap16, ptr %capacity17, align 8
  ret void

if.then31:                                        ; preds = %if.end
  %new_capacity33 = load i64, ptr %first_cap, align 8
  %mul34 = mul i64 %new_capacity33, ptrtoint (ptr getelementptr (%_Z10JsonMember, ptr null, i32 1) to i64)
  %call35 = call ptr @_ZN4Page8allocateEmm(ptr %call, i64 %mul34, i64 8)
  %buffer36 = getelementptr inbounds nuw %_Z5ArrayI10JsonMemberE, ptr %0, i32 0, i32 2
  store ptr %call35, ptr %buffer36, align 8
  %new_capacity37 = load i64, ptr %first_cap, align 8
  %capacity38 = getelementptr inbounds nuw %_Z5ArrayI10JsonMemberE, ptr %0, i32 0, i32 1
  store i64 %new_capacity37, ptr %capacity38, align 8
  %field.inplace = getelementptr inbounds nuw %_Z5ArrayI10JsonMemberE, ptr %0, i32 0, i32 2
  %deref.recv = load ptr, ptr %field.inplace, align 8
  %mul39 = mul i64 %capacity25, ptrtoint (ptr getelementptr (%_Z10JsonMember, ptr null, i32 1) to i64)
  %call40 = call ptr @memcpy(ptr %deref.recv, ptr %buffer23, i64 %mul39)
  ret void

if.end32:                                         ; preds = %if.end
  %call41 = call ptr @_ZN4Page23allocate_exclusive_pageEv(ptr %call)
  %call42 = call i64 @_ZN4Page12get_capacityEm(ptr %call41, i64 8)
  call void @_ZN4Page25deallocate_exclusive_pageER4Page(ptr %call, ptr %call41)
  %new_capacity43 = load i64, ptr %first_cap, align 8
  %udiv44 = udiv i64 %call42, ptrtoint (ptr getelementptr (%_Z10JsonMember, ptr null, i32 1) to i64)
  %lt45 = icmp ult i64 %new_capacity43, %udiv44
  br i1 %lt45, label %if.then46, label %if.end47

if.then46:                                        ; preds = %if.end32
  %udiv48 = udiv i64 %call42, ptrtoint (ptr getelementptr (%_Z10JsonMember, ptr null, i32 1) to i64)
  store i64 %udiv48, ptr %first_cap, align 1
  br label %if.end47

if.end47:                                         ; preds = %if.then46, %if.end32
  %new_capacity49 = load i64, ptr %first_cap, align 8
  %mul50 = mul i64 %new_capacity49, ptrtoint (ptr getelementptr (%_Z10JsonMember, ptr null, i32 1) to i64)
  %call51 = call ptr @_ZN4Page25allocate_exclusive_bufferEmm(ptr %call, i64 %mul50, i64 8)
  %buffer52 = getelementptr inbounds nuw %_Z5ArrayI10JsonMemberE, ptr %0, i32 0, i32 2
  store ptr %call51, ptr %buffer52, align 8
  %new_capacity53 = load i64, ptr %first_cap, align 8
  %capacity54 = getelementptr inbounds nuw %_Z5ArrayI10JsonMemberE, ptr %0, i32 0, i32 1
  store i64 %new_capacity53, ptr %capacity54, align 8
  %field.inplace55 = getelementptr inbounds nuw %_Z5ArrayI10JsonMemberE, ptr %0, i32 0, i32 2
  %deref.recv56 = load ptr, ptr %field.inplace55, align 8
  %mul57 = mul i64 %capacity25, ptrtoint (ptr getelementptr (%_Z10JsonMember, ptr null, i32 1) to i64)
  %call58 = call ptr @memcpy(ptr %deref.recv56, ptr %buffer23, i64 %mul57)
  %eq59 = icmp eq i1 %le21, false
  br i1 %eq59, label %if.then60, label %if.end61

if.then60:                                        ; preds = %if.end47
  %call62 = call ptr @_ZN4Page3getEPv(ptr %buffer23)
  call void @_ZN4Page25deallocate_exclusive_pageER4Page(ptr %call, ptr %call62)
  br label %if.end61

if.end61:                                         ; preds = %if.then60, %if.end47
  ret void
}

define linkonce_odr void @_ZN5ArrayI10JsonMemberB4jsonE3addE10JsonMemberB4json(ptr %0, ptr %1) {
entry:
  %load.struct = load %_Z5ArrayI10JsonMemberE, ptr %0, align 8
  %buffer = extractvalue %_Z5ArrayI10JsonMemberE %load.struct, 2
  %eq = icmp eq ptr %buffer, null
  br i1 %eq, label %if.then, label %lor.rhs

if.then:                                          ; preds = %lor.rhs, %entry
  call void @_ZN5ArrayI10JsonMemberB4jsonE10reallocateEv(ptr %0)
  br label %if.end

if.end:                                           ; preds = %if.then, %lor.rhs
  %load.struct4 = load %_Z5ArrayI10JsonMemberE, ptr %0, align 8
  %buffer5 = extractvalue %_Z5ArrayI10JsonMemberE %load.struct4, 2
  %load.struct6 = load %_Z5ArrayI10JsonMemberE, ptr %0, align 8
  %length7 = extractvalue %_Z5ArrayI10JsonMemberE %load.struct6, 0
  %ptr.add = getelementptr inbounds %_Z10JsonMember, ptr %buffer5, i64 %length7
  %store.load = load %_Z10JsonMember, ptr %1, align 8
  store %_Z10JsonMember %store.load, ptr %ptr.add, align 8
  %load.struct8 = load %_Z5ArrayI10JsonMemberE, ptr %0, align 8
  %length9 = extractvalue %_Z5ArrayI10JsonMemberE %load.struct8, 0
  %add = add i64 %length9, 1
  %length10 = getelementptr inbounds nuw %_Z5ArrayI10JsonMemberE, ptr %0, i32 0, i32 0
  store i64 %add, ptr %length10, align 8
  ret void

lor.rhs:                                          ; preds = %entry
  %load.struct1 = load %_Z5ArrayI10JsonMemberE, ptr %0, align 8
  %length = extractvalue %_Z5ArrayI10JsonMemberE %load.struct1, 0
  %load.struct2 = load %_Z5ArrayI10JsonMemberE, ptr %0, align 8
  %capacity = extractvalue %_Z5ArrayI10JsonMemberE %load.struct2, 1
  %eq3 = icmp eq i64 %length, %capacity
  br i1 %eq3, label %if.then, label %if.end
}

define linkonce_odr ptr @_ZN6VectorI10JsonMemberB4jsonE3getEm(ptr noalias %0, i64 %1) {
entry:
  %load.struct = load %_Z6VectorI10JsonMemberE, ptr %0, align 8
  %length = extractvalue %_Z6VectorI10JsonMemberE %load.struct, 0
  %ge = icmp uge i64 %1, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %load.struct1 = load %_Z6VectorI10JsonMemberE, ptr %0, align 8
  %data = extractvalue %_Z6VectorI10JsonMemberE %load.struct1, 1
  %ptr.add = getelementptr inbounds %_Z10JsonMember, ptr %data, i64 %1
  ret ptr %ptr.add
}

define linkonce_odr ptr @_ZN6VectorI10JsonMemberB4jsonE2atEm(ptr noalias %0, i64 %1) {
entry:
  %load.struct = load %_Z6VectorI10JsonMemberE, ptr %0, align 8
  %length = extractvalue %_Z6VectorI10JsonMemberE %load.struct, 0
  %ge = icmp uge i64 %1, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %field.inplace = getelementptr inbounds nuw %_Z6VectorI10JsonMemberE, ptr %0, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  call void @_Z15scaly_panic_oobP10const_charmm(ptr @.str.41, i64 %1, i64 %field.val)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %load.struct1 = load %_Z6VectorI10JsonMemberE, ptr %0, align 8
  %data = extractvalue %_Z6VectorI10JsonMemberE %load.struct1, 1
  %ptr.add = getelementptr inbounds %_Z10JsonMember, ptr %data, i64 %1
  ret ptr %ptr.add
}

define linkonce_odr ptr @_ZN6VectorI10JsonMemberB4jsonE7get_ptrEm(ptr noalias %0, i64 %1) {
entry:
  %load.struct = load %_Z6VectorI10JsonMemberE, ptr %0, align 8
  %length = extractvalue %_Z6VectorI10JsonMemberE %load.struct, 0
  %ge = icmp uge i64 %1, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %load.struct1 = load %_Z6VectorI10JsonMemberE, ptr %0, align 8
  %data = extractvalue %_Z6VectorI10JsonMemberE %load.struct1, 1
  %ptr.add = getelementptr inbounds %_Z10JsonMember, ptr %data, i64 %1
  ret ptr %ptr.add
}

define linkonce_odr void @_ZN6VectorI10JsonMemberB4jsonE3putEm10JsonMemberB4json(ptr noalias %0, i64 %1, ptr %2) {
entry:
  %load.struct = load %_Z6VectorI10JsonMemberE, ptr %0, align 8
  %length = extractvalue %_Z6VectorI10JsonMemberE %load.struct, 0
  %ge = icmp uge i64 %1, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %field.inplace = getelementptr inbounds nuw %_Z6VectorI10JsonMemberE, ptr %0, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  call void @_Z15scaly_panic_oobP10const_charmm(ptr @.str.42, i64 %1, i64 %field.val)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %load.struct1 = load %_Z6VectorI10JsonMemberE, ptr %0, align 8
  %data = extractvalue %_Z6VectorI10JsonMemberE %load.struct1, 1
  %ptr.add = getelementptr inbounds %_Z10JsonMember, ptr %data, i64 %1
  %store.load = load %_Z10JsonMember, ptr %2, align 8
  store %_Z10JsonMember %store.load, ptr %ptr.add, align 8
  ret void
}

define linkonce_odr ptr @_ZN14VectorIteratorI10JsonMemberB4jsonE4nextEv(ptr %0) {
entry:
  %load.struct = load %_Z14VectorIteratorI10JsonMemberE, ptr %0, align 8
  %vector = extractvalue %_Z14VectorIteratorI10JsonMemberE %load.struct, 0
  %eq = icmp eq ptr %vector, null
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %load.struct1 = load %_Z14VectorIteratorI10JsonMemberE, ptr %0, align 8
  %position = extractvalue %_Z14VectorIteratorI10JsonMemberE %load.struct1, 1
  %load.struct2 = load %_Z14VectorIteratorI10JsonMemberE, ptr %0, align 8
  %vector3 = extractvalue %_Z14VectorIteratorI10JsonMemberE %load.struct2, 0
  %deref = load %_Z6VectorI10JsonMemberE, ptr %vector3, align 8
  %length = extractvalue %_Z6VectorI10JsonMemberE %deref, 0
  %eq4 = icmp eq i64 %position, %length
  br i1 %eq4, label %if.then5, label %if.end6

if.then5:                                         ; preds = %if.end
  ret ptr null

if.end6:                                          ; preds = %if.end
  %load.struct7 = load %_Z14VectorIteratorI10JsonMemberE, ptr %0, align 8
  %position8 = extractvalue %_Z14VectorIteratorI10JsonMemberE %load.struct7, 1
  %add = add i64 %position8, 1
  %position9 = getelementptr inbounds nuw %_Z14VectorIteratorI10JsonMemberE, ptr %0, i32 0, i32 1
  store i64 %add, ptr %position9, align 8
  %field.inplace = getelementptr inbounds nuw %_Z14VectorIteratorI10JsonMemberE, ptr %0, i32 0, i32 0
  %deref.recv = load ptr, ptr %field.inplace, align 8
  %load.struct10 = load %_Z14VectorIteratorI10JsonMemberE, ptr %0, align 8
  %position11 = extractvalue %_Z14VectorIteratorI10JsonMemberE %load.struct10, 1
  %sub = sub i64 %position11, 1
  %call = call ptr @_ZN6VectorI10JsonMemberB4jsonE7get_ptrEm(ptr %deref.recv, i64 %sub)
  ret ptr %call
}

define linkonce_odr void @_ZN14VectorIteratorI10JsonMemberB4jsonEC1E6OptionIR6VectorI10JsonMemberB4jsonEE(ptr %0, ptr %1) {
entry:
  %vector = getelementptr inbounds nuw %_Z14VectorIteratorI10JsonMemberE, ptr %0, i32 0, i32 0
  store ptr %1, ptr %vector, align 8
  %position = getelementptr inbounds nuw %_Z14VectorIteratorI10JsonMemberE, ptr %0, i32 0, i32 1
  store i64 0, ptr %position, align 8
  ret void
}

define linkonce_odr void @_ZN6VectorI10JsonMemberB4jsonE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z14VectorIteratorI10JsonMemberE) %0, ptr %1, ptr noalias %2) {
entry:
  %struct.init = alloca %_Z14VectorIteratorI10JsonMemberE, align 8
  call void @_ZN14VectorIteratorI10JsonMemberB4jsonEC1E6OptionIR6VectorI10JsonMemberB4jsonEE(ptr %struct.init, ptr %2)
  %sret.body = load %_Z14VectorIteratorI10JsonMemberE, ptr %struct.init, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %struct.init, i64 ptrtoint (ptr getelementptr (%_Z14VectorIteratorI10JsonMemberE, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN6VectorI10JsonMemberB4jsonE8as_sliceEv(ptr noalias sret(%_Z5SliceI10JsonMemberE) %0, ptr noalias %1) {
entry:
  %load.struct = load %_Z6VectorI10JsonMemberE, ptr %1, align 8
  %length = extractvalue %_Z6VectorI10JsonMemberE %load.struct, 0
  %load.struct1 = load %_Z6VectorI10JsonMemberE, ptr %1, align 8
  %data = extractvalue %_Z6VectorI10JsonMemberE %load.struct1, 1
  %tuple = alloca %_Z5SliceI10JsonMemberE, align 8
  %tuple.field = getelementptr inbounds nuw %_Z5SliceI10JsonMemberE, ptr %tuple, i32 0, i32 0
  store i64 %length, ptr %tuple.field, align 1
  %tuple.field2 = getelementptr inbounds nuw %_Z5SliceI10JsonMemberE, ptr %tuple, i32 0, i32 1
  store ptr %data, ptr %tuple.field2, align 1
  %tuple.val = load %_Z5SliceI10JsonMemberE, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z5SliceI10JsonMemberE, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN6VectorI10JsonMemberB4jsonEC1Ev(ptr noalias %0) {
entry:
  %length = getelementptr inbounds nuw %_Z6VectorI10JsonMemberE, ptr %0, i32 0, i32 0
  store i64 0, ptr %length, align 8
  %data = getelementptr inbounds nuw %_Z6VectorI10JsonMemberE, ptr %0, i32 0, i32 1
  store ptr null, ptr %data, align 8
  ret void
}

define linkonce_odr void @_ZN6VectorI10JsonMemberB4jsonEC1Em(ptr %0, i64 %1) {
entry:
  %call = call ptr @_ZN4Page3getEPv(ptr %0)
  %length = getelementptr inbounds nuw %_Z6VectorI10JsonMemberE, ptr %0, i32 0, i32 0
  store i64 %1, ptr %length, align 8
  %gt = icmp ugt i64 %1, 0
  br i1 %gt, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %mul = mul i64 %1, ptrtoint (ptr getelementptr (%_Z10JsonMember, ptr null, i32 1) to i64)
  %call1 = call ptr @_ZN4Page8allocateEmm(ptr %call, i64 %mul, i64 8)
  %data = getelementptr inbounds nuw %_Z6VectorI10JsonMemberE, ptr %0, i32 0, i32 1
  store ptr %call1, ptr %data, align 8
  %field.inplace = getelementptr inbounds nuw %_Z6VectorI10JsonMemberE, ptr %0, i32 0, i32 1
  %deref.recv = load ptr, ptr %field.inplace, align 8
  %mul2 = mul i64 %1, ptrtoint (ptr getelementptr (%_Z10JsonMember, ptr null, i32 1) to i64)
  %call3 = call ptr @memset(ptr %deref.recv, i32 0, i64 %mul2)
  br label %if.end

if.else:                                          ; preds = %entry
  %data4 = getelementptr inbounds nuw %_Z6VectorI10JsonMemberE, ptr %0, i32 0, i32 1
  store ptr null, ptr %data4, align 8
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  %if.value = phi ptr [ %call3, %if.then ], [ null, %if.else ]
  ret void
}

define linkonce_odr void @_ZN6VectorI10JsonMemberB4jsonEC1E6VectorI10JsonMemberB4jsonE(ptr %0, ptr %1) {
entry:
  %call = call ptr @_ZN4Page3getEPv(ptr %0)
  %load.struct = load %_Z6VectorI10JsonMemberE, ptr %1, align 8
  %length = extractvalue %_Z6VectorI10JsonMemberE %load.struct, 0
  %length1 = getelementptr inbounds nuw %_Z6VectorI10JsonMemberE, ptr %0, i32 0, i32 0
  store i64 %length, ptr %length1, align 8
  %load.struct2 = load %_Z6VectorI10JsonMemberE, ptr %0, align 8
  %length3 = extractvalue %_Z6VectorI10JsonMemberE %load.struct2, 0
  %gt = icmp ugt i64 %length3, 0
  br i1 %gt, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %load.struct4 = load %_Z6VectorI10JsonMemberE, ptr %0, align 8
  %length5 = extractvalue %_Z6VectorI10JsonMemberE %load.struct4, 0
  %mul = mul i64 %length5, ptrtoint (ptr getelementptr (%_Z10JsonMember, ptr null, i32 1) to i64)
  %call6 = call ptr @_ZN4Page8allocateEmm(ptr %call, i64 %mul, i64 8)
  %data = getelementptr inbounds nuw %_Z6VectorI10JsonMemberE, ptr %0, i32 0, i32 1
  store ptr %call6, ptr %data, align 8
  %field.inplace = getelementptr inbounds nuw %_Z6VectorI10JsonMemberE, ptr %0, i32 0, i32 1
  %deref.recv = load ptr, ptr %field.inplace, align 8
  %field.inplace7 = getelementptr inbounds nuw %_Z6VectorI10JsonMemberE, ptr %1, i32 0, i32 1
  %deref.recv8 = load ptr, ptr %field.inplace7, align 8
  %load.struct9 = load %_Z6VectorI10JsonMemberE, ptr %0, align 8
  %length10 = extractvalue %_Z6VectorI10JsonMemberE %load.struct9, 0
  %mul11 = mul i64 %length10, ptrtoint (ptr getelementptr (%_Z10JsonMember, ptr null, i32 1) to i64)
  %call12 = call ptr @memcpy(ptr %deref.recv, ptr %deref.recv8, i64 %mul11)
  br label %if.end

if.else:                                          ; preds = %entry
  %data13 = getelementptr inbounds nuw %_Z6VectorI10JsonMemberE, ptr %0, i32 0, i32 1
  store ptr null, ptr %data13, align 8
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  %if.value = phi ptr [ %call12, %if.then ], [ null, %if.else ]
  ret void
}

define linkonce_odr void @_ZN6VectorI10JsonMemberB4jsonEC1E5ArrayI10JsonMemberB4jsonE(ptr %0, ptr %1) {
entry:
  %call = call ptr @_ZN4Page3getEPv(ptr %0)
  %load.struct = load %_Z5ArrayI10JsonMemberE, ptr %1, align 8
  %length = extractvalue %_Z5ArrayI10JsonMemberE %load.struct, 0
  %length1 = getelementptr inbounds nuw %_Z6VectorI10JsonMemberE, ptr %0, i32 0, i32 0
  store i64 %length, ptr %length1, align 8
  %load.struct2 = load %_Z6VectorI10JsonMemberE, ptr %0, align 8
  %length3 = extractvalue %_Z6VectorI10JsonMemberE %load.struct2, 0
  %gt = icmp ugt i64 %length3, 0
  br i1 %gt, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %load.struct4 = load %_Z6VectorI10JsonMemberE, ptr %0, align 8
  %length5 = extractvalue %_Z6VectorI10JsonMemberE %load.struct4, 0
  %mul = mul i64 %length5, ptrtoint (ptr getelementptr (%_Z10JsonMember, ptr null, i32 1) to i64)
  %call6 = call ptr @_ZN4Page8allocateEmm(ptr %call, i64 %mul, i64 8)
  %data = getelementptr inbounds nuw %_Z6VectorI10JsonMemberE, ptr %0, i32 0, i32 1
  store ptr %call6, ptr %data, align 8
  %field.inplace = getelementptr inbounds nuw %_Z6VectorI10JsonMemberE, ptr %0, i32 0, i32 1
  %deref.recv = load ptr, ptr %field.inplace, align 8
  %call7 = call ptr @_ZN5ArrayI10JsonMemberB4jsonE10get_bufferEv(ptr %1)
  %load.struct8 = load %_Z6VectorI10JsonMemberE, ptr %0, align 8
  %length9 = extractvalue %_Z6VectorI10JsonMemberE %load.struct8, 0
  %mul10 = mul i64 %length9, ptrtoint (ptr getelementptr (%_Z10JsonMember, ptr null, i32 1) to i64)
  %call11 = call ptr @memcpy(ptr %deref.recv, ptr %call7, i64 %mul10)
  br label %if.end

if.else:                                          ; preds = %entry
  %data12 = getelementptr inbounds nuw %_Z6VectorI10JsonMemberE, ptr %0, i32 0, i32 1
  store ptr null, ptr %data12, align 8
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  %if.value = phi ptr [ %call11, %if.then ], [ null, %if.else ]
  ret void
}

define linkonce_odr ptr @_ZN4ListI10JsonMemberB4jsonE8get_headEv(ptr %0) {
entry:
  %load.struct = load %_Z4ListI10JsonMemberE, ptr %0, align 8
  %head = extractvalue %_Z4ListI10JsonMemberE %load.struct, 0
  %eq = icmp eq ptr %head, null
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %addr.gep = getelementptr inbounds nuw %_Z4ListI10JsonMemberE, ptr %0, i32 0, i32 0
  %addr.hop = load ptr, ptr %addr.gep, align 8
  %addr.gep1 = getelementptr inbounds nuw %_Z4NodeI10JsonMemberE, ptr %addr.hop, i32 0, i32 0
  ret ptr %addr.gep1
}

define linkonce_odr ptr @_ZN12ListIteratorI10JsonMemberB4jsonE4nextEv(ptr %0) {
entry:
  %old_current = alloca ptr, align 8
  %load.struct = load %_Z12ListIteratorI10JsonMemberE, ptr %0, align 8
  %current = extractvalue %_Z12ListIteratorI10JsonMemberE %load.struct, 0
  %ne = icmp ne ptr %current, null
  br i1 %ne, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %load.struct1 = load %_Z12ListIteratorI10JsonMemberE, ptr %0, align 8
  %current2 = extractvalue %_Z12ListIteratorI10JsonMemberE %load.struct1, 0
  store ptr %current2, ptr %old_current, align 1
  %load.struct3 = load %_Z12ListIteratorI10JsonMemberE, ptr %0, align 8
  %current4 = extractvalue %_Z12ListIteratorI10JsonMemberE %load.struct3, 0
  %deref = load %_Z4NodeI10JsonMemberE, ptr %current4, align 8
  %next = extractvalue %_Z4NodeI10JsonMemberE %deref, 1
  %current5 = getelementptr inbounds nuw %_Z12ListIteratorI10JsonMemberE, ptr %0, i32 0, i32 0
  store ptr %next, ptr %current5, align 8
  %old_current6 = load ptr, ptr %old_current, align 8
  %addr.gep = getelementptr inbounds nuw %_Z4NodeI10JsonMemberE, ptr %old_current6, i32 0, i32 0
  ret ptr %addr.gep

if.else:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; No predecessors!
  ret ptr null
}

define linkonce_odr i64 @_ZN4ListI10JsonMemberB4jsonE5countEv(ptr %0) {
entry:
  %sret.result = alloca %_Z12ListIteratorI10JsonMemberE, align 8
  call void @_ZN4ListI10JsonMemberB4jsonE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z12ListIteratorI10JsonMemberE) %sret.result, ptr null, ptr %0)
  %list_iterator = alloca ptr, align 8
  store ptr %sret.result, ptr %list_iterator, align 1
  %i = alloca i64, align 8
  store i64 0, ptr %i, align 1
  br label %while.cond

while.cond:                                       ; preds = %while.body, %entry
  %list_iterator1 = load ptr, ptr %list_iterator, align 8
  %call = call ptr @_ZN12ListIteratorI10JsonMemberB4jsonE4nextEv(ptr %list_iterator1)
  %ne = icmp ne ptr %call, null
  br i1 %ne, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %i2 = load i64, ptr %i, align 8
  %add = add i64 %i2, 1
  store i64 %add, ptr %i, align 1
  br label %while.cond

while.exit:                                       ; preds = %while.cond
  %i3 = load i64, ptr %i, align 8
  ret i64 %i3
}

define linkonce_odr void @_ZN4ListI10JsonMemberB4jsonE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z12ListIteratorI10JsonMemberE) %0, ptr %1, ptr %2) {
entry:
  %load.struct = load %_Z4ListI10JsonMemberE, ptr %2, align 8
  %head = extractvalue %_Z4ListI10JsonMemberE %load.struct, 0
  %tuple = alloca %_Z12ListIteratorI10JsonMemberE, align 8
  %tuple.field = getelementptr inbounds nuw %_Z12ListIteratorI10JsonMemberE, ptr %tuple, i32 0, i32 0
  store ptr %head, ptr %tuple.field, align 1
  %tuple.val = load %_Z12ListIteratorI10JsonMemberE, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z12ListIteratorI10JsonMemberE, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN4ListI10JsonMemberB4jsonE4linkEP4NodeI10JsonMemberB4jsonE(ptr %0, ptr %1) {
entry:
  %load.struct = load %_Z4ListI10JsonMemberE, ptr %0, align 8
  %tail = extractvalue %_Z4ListI10JsonMemberE %load.struct, 1
  %eq = icmp eq ptr %tail, null
  br i1 %eq, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %head = getelementptr inbounds nuw %_Z4ListI10JsonMemberE, ptr %0, i32 0, i32 0
  store ptr %1, ptr %head, align 8
  br label %if.end

if.else:                                          ; preds = %entry
  %tail1 = getelementptr inbounds nuw %_Z4ListI10JsonMemberE, ptr %0, i32 0, i32 1
  %field.deref = load ptr, ptr %tail1, align 8
  %next = getelementptr inbounds nuw %_Z4NodeI10JsonMemberE, ptr %field.deref, i32 0, i32 1
  store ptr %1, ptr %next, align 8
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  %if.value = phi ptr [ %1, %if.then ], [ %1, %if.else ]
  %tail2 = getelementptr inbounds nuw %_Z4ListI10JsonMemberE, ptr %0, i32 0, i32 1
  store ptr %1, ptr %tail2, align 8
  ret void
}

define linkonce_odr void @_ZN4ListI10JsonMemberB4jsonE3addE10JsonMemberB4json(ptr %0, ptr %1) {
entry:
  %own_page = call ptr @_Z3getPv(ptr %0)
  %tuple.region = call ptr @_ZN4Page8allocateEmm(ptr %own_page, i64 ptrtoint (ptr getelementptr (%_Z4NodeI10JsonMemberE, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr ({ i1, %_Z4NodeI10JsonMemberE }, ptr null, i64 0, i32 1) to i64))
  %field.load = load %_Z10JsonMember, ptr %1, align 8
  %tuple.field = getelementptr inbounds nuw %_Z4NodeI10JsonMemberE, ptr %tuple.region, i32 0, i32 0
  store %_Z10JsonMember %field.load, ptr %tuple.field, align 1
  %tuple.field1 = getelementptr inbounds nuw %_Z4NodeI10JsonMemberE, ptr %tuple.region, i32 0, i32 1
  store ptr null, ptr %tuple.field1, align 1
  call void @_ZN4ListI10JsonMemberB4jsonE4linkEP4NodeI10JsonMemberB4jsonE(ptr %0, ptr %tuple.region)
  ret void
}

define linkonce_odr void @_ZN4ListI10JsonMemberB4jsonE6add_onER4Page10JsonMemberB4json(ptr %0, ptr %1, ptr %2) {
entry:
  %tuple.region = call ptr @_ZN4Page8allocateEmm(ptr %1, i64 ptrtoint (ptr getelementptr (%_Z4NodeI10JsonMemberE, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr ({ i1, %_Z4NodeI10JsonMemberE }, ptr null, i64 0, i32 1) to i64))
  %field.load = load %_Z10JsonMember, ptr %2, align 8
  %tuple.field = getelementptr inbounds nuw %_Z4NodeI10JsonMemberE, ptr %tuple.region, i32 0, i32 0
  store %_Z10JsonMember %field.load, ptr %tuple.field, align 1
  %tuple.field1 = getelementptr inbounds nuw %_Z4NodeI10JsonMemberE, ptr %tuple.region, i32 0, i32 1
  store ptr null, ptr %tuple.field1, align 1
  call void @_ZN4ListI10JsonMemberB4jsonE4linkEP4NodeI10JsonMemberB4jsonE(ptr %0, ptr %tuple.region)
  ret void
}

define linkonce_odr void @_ZN6VectorI10JsonMemberB4jsonEC1E4ListI10JsonMemberB4jsonE(ptr %0, ptr %1) {
entry:
  %deref.tmp = alloca %_Z10JsonMember, align 8
  %i = alloca i64, align 8
  %list_iterator = alloca ptr, align 8
  %sret.result = alloca %_Z12ListIteratorI10JsonMemberE, align 8
  %call = call ptr @_ZN4Page3getEPv(ptr %0)
  %call1 = call i64 @_ZN4ListI10JsonMemberB4jsonE5countEv(ptr %1)
  %length = getelementptr inbounds nuw %_Z6VectorI10JsonMemberE, ptr %0, i32 0, i32 0
  store i64 %call1, ptr %length, align 8
  %load.struct = load %_Z6VectorI10JsonMemberE, ptr %0, align 8
  %length2 = extractvalue %_Z6VectorI10JsonMemberE %load.struct, 0
  %gt = icmp ugt i64 %length2, 0
  br i1 %gt, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %load.struct3 = load %_Z6VectorI10JsonMemberE, ptr %0, align 8
  %length4 = extractvalue %_Z6VectorI10JsonMemberE %load.struct3, 0
  %mul = mul i64 %length4, ptrtoint (ptr getelementptr (%_Z10JsonMember, ptr null, i32 1) to i64)
  %call5 = call ptr @_ZN4Page8allocateEmm(ptr %call, i64 %mul, i64 8)
  %data = getelementptr inbounds nuw %_Z6VectorI10JsonMemberE, ptr %0, i32 0, i32 1
  store ptr %call5, ptr %data, align 8
  call void @_ZN4ListI10JsonMemberB4jsonE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z12ListIteratorI10JsonMemberE) %sret.result, ptr null, ptr %1)
  store ptr %sret.result, ptr %list_iterator, align 1
  store i64 0, ptr %i, align 1
  br label %while.cond

if.else:                                          ; preds = %entry
  %data12 = getelementptr inbounds nuw %_Z6VectorI10JsonMemberE, ptr %0, i32 0, i32 1
  store ptr null, ptr %data12, align 8
  br label %if.end

if.end:                                           ; preds = %if.else, %while.exit
  ret void

while.cond:                                       ; preds = %while.body, %if.then
  %list_iterator6 = load ptr, ptr %list_iterator, align 8
  %call7 = call ptr @_ZN12ListIteratorI10JsonMemberB4jsonE4nextEv(ptr %list_iterator6)
  %while.tobool = icmp ne ptr %call7, null
  br i1 %while.tobool, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %deref.tmp, ptr align 1 %call7, i64 ptrtoint (ptr getelementptr (%_Z10JsonMember, ptr null, i32 1) to i64), i1 false)
  %load.struct8 = load %_Z6VectorI10JsonMemberE, ptr %0, align 8
  %data9 = extractvalue %_Z6VectorI10JsonMemberE %load.struct8, 1
  %i10 = load i64, ptr %i, align 8
  %ptr.add = getelementptr inbounds %_Z10JsonMember, ptr %data9, i64 %i10
  %store.load = load %_Z10JsonMember, ptr %deref.tmp, align 8
  store %_Z10JsonMember %store.load, ptr %ptr.add, align 8
  %i11 = load i64, ptr %i, align 8
  %add = add i64 %i11, 1
  store i64 %add, ptr %i, align 1
  br label %while.cond

while.exit:                                       ; preds = %while.cond
  br label %if.end
}

define linkonce_odr void @_ZN5ArrayI10JsonMemberB4jsonE7add_runEmP10JsonMemberB4json(ptr %0, i64 %1, ptr %2) {
entry:
  %load.struct = load %_Z5ArrayI10JsonMemberE, ptr %0, align 8
  %length = extractvalue %_Z5ArrayI10JsonMemberE %load.struct, 0
  %add = add i64 %length, %1
  %new_length = alloca i64, align 8
  store i64 %add, ptr %new_length, align 1
  %new_length1 = load i64, ptr %new_length, align 8
  %load.struct2 = load %_Z5ArrayI10JsonMemberE, ptr %0, align 8
  %length3 = extractvalue %_Z5ArrayI10JsonMemberE %load.struct2, 0
  %lt = icmp ult i64 %new_length1, %length3
  br i1 %lt, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %field.inplace = getelementptr inbounds nuw %_Z5ArrayI10JsonMemberE, ptr %0, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  call void @_Z16scaly_panic_sizeP10const_charmm(ptr @.str.43, i64 %1, i64 %field.val)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %load.struct6 = load %_Z5ArrayI10JsonMemberE, ptr %0, align 8
  %buffer = extractvalue %_Z5ArrayI10JsonMemberE %load.struct6, 2
  %eq = icmp eq ptr %buffer, null
  br i1 %eq, label %if.then4, label %lor.rhs

if.then4:                                         ; preds = %lor.rhs, %if.end
  %new_length9 = load i64, ptr %new_length, align 8
  call void @_ZN5ArrayI10JsonMemberB4jsonE7grow_toEm(ptr %0, i64 %new_length9)
  br label %if.end5

if.end5:                                          ; preds = %if.then4, %lor.rhs
  %gt10 = icmp ugt i64 %1, 0
  br i1 %gt10, label %if.then11, label %if.end12

lor.rhs:                                          ; preds = %if.end
  %new_length7 = load i64, ptr %new_length, align 8
  %load.struct8 = load %_Z5ArrayI10JsonMemberE, ptr %0, align 8
  %capacity = extractvalue %_Z5ArrayI10JsonMemberE %load.struct8, 1
  %gt = icmp ugt i64 %new_length7, %capacity
  br i1 %gt, label %if.then4, label %if.end5

if.then11:                                        ; preds = %if.end5
  %load.struct13 = load %_Z5ArrayI10JsonMemberE, ptr %0, align 8
  %buffer14 = extractvalue %_Z5ArrayI10JsonMemberE %load.struct13, 2
  %load.struct15 = load %_Z5ArrayI10JsonMemberE, ptr %0, align 8
  %length16 = extractvalue %_Z5ArrayI10JsonMemberE %load.struct15, 0
  %ptr.add = getelementptr inbounds %_Z10JsonMember, ptr %buffer14, i64 %length16
  %mul = mul i64 %1, ptrtoint (ptr getelementptr (%_Z10JsonMember, ptr null, i32 1) to i64)
  %call = call ptr @memcpy(ptr %ptr.add, ptr %2, i64 %mul)
  br label %if.end12

if.end12:                                         ; preds = %if.then11, %if.end5
  %load.struct17 = load %_Z5ArrayI10JsonMemberE, ptr %0, align 8
  %length18 = extractvalue %_Z5ArrayI10JsonMemberE %load.struct17, 0
  %add19 = add i64 %length18, %1
  %length20 = getelementptr inbounds nuw %_Z5ArrayI10JsonMemberE, ptr %0, i32 0, i32 0
  store i64 %add19, ptr %length20, align 8
  ret void
}

define linkonce_odr void @_ZN5ArrayI10JsonMemberB4jsonE3addE6VectorI10JsonMemberB4jsonE(ptr %0, ptr %1) {
entry:
  %field.inplace = getelementptr inbounds nuw %_Z6VectorI10JsonMemberE, ptr %1, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  %field.inplace1 = getelementptr inbounds nuw %_Z6VectorI10JsonMemberE, ptr %1, i32 0, i32 1
  %deref.recv = load ptr, ptr %field.inplace1, align 8
  call void @_ZN5ArrayI10JsonMemberB4jsonE7add_runEmP10JsonMemberB4json(ptr %0, i64 %field.val, ptr %deref.recv)
  ret void
}

define linkonce_odr void @_ZN5ArrayI10JsonMemberB4jsonE3addE5SliceI10JsonMemberB4jsonE(ptr %0, ptr %1) {
entry:
  %field.inplace = getelementptr inbounds nuw %_Z5SliceI10JsonMemberE, ptr %1, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  %field.inplace1 = getelementptr inbounds nuw %_Z5SliceI10JsonMemberE, ptr %1, i32 0, i32 1
  %deref.recv = load ptr, ptr %field.inplace1, align 8
  call void @_ZN5ArrayI10JsonMemberB4jsonE7add_runEmP10JsonMemberB4json(ptr %0, i64 %field.val, ptr %deref.recv)
  ret void
}

define linkonce_odr void @_ZN5ArrayI10JsonMemberB4jsonE7grow_toEm(ptr %0, i64 %1) {
entry:
  call void @_ZN5ArrayI10JsonMemberB4jsonE10reallocateEv(ptr %0)
  %load.struct = load %_Z5ArrayI10JsonMemberE, ptr %0, align 8
  %capacity = extractvalue %_Z5ArrayI10JsonMemberE %load.struct, 1
  %gt = icmp ugt i64 %1, %capacity
  br i1 %gt, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %call = call ptr @_ZN4Page3getEPv(ptr %0)
  %load.struct1 = load %_Z5ArrayI10JsonMemberE, ptr %0, align 8
  %buffer = extractvalue %_Z5ArrayI10JsonMemberE %load.struct1, 2
  %load.struct2 = load %_Z5ArrayI10JsonMemberE, ptr %0, align 8
  %capacity3 = extractvalue %_Z5ArrayI10JsonMemberE %load.struct2, 1
  %mul = mul i64 %capacity3, ptrtoint (ptr getelementptr (%_Z10JsonMember, ptr null, i32 1) to i64)
  %le = icmp ule i64 %mul, 1024
  %mul4 = mul i64 %1, ptrtoint (ptr getelementptr (%_Z10JsonMember, ptr null, i32 1) to i64)
  %le5 = icmp ule i64 %mul4, 1024
  br i1 %le5, label %if.then6, label %if.else

if.end:                                           ; preds = %if.end24, %entry
  ret void

if.then6:                                         ; preds = %if.then
  %mul8 = mul i64 %1, ptrtoint (ptr getelementptr (%_Z10JsonMember, ptr null, i32 1) to i64)
  %call9 = call ptr @_ZN4Page8allocateEmm(ptr %call, i64 %mul8, i64 8)
  %buffer10 = getelementptr inbounds nuw %_Z5ArrayI10JsonMemberE, ptr %0, i32 0, i32 2
  store ptr %call9, ptr %buffer10, align 8
  br label %if.end7

if.else:                                          ; preds = %if.then
  %mul11 = mul i64 %1, ptrtoint (ptr getelementptr (%_Z10JsonMember, ptr null, i32 1) to i64)
  %call12 = call ptr @_ZN4Page25allocate_exclusive_bufferEmm(ptr %call, i64 %mul11, i64 8)
  %buffer13 = getelementptr inbounds nuw %_Z5ArrayI10JsonMemberE, ptr %0, i32 0, i32 2
  store ptr %call12, ptr %buffer13, align 8
  br label %if.end7

if.end7:                                          ; preds = %if.else, %if.then6
  %if.value = phi ptr [ %call9, %if.then6 ], [ %call12, %if.else ]
  %capacity14 = getelementptr inbounds nuw %_Z5ArrayI10JsonMemberE, ptr %0, i32 0, i32 1
  store i64 %1, ptr %capacity14, align 8
  %load.struct15 = load %_Z5ArrayI10JsonMemberE, ptr %0, align 8
  %length = extractvalue %_Z5ArrayI10JsonMemberE %load.struct15, 0
  %gt16 = icmp ugt i64 %length, 0
  br i1 %gt16, label %if.then17, label %if.end18

if.then17:                                        ; preds = %if.end7
  %field.inplace = getelementptr inbounds nuw %_Z5ArrayI10JsonMemberE, ptr %0, i32 0, i32 2
  %deref.recv = load ptr, ptr %field.inplace, align 8
  %load.struct19 = load %_Z5ArrayI10JsonMemberE, ptr %0, align 8
  %length20 = extractvalue %_Z5ArrayI10JsonMemberE %load.struct19, 0
  %mul21 = mul i64 %length20, ptrtoint (ptr getelementptr (%_Z10JsonMember, ptr null, i32 1) to i64)
  %call22 = call ptr @memcpy(ptr %deref.recv, ptr %buffer, i64 %mul21)
  br label %if.end18

if.end18:                                         ; preds = %if.then17, %if.end7
  %eq = icmp eq i1 %le, false
  br i1 %eq, label %if.then23, label %if.end24

if.then23:                                        ; preds = %if.end18
  %call25 = call ptr @_ZN4Page3getEPv(ptr %buffer)
  call void @_ZN4Page25deallocate_exclusive_pageER4Page(ptr %call, ptr %call25)
  br label %if.end24

if.end24:                                         ; preds = %if.then23, %if.end18
  br label %if.end
}

define linkonce_odr void @_ZN5ArrayI10JsonMemberB4jsonE6extendEm(ptr noalias sret(%_Z5SliceI10JsonMemberE) %0, ptr %1, i64 %2) {
entry:
  %tuple = alloca %_Z5SliceI10JsonMemberE, align 8
  %load.struct = load %_Z5ArrayI10JsonMemberE, ptr %1, align 8
  %length = extractvalue %_Z5ArrayI10JsonMemberE %load.struct, 0
  %add = add i64 %length, %2
  %lt = icmp ult i64 %add, %length
  br i1 %lt, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  call void @_Z16scaly_panic_sizeP10const_charmm(ptr @.str.44, i64 %2, i64 %length)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %load.struct3 = load %_Z5ArrayI10JsonMemberE, ptr %1, align 8
  %buffer = extractvalue %_Z5ArrayI10JsonMemberE %load.struct3, 2
  %eq = icmp eq ptr %buffer, null
  br i1 %eq, label %if.then1, label %lor.rhs

if.then1:                                         ; preds = %lor.rhs, %if.end
  call void @_ZN5ArrayI10JsonMemberB4jsonE7grow_toEm(ptr %1, i64 %add)
  br label %if.end2

if.end2:                                          ; preds = %if.then1, %lor.rhs
  %length5 = getelementptr inbounds nuw %_Z5ArrayI10JsonMemberE, ptr %1, i32 0, i32 0
  store i64 %add, ptr %length5, align 8
  %load.struct6 = load %_Z5ArrayI10JsonMemberE, ptr %1, align 8
  %buffer7 = extractvalue %_Z5ArrayI10JsonMemberE %load.struct6, 2
  %ptr.add = getelementptr inbounds %_Z10JsonMember, ptr %buffer7, i64 %length
  %tuple.field = getelementptr inbounds nuw %_Z5SliceI10JsonMemberE, ptr %tuple, i32 0, i32 0
  store i64 %2, ptr %tuple.field, align 1
  %tuple.field8 = getelementptr inbounds nuw %_Z5SliceI10JsonMemberE, ptr %tuple, i32 0, i32 1
  store ptr %ptr.add, ptr %tuple.field8, align 1
  %tuple.val = load %_Z5SliceI10JsonMemberE, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z5SliceI10JsonMemberE, ptr null, i32 1) to i64), i1 false)
  ret void

lor.rhs:                                          ; preds = %if.end
  %load.struct4 = load %_Z5ArrayI10JsonMemberE, ptr %1, align 8
  %capacity = extractvalue %_Z5ArrayI10JsonMemberE %load.struct4, 1
  %gt = icmp ugt i64 %add, %capacity
  br i1 %gt, label %if.then1, label %if.end2
}

define linkonce_odr ptr @_ZN5ArrayI10JsonMemberB4jsonE3getEm(ptr %0, i64 %1) {
entry:
  %load.struct = load %_Z5ArrayI10JsonMemberE, ptr %0, align 8
  %length = extractvalue %_Z5ArrayI10JsonMemberE %load.struct, 0
  %ge = icmp uge i64 %1, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %load.struct1 = load %_Z5ArrayI10JsonMemberE, ptr %0, align 8
  %buffer = extractvalue %_Z5ArrayI10JsonMemberE %load.struct1, 2
  %ptr.add = getelementptr inbounds %_Z10JsonMember, ptr %buffer, i64 %1
  ret ptr %ptr.add
}

define linkonce_odr ptr @_ZN5ArrayI10JsonMemberB4jsonE2atEm(ptr %0, i64 %1) {
entry:
  %load.struct = load %_Z5ArrayI10JsonMemberE, ptr %0, align 8
  %length = extractvalue %_Z5ArrayI10JsonMemberE %load.struct, 0
  %ge = icmp uge i64 %1, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %field.inplace = getelementptr inbounds nuw %_Z5ArrayI10JsonMemberE, ptr %0, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  call void @_Z15scaly_panic_oobP10const_charmm(ptr @.str.45, i64 %1, i64 %field.val)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %load.struct1 = load %_Z5ArrayI10JsonMemberE, ptr %0, align 8
  %buffer = extractvalue %_Z5ArrayI10JsonMemberE %load.struct1, 2
  %ptr.add = getelementptr inbounds %_Z10JsonMember, ptr %buffer, i64 %1
  ret ptr %ptr.add
}

define linkonce_odr void @_ZN5ArrayI10JsonMemberB4jsonE5clearEv(ptr %0) {
entry:
  %length = getelementptr inbounds nuw %_Z5ArrayI10JsonMemberE, ptr %0, i32 0, i32 0
  store i64 0, ptr %length, align 8
  ret void
}

define linkonce_odr void @_ZN5ArrayI10JsonMemberB4jsonE3putEm10JsonMemberB4json(ptr %0, i64 %1, ptr %2) {
entry:
  %load.struct = load %_Z5ArrayI10JsonMemberE, ptr %0, align 8
  %length = extractvalue %_Z5ArrayI10JsonMemberE %load.struct, 0
  %ge = icmp uge i64 %1, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %field.inplace = getelementptr inbounds nuw %_Z5ArrayI10JsonMemberE, ptr %0, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  call void @_Z15scaly_panic_oobP10const_charmm(ptr @.str.46, i64 %1, i64 %field.val)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %load.struct1 = load %_Z5ArrayI10JsonMemberE, ptr %0, align 8
  %buffer = extractvalue %_Z5ArrayI10JsonMemberE %load.struct1, 2
  %ptr.add = getelementptr inbounds %_Z10JsonMember, ptr %buffer, i64 %1
  %store.load = load %_Z10JsonMember, ptr %2, align 8
  store %_Z10JsonMember %store.load, ptr %ptr.add, align 8
  ret void
}

define linkonce_odr ptr @_ZN13ArrayIteratorI10JsonMemberB4jsonE4nextEv(ptr %0) {
entry:
  %load.struct = load %_Z13ArrayIteratorI10JsonMemberE, ptr %0, align 8
  %array = extractvalue %_Z13ArrayIteratorI10JsonMemberE %load.struct, 0
  %eq = icmp eq ptr %array, null
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %load.struct1 = load %_Z13ArrayIteratorI10JsonMemberE, ptr %0, align 8
  %position = extractvalue %_Z13ArrayIteratorI10JsonMemberE %load.struct1, 1
  %load.struct2 = load %_Z13ArrayIteratorI10JsonMemberE, ptr %0, align 8
  %array3 = extractvalue %_Z13ArrayIteratorI10JsonMemberE %load.struct2, 0
  %deref = load %_Z5ArrayI10JsonMemberE, ptr %array3, align 8
  %length = extractvalue %_Z5ArrayI10JsonMemberE %deref, 0
  %eq4 = icmp eq i64 %position, %length
  br i1 %eq4, label %if.then5, label %if.end6

if.then5:                                         ; preds = %if.end
  ret ptr null

if.end6:                                          ; preds = %if.end
  %load.struct7 = load %_Z13ArrayIteratorI10JsonMemberE, ptr %0, align 8
  %position8 = extractvalue %_Z13ArrayIteratorI10JsonMemberE %load.struct7, 1
  %add = add i64 %position8, 1
  %position9 = getelementptr inbounds nuw %_Z13ArrayIteratorI10JsonMemberE, ptr %0, i32 0, i32 1
  store i64 %add, ptr %position9, align 8
  %field.inplace = getelementptr inbounds nuw %_Z13ArrayIteratorI10JsonMemberE, ptr %0, i32 0, i32 0
  %deref.recv = load ptr, ptr %field.inplace, align 8
  %call = call ptr @_ZN5ArrayI10JsonMemberB4jsonE10get_bufferEv(ptr %deref.recv)
  %load.struct10 = load %_Z13ArrayIteratorI10JsonMemberE, ptr %0, align 8
  %position11 = extractvalue %_Z13ArrayIteratorI10JsonMemberE %load.struct10, 1
  %sub = sub i64 %position11, 1
  %ptr.add = getelementptr inbounds %_Z10JsonMember, ptr %call, i64 %sub
  ret ptr %ptr.add
}

define linkonce_odr void @_ZN13ArrayIteratorI10JsonMemberB4jsonEC1E6OptionIR5ArrayI10JsonMemberB4jsonEE(ptr %0, ptr %1) {
entry:
  %array = getelementptr inbounds nuw %_Z13ArrayIteratorI10JsonMemberE, ptr %0, i32 0, i32 0
  store ptr %1, ptr %array, align 8
  %position = getelementptr inbounds nuw %_Z13ArrayIteratorI10JsonMemberE, ptr %0, i32 0, i32 1
  store i64 0, ptr %position, align 8
  ret void
}

define linkonce_odr void @_ZN5ArrayI10JsonMemberB4jsonE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z13ArrayIteratorI10JsonMemberE) %0, ptr %1, ptr %2) {
entry:
  %struct.init = alloca %_Z13ArrayIteratorI10JsonMemberE, align 8
  call void @_ZN13ArrayIteratorI10JsonMemberB4jsonEC1E6OptionIR5ArrayI10JsonMemberB4jsonEE(ptr %struct.init, ptr %2)
  %sret.body = load %_Z13ArrayIteratorI10JsonMemberE, ptr %struct.init, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %struct.init, i64 ptrtoint (ptr getelementptr (%_Z13ArrayIteratorI10JsonMemberE, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN5ArrayI10JsonMemberB4jsonE8as_sliceEv(ptr noalias sret(%_Z5SliceI10JsonMemberE) %0, ptr %1) {
entry:
  %load.struct = load %_Z5ArrayI10JsonMemberE, ptr %1, align 8
  %length = extractvalue %_Z5ArrayI10JsonMemberE %load.struct, 0
  %call = call ptr @_ZN5ArrayI10JsonMemberB4jsonE10get_bufferEv(ptr %1)
  %tuple = alloca %_Z5SliceI10JsonMemberE, align 8
  %tuple.field = getelementptr inbounds nuw %_Z5SliceI10JsonMemberE, ptr %tuple, i32 0, i32 0
  store i64 %length, ptr %tuple.field, align 1
  %tuple.field1 = getelementptr inbounds nuw %_Z5SliceI10JsonMemberE, ptr %tuple, i32 0, i32 1
  store ptr %call, ptr %tuple.field1, align 1
  %tuple.val = load %_Z5SliceI10JsonMemberE, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z5SliceI10JsonMemberE, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN5ArrayI10JsonMemberB4jsonEC1Ev(ptr %0) {
entry:
  %length = getelementptr inbounds nuw %_Z5ArrayI10JsonMemberE, ptr %0, i32 0, i32 0
  store i64 0, ptr %length, align 8
  %capacity = getelementptr inbounds nuw %_Z5ArrayI10JsonMemberE, ptr %0, i32 0, i32 1
  store i64 0, ptr %capacity, align 8
  %buffer = getelementptr inbounds nuw %_Z5ArrayI10JsonMemberE, ptr %0, i32 0, i32 2
  store ptr null, ptr %buffer, align 8
  ret void
}

define linkonce_odr void @_ZN5ArrayI10JsonMemberB4jsonEC1Em(ptr %0, i64 %1) {
entry:
  %length = getelementptr inbounds nuw %_Z5ArrayI10JsonMemberE, ptr %0, i32 0, i32 0
  store i64 0, ptr %length, align 8
  %capacity = getelementptr inbounds nuw %_Z5ArrayI10JsonMemberE, ptr %0, i32 0, i32 1
  store i64 0, ptr %capacity, align 8
  %buffer = getelementptr inbounds nuw %_Z5ArrayI10JsonMemberE, ptr %0, i32 0, i32 2
  store ptr null, ptr %buffer, align 8
  %gt = icmp ugt i64 %1, 0
  br i1 %gt, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %call = call ptr @_ZN4Page3getEPv(ptr %0)
  %capacity1 = getelementptr inbounds nuw %_Z5ArrayI10JsonMemberE, ptr %0, i32 0, i32 1
  store i64 %1, ptr %capacity1, align 8
  %mul = mul i64 %1, ptrtoint (ptr getelementptr (%_Z10JsonMember, ptr null, i32 1) to i64)
  %le = icmp ule i64 %mul, 1024
  br i1 %le, label %if.then2, label %if.else

if.end:                                           ; preds = %if.end3, %entry
  ret void

if.then2:                                         ; preds = %if.then
  %mul4 = mul i64 %1, ptrtoint (ptr getelementptr (%_Z10JsonMember, ptr null, i32 1) to i64)
  %call5 = call ptr @_ZN4Page8allocateEmm(ptr %call, i64 %mul4, i64 8)
  %buffer6 = getelementptr inbounds nuw %_Z5ArrayI10JsonMemberE, ptr %0, i32 0, i32 2
  store ptr %call5, ptr %buffer6, align 8
  br label %if.end3

if.else:                                          ; preds = %if.then
  %mul7 = mul i64 %1, ptrtoint (ptr getelementptr (%_Z10JsonMember, ptr null, i32 1) to i64)
  %call8 = call ptr @_ZN4Page25allocate_exclusive_bufferEmm(ptr %call, i64 %mul7, i64 8)
  %buffer9 = getelementptr inbounds nuw %_Z5ArrayI10JsonMemberE, ptr %0, i32 0, i32 2
  store ptr %call8, ptr %buffer9, align 8
  br label %if.end3

if.end3:                                          ; preds = %if.else, %if.then2
  %if.value = phi ptr [ %call5, %if.then2 ], [ %call8, %if.else ]
  br label %if.end
}

define linkonce_odr ptr @_ZN5ArrayImE10get_bufferEv(ptr %0) {
entry:
  %load.struct = load %_Z5ArrayImE, ptr %0, align 8
  %buffer = extractvalue %_Z5ArrayImE %load.struct, 2
  ret ptr %buffer
}

define linkonce_odr i64 @_ZN5ArrayImE10get_lengthEv(ptr %0) {
entry:
  %load.struct = load %_Z5ArrayImE, ptr %0, align 8
  %length = extractvalue %_Z5ArrayImE %load.struct, 0
  ret i64 %length
}

define linkonce_odr i64 @_ZN5ArrayImE12get_capacityEv(ptr %0) {
entry:
  %load.struct = load %_Z5ArrayImE, ptr %0, align 8
  %capacity = extractvalue %_Z5ArrayImE %load.struct, 1
  ret i64 %capacity
}

define linkonce_odr void @_ZN5ArrayImE10reallocateEv(ptr %0) {
entry:
  %first_cap = alloca i64, align 8
  %call = call ptr @_ZN4Page3getEPv(ptr %0)
  %load.struct = load %_Z5ArrayImE, ptr %0, align 8
  %buffer = extractvalue %_Z5ArrayImE %load.struct, 2
  %eq = icmp eq ptr %buffer, null
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %udiv = udiv i64 32, ptrtoint (ptr getelementptr (i64, ptr null, i32 1) to i64)
  store i64 %udiv, ptr %first_cap, align 1
  %first_cap1 = load i64, ptr %first_cap, align 8
  %lt = icmp ult i64 %first_cap1, 1
  br i1 %lt, label %if.then2, label %if.end3

if.end:                                           ; preds = %entry
  %load.struct18 = load %_Z5ArrayImE, ptr %0, align 8
  %capacity19 = extractvalue %_Z5ArrayImE %load.struct18, 1
  %mul20 = mul i64 %capacity19, ptrtoint (ptr getelementptr (i64, ptr null, i32 1) to i64)
  %le21 = icmp ule i64 %mul20, 1024
  %load.struct22 = load %_Z5ArrayImE, ptr %0, align 8
  %buffer23 = extractvalue %_Z5ArrayImE %load.struct22, 2
  %load.struct24 = load %_Z5ArrayImE, ptr %0, align 8
  %capacity25 = extractvalue %_Z5ArrayImE %load.struct24, 1
  %load.struct26 = load %_Z5ArrayImE, ptr %0, align 8
  %capacity27 = extractvalue %_Z5ArrayImE %load.struct26, 1
  %mul28 = mul i64 %capacity27, 2
  store i64 %mul28, ptr %first_cap, align 1
  %new_capacity = load i64, ptr %first_cap, align 8
  %mul29 = mul i64 %new_capacity, ptrtoint (ptr getelementptr (i64, ptr null, i32 1) to i64)
  %le30 = icmp ule i64 %mul29, 1024
  br i1 %le30, label %if.then31, label %if.end32

if.then2:                                         ; preds = %if.then
  store i64 1, ptr %first_cap, align 1
  br label %if.end3

if.end3:                                          ; preds = %if.then2, %if.then
  %first_cap4 = load i64, ptr %first_cap, align 8
  %mul = mul i64 %first_cap4, ptrtoint (ptr getelementptr (i64, ptr null, i32 1) to i64)
  %le = icmp ule i64 %mul, 1024
  br i1 %le, label %if.then5, label %if.end6

if.then5:                                         ; preds = %if.end3
  %first_cap7 = load i64, ptr %first_cap, align 8
  %mul8 = mul i64 %first_cap7, ptrtoint (ptr getelementptr (i64, ptr null, i32 1) to i64)
  %call9 = call ptr @_ZN4Page8allocateEmm(ptr %call, i64 %mul8, i64 8)
  %buffer10 = getelementptr inbounds nuw %_Z5ArrayImE, ptr %0, i32 0, i32 2
  store ptr %call9, ptr %buffer10, align 8
  %first_cap11 = load i64, ptr %first_cap, align 8
  %capacity = getelementptr inbounds nuw %_Z5ArrayImE, ptr %0, i32 0, i32 1
  store i64 %first_cap11, ptr %capacity, align 8
  ret void

if.end6:                                          ; preds = %if.end3
  %first_cap12 = load i64, ptr %first_cap, align 8
  %mul13 = mul i64 %first_cap12, ptrtoint (ptr getelementptr (i64, ptr null, i32 1) to i64)
  %call14 = call ptr @_ZN4Page25allocate_exclusive_bufferEmm(ptr %call, i64 %mul13, i64 8)
  %buffer15 = getelementptr inbounds nuw %_Z5ArrayImE, ptr %0, i32 0, i32 2
  store ptr %call14, ptr %buffer15, align 8
  %first_cap16 = load i64, ptr %first_cap, align 8
  %capacity17 = getelementptr inbounds nuw %_Z5ArrayImE, ptr %0, i32 0, i32 1
  store i64 %first_cap16, ptr %capacity17, align 8
  ret void

if.then31:                                        ; preds = %if.end
  %new_capacity33 = load i64, ptr %first_cap, align 8
  %mul34 = mul i64 %new_capacity33, ptrtoint (ptr getelementptr (i64, ptr null, i32 1) to i64)
  %call35 = call ptr @_ZN4Page8allocateEmm(ptr %call, i64 %mul34, i64 8)
  %buffer36 = getelementptr inbounds nuw %_Z5ArrayImE, ptr %0, i32 0, i32 2
  store ptr %call35, ptr %buffer36, align 8
  %new_capacity37 = load i64, ptr %first_cap, align 8
  %capacity38 = getelementptr inbounds nuw %_Z5ArrayImE, ptr %0, i32 0, i32 1
  store i64 %new_capacity37, ptr %capacity38, align 8
  %field.inplace = getelementptr inbounds nuw %_Z5ArrayImE, ptr %0, i32 0, i32 2
  %deref.recv = load ptr, ptr %field.inplace, align 8
  %mul39 = mul i64 %capacity25, ptrtoint (ptr getelementptr (i64, ptr null, i32 1) to i64)
  %call40 = call ptr @memcpy(ptr %deref.recv, ptr %buffer23, i64 %mul39)
  ret void

if.end32:                                         ; preds = %if.end
  %call41 = call ptr @_ZN4Page23allocate_exclusive_pageEv(ptr %call)
  %call42 = call i64 @_ZN4Page12get_capacityEm(ptr %call41, i64 8)
  call void @_ZN4Page25deallocate_exclusive_pageER4Page(ptr %call, ptr %call41)
  %new_capacity43 = load i64, ptr %first_cap, align 8
  %udiv44 = udiv i64 %call42, ptrtoint (ptr getelementptr (i64, ptr null, i32 1) to i64)
  %lt45 = icmp ult i64 %new_capacity43, %udiv44
  br i1 %lt45, label %if.then46, label %if.end47

if.then46:                                        ; preds = %if.end32
  %udiv48 = udiv i64 %call42, ptrtoint (ptr getelementptr (i64, ptr null, i32 1) to i64)
  store i64 %udiv48, ptr %first_cap, align 1
  br label %if.end47

if.end47:                                         ; preds = %if.then46, %if.end32
  %new_capacity49 = load i64, ptr %first_cap, align 8
  %mul50 = mul i64 %new_capacity49, ptrtoint (ptr getelementptr (i64, ptr null, i32 1) to i64)
  %call51 = call ptr @_ZN4Page25allocate_exclusive_bufferEmm(ptr %call, i64 %mul50, i64 8)
  %buffer52 = getelementptr inbounds nuw %_Z5ArrayImE, ptr %0, i32 0, i32 2
  store ptr %call51, ptr %buffer52, align 8
  %new_capacity53 = load i64, ptr %first_cap, align 8
  %capacity54 = getelementptr inbounds nuw %_Z5ArrayImE, ptr %0, i32 0, i32 1
  store i64 %new_capacity53, ptr %capacity54, align 8
  %field.inplace55 = getelementptr inbounds nuw %_Z5ArrayImE, ptr %0, i32 0, i32 2
  %deref.recv56 = load ptr, ptr %field.inplace55, align 8
  %mul57 = mul i64 %capacity25, ptrtoint (ptr getelementptr (i64, ptr null, i32 1) to i64)
  %call58 = call ptr @memcpy(ptr %deref.recv56, ptr %buffer23, i64 %mul57)
  %eq59 = icmp eq i1 %le21, false
  br i1 %eq59, label %if.then60, label %if.end61

if.then60:                                        ; preds = %if.end47
  %call62 = call ptr @_ZN4Page3getEPv(ptr %buffer23)
  call void @_ZN4Page25deallocate_exclusive_pageER4Page(ptr %call, ptr %call62)
  br label %if.end61

if.end61:                                         ; preds = %if.then60, %if.end47
  ret void
}

define linkonce_odr void @_ZN5ArrayImE3addEm(ptr %0, i64 %1) {
entry:
  %load.struct = load %_Z5ArrayImE, ptr %0, align 8
  %buffer = extractvalue %_Z5ArrayImE %load.struct, 2
  %eq = icmp eq ptr %buffer, null
  br i1 %eq, label %if.then, label %lor.rhs

if.then:                                          ; preds = %lor.rhs, %entry
  call void @_ZN5ArrayImE10reallocateEv(ptr %0)
  br label %if.end

if.end:                                           ; preds = %if.then, %lor.rhs
  %load.struct4 = load %_Z5ArrayImE, ptr %0, align 8
  %buffer5 = extractvalue %_Z5ArrayImE %load.struct4, 2
  %load.struct6 = load %_Z5ArrayImE, ptr %0, align 8
  %length7 = extractvalue %_Z5ArrayImE %load.struct6, 0
  %ptr.add = getelementptr inbounds i64, ptr %buffer5, i64 %length7
  store i64 %1, ptr %ptr.add, align 8
  %load.struct8 = load %_Z5ArrayImE, ptr %0, align 8
  %length9 = extractvalue %_Z5ArrayImE %load.struct8, 0
  %add = add i64 %length9, 1
  %length10 = getelementptr inbounds nuw %_Z5ArrayImE, ptr %0, i32 0, i32 0
  store i64 %add, ptr %length10, align 8
  ret void

lor.rhs:                                          ; preds = %entry
  %load.struct1 = load %_Z5ArrayImE, ptr %0, align 8
  %length = extractvalue %_Z5ArrayImE %load.struct1, 0
  %load.struct2 = load %_Z5ArrayImE, ptr %0, align 8
  %capacity = extractvalue %_Z5ArrayImE %load.struct2, 1
  %eq3 = icmp eq i64 %length, %capacity
  br i1 %eq3, label %if.then, label %if.end
}

define linkonce_odr ptr @_ZN6VectorImE3getEm(ptr noalias %0, i64 %1) {
entry:
  %load.struct = load %_Z6VectorImE, ptr %0, align 8
  %length = extractvalue %_Z6VectorImE %load.struct, 0
  %ge = icmp uge i64 %1, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %load.struct1 = load %_Z6VectorImE, ptr %0, align 8
  %data = extractvalue %_Z6VectorImE %load.struct1, 1
  %ptr.add = getelementptr inbounds i64, ptr %data, i64 %1
  ret ptr %ptr.add
}

define linkonce_odr ptr @_ZN6VectorImE2atEm(ptr noalias %0, i64 %1) {
entry:
  %load.struct = load %_Z6VectorImE, ptr %0, align 8
  %length = extractvalue %_Z6VectorImE %load.struct, 0
  %ge = icmp uge i64 %1, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %field.inplace = getelementptr inbounds nuw %_Z6VectorImE, ptr %0, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  call void @_Z15scaly_panic_oobP10const_charmm(ptr @.str.47, i64 %1, i64 %field.val)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %load.struct1 = load %_Z6VectorImE, ptr %0, align 8
  %data = extractvalue %_Z6VectorImE %load.struct1, 1
  %ptr.add = getelementptr inbounds i64, ptr %data, i64 %1
  ret ptr %ptr.add
}

define linkonce_odr ptr @_ZN6VectorImE7get_ptrEm(ptr noalias %0, i64 %1) {
entry:
  %load.struct = load %_Z6VectorImE, ptr %0, align 8
  %length = extractvalue %_Z6VectorImE %load.struct, 0
  %ge = icmp uge i64 %1, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %load.struct1 = load %_Z6VectorImE, ptr %0, align 8
  %data = extractvalue %_Z6VectorImE %load.struct1, 1
  %ptr.add = getelementptr inbounds i64, ptr %data, i64 %1
  ret ptr %ptr.add
}

define linkonce_odr void @_ZN6VectorImE3putEmm(ptr noalias %0, i64 %1, i64 %2) {
entry:
  %load.struct = load %_Z6VectorImE, ptr %0, align 8
  %length = extractvalue %_Z6VectorImE %load.struct, 0
  %ge = icmp uge i64 %1, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %field.inplace = getelementptr inbounds nuw %_Z6VectorImE, ptr %0, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  call void @_Z15scaly_panic_oobP10const_charmm(ptr @.str.48, i64 %1, i64 %field.val)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %load.struct1 = load %_Z6VectorImE, ptr %0, align 8
  %data = extractvalue %_Z6VectorImE %load.struct1, 1
  %ptr.add = getelementptr inbounds i64, ptr %data, i64 %1
  store i64 %2, ptr %ptr.add, align 8
  ret void
}

define linkonce_odr ptr @_ZN14VectorIteratorImE4nextEv(ptr %0) {
entry:
  %load.struct = load %_Z14VectorIteratorImE, ptr %0, align 8
  %vector = extractvalue %_Z14VectorIteratorImE %load.struct, 0
  %eq = icmp eq ptr %vector, null
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %load.struct1 = load %_Z14VectorIteratorImE, ptr %0, align 8
  %position = extractvalue %_Z14VectorIteratorImE %load.struct1, 1
  %load.struct2 = load %_Z14VectorIteratorImE, ptr %0, align 8
  %vector3 = extractvalue %_Z14VectorIteratorImE %load.struct2, 0
  %deref = load %_Z6VectorImE, ptr %vector3, align 8
  %length = extractvalue %_Z6VectorImE %deref, 0
  %eq4 = icmp eq i64 %position, %length
  br i1 %eq4, label %if.then5, label %if.end6

if.then5:                                         ; preds = %if.end
  ret ptr null

if.end6:                                          ; preds = %if.end
  %load.struct7 = load %_Z14VectorIteratorImE, ptr %0, align 8
  %position8 = extractvalue %_Z14VectorIteratorImE %load.struct7, 1
  %add = add i64 %position8, 1
  %position9 = getelementptr inbounds nuw %_Z14VectorIteratorImE, ptr %0, i32 0, i32 1
  store i64 %add, ptr %position9, align 8
  %field.inplace = getelementptr inbounds nuw %_Z14VectorIteratorImE, ptr %0, i32 0, i32 0
  %deref.recv = load ptr, ptr %field.inplace, align 8
  %load.struct10 = load %_Z14VectorIteratorImE, ptr %0, align 8
  %position11 = extractvalue %_Z14VectorIteratorImE %load.struct10, 1
  %sub = sub i64 %position11, 1
  %call = call ptr @_ZN6VectorImE7get_ptrEm(ptr %deref.recv, i64 %sub)
  ret ptr %call
}

define linkonce_odr void @_ZN14VectorIteratorImEC1E6OptionIR6VectorImEE(ptr %0, ptr %1) {
entry:
  %vector = getelementptr inbounds nuw %_Z14VectorIteratorImE, ptr %0, i32 0, i32 0
  store ptr %1, ptr %vector, align 8
  %position = getelementptr inbounds nuw %_Z14VectorIteratorImE, ptr %0, i32 0, i32 1
  store i64 0, ptr %position, align 8
  ret void
}

define linkonce_odr void @_ZN6VectorImE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z14VectorIteratorImE) %0, ptr %1, ptr noalias %2) {
entry:
  %struct.init = alloca %_Z14VectorIteratorImE, align 8
  call void @_ZN14VectorIteratorImEC1E6OptionIR6VectorImEE(ptr %struct.init, ptr %2)
  %sret.body = load %_Z14VectorIteratorImE, ptr %struct.init, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %struct.init, i64 ptrtoint (ptr getelementptr (%_Z14VectorIteratorImE, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr ptr @_ZN5SliceImE3getEm(ptr %0, i64 %1) {
entry:
  %load.struct = load %_Z5SliceImE, ptr %0, align 8
  %length = extractvalue %_Z5SliceImE %load.struct, 0
  %ge = icmp uge i64 %1, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %load.struct1 = load %_Z5SliceImE, ptr %0, align 8
  %data = extractvalue %_Z5SliceImE %load.struct1, 1
  %ptr.add = getelementptr inbounds i64, ptr %data, i64 %1
  ret ptr %ptr.add
}

define linkonce_odr ptr @_ZN5SliceImE2atEm(ptr %0, i64 %1) {
entry:
  %load.struct = load %_Z5SliceImE, ptr %0, align 8
  %length = extractvalue %_Z5SliceImE %load.struct, 0
  %ge = icmp uge i64 %1, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %field.inplace = getelementptr inbounds nuw %_Z5SliceImE, ptr %0, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  call void @_Z15scaly_panic_oobP10const_charmm(ptr @.str.49, i64 %1, i64 %field.val)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %load.struct1 = load %_Z5SliceImE, ptr %0, align 8
  %data = extractvalue %_Z5SliceImE %load.struct1, 1
  %ptr.add = getelementptr inbounds i64, ptr %data, i64 %1
  ret ptr %ptr.add
}

define linkonce_odr void @_ZN5SliceImE3putEmm(ptr %0, i64 %1, i64 %2) {
entry:
  %load.struct = load %_Z5SliceImE, ptr %0, align 8
  %length = extractvalue %_Z5SliceImE %load.struct, 0
  %ge = icmp uge i64 %1, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %field.inplace = getelementptr inbounds nuw %_Z5SliceImE, ptr %0, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  call void @_Z15scaly_panic_oobP10const_charmm(ptr @.str.50, i64 %1, i64 %field.val)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %load.struct1 = load %_Z5SliceImE, ptr %0, align 8
  %data = extractvalue %_Z5SliceImE %load.struct1, 1
  %ptr.add = getelementptr inbounds i64, ptr %data, i64 %1
  store i64 %2, ptr %ptr.add, align 8
  ret void
}

define linkonce_odr i1 @_ZN5SliceImE8is_emptyEv(ptr %0) {
entry:
  %load.struct = load %_Z5SliceImE, ptr %0, align 8
  %length = extractvalue %_Z5SliceImE %load.struct, 0
  %eq = icmp eq i64 %length, 0
  ret i1 %eq
}

define linkonce_odr void @_ZN5SliceImE8subsliceEmm(ptr noalias sret(%_Z5SliceImE) %0, ptr %1, i64 %2, i64 %3) {
entry:
  %tuple = alloca %_Z5SliceImE, align 8
  %from = alloca i64, align 8
  store i64 %2, ptr %from, align 1
  %to = alloca i64, align 8
  store i64 %3, ptr %to, align 1
  %from1 = load i64, ptr %from, align 8
  %load.struct = load %_Z5SliceImE, ptr %1, align 8
  %length = extractvalue %_Z5SliceImE %load.struct, 0
  %gt = icmp ugt i64 %from1, %length
  br i1 %gt, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %load.struct2 = load %_Z5SliceImE, ptr %1, align 8
  %length3 = extractvalue %_Z5SliceImE %load.struct2, 0
  store i64 %length3, ptr %from, align 1
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %to4 = load i64, ptr %to, align 8
  %load.struct5 = load %_Z5SliceImE, ptr %1, align 8
  %length6 = extractvalue %_Z5SliceImE %load.struct5, 0
  %gt7 = icmp ugt i64 %to4, %length6
  br i1 %gt7, label %if.then8, label %if.end9

if.then8:                                         ; preds = %if.end
  %load.struct10 = load %_Z5SliceImE, ptr %1, align 8
  %length11 = extractvalue %_Z5SliceImE %load.struct10, 0
  store i64 %length11, ptr %to, align 1
  br label %if.end9

if.end9:                                          ; preds = %if.then8, %if.end
  %from12 = load i64, ptr %from, align 8
  %to13 = load i64, ptr %to, align 8
  %gt14 = icmp ugt i64 %from12, %to13
  br i1 %gt14, label %if.then15, label %if.end16

if.then15:                                        ; preds = %if.end9
  %to17 = load i64, ptr %to, align 8
  store i64 %to17, ptr %from, align 1
  br label %if.end16

if.end16:                                         ; preds = %if.then15, %if.end9
  %to18 = load i64, ptr %to, align 8
  %from19 = load i64, ptr %from, align 8
  %sub = sub i64 %to18, %from19
  %load.struct20 = load %_Z5SliceImE, ptr %1, align 8
  %data = extractvalue %_Z5SliceImE %load.struct20, 1
  %from21 = load i64, ptr %from, align 8
  %ptr.add = getelementptr inbounds i64, ptr %data, i64 %from21
  %tuple.field = getelementptr inbounds nuw %_Z5SliceImE, ptr %tuple, i32 0, i32 0
  store i64 %sub, ptr %tuple.field, align 1
  %tuple.field22 = getelementptr inbounds nuw %_Z5SliceImE, ptr %tuple, i32 0, i32 1
  store ptr %ptr.add, ptr %tuple.field22, align 1
  %tuple.val = load %_Z5SliceImE, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z5SliceImE, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN5SliceImE10slice_fromEPN4scaly6memory4PageEm(ptr noalias sret(%_Z5SliceImE) %0, ptr %1, ptr %2, i64 %3) {
entry:
  %sret.result = alloca %_Z5SliceImE, align 8
  %field.inplace = getelementptr inbounds nuw %_Z5SliceImE, ptr %2, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  call void @_ZN5SliceImE8subsliceEmm(ptr noalias sret(%_Z5SliceImE) %sret.result, ptr %2, i64 %3, i64 %field.val)
  %sret.body = load %_Z5SliceImE, ptr %sret.result, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %sret.result, i64 ptrtoint (ptr getelementptr (%_Z5SliceImE, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN5SliceImE8slice_toEPN4scaly6memory4PageEm(ptr noalias sret(%_Z5SliceImE) %0, ptr %1, ptr %2, i64 %3) {
entry:
  %sret.result = alloca %_Z5SliceImE, align 8
  call void @_ZN5SliceImE8subsliceEmm(ptr noalias sret(%_Z5SliceImE) %sret.result, ptr %2, i64 0, i64 %3)
  %sret.body = load %_Z5SliceImE, ptr %sret.result, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %sret.result, i64 ptrtoint (ptr getelementptr (%_Z5SliceImE, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr i1 @_ZN5SliceImE6equalsE5SliceImE(ptr %0, ptr %1) {
entry:
  %load.struct = load %_Z5SliceImE, ptr %0, align 8
  %length = extractvalue %_Z5SliceImE %load.struct, 0
  %load.struct1 = load %_Z5SliceImE, ptr %1, align 8
  %length2 = extractvalue %_Z5SliceImE %load.struct1, 0
  %ne = icmp ne i64 %length, %length2
  br i1 %ne, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret i1 false

if.end:                                           ; preds = %entry
  %load.struct3 = load %_Z5SliceImE, ptr %0, align 8
  %length4 = extractvalue %_Z5SliceImE %load.struct3, 0
  %eq = icmp eq i64 %length4, 0
  br i1 %eq, label %if.then5, label %if.end6

if.then5:                                         ; preds = %if.end
  ret i1 true

if.end6:                                          ; preds = %if.end
  %load.struct7 = load %_Z5SliceImE, ptr %0, align 8
  %data = extractvalue %_Z5SliceImE %load.struct7, 1
  %load.struct8 = load %_Z5SliceImE, ptr %1, align 8
  %data9 = extractvalue %_Z5SliceImE %load.struct8, 1
  %load.struct10 = load %_Z5SliceImE, ptr %0, align 8
  %length11 = extractvalue %_Z5SliceImE %load.struct10, 0
  %mul = mul i64 %length11, ptrtoint (ptr getelementptr (i64, ptr null, i32 1) to i64)
  %call = call i32 @memcmp(ptr %data, ptr %data9, i64 %mul)
  %eq12 = icmp eq i32 %call, 0
  ret i1 %eq12
}

define linkonce_odr i1 @_ZN5SliceImE11starts_withE5SliceImE(ptr %0, ptr %1) {
entry:
  %sret.result = alloca %_Z5SliceImE, align 8
  %load.struct = load %_Z5SliceImE, ptr %1, align 8
  %length = extractvalue %_Z5SliceImE %load.struct, 0
  %load.struct1 = load %_Z5SliceImE, ptr %0, align 8
  %length2 = extractvalue %_Z5SliceImE %load.struct1, 0
  %gt = icmp ugt i64 %length, %length2
  br i1 %gt, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret i1 false

if.end:                                           ; preds = %entry
  %field.inplace = getelementptr inbounds nuw %_Z5SliceImE, ptr %1, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  call void @_ZN5SliceImE8subsliceEmm(ptr noalias sret(%_Z5SliceImE) %sret.result, ptr %0, i64 0, i64 %field.val)
  %call = call i1 @_ZN5SliceImE6equalsE5SliceImE(ptr %sret.result, ptr %1)
  ret i1 %call
}

define linkonce_odr i1 @_ZN5SliceImE9ends_withE5SliceImE(ptr %0, ptr %1) {
entry:
  %sret.result = alloca %_Z5SliceImE, align 8
  %load.struct = load %_Z5SliceImE, ptr %1, align 8
  %length = extractvalue %_Z5SliceImE %load.struct, 0
  %load.struct1 = load %_Z5SliceImE, ptr %0, align 8
  %length2 = extractvalue %_Z5SliceImE %load.struct1, 0
  %gt = icmp ugt i64 %length, %length2
  br i1 %gt, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret i1 false

if.end:                                           ; preds = %entry
  %load.struct3 = load %_Z5SliceImE, ptr %0, align 8
  %length4 = extractvalue %_Z5SliceImE %load.struct3, 0
  %load.struct5 = load %_Z5SliceImE, ptr %1, align 8
  %length6 = extractvalue %_Z5SliceImE %load.struct5, 0
  %sub = sub i64 %length4, %length6
  %field.inplace = getelementptr inbounds nuw %_Z5SliceImE, ptr %0, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  call void @_ZN5SliceImE8subsliceEmm(ptr noalias sret(%_Z5SliceImE) %sret.result, ptr %0, i64 %sub, i64 %field.val)
  %call = call i1 @_ZN5SliceImE6equalsE5SliceImE(ptr %sret.result, ptr %1)
  ret i1 %call
}

define linkonce_odr ptr @_ZN13SliceIteratorImE4nextEv(ptr %0) {
entry:
  %load.struct = load %_Z13SliceIteratorImE, ptr %0, align 8
  %position = extractvalue %_Z13SliceIteratorImE %load.struct, 1
  %load.struct1 = load %_Z13SliceIteratorImE, ptr %0, align 8
  %slice = extractvalue %_Z13SliceIteratorImE %load.struct1, 0
  %length = extractvalue %_Z5SliceImE %slice, 0
  %ge = icmp uge i64 %position, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %load.struct2 = load %_Z13SliceIteratorImE, ptr %0, align 8
  %slice3 = extractvalue %_Z13SliceIteratorImE %load.struct2, 0
  %data = extractvalue %_Z5SliceImE %slice3, 1
  %load.struct4 = load %_Z13SliceIteratorImE, ptr %0, align 8
  %position5 = extractvalue %_Z13SliceIteratorImE %load.struct4, 1
  %ptr.add = getelementptr inbounds i64, ptr %data, i64 %position5
  %load.struct6 = load %_Z13SliceIteratorImE, ptr %0, align 8
  %position7 = extractvalue %_Z13SliceIteratorImE %load.struct6, 1
  %add = add i64 %position7, 1
  %position8 = getelementptr inbounds nuw %_Z13SliceIteratorImE, ptr %0, i32 0, i32 1
  store i64 %add, ptr %position8, align 8
  ret ptr %ptr.add
}

define linkonce_odr void @_ZN13SliceIteratorImEC1E5SliceImE(ptr %0, ptr %1) {
entry:
  %slice = getelementptr inbounds nuw %_Z13SliceIteratorImE, ptr %0, i32 0, i32 0
  %field.load = load %_Z5SliceImE, ptr %1, align 8
  store %_Z5SliceImE %field.load, ptr %slice, align 8
  %position = getelementptr inbounds nuw %_Z13SliceIteratorImE, ptr %0, i32 0, i32 1
  store i64 0, ptr %position, align 8
  ret void
}

define linkonce_odr void @_ZN5SliceImE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z13SliceIteratorImE) %0, ptr %1, ptr %2) {
entry:
  %struct.init = alloca %_Z13SliceIteratorImE, align 8
  call void @_ZN13SliceIteratorImEC1E5SliceImE(ptr %struct.init, ptr %2)
  %sret.body = load %_Z13SliceIteratorImE, ptr %struct.init, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %struct.init, i64 ptrtoint (ptr getelementptr (%_Z13SliceIteratorImE, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN5SliceImEC1Ev(ptr %0) {
entry:
  %data = getelementptr inbounds nuw %_Z5SliceImE, ptr %0, i32 0, i32 1
  store ptr null, ptr %data, align 8
  %length = getelementptr inbounds nuw %_Z5SliceImE, ptr %0, i32 0, i32 0
  store i64 0, ptr %length, align 8
  ret void
}

define linkonce_odr void @_ZN6VectorImE8as_sliceEv(ptr noalias sret(%_Z5SliceImE) %0, ptr noalias %1) {
entry:
  %load.struct = load %_Z6VectorImE, ptr %1, align 8
  %length = extractvalue %_Z6VectorImE %load.struct, 0
  %load.struct1 = load %_Z6VectorImE, ptr %1, align 8
  %data = extractvalue %_Z6VectorImE %load.struct1, 1
  %tuple = alloca %_Z5SliceImE, align 8
  %tuple.field = getelementptr inbounds nuw %_Z5SliceImE, ptr %tuple, i32 0, i32 0
  store i64 %length, ptr %tuple.field, align 1
  %tuple.field2 = getelementptr inbounds nuw %_Z5SliceImE, ptr %tuple, i32 0, i32 1
  store ptr %data, ptr %tuple.field2, align 1
  %tuple.val = load %_Z5SliceImE, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z5SliceImE, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN6VectorImEC1Ev(ptr noalias %0) {
entry:
  %length = getelementptr inbounds nuw %_Z6VectorImE, ptr %0, i32 0, i32 0
  store i64 0, ptr %length, align 8
  %data = getelementptr inbounds nuw %_Z6VectorImE, ptr %0, i32 0, i32 1
  store ptr null, ptr %data, align 8
  ret void
}

define linkonce_odr void @_ZN6VectorImEC1Em(ptr %0, i64 %1) {
entry:
  %call = call ptr @_ZN4Page3getEPv(ptr %0)
  %length = getelementptr inbounds nuw %_Z6VectorImE, ptr %0, i32 0, i32 0
  store i64 %1, ptr %length, align 8
  %gt = icmp ugt i64 %1, 0
  br i1 %gt, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %mul = mul i64 %1, ptrtoint (ptr getelementptr (i64, ptr null, i32 1) to i64)
  %call1 = call ptr @_ZN4Page8allocateEmm(ptr %call, i64 %mul, i64 8)
  %data = getelementptr inbounds nuw %_Z6VectorImE, ptr %0, i32 0, i32 1
  store ptr %call1, ptr %data, align 8
  %field.inplace = getelementptr inbounds nuw %_Z6VectorImE, ptr %0, i32 0, i32 1
  %deref.recv = load ptr, ptr %field.inplace, align 8
  %mul2 = mul i64 %1, ptrtoint (ptr getelementptr (i64, ptr null, i32 1) to i64)
  %call3 = call ptr @memset(ptr %deref.recv, i32 0, i64 %mul2)
  br label %if.end

if.else:                                          ; preds = %entry
  %data4 = getelementptr inbounds nuw %_Z6VectorImE, ptr %0, i32 0, i32 1
  store ptr null, ptr %data4, align 8
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  %if.value = phi ptr [ %call3, %if.then ], [ null, %if.else ]
  ret void
}

define linkonce_odr void @_ZN6VectorImEC1E6VectorImE(ptr %0, ptr %1) {
entry:
  %call = call ptr @_ZN4Page3getEPv(ptr %0)
  %load.struct = load %_Z6VectorImE, ptr %1, align 8
  %length = extractvalue %_Z6VectorImE %load.struct, 0
  %length1 = getelementptr inbounds nuw %_Z6VectorImE, ptr %0, i32 0, i32 0
  store i64 %length, ptr %length1, align 8
  %load.struct2 = load %_Z6VectorImE, ptr %0, align 8
  %length3 = extractvalue %_Z6VectorImE %load.struct2, 0
  %gt = icmp ugt i64 %length3, 0
  br i1 %gt, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %load.struct4 = load %_Z6VectorImE, ptr %0, align 8
  %length5 = extractvalue %_Z6VectorImE %load.struct4, 0
  %mul = mul i64 %length5, ptrtoint (ptr getelementptr (i64, ptr null, i32 1) to i64)
  %call6 = call ptr @_ZN4Page8allocateEmm(ptr %call, i64 %mul, i64 8)
  %data = getelementptr inbounds nuw %_Z6VectorImE, ptr %0, i32 0, i32 1
  store ptr %call6, ptr %data, align 8
  %field.inplace = getelementptr inbounds nuw %_Z6VectorImE, ptr %0, i32 0, i32 1
  %deref.recv = load ptr, ptr %field.inplace, align 8
  %field.inplace7 = getelementptr inbounds nuw %_Z6VectorImE, ptr %1, i32 0, i32 1
  %deref.recv8 = load ptr, ptr %field.inplace7, align 8
  %load.struct9 = load %_Z6VectorImE, ptr %0, align 8
  %length10 = extractvalue %_Z6VectorImE %load.struct9, 0
  %mul11 = mul i64 %length10, ptrtoint (ptr getelementptr (i64, ptr null, i32 1) to i64)
  %call12 = call ptr @memcpy(ptr %deref.recv, ptr %deref.recv8, i64 %mul11)
  br label %if.end

if.else:                                          ; preds = %entry
  %data13 = getelementptr inbounds nuw %_Z6VectorImE, ptr %0, i32 0, i32 1
  store ptr null, ptr %data13, align 8
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  %if.value = phi ptr [ %call12, %if.then ], [ null, %if.else ]
  ret void
}

define linkonce_odr void @_ZN6VectorImEC1E5ArrayImE(ptr %0, ptr %1) {
entry:
  %call = call ptr @_ZN4Page3getEPv(ptr %0)
  %load.struct = load %_Z5ArrayImE, ptr %1, align 8
  %length = extractvalue %_Z5ArrayImE %load.struct, 0
  %length1 = getelementptr inbounds nuw %_Z6VectorImE, ptr %0, i32 0, i32 0
  store i64 %length, ptr %length1, align 8
  %load.struct2 = load %_Z6VectorImE, ptr %0, align 8
  %length3 = extractvalue %_Z6VectorImE %load.struct2, 0
  %gt = icmp ugt i64 %length3, 0
  br i1 %gt, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %load.struct4 = load %_Z6VectorImE, ptr %0, align 8
  %length5 = extractvalue %_Z6VectorImE %load.struct4, 0
  %mul = mul i64 %length5, ptrtoint (ptr getelementptr (i64, ptr null, i32 1) to i64)
  %call6 = call ptr @_ZN4Page8allocateEmm(ptr %call, i64 %mul, i64 8)
  %data = getelementptr inbounds nuw %_Z6VectorImE, ptr %0, i32 0, i32 1
  store ptr %call6, ptr %data, align 8
  %field.inplace = getelementptr inbounds nuw %_Z6VectorImE, ptr %0, i32 0, i32 1
  %deref.recv = load ptr, ptr %field.inplace, align 8
  %call7 = call ptr @_ZN5ArrayImE10get_bufferEv(ptr %1)
  %load.struct8 = load %_Z6VectorImE, ptr %0, align 8
  %length9 = extractvalue %_Z6VectorImE %load.struct8, 0
  %mul10 = mul i64 %length9, ptrtoint (ptr getelementptr (i64, ptr null, i32 1) to i64)
  %call11 = call ptr @memcpy(ptr %deref.recv, ptr %call7, i64 %mul10)
  br label %if.end

if.else:                                          ; preds = %entry
  %data12 = getelementptr inbounds nuw %_Z6VectorImE, ptr %0, i32 0, i32 1
  store ptr null, ptr %data12, align 8
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  %if.value = phi ptr [ %call11, %if.then ], [ null, %if.else ]
  ret void
}

define linkonce_odr ptr @_ZN4ListImE8get_headEv(ptr %0) {
entry:
  %load.struct = load %_Z4ListImE, ptr %0, align 8
  %head = extractvalue %_Z4ListImE %load.struct, 0
  %eq = icmp eq ptr %head, null
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %addr.gep = getelementptr inbounds nuw %_Z4ListImE, ptr %0, i32 0, i32 0
  %addr.hop = load ptr, ptr %addr.gep, align 8
  %addr.gep1 = getelementptr inbounds nuw %_Z4NodeImE, ptr %addr.hop, i32 0, i32 0
  ret ptr %addr.gep1
}

define linkonce_odr ptr @_ZN12ListIteratorImE4nextEv(ptr %0) {
entry:
  %old_current = alloca ptr, align 8
  %load.struct = load %_Z12ListIteratorImE, ptr %0, align 8
  %current = extractvalue %_Z12ListIteratorImE %load.struct, 0
  %ne = icmp ne ptr %current, null
  br i1 %ne, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %load.struct1 = load %_Z12ListIteratorImE, ptr %0, align 8
  %current2 = extractvalue %_Z12ListIteratorImE %load.struct1, 0
  store ptr %current2, ptr %old_current, align 1
  %load.struct3 = load %_Z12ListIteratorImE, ptr %0, align 8
  %current4 = extractvalue %_Z12ListIteratorImE %load.struct3, 0
  %deref = load %_Z4NodeImE, ptr %current4, align 8
  %next = extractvalue %_Z4NodeImE %deref, 1
  %current5 = getelementptr inbounds nuw %_Z12ListIteratorImE, ptr %0, i32 0, i32 0
  store ptr %next, ptr %current5, align 8
  %old_current6 = load ptr, ptr %old_current, align 8
  %addr.gep = getelementptr inbounds nuw %_Z4NodeImE, ptr %old_current6, i32 0, i32 0
  ret ptr %addr.gep

if.else:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; No predecessors!
  ret ptr null
}

define linkonce_odr i64 @_ZN4ListImE5countEv(ptr %0) {
entry:
  %sret.result = alloca %_Z12ListIteratorImE, align 8
  call void @_ZN4ListImE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z12ListIteratorImE) %sret.result, ptr null, ptr %0)
  %list_iterator = alloca ptr, align 8
  store ptr %sret.result, ptr %list_iterator, align 1
  %i = alloca i64, align 8
  store i64 0, ptr %i, align 1
  br label %while.cond

while.cond:                                       ; preds = %while.body, %entry
  %list_iterator1 = load ptr, ptr %list_iterator, align 8
  %call = call ptr @_ZN12ListIteratorImE4nextEv(ptr %list_iterator1)
  %ne = icmp ne ptr %call, null
  br i1 %ne, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %i2 = load i64, ptr %i, align 8
  %add = add i64 %i2, 1
  store i64 %add, ptr %i, align 1
  br label %while.cond

while.exit:                                       ; preds = %while.cond
  %i3 = load i64, ptr %i, align 8
  ret i64 %i3
}

define linkonce_odr void @_ZN4ListImE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z12ListIteratorImE) %0, ptr %1, ptr %2) {
entry:
  %load.struct = load %_Z4ListImE, ptr %2, align 8
  %head = extractvalue %_Z4ListImE %load.struct, 0
  %tuple = alloca %_Z12ListIteratorImE, align 8
  %tuple.field = getelementptr inbounds nuw %_Z12ListIteratorImE, ptr %tuple, i32 0, i32 0
  store ptr %head, ptr %tuple.field, align 1
  %tuple.val = load %_Z12ListIteratorImE, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z12ListIteratorImE, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN4ListImE4linkEP4NodeImE(ptr %0, ptr %1) {
entry:
  %load.struct = load %_Z4ListImE, ptr %0, align 8
  %tail = extractvalue %_Z4ListImE %load.struct, 1
  %eq = icmp eq ptr %tail, null
  br i1 %eq, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %head = getelementptr inbounds nuw %_Z4ListImE, ptr %0, i32 0, i32 0
  store ptr %1, ptr %head, align 8
  br label %if.end

if.else:                                          ; preds = %entry
  %tail1 = getelementptr inbounds nuw %_Z4ListImE, ptr %0, i32 0, i32 1
  %field.deref = load ptr, ptr %tail1, align 8
  %next = getelementptr inbounds nuw %_Z4NodeImE, ptr %field.deref, i32 0, i32 1
  store ptr %1, ptr %next, align 8
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  %if.value = phi ptr [ %1, %if.then ], [ %1, %if.else ]
  %tail2 = getelementptr inbounds nuw %_Z4ListImE, ptr %0, i32 0, i32 1
  store ptr %1, ptr %tail2, align 8
  ret void
}

define linkonce_odr void @_ZN4ListImE3addEm(ptr %0, i64 %1) {
entry:
  %own_page = call ptr @_Z3getPv(ptr %0)
  %tuple.region = call ptr @_ZN4Page8allocateEmm(ptr %own_page, i64 ptrtoint (ptr getelementptr (%_Z4NodeImE, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr ({ i1, %_Z4NodeImE }, ptr null, i64 0, i32 1) to i64))
  %tuple.field = getelementptr inbounds nuw %_Z4NodeImE, ptr %tuple.region, i32 0, i32 0
  store i64 %1, ptr %tuple.field, align 1
  %tuple.field1 = getelementptr inbounds nuw %_Z4NodeImE, ptr %tuple.region, i32 0, i32 1
  store ptr null, ptr %tuple.field1, align 1
  call void @_ZN4ListImE4linkEP4NodeImE(ptr %0, ptr %tuple.region)
  ret void
}

define linkonce_odr void @_ZN4ListImE6add_onER4Pagem(ptr %0, ptr %1, i64 %2) {
entry:
  %tuple.region = call ptr @_ZN4Page8allocateEmm(ptr %1, i64 ptrtoint (ptr getelementptr (%_Z4NodeImE, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr ({ i1, %_Z4NodeImE }, ptr null, i64 0, i32 1) to i64))
  %tuple.field = getelementptr inbounds nuw %_Z4NodeImE, ptr %tuple.region, i32 0, i32 0
  store i64 %2, ptr %tuple.field, align 1
  %tuple.field1 = getelementptr inbounds nuw %_Z4NodeImE, ptr %tuple.region, i32 0, i32 1
  store ptr null, ptr %tuple.field1, align 1
  call void @_ZN4ListImE4linkEP4NodeImE(ptr %0, ptr %tuple.region)
  ret void
}

define linkonce_odr void @_ZN6VectorImEC1E4ListImE(ptr %0, ptr %1) {
entry:
  %i = alloca i64, align 8
  %list_iterator = alloca ptr, align 8
  %sret.result = alloca %_Z12ListIteratorImE, align 8
  %call = call ptr @_ZN4Page3getEPv(ptr %0)
  %call1 = call i64 @_ZN4ListImE5countEv(ptr %1)
  %length = getelementptr inbounds nuw %_Z6VectorImE, ptr %0, i32 0, i32 0
  store i64 %call1, ptr %length, align 8
  %load.struct = load %_Z6VectorImE, ptr %0, align 8
  %length2 = extractvalue %_Z6VectorImE %load.struct, 0
  %gt = icmp ugt i64 %length2, 0
  br i1 %gt, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %load.struct3 = load %_Z6VectorImE, ptr %0, align 8
  %length4 = extractvalue %_Z6VectorImE %load.struct3, 0
  %mul = mul i64 %length4, ptrtoint (ptr getelementptr (i64, ptr null, i32 1) to i64)
  %call5 = call ptr @_ZN4Page8allocateEmm(ptr %call, i64 %mul, i64 8)
  %data = getelementptr inbounds nuw %_Z6VectorImE, ptr %0, i32 0, i32 1
  store ptr %call5, ptr %data, align 8
  call void @_ZN4ListImE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z12ListIteratorImE) %sret.result, ptr null, ptr %1)
  store ptr %sret.result, ptr %list_iterator, align 1
  store i64 0, ptr %i, align 1
  br label %while.cond

if.else:                                          ; preds = %entry
  %data12 = getelementptr inbounds nuw %_Z6VectorImE, ptr %0, i32 0, i32 1
  store ptr null, ptr %data12, align 8
  br label %if.end

if.end:                                           ; preds = %if.else, %while.exit
  ret void

while.cond:                                       ; preds = %while.body, %if.then
  %list_iterator6 = load ptr, ptr %list_iterator, align 8
  %call7 = call ptr @_ZN12ListIteratorImE4nextEv(ptr %list_iterator6)
  %while.tobool = icmp ne ptr %call7, null
  br i1 %while.tobool, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %deref = load i64, ptr %call7, align 8
  %load.struct8 = load %_Z6VectorImE, ptr %0, align 8
  %data9 = extractvalue %_Z6VectorImE %load.struct8, 1
  %i10 = load i64, ptr %i, align 8
  %ptr.add = getelementptr inbounds i64, ptr %data9, i64 %i10
  store i64 %deref, ptr %ptr.add, align 8
  %i11 = load i64, ptr %i, align 8
  %add = add i64 %i11, 1
  store i64 %add, ptr %i, align 1
  br label %while.cond

while.exit:                                       ; preds = %while.cond
  br label %if.end
}

define linkonce_odr void @_ZN5ArrayImE7add_runEmPm(ptr %0, i64 %1, ptr %2) {
entry:
  %load.struct = load %_Z5ArrayImE, ptr %0, align 8
  %length = extractvalue %_Z5ArrayImE %load.struct, 0
  %add = add i64 %length, %1
  %new_length = alloca i64, align 8
  store i64 %add, ptr %new_length, align 1
  %new_length1 = load i64, ptr %new_length, align 8
  %load.struct2 = load %_Z5ArrayImE, ptr %0, align 8
  %length3 = extractvalue %_Z5ArrayImE %load.struct2, 0
  %lt = icmp ult i64 %new_length1, %length3
  br i1 %lt, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %field.inplace = getelementptr inbounds nuw %_Z5ArrayImE, ptr %0, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  call void @_Z16scaly_panic_sizeP10const_charmm(ptr @.str.51, i64 %1, i64 %field.val)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %load.struct6 = load %_Z5ArrayImE, ptr %0, align 8
  %buffer = extractvalue %_Z5ArrayImE %load.struct6, 2
  %eq = icmp eq ptr %buffer, null
  br i1 %eq, label %if.then4, label %lor.rhs

if.then4:                                         ; preds = %lor.rhs, %if.end
  %new_length9 = load i64, ptr %new_length, align 8
  call void @_ZN5ArrayImE7grow_toEm(ptr %0, i64 %new_length9)
  br label %if.end5

if.end5:                                          ; preds = %if.then4, %lor.rhs
  %gt10 = icmp ugt i64 %1, 0
  br i1 %gt10, label %if.then11, label %if.end12

lor.rhs:                                          ; preds = %if.end
  %new_length7 = load i64, ptr %new_length, align 8
  %load.struct8 = load %_Z5ArrayImE, ptr %0, align 8
  %capacity = extractvalue %_Z5ArrayImE %load.struct8, 1
  %gt = icmp ugt i64 %new_length7, %capacity
  br i1 %gt, label %if.then4, label %if.end5

if.then11:                                        ; preds = %if.end5
  %load.struct13 = load %_Z5ArrayImE, ptr %0, align 8
  %buffer14 = extractvalue %_Z5ArrayImE %load.struct13, 2
  %load.struct15 = load %_Z5ArrayImE, ptr %0, align 8
  %length16 = extractvalue %_Z5ArrayImE %load.struct15, 0
  %ptr.add = getelementptr inbounds i64, ptr %buffer14, i64 %length16
  %mul = mul i64 %1, ptrtoint (ptr getelementptr (i64, ptr null, i32 1) to i64)
  %call = call ptr @memcpy(ptr %ptr.add, ptr %2, i64 %mul)
  br label %if.end12

if.end12:                                         ; preds = %if.then11, %if.end5
  %load.struct17 = load %_Z5ArrayImE, ptr %0, align 8
  %length18 = extractvalue %_Z5ArrayImE %load.struct17, 0
  %add19 = add i64 %length18, %1
  %length20 = getelementptr inbounds nuw %_Z5ArrayImE, ptr %0, i32 0, i32 0
  store i64 %add19, ptr %length20, align 8
  ret void
}

define linkonce_odr void @_ZN5ArrayImE3addE6VectorImE(ptr %0, ptr %1) {
entry:
  %field.inplace = getelementptr inbounds nuw %_Z6VectorImE, ptr %1, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  %field.inplace1 = getelementptr inbounds nuw %_Z6VectorImE, ptr %1, i32 0, i32 1
  %deref.recv = load ptr, ptr %field.inplace1, align 8
  call void @_ZN5ArrayImE7add_runEmPm(ptr %0, i64 %field.val, ptr %deref.recv)
  ret void
}

define linkonce_odr void @_ZN5ArrayImE3addE5SliceImE(ptr %0, ptr %1) {
entry:
  %field.inplace = getelementptr inbounds nuw %_Z5SliceImE, ptr %1, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  %field.inplace1 = getelementptr inbounds nuw %_Z5SliceImE, ptr %1, i32 0, i32 1
  %deref.recv = load ptr, ptr %field.inplace1, align 8
  call void @_ZN5ArrayImE7add_runEmPm(ptr %0, i64 %field.val, ptr %deref.recv)
  ret void
}

define linkonce_odr void @_ZN5ArrayImE7grow_toEm(ptr %0, i64 %1) {
entry:
  call void @_ZN5ArrayImE10reallocateEv(ptr %0)
  %load.struct = load %_Z5ArrayImE, ptr %0, align 8
  %capacity = extractvalue %_Z5ArrayImE %load.struct, 1
  %gt = icmp ugt i64 %1, %capacity
  br i1 %gt, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %call = call ptr @_ZN4Page3getEPv(ptr %0)
  %load.struct1 = load %_Z5ArrayImE, ptr %0, align 8
  %buffer = extractvalue %_Z5ArrayImE %load.struct1, 2
  %load.struct2 = load %_Z5ArrayImE, ptr %0, align 8
  %capacity3 = extractvalue %_Z5ArrayImE %load.struct2, 1
  %mul = mul i64 %capacity3, ptrtoint (ptr getelementptr (i64, ptr null, i32 1) to i64)
  %le = icmp ule i64 %mul, 1024
  %mul4 = mul i64 %1, ptrtoint (ptr getelementptr (i64, ptr null, i32 1) to i64)
  %le5 = icmp ule i64 %mul4, 1024
  br i1 %le5, label %if.then6, label %if.else

if.end:                                           ; preds = %if.end24, %entry
  ret void

if.then6:                                         ; preds = %if.then
  %mul8 = mul i64 %1, ptrtoint (ptr getelementptr (i64, ptr null, i32 1) to i64)
  %call9 = call ptr @_ZN4Page8allocateEmm(ptr %call, i64 %mul8, i64 8)
  %buffer10 = getelementptr inbounds nuw %_Z5ArrayImE, ptr %0, i32 0, i32 2
  store ptr %call9, ptr %buffer10, align 8
  br label %if.end7

if.else:                                          ; preds = %if.then
  %mul11 = mul i64 %1, ptrtoint (ptr getelementptr (i64, ptr null, i32 1) to i64)
  %call12 = call ptr @_ZN4Page25allocate_exclusive_bufferEmm(ptr %call, i64 %mul11, i64 8)
  %buffer13 = getelementptr inbounds nuw %_Z5ArrayImE, ptr %0, i32 0, i32 2
  store ptr %call12, ptr %buffer13, align 8
  br label %if.end7

if.end7:                                          ; preds = %if.else, %if.then6
  %if.value = phi ptr [ %call9, %if.then6 ], [ %call12, %if.else ]
  %capacity14 = getelementptr inbounds nuw %_Z5ArrayImE, ptr %0, i32 0, i32 1
  store i64 %1, ptr %capacity14, align 8
  %load.struct15 = load %_Z5ArrayImE, ptr %0, align 8
  %length = extractvalue %_Z5ArrayImE %load.struct15, 0
  %gt16 = icmp ugt i64 %length, 0
  br i1 %gt16, label %if.then17, label %if.end18

if.then17:                                        ; preds = %if.end7
  %field.inplace = getelementptr inbounds nuw %_Z5ArrayImE, ptr %0, i32 0, i32 2
  %deref.recv = load ptr, ptr %field.inplace, align 8
  %load.struct19 = load %_Z5ArrayImE, ptr %0, align 8
  %length20 = extractvalue %_Z5ArrayImE %load.struct19, 0
  %mul21 = mul i64 %length20, ptrtoint (ptr getelementptr (i64, ptr null, i32 1) to i64)
  %call22 = call ptr @memcpy(ptr %deref.recv, ptr %buffer, i64 %mul21)
  br label %if.end18

if.end18:                                         ; preds = %if.then17, %if.end7
  %eq = icmp eq i1 %le, false
  br i1 %eq, label %if.then23, label %if.end24

if.then23:                                        ; preds = %if.end18
  %call25 = call ptr @_ZN4Page3getEPv(ptr %buffer)
  call void @_ZN4Page25deallocate_exclusive_pageER4Page(ptr %call, ptr %call25)
  br label %if.end24

if.end24:                                         ; preds = %if.then23, %if.end18
  br label %if.end
}

define linkonce_odr void @_ZN5ArrayImE6extendEm(ptr noalias sret(%_Z5SliceImE) %0, ptr %1, i64 %2) {
entry:
  %tuple = alloca %_Z5SliceImE, align 8
  %load.struct = load %_Z5ArrayImE, ptr %1, align 8
  %length = extractvalue %_Z5ArrayImE %load.struct, 0
  %add = add i64 %length, %2
  %lt = icmp ult i64 %add, %length
  br i1 %lt, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  call void @_Z16scaly_panic_sizeP10const_charmm(ptr @.str.52, i64 %2, i64 %length)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %load.struct3 = load %_Z5ArrayImE, ptr %1, align 8
  %buffer = extractvalue %_Z5ArrayImE %load.struct3, 2
  %eq = icmp eq ptr %buffer, null
  br i1 %eq, label %if.then1, label %lor.rhs

if.then1:                                         ; preds = %lor.rhs, %if.end
  call void @_ZN5ArrayImE7grow_toEm(ptr %1, i64 %add)
  br label %if.end2

if.end2:                                          ; preds = %if.then1, %lor.rhs
  %length5 = getelementptr inbounds nuw %_Z5ArrayImE, ptr %1, i32 0, i32 0
  store i64 %add, ptr %length5, align 8
  %load.struct6 = load %_Z5ArrayImE, ptr %1, align 8
  %buffer7 = extractvalue %_Z5ArrayImE %load.struct6, 2
  %ptr.add = getelementptr inbounds i64, ptr %buffer7, i64 %length
  %tuple.field = getelementptr inbounds nuw %_Z5SliceImE, ptr %tuple, i32 0, i32 0
  store i64 %2, ptr %tuple.field, align 1
  %tuple.field8 = getelementptr inbounds nuw %_Z5SliceImE, ptr %tuple, i32 0, i32 1
  store ptr %ptr.add, ptr %tuple.field8, align 1
  %tuple.val = load %_Z5SliceImE, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z5SliceImE, ptr null, i32 1) to i64), i1 false)
  ret void

lor.rhs:                                          ; preds = %if.end
  %load.struct4 = load %_Z5ArrayImE, ptr %1, align 8
  %capacity = extractvalue %_Z5ArrayImE %load.struct4, 1
  %gt = icmp ugt i64 %add, %capacity
  br i1 %gt, label %if.then1, label %if.end2
}

define linkonce_odr ptr @_ZN5ArrayImE3getEm(ptr %0, i64 %1) {
entry:
  %load.struct = load %_Z5ArrayImE, ptr %0, align 8
  %length = extractvalue %_Z5ArrayImE %load.struct, 0
  %ge = icmp uge i64 %1, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %load.struct1 = load %_Z5ArrayImE, ptr %0, align 8
  %buffer = extractvalue %_Z5ArrayImE %load.struct1, 2
  %ptr.add = getelementptr inbounds i64, ptr %buffer, i64 %1
  ret ptr %ptr.add
}

define linkonce_odr ptr @_ZN5ArrayImE2atEm(ptr %0, i64 %1) {
entry:
  %load.struct = load %_Z5ArrayImE, ptr %0, align 8
  %length = extractvalue %_Z5ArrayImE %load.struct, 0
  %ge = icmp uge i64 %1, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %field.inplace = getelementptr inbounds nuw %_Z5ArrayImE, ptr %0, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  call void @_Z15scaly_panic_oobP10const_charmm(ptr @.str.53, i64 %1, i64 %field.val)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %load.struct1 = load %_Z5ArrayImE, ptr %0, align 8
  %buffer = extractvalue %_Z5ArrayImE %load.struct1, 2
  %ptr.add = getelementptr inbounds i64, ptr %buffer, i64 %1
  ret ptr %ptr.add
}

define linkonce_odr void @_ZN5ArrayImE5clearEv(ptr %0) {
entry:
  %length = getelementptr inbounds nuw %_Z5ArrayImE, ptr %0, i32 0, i32 0
  store i64 0, ptr %length, align 8
  ret void
}

define linkonce_odr void @_ZN5ArrayImE3putEmm(ptr %0, i64 %1, i64 %2) {
entry:
  %load.struct = load %_Z5ArrayImE, ptr %0, align 8
  %length = extractvalue %_Z5ArrayImE %load.struct, 0
  %ge = icmp uge i64 %1, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %field.inplace = getelementptr inbounds nuw %_Z5ArrayImE, ptr %0, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  call void @_Z15scaly_panic_oobP10const_charmm(ptr @.str.54, i64 %1, i64 %field.val)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %load.struct1 = load %_Z5ArrayImE, ptr %0, align 8
  %buffer = extractvalue %_Z5ArrayImE %load.struct1, 2
  %ptr.add = getelementptr inbounds i64, ptr %buffer, i64 %1
  store i64 %2, ptr %ptr.add, align 8
  ret void
}

define linkonce_odr ptr @_ZN13ArrayIteratorImE4nextEv(ptr %0) {
entry:
  %load.struct = load %_Z13ArrayIteratorImE, ptr %0, align 8
  %array = extractvalue %_Z13ArrayIteratorImE %load.struct, 0
  %eq = icmp eq ptr %array, null
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %load.struct1 = load %_Z13ArrayIteratorImE, ptr %0, align 8
  %position = extractvalue %_Z13ArrayIteratorImE %load.struct1, 1
  %load.struct2 = load %_Z13ArrayIteratorImE, ptr %0, align 8
  %array3 = extractvalue %_Z13ArrayIteratorImE %load.struct2, 0
  %deref = load %_Z5ArrayImE, ptr %array3, align 8
  %length = extractvalue %_Z5ArrayImE %deref, 0
  %eq4 = icmp eq i64 %position, %length
  br i1 %eq4, label %if.then5, label %if.end6

if.then5:                                         ; preds = %if.end
  ret ptr null

if.end6:                                          ; preds = %if.end
  %load.struct7 = load %_Z13ArrayIteratorImE, ptr %0, align 8
  %position8 = extractvalue %_Z13ArrayIteratorImE %load.struct7, 1
  %add = add i64 %position8, 1
  %position9 = getelementptr inbounds nuw %_Z13ArrayIteratorImE, ptr %0, i32 0, i32 1
  store i64 %add, ptr %position9, align 8
  %field.inplace = getelementptr inbounds nuw %_Z13ArrayIteratorImE, ptr %0, i32 0, i32 0
  %deref.recv = load ptr, ptr %field.inplace, align 8
  %call = call ptr @_ZN5ArrayImE10get_bufferEv(ptr %deref.recv)
  %load.struct10 = load %_Z13ArrayIteratorImE, ptr %0, align 8
  %position11 = extractvalue %_Z13ArrayIteratorImE %load.struct10, 1
  %sub = sub i64 %position11, 1
  %ptr.add = getelementptr inbounds i64, ptr %call, i64 %sub
  ret ptr %ptr.add
}

define linkonce_odr void @_ZN13ArrayIteratorImEC1E6OptionIR5ArrayImEE(ptr %0, ptr %1) {
entry:
  %array = getelementptr inbounds nuw %_Z13ArrayIteratorImE, ptr %0, i32 0, i32 0
  store ptr %1, ptr %array, align 8
  %position = getelementptr inbounds nuw %_Z13ArrayIteratorImE, ptr %0, i32 0, i32 1
  store i64 0, ptr %position, align 8
  ret void
}

define linkonce_odr void @_ZN5ArrayImE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z13ArrayIteratorImE) %0, ptr %1, ptr %2) {
entry:
  %struct.init = alloca %_Z13ArrayIteratorImE, align 8
  call void @_ZN13ArrayIteratorImEC1E6OptionIR5ArrayImEE(ptr %struct.init, ptr %2)
  %sret.body = load %_Z13ArrayIteratorImE, ptr %struct.init, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %struct.init, i64 ptrtoint (ptr getelementptr (%_Z13ArrayIteratorImE, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN5ArrayImE8as_sliceEv(ptr noalias sret(%_Z5SliceImE) %0, ptr %1) {
entry:
  %load.struct = load %_Z5ArrayImE, ptr %1, align 8
  %length = extractvalue %_Z5ArrayImE %load.struct, 0
  %call = call ptr @_ZN5ArrayImE10get_bufferEv(ptr %1)
  %tuple = alloca %_Z5SliceImE, align 8
  %tuple.field = getelementptr inbounds nuw %_Z5SliceImE, ptr %tuple, i32 0, i32 0
  store i64 %length, ptr %tuple.field, align 1
  %tuple.field1 = getelementptr inbounds nuw %_Z5SliceImE, ptr %tuple, i32 0, i32 1
  store ptr %call, ptr %tuple.field1, align 1
  %tuple.val = load %_Z5SliceImE, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z5SliceImE, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN5ArrayImEC1Ev(ptr %0) {
entry:
  %length = getelementptr inbounds nuw %_Z5ArrayImE, ptr %0, i32 0, i32 0
  store i64 0, ptr %length, align 8
  %capacity = getelementptr inbounds nuw %_Z5ArrayImE, ptr %0, i32 0, i32 1
  store i64 0, ptr %capacity, align 8
  %buffer = getelementptr inbounds nuw %_Z5ArrayImE, ptr %0, i32 0, i32 2
  store ptr null, ptr %buffer, align 8
  ret void
}

define linkonce_odr void @_ZN5ArrayImEC1Em(ptr %0, i64 %1) {
entry:
  %length = getelementptr inbounds nuw %_Z5ArrayImE, ptr %0, i32 0, i32 0
  store i64 0, ptr %length, align 8
  %capacity = getelementptr inbounds nuw %_Z5ArrayImE, ptr %0, i32 0, i32 1
  store i64 0, ptr %capacity, align 8
  %buffer = getelementptr inbounds nuw %_Z5ArrayImE, ptr %0, i32 0, i32 2
  store ptr null, ptr %buffer, align 8
  %gt = icmp ugt i64 %1, 0
  br i1 %gt, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %call = call ptr @_ZN4Page3getEPv(ptr %0)
  %capacity1 = getelementptr inbounds nuw %_Z5ArrayImE, ptr %0, i32 0, i32 1
  store i64 %1, ptr %capacity1, align 8
  %mul = mul i64 %1, ptrtoint (ptr getelementptr (i64, ptr null, i32 1) to i64)
  %le = icmp ule i64 %mul, 1024
  br i1 %le, label %if.then2, label %if.else

if.end:                                           ; preds = %if.end3, %entry
  ret void

if.then2:                                         ; preds = %if.then
  %mul4 = mul i64 %1, ptrtoint (ptr getelementptr (i64, ptr null, i32 1) to i64)
  %call5 = call ptr @_ZN4Page8allocateEmm(ptr %call, i64 %mul4, i64 8)
  %buffer6 = getelementptr inbounds nuw %_Z5ArrayImE, ptr %0, i32 0, i32 2
  store ptr %call5, ptr %buffer6, align 8
  br label %if.end3

if.else:                                          ; preds = %if.then
  %mul7 = mul i64 %1, ptrtoint (ptr getelementptr (i64, ptr null, i32 1) to i64)
  %call8 = call ptr @_ZN4Page25allocate_exclusive_bufferEmm(ptr %call, i64 %mul7, i64 8)
  %buffer9 = getelementptr inbounds nuw %_Z5ArrayImE, ptr %0, i32 0, i32 2
  store ptr %call8, ptr %buffer9, align 8
  br label %if.end3

if.end3:                                          ; preds = %if.else, %if.then2
  %if.value = phi ptr [ %call5, %if.then2 ], [ %call8, %if.else ]
  br label %if.end
}

define linkonce_odr void @_ZN9JsonValueB4json13closing_quoteEPN4scaly6memory4PageE5SliceI2u8Emm(ptr noalias sret(%_Z8JsonScan) %0, ptr %1, ptr %2, i64 %3, i64 %4) {
entry:
  %tuple = alloca %_Z8JsonScan, align 8
  %load.struct = load %_Z5SliceI2u8E, ptr %2, align 8
  %length = extractvalue %_Z5SliceI2u8E %load.struct, 0
  %lt = icmp ult i64 %4, %length
  br i1 %lt, label %land.rhs, label %if.end

if.then:                                          ; preds = %land.rhs
  %tuple.field = getelementptr inbounds nuw %_Z8JsonScan, ptr %tuple, i32 0, i32 0
  store i64 0, ptr %tuple.field, align 1
  %tuple.field1 = getelementptr inbounds nuw %_Z8JsonScan, ptr %tuple, i32 0, i32 1
  store i64 %4, ptr %tuple.field1, align 1
  %tuple.field2 = getelementptr inbounds nuw %_Z8JsonScan, ptr %tuple, i32 0, i32 2
  store i1 false, ptr %tuple.field2, align 1
  %tuple.val = load %_Z8JsonScan, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z8JsonScan, ptr null, i32 1) to i64), i1 false)
  ret void

if.end:                                           ; preds = %land.rhs, %entry
  call void @_ZN10JsonReaderB4json12string_closeEPN4scaly6memory4PageE5SliceI2u8Em(ptr noalias sret(%_Z8JsonScan) %tuple, ptr null, ptr %2, i64 %3)
  %sret.body = load %_Z8JsonScan, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z8JsonScan, ptr null, i32 1) to i64), i1 false)
  ret void

land.rhs:                                         ; preds = %entry
  %call = call i8 @_ZN5SliceI2u8EixEm(ptr %2, i64 %4)
  %eq = icmp eq i8 %call, 34
  br i1 %eq, label %if.then, label %if.end
}

define linkonce_odr void @_ZN9JsonValueB4json11string_textE5SliceI2u8Em8JsonScanB4json(ptr noalias sret(%_Z5SliceI2u8E) %0, ptr %1, i64 %2, ptr %3) {
entry:
  %sret.result = alloca %_Z5SliceI2u8E, align 8
  %load.struct = load %_Z8JsonScan, ptr %3, align 8
  %flag = extractvalue %_Z8JsonScan %load.struct, 2
  %eq = icmp eq i1 %flag, false
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %add = add i64 %2, 1
  %field.inplace = getelementptr inbounds nuw %_Z8JsonScan, ptr %3, i32 0, i32 1
  %field.val = load i64, ptr %field.inplace, align 8
  call void @_ZN5SliceI2u8E8subsliceEmm(ptr noalias sret(%_Z5SliceI2u8E) %sret.result, ptr %1, i64 %add, i64 %field.val)
  %sret.body = load %_Z5SliceI2u8E, ptr %sret.result, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %sret.result, i64 ptrtoint (ptr getelementptr (%_Z5SliceI2u8E, ptr null, i32 1) to i64), i1 false)
  ret void

if.end:                                           ; preds = %entry
  %add1 = add i64 %2, 1
  %field.inplace2 = getelementptr inbounds nuw %_Z8JsonScan, ptr %3, i32 0, i32 1
  %field.val3 = load i64, ptr %field.inplace2, align 8
  %call = call i64 @_ZN10JsonReaderB4json15decode_in_placeE5SliceI2u8Emm(ptr %1, i64 %add1, i64 %field.val3)
  %add4 = add i64 %2, 1
  %add5 = add i64 %2, 1
  %add6 = add i64 %add5, %call
  call void @_ZN5SliceI2u8E8subsliceEmm(ptr noalias sret(%_Z5SliceI2u8E) %sret.result, ptr %1, i64 %add4, i64 %add6)
  %sret.body7 = load %_Z5SliceI2u8E, ptr %sret.result, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %sret.result, i64 ptrtoint (ptr getelementptr (%_Z5SliceI2u8E, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr i1 @_ZN9JsonValueB4json10literal_atE5SliceI2u8Em5SliceI2u8E(ptr %0, i64 %1, ptr %2) {
entry:
  %i = alloca i64, align 8
  %load.struct = load %_Z5SliceI2u8E, ptr %2, align 8
  %length = extractvalue %_Z5SliceI2u8E %load.struct, 0
  %add = add i64 %1, %length
  %load.struct1 = load %_Z5SliceI2u8E, ptr %0, align 8
  %length2 = extractvalue %_Z5SliceI2u8E %load.struct1, 0
  %gt = icmp ugt i64 %add, %length2
  br i1 %gt, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret i1 false

if.end:                                           ; preds = %entry
  store i64 0, ptr %i, align 1
  br label %while.cond

while.cond:                                       ; preds = %if.end11, %if.end
  %i3 = load i64, ptr %i, align 8
  %load.struct4 = load %_Z5SliceI2u8E, ptr %2, align 8
  %length5 = extractvalue %_Z5SliceI2u8E %load.struct4, 0
  %lt = icmp ult i64 %i3, %length5
  br i1 %lt, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %i6 = load i64, ptr %i, align 8
  %add7 = add i64 %1, %i6
  %call = call i8 @_ZN5SliceI2u8EixEm(ptr %0, i64 %add7)
  %i8 = load i64, ptr %i, align 8
  %call9 = call i8 @_ZN5SliceI2u8EixEm(ptr %2, i64 %i8)
  %ne = icmp ne i8 %call, %call9
  br i1 %ne, label %if.then10, label %if.end11

while.exit:                                       ; preds = %while.cond
  ret i1 true

if.then10:                                        ; preds = %while.body
  ret i1 false

if.end11:                                         ; preds = %while.body
  %i12 = load i64, ptr %i, align 8
  %add13 = add i64 %i12, 1
  store i64 %add13, ptr %i, align 1
  br label %while.cond
}

define linkonce_odr i64 @_ZN5ArrayImEixEm(ptr %0, i64 %1) {
entry:
  %load.struct = load %_Z5ArrayImE, ptr %0, align 8
  %length = extractvalue %_Z5ArrayImE %load.struct, 0
  %ge = icmp uge i64 %1, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %field.inplace = getelementptr inbounds nuw %_Z5ArrayImE, ptr %0, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  call void @_Z15scaly_panic_oobP10const_charmm(ptr @.str.55, i64 %1, i64 %field.val)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %load.struct1 = load %_Z5ArrayImE, ptr %0, align 8
  %buffer = extractvalue %_Z5ArrayImE %load.struct1, 2
  %ptr.add = getelementptr inbounds i64, ptr %buffer, i64 %1
  %deref = load i64, ptr %ptr.add, align 8
  ret i64 %deref
}

define linkonce_odr void @_ZN5ArrayI10JsonMemberB4jsonEixEm(ptr noalias sret(%_Z10JsonMember) %0, ptr %1, i64 %2) {
entry:
  %deref.tmp = alloca %_Z10JsonMember, align 8
  %load.struct = load %_Z5ArrayI10JsonMemberE, ptr %1, align 8
  %length = extractvalue %_Z5ArrayI10JsonMemberE %load.struct, 0
  %ge = icmp uge i64 %2, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %field.inplace = getelementptr inbounds nuw %_Z5ArrayI10JsonMemberE, ptr %1, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  call void @_Z15scaly_panic_oobP10const_charmm(ptr @.str.56, i64 %2, i64 %field.val)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %load.struct1 = load %_Z5ArrayI10JsonMemberE, ptr %1, align 8
  %buffer = extractvalue %_Z5ArrayI10JsonMemberE %load.struct1, 2
  %ptr.add = getelementptr inbounds %_Z10JsonMember, ptr %buffer, i64 %2
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %deref.tmp, ptr align 1 %ptr.add, i64 ptrtoint (ptr getelementptr (%_Z10JsonMember, ptr null, i32 1) to i64), i1 false)
  %sret.body = load %_Z10JsonMember, ptr %deref.tmp, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %deref.tmp, i64 ptrtoint (ptr getelementptr (%_Z10JsonMember, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN9JsonValueB4json5parseEPN4scaly6memory4PageE5SliceI2u8E(ptr noalias sret(%_Z10JsonParsed) %0, ptr %1, ptr %2) {
entry:
  %tuple420 = alloca %_Z10JsonParsed, align 8
  %variant.ptr415 = alloca %_Z9JsonValue, align 8
  %tuple396 = alloca %_Z9JsonArray, align 8
  %variant.ptr394 = alloca %_Z9JsonValue, align 8
  %sret.result387 = alloca %_Z5SliceI9JsonValueE, align 8
  %sret.result386 = alloca %_Z5SliceI9JsonValueE, align 8
  %sret.result384 = alloca %_Z5SliceI9JsonValueE, align 8
  %tuple372 = alloca %_Z10JsonObject, align 8
  %variant.ptr370 = alloca %_Z9JsonValue, align 8
  %sret.result365 = alloca %_Z5SliceI10JsonMemberE, align 8
  %sret.result364 = alloca %_Z5SliceI10JsonMemberE, align 8
  %sret.result362 = alloca %_Z5SliceI10JsonMemberE, align 8
  %arg.tmp340 = alloca %_Z10JsonMember, align 8
  %tuple335 = alloca %_Z10JsonMember, align 8
  %sret.result329 = alloca %_Z10JsonMember, align 8
  %variant.ptr283 = alloca %_Z9JsonValue, align 8
  %arg.tmp278 = alloca %_Z5SliceI2u8E, align 8
  %tuple274 = alloca %_Z5SliceI2u8E, align 8
  %variant.ptr266 = alloca %_Z9JsonValue, align 8
  %arg.tmp261 = alloca %_Z5SliceI2u8E, align 8
  %tuple257 = alloca %_Z5SliceI2u8E, align 8
  %variant.ptr249 = alloca %_Z9JsonValue, align 8
  %arg.tmp244 = alloca %_Z5SliceI2u8E, align 8
  %tuple240 = alloca %_Z5SliceI2u8E, align 8
  %tuple229 = alloca %_Z10JsonNumber, align 8
  %sret.result226 = alloca %_Z5SliceI2u8E, align 8
  %variant.ptr224 = alloca %_Z9JsonValue, align 8
  %tuple197 = alloca %_Z10JsonString, align 8
  %variant.ptr190 = alloca %_Z9JsonValue, align 8
  %tuple152 = alloca %_Z9JsonArray, align 8
  %variant.ptr150 = alloca %_Z9JsonValue, align 8
  %tuple131 = alloca %_Z10JsonObject, align 8
  %variant.ptr129 = alloca %_Z9JsonValue, align 8
  %arg.tmp = alloca %_Z10JsonMember, align 8
  %tuple86 = alloca %_Z10JsonMember, align 8
  %sret.result67 = alloca %_Z5SliceI2u8E, align 8
  %sret.result51 = alloca %_Z8JsonScan, align 8
  %u = alloca i64, align 8
  %error_at = alloca i64, align 8
  %error = alloca i64, align 8
  %v = alloca %_Z9JsonValue, align 8
  %variant.ptr = alloca %_Z9JsonValue, align 8
  %t = alloca i64, align 8
  %state = alloca i64, align 8
  %s = alloca %_Z10JsonStream, align 8
  %tuple = alloca %_Z10JsonStream, align 8
  %open = alloca ptr, align 8
  %members = alloca ptr, align 8
  %values = alloca ptr, align 8
  %frame = alloca { ptr, ptr }, align 8
  store ptr null, ptr %frame, align 8
  %frame.parent = getelementptr inbounds nuw { ptr, ptr }, ptr %frame, i32 0, i32 1
  store ptr %1, ptr %frame.parent, align 8
  %doc = alloca ptr, align 8
  %sret.result = alloca %_Z5SliceI2u8E, align 8
  %arena = alloca ptr, align 8
  %frame.page = load ptr, ptr %1, align 8
  %frame.has_page = icmp ne ptr %frame.page, null
  br i1 %frame.has_page, label %frame.forced, label %frame.force

frame.force:                                      ; preds = %entry
  %forced_page = call ptr @_Z17scaly_force_frameP5Frame(ptr %1)
  br label %frame.forced

frame.forced:                                     ; preds = %frame.force, %entry
  %forced_page1 = phi ptr [ %frame.page, %entry ], [ %forced_page, %frame.force ]
  %struct.region = call ptr @_ZN4Page8allocateEmm(ptr %forced_page1, i64 ptrtoint (ptr getelementptr (%_Z9JsonArena, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr ({ i1, %_Z9JsonArena }, ptr null, i64 0, i32 1) to i64))
  call void @_ZN9JsonArenaB4jsonC1Ev(ptr %struct.region)
  store ptr %struct.region, ptr %arena, align 1
  %arena2 = load ptr, ptr %arena, align 8
  call void @_ZN9JsonArenaB4json13keep_documentEPN4scaly6memory4PageE5SliceI2u8E(ptr noalias sret(%_Z5SliceI2u8E) %sret.result, ptr null, ptr %arena2, ptr %2)
  store ptr %sret.result, ptr %doc, align 1
  %load.struct = load %_Z5SliceI2u8E, ptr %2, align 8
  %length = extractvalue %_Z5SliceI2u8E %load.struct, 0
  %frame.page3 = load ptr, ptr %frame, align 8
  %frame.has_page4 = icmp ne ptr %frame.page3, null
  br i1 %frame.has_page4, label %frame.forced6, label %frame.force5

frame.force5:                                     ; preds = %frame.forced
  %forced_page7 = call ptr @_Z17scaly_force_frameP5Frame(ptr %frame)
  br label %frame.forced6

frame.forced6:                                    ; preds = %frame.force5, %frame.forced
  %forced_page8 = phi ptr [ %frame.page3, %frame.forced ], [ %forced_page7, %frame.force5 ]
  %struct.region9 = call ptr @_ZN4Page8allocateEmm(ptr %forced_page8, i64 ptrtoint (ptr getelementptr (%_Z5ArrayI9JsonValueE, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr ({ i1, %_Z5ArrayI9JsonValueE }, ptr null, i64 0, i32 1) to i64))
  %tuple.field = getelementptr inbounds nuw %_Z5ArrayI9JsonValueE, ptr %struct.region9, i32 0, i32 0
  store i64 0, ptr %tuple.field, align 8
  %tuple.field10 = getelementptr inbounds nuw %_Z5ArrayI9JsonValueE, ptr %struct.region9, i32 0, i32 1
  store i64 0, ptr %tuple.field10, align 8
  %tuple.field11 = getelementptr inbounds nuw %_Z5ArrayI9JsonValueE, ptr %struct.region9, i32 0, i32 2
  store ptr null, ptr %tuple.field11, align 8
  store ptr %struct.region9, ptr %values, align 1
  %frame.page12 = load ptr, ptr %frame, align 8
  %frame.has_page13 = icmp ne ptr %frame.page12, null
  br i1 %frame.has_page13, label %frame.forced15, label %frame.force14

frame.force14:                                    ; preds = %frame.forced6
  %forced_page16 = call ptr @_Z17scaly_force_frameP5Frame(ptr %frame)
  br label %frame.forced15

frame.forced15:                                   ; preds = %frame.force14, %frame.forced6
  %forced_page17 = phi ptr [ %frame.page12, %frame.forced6 ], [ %forced_page16, %frame.force14 ]
  %struct.region18 = call ptr @_ZN4Page8allocateEmm(ptr %forced_page17, i64 ptrtoint (ptr getelementptr (%_Z5ArrayI10JsonMemberE, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr ({ i1, %_Z5ArrayI10JsonMemberE }, ptr null, i64 0, i32 1) to i64))
  %tuple.field19 = getelementptr inbounds nuw %_Z5ArrayI10JsonMemberE, ptr %struct.region18, i32 0, i32 0
  store i64 0, ptr %tuple.field19, align 8
  %tuple.field20 = getelementptr inbounds nuw %_Z5ArrayI10JsonMemberE, ptr %struct.region18, i32 0, i32 1
  store i64 0, ptr %tuple.field20, align 8
  %tuple.field21 = getelementptr inbounds nuw %_Z5ArrayI10JsonMemberE, ptr %struct.region18, i32 0, i32 2
  store ptr null, ptr %tuple.field21, align 8
  store ptr %struct.region18, ptr %members, align 1
  %frame.page22 = load ptr, ptr %frame, align 8
  %frame.has_page23 = icmp ne ptr %frame.page22, null
  br i1 %frame.has_page23, label %frame.forced25, label %frame.force24

frame.force24:                                    ; preds = %frame.forced15
  %forced_page26 = call ptr @_Z17scaly_force_frameP5Frame(ptr %frame)
  br label %frame.forced25

frame.forced25:                                   ; preds = %frame.force24, %frame.forced15
  %forced_page27 = phi ptr [ %frame.page22, %frame.forced15 ], [ %forced_page26, %frame.force24 ]
  %struct.region28 = call ptr @_ZN4Page8allocateEmm(ptr %forced_page27, i64 ptrtoint (ptr getelementptr (%_Z5ArrayImE, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr ({ i1, %_Z5ArrayImE }, ptr null, i64 0, i32 1) to i64))
  %tuple.field29 = getelementptr inbounds nuw %_Z5ArrayImE, ptr %struct.region28, i32 0, i32 0
  store i64 0, ptr %tuple.field29, align 8
  %tuple.field30 = getelementptr inbounds nuw %_Z5ArrayImE, ptr %struct.region28, i32 0, i32 1
  store i64 0, ptr %tuple.field30, align 8
  %tuple.field31 = getelementptr inbounds nuw %_Z5ArrayImE, ptr %struct.region28, i32 0, i32 2
  store ptr null, ptr %tuple.field31, align 8
  store ptr %struct.region28, ptr %open, align 1
  %tuple.field32 = getelementptr inbounds nuw %_Z10JsonStream, ptr %tuple, i32 0, i32 0
  store i64 0, ptr %tuple.field32, align 1
  %tuple.field33 = getelementptr inbounds nuw %_Z10JsonStream, ptr %tuple, i32 0, i32 1
  store i64 0, ptr %tuple.field33, align 1
  %tuple.field34 = getelementptr inbounds nuw %_Z10JsonStream, ptr %tuple, i32 0, i32 2
  store i64 0, ptr %tuple.field34, align 1
  %tuple.field35 = getelementptr inbounds nuw %_Z10JsonStream, ptr %tuple, i32 0, i32 3
  store i64 0, ptr %tuple.field35, align 1
  %tuple.field36 = getelementptr inbounds nuw %_Z10JsonStream, ptr %tuple, i32 0, i32 4
  store i64 0, ptr %tuple.field36, align 1
  %tuple.field37 = getelementptr inbounds nuw %_Z10JsonStream, ptr %tuple, i32 0, i32 5
  store i64 0, ptr %tuple.field37, align 1
  %tuple.val = load %_Z10JsonStream, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %s, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z10JsonStream, ptr null, i32 1) to i64), i1 false)
  store i64 0, ptr %state, align 1
  %doc38 = load ptr, ptr %doc, align 8
  %call = call i64 @_ZN10JsonStreamB4json4takeE5SliceI2u8E(ptr %s, ptr %doc38)
  store i64 %call, ptr %t, align 1
  %variant.tag.ptr = getelementptr inbounds nuw %_Z9JsonValue, ptr %variant.ptr, i32 0, i32 0
  store i8 0, ptr %variant.tag.ptr, align 1
  %variant.val = load %_Z9JsonValue, ptr %variant.ptr, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %v, ptr align 1 %variant.ptr, i64 ptrtoint (ptr getelementptr (%_Z9JsonValue, ptr null, i32 1) to i64), i1 false)
  store i64 0, ptr %error, align 1
  store i64 0, ptr %error_at, align 1
  br label %repeat.body

repeat.body:                                      ; preds = %if.end359, %if.end356, %if.end142, %if.then141, %if.end122, %if.then121, %frame.forced25
  %state39 = load i64, ptr %state, align 8
  %eq = icmp eq i64 %state39, 1
  br i1 %eq, label %if.then, label %if.end

repeat.exit:                                      ; preds = %if.else379, %if.then344, %if.end314, %if.else281, %if.then218, %if.then175, %if.then113, %if.then99, %if.then72, %if.then57, %if.then46, %if.then41
  %error411 = load i64, ptr %error, align 8
  %ne412 = icmp ne i64 %error411, 0
  br i1 %ne412, label %if.then413, label %if.end414

if.then:                                          ; preds = %repeat.body
  %t40 = load i64, ptr %t, align 8
  %ge = icmp uge i64 %t40, %length
  br i1 %ge, label %if.then41, label %if.end42

if.end:                                           ; preds = %if.end73, %repeat.body
  %state93 = load i64, ptr %state, align 8
  %eq94 = icmp eq i64 %state93, 0
  br i1 %eq94, label %if.then95, label %if.end96

if.then41:                                        ; preds = %if.then
  store i64 1, ptr %error, align 1
  store i64 %length, ptr %error_at, align 1
  br label %repeat.exit

if.end42:                                         ; preds = %if.then
  %doc43 = load ptr, ptr %doc, align 8
  %t44 = load i64, ptr %t, align 8
  %call45 = call i8 @_ZN5SliceI2u8EixEm(ptr %doc43, i64 %t44)
  %ne = icmp ne i8 %call45, 34
  br i1 %ne, label %if.then46, label %if.end47

if.then46:                                        ; preds = %if.end42
  store i64 4, ptr %error, align 1
  %t48 = load i64, ptr %t, align 8
  store i64 %t48, ptr %error_at, align 1
  br label %repeat.exit

if.end47:                                         ; preds = %if.end42
  %doc49 = load ptr, ptr %doc, align 8
  %call50 = call i64 @_ZN10JsonStreamB4json4takeE5SliceI2u8E(ptr %s, ptr %doc49)
  store i64 %call50, ptr %u, align 1
  %doc52 = load ptr, ptr %doc, align 8
  %t53 = load i64, ptr %t, align 8
  %u54 = load i64, ptr %u, align 8
  call void @_ZN9JsonValueB4json13closing_quoteEPN4scaly6memory4PageE5SliceI2u8Emm(ptr noalias sret(%_Z8JsonScan) %sret.result51, ptr %1, ptr %doc52, i64 %t53, i64 %u54)
  %load.struct55 = load %_Z8JsonScan, ptr %sret.result51, align 8
  %code = extractvalue %_Z8JsonScan %load.struct55, 0
  %ne56 = icmp ne i64 %code, 0
  br i1 %ne56, label %if.then57, label %if.end58

if.then57:                                        ; preds = %if.end47
  %load.struct59 = load %_Z8JsonScan, ptr %sret.result51, align 8
  %code60 = extractvalue %_Z8JsonScan %load.struct59, 0
  store i64 %code60, ptr %error, align 1
  %load.struct61 = load %_Z8JsonScan, ptr %sret.result51, align 8
  %at = extractvalue %_Z8JsonScan %load.struct61, 1
  store i64 %at, ptr %error_at, align 1
  br label %repeat.exit

if.end58:                                         ; preds = %if.end47
  br label %while.cond

while.cond:                                       ; preds = %while.body, %if.end58
  %u62 = load i64, ptr %u, align 8
  %load.struct63 = load %_Z8JsonScan, ptr %sret.result51, align 8
  %at64 = extractvalue %_Z8JsonScan %load.struct63, 1
  %lt = icmp ult i64 %u62, %at64
  br i1 %lt, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %doc65 = load ptr, ptr %doc, align 8
  %call66 = call i64 @_ZN10JsonStreamB4json4takeE5SliceI2u8E(ptr %s, ptr %doc65)
  store i64 %call66, ptr %u, align 1
  br label %while.cond

while.exit:                                       ; preds = %while.cond
  %doc68 = load ptr, ptr %doc, align 8
  %t69 = load i64, ptr %t, align 8
  call void @_ZN9JsonValueB4json11string_textE5SliceI2u8Em8JsonScanB4json(ptr noalias sret(%_Z5SliceI2u8E) %sret.result67, ptr %doc68, i64 %t69, ptr %sret.result51)
  %doc70 = load ptr, ptr %doc, align 8
  %call71 = call i64 @_ZN10JsonStreamB4json4takeE5SliceI2u8E(ptr %s, ptr %doc70)
  store i64 %call71, ptr %t, align 1
  %t74 = load i64, ptr %t, align 8
  %ge75 = icmp uge i64 %t74, %length
  br i1 %ge75, label %if.then72, label %lor.rhs

if.then72:                                        ; preds = %lor.rhs, %while.exit
  store i64 5, ptr %error, align 1
  %t80 = load i64, ptr %t, align 8
  store i64 %t80, ptr %error_at, align 1
  br label %repeat.exit

if.end73:                                         ; preds = %lor.rhs
  %members81 = load ptr, ptr %members, align 8
  %variant.tag.ptr82 = getelementptr inbounds nuw %_Z9JsonValue, ptr %variant.ptr, i32 0, i32 0
  store i8 0, ptr %variant.tag.ptr82, align 1
  %variant.val83 = load %_Z9JsonValue, ptr %variant.ptr, align 1
  %load.struct84 = load %_Z8JsonScan, ptr %sret.result51, align 8
  %flag = extractvalue %_Z8JsonScan %load.struct84, 2
  %eq85 = icmp eq i1 %flag, false
  %field.load = load %_Z5SliceI2u8E, ptr %sret.result67, align 8
  %tuple.field87 = getelementptr inbounds nuw %_Z10JsonMember, ptr %tuple86, i32 0, i32 0
  store %_Z5SliceI2u8E %field.load, ptr %tuple.field87, align 1
  %tuple.field88 = getelementptr inbounds nuw %_Z10JsonMember, ptr %tuple86, i32 0, i32 1
  store %_Z9JsonValue %variant.val83, ptr %tuple.field88, align 1
  %tuple.field89 = getelementptr inbounds nuw %_Z10JsonMember, ptr %tuple86, i32 0, i32 2
  store i1 %eq85, ptr %tuple.field89, align 1
  %tuple.val90 = load %_Z10JsonMember, ptr %tuple86, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %arg.tmp, ptr align 1 %tuple86, i64 ptrtoint (ptr getelementptr (%_Z10JsonMember, ptr null, i32 1) to i64), i1 false)
  call void @_ZN5ArrayI10JsonMemberB4jsonE3addE10JsonMemberB4json(ptr %members81, ptr %arg.tmp)
  %doc91 = load ptr, ptr %doc, align 8
  %call92 = call i64 @_ZN10JsonStreamB4json4takeE5SliceI2u8E(ptr %s, ptr %doc91)
  store i64 %call92, ptr %t, align 1
  store i64 0, ptr %state, align 1
  br label %if.end

lor.rhs:                                          ; preds = %while.exit
  %doc76 = load ptr, ptr %doc, align 8
  %t77 = load i64, ptr %t, align 8
  %call78 = call i8 @_ZN5SliceI2u8EixEm(ptr %doc76, i64 %t77)
  %ne79 = icmp ne i8 %call78, 58
  br i1 %ne79, label %if.then72, label %if.end73

if.then95:                                        ; preds = %if.end
  %t97 = load i64, ptr %t, align 8
  %ge98 = icmp uge i64 %t97, %length
  br i1 %ge98, label %if.then99, label %if.end100

if.end96:                                         ; preds = %if.end166, %if.end
  %open305 = load ptr, ptr %open, align 8
  %load.struct306 = load %_Z5ArrayImE, ptr %open305, align 8
  %length307 = extractvalue %_Z5ArrayImE %load.struct306, 0
  %eq308 = icmp eq i64 %length307, 0
  br i1 %eq308, label %if.then309, label %if.end310

if.then99:                                        ; preds = %if.then95
  store i64 1, ptr %error, align 1
  store i64 %length, ptr %error_at, align 1
  br label %repeat.exit

if.end100:                                        ; preds = %if.then95
  %doc101 = load ptr, ptr %doc, align 8
  %t102 = load i64, ptr %t, align 8
  %call103 = call i8 @_ZN5SliceI2u8EixEm(ptr %doc101, i64 %t102)
  %eq107 = icmp eq i8 %call103, 123
  br i1 %eq107, label %if.then104, label %lor.rhs106

if.then104:                                       ; preds = %lor.rhs106, %if.end100
  %open109 = load ptr, ptr %open, align 8
  %load.struct110 = load %_Z5ArrayImE, ptr %open109, align 8
  %length111 = extractvalue %_Z5ArrayImE %load.struct110, 0
  %ge112 = icmp uge i64 %length111, 256
  br i1 %ge112, label %if.then113, label %if.end114

if.end105:                                        ; preds = %lor.rhs106
  %eq164 = icmp eq i8 %call103, 34
  br i1 %eq164, label %if.then165, label %if.else

lor.rhs106:                                       ; preds = %if.end100
  %eq108 = icmp eq i8 %call103, 91
  br i1 %eq108, label %if.then104, label %if.end105

if.then113:                                       ; preds = %if.then104
  store i64 10, ptr %error, align 1
  %t115 = load i64, ptr %t, align 8
  store i64 %t115, ptr %error_at, align 1
  br label %repeat.exit

if.end114:                                        ; preds = %if.then104
  %eq116 = icmp eq i8 %call103, 123
  %doc117 = load ptr, ptr %doc, align 8
  %call118 = call i64 @_ZN10JsonStreamB4json4takeE5SliceI2u8E(ptr %s, ptr %doc117)
  store i64 %call118, ptr %t, align 1
  br i1 %eq116, label %if.then119, label %if.end120

if.then119:                                       ; preds = %if.end114
  %t123 = load i64, ptr %t, align 8
  %lt124 = icmp ult i64 %t123, %length
  br i1 %lt124, label %land.rhs, label %if.end122

if.end120:                                        ; preds = %if.end114
  %t144 = load i64, ptr %t, align 8
  %lt145 = icmp ult i64 %t144, %length
  br i1 %lt145, label %land.rhs143, label %if.end142

if.then121:                                       ; preds = %land.rhs
  %variant.tag.ptr130 = getelementptr inbounds nuw %_Z9JsonValue, ptr %variant.ptr129, i32 0, i32 0
  store i8 5, ptr %variant.tag.ptr130, align 1
  %tuple.field132 = getelementptr inbounds nuw %_Z10JsonObject, ptr %tuple131, i32 0, i32 0
  store %_Z5SliceI10JsonMemberE zeroinitializer, ptr %tuple.field132, align 1
  %tuple.val133 = load %_Z10JsonObject, ptr %tuple131, align 8
  %variant.data.ptr = getelementptr inbounds nuw %_Z9JsonValue, ptr %variant.ptr129, i32 0, i32 1
  store %_Z10JsonObject %tuple.val133, ptr %variant.data.ptr, align 1
  %variant.val134 = load %_Z9JsonValue, ptr %variant.ptr129, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %v, ptr align 1 %variant.ptr129, i64 ptrtoint (ptr getelementptr (%_Z9JsonValue, ptr null, i32 1) to i64), i1 false)
  %doc135 = load ptr, ptr %doc, align 8
  %call136 = call i64 @_ZN10JsonStreamB4json4takeE5SliceI2u8E(ptr %s, ptr %doc135)
  store i64 %call136, ptr %t, align 1
  store i64 2, ptr %state, align 1
  br label %repeat.body

if.end122:                                        ; preds = %land.rhs, %if.then119
  %open137 = load ptr, ptr %open, align 8
  %members138 = load ptr, ptr %members, align 8
  %load.struct139 = load %_Z5ArrayI10JsonMemberE, ptr %members138, align 8
  %length140 = extractvalue %_Z5ArrayI10JsonMemberE %load.struct139, 0
  %shl = shl i64 %length140, 1
  %add = add i64 %shl, 1
  call void @_ZN5ArrayImE3addEm(ptr %open137, i64 %add)
  store i64 1, ptr %state, align 1
  br label %repeat.body

land.rhs:                                         ; preds = %if.then119
  %doc125 = load ptr, ptr %doc, align 8
  %t126 = load i64, ptr %t, align 8
  %call127 = call i8 @_ZN5SliceI2u8EixEm(ptr %doc125, i64 %t126)
  %eq128 = icmp eq i8 %call127, 125
  br i1 %eq128, label %if.then121, label %if.end122

if.then141:                                       ; preds = %land.rhs143
  %variant.tag.ptr151 = getelementptr inbounds nuw %_Z9JsonValue, ptr %variant.ptr150, i32 0, i32 0
  store i8 4, ptr %variant.tag.ptr151, align 1
  %tuple.field153 = getelementptr inbounds nuw %_Z9JsonArray, ptr %tuple152, i32 0, i32 0
  store %_Z5SliceI9JsonValueE zeroinitializer, ptr %tuple.field153, align 1
  %tuple.val154 = load %_Z9JsonArray, ptr %tuple152, align 8
  %variant.data.ptr155 = getelementptr inbounds nuw %_Z9JsonValue, ptr %variant.ptr150, i32 0, i32 1
  store %_Z9JsonArray %tuple.val154, ptr %variant.data.ptr155, align 1
  %variant.val156 = load %_Z9JsonValue, ptr %variant.ptr150, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %v, ptr align 1 %variant.ptr150, i64 ptrtoint (ptr getelementptr (%_Z9JsonValue, ptr null, i32 1) to i64), i1 false)
  %doc157 = load ptr, ptr %doc, align 8
  %call158 = call i64 @_ZN10JsonStreamB4json4takeE5SliceI2u8E(ptr %s, ptr %doc157)
  store i64 %call158, ptr %t, align 1
  store i64 2, ptr %state, align 1
  br label %repeat.body

if.end142:                                        ; preds = %land.rhs143, %if.end120
  %open159 = load ptr, ptr %open, align 8
  %values160 = load ptr, ptr %values, align 8
  %load.struct161 = load %_Z5ArrayI9JsonValueE, ptr %values160, align 8
  %length162 = extractvalue %_Z5ArrayI9JsonValueE %load.struct161, 0
  %shl163 = shl i64 %length162, 1
  call void @_ZN5ArrayImE3addEm(ptr %open159, i64 %shl163)
  br label %repeat.body

land.rhs143:                                      ; preds = %if.end120
  %doc146 = load ptr, ptr %doc, align 8
  %t147 = load i64, ptr %t, align 8
  %call148 = call i8 @_ZN5SliceI2u8EixEm(ptr %doc146, i64 %t147)
  %eq149 = icmp eq i8 %call148, 93
  br i1 %eq149, label %if.then141, label %if.end142

if.then165:                                       ; preds = %if.end105
  %doc167 = load ptr, ptr %doc, align 8
  %call168 = call i64 @_ZN10JsonStreamB4json4takeE5SliceI2u8E(ptr %s, ptr %doc167)
  store i64 %call168, ptr %u, align 1
  %doc169 = load ptr, ptr %doc, align 8
  %t170 = load i64, ptr %t, align 8
  %u171 = load i64, ptr %u, align 8
  call void @_ZN9JsonValueB4json13closing_quoteEPN4scaly6memory4PageE5SliceI2u8Emm(ptr noalias sret(%_Z8JsonScan) %sret.result51, ptr %1, ptr %doc169, i64 %t170, i64 %u171)
  %load.struct172 = load %_Z8JsonScan, ptr %sret.result51, align 8
  %code173 = extractvalue %_Z8JsonScan %load.struct172, 0
  %ne174 = icmp ne i64 %code173, 0
  br i1 %ne174, label %if.then175, label %if.end176

if.else:                                          ; preds = %if.end105
  store i64 0, ptr %u, align 1
  %eq210 = icmp eq i8 %call103, 45
  br i1 %eq210, label %if.then206, label %lor.rhs209

if.end166:                                        ; preds = %if.end292, %while.exit183
  br label %if.end96

if.then175:                                       ; preds = %if.then165
  %load.struct177 = load %_Z8JsonScan, ptr %sret.result51, align 8
  %code178 = extractvalue %_Z8JsonScan %load.struct177, 0
  store i64 %code178, ptr %error, align 1
  %load.struct179 = load %_Z8JsonScan, ptr %sret.result51, align 8
  %at180 = extractvalue %_Z8JsonScan %load.struct179, 1
  store i64 %at180, ptr %error_at, align 1
  br label %repeat.exit

if.end176:                                        ; preds = %if.then165
  br label %while.cond181

while.cond181:                                    ; preds = %while.body182, %if.end176
  %u184 = load i64, ptr %u, align 8
  %load.struct185 = load %_Z8JsonScan, ptr %sret.result51, align 8
  %at186 = extractvalue %_Z8JsonScan %load.struct185, 1
  %lt187 = icmp ult i64 %u184, %at186
  br i1 %lt187, label %while.body182, label %while.exit183

while.body182:                                    ; preds = %while.cond181
  %doc188 = load ptr, ptr %doc, align 8
  %call189 = call i64 @_ZN10JsonStreamB4json4takeE5SliceI2u8E(ptr %s, ptr %doc188)
  store i64 %call189, ptr %u, align 1
  br label %while.cond181

while.exit183:                                    ; preds = %while.cond181
  %variant.tag.ptr191 = getelementptr inbounds nuw %_Z9JsonValue, ptr %variant.ptr190, i32 0, i32 0
  store i8 3, ptr %variant.tag.ptr191, align 1
  %doc192 = load ptr, ptr %doc, align 8
  %t193 = load i64, ptr %t, align 8
  call void @_ZN9JsonValueB4json11string_textE5SliceI2u8Em8JsonScanB4json(ptr noalias sret(%_Z5SliceI2u8E) %sret.result67, ptr %doc192, i64 %t193, ptr %sret.result51)
  %load.struct194 = load %_Z8JsonScan, ptr %sret.result51, align 8
  %flag195 = extractvalue %_Z8JsonScan %load.struct194, 2
  %eq196 = icmp eq i1 %flag195, false
  %field.load198 = load %_Z5SliceI2u8E, ptr %sret.result67, align 8
  %tuple.field199 = getelementptr inbounds nuw %_Z10JsonString, ptr %tuple197, i32 0, i32 0
  store %_Z5SliceI2u8E %field.load198, ptr %tuple.field199, align 1
  %tuple.field200 = getelementptr inbounds nuw %_Z10JsonString, ptr %tuple197, i32 0, i32 1
  store i1 %eq196, ptr %tuple.field200, align 1
  %tuple.val201 = load %_Z10JsonString, ptr %tuple197, align 8
  %variant.data.ptr202 = getelementptr inbounds nuw %_Z9JsonValue, ptr %variant.ptr190, i32 0, i32 1
  store %_Z10JsonString %tuple.val201, ptr %variant.data.ptr202, align 1
  %variant.val203 = load %_Z9JsonValue, ptr %variant.ptr190, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %v, ptr align 1 %variant.ptr190, i64 ptrtoint (ptr getelementptr (%_Z9JsonValue, ptr null, i32 1) to i64), i1 false)
  %doc204 = load ptr, ptr %doc, align 8
  %call205 = call i64 @_ZN10JsonStreamB4json4takeE5SliceI2u8E(ptr %s, ptr %doc204)
  store i64 %call205, ptr %t, align 1
  store i64 2, ptr %state, align 1
  br label %if.end166

if.then206:                                       ; preds = %lor.end, %if.else
  %doc213 = load ptr, ptr %doc, align 8
  %t214 = load i64, ptr %t, align 8
  call void @_ZN10JsonReaderB4json10number_endEPN4scaly6memory4PageE5SliceI2u8Em(ptr noalias sret(%_Z8JsonScan) %sret.result51, ptr null, ptr %doc213, i64 %t214)
  %load.struct215 = load %_Z8JsonScan, ptr %sret.result51, align 8
  %code216 = extractvalue %_Z8JsonScan %load.struct215, 0
  %ne217 = icmp ne i64 %code216, 0
  br i1 %ne217, label %if.then218, label %if.end219

if.else207:                                       ; preds = %lor.end
  %doc238 = load ptr, ptr %doc, align 8
  %t239 = load i64, ptr %t, align 8
  %tuple.field241 = getelementptr inbounds nuw %_Z5SliceI2u8E, ptr %tuple240, i32 0, i32 0
  store i64 4, ptr %tuple.field241, align 1
  %tuple.field242 = getelementptr inbounds nuw %_Z5SliceI2u8E, ptr %tuple240, i32 0, i32 1
  store ptr @.str.57, ptr %tuple.field242, align 1
  %tuple.val243 = load %_Z5SliceI2u8E, ptr %tuple240, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %arg.tmp244, ptr align 1 %tuple240, i64 ptrtoint (ptr getelementptr (%_Z5SliceI2u8E, ptr null, i32 1) to i64), i1 false)
  %call245 = call i1 @_ZN9JsonValueB4json10literal_atE5SliceI2u8Em5SliceI2u8E(ptr %doc238, i64 %t239, ptr %arg.tmp244)
  br i1 %call245, label %if.then246, label %if.else247

if.end208:                                        ; preds = %if.end248, %if.end219
  %doc289 = load ptr, ptr %doc, align 8
  %call290 = call i64 @_ZN10JsonStreamB4json4takeE5SliceI2u8E(ptr %s, ptr %doc289)
  store i64 %call290, ptr %t, align 1
  %end = load i64, ptr %u, align 8
  %lt295 = icmp ult i64 %end, %length
  br i1 %lt295, label %land.rhs294, label %if.end292

lor.rhs209:                                       ; preds = %if.else
  %ge211 = icmp uge i8 %call103, 48
  br i1 %ge211, label %lor.rhs212, label %lor.end

lor.rhs212:                                       ; preds = %lor.rhs209
  %le = icmp ule i8 %call103, 57
  br label %lor.end

lor.end:                                          ; preds = %lor.rhs212, %lor.rhs209
  %lor.result = phi i1 [ false, %lor.rhs209 ], [ %le, %lor.rhs212 ]
  br i1 %lor.result, label %if.then206, label %if.else207

if.then218:                                       ; preds = %if.then206
  %load.struct220 = load %_Z8JsonScan, ptr %sret.result51, align 8
  %code221 = extractvalue %_Z8JsonScan %load.struct220, 0
  store i64 %code221, ptr %error, align 1
  %load.struct222 = load %_Z8JsonScan, ptr %sret.result51, align 8
  %at223 = extractvalue %_Z8JsonScan %load.struct222, 1
  store i64 %at223, ptr %error_at, align 1
  br label %repeat.exit

if.end219:                                        ; preds = %if.then206
  %variant.tag.ptr225 = getelementptr inbounds nuw %_Z9JsonValue, ptr %variant.ptr224, i32 0, i32 0
  store i8 2, ptr %variant.tag.ptr225, align 1
  %doc227 = load ptr, ptr %doc, align 8
  %t228 = load i64, ptr %t, align 8
  %field.inplace = getelementptr inbounds nuw %_Z8JsonScan, ptr %sret.result51, i32 0, i32 1
  %field.val = load i64, ptr %field.inplace, align 8
  call void @_ZN5SliceI2u8E8subsliceEmm(ptr noalias sret(%_Z5SliceI2u8E) %sret.result226, ptr %doc227, i64 %t228, i64 %field.val)
  %field.load230 = load %_Z5SliceI2u8E, ptr %sret.result226, align 8
  %tuple.field231 = getelementptr inbounds nuw %_Z10JsonNumber, ptr %tuple229, i32 0, i32 0
  store %_Z5SliceI2u8E %field.load230, ptr %tuple.field231, align 1
  %tuple.field232 = getelementptr inbounds nuw %_Z10JsonNumber, ptr %tuple229, i32 0, i32 1
  store i1 true, ptr %tuple.field232, align 1
  %tuple.val233 = load %_Z10JsonNumber, ptr %tuple229, align 8
  %variant.data.ptr234 = getelementptr inbounds nuw %_Z9JsonValue, ptr %variant.ptr224, i32 0, i32 1
  store %_Z10JsonNumber %tuple.val233, ptr %variant.data.ptr234, align 1
  %variant.val235 = load %_Z9JsonValue, ptr %variant.ptr224, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %v, ptr align 1 %variant.ptr224, i64 ptrtoint (ptr getelementptr (%_Z9JsonValue, ptr null, i32 1) to i64), i1 false)
  %load.struct236 = load %_Z8JsonScan, ptr %sret.result51, align 8
  %at237 = extractvalue %_Z8JsonScan %load.struct236, 1
  store i64 %at237, ptr %u, align 1
  br label %if.end208

if.then246:                                       ; preds = %if.else207
  %variant.tag.ptr250 = getelementptr inbounds nuw %_Z9JsonValue, ptr %variant.ptr249, i32 0, i32 0
  store i8 1, ptr %variant.tag.ptr250, align 1
  %variant.data.ptr251 = getelementptr inbounds nuw %_Z9JsonValue, ptr %variant.ptr249, i32 0, i32 1
  store i1 true, ptr %variant.data.ptr251, align 1
  %variant.val252 = load %_Z9JsonValue, ptr %variant.ptr249, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %v, ptr align 1 %variant.ptr249, i64 ptrtoint (ptr getelementptr (%_Z9JsonValue, ptr null, i32 1) to i64), i1 false)
  %t253 = load i64, ptr %t, align 8
  %add254 = add i64 %t253, 4
  store i64 %add254, ptr %u, align 1
  br label %if.end248

if.else247:                                       ; preds = %if.else207
  %doc255 = load ptr, ptr %doc, align 8
  %t256 = load i64, ptr %t, align 8
  %tuple.field258 = getelementptr inbounds nuw %_Z5SliceI2u8E, ptr %tuple257, i32 0, i32 0
  store i64 5, ptr %tuple.field258, align 1
  %tuple.field259 = getelementptr inbounds nuw %_Z5SliceI2u8E, ptr %tuple257, i32 0, i32 1
  store ptr @.str.58, ptr %tuple.field259, align 1
  %tuple.val260 = load %_Z5SliceI2u8E, ptr %tuple257, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %arg.tmp261, ptr align 1 %tuple257, i64 ptrtoint (ptr getelementptr (%_Z5SliceI2u8E, ptr null, i32 1) to i64), i1 false)
  %call262 = call i1 @_ZN9JsonValueB4json10literal_atE5SliceI2u8Em5SliceI2u8E(ptr %doc255, i64 %t256, ptr %arg.tmp261)
  br i1 %call262, label %if.then263, label %if.else264

if.end248:                                        ; preds = %if.end265, %if.then246
  br label %if.end208

if.then263:                                       ; preds = %if.else247
  %variant.tag.ptr267 = getelementptr inbounds nuw %_Z9JsonValue, ptr %variant.ptr266, i32 0, i32 0
  store i8 1, ptr %variant.tag.ptr267, align 1
  %variant.data.ptr268 = getelementptr inbounds nuw %_Z9JsonValue, ptr %variant.ptr266, i32 0, i32 1
  store i1 false, ptr %variant.data.ptr268, align 1
  %variant.val269 = load %_Z9JsonValue, ptr %variant.ptr266, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %v, ptr align 1 %variant.ptr266, i64 ptrtoint (ptr getelementptr (%_Z9JsonValue, ptr null, i32 1) to i64), i1 false)
  %t270 = load i64, ptr %t, align 8
  %add271 = add i64 %t270, 5
  store i64 %add271, ptr %u, align 1
  br label %if.end265

if.else264:                                       ; preds = %if.else247
  %doc272 = load ptr, ptr %doc, align 8
  %t273 = load i64, ptr %t, align 8
  %tuple.field275 = getelementptr inbounds nuw %_Z5SliceI2u8E, ptr %tuple274, i32 0, i32 0
  store i64 4, ptr %tuple.field275, align 1
  %tuple.field276 = getelementptr inbounds nuw %_Z5SliceI2u8E, ptr %tuple274, i32 0, i32 1
  store ptr @.str.59, ptr %tuple.field276, align 1
  %tuple.val277 = load %_Z5SliceI2u8E, ptr %tuple274, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %arg.tmp278, ptr align 1 %tuple274, i64 ptrtoint (ptr getelementptr (%_Z5SliceI2u8E, ptr null, i32 1) to i64), i1 false)
  %call279 = call i1 @_ZN9JsonValueB4json10literal_atE5SliceI2u8Em5SliceI2u8E(ptr %doc272, i64 %t273, ptr %arg.tmp278)
  br i1 %call279, label %if.then280, label %if.else281

if.end265:                                        ; preds = %if.end282, %if.then263
  br label %if.end248

if.then280:                                       ; preds = %if.else264
  %variant.tag.ptr284 = getelementptr inbounds nuw %_Z9JsonValue, ptr %variant.ptr283, i32 0, i32 0
  store i8 0, ptr %variant.tag.ptr284, align 1
  %variant.val285 = load %_Z9JsonValue, ptr %variant.ptr283, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %v, ptr align 1 %variant.ptr283, i64 ptrtoint (ptr getelementptr (%_Z9JsonValue, ptr null, i32 1) to i64), i1 false)
  %t286 = load i64, ptr %t, align 8
  %add287 = add i64 %t286, 4
  store i64 %add287, ptr %u, align 1
  br label %if.end282

if.else281:                                       ; preds = %if.else264
  store i64 3, ptr %error, align 1
  %t288 = load i64, ptr %t, align 8
  store i64 %t288, ptr %error_at, align 1
  br label %repeat.exit

if.end282:                                        ; preds = %if.then280
  br label %if.end265

if.then291:                                       ; preds = %land.rhs293
  %end304 = load i64, ptr %u, align 8
  store i64 %end304, ptr %t, align 1
  br label %if.end292

if.end292:                                        ; preds = %if.then291, %land.rhs293, %land.rhs294, %if.end208
  store i64 2, ptr %state, align 1
  br label %if.end166

land.rhs293:                                      ; preds = %land.rhs294
  %doc299 = load ptr, ptr %doc, align 8
  %end300 = load i64, ptr %u, align 8
  %call301 = call i8 @_ZN5SliceI2u8EixEm(ptr %doc299, i64 %end300)
  %call302 = call i1 @_ZN10JsonReaderB4json5is_wsE2u8(i8 %call301)
  %eq303 = icmp eq i1 %call302, false
  br i1 %eq303, label %if.then291, label %if.end292

land.rhs294:                                      ; preds = %if.end208
  %t296 = load i64, ptr %t, align 8
  %end297 = load i64, ptr %u, align 8
  %ne298 = icmp ne i64 %t296, %end297
  br i1 %ne298, label %land.rhs293, label %if.end292

if.then309:                                       ; preds = %if.end96
  %t311 = load i64, ptr %t, align 8
  %lt312 = icmp ult i64 %t311, %length
  br i1 %lt312, label %if.then313, label %if.end314

if.end310:                                        ; preds = %if.end96
  %open316 = load ptr, ptr %open, align 8
  %open317 = load ptr, ptr %open, align 8
  %load.struct318 = load %_Z5ArrayImE, ptr %open317, align 8
  %length319 = extractvalue %_Z5ArrayImE %load.struct318, 0
  %sub = sub i64 %length319, 1
  %call320 = call i64 @_ZN5ArrayImEixEm(ptr %open316, i64 %sub)
  %and = and i64 %call320, 1
  %eq321 = icmp eq i64 %and, 1
  br i1 %eq321, label %if.then322, label %if.else323

if.then313:                                       ; preds = %if.then309
  store i64 2, ptr %error, align 1
  %t315 = load i64, ptr %t, align 8
  store i64 %t315, ptr %error_at, align 1
  br label %if.end314

if.end314:                                        ; preds = %if.then313, %if.then309
  br label %repeat.exit

if.then322:                                       ; preds = %if.end310
  %members325 = load ptr, ptr %members, align 8
  %load.struct326 = load %_Z5ArrayI10JsonMemberE, ptr %members325, align 8
  %length327 = extractvalue %_Z5ArrayI10JsonMemberE %load.struct326, 0
  %sub328 = sub i64 %length327, 1
  %members330 = load ptr, ptr %members, align 8
  call void @_ZN5ArrayI10JsonMemberB4jsonEixEm(ptr noalias sret(%_Z10JsonMember) %sret.result329, ptr %members330, i64 %sub328)
  %members331 = load ptr, ptr %members, align 8
  %load.struct332 = load %_Z10JsonMember, ptr %sret.result329, align 8
  %key = extractvalue %_Z10JsonMember %load.struct332, 0
  %v333 = load %_Z9JsonValue, ptr %v, align 1
  %load.struct334 = load %_Z10JsonMember, ptr %sret.result329, align 8
  %verbatim_key = extractvalue %_Z10JsonMember %load.struct334, 2
  %tuple.field336 = getelementptr inbounds nuw %_Z10JsonMember, ptr %tuple335, i32 0, i32 0
  store %_Z5SliceI2u8E %key, ptr %tuple.field336, align 1
  %tuple.field337 = getelementptr inbounds nuw %_Z10JsonMember, ptr %tuple335, i32 0, i32 1
  store %_Z9JsonValue %v333, ptr %tuple.field337, align 1
  %tuple.field338 = getelementptr inbounds nuw %_Z10JsonMember, ptr %tuple335, i32 0, i32 2
  store i1 %verbatim_key, ptr %tuple.field338, align 1
  %tuple.val339 = load %_Z10JsonMember, ptr %tuple335, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %arg.tmp340, ptr align 1 %tuple335, i64 ptrtoint (ptr getelementptr (%_Z10JsonMember, ptr null, i32 1) to i64), i1 false)
  call void @_ZN5ArrayI10JsonMemberB4jsonE3putEm10JsonMemberB4json(ptr %members331, i64 %sub328, ptr %arg.tmp340)
  br label %if.end324

if.else323:                                       ; preds = %if.end310
  %values341 = load ptr, ptr %values, align 8
  call void @_ZN5ArrayI9JsonValueB4jsonE3addE9JsonValueB4json(ptr %values341, ptr %v)
  br label %if.end324

if.end324:                                        ; preds = %if.else323, %if.then322
  %t342 = load i64, ptr %t, align 8
  %ge343 = icmp uge i64 %t342, %length
  br i1 %ge343, label %if.then344, label %if.end345

if.then344:                                       ; preds = %if.end324
  store i64 1, ptr %error, align 1
  store i64 %length, ptr %error_at, align 1
  br label %repeat.exit

if.end345:                                        ; preds = %if.end324
  %doc346 = load ptr, ptr %doc, align 8
  %t347 = load i64, ptr %t, align 8
  %call348 = call i8 @_ZN5SliceI2u8EixEm(ptr %doc346, i64 %t347)
  %eq349 = icmp eq i8 %call348, 44
  br i1 %eq349, label %if.then350, label %if.end351

if.then350:                                       ; preds = %if.end345
  %doc352 = load ptr, ptr %doc, align 8
  %call353 = call i64 @_ZN10JsonStreamB4json4takeE5SliceI2u8E(ptr %s, ptr %doc352)
  store i64 %call353, ptr %t, align 1
  br i1 %eq321, label %if.then354, label %if.else355

if.end351:                                        ; preds = %if.end345
  %lshr = lshr i64 %call320, 1
  br i1 %eq321, label %land.rhs360, label %if.else358

if.then354:                                       ; preds = %if.then350
  store i64 1, ptr %state, align 1
  br label %if.end356

if.else355:                                       ; preds = %if.then350
  store i64 0, ptr %state, align 1
  br label %if.end356

if.end356:                                        ; preds = %if.else355, %if.then354
  %if.value = phi i64 [ 1, %if.then354 ], [ 0, %if.else355 ]
  br label %repeat.body

if.then357:                                       ; preds = %land.rhs360
  %arena363 = load ptr, ptr %arena, align 8
  %members366 = load ptr, ptr %members, align 8
  call void @_ZN5ArrayI10JsonMemberB4jsonE8as_sliceEv(ptr noalias sret(%_Z5SliceI10JsonMemberE) %sret.result365, ptr %members366)
  %base.deref = load ptr, ptr %members, align 8
  %field.inplace367 = getelementptr inbounds nuw %_Z5ArrayI10JsonMemberE, ptr %base.deref, i32 0, i32 0
  %field.val368 = load i64, ptr %field.inplace367, align 8
  call void @_ZN5SliceI10JsonMemberB4jsonE8subsliceEmm(ptr noalias sret(%_Z5SliceI10JsonMemberE) %sret.result364, ptr %sret.result365, i64 %lshr, i64 %field.val368)
  call void @_ZN9JsonArenaB4json12keep_membersEPN4scaly6memory4PageE5SliceI10JsonMemberB4jsonE(ptr noalias sret(%_Z5SliceI10JsonMemberE) %sret.result362, ptr null, ptr %arena363, ptr %sret.result364)
  %ptr.load = load ptr, ptr %members, align 8
  %length369 = getelementptr inbounds nuw %_Z5ArrayI10JsonMemberE, ptr %ptr.load, i32 0, i32 0
  store i64 %lshr, ptr %length369, align 8
  %variant.tag.ptr371 = getelementptr inbounds nuw %_Z9JsonValue, ptr %variant.ptr370, i32 0, i32 0
  store i8 5, ptr %variant.tag.ptr371, align 1
  %field.load373 = load %_Z5SliceI10JsonMemberE, ptr %sret.result362, align 8
  %tuple.field374 = getelementptr inbounds nuw %_Z10JsonObject, ptr %tuple372, i32 0, i32 0
  store %_Z5SliceI10JsonMemberE %field.load373, ptr %tuple.field374, align 1
  %tuple.val375 = load %_Z10JsonObject, ptr %tuple372, align 8
  %variant.data.ptr376 = getelementptr inbounds nuw %_Z9JsonValue, ptr %variant.ptr370, i32 0, i32 1
  store %_Z10JsonObject %tuple.val375, ptr %variant.data.ptr376, align 1
  %variant.val377 = load %_Z9JsonValue, ptr %variant.ptr370, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %v, ptr align 1 %variant.ptr370, i64 ptrtoint (ptr getelementptr (%_Z9JsonValue, ptr null, i32 1) to i64), i1 false)
  br label %if.end359

if.else358:                                       ; preds = %land.rhs360, %if.end351
  %eq382 = icmp eq i1 %eq321, false
  br i1 %eq382, label %land.rhs381, label %if.else379

if.end359:                                        ; preds = %if.end380, %if.then357
  %open403 = load ptr, ptr %open, align 8
  %load.struct404 = load %_Z5ArrayImE, ptr %open403, align 8
  %length405 = extractvalue %_Z5ArrayImE %load.struct404, 0
  %sub406 = sub i64 %length405, 1
  %ptr.load407 = load ptr, ptr %open, align 8
  %length408 = getelementptr inbounds nuw %_Z5ArrayImE, ptr %ptr.load407, i32 0, i32 0
  store i64 %sub406, ptr %length408, align 8
  %doc409 = load ptr, ptr %doc, align 8
  %call410 = call i64 @_ZN10JsonStreamB4json4takeE5SliceI2u8E(ptr %s, ptr %doc409)
  store i64 %call410, ptr %t, align 1
  br label %repeat.body

land.rhs360:                                      ; preds = %if.end351
  %eq361 = icmp eq i8 %call348, 125
  br i1 %eq361, label %if.then357, label %if.else358

if.then378:                                       ; preds = %land.rhs381
  %arena385 = load ptr, ptr %arena, align 8
  %values388 = load ptr, ptr %values, align 8
  call void @_ZN5ArrayI9JsonValueB4jsonE8as_sliceEv(ptr noalias sret(%_Z5SliceI9JsonValueE) %sret.result387, ptr %values388)
  %base.deref389 = load ptr, ptr %values, align 8
  %field.inplace390 = getelementptr inbounds nuw %_Z5ArrayI9JsonValueE, ptr %base.deref389, i32 0, i32 0
  %field.val391 = load i64, ptr %field.inplace390, align 8
  call void @_ZN5SliceI9JsonValueB4jsonE8subsliceEmm(ptr noalias sret(%_Z5SliceI9JsonValueE) %sret.result386, ptr %sret.result387, i64 %lshr, i64 %field.val391)
  call void @_ZN9JsonArenaB4json11keep_valuesEPN4scaly6memory4PageE5SliceI9JsonValueB4jsonE(ptr noalias sret(%_Z5SliceI9JsonValueE) %sret.result384, ptr null, ptr %arena385, ptr %sret.result386)
  %ptr.load392 = load ptr, ptr %values, align 8
  %length393 = getelementptr inbounds nuw %_Z5ArrayI9JsonValueE, ptr %ptr.load392, i32 0, i32 0
  store i64 %lshr, ptr %length393, align 8
  %variant.tag.ptr395 = getelementptr inbounds nuw %_Z9JsonValue, ptr %variant.ptr394, i32 0, i32 0
  store i8 4, ptr %variant.tag.ptr395, align 1
  %field.load397 = load %_Z5SliceI9JsonValueE, ptr %sret.result384, align 8
  %tuple.field398 = getelementptr inbounds nuw %_Z9JsonArray, ptr %tuple396, i32 0, i32 0
  store %_Z5SliceI9JsonValueE %field.load397, ptr %tuple.field398, align 1
  %tuple.val399 = load %_Z9JsonArray, ptr %tuple396, align 8
  %variant.data.ptr400 = getelementptr inbounds nuw %_Z9JsonValue, ptr %variant.ptr394, i32 0, i32 1
  store %_Z9JsonArray %tuple.val399, ptr %variant.data.ptr400, align 1
  %variant.val401 = load %_Z9JsonValue, ptr %variant.ptr394, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %v, ptr align 1 %variant.ptr394, i64 ptrtoint (ptr getelementptr (%_Z9JsonValue, ptr null, i32 1) to i64), i1 false)
  br label %if.end380

if.else379:                                       ; preds = %land.rhs381, %if.else358
  store i64 6, ptr %error, align 1
  %t402 = load i64, ptr %t, align 8
  store i64 %t402, ptr %error_at, align 1
  br label %repeat.exit

if.end380:                                        ; preds = %if.then378
  br label %if.end359

land.rhs381:                                      ; preds = %if.else358
  %eq383 = icmp eq i8 %call348, 93
  br i1 %eq383, label %if.then378, label %if.else379

if.then413:                                       ; preds = %repeat.exit
  %variant.tag.ptr416 = getelementptr inbounds nuw %_Z9JsonValue, ptr %variant.ptr415, i32 0, i32 0
  store i8 0, ptr %variant.tag.ptr416, align 1
  %variant.val417 = load %_Z9JsonValue, ptr %variant.ptr415, align 1
  %error418 = load i64, ptr %error, align 8
  %error_at419 = load i64, ptr %error_at, align 8
  %tuple.field421 = getelementptr inbounds nuw %_Z10JsonParsed, ptr %tuple420, i32 0, i32 0
  store %_Z9JsonValue %variant.val417, ptr %tuple.field421, align 1
  %tuple.field422 = getelementptr inbounds nuw %_Z10JsonParsed, ptr %tuple420, i32 0, i32 1
  store i64 %error418, ptr %tuple.field422, align 1
  %tuple.field423 = getelementptr inbounds nuw %_Z10JsonParsed, ptr %tuple420, i32 0, i32 2
  store i64 %error_at419, ptr %tuple.field423, align 1
  %tuple.val424 = load %_Z10JsonParsed, ptr %tuple420, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple420, i64 ptrtoint (ptr getelementptr (%_Z10JsonParsed, ptr null, i32 1) to i64), i1 false)
  call void @_Z19scaly_release_frameP5Frame(ptr %frame)
  ret void

if.end414:                                        ; preds = %repeat.exit
  %v425 = load %_Z9JsonValue, ptr %v, align 1
  %tuple.field426 = getelementptr inbounds nuw %_Z10JsonParsed, ptr %tuple420, i32 0, i32 0
  store %_Z9JsonValue %v425, ptr %tuple.field426, align 1
  %tuple.field427 = getelementptr inbounds nuw %_Z10JsonParsed, ptr %tuple420, i32 0, i32 1
  store i64 0, ptr %tuple.field427, align 1
  %tuple.field428 = getelementptr inbounds nuw %_Z10JsonParsed, ptr %tuple420, i32 0, i32 2
  store i64 0, ptr %tuple.field428, align 1
  %tuple.val429 = load %_Z10JsonParsed, ptr %tuple420, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple420, i64 ptrtoint (ptr getelementptr (%_Z10JsonParsed, ptr null, i32 1) to i64), i1 false)
  call void @_Z19scaly_release_frameP5Frame(ptr %frame)
  ret void
}

define linkonce_odr i1 @_ZN10JsonParsedB4json2okEv(ptr %0) {
entry:
  %load.struct = load %_Z10JsonParsed, ptr %0, align 8
  %error = extractvalue %_Z10JsonParsed %load.struct, 1
  %eq = icmp eq i64 %error, 0
  ret i1 %eq
}

define linkonce_odr void @_ZN10JsonParsedB4json7messageEv(ptr noalias sret(%_Z5SliceIcE) %0, ptr %1) {
entry:
  %sret.result = alloca %_Z5SliceIcE, align 8
  %field.inplace = getelementptr inbounds nuw %_Z10JsonParsed, ptr %1, i32 0, i32 1
  %field.val = load i64, ptr %field.inplace, align 8
  call void @_ZN10JsonReaderB4json7messageEi(ptr noalias sret(%_Z5SliceIcE) %sret.result, i64 %field.val)
  %sret.body = load %_Z5SliceIcE, ptr %sret.result, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %sret.result, i64 ptrtoint (ptr getelementptr (%_Z5SliceIcE, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN10JsonWriterB4json5bytesEv(ptr noalias sret(%_Z5SliceI2u8E) %0, ptr %1) {
entry:
  %sret.result = alloca %_Z5SliceI2u8E, align 8
  %field.inplace = getelementptr inbounds nuw %_Z10JsonWriter, ptr %1, i32 0, i32 0
  call void @_ZN5ArrayI2u8E8as_sliceEv(ptr noalias sret(%_Z5SliceI2u8E) %sret.result, ptr %field.inplace)
  %sret.body = load %_Z5SliceI2u8E, ptr %sret.result, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %sret.result, i64 ptrtoint (ptr getelementptr (%_Z5SliceI2u8E, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN10JsonWriterB4json9to_stringEPN4scaly6memory4PageE(ptr noalias sret({ ptr }) %0, ptr %1, ptr %2) {
entry:
  %frame.page = load ptr, ptr %1, align 8
  %frame.has_page = icmp ne ptr %frame.page, null
  br i1 %frame.has_page, label %frame.forced, label %frame.force

frame.force:                                      ; preds = %entry
  %forced_page = call ptr @_Z17scaly_force_frameP5Frame(ptr %1)
  br label %frame.forced

frame.forced:                                     ; preds = %frame.force, %entry
  %forced_page1 = phi ptr [ %frame.page, %entry ], [ %forced_page, %frame.force ]
  %struct.region = call ptr @_ZN4Page8allocateEmm(ptr %forced_page1, i64 ptrtoint (ptr getelementptr ({ ptr }, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr ({ i1, { ptr } }, ptr null, i64 0, i32 1) to i64))
  %field.inplace = getelementptr inbounds nuw %_Z10JsonWriter, ptr %2, i32 0, i32 0
  %call = call ptr @_ZN5ArrayI2u8E10get_bufferEv(ptr %field.inplace)
  %load.struct = load %_Z10JsonWriter, ptr %2, align 8
  %buffer = extractvalue %_Z10JsonWriter %load.struct, 0
  %length = extractvalue %_Z5ArrayI2u8E %buffer, 0
  call void @_ZN6StringC1EP10const_charm(ptr %struct.region, ptr %call, i64 %length)
  %sret.body = load { ptr }, ptr %struct.region, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %struct.region, i64 ptrtoint (ptr getelementptr ({ ptr }, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN10JsonWriterB4json5clearEv(ptr %0) {
entry:
  %field.inplace = getelementptr inbounds nuw %_Z10JsonWriter, ptr %0, i32 0, i32 0
  call void @_ZN5ArrayI2u8E5clearEv(ptr %field.inplace)
  %first = getelementptr inbounds nuw %_Z10JsonWriter, ptr %0, i32 0, i32 1
  store i1 true, ptr %first, align 1
  %after_key = getelementptr inbounds nuw %_Z10JsonWriter, ptr %0, i32 0, i32 2
  store i1 false, ptr %after_key, align 1
  ret void
}

define linkonce_odr void @_ZN10JsonWriterB4json8separateEv(ptr %0) {
entry:
  %load.struct = load %_Z10JsonWriter, ptr %0, align 8
  %after_key = extractvalue %_Z10JsonWriter %load.struct, 2
  br i1 %after_key, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %after_key1 = getelementptr inbounds nuw %_Z10JsonWriter, ptr %0, i32 0, i32 2
  store i1 false, ptr %after_key1, align 1
  ret void

if.end:                                           ; preds = %entry
  %load.struct2 = load %_Z10JsonWriter, ptr %0, align 8
  %first = extractvalue %_Z10JsonWriter %load.struct2, 1
  br i1 %first, label %if.then3, label %if.else

if.then3:                                         ; preds = %if.end
  %first5 = getelementptr inbounds nuw %_Z10JsonWriter, ptr %0, i32 0, i32 1
  store i1 false, ptr %first5, align 1
  br label %if.end4

if.else:                                          ; preds = %if.end
  call void @_ZN10JsonWriterB4json4byteE2u8(ptr %0, i8 44)
  br label %if.end4

if.end4:                                          ; preds = %if.else, %if.then3
  ret void
}

define linkonce_odr void @_ZN10JsonWriterB4json4byteE2u8(ptr %0, i8 %1) {
entry:
  %field.inplace = getelementptr inbounds nuw %_Z10JsonWriter, ptr %0, i32 0, i32 0
  call void @_ZN5ArrayI2u8E3addE2u8(ptr %field.inplace, i8 %1)
  ret void
}

define linkonce_odr void @_ZN10JsonWriterB4json12begin_objectEv(ptr %0) {
entry:
  call void @_ZN10JsonWriterB4json8separateEv(ptr %0)
  call void @_ZN10JsonWriterB4json4byteE2u8(ptr %0, i8 123)
  %first = getelementptr inbounds nuw %_Z10JsonWriter, ptr %0, i32 0, i32 1
  store i1 true, ptr %first, align 1
  ret void
}

define linkonce_odr void @_ZN10JsonWriterB4json10end_objectEv(ptr %0) {
entry:
  call void @_ZN10JsonWriterB4json4byteE2u8(ptr %0, i8 125)
  %first = getelementptr inbounds nuw %_Z10JsonWriter, ptr %0, i32 0, i32 1
  store i1 false, ptr %first, align 1
  ret void
}

define linkonce_odr void @_ZN10JsonWriterB4json11begin_arrayEv(ptr %0) {
entry:
  call void @_ZN10JsonWriterB4json8separateEv(ptr %0)
  call void @_ZN10JsonWriterB4json4byteE2u8(ptr %0, i8 91)
  %first = getelementptr inbounds nuw %_Z10JsonWriter, ptr %0, i32 0, i32 1
  store i1 true, ptr %first, align 1
  ret void
}

define linkonce_odr void @_ZN10JsonWriterB4json9end_arrayEv(ptr %0) {
entry:
  call void @_ZN10JsonWriterB4json4byteE2u8(ptr %0, i8 93)
  %first = getelementptr inbounds nuw %_Z10JsonWriter, ptr %0, i32 0, i32 1
  store i1 false, ptr %first, align 1
  ret void
}

define linkonce_odr void @_ZN10JsonWriterB4json6quotedE5SliceI2u8E(ptr %0, ptr %1) {
entry:
  %sret.result103 = alloca %_Z5SliceI2u8E, align 8
  %sret.result98 = alloca %_Z5SliceI2u8E, align 8
  %j = alloca i64, align 8
  %load.struct = load %_Z5SliceI2u8E, ptr %1, align 8
  %length = extractvalue %_Z5SliceI2u8E %load.struct, 0
  %load.struct1 = load %_Z10JsonWriter, ptr %0, align 8
  %buffer = extractvalue %_Z10JsonWriter %load.struct1, 0
  %length2 = extractvalue %_Z5ArrayI2u8E %buffer, 0
  %sret.result = alloca %_Z5SliceI2u8E, align 8
  %field.inplace = getelementptr inbounds nuw %_Z10JsonWriter, ptr %0, i32 0, i32 0
  %add = add i64 %length, 2
  call void @_ZN5ArrayI2u8E6extendEm(ptr noalias sret(%_Z5SliceI2u8E) %sret.result, ptr %field.inplace, i64 %add)
  call void @_ZN5SliceI2u8E3putEm2u8(ptr %sret.result, i64 0, i8 34)
  %i = alloca i64, align 8
  store i64 0, ptr %i, align 1
  br label %while.cond

while.cond:                                       ; preds = %if.end, %entry
  %i3 = load i64, ptr %i, align 8
  %add4 = add i64 %i3, 16
  %le = icmp ule i64 %add4, %length
  br i1 %le, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %i5 = load i64, ptr %i, align 8
  %simd.cont = getelementptr inbounds nuw %_Z5SliceI2u8E, ptr %1, i32 0, i32 0
  %simd.len = load i64, ptr %simd.cont, align 8
  %simd.cont6 = getelementptr inbounds nuw %_Z5SliceI2u8E, ptr %1, i32 0, i32 1
  %simd.data = load ptr, ptr %simd.cont6, align 8
  %simd.fits = icmp uge i64 %simd.len, 16
  %simd.room = sub i64 %simd.len, 16
  %simd.within = icmp ule i64 %i5, %simd.room
  %simd.inrange = and i1 %simd.fits, %simd.within
  br i1 %simd.inrange, label %simd.mem.ok, label %simd.mem.oob

while.exit:                                       ; preds = %if.then, %while.cond
  %i27 = load i64, ptr %i, align 8
  store i64 %i27, ptr %j, align 1
  br label %while.cond28

simd.mem.oob:                                     ; preds = %while.body
  call void @_Z15scaly_panic_oobP10const_charmm(ptr @.simd.mem.78, i64 %i5, i64 %simd.len)
  unreachable

simd.mem.ok:                                      ; preds = %while.body
  %simd.addr = getelementptr inbounds i8, ptr %simd.data, i64 %i5
  %simd.load = load <16 x i8>, ptr %simd.addr, align 1
  %i7 = load i64, ptr %i, align 8
  %add8 = add i64 %i7, 1
  %simd.cont9 = getelementptr inbounds nuw %_Z5SliceI2u8E, ptr %sret.result, i32 0, i32 0
  %simd.len10 = load i64, ptr %simd.cont9, align 8
  %simd.cont11 = getelementptr inbounds nuw %_Z5SliceI2u8E, ptr %sret.result, i32 0, i32 1
  %simd.data12 = load ptr, ptr %simd.cont11, align 8
  %simd.fits13 = icmp uge i64 %simd.len10, 16
  %simd.room14 = sub i64 %simd.len10, 16
  %simd.within15 = icmp ule i64 %add8, %simd.room14
  %simd.inrange16 = and i1 %simd.fits13, %simd.within15
  br i1 %simd.inrange16, label %simd.mem.ok18, label %simd.mem.oob17

simd.mem.oob17:                                   ; preds = %simd.mem.ok
  call void @_Z15scaly_panic_oobP10const_charmm(ptr @.simd.mem.79, i64 %add8, i64 %simd.len10)
  unreachable

simd.mem.ok18:                                    ; preds = %simd.mem.ok
  %simd.addr19 = getelementptr inbounds i8, ptr %simd.data12, i64 %add8
  store <16 x i8> %simd.load, ptr %simd.addr19, align 1
  %simd.cmp = icmp eq <16 x i8> %simd.load, splat (i8 34)
  %simd.cmp20 = icmp eq <16 x i8> %simd.load, splat (i8 92)
  %simd.or = or <16 x i1> %simd.cmp, %simd.cmp20
  %simd.cmp21 = icmp ult <16 x i8> %simd.load, splat (i8 32)
  %simd.or22 = or <16 x i1> %simd.or, %simd.cmp21
  %simd.bits = bitcast <16 x i1> %simd.or22 to i16
  %simd.bits64 = zext i16 %simd.bits to i64
  %ne = icmp ne i64 %simd.bits64, 0
  br i1 %ne, label %if.then, label %if.end

if.then:                                          ; preds = %simd.mem.ok18
  %i23 = load i64, ptr %i, align 8
  %call = call i64 @_Z14trailing_zeros3u64(i64 %simd.bits64)
  %add24 = add i64 %i23, %call
  store i64 %add24, ptr %i, align 1
  br label %while.exit

if.end:                                           ; preds = %simd.mem.ok18
  %i25 = load i64, ptr %i, align 8
  %add26 = add i64 %i25, 16
  store i64 %add26, ptr %i, align 1
  br label %while.cond

while.cond28:                                     ; preds = %if.end82, %while.exit
  %j31 = load i64, ptr %j, align 8
  %add32 = add i64 %j31, 8
  %le33 = icmp ule i64 %add32, %length
  br i1 %le33, label %while.body29, label %while.exit30

while.body29:                                     ; preds = %while.cond28
  %load.struct34 = load %_Z5SliceI2u8E, ptr %1, align 8
  %data = extractvalue %_Z5SliceI2u8E %load.struct34, 1
  %j35 = load i64, ptr %j, align 8
  %ptr.add = getelementptr inbounds i8, ptr %data, i64 %j35
  %deref = load i8, ptr %ptr.add, align 1
  %as.zext = zext i8 %deref to i64
  %ptr.add36 = getelementptr inbounds i8, ptr %ptr.add, i64 1
  %deref37 = load i8, ptr %ptr.add36, align 1
  %as.zext38 = zext i8 %deref37 to i64
  %shl = shl i64 %as.zext38, 8
  %or = or i64 %as.zext, %shl
  %ptr.add39 = getelementptr inbounds i8, ptr %ptr.add, i64 2
  %deref40 = load i8, ptr %ptr.add39, align 1
  %as.zext41 = zext i8 %deref40 to i64
  %shl42 = shl i64 %as.zext41, 16
  %or43 = or i64 %or, %shl42
  %ptr.add44 = getelementptr inbounds i8, ptr %ptr.add, i64 3
  %deref45 = load i8, ptr %ptr.add44, align 1
  %as.zext46 = zext i8 %deref45 to i64
  %shl47 = shl i64 %as.zext46, 24
  %or48 = or i64 %or43, %shl47
  %ptr.add49 = getelementptr inbounds i8, ptr %ptr.add, i64 4
  %deref50 = load i8, ptr %ptr.add49, align 1
  %as.zext51 = zext i8 %deref50 to i64
  %shl52 = shl i64 %as.zext51, 32
  %or53 = or i64 %or48, %shl52
  %ptr.add54 = getelementptr inbounds i8, ptr %ptr.add, i64 5
  %deref55 = load i8, ptr %ptr.add54, align 1
  %as.zext56 = zext i8 %deref55 to i64
  %shl57 = shl i64 %as.zext56, 40
  %or58 = or i64 %or53, %shl57
  %ptr.add59 = getelementptr inbounds i8, ptr %ptr.add, i64 6
  %deref60 = load i8, ptr %ptr.add59, align 1
  %as.zext61 = zext i8 %deref60 to i64
  %shl62 = shl i64 %as.zext61, 48
  %or63 = or i64 %or58, %shl62
  %ptr.add64 = getelementptr inbounds i8, ptr %ptr.add, i64 7
  %deref65 = load i8, ptr %ptr.add64, align 1
  %as.zext66 = zext i8 %deref65 to i64
  %shl67 = shl i64 %as.zext66, 56
  %or68 = or i64 %or63, %shl67
  %xor = xor i64 %or68, 2459565876494606882
  %xor69 = xor i64 %or68, 6655295901103053916
  %sub = sub i64 %or68, 2314885530818453536
  %xor70 = xor i64 %or68, -1
  %and = and i64 %sub, %xor70
  %sub71 = sub i64 %xor, 72340172838076673
  %xor72 = xor i64 %xor, -1
  %and73 = and i64 %sub71, %xor72
  %sub74 = sub i64 %xor69, 72340172838076673
  %xor75 = xor i64 %xor69, -1
  %and76 = and i64 %sub74, %xor75
  %or77 = or i64 %and, %and73
  %or78 = or i64 %or77, %and76
  %and79 = and i64 %or78, -9187201950435737472
  %ne80 = icmp ne i64 %and79, 0
  br i1 %ne80, label %if.then81, label %if.end82

while.exit30:                                     ; preds = %if.then81, %while.cond28
  br label %while.cond85

if.then81:                                        ; preds = %while.body29
  br label %while.exit30

if.end82:                                         ; preds = %while.body29
  %j83 = load i64, ptr %j, align 8
  %add84 = add i64 %j83, 8
  store i64 %add84, ptr %j, align 1
  br label %while.cond28

while.cond85:                                     ; preds = %while.body86, %while.exit30
  %j88 = load i64, ptr %j, align 8
  %lt = icmp ult i64 %j88, %length
  br i1 %lt, label %lor.rhs, label %lor.end

while.body86:                                     ; preds = %lor.end
  %j92 = load i64, ptr %j, align 8
  %add93 = add i64 %j92, 1
  store i64 %add93, ptr %j, align 1
  br label %while.cond85

while.exit87:                                     ; preds = %lor.end
  %j94 = load i64, ptr %j, align 8
  %i95 = load i64, ptr %i, align 8
  %gt = icmp ugt i64 %j94, %i95
  br i1 %gt, label %if.then96, label %if.end97

lor.rhs:                                          ; preds = %while.cond85
  %j89 = load i64, ptr %j, align 8
  %call90 = call i8 @_ZN5SliceI2u8EixEm(ptr %1, i64 %j89)
  %call91 = call i1 @_ZN10JsonWriterB4json5plainE2u8(i8 %call90)
  br label %lor.end

lor.end:                                          ; preds = %lor.rhs, %while.cond85
  %lor.result = phi i1 [ false, %while.cond85 ], [ %call91, %lor.rhs ]
  br i1 %lor.result, label %while.body86, label %while.exit87

if.then96:                                        ; preds = %while.exit87
  %i99 = load i64, ptr %i, align 8
  %add100 = add i64 %i99, 1
  call void @_ZN5SliceI2u8E10slice_fromEPN4scaly6memory4PageEm(ptr noalias sret(%_Z5SliceI2u8E) %sret.result98, ptr null, ptr %sret.result, i64 %add100)
  %load.struct101 = load %_Z5SliceI2u8E, ptr %sret.result98, align 8
  %data102 = extractvalue %_Z5SliceI2u8E %load.struct101, 1
  %i104 = load i64, ptr %i, align 8
  call void @_ZN5SliceI2u8E10slice_fromEPN4scaly6memory4PageEm(ptr noalias sret(%_Z5SliceI2u8E) %sret.result103, ptr null, ptr %1, i64 %i104)
  %load.struct105 = load %_Z5SliceI2u8E, ptr %sret.result103, align 8
  %data106 = extractvalue %_Z5SliceI2u8E %load.struct105, 1
  %j107 = load i64, ptr %j, align 8
  %i108 = load i64, ptr %i, align 8
  %sub109 = sub i64 %j107, %i108
  %call110 = call ptr @memcpy(ptr %data102, ptr %data106, i64 %sub109)
  br label %if.end97

if.end97:                                         ; preds = %if.then96, %while.exit87
  %j111 = load i64, ptr %j, align 8
  store i64 %j111, ptr %i, align 1
  %i112 = load i64, ptr %i, align 8
  %eq = icmp eq i64 %i112, %length
  br i1 %eq, label %if.then113, label %if.end114

if.then113:                                       ; preds = %if.end97
  %add115 = add i64 %length, 1
  call void @_ZN5SliceI2u8E3putEm2u8(ptr %sret.result, i64 %add115, i8 34)
  ret void

if.end114:                                        ; preds = %if.end97
  %add116 = add i64 %length2, 1
  %i117 = load i64, ptr %i, align 8
  %add118 = add i64 %add116, %i117
  %buffer119 = getelementptr inbounds nuw %_Z10JsonWriter, ptr %0, i32 0, i32 0
  %length120 = getelementptr inbounds nuw %_Z5ArrayI2u8E, ptr %buffer119, i32 0, i32 0
  store i64 %add118, ptr %length120, align 8
  %i121 = load i64, ptr %i, align 8
  call void @_ZN10JsonWriterB4json11quoted_restE5SliceI2u8Em(ptr %0, ptr %1, i64 %i121)
  ret void
}

define linkonce_odr void @_ZN10JsonWriterB4json3keyE5SliceI2u8E(ptr %0, ptr %1) {
entry:
  call void @_ZN10JsonWriterB4json8separateEv(ptr %0)
  call void @_ZN10JsonWriterB4json6quotedE5SliceI2u8E(ptr %0, ptr %1)
  call void @_ZN10JsonWriterB4json4byteE2u8(ptr %0, i8 58)
  %after_key = getelementptr inbounds nuw %_Z10JsonWriter, ptr %0, i32 0, i32 2
  store i1 true, ptr %after_key, align 1
  ret void
}

define linkonce_odr void @_ZN10JsonWriterB4json4textE5SliceI2u8E(ptr %0, ptr %1) {
entry:
  call void @_ZN10JsonWriterB4json8separateEv(ptr %0)
  call void @_ZN10JsonWriterB4json6quotedE5SliceI2u8E(ptr %0, ptr %1)
  ret void
}

define linkonce_odr void @_ZN10JsonWriterB4json7integerE3i64(ptr %0, i64 %1) {
entry:
  %at = alloca i64, align 8
  call void @_ZN10JsonWriterB4json8separateEv(ptr %0)
  %digits = alloca [20 x i8], align 1
  %arr.ptr = getelementptr inbounds [20 x i8], ptr %digits, i64 0, i64 0
  %digits1 = alloca ptr, align 8
  store ptr %arr.ptr, ptr %digits1, align 8
  %v = alloca i64, align 8
  store i64 %1, ptr %v, align 1
  %lt = icmp slt i64 %1, 0
  br i1 %lt, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  call void @_ZN10JsonWriterB4json4byteE2u8(ptr %0, i8 45)
  %v2 = load i64, ptr %v, align 8
  %sub = sub i64 0, %v2
  store i64 %sub, ptr %v, align 1
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  store i64 20, ptr %at, align 1
  br label %repeat.body

repeat.body:                                      ; preds = %if.end11, %if.end
  %at3 = load i64, ptr %at, align 8
  %sub4 = sub i64 %at3, 1
  store i64 %sub4, ptr %at, align 1
  %v5 = load i64, ptr %v, align 8
  %urem = urem i64 %v5, 10
  %as.trunc = trunc i64 %urem to i8
  %add = add i8 %as.trunc, 48
  %digits6 = load ptr, ptr %digits1, align 8
  %at7 = load i64, ptr %at, align 8
  %ptr.add = getelementptr inbounds i8, ptr %digits6, i64 %at7
  store i8 %add, ptr %ptr.add, align 1
  %v8 = load i64, ptr %v, align 8
  %udiv = udiv i64 %v8, 10
  store i64 %udiv, ptr %v, align 1
  %v9 = load i64, ptr %v, align 8
  %eq = icmp eq i64 %v9, 0
  br i1 %eq, label %if.then10, label %if.end11

repeat.exit:                                      ; preds = %if.then10
  br label %while.cond

if.then10:                                        ; preds = %repeat.body
  br label %repeat.exit

if.end11:                                         ; preds = %repeat.body
  br label %repeat.body

while.cond:                                       ; preds = %while.body, %repeat.exit
  %at12 = load i64, ptr %at, align 8
  %lt13 = icmp ult i64 %at12, 20
  br i1 %lt13, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %field.inplace = getelementptr inbounds nuw %_Z10JsonWriter, ptr %0, i32 0, i32 0
  %digits14 = load ptr, ptr %digits1, align 8
  %at15 = load i64, ptr %at, align 8
  %ptr.add16 = getelementptr inbounds i8, ptr %digits14, i64 %at15
  %deref = load i8, ptr %ptr.add16, align 1
  call void @_ZN5ArrayI2u8E3addE2u8(ptr %field.inplace, i8 %deref)
  %at17 = load i64, ptr %at, align 8
  %add18 = add i64 %at17, 1
  store i64 %add18, ptr %at, align 1
  br label %while.cond

while.exit:                                       ; preds = %while.cond
  ret void
}

define linkonce_odr void @_ZN10JsonWriterB4json6appendE5SliceI2u8E(ptr %0, ptr %1) {
entry:
  %load.struct = load %_Z5SliceI2u8E, ptr %1, align 8
  %length = extractvalue %_Z5SliceI2u8E %load.struct, 0
  %eq = icmp eq i64 %length, 0
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret void

if.end:                                           ; preds = %entry
  %field.inplace = getelementptr inbounds nuw %_Z10JsonWriter, ptr %0, i32 0, i32 0
  call void @_ZN5ArrayI2u8E3addE5SliceI2u8E(ptr %field.inplace, ptr %1)
  ret void
}

define linkonce_odr void @_ZN10JsonWriterB4json6numberE5SliceI2u8E(ptr %0, ptr %1) {
entry:
  call void @_ZN10JsonWriterB4json8separateEv(ptr %0)
  call void @_ZN10JsonWriterB4json6appendE5SliceI2u8E(ptr %0, ptr %1)
  ret void
}

define linkonce_odr void @_ZN10JsonWriterB4json7booleanEb(ptr %0, i1 %1) {
entry:
  %arg.tmp6 = alloca %_Z5SliceI2u8E, align 8
  %tuple2 = alloca %_Z5SliceI2u8E, align 8
  %arg.tmp = alloca %_Z5SliceI2u8E, align 8
  %tuple = alloca %_Z5SliceI2u8E, align 8
  call void @_ZN10JsonWriterB4json8separateEv(ptr %0)
  br i1 %1, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %tuple.field = getelementptr inbounds nuw %_Z5SliceI2u8E, ptr %tuple, i32 0, i32 0
  store i64 4, ptr %tuple.field, align 1
  %tuple.field1 = getelementptr inbounds nuw %_Z5SliceI2u8E, ptr %tuple, i32 0, i32 1
  store ptr @.str.60, ptr %tuple.field1, align 1
  %tuple.val = load %_Z5SliceI2u8E, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %arg.tmp, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z5SliceI2u8E, ptr null, i32 1) to i64), i1 false)
  call void @_ZN10JsonWriterB4json6appendE5SliceI2u8E(ptr %0, ptr %arg.tmp)
  br label %if.end

if.else:                                          ; preds = %entry
  %tuple.field3 = getelementptr inbounds nuw %_Z5SliceI2u8E, ptr %tuple2, i32 0, i32 0
  store i64 5, ptr %tuple.field3, align 1
  %tuple.field4 = getelementptr inbounds nuw %_Z5SliceI2u8E, ptr %tuple2, i32 0, i32 1
  store ptr @.str.61, ptr %tuple.field4, align 1
  %tuple.val5 = load %_Z5SliceI2u8E, ptr %tuple2, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %arg.tmp6, ptr align 1 %tuple2, i64 ptrtoint (ptr getelementptr (%_Z5SliceI2u8E, ptr null, i32 1) to i64), i1 false)
  call void @_ZN10JsonWriterB4json6appendE5SliceI2u8E(ptr %0, ptr %arg.tmp6)
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  ret void
}

define linkonce_odr void @_ZN10JsonWriterB4json10null_valueEv(ptr %0) {
entry:
  call void @_ZN10JsonWriterB4json8separateEv(ptr %0)
  %tuple = alloca %_Z5SliceI2u8E, align 8
  %tuple.field = getelementptr inbounds nuw %_Z5SliceI2u8E, ptr %tuple, i32 0, i32 0
  store i64 4, ptr %tuple.field, align 1
  %tuple.field1 = getelementptr inbounds nuw %_Z5SliceI2u8E, ptr %tuple, i32 0, i32 1
  store ptr @.str.62, ptr %tuple.field1, align 1
  %tuple.val = load %_Z5SliceI2u8E, ptr %tuple, align 8
  %arg.tmp = alloca %_Z5SliceI2u8E, align 8
  store %_Z5SliceI2u8E %tuple.val, ptr %arg.tmp, align 1
  call void @_ZN10JsonWriterB4json6appendE5SliceI2u8E(ptr %0, ptr %arg.tmp)
  ret void
}

define linkonce_odr ptr @_ZN5ArrayI14JsonWriteFrameB4jsonE10get_bufferEv(ptr %0) {
entry:
  %load.struct = load %_Z5ArrayI14JsonWriteFrameE, ptr %0, align 8
  %buffer = extractvalue %_Z5ArrayI14JsonWriteFrameE %load.struct, 2
  ret ptr %buffer
}

define linkonce_odr i64 @_ZN5ArrayI14JsonWriteFrameB4jsonE10get_lengthEv(ptr %0) {
entry:
  %load.struct = load %_Z5ArrayI14JsonWriteFrameE, ptr %0, align 8
  %length = extractvalue %_Z5ArrayI14JsonWriteFrameE %load.struct, 0
  ret i64 %length
}

define linkonce_odr i64 @_ZN5ArrayI14JsonWriteFrameB4jsonE12get_capacityEv(ptr %0) {
entry:
  %load.struct = load %_Z5ArrayI14JsonWriteFrameE, ptr %0, align 8
  %capacity = extractvalue %_Z5ArrayI14JsonWriteFrameE %load.struct, 1
  ret i64 %capacity
}

define linkonce_odr void @_ZN5ArrayI14JsonWriteFrameB4jsonE10reallocateEv(ptr %0) {
entry:
  %first_cap = alloca i64, align 8
  %call = call ptr @_ZN4Page3getEPv(ptr %0)
  %load.struct = load %_Z5ArrayI14JsonWriteFrameE, ptr %0, align 8
  %buffer = extractvalue %_Z5ArrayI14JsonWriteFrameE %load.struct, 2
  %eq = icmp eq ptr %buffer, null
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %udiv = udiv i64 32, ptrtoint (ptr getelementptr (%_Z14JsonWriteFrame, ptr null, i32 1) to i64)
  store i64 %udiv, ptr %first_cap, align 1
  %first_cap1 = load i64, ptr %first_cap, align 8
  %lt = icmp ult i64 %first_cap1, 1
  br i1 %lt, label %if.then2, label %if.end3

if.end:                                           ; preds = %entry
  %load.struct18 = load %_Z5ArrayI14JsonWriteFrameE, ptr %0, align 8
  %capacity19 = extractvalue %_Z5ArrayI14JsonWriteFrameE %load.struct18, 1
  %mul20 = mul i64 %capacity19, ptrtoint (ptr getelementptr (%_Z14JsonWriteFrame, ptr null, i32 1) to i64)
  %le21 = icmp ule i64 %mul20, 1024
  %load.struct22 = load %_Z5ArrayI14JsonWriteFrameE, ptr %0, align 8
  %buffer23 = extractvalue %_Z5ArrayI14JsonWriteFrameE %load.struct22, 2
  %load.struct24 = load %_Z5ArrayI14JsonWriteFrameE, ptr %0, align 8
  %capacity25 = extractvalue %_Z5ArrayI14JsonWriteFrameE %load.struct24, 1
  %load.struct26 = load %_Z5ArrayI14JsonWriteFrameE, ptr %0, align 8
  %capacity27 = extractvalue %_Z5ArrayI14JsonWriteFrameE %load.struct26, 1
  %mul28 = mul i64 %capacity27, 2
  store i64 %mul28, ptr %first_cap, align 1
  %new_capacity = load i64, ptr %first_cap, align 8
  %mul29 = mul i64 %new_capacity, ptrtoint (ptr getelementptr (%_Z14JsonWriteFrame, ptr null, i32 1) to i64)
  %le30 = icmp ule i64 %mul29, 1024
  br i1 %le30, label %if.then31, label %if.end32

if.then2:                                         ; preds = %if.then
  store i64 1, ptr %first_cap, align 1
  br label %if.end3

if.end3:                                          ; preds = %if.then2, %if.then
  %first_cap4 = load i64, ptr %first_cap, align 8
  %mul = mul i64 %first_cap4, ptrtoint (ptr getelementptr (%_Z14JsonWriteFrame, ptr null, i32 1) to i64)
  %le = icmp ule i64 %mul, 1024
  br i1 %le, label %if.then5, label %if.end6

if.then5:                                         ; preds = %if.end3
  %first_cap7 = load i64, ptr %first_cap, align 8
  %mul8 = mul i64 %first_cap7, ptrtoint (ptr getelementptr (%_Z14JsonWriteFrame, ptr null, i32 1) to i64)
  %call9 = call ptr @_ZN4Page8allocateEmm(ptr %call, i64 %mul8, i64 8)
  %buffer10 = getelementptr inbounds nuw %_Z5ArrayI14JsonWriteFrameE, ptr %0, i32 0, i32 2
  store ptr %call9, ptr %buffer10, align 8
  %first_cap11 = load i64, ptr %first_cap, align 8
  %capacity = getelementptr inbounds nuw %_Z5ArrayI14JsonWriteFrameE, ptr %0, i32 0, i32 1
  store i64 %first_cap11, ptr %capacity, align 8
  ret void

if.end6:                                          ; preds = %if.end3
  %first_cap12 = load i64, ptr %first_cap, align 8
  %mul13 = mul i64 %first_cap12, ptrtoint (ptr getelementptr (%_Z14JsonWriteFrame, ptr null, i32 1) to i64)
  %call14 = call ptr @_ZN4Page25allocate_exclusive_bufferEmm(ptr %call, i64 %mul13, i64 8)
  %buffer15 = getelementptr inbounds nuw %_Z5ArrayI14JsonWriteFrameE, ptr %0, i32 0, i32 2
  store ptr %call14, ptr %buffer15, align 8
  %first_cap16 = load i64, ptr %first_cap, align 8
  %capacity17 = getelementptr inbounds nuw %_Z5ArrayI14JsonWriteFrameE, ptr %0, i32 0, i32 1
  store i64 %first_cap16, ptr %capacity17, align 8
  ret void

if.then31:                                        ; preds = %if.end
  %new_capacity33 = load i64, ptr %first_cap, align 8
  %mul34 = mul i64 %new_capacity33, ptrtoint (ptr getelementptr (%_Z14JsonWriteFrame, ptr null, i32 1) to i64)
  %call35 = call ptr @_ZN4Page8allocateEmm(ptr %call, i64 %mul34, i64 8)
  %buffer36 = getelementptr inbounds nuw %_Z5ArrayI14JsonWriteFrameE, ptr %0, i32 0, i32 2
  store ptr %call35, ptr %buffer36, align 8
  %new_capacity37 = load i64, ptr %first_cap, align 8
  %capacity38 = getelementptr inbounds nuw %_Z5ArrayI14JsonWriteFrameE, ptr %0, i32 0, i32 1
  store i64 %new_capacity37, ptr %capacity38, align 8
  %field.inplace = getelementptr inbounds nuw %_Z5ArrayI14JsonWriteFrameE, ptr %0, i32 0, i32 2
  %deref.recv = load ptr, ptr %field.inplace, align 8
  %mul39 = mul i64 %capacity25, ptrtoint (ptr getelementptr (%_Z14JsonWriteFrame, ptr null, i32 1) to i64)
  %call40 = call ptr @memcpy(ptr %deref.recv, ptr %buffer23, i64 %mul39)
  ret void

if.end32:                                         ; preds = %if.end
  %call41 = call ptr @_ZN4Page23allocate_exclusive_pageEv(ptr %call)
  %call42 = call i64 @_ZN4Page12get_capacityEm(ptr %call41, i64 8)
  call void @_ZN4Page25deallocate_exclusive_pageER4Page(ptr %call, ptr %call41)
  %new_capacity43 = load i64, ptr %first_cap, align 8
  %udiv44 = udiv i64 %call42, ptrtoint (ptr getelementptr (%_Z14JsonWriteFrame, ptr null, i32 1) to i64)
  %lt45 = icmp ult i64 %new_capacity43, %udiv44
  br i1 %lt45, label %if.then46, label %if.end47

if.then46:                                        ; preds = %if.end32
  %udiv48 = udiv i64 %call42, ptrtoint (ptr getelementptr (%_Z14JsonWriteFrame, ptr null, i32 1) to i64)
  store i64 %udiv48, ptr %first_cap, align 1
  br label %if.end47

if.end47:                                         ; preds = %if.then46, %if.end32
  %new_capacity49 = load i64, ptr %first_cap, align 8
  %mul50 = mul i64 %new_capacity49, ptrtoint (ptr getelementptr (%_Z14JsonWriteFrame, ptr null, i32 1) to i64)
  %call51 = call ptr @_ZN4Page25allocate_exclusive_bufferEmm(ptr %call, i64 %mul50, i64 8)
  %buffer52 = getelementptr inbounds nuw %_Z5ArrayI14JsonWriteFrameE, ptr %0, i32 0, i32 2
  store ptr %call51, ptr %buffer52, align 8
  %new_capacity53 = load i64, ptr %first_cap, align 8
  %capacity54 = getelementptr inbounds nuw %_Z5ArrayI14JsonWriteFrameE, ptr %0, i32 0, i32 1
  store i64 %new_capacity53, ptr %capacity54, align 8
  %field.inplace55 = getelementptr inbounds nuw %_Z5ArrayI14JsonWriteFrameE, ptr %0, i32 0, i32 2
  %deref.recv56 = load ptr, ptr %field.inplace55, align 8
  %mul57 = mul i64 %capacity25, ptrtoint (ptr getelementptr (%_Z14JsonWriteFrame, ptr null, i32 1) to i64)
  %call58 = call ptr @memcpy(ptr %deref.recv56, ptr %buffer23, i64 %mul57)
  %eq59 = icmp eq i1 %le21, false
  br i1 %eq59, label %if.then60, label %if.end61

if.then60:                                        ; preds = %if.end47
  %call62 = call ptr @_ZN4Page3getEPv(ptr %buffer23)
  call void @_ZN4Page25deallocate_exclusive_pageER4Page(ptr %call, ptr %call62)
  br label %if.end61

if.end61:                                         ; preds = %if.then60, %if.end47
  ret void
}

define linkonce_odr void @_ZN5ArrayI14JsonWriteFrameB4jsonE3addE14JsonWriteFrameB4json(ptr %0, ptr %1) {
entry:
  %load.struct = load %_Z5ArrayI14JsonWriteFrameE, ptr %0, align 8
  %buffer = extractvalue %_Z5ArrayI14JsonWriteFrameE %load.struct, 2
  %eq = icmp eq ptr %buffer, null
  br i1 %eq, label %if.then, label %lor.rhs

if.then:                                          ; preds = %lor.rhs, %entry
  call void @_ZN5ArrayI14JsonWriteFrameB4jsonE10reallocateEv(ptr %0)
  br label %if.end

if.end:                                           ; preds = %if.then, %lor.rhs
  %load.struct4 = load %_Z5ArrayI14JsonWriteFrameE, ptr %0, align 8
  %buffer5 = extractvalue %_Z5ArrayI14JsonWriteFrameE %load.struct4, 2
  %load.struct6 = load %_Z5ArrayI14JsonWriteFrameE, ptr %0, align 8
  %length7 = extractvalue %_Z5ArrayI14JsonWriteFrameE %load.struct6, 0
  %ptr.add = getelementptr inbounds %_Z14JsonWriteFrame, ptr %buffer5, i64 %length7
  %store.load = load %_Z14JsonWriteFrame, ptr %1, align 8
  store %_Z14JsonWriteFrame %store.load, ptr %ptr.add, align 8
  %load.struct8 = load %_Z5ArrayI14JsonWriteFrameE, ptr %0, align 8
  %length9 = extractvalue %_Z5ArrayI14JsonWriteFrameE %load.struct8, 0
  %add = add i64 %length9, 1
  %length10 = getelementptr inbounds nuw %_Z5ArrayI14JsonWriteFrameE, ptr %0, i32 0, i32 0
  store i64 %add, ptr %length10, align 8
  ret void

lor.rhs:                                          ; preds = %entry
  %load.struct1 = load %_Z5ArrayI14JsonWriteFrameE, ptr %0, align 8
  %length = extractvalue %_Z5ArrayI14JsonWriteFrameE %load.struct1, 0
  %load.struct2 = load %_Z5ArrayI14JsonWriteFrameE, ptr %0, align 8
  %capacity = extractvalue %_Z5ArrayI14JsonWriteFrameE %load.struct2, 1
  %eq3 = icmp eq i64 %length, %capacity
  br i1 %eq3, label %if.then, label %if.end
}

define linkonce_odr ptr @_ZN6VectorI14JsonWriteFrameB4jsonE3getEm(ptr noalias %0, i64 %1) {
entry:
  %load.struct = load %_Z6VectorI14JsonWriteFrameE, ptr %0, align 8
  %length = extractvalue %_Z6VectorI14JsonWriteFrameE %load.struct, 0
  %ge = icmp uge i64 %1, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %load.struct1 = load %_Z6VectorI14JsonWriteFrameE, ptr %0, align 8
  %data = extractvalue %_Z6VectorI14JsonWriteFrameE %load.struct1, 1
  %ptr.add = getelementptr inbounds %_Z14JsonWriteFrame, ptr %data, i64 %1
  ret ptr %ptr.add
}

define linkonce_odr ptr @_ZN6VectorI14JsonWriteFrameB4jsonE2atEm(ptr noalias %0, i64 %1) {
entry:
  %load.struct = load %_Z6VectorI14JsonWriteFrameE, ptr %0, align 8
  %length = extractvalue %_Z6VectorI14JsonWriteFrameE %load.struct, 0
  %ge = icmp uge i64 %1, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %field.inplace = getelementptr inbounds nuw %_Z6VectorI14JsonWriteFrameE, ptr %0, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  call void @_Z15scaly_panic_oobP10const_charmm(ptr @.str.63, i64 %1, i64 %field.val)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %load.struct1 = load %_Z6VectorI14JsonWriteFrameE, ptr %0, align 8
  %data = extractvalue %_Z6VectorI14JsonWriteFrameE %load.struct1, 1
  %ptr.add = getelementptr inbounds %_Z14JsonWriteFrame, ptr %data, i64 %1
  ret ptr %ptr.add
}

define linkonce_odr ptr @_ZN6VectorI14JsonWriteFrameB4jsonE7get_ptrEm(ptr noalias %0, i64 %1) {
entry:
  %load.struct = load %_Z6VectorI14JsonWriteFrameE, ptr %0, align 8
  %length = extractvalue %_Z6VectorI14JsonWriteFrameE %load.struct, 0
  %ge = icmp uge i64 %1, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %load.struct1 = load %_Z6VectorI14JsonWriteFrameE, ptr %0, align 8
  %data = extractvalue %_Z6VectorI14JsonWriteFrameE %load.struct1, 1
  %ptr.add = getelementptr inbounds %_Z14JsonWriteFrame, ptr %data, i64 %1
  ret ptr %ptr.add
}

define linkonce_odr void @_ZN6VectorI14JsonWriteFrameB4jsonE3putEm14JsonWriteFrameB4json(ptr noalias %0, i64 %1, ptr %2) {
entry:
  %load.struct = load %_Z6VectorI14JsonWriteFrameE, ptr %0, align 8
  %length = extractvalue %_Z6VectorI14JsonWriteFrameE %load.struct, 0
  %ge = icmp uge i64 %1, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %field.inplace = getelementptr inbounds nuw %_Z6VectorI14JsonWriteFrameE, ptr %0, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  call void @_Z15scaly_panic_oobP10const_charmm(ptr @.str.64, i64 %1, i64 %field.val)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %load.struct1 = load %_Z6VectorI14JsonWriteFrameE, ptr %0, align 8
  %data = extractvalue %_Z6VectorI14JsonWriteFrameE %load.struct1, 1
  %ptr.add = getelementptr inbounds %_Z14JsonWriteFrame, ptr %data, i64 %1
  %store.load = load %_Z14JsonWriteFrame, ptr %2, align 8
  store %_Z14JsonWriteFrame %store.load, ptr %ptr.add, align 8
  ret void
}

define linkonce_odr ptr @_ZN14VectorIteratorI14JsonWriteFrameB4jsonE4nextEv(ptr %0) {
entry:
  %load.struct = load %_Z14VectorIteratorI14JsonWriteFrameE, ptr %0, align 8
  %vector = extractvalue %_Z14VectorIteratorI14JsonWriteFrameE %load.struct, 0
  %eq = icmp eq ptr %vector, null
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %load.struct1 = load %_Z14VectorIteratorI14JsonWriteFrameE, ptr %0, align 8
  %position = extractvalue %_Z14VectorIteratorI14JsonWriteFrameE %load.struct1, 1
  %load.struct2 = load %_Z14VectorIteratorI14JsonWriteFrameE, ptr %0, align 8
  %vector3 = extractvalue %_Z14VectorIteratorI14JsonWriteFrameE %load.struct2, 0
  %deref = load %_Z6VectorI14JsonWriteFrameE, ptr %vector3, align 8
  %length = extractvalue %_Z6VectorI14JsonWriteFrameE %deref, 0
  %eq4 = icmp eq i64 %position, %length
  br i1 %eq4, label %if.then5, label %if.end6

if.then5:                                         ; preds = %if.end
  ret ptr null

if.end6:                                          ; preds = %if.end
  %load.struct7 = load %_Z14VectorIteratorI14JsonWriteFrameE, ptr %0, align 8
  %position8 = extractvalue %_Z14VectorIteratorI14JsonWriteFrameE %load.struct7, 1
  %add = add i64 %position8, 1
  %position9 = getelementptr inbounds nuw %_Z14VectorIteratorI14JsonWriteFrameE, ptr %0, i32 0, i32 1
  store i64 %add, ptr %position9, align 8
  %field.inplace = getelementptr inbounds nuw %_Z14VectorIteratorI14JsonWriteFrameE, ptr %0, i32 0, i32 0
  %deref.recv = load ptr, ptr %field.inplace, align 8
  %load.struct10 = load %_Z14VectorIteratorI14JsonWriteFrameE, ptr %0, align 8
  %position11 = extractvalue %_Z14VectorIteratorI14JsonWriteFrameE %load.struct10, 1
  %sub = sub i64 %position11, 1
  %call = call ptr @_ZN6VectorI14JsonWriteFrameB4jsonE7get_ptrEm(ptr %deref.recv, i64 %sub)
  ret ptr %call
}

define linkonce_odr void @_ZN14VectorIteratorI14JsonWriteFrameB4jsonEC1E6OptionIR6VectorI14JsonWriteFrameB4jsonEE(ptr %0, ptr %1) {
entry:
  %vector = getelementptr inbounds nuw %_Z14VectorIteratorI14JsonWriteFrameE, ptr %0, i32 0, i32 0
  store ptr %1, ptr %vector, align 8
  %position = getelementptr inbounds nuw %_Z14VectorIteratorI14JsonWriteFrameE, ptr %0, i32 0, i32 1
  store i64 0, ptr %position, align 8
  ret void
}

define linkonce_odr void @_ZN6VectorI14JsonWriteFrameB4jsonE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z14VectorIteratorI14JsonWriteFrameE) %0, ptr %1, ptr noalias %2) {
entry:
  %struct.init = alloca %_Z14VectorIteratorI14JsonWriteFrameE, align 8
  call void @_ZN14VectorIteratorI14JsonWriteFrameB4jsonEC1E6OptionIR6VectorI14JsonWriteFrameB4jsonEE(ptr %struct.init, ptr %2)
  %sret.body = load %_Z14VectorIteratorI14JsonWriteFrameE, ptr %struct.init, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %struct.init, i64 ptrtoint (ptr getelementptr (%_Z14VectorIteratorI14JsonWriteFrameE, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr ptr @_ZN5SliceI14JsonWriteFrameB4jsonE3getEm(ptr %0, i64 %1) {
entry:
  %load.struct = load %_Z5SliceI14JsonWriteFrameE, ptr %0, align 8
  %length = extractvalue %_Z5SliceI14JsonWriteFrameE %load.struct, 0
  %ge = icmp uge i64 %1, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %load.struct1 = load %_Z5SliceI14JsonWriteFrameE, ptr %0, align 8
  %data = extractvalue %_Z5SliceI14JsonWriteFrameE %load.struct1, 1
  %ptr.add = getelementptr inbounds %_Z14JsonWriteFrame, ptr %data, i64 %1
  ret ptr %ptr.add
}

define linkonce_odr ptr @_ZN5SliceI14JsonWriteFrameB4jsonE2atEm(ptr %0, i64 %1) {
entry:
  %load.struct = load %_Z5SliceI14JsonWriteFrameE, ptr %0, align 8
  %length = extractvalue %_Z5SliceI14JsonWriteFrameE %load.struct, 0
  %ge = icmp uge i64 %1, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %field.inplace = getelementptr inbounds nuw %_Z5SliceI14JsonWriteFrameE, ptr %0, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  call void @_Z15scaly_panic_oobP10const_charmm(ptr @.str.65, i64 %1, i64 %field.val)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %load.struct1 = load %_Z5SliceI14JsonWriteFrameE, ptr %0, align 8
  %data = extractvalue %_Z5SliceI14JsonWriteFrameE %load.struct1, 1
  %ptr.add = getelementptr inbounds %_Z14JsonWriteFrame, ptr %data, i64 %1
  ret ptr %ptr.add
}

define linkonce_odr void @_ZN5SliceI14JsonWriteFrameB4jsonE3putEm14JsonWriteFrameB4json(ptr %0, i64 %1, ptr %2) {
entry:
  %load.struct = load %_Z5SliceI14JsonWriteFrameE, ptr %0, align 8
  %length = extractvalue %_Z5SliceI14JsonWriteFrameE %load.struct, 0
  %ge = icmp uge i64 %1, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %field.inplace = getelementptr inbounds nuw %_Z5SliceI14JsonWriteFrameE, ptr %0, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  call void @_Z15scaly_panic_oobP10const_charmm(ptr @.str.66, i64 %1, i64 %field.val)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %load.struct1 = load %_Z5SliceI14JsonWriteFrameE, ptr %0, align 8
  %data = extractvalue %_Z5SliceI14JsonWriteFrameE %load.struct1, 1
  %ptr.add = getelementptr inbounds %_Z14JsonWriteFrame, ptr %data, i64 %1
  %store.load = load %_Z14JsonWriteFrame, ptr %2, align 8
  store %_Z14JsonWriteFrame %store.load, ptr %ptr.add, align 8
  ret void
}

define linkonce_odr i1 @_ZN5SliceI14JsonWriteFrameB4jsonE8is_emptyEv(ptr %0) {
entry:
  %load.struct = load %_Z5SliceI14JsonWriteFrameE, ptr %0, align 8
  %length = extractvalue %_Z5SliceI14JsonWriteFrameE %load.struct, 0
  %eq = icmp eq i64 %length, 0
  ret i1 %eq
}

define linkonce_odr void @_ZN5SliceI14JsonWriteFrameB4jsonE8subsliceEmm(ptr noalias sret(%_Z5SliceI14JsonWriteFrameE) %0, ptr %1, i64 %2, i64 %3) {
entry:
  %tuple = alloca %_Z5SliceI14JsonWriteFrameE, align 8
  %from = alloca i64, align 8
  store i64 %2, ptr %from, align 1
  %to = alloca i64, align 8
  store i64 %3, ptr %to, align 1
  %from1 = load i64, ptr %from, align 8
  %load.struct = load %_Z5SliceI14JsonWriteFrameE, ptr %1, align 8
  %length = extractvalue %_Z5SliceI14JsonWriteFrameE %load.struct, 0
  %gt = icmp ugt i64 %from1, %length
  br i1 %gt, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %load.struct2 = load %_Z5SliceI14JsonWriteFrameE, ptr %1, align 8
  %length3 = extractvalue %_Z5SliceI14JsonWriteFrameE %load.struct2, 0
  store i64 %length3, ptr %from, align 1
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %to4 = load i64, ptr %to, align 8
  %load.struct5 = load %_Z5SliceI14JsonWriteFrameE, ptr %1, align 8
  %length6 = extractvalue %_Z5SliceI14JsonWriteFrameE %load.struct5, 0
  %gt7 = icmp ugt i64 %to4, %length6
  br i1 %gt7, label %if.then8, label %if.end9

if.then8:                                         ; preds = %if.end
  %load.struct10 = load %_Z5SliceI14JsonWriteFrameE, ptr %1, align 8
  %length11 = extractvalue %_Z5SliceI14JsonWriteFrameE %load.struct10, 0
  store i64 %length11, ptr %to, align 1
  br label %if.end9

if.end9:                                          ; preds = %if.then8, %if.end
  %from12 = load i64, ptr %from, align 8
  %to13 = load i64, ptr %to, align 8
  %gt14 = icmp ugt i64 %from12, %to13
  br i1 %gt14, label %if.then15, label %if.end16

if.then15:                                        ; preds = %if.end9
  %to17 = load i64, ptr %to, align 8
  store i64 %to17, ptr %from, align 1
  br label %if.end16

if.end16:                                         ; preds = %if.then15, %if.end9
  %to18 = load i64, ptr %to, align 8
  %from19 = load i64, ptr %from, align 8
  %sub = sub i64 %to18, %from19
  %load.struct20 = load %_Z5SliceI14JsonWriteFrameE, ptr %1, align 8
  %data = extractvalue %_Z5SliceI14JsonWriteFrameE %load.struct20, 1
  %from21 = load i64, ptr %from, align 8
  %ptr.add = getelementptr inbounds %_Z14JsonWriteFrame, ptr %data, i64 %from21
  %tuple.field = getelementptr inbounds nuw %_Z5SliceI14JsonWriteFrameE, ptr %tuple, i32 0, i32 0
  store i64 %sub, ptr %tuple.field, align 1
  %tuple.field22 = getelementptr inbounds nuw %_Z5SliceI14JsonWriteFrameE, ptr %tuple, i32 0, i32 1
  store ptr %ptr.add, ptr %tuple.field22, align 1
  %tuple.val = load %_Z5SliceI14JsonWriteFrameE, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z5SliceI14JsonWriteFrameE, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN5SliceI14JsonWriteFrameB4jsonE10slice_fromEPN4scaly6memory4PageEm(ptr noalias sret(%_Z5SliceI14JsonWriteFrameE) %0, ptr %1, ptr %2, i64 %3) {
entry:
  %sret.result = alloca %_Z5SliceI14JsonWriteFrameE, align 8
  %field.inplace = getelementptr inbounds nuw %_Z5SliceI14JsonWriteFrameE, ptr %2, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  call void @_ZN5SliceI14JsonWriteFrameB4jsonE8subsliceEmm(ptr noalias sret(%_Z5SliceI14JsonWriteFrameE) %sret.result, ptr %2, i64 %3, i64 %field.val)
  %sret.body = load %_Z5SliceI14JsonWriteFrameE, ptr %sret.result, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %sret.result, i64 ptrtoint (ptr getelementptr (%_Z5SliceI14JsonWriteFrameE, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN5SliceI14JsonWriteFrameB4jsonE8slice_toEPN4scaly6memory4PageEm(ptr noalias sret(%_Z5SliceI14JsonWriteFrameE) %0, ptr %1, ptr %2, i64 %3) {
entry:
  %sret.result = alloca %_Z5SliceI14JsonWriteFrameE, align 8
  call void @_ZN5SliceI14JsonWriteFrameB4jsonE8subsliceEmm(ptr noalias sret(%_Z5SliceI14JsonWriteFrameE) %sret.result, ptr %2, i64 0, i64 %3)
  %sret.body = load %_Z5SliceI14JsonWriteFrameE, ptr %sret.result, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %sret.result, i64 ptrtoint (ptr getelementptr (%_Z5SliceI14JsonWriteFrameE, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr i1 @_ZN5SliceI14JsonWriteFrameB4jsonE6equalsE5SliceI14JsonWriteFrameB4jsonE(ptr %0, ptr %1) {
entry:
  %load.struct = load %_Z5SliceI14JsonWriteFrameE, ptr %0, align 8
  %length = extractvalue %_Z5SliceI14JsonWriteFrameE %load.struct, 0
  %load.struct1 = load %_Z5SliceI14JsonWriteFrameE, ptr %1, align 8
  %length2 = extractvalue %_Z5SliceI14JsonWriteFrameE %load.struct1, 0
  %ne = icmp ne i64 %length, %length2
  br i1 %ne, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret i1 false

if.end:                                           ; preds = %entry
  %load.struct3 = load %_Z5SliceI14JsonWriteFrameE, ptr %0, align 8
  %length4 = extractvalue %_Z5SliceI14JsonWriteFrameE %load.struct3, 0
  %eq = icmp eq i64 %length4, 0
  br i1 %eq, label %if.then5, label %if.end6

if.then5:                                         ; preds = %if.end
  ret i1 true

if.end6:                                          ; preds = %if.end
  %load.struct7 = load %_Z5SliceI14JsonWriteFrameE, ptr %0, align 8
  %data = extractvalue %_Z5SliceI14JsonWriteFrameE %load.struct7, 1
  %load.struct8 = load %_Z5SliceI14JsonWriteFrameE, ptr %1, align 8
  %data9 = extractvalue %_Z5SliceI14JsonWriteFrameE %load.struct8, 1
  %load.struct10 = load %_Z5SliceI14JsonWriteFrameE, ptr %0, align 8
  %length11 = extractvalue %_Z5SliceI14JsonWriteFrameE %load.struct10, 0
  %mul = mul i64 %length11, ptrtoint (ptr getelementptr (%_Z14JsonWriteFrame, ptr null, i32 1) to i64)
  %call = call i32 @memcmp(ptr %data, ptr %data9, i64 %mul)
  %eq12 = icmp eq i32 %call, 0
  ret i1 %eq12
}

define linkonce_odr i1 @_ZN5SliceI14JsonWriteFrameB4jsonE11starts_withE5SliceI14JsonWriteFrameB4jsonE(ptr %0, ptr %1) {
entry:
  %sret.result = alloca %_Z5SliceI14JsonWriteFrameE, align 8
  %load.struct = load %_Z5SliceI14JsonWriteFrameE, ptr %1, align 8
  %length = extractvalue %_Z5SliceI14JsonWriteFrameE %load.struct, 0
  %load.struct1 = load %_Z5SliceI14JsonWriteFrameE, ptr %0, align 8
  %length2 = extractvalue %_Z5SliceI14JsonWriteFrameE %load.struct1, 0
  %gt = icmp ugt i64 %length, %length2
  br i1 %gt, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret i1 false

if.end:                                           ; preds = %entry
  %field.inplace = getelementptr inbounds nuw %_Z5SliceI14JsonWriteFrameE, ptr %1, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  call void @_ZN5SliceI14JsonWriteFrameB4jsonE8subsliceEmm(ptr noalias sret(%_Z5SliceI14JsonWriteFrameE) %sret.result, ptr %0, i64 0, i64 %field.val)
  %call = call i1 @_ZN5SliceI14JsonWriteFrameB4jsonE6equalsE5SliceI14JsonWriteFrameB4jsonE(ptr %sret.result, ptr %1)
  ret i1 %call
}

define linkonce_odr i1 @_ZN5SliceI14JsonWriteFrameB4jsonE9ends_withE5SliceI14JsonWriteFrameB4jsonE(ptr %0, ptr %1) {
entry:
  %sret.result = alloca %_Z5SliceI14JsonWriteFrameE, align 8
  %load.struct = load %_Z5SliceI14JsonWriteFrameE, ptr %1, align 8
  %length = extractvalue %_Z5SliceI14JsonWriteFrameE %load.struct, 0
  %load.struct1 = load %_Z5SliceI14JsonWriteFrameE, ptr %0, align 8
  %length2 = extractvalue %_Z5SliceI14JsonWriteFrameE %load.struct1, 0
  %gt = icmp ugt i64 %length, %length2
  br i1 %gt, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret i1 false

if.end:                                           ; preds = %entry
  %load.struct3 = load %_Z5SliceI14JsonWriteFrameE, ptr %0, align 8
  %length4 = extractvalue %_Z5SliceI14JsonWriteFrameE %load.struct3, 0
  %load.struct5 = load %_Z5SliceI14JsonWriteFrameE, ptr %1, align 8
  %length6 = extractvalue %_Z5SliceI14JsonWriteFrameE %load.struct5, 0
  %sub = sub i64 %length4, %length6
  %field.inplace = getelementptr inbounds nuw %_Z5SliceI14JsonWriteFrameE, ptr %0, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  call void @_ZN5SliceI14JsonWriteFrameB4jsonE8subsliceEmm(ptr noalias sret(%_Z5SliceI14JsonWriteFrameE) %sret.result, ptr %0, i64 %sub, i64 %field.val)
  %call = call i1 @_ZN5SliceI14JsonWriteFrameB4jsonE6equalsE5SliceI14JsonWriteFrameB4jsonE(ptr %sret.result, ptr %1)
  ret i1 %call
}

define linkonce_odr ptr @_ZN13SliceIteratorI14JsonWriteFrameB4jsonE4nextEv(ptr %0) {
entry:
  %load.struct = load %_Z13SliceIteratorI14JsonWriteFrameE, ptr %0, align 8
  %position = extractvalue %_Z13SliceIteratorI14JsonWriteFrameE %load.struct, 1
  %load.struct1 = load %_Z13SliceIteratorI14JsonWriteFrameE, ptr %0, align 8
  %slice = extractvalue %_Z13SliceIteratorI14JsonWriteFrameE %load.struct1, 0
  %length = extractvalue %_Z5SliceI14JsonWriteFrameE %slice, 0
  %ge = icmp uge i64 %position, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %load.struct2 = load %_Z13SliceIteratorI14JsonWriteFrameE, ptr %0, align 8
  %slice3 = extractvalue %_Z13SliceIteratorI14JsonWriteFrameE %load.struct2, 0
  %data = extractvalue %_Z5SliceI14JsonWriteFrameE %slice3, 1
  %load.struct4 = load %_Z13SliceIteratorI14JsonWriteFrameE, ptr %0, align 8
  %position5 = extractvalue %_Z13SliceIteratorI14JsonWriteFrameE %load.struct4, 1
  %ptr.add = getelementptr inbounds %_Z14JsonWriteFrame, ptr %data, i64 %position5
  %load.struct6 = load %_Z13SliceIteratorI14JsonWriteFrameE, ptr %0, align 8
  %position7 = extractvalue %_Z13SliceIteratorI14JsonWriteFrameE %load.struct6, 1
  %add = add i64 %position7, 1
  %position8 = getelementptr inbounds nuw %_Z13SliceIteratorI14JsonWriteFrameE, ptr %0, i32 0, i32 1
  store i64 %add, ptr %position8, align 8
  ret ptr %ptr.add
}

define linkonce_odr void @_ZN13SliceIteratorI14JsonWriteFrameB4jsonEC1E5SliceI14JsonWriteFrameB4jsonE(ptr %0, ptr %1) {
entry:
  %slice = getelementptr inbounds nuw %_Z13SliceIteratorI14JsonWriteFrameE, ptr %0, i32 0, i32 0
  %field.load = load %_Z5SliceI14JsonWriteFrameE, ptr %1, align 8
  store %_Z5SliceI14JsonWriteFrameE %field.load, ptr %slice, align 8
  %position = getelementptr inbounds nuw %_Z13SliceIteratorI14JsonWriteFrameE, ptr %0, i32 0, i32 1
  store i64 0, ptr %position, align 8
  ret void
}

define linkonce_odr void @_ZN5SliceI14JsonWriteFrameB4jsonE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z13SliceIteratorI14JsonWriteFrameE) %0, ptr %1, ptr %2) {
entry:
  %struct.init = alloca %_Z13SliceIteratorI14JsonWriteFrameE, align 8
  call void @_ZN13SliceIteratorI14JsonWriteFrameB4jsonEC1E5SliceI14JsonWriteFrameB4jsonE(ptr %struct.init, ptr %2)
  %sret.body = load %_Z13SliceIteratorI14JsonWriteFrameE, ptr %struct.init, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %struct.init, i64 ptrtoint (ptr getelementptr (%_Z13SliceIteratorI14JsonWriteFrameE, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN5SliceI14JsonWriteFrameB4jsonEC1Ev(ptr %0) {
entry:
  %data = getelementptr inbounds nuw %_Z5SliceI14JsonWriteFrameE, ptr %0, i32 0, i32 1
  store ptr null, ptr %data, align 8
  %length = getelementptr inbounds nuw %_Z5SliceI14JsonWriteFrameE, ptr %0, i32 0, i32 0
  store i64 0, ptr %length, align 8
  ret void
}

define linkonce_odr void @_ZN6VectorI14JsonWriteFrameB4jsonE8as_sliceEv(ptr noalias sret(%_Z5SliceI14JsonWriteFrameE) %0, ptr noalias %1) {
entry:
  %load.struct = load %_Z6VectorI14JsonWriteFrameE, ptr %1, align 8
  %length = extractvalue %_Z6VectorI14JsonWriteFrameE %load.struct, 0
  %load.struct1 = load %_Z6VectorI14JsonWriteFrameE, ptr %1, align 8
  %data = extractvalue %_Z6VectorI14JsonWriteFrameE %load.struct1, 1
  %tuple = alloca %_Z5SliceI14JsonWriteFrameE, align 8
  %tuple.field = getelementptr inbounds nuw %_Z5SliceI14JsonWriteFrameE, ptr %tuple, i32 0, i32 0
  store i64 %length, ptr %tuple.field, align 1
  %tuple.field2 = getelementptr inbounds nuw %_Z5SliceI14JsonWriteFrameE, ptr %tuple, i32 0, i32 1
  store ptr %data, ptr %tuple.field2, align 1
  %tuple.val = load %_Z5SliceI14JsonWriteFrameE, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z5SliceI14JsonWriteFrameE, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN6VectorI14JsonWriteFrameB4jsonEC1Ev(ptr noalias %0) {
entry:
  %length = getelementptr inbounds nuw %_Z6VectorI14JsonWriteFrameE, ptr %0, i32 0, i32 0
  store i64 0, ptr %length, align 8
  %data = getelementptr inbounds nuw %_Z6VectorI14JsonWriteFrameE, ptr %0, i32 0, i32 1
  store ptr null, ptr %data, align 8
  ret void
}

define linkonce_odr void @_ZN6VectorI14JsonWriteFrameB4jsonEC1Em(ptr %0, i64 %1) {
entry:
  %call = call ptr @_ZN4Page3getEPv(ptr %0)
  %length = getelementptr inbounds nuw %_Z6VectorI14JsonWriteFrameE, ptr %0, i32 0, i32 0
  store i64 %1, ptr %length, align 8
  %gt = icmp ugt i64 %1, 0
  br i1 %gt, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %mul = mul i64 %1, ptrtoint (ptr getelementptr (%_Z14JsonWriteFrame, ptr null, i32 1) to i64)
  %call1 = call ptr @_ZN4Page8allocateEmm(ptr %call, i64 %mul, i64 8)
  %data = getelementptr inbounds nuw %_Z6VectorI14JsonWriteFrameE, ptr %0, i32 0, i32 1
  store ptr %call1, ptr %data, align 8
  %field.inplace = getelementptr inbounds nuw %_Z6VectorI14JsonWriteFrameE, ptr %0, i32 0, i32 1
  %deref.recv = load ptr, ptr %field.inplace, align 8
  %mul2 = mul i64 %1, ptrtoint (ptr getelementptr (%_Z14JsonWriteFrame, ptr null, i32 1) to i64)
  %call3 = call ptr @memset(ptr %deref.recv, i32 0, i64 %mul2)
  br label %if.end

if.else:                                          ; preds = %entry
  %data4 = getelementptr inbounds nuw %_Z6VectorI14JsonWriteFrameE, ptr %0, i32 0, i32 1
  store ptr null, ptr %data4, align 8
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  %if.value = phi ptr [ %call3, %if.then ], [ null, %if.else ]
  ret void
}

define linkonce_odr void @_ZN6VectorI14JsonWriteFrameB4jsonEC1E6VectorI14JsonWriteFrameB4jsonE(ptr %0, ptr %1) {
entry:
  %call = call ptr @_ZN4Page3getEPv(ptr %0)
  %load.struct = load %_Z6VectorI14JsonWriteFrameE, ptr %1, align 8
  %length = extractvalue %_Z6VectorI14JsonWriteFrameE %load.struct, 0
  %length1 = getelementptr inbounds nuw %_Z6VectorI14JsonWriteFrameE, ptr %0, i32 0, i32 0
  store i64 %length, ptr %length1, align 8
  %load.struct2 = load %_Z6VectorI14JsonWriteFrameE, ptr %0, align 8
  %length3 = extractvalue %_Z6VectorI14JsonWriteFrameE %load.struct2, 0
  %gt = icmp ugt i64 %length3, 0
  br i1 %gt, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %load.struct4 = load %_Z6VectorI14JsonWriteFrameE, ptr %0, align 8
  %length5 = extractvalue %_Z6VectorI14JsonWriteFrameE %load.struct4, 0
  %mul = mul i64 %length5, ptrtoint (ptr getelementptr (%_Z14JsonWriteFrame, ptr null, i32 1) to i64)
  %call6 = call ptr @_ZN4Page8allocateEmm(ptr %call, i64 %mul, i64 8)
  %data = getelementptr inbounds nuw %_Z6VectorI14JsonWriteFrameE, ptr %0, i32 0, i32 1
  store ptr %call6, ptr %data, align 8
  %field.inplace = getelementptr inbounds nuw %_Z6VectorI14JsonWriteFrameE, ptr %0, i32 0, i32 1
  %deref.recv = load ptr, ptr %field.inplace, align 8
  %field.inplace7 = getelementptr inbounds nuw %_Z6VectorI14JsonWriteFrameE, ptr %1, i32 0, i32 1
  %deref.recv8 = load ptr, ptr %field.inplace7, align 8
  %load.struct9 = load %_Z6VectorI14JsonWriteFrameE, ptr %0, align 8
  %length10 = extractvalue %_Z6VectorI14JsonWriteFrameE %load.struct9, 0
  %mul11 = mul i64 %length10, ptrtoint (ptr getelementptr (%_Z14JsonWriteFrame, ptr null, i32 1) to i64)
  %call12 = call ptr @memcpy(ptr %deref.recv, ptr %deref.recv8, i64 %mul11)
  br label %if.end

if.else:                                          ; preds = %entry
  %data13 = getelementptr inbounds nuw %_Z6VectorI14JsonWriteFrameE, ptr %0, i32 0, i32 1
  store ptr null, ptr %data13, align 8
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  %if.value = phi ptr [ %call12, %if.then ], [ null, %if.else ]
  ret void
}

define linkonce_odr void @_ZN6VectorI14JsonWriteFrameB4jsonEC1E5ArrayI14JsonWriteFrameB4jsonE(ptr %0, ptr %1) {
entry:
  %call = call ptr @_ZN4Page3getEPv(ptr %0)
  %load.struct = load %_Z5ArrayI14JsonWriteFrameE, ptr %1, align 8
  %length = extractvalue %_Z5ArrayI14JsonWriteFrameE %load.struct, 0
  %length1 = getelementptr inbounds nuw %_Z6VectorI14JsonWriteFrameE, ptr %0, i32 0, i32 0
  store i64 %length, ptr %length1, align 8
  %load.struct2 = load %_Z6VectorI14JsonWriteFrameE, ptr %0, align 8
  %length3 = extractvalue %_Z6VectorI14JsonWriteFrameE %load.struct2, 0
  %gt = icmp ugt i64 %length3, 0
  br i1 %gt, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %load.struct4 = load %_Z6VectorI14JsonWriteFrameE, ptr %0, align 8
  %length5 = extractvalue %_Z6VectorI14JsonWriteFrameE %load.struct4, 0
  %mul = mul i64 %length5, ptrtoint (ptr getelementptr (%_Z14JsonWriteFrame, ptr null, i32 1) to i64)
  %call6 = call ptr @_ZN4Page8allocateEmm(ptr %call, i64 %mul, i64 8)
  %data = getelementptr inbounds nuw %_Z6VectorI14JsonWriteFrameE, ptr %0, i32 0, i32 1
  store ptr %call6, ptr %data, align 8
  %field.inplace = getelementptr inbounds nuw %_Z6VectorI14JsonWriteFrameE, ptr %0, i32 0, i32 1
  %deref.recv = load ptr, ptr %field.inplace, align 8
  %call7 = call ptr @_ZN5ArrayI14JsonWriteFrameB4jsonE10get_bufferEv(ptr %1)
  %load.struct8 = load %_Z6VectorI14JsonWriteFrameE, ptr %0, align 8
  %length9 = extractvalue %_Z6VectorI14JsonWriteFrameE %load.struct8, 0
  %mul10 = mul i64 %length9, ptrtoint (ptr getelementptr (%_Z14JsonWriteFrame, ptr null, i32 1) to i64)
  %call11 = call ptr @memcpy(ptr %deref.recv, ptr %call7, i64 %mul10)
  br label %if.end

if.else:                                          ; preds = %entry
  %data12 = getelementptr inbounds nuw %_Z6VectorI14JsonWriteFrameE, ptr %0, i32 0, i32 1
  store ptr null, ptr %data12, align 8
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  %if.value = phi ptr [ %call11, %if.then ], [ null, %if.else ]
  ret void
}

define linkonce_odr ptr @_ZN4ListI14JsonWriteFrameB4jsonE8get_headEv(ptr %0) {
entry:
  %load.struct = load %_Z4ListI14JsonWriteFrameE, ptr %0, align 8
  %head = extractvalue %_Z4ListI14JsonWriteFrameE %load.struct, 0
  %eq = icmp eq ptr %head, null
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %addr.gep = getelementptr inbounds nuw %_Z4ListI14JsonWriteFrameE, ptr %0, i32 0, i32 0
  %addr.hop = load ptr, ptr %addr.gep, align 8
  %addr.gep1 = getelementptr inbounds nuw %_Z4NodeI14JsonWriteFrameE, ptr %addr.hop, i32 0, i32 0
  ret ptr %addr.gep1
}

define linkonce_odr ptr @_ZN12ListIteratorI14JsonWriteFrameB4jsonE4nextEv(ptr %0) {
entry:
  %old_current = alloca ptr, align 8
  %load.struct = load %_Z12ListIteratorI14JsonWriteFrameE, ptr %0, align 8
  %current = extractvalue %_Z12ListIteratorI14JsonWriteFrameE %load.struct, 0
  %ne = icmp ne ptr %current, null
  br i1 %ne, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %load.struct1 = load %_Z12ListIteratorI14JsonWriteFrameE, ptr %0, align 8
  %current2 = extractvalue %_Z12ListIteratorI14JsonWriteFrameE %load.struct1, 0
  store ptr %current2, ptr %old_current, align 1
  %load.struct3 = load %_Z12ListIteratorI14JsonWriteFrameE, ptr %0, align 8
  %current4 = extractvalue %_Z12ListIteratorI14JsonWriteFrameE %load.struct3, 0
  %deref = load %_Z4NodeI14JsonWriteFrameE, ptr %current4, align 8
  %next = extractvalue %_Z4NodeI14JsonWriteFrameE %deref, 1
  %current5 = getelementptr inbounds nuw %_Z12ListIteratorI14JsonWriteFrameE, ptr %0, i32 0, i32 0
  store ptr %next, ptr %current5, align 8
  %old_current6 = load ptr, ptr %old_current, align 8
  %addr.gep = getelementptr inbounds nuw %_Z4NodeI14JsonWriteFrameE, ptr %old_current6, i32 0, i32 0
  ret ptr %addr.gep

if.else:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; No predecessors!
  ret ptr null
}

define linkonce_odr i64 @_ZN4ListI14JsonWriteFrameB4jsonE5countEv(ptr %0) {
entry:
  %sret.result = alloca %_Z12ListIteratorI14JsonWriteFrameE, align 8
  call void @_ZN4ListI14JsonWriteFrameB4jsonE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z12ListIteratorI14JsonWriteFrameE) %sret.result, ptr null, ptr %0)
  %list_iterator = alloca ptr, align 8
  store ptr %sret.result, ptr %list_iterator, align 1
  %i = alloca i64, align 8
  store i64 0, ptr %i, align 1
  br label %while.cond

while.cond:                                       ; preds = %while.body, %entry
  %list_iterator1 = load ptr, ptr %list_iterator, align 8
  %call = call ptr @_ZN12ListIteratorI14JsonWriteFrameB4jsonE4nextEv(ptr %list_iterator1)
  %ne = icmp ne ptr %call, null
  br i1 %ne, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %i2 = load i64, ptr %i, align 8
  %add = add i64 %i2, 1
  store i64 %add, ptr %i, align 1
  br label %while.cond

while.exit:                                       ; preds = %while.cond
  %i3 = load i64, ptr %i, align 8
  ret i64 %i3
}

define linkonce_odr void @_ZN4ListI14JsonWriteFrameB4jsonE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z12ListIteratorI14JsonWriteFrameE) %0, ptr %1, ptr %2) {
entry:
  %load.struct = load %_Z4ListI14JsonWriteFrameE, ptr %2, align 8
  %head = extractvalue %_Z4ListI14JsonWriteFrameE %load.struct, 0
  %tuple = alloca %_Z12ListIteratorI14JsonWriteFrameE, align 8
  %tuple.field = getelementptr inbounds nuw %_Z12ListIteratorI14JsonWriteFrameE, ptr %tuple, i32 0, i32 0
  store ptr %head, ptr %tuple.field, align 1
  %tuple.val = load %_Z12ListIteratorI14JsonWriteFrameE, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z12ListIteratorI14JsonWriteFrameE, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN4ListI14JsonWriteFrameB4jsonE4linkEP4NodeI14JsonWriteFrameB4jsonE(ptr %0, ptr %1) {
entry:
  %load.struct = load %_Z4ListI14JsonWriteFrameE, ptr %0, align 8
  %tail = extractvalue %_Z4ListI14JsonWriteFrameE %load.struct, 1
  %eq = icmp eq ptr %tail, null
  br i1 %eq, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %head = getelementptr inbounds nuw %_Z4ListI14JsonWriteFrameE, ptr %0, i32 0, i32 0
  store ptr %1, ptr %head, align 8
  br label %if.end

if.else:                                          ; preds = %entry
  %tail1 = getelementptr inbounds nuw %_Z4ListI14JsonWriteFrameE, ptr %0, i32 0, i32 1
  %field.deref = load ptr, ptr %tail1, align 8
  %next = getelementptr inbounds nuw %_Z4NodeI14JsonWriteFrameE, ptr %field.deref, i32 0, i32 1
  store ptr %1, ptr %next, align 8
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  %if.value = phi ptr [ %1, %if.then ], [ %1, %if.else ]
  %tail2 = getelementptr inbounds nuw %_Z4ListI14JsonWriteFrameE, ptr %0, i32 0, i32 1
  store ptr %1, ptr %tail2, align 8
  ret void
}

define linkonce_odr void @_ZN4ListI14JsonWriteFrameB4jsonE3addE14JsonWriteFrameB4json(ptr %0, ptr %1) {
entry:
  %own_page = call ptr @_Z3getPv(ptr %0)
  %tuple.region = call ptr @_ZN4Page8allocateEmm(ptr %own_page, i64 ptrtoint (ptr getelementptr (%_Z4NodeI14JsonWriteFrameE, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr ({ i1, %_Z4NodeI14JsonWriteFrameE }, ptr null, i64 0, i32 1) to i64))
  %field.load = load %_Z14JsonWriteFrame, ptr %1, align 8
  %tuple.field = getelementptr inbounds nuw %_Z4NodeI14JsonWriteFrameE, ptr %tuple.region, i32 0, i32 0
  store %_Z14JsonWriteFrame %field.load, ptr %tuple.field, align 1
  %tuple.field1 = getelementptr inbounds nuw %_Z4NodeI14JsonWriteFrameE, ptr %tuple.region, i32 0, i32 1
  store ptr null, ptr %tuple.field1, align 1
  call void @_ZN4ListI14JsonWriteFrameB4jsonE4linkEP4NodeI14JsonWriteFrameB4jsonE(ptr %0, ptr %tuple.region)
  ret void
}

define linkonce_odr void @_ZN4ListI14JsonWriteFrameB4jsonE6add_onER4Page14JsonWriteFrameB4json(ptr %0, ptr %1, ptr %2) {
entry:
  %tuple.region = call ptr @_ZN4Page8allocateEmm(ptr %1, i64 ptrtoint (ptr getelementptr (%_Z4NodeI14JsonWriteFrameE, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr ({ i1, %_Z4NodeI14JsonWriteFrameE }, ptr null, i64 0, i32 1) to i64))
  %field.load = load %_Z14JsonWriteFrame, ptr %2, align 8
  %tuple.field = getelementptr inbounds nuw %_Z4NodeI14JsonWriteFrameE, ptr %tuple.region, i32 0, i32 0
  store %_Z14JsonWriteFrame %field.load, ptr %tuple.field, align 1
  %tuple.field1 = getelementptr inbounds nuw %_Z4NodeI14JsonWriteFrameE, ptr %tuple.region, i32 0, i32 1
  store ptr null, ptr %tuple.field1, align 1
  call void @_ZN4ListI14JsonWriteFrameB4jsonE4linkEP4NodeI14JsonWriteFrameB4jsonE(ptr %0, ptr %tuple.region)
  ret void
}

define linkonce_odr void @_ZN6VectorI14JsonWriteFrameB4jsonEC1E4ListI14JsonWriteFrameB4jsonE(ptr %0, ptr %1) {
entry:
  %deref.tmp = alloca %_Z14JsonWriteFrame, align 8
  %i = alloca i64, align 8
  %list_iterator = alloca ptr, align 8
  %sret.result = alloca %_Z12ListIteratorI14JsonWriteFrameE, align 8
  %call = call ptr @_ZN4Page3getEPv(ptr %0)
  %call1 = call i64 @_ZN4ListI14JsonWriteFrameB4jsonE5countEv(ptr %1)
  %length = getelementptr inbounds nuw %_Z6VectorI14JsonWriteFrameE, ptr %0, i32 0, i32 0
  store i64 %call1, ptr %length, align 8
  %load.struct = load %_Z6VectorI14JsonWriteFrameE, ptr %0, align 8
  %length2 = extractvalue %_Z6VectorI14JsonWriteFrameE %load.struct, 0
  %gt = icmp ugt i64 %length2, 0
  br i1 %gt, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %load.struct3 = load %_Z6VectorI14JsonWriteFrameE, ptr %0, align 8
  %length4 = extractvalue %_Z6VectorI14JsonWriteFrameE %load.struct3, 0
  %mul = mul i64 %length4, ptrtoint (ptr getelementptr (%_Z14JsonWriteFrame, ptr null, i32 1) to i64)
  %call5 = call ptr @_ZN4Page8allocateEmm(ptr %call, i64 %mul, i64 8)
  %data = getelementptr inbounds nuw %_Z6VectorI14JsonWriteFrameE, ptr %0, i32 0, i32 1
  store ptr %call5, ptr %data, align 8
  call void @_ZN4ListI14JsonWriteFrameB4jsonE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z12ListIteratorI14JsonWriteFrameE) %sret.result, ptr null, ptr %1)
  store ptr %sret.result, ptr %list_iterator, align 1
  store i64 0, ptr %i, align 1
  br label %while.cond

if.else:                                          ; preds = %entry
  %data12 = getelementptr inbounds nuw %_Z6VectorI14JsonWriteFrameE, ptr %0, i32 0, i32 1
  store ptr null, ptr %data12, align 8
  br label %if.end

if.end:                                           ; preds = %if.else, %while.exit
  ret void

while.cond:                                       ; preds = %while.body, %if.then
  %list_iterator6 = load ptr, ptr %list_iterator, align 8
  %call7 = call ptr @_ZN12ListIteratorI14JsonWriteFrameB4jsonE4nextEv(ptr %list_iterator6)
  %while.tobool = icmp ne ptr %call7, null
  br i1 %while.tobool, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %deref.tmp, ptr align 1 %call7, i64 ptrtoint (ptr getelementptr (%_Z14JsonWriteFrame, ptr null, i32 1) to i64), i1 false)
  %load.struct8 = load %_Z6VectorI14JsonWriteFrameE, ptr %0, align 8
  %data9 = extractvalue %_Z6VectorI14JsonWriteFrameE %load.struct8, 1
  %i10 = load i64, ptr %i, align 8
  %ptr.add = getelementptr inbounds %_Z14JsonWriteFrame, ptr %data9, i64 %i10
  %store.load = load %_Z14JsonWriteFrame, ptr %deref.tmp, align 8
  store %_Z14JsonWriteFrame %store.load, ptr %ptr.add, align 8
  %i11 = load i64, ptr %i, align 8
  %add = add i64 %i11, 1
  store i64 %add, ptr %i, align 1
  br label %while.cond

while.exit:                                       ; preds = %while.cond
  br label %if.end
}

define linkonce_odr void @_ZN5ArrayI14JsonWriteFrameB4jsonE7add_runEmP14JsonWriteFrameB4json(ptr %0, i64 %1, ptr %2) {
entry:
  %load.struct = load %_Z5ArrayI14JsonWriteFrameE, ptr %0, align 8
  %length = extractvalue %_Z5ArrayI14JsonWriteFrameE %load.struct, 0
  %add = add i64 %length, %1
  %new_length = alloca i64, align 8
  store i64 %add, ptr %new_length, align 1
  %new_length1 = load i64, ptr %new_length, align 8
  %load.struct2 = load %_Z5ArrayI14JsonWriteFrameE, ptr %0, align 8
  %length3 = extractvalue %_Z5ArrayI14JsonWriteFrameE %load.struct2, 0
  %lt = icmp ult i64 %new_length1, %length3
  br i1 %lt, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %field.inplace = getelementptr inbounds nuw %_Z5ArrayI14JsonWriteFrameE, ptr %0, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  call void @_Z16scaly_panic_sizeP10const_charmm(ptr @.str.67, i64 %1, i64 %field.val)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %load.struct6 = load %_Z5ArrayI14JsonWriteFrameE, ptr %0, align 8
  %buffer = extractvalue %_Z5ArrayI14JsonWriteFrameE %load.struct6, 2
  %eq = icmp eq ptr %buffer, null
  br i1 %eq, label %if.then4, label %lor.rhs

if.then4:                                         ; preds = %lor.rhs, %if.end
  %new_length9 = load i64, ptr %new_length, align 8
  call void @_ZN5ArrayI14JsonWriteFrameB4jsonE7grow_toEm(ptr %0, i64 %new_length9)
  br label %if.end5

if.end5:                                          ; preds = %if.then4, %lor.rhs
  %gt10 = icmp ugt i64 %1, 0
  br i1 %gt10, label %if.then11, label %if.end12

lor.rhs:                                          ; preds = %if.end
  %new_length7 = load i64, ptr %new_length, align 8
  %load.struct8 = load %_Z5ArrayI14JsonWriteFrameE, ptr %0, align 8
  %capacity = extractvalue %_Z5ArrayI14JsonWriteFrameE %load.struct8, 1
  %gt = icmp ugt i64 %new_length7, %capacity
  br i1 %gt, label %if.then4, label %if.end5

if.then11:                                        ; preds = %if.end5
  %load.struct13 = load %_Z5ArrayI14JsonWriteFrameE, ptr %0, align 8
  %buffer14 = extractvalue %_Z5ArrayI14JsonWriteFrameE %load.struct13, 2
  %load.struct15 = load %_Z5ArrayI14JsonWriteFrameE, ptr %0, align 8
  %length16 = extractvalue %_Z5ArrayI14JsonWriteFrameE %load.struct15, 0
  %ptr.add = getelementptr inbounds %_Z14JsonWriteFrame, ptr %buffer14, i64 %length16
  %mul = mul i64 %1, ptrtoint (ptr getelementptr (%_Z14JsonWriteFrame, ptr null, i32 1) to i64)
  %call = call ptr @memcpy(ptr %ptr.add, ptr %2, i64 %mul)
  br label %if.end12

if.end12:                                         ; preds = %if.then11, %if.end5
  %load.struct17 = load %_Z5ArrayI14JsonWriteFrameE, ptr %0, align 8
  %length18 = extractvalue %_Z5ArrayI14JsonWriteFrameE %load.struct17, 0
  %add19 = add i64 %length18, %1
  %length20 = getelementptr inbounds nuw %_Z5ArrayI14JsonWriteFrameE, ptr %0, i32 0, i32 0
  store i64 %add19, ptr %length20, align 8
  ret void
}

define linkonce_odr void @_ZN5ArrayI14JsonWriteFrameB4jsonE3addE6VectorI14JsonWriteFrameB4jsonE(ptr %0, ptr %1) {
entry:
  %field.inplace = getelementptr inbounds nuw %_Z6VectorI14JsonWriteFrameE, ptr %1, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  %field.inplace1 = getelementptr inbounds nuw %_Z6VectorI14JsonWriteFrameE, ptr %1, i32 0, i32 1
  %deref.recv = load ptr, ptr %field.inplace1, align 8
  call void @_ZN5ArrayI14JsonWriteFrameB4jsonE7add_runEmP14JsonWriteFrameB4json(ptr %0, i64 %field.val, ptr %deref.recv)
  ret void
}

define linkonce_odr void @_ZN5ArrayI14JsonWriteFrameB4jsonE3addE5SliceI14JsonWriteFrameB4jsonE(ptr %0, ptr %1) {
entry:
  %field.inplace = getelementptr inbounds nuw %_Z5SliceI14JsonWriteFrameE, ptr %1, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  %field.inplace1 = getelementptr inbounds nuw %_Z5SliceI14JsonWriteFrameE, ptr %1, i32 0, i32 1
  %deref.recv = load ptr, ptr %field.inplace1, align 8
  call void @_ZN5ArrayI14JsonWriteFrameB4jsonE7add_runEmP14JsonWriteFrameB4json(ptr %0, i64 %field.val, ptr %deref.recv)
  ret void
}

define linkonce_odr void @_ZN5ArrayI14JsonWriteFrameB4jsonE7grow_toEm(ptr %0, i64 %1) {
entry:
  call void @_ZN5ArrayI14JsonWriteFrameB4jsonE10reallocateEv(ptr %0)
  %load.struct = load %_Z5ArrayI14JsonWriteFrameE, ptr %0, align 8
  %capacity = extractvalue %_Z5ArrayI14JsonWriteFrameE %load.struct, 1
  %gt = icmp ugt i64 %1, %capacity
  br i1 %gt, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %call = call ptr @_ZN4Page3getEPv(ptr %0)
  %load.struct1 = load %_Z5ArrayI14JsonWriteFrameE, ptr %0, align 8
  %buffer = extractvalue %_Z5ArrayI14JsonWriteFrameE %load.struct1, 2
  %load.struct2 = load %_Z5ArrayI14JsonWriteFrameE, ptr %0, align 8
  %capacity3 = extractvalue %_Z5ArrayI14JsonWriteFrameE %load.struct2, 1
  %mul = mul i64 %capacity3, ptrtoint (ptr getelementptr (%_Z14JsonWriteFrame, ptr null, i32 1) to i64)
  %le = icmp ule i64 %mul, 1024
  %mul4 = mul i64 %1, ptrtoint (ptr getelementptr (%_Z14JsonWriteFrame, ptr null, i32 1) to i64)
  %le5 = icmp ule i64 %mul4, 1024
  br i1 %le5, label %if.then6, label %if.else

if.end:                                           ; preds = %if.end24, %entry
  ret void

if.then6:                                         ; preds = %if.then
  %mul8 = mul i64 %1, ptrtoint (ptr getelementptr (%_Z14JsonWriteFrame, ptr null, i32 1) to i64)
  %call9 = call ptr @_ZN4Page8allocateEmm(ptr %call, i64 %mul8, i64 8)
  %buffer10 = getelementptr inbounds nuw %_Z5ArrayI14JsonWriteFrameE, ptr %0, i32 0, i32 2
  store ptr %call9, ptr %buffer10, align 8
  br label %if.end7

if.else:                                          ; preds = %if.then
  %mul11 = mul i64 %1, ptrtoint (ptr getelementptr (%_Z14JsonWriteFrame, ptr null, i32 1) to i64)
  %call12 = call ptr @_ZN4Page25allocate_exclusive_bufferEmm(ptr %call, i64 %mul11, i64 8)
  %buffer13 = getelementptr inbounds nuw %_Z5ArrayI14JsonWriteFrameE, ptr %0, i32 0, i32 2
  store ptr %call12, ptr %buffer13, align 8
  br label %if.end7

if.end7:                                          ; preds = %if.else, %if.then6
  %if.value = phi ptr [ %call9, %if.then6 ], [ %call12, %if.else ]
  %capacity14 = getelementptr inbounds nuw %_Z5ArrayI14JsonWriteFrameE, ptr %0, i32 0, i32 1
  store i64 %1, ptr %capacity14, align 8
  %load.struct15 = load %_Z5ArrayI14JsonWriteFrameE, ptr %0, align 8
  %length = extractvalue %_Z5ArrayI14JsonWriteFrameE %load.struct15, 0
  %gt16 = icmp ugt i64 %length, 0
  br i1 %gt16, label %if.then17, label %if.end18

if.then17:                                        ; preds = %if.end7
  %field.inplace = getelementptr inbounds nuw %_Z5ArrayI14JsonWriteFrameE, ptr %0, i32 0, i32 2
  %deref.recv = load ptr, ptr %field.inplace, align 8
  %load.struct19 = load %_Z5ArrayI14JsonWriteFrameE, ptr %0, align 8
  %length20 = extractvalue %_Z5ArrayI14JsonWriteFrameE %load.struct19, 0
  %mul21 = mul i64 %length20, ptrtoint (ptr getelementptr (%_Z14JsonWriteFrame, ptr null, i32 1) to i64)
  %call22 = call ptr @memcpy(ptr %deref.recv, ptr %buffer, i64 %mul21)
  br label %if.end18

if.end18:                                         ; preds = %if.then17, %if.end7
  %eq = icmp eq i1 %le, false
  br i1 %eq, label %if.then23, label %if.end24

if.then23:                                        ; preds = %if.end18
  %call25 = call ptr @_ZN4Page3getEPv(ptr %buffer)
  call void @_ZN4Page25deallocate_exclusive_pageER4Page(ptr %call, ptr %call25)
  br label %if.end24

if.end24:                                         ; preds = %if.then23, %if.end18
  br label %if.end
}

define linkonce_odr void @_ZN5ArrayI14JsonWriteFrameB4jsonE6extendEm(ptr noalias sret(%_Z5SliceI14JsonWriteFrameE) %0, ptr %1, i64 %2) {
entry:
  %tuple = alloca %_Z5SliceI14JsonWriteFrameE, align 8
  %load.struct = load %_Z5ArrayI14JsonWriteFrameE, ptr %1, align 8
  %length = extractvalue %_Z5ArrayI14JsonWriteFrameE %load.struct, 0
  %add = add i64 %length, %2
  %lt = icmp ult i64 %add, %length
  br i1 %lt, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  call void @_Z16scaly_panic_sizeP10const_charmm(ptr @.str.68, i64 %2, i64 %length)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %load.struct3 = load %_Z5ArrayI14JsonWriteFrameE, ptr %1, align 8
  %buffer = extractvalue %_Z5ArrayI14JsonWriteFrameE %load.struct3, 2
  %eq = icmp eq ptr %buffer, null
  br i1 %eq, label %if.then1, label %lor.rhs

if.then1:                                         ; preds = %lor.rhs, %if.end
  call void @_ZN5ArrayI14JsonWriteFrameB4jsonE7grow_toEm(ptr %1, i64 %add)
  br label %if.end2

if.end2:                                          ; preds = %if.then1, %lor.rhs
  %length5 = getelementptr inbounds nuw %_Z5ArrayI14JsonWriteFrameE, ptr %1, i32 0, i32 0
  store i64 %add, ptr %length5, align 8
  %load.struct6 = load %_Z5ArrayI14JsonWriteFrameE, ptr %1, align 8
  %buffer7 = extractvalue %_Z5ArrayI14JsonWriteFrameE %load.struct6, 2
  %ptr.add = getelementptr inbounds %_Z14JsonWriteFrame, ptr %buffer7, i64 %length
  %tuple.field = getelementptr inbounds nuw %_Z5SliceI14JsonWriteFrameE, ptr %tuple, i32 0, i32 0
  store i64 %2, ptr %tuple.field, align 1
  %tuple.field8 = getelementptr inbounds nuw %_Z5SliceI14JsonWriteFrameE, ptr %tuple, i32 0, i32 1
  store ptr %ptr.add, ptr %tuple.field8, align 1
  %tuple.val = load %_Z5SliceI14JsonWriteFrameE, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z5SliceI14JsonWriteFrameE, ptr null, i32 1) to i64), i1 false)
  ret void

lor.rhs:                                          ; preds = %if.end
  %load.struct4 = load %_Z5ArrayI14JsonWriteFrameE, ptr %1, align 8
  %capacity = extractvalue %_Z5ArrayI14JsonWriteFrameE %load.struct4, 1
  %gt = icmp ugt i64 %add, %capacity
  br i1 %gt, label %if.then1, label %if.end2
}

define linkonce_odr ptr @_ZN5ArrayI14JsonWriteFrameB4jsonE3getEm(ptr %0, i64 %1) {
entry:
  %load.struct = load %_Z5ArrayI14JsonWriteFrameE, ptr %0, align 8
  %length = extractvalue %_Z5ArrayI14JsonWriteFrameE %load.struct, 0
  %ge = icmp uge i64 %1, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %load.struct1 = load %_Z5ArrayI14JsonWriteFrameE, ptr %0, align 8
  %buffer = extractvalue %_Z5ArrayI14JsonWriteFrameE %load.struct1, 2
  %ptr.add = getelementptr inbounds %_Z14JsonWriteFrame, ptr %buffer, i64 %1
  ret ptr %ptr.add
}

define linkonce_odr ptr @_ZN5ArrayI14JsonWriteFrameB4jsonE2atEm(ptr %0, i64 %1) {
entry:
  %load.struct = load %_Z5ArrayI14JsonWriteFrameE, ptr %0, align 8
  %length = extractvalue %_Z5ArrayI14JsonWriteFrameE %load.struct, 0
  %ge = icmp uge i64 %1, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %field.inplace = getelementptr inbounds nuw %_Z5ArrayI14JsonWriteFrameE, ptr %0, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  call void @_Z15scaly_panic_oobP10const_charmm(ptr @.str.69, i64 %1, i64 %field.val)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %load.struct1 = load %_Z5ArrayI14JsonWriteFrameE, ptr %0, align 8
  %buffer = extractvalue %_Z5ArrayI14JsonWriteFrameE %load.struct1, 2
  %ptr.add = getelementptr inbounds %_Z14JsonWriteFrame, ptr %buffer, i64 %1
  ret ptr %ptr.add
}

define linkonce_odr void @_ZN5ArrayI14JsonWriteFrameB4jsonE5clearEv(ptr %0) {
entry:
  %length = getelementptr inbounds nuw %_Z5ArrayI14JsonWriteFrameE, ptr %0, i32 0, i32 0
  store i64 0, ptr %length, align 8
  ret void
}

define linkonce_odr void @_ZN5ArrayI14JsonWriteFrameB4jsonE3putEm14JsonWriteFrameB4json(ptr %0, i64 %1, ptr %2) {
entry:
  %load.struct = load %_Z5ArrayI14JsonWriteFrameE, ptr %0, align 8
  %length = extractvalue %_Z5ArrayI14JsonWriteFrameE %load.struct, 0
  %ge = icmp uge i64 %1, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %field.inplace = getelementptr inbounds nuw %_Z5ArrayI14JsonWriteFrameE, ptr %0, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  call void @_Z15scaly_panic_oobP10const_charmm(ptr @.str.70, i64 %1, i64 %field.val)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %load.struct1 = load %_Z5ArrayI14JsonWriteFrameE, ptr %0, align 8
  %buffer = extractvalue %_Z5ArrayI14JsonWriteFrameE %load.struct1, 2
  %ptr.add = getelementptr inbounds %_Z14JsonWriteFrame, ptr %buffer, i64 %1
  %store.load = load %_Z14JsonWriteFrame, ptr %2, align 8
  store %_Z14JsonWriteFrame %store.load, ptr %ptr.add, align 8
  ret void
}

define linkonce_odr ptr @_ZN13ArrayIteratorI14JsonWriteFrameB4jsonE4nextEv(ptr %0) {
entry:
  %load.struct = load %_Z13ArrayIteratorI14JsonWriteFrameE, ptr %0, align 8
  %array = extractvalue %_Z13ArrayIteratorI14JsonWriteFrameE %load.struct, 0
  %eq = icmp eq ptr %array, null
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  ret ptr null

if.end:                                           ; preds = %entry
  %load.struct1 = load %_Z13ArrayIteratorI14JsonWriteFrameE, ptr %0, align 8
  %position = extractvalue %_Z13ArrayIteratorI14JsonWriteFrameE %load.struct1, 1
  %load.struct2 = load %_Z13ArrayIteratorI14JsonWriteFrameE, ptr %0, align 8
  %array3 = extractvalue %_Z13ArrayIteratorI14JsonWriteFrameE %load.struct2, 0
  %deref = load %_Z5ArrayI14JsonWriteFrameE, ptr %array3, align 8
  %length = extractvalue %_Z5ArrayI14JsonWriteFrameE %deref, 0
  %eq4 = icmp eq i64 %position, %length
  br i1 %eq4, label %if.then5, label %if.end6

if.then5:                                         ; preds = %if.end
  ret ptr null

if.end6:                                          ; preds = %if.end
  %load.struct7 = load %_Z13ArrayIteratorI14JsonWriteFrameE, ptr %0, align 8
  %position8 = extractvalue %_Z13ArrayIteratorI14JsonWriteFrameE %load.struct7, 1
  %add = add i64 %position8, 1
  %position9 = getelementptr inbounds nuw %_Z13ArrayIteratorI14JsonWriteFrameE, ptr %0, i32 0, i32 1
  store i64 %add, ptr %position9, align 8
  %field.inplace = getelementptr inbounds nuw %_Z13ArrayIteratorI14JsonWriteFrameE, ptr %0, i32 0, i32 0
  %deref.recv = load ptr, ptr %field.inplace, align 8
  %call = call ptr @_ZN5ArrayI14JsonWriteFrameB4jsonE10get_bufferEv(ptr %deref.recv)
  %load.struct10 = load %_Z13ArrayIteratorI14JsonWriteFrameE, ptr %0, align 8
  %position11 = extractvalue %_Z13ArrayIteratorI14JsonWriteFrameE %load.struct10, 1
  %sub = sub i64 %position11, 1
  %ptr.add = getelementptr inbounds %_Z14JsonWriteFrame, ptr %call, i64 %sub
  ret ptr %ptr.add
}

define linkonce_odr void @_ZN13ArrayIteratorI14JsonWriteFrameB4jsonEC1E6OptionIR5ArrayI14JsonWriteFrameB4jsonEE(ptr %0, ptr %1) {
entry:
  %array = getelementptr inbounds nuw %_Z13ArrayIteratorI14JsonWriteFrameE, ptr %0, i32 0, i32 0
  store ptr %1, ptr %array, align 8
  %position = getelementptr inbounds nuw %_Z13ArrayIteratorI14JsonWriteFrameE, ptr %0, i32 0, i32 1
  store i64 0, ptr %position, align 8
  ret void
}

define linkonce_odr void @_ZN5ArrayI14JsonWriteFrameB4jsonE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z13ArrayIteratorI14JsonWriteFrameE) %0, ptr %1, ptr %2) {
entry:
  %struct.init = alloca %_Z13ArrayIteratorI14JsonWriteFrameE, align 8
  call void @_ZN13ArrayIteratorI14JsonWriteFrameB4jsonEC1E6OptionIR5ArrayI14JsonWriteFrameB4jsonEE(ptr %struct.init, ptr %2)
  %sret.body = load %_Z13ArrayIteratorI14JsonWriteFrameE, ptr %struct.init, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %struct.init, i64 ptrtoint (ptr getelementptr (%_Z13ArrayIteratorI14JsonWriteFrameE, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN5ArrayI14JsonWriteFrameB4jsonE8as_sliceEv(ptr noalias sret(%_Z5SliceI14JsonWriteFrameE) %0, ptr %1) {
entry:
  %load.struct = load %_Z5ArrayI14JsonWriteFrameE, ptr %1, align 8
  %length = extractvalue %_Z5ArrayI14JsonWriteFrameE %load.struct, 0
  %call = call ptr @_ZN5ArrayI14JsonWriteFrameB4jsonE10get_bufferEv(ptr %1)
  %tuple = alloca %_Z5SliceI14JsonWriteFrameE, align 8
  %tuple.field = getelementptr inbounds nuw %_Z5SliceI14JsonWriteFrameE, ptr %tuple, i32 0, i32 0
  store i64 %length, ptr %tuple.field, align 1
  %tuple.field1 = getelementptr inbounds nuw %_Z5SliceI14JsonWriteFrameE, ptr %tuple, i32 0, i32 1
  store ptr %call, ptr %tuple.field1, align 1
  %tuple.val = load %_Z5SliceI14JsonWriteFrameE, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z5SliceI14JsonWriteFrameE, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN5ArrayI14JsonWriteFrameB4jsonEC1Ev(ptr %0) {
entry:
  %length = getelementptr inbounds nuw %_Z5ArrayI14JsonWriteFrameE, ptr %0, i32 0, i32 0
  store i64 0, ptr %length, align 8
  %capacity = getelementptr inbounds nuw %_Z5ArrayI14JsonWriteFrameE, ptr %0, i32 0, i32 1
  store i64 0, ptr %capacity, align 8
  %buffer = getelementptr inbounds nuw %_Z5ArrayI14JsonWriteFrameE, ptr %0, i32 0, i32 2
  store ptr null, ptr %buffer, align 8
  ret void
}

define linkonce_odr void @_ZN5ArrayI14JsonWriteFrameB4jsonEC1Em(ptr %0, i64 %1) {
entry:
  %length = getelementptr inbounds nuw %_Z5ArrayI14JsonWriteFrameE, ptr %0, i32 0, i32 0
  store i64 0, ptr %length, align 8
  %capacity = getelementptr inbounds nuw %_Z5ArrayI14JsonWriteFrameE, ptr %0, i32 0, i32 1
  store i64 0, ptr %capacity, align 8
  %buffer = getelementptr inbounds nuw %_Z5ArrayI14JsonWriteFrameE, ptr %0, i32 0, i32 2
  store ptr null, ptr %buffer, align 8
  %gt = icmp ugt i64 %1, 0
  br i1 %gt, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %call = call ptr @_ZN4Page3getEPv(ptr %0)
  %capacity1 = getelementptr inbounds nuw %_Z5ArrayI14JsonWriteFrameE, ptr %0, i32 0, i32 1
  store i64 %1, ptr %capacity1, align 8
  %mul = mul i64 %1, ptrtoint (ptr getelementptr (%_Z14JsonWriteFrame, ptr null, i32 1) to i64)
  %le = icmp ule i64 %mul, 1024
  br i1 %le, label %if.then2, label %if.else

if.end:                                           ; preds = %if.end3, %entry
  ret void

if.then2:                                         ; preds = %if.then
  %mul4 = mul i64 %1, ptrtoint (ptr getelementptr (%_Z14JsonWriteFrame, ptr null, i32 1) to i64)
  %call5 = call ptr @_ZN4Page8allocateEmm(ptr %call, i64 %mul4, i64 8)
  %buffer6 = getelementptr inbounds nuw %_Z5ArrayI14JsonWriteFrameE, ptr %0, i32 0, i32 2
  store ptr %call5, ptr %buffer6, align 8
  br label %if.end3

if.else:                                          ; preds = %if.then
  %mul7 = mul i64 %1, ptrtoint (ptr getelementptr (%_Z14JsonWriteFrame, ptr null, i32 1) to i64)
  %call8 = call ptr @_ZN4Page25allocate_exclusive_bufferEmm(ptr %call, i64 %mul7, i64 8)
  %buffer9 = getelementptr inbounds nuw %_Z5ArrayI14JsonWriteFrameE, ptr %0, i32 0, i32 2
  store ptr %call8, ptr %buffer9, align 8
  br label %if.end3

if.end3:                                          ; preds = %if.else, %if.then2
  %if.value = phi ptr [ %call5, %if.then2 ], [ %call8, %if.else ]
  br label %if.end
}

define linkonce_odr void @_ZN5ArrayI14JsonWriteFrameB4jsonEixEm(ptr noalias sret(%_Z14JsonWriteFrame) %0, ptr %1, i64 %2) {
entry:
  %deref.tmp = alloca %_Z14JsonWriteFrame, align 8
  %load.struct = load %_Z5ArrayI14JsonWriteFrameE, ptr %1, align 8
  %length = extractvalue %_Z5ArrayI14JsonWriteFrameE %load.struct, 0
  %ge = icmp uge i64 %2, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %field.inplace = getelementptr inbounds nuw %_Z5ArrayI14JsonWriteFrameE, ptr %1, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  call void @_Z15scaly_panic_oobP10const_charmm(ptr @.str.71, i64 %2, i64 %field.val)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %load.struct1 = load %_Z5ArrayI14JsonWriteFrameE, ptr %1, align 8
  %buffer = extractvalue %_Z5ArrayI14JsonWriteFrameE %load.struct1, 2
  %ptr.add = getelementptr inbounds %_Z14JsonWriteFrame, ptr %buffer, i64 %2
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %deref.tmp, ptr align 1 %ptr.add, i64 ptrtoint (ptr getelementptr (%_Z14JsonWriteFrame, ptr null, i32 1) to i64), i1 false)
  %sret.body = load %_Z14JsonWriteFrame, ptr %deref.tmp, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %deref.tmp, i64 ptrtoint (ptr getelementptr (%_Z14JsonWriteFrame, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN10JsonWriterB4json5valueE9JsonValueB4json(ptr %0, ptr %1) {
entry:
  %arg.tmp506 = alloca %_Z14JsonWriteFrame, align 8
  %tuple501 = alloca %_Z14JsonWriteFrame, align 8
  %sret.result493 = alloca %_Z9JsonValue, align 8
  %arg.tmp476 = alloca %_Z14JsonWriteFrame, align 8
  %tuple471 = alloca %_Z14JsonWriteFrame, align 8
  %sret.result462 = alloca %_Z10JsonMember, align 8
  %sret.result459 = alloca %_Z10JsonMember, align 8
  %sret.result437 = alloca %_Z14JsonWriteFrame, align 8
  %sret.result430 = alloca %_Z5SliceI2u8E, align 8
  %next = alloca i1, align 1
  %arg.tmp400 = alloca %_Z5SliceI10JsonMemberE, align 8
  %sret.result398 = alloca %_Z10JsonMember, align 8
  %arg.tmp395 = alloca %_Z14JsonWriteFrame, align 8
  %tuple390 = alloca %_Z14JsonWriteFrame, align 8
  %sret.result367 = alloca %_Z9JsonValue, align 8
  %arg.tmp = alloca %_Z14JsonWriteFrame, align 8
  %tuple362 = alloca %_Z14JsonWriteFrame, align 8
  %sret.result338 = alloca %_Z5SliceI2u8E, align 8
  %tuple284 = alloca %_Z5SliceI2u8E, align 8
  %sret.result273 = alloca %_Z5SliceI2u8E, align 8
  %tuple196 = alloca %_Z5SliceI2u8E, align 8
  %sret.result184 = alloca %_Z5SliceI2u8E, align 8
  %opened = alloca i1, align 1
  %sret.result110 = alloca %_Z5SliceI2u8E, align 8
  %i = alloca i64, align 8
  %tuple48 = alloca %_Z5SliceI2u8E, align 8
  %name = alloca %_Z10JsonMember, align 8
  %tuple14 = alloca %_Z10JsonMember, align 8
  %variant.ptr = alloca %_Z9JsonValue, align 8
  %tuple10 = alloca %_Z5SliceI2u8E, align 8
  %named = alloca i1, align 1
  %cur = alloca %_Z9JsonValue, align 8
  %p = alloca i64, align 8
  %block = alloca ptr, align 8
  %sret.result = alloca %_Z5SliceI2u8E, align 8
  %base = alloca i64, align 8
  %tuple6 = alloca %_Z5SliceI10JsonMemberE, align 8
  %tuple = alloca %_Z5SliceI9JsonValueE, align 8
  %open = alloca ptr, align 8
  %frame = alloca { ptr, ptr }, align 8
  store ptr null, ptr %frame, align 8
  %frame.parent = getelementptr inbounds nuw { ptr, ptr }, ptr %frame, i32 0, i32 1
  store ptr null, ptr %frame.parent, align 8
  call void @_ZN10JsonWriterB4json8separateEv(ptr %0)
  %frame.page = load ptr, ptr %frame, align 8
  %frame.has_page = icmp ne ptr %frame.page, null
  br i1 %frame.has_page, label %frame.forced, label %frame.force

frame.force:                                      ; preds = %entry
  %forced_page = call ptr @_Z17scaly_force_frameP5Frame(ptr %frame)
  br label %frame.forced

frame.forced:                                     ; preds = %frame.force, %entry
  %forced_page1 = phi ptr [ %frame.page, %entry ], [ %forced_page, %frame.force ]
  %struct.region = call ptr @_ZN4Page8allocateEmm(ptr %forced_page1, i64 ptrtoint (ptr getelementptr (%_Z5ArrayI14JsonWriteFrameE, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr ({ i1, %_Z5ArrayI14JsonWriteFrameE }, ptr null, i64 0, i32 1) to i64))
  %tuple.field = getelementptr inbounds nuw %_Z5ArrayI14JsonWriteFrameE, ptr %struct.region, i32 0, i32 0
  store i64 0, ptr %tuple.field, align 8
  %tuple.field2 = getelementptr inbounds nuw %_Z5ArrayI14JsonWriteFrameE, ptr %struct.region, i32 0, i32 1
  store i64 0, ptr %tuple.field2, align 8
  %tuple.field3 = getelementptr inbounds nuw %_Z5ArrayI14JsonWriteFrameE, ptr %struct.region, i32 0, i32 2
  store ptr null, ptr %tuple.field3, align 8
  store ptr %struct.region, ptr %open, align 1
  %tuple.field4 = getelementptr inbounds nuw %_Z5SliceI9JsonValueE, ptr %tuple, i32 0, i32 0
  store i64 0, ptr %tuple.field4, align 1
  %tuple.field5 = getelementptr inbounds nuw %_Z5SliceI9JsonValueE, ptr %tuple, i32 0, i32 1
  store ptr null, ptr %tuple.field5, align 1
  %tuple.val = load %_Z5SliceI9JsonValueE, ptr %tuple, align 8
  %tuple.field7 = getelementptr inbounds nuw %_Z5SliceI10JsonMemberE, ptr %tuple6, i32 0, i32 0
  store i64 0, ptr %tuple.field7, align 1
  %tuple.field8 = getelementptr inbounds nuw %_Z5SliceI10JsonMemberE, ptr %tuple6, i32 0, i32 1
  store ptr null, ptr %tuple.field8, align 1
  %tuple.val9 = load %_Z5SliceI10JsonMemberE, ptr %tuple6, align 8
  %load.struct = load %_Z10JsonWriter, ptr %0, align 8
  %buffer = extractvalue %_Z10JsonWriter %load.struct, 0
  %length = extractvalue %_Z5ArrayI2u8E %buffer, 0
  store i64 %length, ptr %base, align 1
  %field.inplace = getelementptr inbounds nuw %_Z10JsonWriter, ptr %0, i32 0, i32 0
  call void @_ZN5ArrayI2u8E6extendEm(ptr noalias sret(%_Z5SliceI2u8E) %sret.result, ptr %field.inplace, i64 4096)
  store ptr %sret.result, ptr %block, align 1
  store i64 0, ptr %p, align 1
  %binding.load = load %_Z9JsonValue, ptr %1, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %cur, ptr align 1 %1, i64 ptrtoint (ptr getelementptr (%_Z9JsonValue, ptr null, i32 1) to i64), i1 false)
  store i1 false, ptr %named, align 1
  %tuple.field11 = getelementptr inbounds nuw %_Z5SliceI2u8E, ptr %tuple10, i32 0, i32 0
  store i64 0, ptr %tuple.field11, align 1
  %tuple.field12 = getelementptr inbounds nuw %_Z5SliceI2u8E, ptr %tuple10, i32 0, i32 1
  store ptr null, ptr %tuple.field12, align 1
  %tuple.val13 = load %_Z5SliceI2u8E, ptr %tuple10, align 8
  %variant.tag.ptr = getelementptr inbounds nuw %_Z9JsonValue, ptr %variant.ptr, i32 0, i32 0
  store i8 0, ptr %variant.tag.ptr, align 1
  %variant.val = load %_Z9JsonValue, ptr %variant.ptr, align 1
  %tuple.field15 = getelementptr inbounds nuw %_Z10JsonMember, ptr %tuple14, i32 0, i32 0
  store %_Z5SliceI2u8E %tuple.val13, ptr %tuple.field15, align 1
  %tuple.field16 = getelementptr inbounds nuw %_Z10JsonMember, ptr %tuple14, i32 0, i32 1
  store %_Z9JsonValue %variant.val, ptr %tuple.field16, align 1
  %tuple.field17 = getelementptr inbounds nuw %_Z10JsonMember, ptr %tuple14, i32 0, i32 2
  store i1 false, ptr %tuple.field17, align 1
  %tuple.val18 = load %_Z10JsonMember, ptr %tuple14, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %name, ptr align 1 %tuple14, i64 ptrtoint (ptr getelementptr (%_Z10JsonMember, ptr null, i32 1) to i64), i1 false)
  br label %repeat.body

repeat.body:                                      ; preds = %if.end516, %if.then403, %frame.forced
  %named19 = load i1, ptr %named, align 1
  br i1 %named19, label %if.then, label %if.end

repeat.exit:                                      ; preds = %if.then515
  %base517 = load i64, ptr %base, align 8
  %p518 = load i64, ptr %p, align 8
  %add519 = add i64 %base517, %p518
  %buffer520 = getelementptr inbounds nuw %_Z10JsonWriter, ptr %0, i32 0, i32 0
  %length521 = getelementptr inbounds nuw %_Z5ArrayI2u8E, ptr %buffer520, i32 0, i32 0
  store i64 %add519, ptr %length521, align 8
  %first = getelementptr inbounds nuw %_Z10JsonWriter, ptr %0, i32 0, i32 1
  store i1 false, ptr %first, align 1
  call void @_Z19scaly_release_frameP5Frame(ptr %frame)
  ret void

if.then:                                          ; preds = %repeat.body
  %name20 = load %_Z10JsonMember, ptr %name, align 8
  %key = extractvalue %_Z10JsonMember %name20, 0
  %length21 = extractvalue %_Z5SliceI2u8E %key, 0
  %p22 = load i64, ptr %p, align 8
  %add = add i64 %p22, %length21
  %add23 = add i64 %add, 19
  %block24 = load ptr, ptr %block, align 8
  %load.struct25 = load %_Z5SliceI2u8E, ptr %block24, align 8
  %length26 = extractvalue %_Z5SliceI2u8E %load.struct25, 0
  %gt = icmp ugt i64 %add23, %length26
  br i1 %gt, label %if.then27, label %if.end28

if.end:                                           ; preds = %if.end42, %repeat.body
  %p94 = load i64, ptr %p, align 8
  %add95 = add i64 %p94, 16
  %block96 = load ptr, ptr %block, align 8
  %load.struct97 = load %_Z5SliceI2u8E, ptr %block96, align 8
  %length98 = extractvalue %_Z5SliceI2u8E %load.struct97, 0
  %gt99 = icmp ugt i64 %add95, %length98
  br i1 %gt99, label %if.then100, label %if.end101

if.then27:                                        ; preds = %if.then
  %base29 = load i64, ptr %base, align 8
  %p30 = load i64, ptr %p, align 8
  %add31 = add i64 %base29, %p30
  %buffer32 = getelementptr inbounds nuw %_Z10JsonWriter, ptr %0, i32 0, i32 0
  %length33 = getelementptr inbounds nuw %_Z5ArrayI2u8E, ptr %buffer32, i32 0, i32 0
  store i64 %add31, ptr %length33, align 8
  %load.struct34 = load %_Z10JsonWriter, ptr %0, align 8
  %buffer35 = extractvalue %_Z10JsonWriter %load.struct34, 0
  %length36 = extractvalue %_Z5ArrayI2u8E %buffer35, 0
  store i64 %length36, ptr %base, align 1
  %field.inplace37 = getelementptr inbounds nuw %_Z10JsonWriter, ptr %0, i32 0, i32 0
  %add38 = add i64 %length21, 19
  %add39 = add i64 %add38, 4096
  call void @_ZN5ArrayI2u8E6extendEm(ptr noalias sret(%_Z5SliceI2u8E) %tuple10, ptr %field.inplace37, i64 %add39)
  %set.dest = load ptr, ptr %block, align 8
  %set.thru = load %_Z5SliceI2u8E, ptr %tuple10, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %set.dest, ptr align 1 %tuple10, i64 ptrtoint (ptr getelementptr (%_Z5SliceI2u8E, ptr null, i32 1) to i64), i1 false)
  store i64 0, ptr %p, align 1
  br label %if.end28

if.end28:                                         ; preds = %if.then27, %if.then
  %name40 = load %_Z10JsonMember, ptr %name, align 8
  %verbatim_key = extractvalue %_Z10JsonMember %name40, 2
  br i1 %verbatim_key, label %if.then41, label %if.else

if.then41:                                        ; preds = %if.end28
  %block43 = load ptr, ptr %block, align 8
  %p44 = load i64, ptr %p, align 8
  call void @_ZN5SliceI2u8E3putEm2u8(ptr %block43, i64 %p44, i8 34)
  %add45 = add i64 %length21, 16
  %name46 = load %_Z10JsonMember, ptr %name, align 8
  %key47 = extractvalue %_Z10JsonMember %name46, 0
  %data = extractvalue %_Z5SliceI2u8E %key47, 1
  %tuple.field49 = getelementptr inbounds nuw %_Z5SliceI2u8E, ptr %tuple48, i32 0, i32 0
  store i64 %add45, ptr %tuple.field49, align 1
  %tuple.field50 = getelementptr inbounds nuw %_Z5SliceI2u8E, ptr %tuple48, i32 0, i32 1
  store ptr %data, ptr %tuple.field50, align 1
  %tuple.val51 = load %_Z5SliceI2u8E, ptr %tuple48, align 8
  store i64 0, ptr %i, align 1
  br label %while.cond

if.else:                                          ; preds = %if.end28
  %base82 = load i64, ptr %base, align 8
  %p83 = load i64, ptr %p, align 8
  %add84 = add i64 %base82, %p83
  %buffer85 = getelementptr inbounds nuw %_Z10JsonWriter, ptr %0, i32 0, i32 0
  %length86 = getelementptr inbounds nuw %_Z5ArrayI2u8E, ptr %buffer85, i32 0, i32 0
  store i64 %add84, ptr %length86, align 8
  %field.inplace87 = getelementptr inbounds nuw %_Z10JsonMember, ptr %name, i32 0, i32 0
  call void @_ZN10JsonWriterB4json6quotedE5SliceI2u8E(ptr %0, ptr %field.inplace87)
  call void @_ZN10JsonWriterB4json4byteE2u8(ptr %0, i8 58)
  %load.struct88 = load %_Z10JsonWriter, ptr %0, align 8
  %buffer89 = extractvalue %_Z10JsonWriter %load.struct88, 0
  %length90 = extractvalue %_Z5ArrayI2u8E %buffer89, 0
  store i64 %length90, ptr %base, align 1
  %field.inplace91 = getelementptr inbounds nuw %_Z10JsonWriter, ptr %0, i32 0, i32 0
  call void @_ZN5ArrayI2u8E6extendEm(ptr noalias sret(%_Z5SliceI2u8E) %tuple48, ptr %field.inplace91, i64 4096)
  %set.dest92 = load ptr, ptr %block, align 8
  %set.thru93 = load %_Z5SliceI2u8E, ptr %tuple48, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %set.dest92, ptr align 1 %tuple48, i64 ptrtoint (ptr getelementptr (%_Z5SliceI2u8E, ptr null, i32 1) to i64), i1 false)
  store i64 0, ptr %p, align 1
  br label %if.end42

if.end42:                                         ; preds = %if.else, %while.exit
  store i1 false, ptr %named, align 1
  br label %if.end

while.cond:                                       ; preds = %simd.mem.ok67, %if.then41
  %i52 = load i64, ptr %i, align 8
  %lt = icmp ult i64 %i52, %length21
  br i1 %lt, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %i53 = load i64, ptr %i, align 8
  %simd.cont = extractvalue %_Z5SliceI2u8E %tuple.val51, 0
  %simd.cont54 = extractvalue %_Z5SliceI2u8E %tuple.val51, 1
  %simd.fits = icmp uge i64 %simd.cont, 16
  %simd.room = sub i64 %simd.cont, 16
  %simd.within = icmp ule i64 %i53, %simd.room
  %simd.inrange = and i1 %simd.fits, %simd.within
  br i1 %simd.inrange, label %simd.mem.ok, label %simd.mem.oob

while.exit:                                       ; preds = %while.cond
  %block71 = load ptr, ptr %block, align 8
  %p72 = load i64, ptr %p, align 8
  %add73 = add i64 %p72, 1
  %add74 = add i64 %add73, %length21
  call void @_ZN5SliceI2u8E3putEm2u8(ptr %block71, i64 %add74, i8 34)
  %block75 = load ptr, ptr %block, align 8
  %p76 = load i64, ptr %p, align 8
  %add77 = add i64 %p76, 2
  %add78 = add i64 %add77, %length21
  call void @_ZN5SliceI2u8E3putEm2u8(ptr %block75, i64 %add78, i8 58)
  %p79 = load i64, ptr %p, align 8
  %add80 = add i64 %p79, %length21
  %add81 = add i64 %add80, 3
  store i64 %add81, ptr %p, align 1
  br label %if.end42

simd.mem.oob:                                     ; preds = %while.body
  call void @_Z15scaly_panic_oobP10const_charmm(ptr @.simd.mem.72, i64 %i53, i64 %simd.cont)
  unreachable

simd.mem.ok:                                      ; preds = %while.body
  %simd.addr = getelementptr inbounds i8, ptr %simd.cont54, i64 %i53
  %simd.load = load <16 x i8>, ptr %simd.addr, align 1
  %block55 = load ptr, ptr %block, align 8
  %p56 = load i64, ptr %p, align 8
  %add57 = add i64 %p56, 1
  %i58 = load i64, ptr %i, align 8
  %add59 = add i64 %add57, %i58
  %simd.cont60 = getelementptr inbounds nuw %_Z5SliceI2u8E, ptr %block55, i32 0, i32 0
  %simd.len = load i64, ptr %simd.cont60, align 8
  %simd.cont61 = getelementptr inbounds nuw %_Z5SliceI2u8E, ptr %block55, i32 0, i32 1
  %simd.data = load ptr, ptr %simd.cont61, align 8
  %simd.fits62 = icmp uge i64 %simd.len, 16
  %simd.room63 = sub i64 %simd.len, 16
  %simd.within64 = icmp ule i64 %add59, %simd.room63
  %simd.inrange65 = and i1 %simd.fits62, %simd.within64
  br i1 %simd.inrange65, label %simd.mem.ok67, label %simd.mem.oob66

simd.mem.oob66:                                   ; preds = %simd.mem.ok
  call void @_Z15scaly_panic_oobP10const_charmm(ptr @.simd.mem.73, i64 %add59, i64 %simd.len)
  unreachable

simd.mem.ok67:                                    ; preds = %simd.mem.ok
  %simd.addr68 = getelementptr inbounds i8, ptr %simd.data, i64 %add59
  store <16 x i8> %simd.load, ptr %simd.addr68, align 1
  %i69 = load i64, ptr %i, align 8
  %add70 = add i64 %i69, 16
  store i64 %add70, ptr %i, align 1
  br label %while.cond

if.then100:                                       ; preds = %if.end
  %base102 = load i64, ptr %base, align 8
  %p103 = load i64, ptr %p, align 8
  %add104 = add i64 %base102, %p103
  %buffer105 = getelementptr inbounds nuw %_Z10JsonWriter, ptr %0, i32 0, i32 0
  %length106 = getelementptr inbounds nuw %_Z5ArrayI2u8E, ptr %buffer105, i32 0, i32 0
  store i64 %add104, ptr %length106, align 8
  %load.struct107 = load %_Z10JsonWriter, ptr %0, align 8
  %buffer108 = extractvalue %_Z10JsonWriter %load.struct107, 0
  %length109 = extractvalue %_Z5ArrayI2u8E %buffer108, 0
  store i64 %length109, ptr %base, align 1
  %field.inplace111 = getelementptr inbounds nuw %_Z10JsonWriter, ptr %0, i32 0, i32 0
  call void @_ZN5ArrayI2u8E6extendEm(ptr noalias sret(%_Z5SliceI2u8E) %sret.result110, ptr %field.inplace111, i64 4096)
  %set.dest112 = load ptr, ptr %block, align 8
  %set.thru113 = load %_Z5SliceI2u8E, ptr %sret.result110, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %set.dest112, ptr align 1 %sret.result110, i64 ptrtoint (ptr getelementptr (%_Z5SliceI2u8E, ptr null, i32 1) to i64), i1 false)
  store i64 0, ptr %p, align 1
  br label %if.end101

if.end101:                                        ; preds = %if.then100, %if.end
  store i1 false, ptr %opened, align 1
  %cur114 = load %_Z9JsonValue, ptr %cur, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %variant.ptr, ptr align 1 %cur, i64 ptrtoint (ptr getelementptr (%_Z9JsonValue, ptr null, i32 1) to i64), i1 false)
  %tag.ptr = getelementptr inbounds nuw %_Z9JsonValue, ptr %variant.ptr, i32 0, i32 0
  %tag = load i8, ptr %tag.ptr, align 1
  switch i8 %tag, label %choose.else [
    i8 0, label %choose.when
    i8 1, label %choose.when128
    i8 2, label %choose.when163
    i8 3, label %choose.when248
    i8 4, label %choose.when342
    i8 5, label %choose.when369
  ]

choose.end:                                       ; preds = %choose.else, %if.end376, %if.end348, %if.end255, %if.end192, %if.end133, %choose.when
  %opened402 = load i1, ptr %opened, align 1
  br i1 %opened402, label %if.then403, label %if.end404

choose.else:                                      ; preds = %if.end101
  br label %choose.end

choose.when:                                      ; preds = %if.end101
  %"variant.c_data().ptr" = getelementptr inbounds nuw %_Z9JsonValue, ptr %variant.ptr, i32 0, i32 1
  %block115 = load ptr, ptr %block, align 8
  %p116 = load i64, ptr %p, align 8
  call void @_ZN5SliceI2u8E3putEm2u8(ptr %block115, i64 %p116, i8 110)
  %block117 = load ptr, ptr %block, align 8
  %p118 = load i64, ptr %p, align 8
  %add119 = add i64 %p118, 1
  call void @_ZN5SliceI2u8E3putEm2u8(ptr %block117, i64 %add119, i8 117)
  %block120 = load ptr, ptr %block, align 8
  %p121 = load i64, ptr %p, align 8
  %add122 = add i64 %p121, 2
  call void @_ZN5SliceI2u8E3putEm2u8(ptr %block120, i64 %add122, i8 108)
  %block123 = load ptr, ptr %block, align 8
  %p124 = load i64, ptr %p, align 8
  %add125 = add i64 %p124, 3
  call void @_ZN5SliceI2u8E3putEm2u8(ptr %block123, i64 %add125, i8 108)
  %p126 = load i64, ptr %p, align 8
  %add127 = add i64 %p126, 4
  store i64 %add127, ptr %p, align 1
  br label %choose.end

choose.when128:                                   ; preds = %if.end101
  %"variant.c_data().ptr129" = getelementptr inbounds nuw %_Z9JsonValue, ptr %variant.ptr, i32 0, i32 1
  %variant.val130 = load i1, ptr %"variant.c_data().ptr129", align 1
  br i1 %variant.val130, label %if.then131, label %if.else132

if.then131:                                       ; preds = %choose.when128
  %block134 = load ptr, ptr %block, align 8
  %p135 = load i64, ptr %p, align 8
  call void @_ZN5SliceI2u8E3putEm2u8(ptr %block134, i64 %p135, i8 116)
  %block136 = load ptr, ptr %block, align 8
  %p137 = load i64, ptr %p, align 8
  %add138 = add i64 %p137, 1
  call void @_ZN5SliceI2u8E3putEm2u8(ptr %block136, i64 %add138, i8 114)
  %block139 = load ptr, ptr %block, align 8
  %p140 = load i64, ptr %p, align 8
  %add141 = add i64 %p140, 2
  call void @_ZN5SliceI2u8E3putEm2u8(ptr %block139, i64 %add141, i8 117)
  %block142 = load ptr, ptr %block, align 8
  %p143 = load i64, ptr %p, align 8
  %add144 = add i64 %p143, 3
  call void @_ZN5SliceI2u8E3putEm2u8(ptr %block142, i64 %add144, i8 101)
  %p145 = load i64, ptr %p, align 8
  %add146 = add i64 %p145, 4
  store i64 %add146, ptr %p, align 1
  br label %if.end133

if.else132:                                       ; preds = %choose.when128
  %block147 = load ptr, ptr %block, align 8
  %p148 = load i64, ptr %p, align 8
  call void @_ZN5SliceI2u8E3putEm2u8(ptr %block147, i64 %p148, i8 102)
  %block149 = load ptr, ptr %block, align 8
  %p150 = load i64, ptr %p, align 8
  %add151 = add i64 %p150, 1
  call void @_ZN5SliceI2u8E3putEm2u8(ptr %block149, i64 %add151, i8 97)
  %block152 = load ptr, ptr %block, align 8
  %p153 = load i64, ptr %p, align 8
  %add154 = add i64 %p153, 2
  call void @_ZN5SliceI2u8E3putEm2u8(ptr %block152, i64 %add154, i8 108)
  %block155 = load ptr, ptr %block, align 8
  %p156 = load i64, ptr %p, align 8
  %add157 = add i64 %p156, 3
  call void @_ZN5SliceI2u8E3putEm2u8(ptr %block155, i64 %add157, i8 115)
  %block158 = load ptr, ptr %block, align 8
  %p159 = load i64, ptr %p, align 8
  %add160 = add i64 %p159, 4
  call void @_ZN5SliceI2u8E3putEm2u8(ptr %block158, i64 %add160, i8 101)
  %p161 = load i64, ptr %p, align 8
  %add162 = add i64 %p161, 5
  store i64 %add162, ptr %p, align 1
  br label %if.end133

if.end133:                                        ; preds = %if.else132, %if.then131
  br label %choose.end

choose.when163:                                   ; preds = %if.end101
  %"variant.c_data().ptr164" = getelementptr inbounds nuw %_Z9JsonValue, ptr %variant.ptr, i32 0, i32 1
  %variant.val165 = load %_Z10JsonNumber, ptr %"variant.c_data().ptr164", align 8
  %text = extractvalue %_Z10JsonNumber %variant.val165, 0
  %length166 = extractvalue %_Z5SliceI2u8E %text, 0
  %p167 = load i64, ptr %p, align 8
  %add168 = add i64 %p167, %length166
  %add169 = add i64 %add168, 18
  %block170 = load ptr, ptr %block, align 8
  %load.struct171 = load %_Z5SliceI2u8E, ptr %block170, align 8
  %length172 = extractvalue %_Z5SliceI2u8E %load.struct171, 0
  %gt173 = icmp ugt i64 %add169, %length172
  br i1 %gt173, label %if.then174, label %if.end175

if.then174:                                       ; preds = %choose.when163
  %base176 = load i64, ptr %base, align 8
  %p177 = load i64, ptr %p, align 8
  %add178 = add i64 %base176, %p177
  %buffer179 = getelementptr inbounds nuw %_Z10JsonWriter, ptr %0, i32 0, i32 0
  %length180 = getelementptr inbounds nuw %_Z5ArrayI2u8E, ptr %buffer179, i32 0, i32 0
  store i64 %add178, ptr %length180, align 8
  %load.struct181 = load %_Z10JsonWriter, ptr %0, align 8
  %buffer182 = extractvalue %_Z10JsonWriter %load.struct181, 0
  %length183 = extractvalue %_Z5ArrayI2u8E %buffer182, 0
  store i64 %length183, ptr %base, align 1
  %field.inplace185 = getelementptr inbounds nuw %_Z10JsonWriter, ptr %0, i32 0, i32 0
  %add186 = add i64 %length166, 18
  %add187 = add i64 %add186, 4096
  call void @_ZN5ArrayI2u8E6extendEm(ptr noalias sret(%_Z5SliceI2u8E) %sret.result184, ptr %field.inplace185, i64 %add187)
  %set.dest188 = load ptr, ptr %block, align 8
  %set.thru189 = load %_Z5SliceI2u8E, ptr %sret.result184, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %set.dest188, ptr align 1 %sret.result184, i64 ptrtoint (ptr getelementptr (%_Z5SliceI2u8E, ptr null, i32 1) to i64), i1 false)
  store i64 0, ptr %p, align 1
  br label %if.end175

if.end175:                                        ; preds = %if.then174, %choose.when163
  %verbatim = extractvalue %_Z10JsonNumber %variant.val165, 1
  br i1 %verbatim, label %if.then190, label %if.else191

if.then190:                                       ; preds = %if.end175
  %add193 = add i64 %length166, 16
  %text194 = extractvalue %_Z10JsonNumber %variant.val165, 0
  %data195 = extractvalue %_Z5SliceI2u8E %text194, 1
  %tuple.field197 = getelementptr inbounds nuw %_Z5SliceI2u8E, ptr %tuple196, i32 0, i32 0
  store i64 %add193, ptr %tuple.field197, align 1
  %tuple.field198 = getelementptr inbounds nuw %_Z5SliceI2u8E, ptr %tuple196, i32 0, i32 1
  store ptr %data195, ptr %tuple.field198, align 1
  %tuple.val199 = load %_Z5SliceI2u8E, ptr %tuple196, align 8
  store i64 0, ptr %i, align 1
  br label %while.cond200

if.else191:                                       ; preds = %if.end175
  store i64 0, ptr %i, align 1
  br label %while.cond233

if.end192:                                        ; preds = %while.exit235, %while.exit202
  %p246 = load i64, ptr %p, align 8
  %add247 = add i64 %p246, %length166
  store i64 %add247, ptr %p, align 1
  br label %choose.end

while.cond200:                                    ; preds = %simd.mem.ok229, %if.then190
  %i203 = load i64, ptr %i, align 8
  %lt204 = icmp ult i64 %i203, %length166
  br i1 %lt204, label %while.body201, label %while.exit202

while.body201:                                    ; preds = %while.cond200
  %i205 = load i64, ptr %i, align 8
  %simd.cont206 = extractvalue %_Z5SliceI2u8E %tuple.val199, 0
  %simd.cont207 = extractvalue %_Z5SliceI2u8E %tuple.val199, 1
  %simd.fits208 = icmp uge i64 %simd.cont206, 16
  %simd.room209 = sub i64 %simd.cont206, 16
  %simd.within210 = icmp ule i64 %i205, %simd.room209
  %simd.inrange211 = and i1 %simd.fits208, %simd.within210
  br i1 %simd.inrange211, label %simd.mem.ok213, label %simd.mem.oob212

while.exit202:                                    ; preds = %while.cond200
  br label %if.end192

simd.mem.oob212:                                  ; preds = %while.body201
  call void @_Z15scaly_panic_oobP10const_charmm(ptr @.simd.mem.74, i64 %i205, i64 %simd.cont206)
  unreachable

simd.mem.ok213:                                   ; preds = %while.body201
  %simd.addr214 = getelementptr inbounds i8, ptr %simd.cont207, i64 %i205
  %simd.load215 = load <16 x i8>, ptr %simd.addr214, align 1
  %block216 = load ptr, ptr %block, align 8
  %p217 = load i64, ptr %p, align 8
  %i218 = load i64, ptr %i, align 8
  %add219 = add i64 %p217, %i218
  %simd.cont220 = getelementptr inbounds nuw %_Z5SliceI2u8E, ptr %block216, i32 0, i32 0
  %simd.len221 = load i64, ptr %simd.cont220, align 8
  %simd.cont222 = getelementptr inbounds nuw %_Z5SliceI2u8E, ptr %block216, i32 0, i32 1
  %simd.data223 = load ptr, ptr %simd.cont222, align 8
  %simd.fits224 = icmp uge i64 %simd.len221, 16
  %simd.room225 = sub i64 %simd.len221, 16
  %simd.within226 = icmp ule i64 %add219, %simd.room225
  %simd.inrange227 = and i1 %simd.fits224, %simd.within226
  br i1 %simd.inrange227, label %simd.mem.ok229, label %simd.mem.oob228

simd.mem.oob228:                                  ; preds = %simd.mem.ok213
  call void @_Z15scaly_panic_oobP10const_charmm(ptr @.simd.mem.75, i64 %add219, i64 %simd.len221)
  unreachable

simd.mem.ok229:                                   ; preds = %simd.mem.ok213
  %simd.addr230 = getelementptr inbounds i8, ptr %simd.data223, i64 %add219
  store <16 x i8> %simd.load215, ptr %simd.addr230, align 1
  %i231 = load i64, ptr %i, align 8
  %add232 = add i64 %i231, 16
  store i64 %add232, ptr %i, align 1
  br label %while.cond200

while.cond233:                                    ; preds = %while.body234, %if.else191
  %i236 = load i64, ptr %i, align 8
  %lt237 = icmp ult i64 %i236, %length166
  br i1 %lt237, label %while.body234, label %while.exit235

while.body234:                                    ; preds = %while.cond233
  %block238 = load ptr, ptr %block, align 8
  %p239 = load i64, ptr %p, align 8
  %i240 = load i64, ptr %i, align 8
  %add241 = add i64 %p239, %i240
  %text242 = extractvalue %_Z10JsonNumber %variant.val165, 0
  store %_Z5SliceI2u8E %text242, ptr %tuple196, align 1
  %i243 = load i64, ptr %i, align 8
  %call = call i8 @_ZN5SliceI2u8EixEm(ptr %tuple196, i64 %i243)
  call void @_ZN5SliceI2u8E3putEm2u8(ptr %block238, i64 %add241, i8 %call)
  %i244 = load i64, ptr %i, align 8
  %add245 = add i64 %i244, 1
  store i64 %add245, ptr %i, align 1
  br label %while.cond233

while.exit235:                                    ; preds = %while.cond233
  br label %if.end192

choose.when248:                                   ; preds = %if.end101
  %"variant.c_data().ptr249" = getelementptr inbounds nuw %_Z9JsonValue, ptr %variant.ptr, i32 0, i32 1
  %variant.val250 = load %_Z10JsonString, ptr %"variant.c_data().ptr249", align 8
  %bytes = extractvalue %_Z10JsonString %variant.val250, 0
  %length251 = extractvalue %_Z5SliceI2u8E %bytes, 0
  %verbatim252 = extractvalue %_Z10JsonString %variant.val250, 1
  br i1 %verbatim252, label %if.then253, label %if.else254

if.then253:                                       ; preds = %choose.when248
  %p256 = load i64, ptr %p, align 8
  %add257 = add i64 %p256, %length251
  %add258 = add i64 %add257, 18
  %block259 = load ptr, ptr %block, align 8
  %load.struct260 = load %_Z5SliceI2u8E, ptr %block259, align 8
  %length261 = extractvalue %_Z5SliceI2u8E %load.struct260, 0
  %gt262 = icmp ugt i64 %add258, %length261
  br i1 %gt262, label %if.then263, label %if.end264

if.else254:                                       ; preds = %choose.when248
  %base329 = load i64, ptr %base, align 8
  %p330 = load i64, ptr %p, align 8
  %add331 = add i64 %base329, %p330
  %buffer332 = getelementptr inbounds nuw %_Z10JsonWriter, ptr %0, i32 0, i32 0
  %length333 = getelementptr inbounds nuw %_Z5ArrayI2u8E, ptr %buffer332, i32 0, i32 0
  store i64 %add331, ptr %length333, align 8
  %bytes334 = extractvalue %_Z10JsonString %variant.val250, 0
  store %_Z5SliceI2u8E %bytes334, ptr %tuple284, align 1
  call void @_ZN10JsonWriterB4json6quotedE5SliceI2u8E(ptr %0, ptr %tuple284)
  %load.struct335 = load %_Z10JsonWriter, ptr %0, align 8
  %buffer336 = extractvalue %_Z10JsonWriter %load.struct335, 0
  %length337 = extractvalue %_Z5ArrayI2u8E %buffer336, 0
  store i64 %length337, ptr %base, align 1
  %field.inplace339 = getelementptr inbounds nuw %_Z10JsonWriter, ptr %0, i32 0, i32 0
  call void @_ZN5ArrayI2u8E6extendEm(ptr noalias sret(%_Z5SliceI2u8E) %sret.result338, ptr %field.inplace339, i64 4096)
  %set.dest340 = load ptr, ptr %block, align 8
  %set.thru341 = load %_Z5SliceI2u8E, ptr %sret.result338, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %set.dest340, ptr align 1 %sret.result338, i64 ptrtoint (ptr getelementptr (%_Z5SliceI2u8E, ptr null, i32 1) to i64), i1 false)
  store i64 0, ptr %p, align 1
  br label %if.end255

if.end255:                                        ; preds = %if.else254, %while.exit290
  br label %choose.end

if.then263:                                       ; preds = %if.then253
  %base265 = load i64, ptr %base, align 8
  %p266 = load i64, ptr %p, align 8
  %add267 = add i64 %base265, %p266
  %buffer268 = getelementptr inbounds nuw %_Z10JsonWriter, ptr %0, i32 0, i32 0
  %length269 = getelementptr inbounds nuw %_Z5ArrayI2u8E, ptr %buffer268, i32 0, i32 0
  store i64 %add267, ptr %length269, align 8
  %load.struct270 = load %_Z10JsonWriter, ptr %0, align 8
  %buffer271 = extractvalue %_Z10JsonWriter %load.struct270, 0
  %length272 = extractvalue %_Z5ArrayI2u8E %buffer271, 0
  store i64 %length272, ptr %base, align 1
  %field.inplace274 = getelementptr inbounds nuw %_Z10JsonWriter, ptr %0, i32 0, i32 0
  %add275 = add i64 %length251, 18
  %add276 = add i64 %add275, 4096
  call void @_ZN5ArrayI2u8E6extendEm(ptr noalias sret(%_Z5SliceI2u8E) %sret.result273, ptr %field.inplace274, i64 %add276)
  %set.dest277 = load ptr, ptr %block, align 8
  %set.thru278 = load %_Z5SliceI2u8E, ptr %sret.result273, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %set.dest277, ptr align 1 %sret.result273, i64 ptrtoint (ptr getelementptr (%_Z5SliceI2u8E, ptr null, i32 1) to i64), i1 false)
  store i64 0, ptr %p, align 1
  br label %if.end264

if.end264:                                        ; preds = %if.then263, %if.then253
  %block279 = load ptr, ptr %block, align 8
  %p280 = load i64, ptr %p, align 8
  call void @_ZN5SliceI2u8E3putEm2u8(ptr %block279, i64 %p280, i8 34)
  %add281 = add i64 %length251, 16
  %bytes282 = extractvalue %_Z10JsonString %variant.val250, 0
  %data283 = extractvalue %_Z5SliceI2u8E %bytes282, 1
  %tuple.field285 = getelementptr inbounds nuw %_Z5SliceI2u8E, ptr %tuple284, i32 0, i32 0
  store i64 %add281, ptr %tuple.field285, align 1
  %tuple.field286 = getelementptr inbounds nuw %_Z5SliceI2u8E, ptr %tuple284, i32 0, i32 1
  store ptr %data283, ptr %tuple.field286, align 1
  %tuple.val287 = load %_Z5SliceI2u8E, ptr %tuple284, align 8
  store i64 0, ptr %i, align 1
  br label %while.cond288

while.cond288:                                    ; preds = %simd.mem.ok318, %if.end264
  %i291 = load i64, ptr %i, align 8
  %lt292 = icmp ult i64 %i291, %length251
  br i1 %lt292, label %while.body289, label %while.exit290

while.body289:                                    ; preds = %while.cond288
  %i293 = load i64, ptr %i, align 8
  %simd.cont294 = extractvalue %_Z5SliceI2u8E %tuple.val287, 0
  %simd.cont295 = extractvalue %_Z5SliceI2u8E %tuple.val287, 1
  %simd.fits296 = icmp uge i64 %simd.cont294, 16
  %simd.room297 = sub i64 %simd.cont294, 16
  %simd.within298 = icmp ule i64 %i293, %simd.room297
  %simd.inrange299 = and i1 %simd.fits296, %simd.within298
  br i1 %simd.inrange299, label %simd.mem.ok301, label %simd.mem.oob300

while.exit290:                                    ; preds = %while.cond288
  %block322 = load ptr, ptr %block, align 8
  %p323 = load i64, ptr %p, align 8
  %add324 = add i64 %p323, 1
  %add325 = add i64 %add324, %length251
  call void @_ZN5SliceI2u8E3putEm2u8(ptr %block322, i64 %add325, i8 34)
  %p326 = load i64, ptr %p, align 8
  %add327 = add i64 %p326, %length251
  %add328 = add i64 %add327, 2
  store i64 %add328, ptr %p, align 1
  br label %if.end255

simd.mem.oob300:                                  ; preds = %while.body289
  call void @_Z15scaly_panic_oobP10const_charmm(ptr @.simd.mem.76, i64 %i293, i64 %simd.cont294)
  unreachable

simd.mem.ok301:                                   ; preds = %while.body289
  %simd.addr302 = getelementptr inbounds i8, ptr %simd.cont295, i64 %i293
  %simd.load303 = load <16 x i8>, ptr %simd.addr302, align 1
  %block304 = load ptr, ptr %block, align 8
  %p305 = load i64, ptr %p, align 8
  %add306 = add i64 %p305, 1
  %i307 = load i64, ptr %i, align 8
  %add308 = add i64 %add306, %i307
  %simd.cont309 = getelementptr inbounds nuw %_Z5SliceI2u8E, ptr %block304, i32 0, i32 0
  %simd.len310 = load i64, ptr %simd.cont309, align 8
  %simd.cont311 = getelementptr inbounds nuw %_Z5SliceI2u8E, ptr %block304, i32 0, i32 1
  %simd.data312 = load ptr, ptr %simd.cont311, align 8
  %simd.fits313 = icmp uge i64 %simd.len310, 16
  %simd.room314 = sub i64 %simd.len310, 16
  %simd.within315 = icmp ule i64 %add308, %simd.room314
  %simd.inrange316 = and i1 %simd.fits313, %simd.within315
  br i1 %simd.inrange316, label %simd.mem.ok318, label %simd.mem.oob317

simd.mem.oob317:                                  ; preds = %simd.mem.ok301
  call void @_Z15scaly_panic_oobP10const_charmm(ptr @.simd.mem.77, i64 %add308, i64 %simd.len310)
  unreachable

simd.mem.ok318:                                   ; preds = %simd.mem.ok301
  %simd.addr319 = getelementptr inbounds i8, ptr %simd.data312, i64 %add308
  store <16 x i8> %simd.load303, ptr %simd.addr319, align 1
  %i320 = load i64, ptr %i, align 8
  %add321 = add i64 %i320, 16
  store i64 %add321, ptr %i, align 1
  br label %while.cond288

choose.when342:                                   ; preds = %if.end101
  %"variant.c_data().ptr343" = getelementptr inbounds nuw %_Z9JsonValue, ptr %variant.ptr, i32 0, i32 1
  %variant.val344 = load %_Z9JsonArray, ptr %"variant.c_data().ptr343", align 8
  %items = extractvalue %_Z9JsonArray %variant.val344, 0
  %length345 = extractvalue %_Z5SliceI9JsonValueE %items, 0
  %eq = icmp eq i64 %length345, 0
  br i1 %eq, label %if.then346, label %if.else347

if.then346:                                       ; preds = %choose.when342
  %block349 = load ptr, ptr %block, align 8
  %p350 = load i64, ptr %p, align 8
  call void @_ZN5SliceI2u8E3putEm2u8(ptr %block349, i64 %p350, i8 91)
  %block351 = load ptr, ptr %block, align 8
  %p352 = load i64, ptr %p, align 8
  %add353 = add i64 %p352, 1
  call void @_ZN5SliceI2u8E3putEm2u8(ptr %block351, i64 %add353, i8 93)
  %p354 = load i64, ptr %p, align 8
  %add355 = add i64 %p354, 2
  store i64 %add355, ptr %p, align 1
  br label %if.end348

if.else347:                                       ; preds = %choose.when342
  %block356 = load ptr, ptr %block, align 8
  %p357 = load i64, ptr %p, align 8
  call void @_ZN5SliceI2u8E3putEm2u8(ptr %block356, i64 %p357, i8 91)
  %p358 = load i64, ptr %p, align 8
  %add359 = add i64 %p358, 1
  store i64 %add359, ptr %p, align 1
  %open360 = load ptr, ptr %open, align 8
  %items361 = extractvalue %_Z9JsonArray %variant.val344, 0
  %tuple.field363 = getelementptr inbounds nuw %_Z14JsonWriteFrame, ptr %tuple362, i32 0, i32 0
  store %_Z5SliceI9JsonValueE %items361, ptr %tuple.field363, align 1
  %tuple.field364 = getelementptr inbounds nuw %_Z14JsonWriteFrame, ptr %tuple362, i32 0, i32 1
  store %_Z5SliceI10JsonMemberE %tuple.val9, ptr %tuple.field364, align 1
  %tuple.field365 = getelementptr inbounds nuw %_Z14JsonWriteFrame, ptr %tuple362, i32 0, i32 2
  store i64 0, ptr %tuple.field365, align 1
  %tuple.val366 = load %_Z14JsonWriteFrame, ptr %tuple362, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %arg.tmp, ptr align 1 %tuple362, i64 ptrtoint (ptr getelementptr (%_Z14JsonWriteFrame, ptr null, i32 1) to i64), i1 false)
  call void @_ZN5ArrayI14JsonWriteFrameB4jsonE3addE14JsonWriteFrameB4json(ptr %open360, ptr %arg.tmp)
  %items368 = extractvalue %_Z9JsonArray %variant.val344, 0
  store %_Z5SliceI9JsonValueE %items368, ptr %tuple, align 1
  call void @_ZN5SliceI9JsonValueB4jsonEixEm(ptr noalias sret(%_Z9JsonValue) %sret.result367, ptr %tuple, i64 0)
  %set.load = load %_Z9JsonValue, ptr %sret.result367, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %cur, ptr align 1 %sret.result367, i64 ptrtoint (ptr getelementptr (%_Z9JsonValue, ptr null, i32 1) to i64), i1 false)
  store i1 true, ptr %opened, align 1
  br label %if.end348

if.end348:                                        ; preds = %if.else347, %if.then346
  br label %choose.end

choose.when369:                                   ; preds = %if.end101
  %"variant.c_data().ptr370" = getelementptr inbounds nuw %_Z9JsonValue, ptr %variant.ptr, i32 0, i32 1
  %variant.val371 = load %_Z10JsonObject, ptr %"variant.c_data().ptr370", align 8
  %members = extractvalue %_Z10JsonObject %variant.val371, 0
  %length372 = extractvalue %_Z5SliceI10JsonMemberE %members, 0
  %eq373 = icmp eq i64 %length372, 0
  br i1 %eq373, label %if.then374, label %if.else375

if.then374:                                       ; preds = %choose.when369
  %block377 = load ptr, ptr %block, align 8
  %p378 = load i64, ptr %p, align 8
  call void @_ZN5SliceI2u8E3putEm2u8(ptr %block377, i64 %p378, i8 123)
  %block379 = load ptr, ptr %block, align 8
  %p380 = load i64, ptr %p, align 8
  %add381 = add i64 %p380, 1
  call void @_ZN5SliceI2u8E3putEm2u8(ptr %block379, i64 %add381, i8 125)
  %p382 = load i64, ptr %p, align 8
  %add383 = add i64 %p382, 2
  store i64 %add383, ptr %p, align 1
  br label %if.end376

if.else375:                                       ; preds = %choose.when369
  %block384 = load ptr, ptr %block, align 8
  %p385 = load i64, ptr %p, align 8
  call void @_ZN5SliceI2u8E3putEm2u8(ptr %block384, i64 %p385, i8 123)
  %p386 = load i64, ptr %p, align 8
  %add387 = add i64 %p386, 1
  store i64 %add387, ptr %p, align 1
  %open388 = load ptr, ptr %open, align 8
  %members389 = extractvalue %_Z10JsonObject %variant.val371, 0
  %tuple.field391 = getelementptr inbounds nuw %_Z14JsonWriteFrame, ptr %tuple390, i32 0, i32 0
  store %_Z5SliceI9JsonValueE %tuple.val, ptr %tuple.field391, align 1
  %tuple.field392 = getelementptr inbounds nuw %_Z14JsonWriteFrame, ptr %tuple390, i32 0, i32 1
  store %_Z5SliceI10JsonMemberE %members389, ptr %tuple.field392, align 1
  %tuple.field393 = getelementptr inbounds nuw %_Z14JsonWriteFrame, ptr %tuple390, i32 0, i32 2
  store i64 0, ptr %tuple.field393, align 1
  %tuple.val394 = load %_Z14JsonWriteFrame, ptr %tuple390, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %arg.tmp395, ptr align 1 %tuple390, i64 ptrtoint (ptr getelementptr (%_Z14JsonWriteFrame, ptr null, i32 1) to i64), i1 false)
  call void @_ZN5ArrayI14JsonWriteFrameB4jsonE3addE14JsonWriteFrameB4json(ptr %open388, ptr %arg.tmp395)
  %members396 = extractvalue %_Z10JsonObject %variant.val371, 0
  store %_Z5SliceI10JsonMemberE %members396, ptr %tuple6, align 1
  call void @_ZN5SliceI10JsonMemberB4jsonEixEm(ptr noalias sret(%_Z10JsonMember) %tuple14, ptr %tuple6, i64 0)
  %set.load397 = load %_Z10JsonMember, ptr %tuple14, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %name, ptr align 1 %tuple14, i64 ptrtoint (ptr getelementptr (%_Z10JsonMember, ptr null, i32 1) to i64), i1 false)
  store i1 true, ptr %named, align 1
  %members399 = extractvalue %_Z10JsonObject %variant.val371, 0
  store %_Z5SliceI10JsonMemberE %members399, ptr %arg.tmp400, align 1
  call void @_ZN5SliceI10JsonMemberB4jsonEixEm(ptr noalias sret(%_Z10JsonMember) %sret.result398, ptr %arg.tmp400, i64 0)
  %load.struct401 = load %_Z10JsonMember, ptr %sret.result398, align 8
  %value = extractvalue %_Z10JsonMember %load.struct401, 1
  store %_Z9JsonValue %value, ptr %cur, align 1
  store i1 true, ptr %opened, align 1
  br label %if.end376

if.end376:                                        ; preds = %if.else375, %if.then374
  br label %choose.end

if.then403:                                       ; preds = %choose.end
  br label %repeat.body

if.end404:                                        ; preds = %choose.end
  store i1 false, ptr %next, align 1
  br label %while.cond405

while.cond405:                                    ; preds = %if.end447, %if.end404
  %next408 = load i1, ptr %next, align 1
  %eq409 = icmp eq i1 %next408, false
  br i1 %eq409, label %lor.rhs, label %lor.end

while.body406:                                    ; preds = %lor.end
  %p414 = load i64, ptr %p, align 8
  %add415 = add i64 %p414, 2
  %block416 = load ptr, ptr %block, align 8
  %load.struct417 = load %_Z5SliceI2u8E, ptr %block416, align 8
  %length418 = extractvalue %_Z5SliceI2u8E %load.struct417, 0
  %gt419 = icmp ugt i64 %add415, %length418
  br i1 %gt419, label %if.then420, label %if.end421

while.exit407:                                    ; preds = %lor.end
  %next513 = load i1, ptr %next, align 1
  %eq514 = icmp eq i1 %next513, false
  br i1 %eq514, label %if.then515, label %if.end516

lor.rhs:                                          ; preds = %while.cond405
  %open410 = load ptr, ptr %open, align 8
  %load.struct411 = load %_Z5ArrayI14JsonWriteFrameE, ptr %open410, align 8
  %length412 = extractvalue %_Z5ArrayI14JsonWriteFrameE %load.struct411, 0
  %gt413 = icmp ugt i64 %length412, 0
  br label %lor.end

lor.end:                                          ; preds = %lor.rhs, %while.cond405
  %lor.result = phi i1 [ false, %while.cond405 ], [ %gt413, %lor.rhs ]
  br i1 %lor.result, label %while.body406, label %while.exit407

if.then420:                                       ; preds = %while.body406
  %base422 = load i64, ptr %base, align 8
  %p423 = load i64, ptr %p, align 8
  %add424 = add i64 %base422, %p423
  %buffer425 = getelementptr inbounds nuw %_Z10JsonWriter, ptr %0, i32 0, i32 0
  %length426 = getelementptr inbounds nuw %_Z5ArrayI2u8E, ptr %buffer425, i32 0, i32 0
  store i64 %add424, ptr %length426, align 8
  %load.struct427 = load %_Z10JsonWriter, ptr %0, align 8
  %buffer428 = extractvalue %_Z10JsonWriter %load.struct427, 0
  %length429 = extractvalue %_Z5ArrayI2u8E %buffer428, 0
  store i64 %length429, ptr %base, align 1
  %field.inplace431 = getelementptr inbounds nuw %_Z10JsonWriter, ptr %0, i32 0, i32 0
  call void @_ZN5ArrayI2u8E6extendEm(ptr noalias sret(%_Z5SliceI2u8E) %sret.result430, ptr %field.inplace431, i64 4096)
  %set.dest432 = load ptr, ptr %block, align 8
  %set.thru433 = load %_Z5SliceI2u8E, ptr %sret.result430, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %set.dest432, ptr align 1 %sret.result430, i64 ptrtoint (ptr getelementptr (%_Z5SliceI2u8E, ptr null, i32 1) to i64), i1 false)
  store i64 0, ptr %p, align 1
  br label %if.end421

if.end421:                                        ; preds = %if.then420, %while.body406
  %open434 = load ptr, ptr %open, align 8
  %load.struct435 = load %_Z5ArrayI14JsonWriteFrameE, ptr %open434, align 8
  %length436 = extractvalue %_Z5ArrayI14JsonWriteFrameE %load.struct435, 0
  %sub = sub i64 %length436, 1
  %open438 = load ptr, ptr %open, align 8
  call void @_ZN5ArrayI14JsonWriteFrameB4jsonEixEm(ptr noalias sret(%_Z14JsonWriteFrame) %sret.result437, ptr %open438, i64 %sub)
  %load.struct439 = load %_Z14JsonWriteFrame, ptr %sret.result437, align 8
  %at = extractvalue %_Z14JsonWriteFrame %load.struct439, 2
  %add440 = add i64 %at, 1
  %load.struct441 = load %_Z14JsonWriteFrame, ptr %sret.result437, align 8
  %members442 = extractvalue %_Z14JsonWriteFrame %load.struct441, 1
  %length443 = extractvalue %_Z5SliceI10JsonMemberE %members442, 0
  %gt444 = icmp ugt i64 %length443, 0
  br i1 %gt444, label %if.then445, label %if.else446

if.then445:                                       ; preds = %if.end421
  %load.struct448 = load %_Z14JsonWriteFrame, ptr %sret.result437, align 8
  %members449 = extractvalue %_Z14JsonWriteFrame %load.struct448, 1
  %length450 = extractvalue %_Z5SliceI10JsonMemberE %members449, 0
  %lt451 = icmp ult i64 %add440, %length450
  br i1 %lt451, label %if.then452, label %if.else453

if.else446:                                       ; preds = %if.end421
  %load.struct482 = load %_Z14JsonWriteFrame, ptr %sret.result437, align 8
  %items483 = extractvalue %_Z14JsonWriteFrame %load.struct482, 0
  %length484 = extractvalue %_Z5SliceI9JsonValueE %items483, 0
  %lt485 = icmp ult i64 %add440, %length484
  br i1 %lt485, label %if.then486, label %if.else487

if.end447:                                        ; preds = %if.end488, %if.end454
  br label %while.cond405

if.then452:                                       ; preds = %if.then445
  %block455 = load ptr, ptr %block, align 8
  %p456 = load i64, ptr %p, align 8
  call void @_ZN5SliceI2u8E3putEm2u8(ptr %block455, i64 %p456, i8 44)
  %p457 = load i64, ptr %p, align 8
  %add458 = add i64 %p457, 1
  store i64 %add458, ptr %p, align 1
  %field.inplace460 = getelementptr inbounds nuw %_Z14JsonWriteFrame, ptr %sret.result437, i32 0, i32 1
  call void @_ZN5SliceI10JsonMemberB4jsonEixEm(ptr noalias sret(%_Z10JsonMember) %sret.result459, ptr %field.inplace460, i64 %add440)
  %set.load461 = load %_Z10JsonMember, ptr %sret.result459, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %name, ptr align 1 %sret.result459, i64 ptrtoint (ptr getelementptr (%_Z10JsonMember, ptr null, i32 1) to i64), i1 false)
  store i1 true, ptr %named, align 1
  %field.inplace463 = getelementptr inbounds nuw %_Z14JsonWriteFrame, ptr %sret.result437, i32 0, i32 1
  call void @_ZN5SliceI10JsonMemberB4jsonEixEm(ptr noalias sret(%_Z10JsonMember) %sret.result462, ptr %field.inplace463, i64 %add440)
  %load.struct464 = load %_Z10JsonMember, ptr %sret.result462, align 8
  %value465 = extractvalue %_Z10JsonMember %load.struct464, 1
  store %_Z9JsonValue %value465, ptr %cur, align 1
  %open466 = load ptr, ptr %open, align 8
  %load.struct467 = load %_Z14JsonWriteFrame, ptr %sret.result437, align 8
  %items468 = extractvalue %_Z14JsonWriteFrame %load.struct467, 0
  %load.struct469 = load %_Z14JsonWriteFrame, ptr %sret.result437, align 8
  %members470 = extractvalue %_Z14JsonWriteFrame %load.struct469, 1
  %tuple.field472 = getelementptr inbounds nuw %_Z14JsonWriteFrame, ptr %tuple471, i32 0, i32 0
  store %_Z5SliceI9JsonValueE %items468, ptr %tuple.field472, align 1
  %tuple.field473 = getelementptr inbounds nuw %_Z14JsonWriteFrame, ptr %tuple471, i32 0, i32 1
  store %_Z5SliceI10JsonMemberE %members470, ptr %tuple.field473, align 1
  %tuple.field474 = getelementptr inbounds nuw %_Z14JsonWriteFrame, ptr %tuple471, i32 0, i32 2
  store i64 %add440, ptr %tuple.field474, align 1
  %tuple.val475 = load %_Z14JsonWriteFrame, ptr %tuple471, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %arg.tmp476, ptr align 1 %tuple471, i64 ptrtoint (ptr getelementptr (%_Z14JsonWriteFrame, ptr null, i32 1) to i64), i1 false)
  call void @_ZN5ArrayI14JsonWriteFrameB4jsonE3putEm14JsonWriteFrameB4json(ptr %open466, i64 %sub, ptr %arg.tmp476)
  store i1 true, ptr %next, align 1
  br label %if.end454

if.else453:                                       ; preds = %if.then445
  %block477 = load ptr, ptr %block, align 8
  %p478 = load i64, ptr %p, align 8
  call void @_ZN5SliceI2u8E3putEm2u8(ptr %block477, i64 %p478, i8 125)
  %p479 = load i64, ptr %p, align 8
  %add480 = add i64 %p479, 1
  store i64 %add480, ptr %p, align 1
  %ptr.load = load ptr, ptr %open, align 8
  %length481 = getelementptr inbounds nuw %_Z5ArrayI14JsonWriteFrameE, ptr %ptr.load, i32 0, i32 0
  store i64 %sub, ptr %length481, align 8
  br label %if.end454

if.end454:                                        ; preds = %if.else453, %if.then452
  br label %if.end447

if.then486:                                       ; preds = %if.else446
  %block489 = load ptr, ptr %block, align 8
  %p490 = load i64, ptr %p, align 8
  call void @_ZN5SliceI2u8E3putEm2u8(ptr %block489, i64 %p490, i8 44)
  %p491 = load i64, ptr %p, align 8
  %add492 = add i64 %p491, 1
  store i64 %add492, ptr %p, align 1
  %field.inplace494 = getelementptr inbounds nuw %_Z14JsonWriteFrame, ptr %sret.result437, i32 0, i32 0
  call void @_ZN5SliceI9JsonValueB4jsonEixEm(ptr noalias sret(%_Z9JsonValue) %sret.result493, ptr %field.inplace494, i64 %add440)
  %set.load495 = load %_Z9JsonValue, ptr %sret.result493, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %cur, ptr align 1 %sret.result493, i64 ptrtoint (ptr getelementptr (%_Z9JsonValue, ptr null, i32 1) to i64), i1 false)
  %open496 = load ptr, ptr %open, align 8
  %load.struct497 = load %_Z14JsonWriteFrame, ptr %sret.result437, align 8
  %items498 = extractvalue %_Z14JsonWriteFrame %load.struct497, 0
  %load.struct499 = load %_Z14JsonWriteFrame, ptr %sret.result437, align 8
  %members500 = extractvalue %_Z14JsonWriteFrame %load.struct499, 1
  %tuple.field502 = getelementptr inbounds nuw %_Z14JsonWriteFrame, ptr %tuple501, i32 0, i32 0
  store %_Z5SliceI9JsonValueE %items498, ptr %tuple.field502, align 1
  %tuple.field503 = getelementptr inbounds nuw %_Z14JsonWriteFrame, ptr %tuple501, i32 0, i32 1
  store %_Z5SliceI10JsonMemberE %members500, ptr %tuple.field503, align 1
  %tuple.field504 = getelementptr inbounds nuw %_Z14JsonWriteFrame, ptr %tuple501, i32 0, i32 2
  store i64 %add440, ptr %tuple.field504, align 1
  %tuple.val505 = load %_Z14JsonWriteFrame, ptr %tuple501, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %arg.tmp506, ptr align 1 %tuple501, i64 ptrtoint (ptr getelementptr (%_Z14JsonWriteFrame, ptr null, i32 1) to i64), i1 false)
  call void @_ZN5ArrayI14JsonWriteFrameB4jsonE3putEm14JsonWriteFrameB4json(ptr %open496, i64 %sub, ptr %arg.tmp506)
  store i1 true, ptr %next, align 1
  br label %if.end488

if.else487:                                       ; preds = %if.else446
  %block507 = load ptr, ptr %block, align 8
  %p508 = load i64, ptr %p, align 8
  call void @_ZN5SliceI2u8E3putEm2u8(ptr %block507, i64 %p508, i8 93)
  %p509 = load i64, ptr %p, align 8
  %add510 = add i64 %p509, 1
  store i64 %add510, ptr %p, align 1
  %ptr.load511 = load ptr, ptr %open, align 8
  %length512 = getelementptr inbounds nuw %_Z5ArrayI14JsonWriteFrameE, ptr %ptr.load511, i32 0, i32 0
  store i64 %sub, ptr %length512, align 8
  br label %if.end488

if.end488:                                        ; preds = %if.else487, %if.then486
  br label %if.end447

if.then515:                                       ; preds = %while.exit407
  br label %repeat.exit

if.end516:                                        ; preds = %while.exit407
  br label %repeat.body
}

define linkonce_odr void @_ZN10JsonWriterB4json3rawE5SliceI2u8E(ptr %0, ptr %1) {
entry:
  call void @_ZN10JsonWriterB4json8separateEv(ptr %0)
  call void @_ZN10JsonWriterB4json6appendE5SliceI2u8E(ptr %0, ptr %1)
  ret void
}

define linkonce_odr i1 @_ZN10JsonWriterB4json5plainE2u8(i8 %0) {
entry:
  %ge = icmp uge i8 %0, 32
  br i1 %ge, label %lor.rhs, label %lor.end

lor.rhs:                                          ; preds = %entry
  %ne = icmp ne i8 %0, 34
  br label %lor.end

lor.end:                                          ; preds = %lor.rhs, %entry
  %lor.result = phi i1 [ false, %entry ], [ %ne, %lor.rhs ]
  br i1 %lor.result, label %lor.rhs1, label %lor.end2

lor.rhs1:                                         ; preds = %lor.end
  %ne3 = icmp ne i8 %0, 92
  br label %lor.end2

lor.end2:                                         ; preds = %lor.rhs1, %lor.end
  %lor.result4 = phi i1 [ false, %lor.end ], [ %ne3, %lor.rhs1 ]
  ret i1 %lor.result4
}

define linkonce_odr void @_ZN10JsonWriterB4json11quoted_restE5SliceI2u8Em(ptr %0, ptr %1, i64 %2) {
entry:
  %sret.result = alloca %_Z5SliceI2u8E, align 8
  %j = alloca i64, align 8
  %load.struct = load %_Z5SliceI2u8E, ptr %1, align 8
  %length = extractvalue %_Z5SliceI2u8E %load.struct, 0
  %i = alloca i64, align 8
  store i64 %2, ptr %i, align 1
  br label %while.cond

while.cond:                                       ; preds = %if.end17, %entry
  %i1 = load i64, ptr %i, align 8
  %lt = icmp ult i64 %i1, %length
  br i1 %lt, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %i2 = load i64, ptr %i, align 8
  store i64 %i2, ptr %j, align 1
  br label %while.cond3

while.exit:                                       ; preds = %if.then16, %while.cond
  call void @_ZN10JsonWriterB4json4byteE2u8(ptr %0, i8 34)
  ret void

while.cond3:                                      ; preds = %while.body4, %while.body
  %j6 = load i64, ptr %j, align 8
  %lt7 = icmp ult i64 %j6, %length
  br i1 %lt7, label %lor.rhs, label %lor.end

while.body4:                                      ; preds = %lor.end
  %j10 = load i64, ptr %j, align 8
  %add = add i64 %j10, 1
  store i64 %add, ptr %j, align 1
  br label %while.cond3

while.exit5:                                      ; preds = %lor.end
  %j11 = load i64, ptr %j, align 8
  %i12 = load i64, ptr %i, align 8
  %gt = icmp ugt i64 %j11, %i12
  br i1 %gt, label %if.then, label %if.end

lor.rhs:                                          ; preds = %while.cond3
  %j8 = load i64, ptr %j, align 8
  %call = call i8 @_ZN5SliceI2u8EixEm(ptr %1, i64 %j8)
  %call9 = call i1 @_ZN10JsonWriterB4json5plainE2u8(i8 %call)
  br label %lor.end

lor.end:                                          ; preds = %lor.rhs, %while.cond3
  %lor.result = phi i1 [ false, %while.cond3 ], [ %call9, %lor.rhs ]
  br i1 %lor.result, label %while.body4, label %while.exit5

if.then:                                          ; preds = %while.exit5
  %i13 = load i64, ptr %i, align 8
  %j14 = load i64, ptr %j, align 8
  call void @_ZN5SliceI2u8E8subsliceEmm(ptr noalias sret(%_Z5SliceI2u8E) %sret.result, ptr %1, i64 %i13, i64 %j14)
  call void @_ZN10JsonWriterB4json6appendE5SliceI2u8E(ptr %0, ptr %sret.result)
  br label %if.end

if.end:                                           ; preds = %if.then, %while.exit5
  %j15 = load i64, ptr %j, align 8
  %ge = icmp uge i64 %j15, %length
  br i1 %ge, label %if.then16, label %if.end17

if.then16:                                        ; preds = %if.end
  br label %while.exit

if.end17:                                         ; preds = %if.end
  %j18 = load i64, ptr %j, align 8
  %call19 = call i8 @_ZN5SliceI2u8EixEm(ptr %1, i64 %j18)
  call void @_ZN10JsonWriterB4json6escapeE2u8(ptr %0, i8 %call19)
  %j20 = load i64, ptr %j, align 8
  %add21 = add i64 %j20, 1
  store i64 %add21, ptr %i, align 1
  br label %while.cond
}

define linkonce_odr void @_ZN10JsonWriterB4json6escapeE2u8(ptr %0, i8 %1) {
entry:
  %arg.tmp = alloca %_Z5SliceI2u8E, align 8
  %tuple = alloca %_Z5SliceI2u8E, align 8
  call void @_ZN10JsonWriterB4json4byteE2u8(ptr %0, i8 92)
  %call = call i8 @_ZN10JsonWriterB4json13escape_letterE2u8(i8 %1)
  %ne = icmp ne i8 %call, 0
  br i1 %ne, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  call void @_ZN10JsonWriterB4json4byteE2u8(ptr %0, i8 %call)
  br label %if.end

if.else:                                          ; preds = %entry
  %tuple.field = getelementptr inbounds nuw %_Z5SliceI2u8E, ptr %tuple, i32 0, i32 0
  store i64 3, ptr %tuple.field, align 1
  %tuple.field1 = getelementptr inbounds nuw %_Z5SliceI2u8E, ptr %tuple, i32 0, i32 1
  store ptr @.str.80, ptr %tuple.field1, align 1
  %tuple.val = load %_Z5SliceI2u8E, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %arg.tmp, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z5SliceI2u8E, ptr null, i32 1) to i64), i1 false)
  call void @_ZN10JsonWriterB4json6appendE5SliceI2u8E(ptr %0, ptr %arg.tmp)
  %zext = zext i8 %1 to i64
  %lshr = lshr i64 %zext, 4
  %call2 = call i8 @_ZN10JsonWriterB4json9hex_digitE2u8(i64 %lshr)
  call void @_ZN10JsonWriterB4json4byteE2u8(ptr %0, i8 %call2)
  %and = and i8 %1, 15
  %call3 = call i8 @_ZN10JsonWriterB4json9hex_digitE2u8(i8 %and)
  call void @_ZN10JsonWriterB4json4byteE2u8(ptr %0, i8 %call3)
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  ret void
}

define linkonce_odr i8 @_ZN10JsonWriterB4json13escape_letterE2u8(i8 %0) {
entry:
  %match.cmp = icmp eq i8 %0, 34
  br i1 %match.cmp, label %match.case, label %match.next

match.end:                                        ; No predecessors!
  ret i8 0

match.case:                                       ; preds = %entry
  ret i8 34

match.next:                                       ; preds = %entry
  %match.cmp3 = icmp eq i8 %0, 92
  br i1 %match.cmp3, label %match.case1, label %match.next2

match.case1:                                      ; preds = %match.next
  ret i8 92

match.next2:                                      ; preds = %match.next
  %match.cmp6 = icmp eq i8 %0, 8
  br i1 %match.cmp6, label %match.case4, label %match.next5

match.case4:                                      ; preds = %match.next2
  ret i8 98

match.next5:                                      ; preds = %match.next2
  %match.cmp9 = icmp eq i8 %0, 12
  br i1 %match.cmp9, label %match.case7, label %match.next8

match.case7:                                      ; preds = %match.next5
  ret i8 102

match.next8:                                      ; preds = %match.next5
  %match.cmp12 = icmp eq i8 %0, 10
  br i1 %match.cmp12, label %match.case10, label %match.next11

match.case10:                                     ; preds = %match.next8
  ret i8 110

match.next11:                                     ; preds = %match.next8
  %match.cmp15 = icmp eq i8 %0, 13
  br i1 %match.cmp15, label %match.case13, label %match.next14

match.case13:                                     ; preds = %match.next11
  ret i8 114

match.next14:                                     ; preds = %match.next11
  %match.cmp18 = icmp eq i8 %0, 9
  br i1 %match.cmp18, label %match.case16, label %match.next17

match.case16:                                     ; preds = %match.next14
  ret i8 116

match.next17:                                     ; preds = %match.next14
  ret i8 0
}

define linkonce_odr i8 @_ZN10JsonWriterB4json9hex_digitE2u8(i8 %0) {
entry:
  %lt = icmp ult i8 %0, 10
  br i1 %lt, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %add = add i8 %0, 48
  ret i8 %add

if.end:                                           ; preds = %entry
  %add1 = add i8 %0, 87
  ret i8 %add1
}

define linkonce_odr void @_ZN10JsonWriterB4json13append_quotedER13StringBuilder5SliceI2u8E(ptr %0, ptr %1) {
entry:
  %arg.tmp = alloca { ptr }, align 8
  call void @_ZN13StringBuilder6appendEc(ptr %0, i8 34)
  %i = alloca i64, align 8
  store i64 0, ptr %i, align 1
  br label %while.cond

while.cond:                                       ; preds = %if.end, %entry
  %i1 = load i64, ptr %i, align 8
  %load.struct = load %_Z5SliceI2u8E, ptr %1, align 8
  %length = extractvalue %_Z5SliceI2u8E %load.struct, 0
  %lt = icmp ult i64 %i1, %length
  br i1 %lt, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %i2 = load i64, ptr %i, align 8
  %call = call i8 @_ZN5SliceI2u8EixEm(ptr %1, i64 %i2)
  %call3 = call i1 @_ZN10JsonWriterB4json5plainE2u8(i8 %call)
  br i1 %call3, label %if.then, label %if.else

while.exit:                                       ; preds = %while.cond
  call void @_ZN13StringBuilder6appendEc(ptr %0, i8 34)
  ret void

if.then:                                          ; preds = %while.body
  call void @_ZN13StringBuilder6appendEc(ptr %0, i8 %call)
  br label %if.end

if.else:                                          ; preds = %while.body
  call void @_ZN13StringBuilder6appendEc(ptr %0, i8 92)
  %call4 = call i8 @_ZN10JsonWriterB4json13escape_letterE2u8(i8 %call)
  %ne = icmp ne i8 %call4, 0
  br i1 %ne, label %if.then5, label %if.else6

if.end:                                           ; preds = %if.end7, %if.then
  %i10 = load i64, ptr %i, align 8
  %add = add i64 %i10, 1
  store i64 %add, ptr %i, align 1
  br label %while.cond

if.then5:                                         ; preds = %if.else
  call void @_ZN13StringBuilder6appendEc(ptr %0, i8 %call4)
  br label %if.end7

if.else6:                                         ; preds = %if.else
  store { ptr } { ptr @.sconst }, ptr %arg.tmp, align 1
  call void @_ZN13StringBuilder6appendE6String(ptr %0, ptr %arg.tmp)
  %zext = zext i8 %call to i64
  %lshr = lshr i64 %zext, 4
  %call8 = call i8 @_ZN10JsonWriterB4json9hex_digitE2u8(i64 %lshr)
  call void @_ZN13StringBuilder6appendEc(ptr %0, i8 %call8)
  %and = and i8 %call, 15
  %call9 = call i8 @_ZN10JsonWriterB4json9hex_digitE2u8(i8 %and)
  call void @_ZN13StringBuilder6appendEc(ptr %0, i8 %call9)
  br label %if.end7

if.end7:                                          ; preds = %if.else6, %if.then5
  br label %if.end
}

define linkonce_odr void @_ZN10JsonWriterB4json5quoteEPN4scaly6memory4PageE5SliceI2u8E(ptr noalias sret({ ptr }) %0, ptr %1, ptr %2) {
entry:
  %sret.result = alloca { ptr }, align 8
  %sb = alloca ptr, align 8
  %frame = alloca { ptr, ptr }, align 8
  store ptr null, ptr %frame, align 8
  %frame.parent = getelementptr inbounds nuw { ptr, ptr }, ptr %frame, i32 0, i32 1
  store ptr %1, ptr %frame.parent, align 8
  %frame.page = load ptr, ptr %frame, align 8
  %frame.has_page = icmp ne ptr %frame.page, null
  br i1 %frame.has_page, label %frame.forced, label %frame.force

frame.force:                                      ; preds = %entry
  %forced_page = call ptr @_Z17scaly_force_frameP5Frame(ptr %frame)
  br label %frame.forced

frame.forced:                                     ; preds = %frame.force, %entry
  %forced_page1 = phi ptr [ %frame.page, %entry ], [ %forced_page, %frame.force ]
  %struct.region = call ptr @_ZN4Page8allocateEmm(ptr %forced_page1, i64 64, i64 ptrtoint (ptr getelementptr ({ i1, ptr }, ptr null, i64 0, i32 1) to i64))
  call void @_ZN13StringBuilderC1Ev(ptr %struct.region)
  store ptr %struct.region, ptr %sb, align 1
  %sb2 = load ptr, ptr %sb, align 8
  call void @_ZN10JsonWriterB4json13append_quotedER13StringBuilder5SliceI2u8E(ptr %sb2, ptr %2)
  %sb3 = load ptr, ptr %sb, align 8
  call void @_ZN13StringBuilder9to_stringEPN4scaly6memory4PageE(ptr noalias sret({ ptr }) %sret.result, ptr %1, ptr %sb3)
  %sret.body = load { ptr }, ptr %sret.result, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %sret.result, i64 ptrtoint (ptr getelementptr ({ ptr }, ptr null, i32 1) to i64), i1 false)
  call void @_Z19scaly_release_frameP5Frame(ptr %frame)
  ret void
}

declare void @_ZN6String8as_sliceEv(ptr noalias sret(%_Z5SliceI2u8E), ptr)

define linkonce_odr void @_ZN10JsonWriterB4json5quoteEPN4scaly6memory4PageE6String(ptr noalias sret({ ptr }) %0, ptr %1, ptr %2) {
entry:
  %sret.result = alloca { ptr }, align 8
  %sret.result1 = alloca %_Z5SliceI2u8E, align 8
  call void @_ZN6String8as_sliceEv(ptr noalias sret(%_Z5SliceI2u8E) %sret.result1, ptr %2)
  call void @_ZN10JsonWriterB4json5quoteEPN4scaly6memory4PageE5SliceI2u8E(ptr noalias sret({ ptr }) %sret.result, ptr %1, ptr %sret.result1)
  %sret.body = load { ptr }, ptr %sret.result, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %sret.result, i64 ptrtoint (ptr getelementptr ({ ptr }, ptr null, i32 1) to i64), i1 false)
  ret void
}

declare void @_ZN13StringBuilder6appendE6String(ptr, ptr)

define linkonce_odr void @_ZN10JsonWriterB4json12append_valueER13StringBuilder9JsonValueB4json(ptr %0, ptr %1) {
entry:
  %arg.tmp40 = alloca %_Z5SliceI10JsonMemberE, align 8
  %sret.result38 = alloca %_Z10JsonMember, align 8
  %arg.tmp22 = alloca %_Z5SliceI9JsonValueE, align 8
  %sret.result = alloca %_Z9JsonValue, align 8
  %i = alloca i64, align 8
  %arg.tmp12 = alloca %_Z5SliceI2u8E, align 8
  %arg.tmp4 = alloca { ptr }, align 8
  %arg.tmp3 = alloca { ptr }, align 8
  %arg.tmp = alloca { ptr }, align 8
  %tag.ptr = getelementptr inbounds nuw %_Z9JsonValue, ptr %1, i32 0, i32 0
  %tag = load i8, ptr %tag.ptr, align 1
  switch i8 %tag, label %choose.else [
    i8 0, label %choose.when
    i8 1, label %choose.when1
    i8 2, label %choose.when5
    i8 3, label %choose.when9
    i8 4, label %choose.when13
    i8 5, label %choose.when25
  ]

choose.end:                                       ; preds = %choose.else, %while.exit30, %while.exit, %choose.when9, %choose.when5, %if.end, %choose.when
  ret void

choose.else:                                      ; preds = %entry
  br label %choose.end

choose.when:                                      ; preds = %entry
  %"variant.c_data().ptr" = getelementptr inbounds nuw %_Z9JsonValue, ptr %1, i32 0, i32 1
  store { ptr } { ptr @.sconst.81 }, ptr %arg.tmp, align 1
  call void @_ZN13StringBuilder6appendE6String(ptr %0, ptr %arg.tmp)
  br label %choose.end

choose.when1:                                     ; preds = %entry
  %"variant.c_data().ptr2" = getelementptr inbounds nuw %_Z9JsonValue, ptr %1, i32 0, i32 1
  %variant.val = load i1, ptr %"variant.c_data().ptr2", align 1
  br i1 %variant.val, label %if.then, label %if.else

if.then:                                          ; preds = %choose.when1
  store { ptr } { ptr @.sconst.82 }, ptr %arg.tmp3, align 1
  call void @_ZN13StringBuilder6appendE6String(ptr %0, ptr %arg.tmp3)
  br label %if.end

if.else:                                          ; preds = %choose.when1
  store { ptr } { ptr @.sconst.83 }, ptr %arg.tmp4, align 1
  call void @_ZN13StringBuilder6appendE6String(ptr %0, ptr %arg.tmp4)
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  br label %choose.end

choose.when5:                                     ; preds = %entry
  %"variant.c_data().ptr6" = getelementptr inbounds nuw %_Z9JsonValue, ptr %1, i32 0, i32 1
  %variant.val7 = load %_Z10JsonNumber, ptr %"variant.c_data().ptr6", align 8
  %text = extractvalue %_Z10JsonNumber %variant.val7, 0
  %data = extractvalue %_Z5SliceI2u8E %text, 1
  %text8 = extractvalue %_Z10JsonNumber %variant.val7, 0
  %length = extractvalue %_Z5SliceI2u8E %text8, 0
  call void @_ZN13StringBuilder6appendEPcm(ptr %0, ptr %data, i64 %length)
  br label %choose.end

choose.when9:                                     ; preds = %entry
  %"variant.c_data().ptr10" = getelementptr inbounds nuw %_Z9JsonValue, ptr %1, i32 0, i32 1
  %variant.val11 = load %_Z10JsonString, ptr %"variant.c_data().ptr10", align 8
  %bytes = extractvalue %_Z10JsonString %variant.val11, 0
  store %_Z5SliceI2u8E %bytes, ptr %arg.tmp12, align 1
  call void @_ZN10JsonWriterB4json13append_quotedER13StringBuilder5SliceI2u8E(ptr %0, ptr %arg.tmp12)
  br label %choose.end

choose.when13:                                    ; preds = %entry
  %"variant.c_data().ptr14" = getelementptr inbounds nuw %_Z9JsonValue, ptr %1, i32 0, i32 1
  %variant.val15 = load %_Z9JsonArray, ptr %"variant.c_data().ptr14", align 8
  call void @_ZN13StringBuilder6appendEc(ptr %0, i8 91)
  store i64 0, ptr %i, align 1
  br label %while.cond

while.cond:                                       ; preds = %if.end20, %choose.when13
  %i16 = load i64, ptr %i, align 8
  %items = extractvalue %_Z9JsonArray %variant.val15, 0
  %length17 = extractvalue %_Z5SliceI9JsonValueE %items, 0
  %lt = icmp ult i64 %i16, %length17
  br i1 %lt, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %i18 = load i64, ptr %i, align 8
  %gt = icmp ugt i64 %i18, 0
  br i1 %gt, label %if.then19, label %if.end20

while.exit:                                       ; preds = %while.cond
  call void @_ZN13StringBuilder6appendEc(ptr %0, i8 93)
  br label %choose.end

if.then19:                                        ; preds = %while.body
  call void @_ZN13StringBuilder6appendEc(ptr %0, i8 44)
  br label %if.end20

if.end20:                                         ; preds = %if.then19, %while.body
  %items21 = extractvalue %_Z9JsonArray %variant.val15, 0
  store %_Z5SliceI9JsonValueE %items21, ptr %arg.tmp22, align 1
  %i23 = load i64, ptr %i, align 8
  call void @_ZN5SliceI9JsonValueB4jsonEixEm(ptr noalias sret(%_Z9JsonValue) %sret.result, ptr %arg.tmp22, i64 %i23)
  call void @_ZN10JsonWriterB4json12append_valueER13StringBuilder9JsonValueB4json(ptr %0, ptr %sret.result)
  %i24 = load i64, ptr %i, align 8
  %add = add i64 %i24, 1
  store i64 %add, ptr %i, align 1
  br label %while.cond

choose.when25:                                    ; preds = %entry
  %"variant.c_data().ptr26" = getelementptr inbounds nuw %_Z9JsonValue, ptr %1, i32 0, i32 1
  %variant.val27 = load %_Z10JsonObject, ptr %"variant.c_data().ptr26", align 8
  call void @_ZN13StringBuilder6appendEc(ptr %0, i8 123)
  store i64 0, ptr %i, align 1
  br label %while.cond28

while.cond28:                                     ; preds = %if.end37, %choose.when25
  %i31 = load i64, ptr %i, align 8
  %members = extractvalue %_Z10JsonObject %variant.val27, 0
  %length32 = extractvalue %_Z5SliceI10JsonMemberE %members, 0
  %lt33 = icmp ult i64 %i31, %length32
  br i1 %lt33, label %while.body29, label %while.exit30

while.body29:                                     ; preds = %while.cond28
  %i34 = load i64, ptr %i, align 8
  %gt35 = icmp ugt i64 %i34, 0
  br i1 %gt35, label %if.then36, label %if.end37

while.exit30:                                     ; preds = %while.cond28
  call void @_ZN13StringBuilder6appendEc(ptr %0, i8 125)
  br label %choose.end

if.then36:                                        ; preds = %while.body29
  call void @_ZN13StringBuilder6appendEc(ptr %0, i8 44)
  br label %if.end37

if.end37:                                         ; preds = %if.then36, %while.body29
  %members39 = extractvalue %_Z10JsonObject %variant.val27, 0
  store %_Z5SliceI10JsonMemberE %members39, ptr %arg.tmp40, align 1
  %i41 = load i64, ptr %i, align 8
  call void @_ZN5SliceI10JsonMemberB4jsonEixEm(ptr noalias sret(%_Z10JsonMember) %sret.result38, ptr %arg.tmp40, i64 %i41)
  %field.inplace = getelementptr inbounds nuw %_Z10JsonMember, ptr %sret.result38, i32 0, i32 0
  call void @_ZN10JsonWriterB4json13append_quotedER13StringBuilder5SliceI2u8E(ptr %0, ptr %field.inplace)
  call void @_ZN13StringBuilder6appendEc(ptr %0, i8 58)
  %field.inplace42 = getelementptr inbounds nuw %_Z10JsonMember, ptr %sret.result38, i32 0, i32 1
  call void @_ZN10JsonWriterB4json12append_valueER13StringBuilder9JsonValueB4json(ptr %0, ptr %field.inplace42)
  %i43 = load i64, ptr %i, align 8
  %add44 = add i64 %i43, 1
  store i64 %add44, ptr %i, align 1
  br label %while.cond28
}

define linkonce_odr void @_ZN10JsonWriterB4json9stringifyEPN4scaly6memory4PageE9JsonValueB4json(ptr noalias sret({ ptr }) %0, ptr %1, ptr %2) {
entry:
  %sret.result = alloca { ptr }, align 8
  %sb = alloca ptr, align 8
  %frame = alloca { ptr, ptr }, align 8
  store ptr null, ptr %frame, align 8
  %frame.parent = getelementptr inbounds nuw { ptr, ptr }, ptr %frame, i32 0, i32 1
  store ptr %1, ptr %frame.parent, align 8
  %frame.page = load ptr, ptr %frame, align 8
  %frame.has_page = icmp ne ptr %frame.page, null
  br i1 %frame.has_page, label %frame.forced, label %frame.force

frame.force:                                      ; preds = %entry
  %forced_page = call ptr @_Z17scaly_force_frameP5Frame(ptr %frame)
  br label %frame.forced

frame.forced:                                     ; preds = %frame.force, %entry
  %forced_page1 = phi ptr [ %frame.page, %entry ], [ %forced_page, %frame.force ]
  %struct.region = call ptr @_ZN4Page8allocateEmm(ptr %forced_page1, i64 64, i64 ptrtoint (ptr getelementptr ({ i1, ptr }, ptr null, i64 0, i32 1) to i64))
  call void @_ZN13StringBuilderC1Ev(ptr %struct.region)
  store ptr %struct.region, ptr %sb, align 1
  %sb2 = load ptr, ptr %sb, align 8
  call void @_ZN10JsonWriterB4json12append_valueER13StringBuilder9JsonValueB4json(ptr %sb2, ptr %2)
  %sb3 = load ptr, ptr %sb, align 8
  call void @_ZN13StringBuilder9to_stringEPN4scaly6memory4PageE(ptr noalias sret({ ptr }) %sret.result, ptr %1, ptr %sb3)
  %sret.body = load { ptr }, ptr %sret.result, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %sret.result, i64 ptrtoint (ptr getelementptr ({ ptr }, ptr null, i32 1) to i64), i1 false)
  call void @_Z19scaly_release_frameP5Frame(ptr %frame)
  ret void
}

define linkonce_odr void @_ZN10JsonWriterB4jsonC1Ev(ptr %0) {
entry:
  %buffer = getelementptr inbounds nuw %_Z10JsonWriter, ptr %0, i32 0, i32 0
  %tuple.field = getelementptr inbounds nuw %_Z5ArrayI2u8E, ptr %buffer, i32 0, i32 0
  store i64 0, ptr %tuple.field, align 8
  %tuple.field1 = getelementptr inbounds nuw %_Z5ArrayI2u8E, ptr %buffer, i32 0, i32 1
  store i64 0, ptr %tuple.field1, align 8
  %tuple.field2 = getelementptr inbounds nuw %_Z5ArrayI2u8E, ptr %buffer, i32 0, i32 2
  store ptr null, ptr %tuple.field2, align 8
  %first = getelementptr inbounds nuw %_Z10JsonWriter, ptr %0, i32 0, i32 1
  store i1 true, ptr %first, align 1
  %after_key = getelementptr inbounds nuw %_Z10JsonWriter, ptr %0, i32 0, i32 2
  store i1 false, ptr %after_key, align 1
  ret void
}

; Function Attrs: cold noinline noreturn
declare void @_Z15scaly_panic_oobP10const_charmm(...) #0

; Function Attrs: nocallback nofree nounwind willreturn memory(argmem: readwrite)
declare void @llvm.memcpy.p0.p0.i64(ptr noalias writeonly captures(none), ptr noalias readonly captures(none), i64, i1 immarg) #1

declare i32 @memcmp(...)

declare void @_Z19scaly_release_frameP5Frame(ptr)

declare i64 @_Z14trailing_zeros3u64(...)

declare ptr @_Z17scaly_force_frameP5Frame(ptr)

declare void @_ZN6StringC1EP10const_charm(ptr, ptr, i64)

declare void @_ZN13StringBuilderC1Ev(ptr)

declare void @_ZN6StringC1Ev(ptr)

; Function Attrs: cold noinline noreturn
declare void @_Z16scaly_panic_sizeP10const_charmm(...) #0

declare ptr @_Z3getPv(ptr)

attributes #0 = { cold noinline noreturn }
attributes #1 = { nocallback nofree nounwind willreturn memory(argmem: readwrite) }
