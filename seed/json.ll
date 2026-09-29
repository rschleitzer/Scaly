; ModuleID = 'json'
source_filename = "json"
target datalayout = "e-m:o-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-n32:64-S128-Fn32"

%_Z5SliceI2u8E = type { i64, ptr }
%_Z13SliceIteratorI2u8E = type { %_Z5SliceI2u8E, i64 }
%_Z10JsonReader = type { %_Z5SliceI2u8E, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i1, i1, i64, i64 }
%_Z9JsonToken = type { i8, [0 x i8] }
%_Z5SliceI1TE = type { i64, ptr }
%_Z13SliceIteratorI1TE = type { %_Z5SliceI1TE, i64 }
%_Z5SliceIcE = type { i64, ptr }
%_Z13SliceIteratorIcE = type { %_Z5SliceIcE, i64 }
%_Z5SliceI10JsonMemberE = type { i64, ptr }
%_Z10JsonMember = type { { ptr }, %_Z9JsonValue }
%_Z9JsonValue = type { i8, [16 x i8] }
%_Z13SliceIteratorI10JsonMemberE = type { %_Z5SliceI10JsonMemberE, i64 }
%_Z5SliceI9JsonValueE = type { i64, ptr }
%_Z13SliceIteratorI9JsonValueE = type { %_Z5SliceI9JsonValueE, i64 }
%_Z9JsonArray = type { %_Z5SliceI9JsonValueE }
%_Z10JsonObject = type { %_Z5SliceI10JsonMemberE }
%_Z10JsonParsed = type { %_Z9JsonValue, i64, i64 }
%_Z12ListIteratorI10JsonMemberE = type { ptr }
%_Z4ListI10JsonMemberE = type { ptr, ptr }
%_Z6VectorI10JsonMemberE = type { i64, ptr }
%_Z12ListIteratorI9JsonValueE = type { ptr }
%_Z4ListI9JsonValueE = type { ptr, ptr }
%_Z6VectorI9JsonValueE = type { i64, ptr }
%_Z4NodeI10JsonMemberE = type { %_Z10JsonMember, ptr }
%_Z14VectorIteratorI10JsonMemberE = type { ptr, i64 }
%_Z5ArrayI10JsonMemberE = type { i64, i64, ptr }
%_Z13ArrayIteratorI10JsonMemberE = type { ptr, i64 }
%_Z4NodeI9JsonValueE = type { %_Z9JsonValue, ptr }
%_Z14VectorIteratorI9JsonValueE = type { ptr, i64 }
%_Z5ArrayI9JsonValueE = type { i64, i64, ptr }
%_Z13ArrayIteratorI9JsonValueE = type { ptr, i64 }
%_Z5ArrayI2u8E = type { i64, i64, ptr }
%_Z6VectorI2u8E = type { i64, ptr }
%_Z14VectorIteratorI2u8E = type { ptr, i64 }
%_Z4ListI2u8E = type { ptr, ptr }
%_Z4NodeI2u8E = type { i8, ptr }
%_Z12ListIteratorI2u8E = type { ptr }
%_Z13ArrayIteratorI2u8E = type { ptr, i64 }
%_Z10JsonWriter = type { %_Z5ArrayI2u8E, i1, i1 }

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
@.str = private unnamed_addr constant [9 x i8] c"Slice.at\00", align 1
@.str.1 = private unnamed_addr constant [10 x i8] c"Slice.put\00", align 1
@.str.2 = private unnamed_addr constant [8 x i8] c"Slice[]\00", align 1
@.str.3 = private unnamed_addr constant [9 x i8] c"Slice.at\00", align 1
@.str.4 = private unnamed_addr constant [10 x i8] c"Slice.put\00", align 1
@.str.5 = private unnamed_addr constant [9 x i8] c"no error\00", align 1
@.str.6 = private unnamed_addr constant [31 x i8] c"unexpected end of the document\00", align 1
@.str.7 = private unnamed_addr constant [38 x i8] c"characters after the document's value\00", align 1
@.str.8 = private unnamed_addr constant [21 x i8] c"a value was expected\00", align 1
@.str.9 = private unnamed_addr constant [38 x i8] c"a member name (a string) was expected\00", align 1
@.str.10 = private unnamed_addr constant [39 x i8] c"':' was expected after the member name\00", align 1
@.str.11 = private unnamed_addr constant [40 x i8] c"',' or the closing bracket was expected\00", align 1
@.str.12 = private unnamed_addr constant [17 x i8] c"malformed number\00", align 1
@.str.13 = private unnamed_addr constant [34 x i8] c"raw control character in a string\00", align 1
@.str.14 = private unnamed_addr constant [27 x i8] c"invalid escape in a string\00", align 1
@.str.15 = private unnamed_addr constant [17 x i8] c"nesting too deep\00", align 1
@.str.16 = private unnamed_addr constant [14 x i8] c"unknown error\00", align 1
@.str.17 = private unnamed_addr constant [5 x i8] c"true\00", align 1
@.str.18 = private unnamed_addr constant [6 x i8] c"false\00", align 1
@.str.19 = private unnamed_addr constant [5 x i8] c"null\00", align 1
@.str.20 = private unnamed_addr constant [9 x i8] c"Slice.at\00", align 1
@.str.21 = private unnamed_addr constant [10 x i8] c"Slice.put\00", align 1
@.str.22 = private unnamed_addr constant [9 x i8] c"Slice.at\00", align 1
@.str.23 = private unnamed_addr constant [10 x i8] c"Slice.put\00", align 1
@.str.24 = private unnamed_addr constant [8 x i8] c"Slice[]\00", align 1
@.str.25 = private unnamed_addr constant [8 x i8] c"Slice[]\00", align 1
@.str.26 = private unnamed_addr constant [10 x i8] c"Vector.at\00", align 1
@.str.27 = private unnamed_addr constant [11 x i8] c"Vector.put\00", align 1
@.str.28 = private unnamed_addr constant [10 x i8] c"Array.add\00", align 1
@.str.29 = private unnamed_addr constant [9 x i8] c"Array.at\00", align 1
@.str.30 = private unnamed_addr constant [10 x i8] c"Array.put\00", align 1
@.str.31 = private unnamed_addr constant [10 x i8] c"Vector.at\00", align 1
@.str.32 = private unnamed_addr constant [11 x i8] c"Vector.put\00", align 1
@.str.33 = private unnamed_addr constant [10 x i8] c"Array.add\00", align 1
@.str.34 = private unnamed_addr constant [9 x i8] c"Array.at\00", align 1
@.str.35 = private unnamed_addr constant [10 x i8] c"Array.put\00", align 1
@.str.36 = private unnamed_addr constant [10 x i8] c"Vector.at\00", align 1
@.str.37 = private unnamed_addr constant [11 x i8] c"Vector.put\00", align 1
@.str.38 = private unnamed_addr constant [10 x i8] c"Array.add\00", align 1
@.str.39 = private unnamed_addr constant [9 x i8] c"Array.at\00", align 1
@.str.40 = private unnamed_addr constant [10 x i8] c"Array.put\00", align 1
@.str.41 = private unnamed_addr constant [5 x i8] c"true\00", align 1
@.str.42 = private unnamed_addr constant [6 x i8] c"false\00", align 1
@.str.43 = private unnamed_addr constant [5 x i8] c"null\00", align 1
@.str.44 = private unnamed_addr constant [4 x i8] c"u00\00", align 1
@.sconst = private constant [5 x i8] c"\03u00\00"
@.sconst.45 = private constant [6 x i8] c"\04null\00"
@.sconst.46 = private constant [6 x i8] c"\04true\00"
@.sconst.47 = private constant [7 x i8] c"\05false\00"

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

define linkonce_odr void @_ZN5SliceI2u8E8subsliceEPN4scaly6memory4PageEmm(ptr noalias sret(%_Z5SliceI2u8E) %0, ptr %1, ptr %2, i64 %3, i64 %4) {
entry:
  %tuple = alloca %_Z5SliceI2u8E, align 8
  %from = alloca i64, align 8
  store i64 %3, ptr %from, align 1
  %to = alloca i64, align 8
  store i64 %4, ptr %to, align 1
  %from1 = load i64, ptr %from, align 8
  %load.struct = load %_Z5SliceI2u8E, ptr %2, align 8
  %length = extractvalue %_Z5SliceI2u8E %load.struct, 0
  %gt = icmp ugt i64 %from1, %length
  br i1 %gt, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %load.struct2 = load %_Z5SliceI2u8E, ptr %2, align 8
  %length3 = extractvalue %_Z5SliceI2u8E %load.struct2, 0
  store i64 %length3, ptr %from, align 1
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %to4 = load i64, ptr %to, align 8
  %load.struct5 = load %_Z5SliceI2u8E, ptr %2, align 8
  %length6 = extractvalue %_Z5SliceI2u8E %load.struct5, 0
  %gt7 = icmp ugt i64 %to4, %length6
  br i1 %gt7, label %if.then8, label %if.end9

if.then8:                                         ; preds = %if.end
  %load.struct10 = load %_Z5SliceI2u8E, ptr %2, align 8
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
  %load.struct20 = load %_Z5SliceI2u8E, ptr %2, align 8
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
  call void @_ZN5SliceI2u8E8subsliceEPN4scaly6memory4PageEmm(ptr noalias sret(%_Z5SliceI2u8E) %sret.result, ptr null, ptr %2, i64 %3, i64 %field.val)
  %sret.body = load %_Z5SliceI2u8E, ptr %sret.result, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %sret.result, i64 ptrtoint (ptr getelementptr (%_Z5SliceI2u8E, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN5SliceI2u8E8slice_toEPN4scaly6memory4PageEm(ptr noalias sret(%_Z5SliceI2u8E) %0, ptr %1, ptr %2, i64 %3) {
entry:
  %sret.result = alloca %_Z5SliceI2u8E, align 8
  call void @_ZN5SliceI2u8E8subsliceEPN4scaly6memory4PageEmm(ptr noalias sret(%_Z5SliceI2u8E) %sret.result, ptr null, ptr %2, i64 0, i64 %3)
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
  call void @_ZN5SliceI2u8E8subsliceEPN4scaly6memory4PageEmm(ptr noalias sret(%_Z5SliceI2u8E) %sret.result, ptr null, ptr %0, i64 0, i64 %field.val)
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
  call void @_ZN5SliceI2u8E8subsliceEPN4scaly6memory4PageEmm(ptr noalias sret(%_Z5SliceI2u8E) %sret.result, ptr null, ptr %0, i64 %sub, i64 %field.val)
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

define linkonce_odr void @_ZN10JsonReader6createEPN4scaly6memory4PageE5SliceI2u8E(ptr noalias sret(%_Z10JsonReader) %0, ptr %1, ptr %2) {
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

define linkonce_odr void @_ZN10JsonReader14create_limitedEPN4scaly6memory4PageE5SliceI2u8Em(ptr noalias sret(%_Z10JsonReader) %0, ptr %1, ptr %2, i64 %3) {
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

define linkonce_odr void @_ZN10JsonReader7skip_wsEv(ptr %0) {
entry:
  br label %while.cond

while.cond:                                       ; preds = %if.end, %entry
  %load.struct = load %_Z10JsonReader, ptr %0, align 8
  %pos = extractvalue %_Z10JsonReader %load.struct, 7
  %load.struct1 = load %_Z10JsonReader, ptr %0, align 8
  %source = extractvalue %_Z10JsonReader %load.struct1, 0
  %length = extractvalue %_Z5SliceI2u8E %source, 0
  %lt = icmp ult i64 %pos, %length
  br i1 %lt, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %field.inplace = getelementptr inbounds nuw %_Z10JsonReader, ptr %0, i32 0, i32 0
  %field.inplace2 = getelementptr inbounds nuw %_Z10JsonReader, ptr %0, i32 0, i32 7
  %field.val = load i64, ptr %field.inplace2, align 8
  %call = call i8 @_ZN5SliceI2u8EixEm(ptr %field.inplace, i64 %field.val)
  %ne = icmp ne i8 %call, 32
  br i1 %ne, label %land.rhs4, label %if.end

while.exit:                                       ; preds = %while.cond
  ret void

if.then:                                          ; preds = %land.rhs
  ret void

if.end:                                           ; preds = %land.rhs, %land.rhs3, %land.rhs4, %while.body
  %load.struct8 = load %_Z10JsonReader, ptr %0, align 8
  %pos9 = extractvalue %_Z10JsonReader %load.struct8, 7
  %add = add i64 %pos9, 1
  %pos10 = getelementptr inbounds nuw %_Z10JsonReader, ptr %0, i32 0, i32 7
  store i64 %add, ptr %pos10, align 8
  br label %while.cond

land.rhs:                                         ; preds = %land.rhs3
  %ne7 = icmp ne i8 %call, 9
  br i1 %ne7, label %if.then, label %if.end

land.rhs3:                                        ; preds = %land.rhs4
  %ne6 = icmp ne i8 %call, 13
  br i1 %ne6, label %land.rhs, label %if.end

land.rhs4:                                        ; preds = %while.body
  %ne5 = icmp ne i8 %call, 10
  br i1 %ne5, label %land.rhs3, label %if.end
}

define linkonce_odr void @_ZN10JsonReader4failEPN4scaly6memory4PageEi(ptr noalias sret(%_Z9JsonToken) %0, ptr %1, ptr %2, i64 %3) {
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

define linkonce_odr i1 @_ZN10JsonReader9in_objectEv(ptr %0) {
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

define linkonce_odr void @_ZN10JsonReader5closeEPN4scaly6memory4PageE9JsonToken(ptr noalias sret(%_Z9JsonToken) %0, ptr %1, ptr %2, ptr %3) {
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
  call void @_ZN10JsonReader11after_valueEv(ptr %2)
  %sret.body = load %_Z9JsonToken, ptr %3, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %3, i64 ptrtoint (ptr getelementptr (%_Z9JsonToken, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr i1 @_ZN10JsonReader11scan_stringEv(ptr %0) {
entry:
  %sret.result109 = alloca %_Z9JsonToken, align 8
  %sret.result104 = alloca %_Z9JsonToken, align 8
  %sret.result76 = alloca %_Z9JsonToken, align 8
  %k = alloca i64, align 8
  %sret.result55 = alloca %_Z9JsonToken, align 8
  %sret.result33 = alloca %_Z9JsonToken, align 8
  %sret.result = alloca %_Z9JsonToken, align 8
  %load.struct = load %_Z10JsonReader, ptr %0, align 8
  %pos = extractvalue %_Z10JsonReader %load.struct, 7
  %add = add i64 %pos, 1
  %pos1 = getelementptr inbounds nuw %_Z10JsonReader, ptr %0, i32 0, i32 7
  store i64 %add, ptr %pos1, align 8
  %load.struct2 = load %_Z10JsonReader, ptr %0, align 8
  %pos3 = extractvalue %_Z10JsonReader %load.struct2, 7
  %start = getelementptr inbounds nuw %_Z10JsonReader, ptr %0, i32 0, i32 9
  store i64 %pos3, ptr %start, align 8
  %escaped = getelementptr inbounds nuw %_Z10JsonReader, ptr %0, i32 0, i32 11
  store i1 false, ptr %escaped, align 1
  br label %while.cond

while.cond:                                       ; preds = %if.end19, %entry
  %load.struct4 = load %_Z10JsonReader, ptr %0, align 8
  %pos5 = extractvalue %_Z10JsonReader %load.struct4, 7
  %load.struct6 = load %_Z10JsonReader, ptr %0, align 8
  %source = extractvalue %_Z10JsonReader %load.struct6, 0
  %length = extractvalue %_Z5SliceI2u8E %source, 0
  %lt = icmp ult i64 %pos5, %length
  br i1 %lt, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %field.inplace = getelementptr inbounds nuw %_Z10JsonReader, ptr %0, i32 0, i32 0
  %field.inplace7 = getelementptr inbounds nuw %_Z10JsonReader, ptr %0, i32 0, i32 7
  %field.val = load i64, ptr %field.inplace7, align 8
  %call = call i8 @_ZN5SliceI2u8EixEm(ptr %field.inplace, i64 %field.val)
  %eq = icmp eq i8 %call, 34
  br i1 %eq, label %if.then, label %if.end

while.exit:                                       ; preds = %while.cond
  call void @_ZN10JsonReader4failEPN4scaly6memory4PageEi(ptr noalias sret(%_Z9JsonToken) %sret.result109, ptr null, ptr %0, i64 1)
  ret i1 false

if.then:                                          ; preds = %while.body
  %load.struct8 = load %_Z10JsonReader, ptr %0, align 8
  %pos9 = extractvalue %_Z10JsonReader %load.struct8, 7
  %end = getelementptr inbounds nuw %_Z10JsonReader, ptr %0, i32 0, i32 10
  store i64 %pos9, ptr %end, align 8
  %load.struct10 = load %_Z10JsonReader, ptr %0, align 8
  %pos11 = extractvalue %_Z10JsonReader %load.struct10, 7
  %add12 = add i64 %pos11, 1
  %pos13 = getelementptr inbounds nuw %_Z10JsonReader, ptr %0, i32 0, i32 7
  store i64 %add12, ptr %pos13, align 8
  ret i1 true

if.end:                                           ; preds = %while.body
  %lt14 = icmp ult i8 %call, 32
  br i1 %lt14, label %if.then15, label %if.end16

if.then15:                                        ; preds = %if.end
  call void @_ZN10JsonReader4failEPN4scaly6memory4PageEi(ptr noalias sret(%_Z9JsonToken) %sret.result, ptr null, ptr %0, i64 8)
  ret i1 false

if.end16:                                         ; preds = %if.end
  %eq17 = icmp eq i8 %call, 92
  br i1 %eq17, label %if.then18, label %if.else

if.then18:                                        ; preds = %if.end16
  %escaped20 = getelementptr inbounds nuw %_Z10JsonReader, ptr %0, i32 0, i32 11
  store i1 true, ptr %escaped20, align 1
  %load.struct21 = load %_Z10JsonReader, ptr %0, align 8
  %pos22 = extractvalue %_Z10JsonReader %load.struct21, 7
  %add23 = add i64 %pos22, 1
  %load.struct24 = load %_Z10JsonReader, ptr %0, align 8
  %source25 = extractvalue %_Z10JsonReader %load.struct24, 0
  %length26 = extractvalue %_Z5SliceI2u8E %source25, 0
  %ge = icmp uge i64 %add23, %length26
  br i1 %ge, label %if.then27, label %if.end28

if.else:                                          ; preds = %if.end16
  %load.struct105 = load %_Z10JsonReader, ptr %0, align 8
  %pos106 = extractvalue %_Z10JsonReader %load.struct105, 7
  %add107 = add i64 %pos106, 1
  %pos108 = getelementptr inbounds nuw %_Z10JsonReader, ptr %0, i32 0, i32 7
  store i64 %add107, ptr %pos108, align 8
  br label %if.end19

if.end19:                                         ; preds = %if.else, %if.end42
  br label %while.cond

if.then27:                                        ; preds = %if.then18
  %load.struct29 = load %_Z10JsonReader, ptr %0, align 8
  %pos30 = extractvalue %_Z10JsonReader %load.struct29, 7
  %add31 = add i64 %pos30, 1
  %pos32 = getelementptr inbounds nuw %_Z10JsonReader, ptr %0, i32 0, i32 7
  store i64 %add31, ptr %pos32, align 8
  call void @_ZN10JsonReader4failEPN4scaly6memory4PageEi(ptr noalias sret(%_Z9JsonToken) %sret.result33, ptr null, ptr %0, i64 1)
  ret i1 false

if.end28:                                         ; preds = %if.then18
  %field.inplace34 = getelementptr inbounds nuw %_Z10JsonReader, ptr %0, i32 0, i32 0
  %load.struct35 = load %_Z10JsonReader, ptr %0, align 8
  %pos36 = extractvalue %_Z10JsonReader %load.struct35, 7
  %add37 = add i64 %pos36, 1
  %call38 = call i8 @_ZN5SliceI2u8EixEm(ptr %field.inplace34, i64 %add37)
  %eq39 = icmp eq i8 %call38, 117
  br i1 %eq39, label %if.then40, label %if.else41

if.then40:                                        ; preds = %if.end28
  %load.struct43 = load %_Z10JsonReader, ptr %0, align 8
  %pos44 = extractvalue %_Z10JsonReader %load.struct43, 7
  %add45 = add i64 %pos44, 6
  %load.struct46 = load %_Z10JsonReader, ptr %0, align 8
  %source47 = extractvalue %_Z10JsonReader %load.struct46, 0
  %length48 = extractvalue %_Z5SliceI2u8E %source47, 0
  %gt = icmp ugt i64 %add45, %length48
  br i1 %gt, label %if.then49, label %if.end50

if.else41:                                        ; preds = %if.end28
  %match.cmp = icmp eq i8 %call38, 34
  br i1 %match.cmp, label %match.case, label %match.alt

if.end42:                                         ; preds = %match.end, %while.exit58
  br label %if.end19

if.then49:                                        ; preds = %if.then40
  %load.struct51 = load %_Z10JsonReader, ptr %0, align 8
  %source52 = extractvalue %_Z10JsonReader %load.struct51, 0
  %length53 = extractvalue %_Z5SliceI2u8E %source52, 0
  %pos54 = getelementptr inbounds nuw %_Z10JsonReader, ptr %0, i32 0, i32 7
  store i64 %length53, ptr %pos54, align 8
  call void @_ZN10JsonReader4failEPN4scaly6memory4PageEi(ptr noalias sret(%_Z9JsonToken) %sret.result55, ptr null, ptr %0, i64 1)
  ret i1 false

if.end50:                                         ; preds = %if.then40
  store i64 2, ptr %k, align 1
  br label %while.cond56

while.cond56:                                     ; preds = %if.end70, %if.end50
  %k59 = load i64, ptr %k, align 8
  %lt60 = icmp ult i64 %k59, 6
  br i1 %lt60, label %while.body57, label %while.exit58

while.body57:                                     ; preds = %while.cond56
  %field.inplace61 = getelementptr inbounds nuw %_Z10JsonReader, ptr %0, i32 0, i32 0
  %load.struct62 = load %_Z10JsonReader, ptr %0, align 8
  %pos63 = extractvalue %_Z10JsonReader %load.struct62, 7
  %k64 = load i64, ptr %k, align 8
  %add65 = add i64 %pos63, %k64
  %call66 = call i8 @_ZN5SliceI2u8EixEm(ptr %field.inplace61, i64 %add65)
  %call67 = call i64 @_ZN10JsonReader9hex_digitE2u8(i8 %call66)
  %lt68 = icmp slt i64 %call67, 0
  br i1 %lt68, label %if.then69, label %if.end70

while.exit58:                                     ; preds = %while.cond56
  %load.struct79 = load %_Z10JsonReader, ptr %0, align 8
  %pos80 = extractvalue %_Z10JsonReader %load.struct79, 7
  %add81 = add i64 %pos80, 6
  %pos82 = getelementptr inbounds nuw %_Z10JsonReader, ptr %0, i32 0, i32 7
  store i64 %add81, ptr %pos82, align 8
  br label %if.end42

if.then69:                                        ; preds = %while.body57
  %load.struct71 = load %_Z10JsonReader, ptr %0, align 8
  %pos72 = extractvalue %_Z10JsonReader %load.struct71, 7
  %k73 = load i64, ptr %k, align 8
  %add74 = add i64 %pos72, %k73
  %pos75 = getelementptr inbounds nuw %_Z10JsonReader, ptr %0, i32 0, i32 7
  store i64 %add74, ptr %pos75, align 8
  call void @_ZN10JsonReader4failEPN4scaly6memory4PageEi(ptr noalias sret(%_Z9JsonToken) %sret.result76, ptr null, ptr %0, i64 9)
  ret i1 false

if.end70:                                         ; preds = %while.body57
  %k77 = load i64, ptr %k, align 8
  %add78 = add i64 %k77, 1
  store i64 %add78, ptr %k, align 1
  br label %while.cond56

match.end:                                        ; preds = %match.case
  %match.value = phi i64 [ %add98, %match.case ]
  br label %if.end42

match.case:                                       ; preds = %match.alt94, %match.alt92, %match.alt90, %match.alt88, %match.alt86, %match.alt84, %match.alt, %if.else41
  %load.struct96 = load %_Z10JsonReader, ptr %0, align 8
  %pos97 = extractvalue %_Z10JsonReader %load.struct96, 7
  %add98 = add i64 %pos97, 2
  %pos99 = getelementptr inbounds nuw %_Z10JsonReader, ptr %0, i32 0, i32 7
  store i64 %add98, ptr %pos99, align 8
  br label %match.end

match.next:                                       ; preds = %match.alt94
  %load.struct100 = load %_Z10JsonReader, ptr %0, align 8
  %pos101 = extractvalue %_Z10JsonReader %load.struct100, 7
  %add102 = add i64 %pos101, 1
  %pos103 = getelementptr inbounds nuw %_Z10JsonReader, ptr %0, i32 0, i32 7
  store i64 %add102, ptr %pos103, align 8
  call void @_ZN10JsonReader4failEPN4scaly6memory4PageEi(ptr noalias sret(%_Z9JsonToken) %sret.result104, ptr null, ptr %0, i64 9)
  ret i1 false

match.alt:                                        ; preds = %if.else41
  %match.cmp83 = icmp eq i8 %call38, 92
  br i1 %match.cmp83, label %match.case, label %match.alt84

match.alt84:                                      ; preds = %match.alt
  %match.cmp85 = icmp eq i8 %call38, 47
  br i1 %match.cmp85, label %match.case, label %match.alt86

match.alt86:                                      ; preds = %match.alt84
  %match.cmp87 = icmp eq i8 %call38, 98
  br i1 %match.cmp87, label %match.case, label %match.alt88

match.alt88:                                      ; preds = %match.alt86
  %match.cmp89 = icmp eq i8 %call38, 102
  br i1 %match.cmp89, label %match.case, label %match.alt90

match.alt90:                                      ; preds = %match.alt88
  %match.cmp91 = icmp eq i8 %call38, 110
  br i1 %match.cmp91, label %match.case, label %match.alt92

match.alt92:                                      ; preds = %match.alt90
  %match.cmp93 = icmp eq i8 %call38, 114
  br i1 %match.cmp93, label %match.case, label %match.alt94

match.alt94:                                      ; preds = %match.alt92
  %match.cmp95 = icmp eq i8 %call38, 116
  br i1 %match.cmp95, label %match.case, label %match.next
}

define linkonce_odr void @_ZN10JsonReader5valueEPN4scaly6memory4PageE2u8(ptr noalias sret(%_Z9JsonToken) %0, ptr %1, ptr %2, i8 %3) {
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
  %call = call i1 @_ZN10JsonReader4openEb(ptr %2, i1 true)
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
  %call9 = call i1 @_ZN10JsonReader4openEb(ptr %2, i1 false)
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
  %call22 = call i1 @_ZN10JsonReader11scan_stringEv(ptr %2)
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
  call void @_ZN10JsonReader11after_valueEv(ptr %2)
  %variant.tag.ptr29 = getelementptr inbounds nuw %_Z9JsonToken, ptr %variant.ptr26, i32 0, i32 0
  store i8 7, ptr %variant.tag.ptr29, align 1
  %variant.val30 = load %_Z9JsonToken, ptr %variant.ptr26, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %variant.ptr26, i64 ptrtoint (ptr getelementptr (%_Z9JsonToken, ptr null, i32 1) to i64), i1 false)
  ret void

if.then31:                                        ; preds = %lor.end, %if.end21
  %call35 = call i1 @_ZN10JsonReader11scan_numberEv(ptr %2)
  %eq36 = icmp eq i1 %call35, false
  br i1 %eq36, label %if.then37, label %if.end38

if.end32:                                         ; preds = %lor.end
  %tuple.field = getelementptr inbounds nuw %_Z5SliceI2u8E, ptr %tuple, i32 0, i32 0
  store i64 4, ptr %tuple.field, align 1
  %tuple.field44 = getelementptr inbounds nuw %_Z5SliceI2u8E, ptr %tuple, i32 0, i32 1
  store ptr @.str.17, ptr %tuple.field44, align 1
  %tuple.val = load %_Z5SliceI2u8E, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %arg.tmp, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z5SliceI2u8E, ptr null, i32 1) to i64), i1 false)
  %call45 = call i1 @_ZN10JsonReader7literalE5SliceI2u8E(ptr %2, ptr %arg.tmp)
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
  call void @_ZN10JsonReader11after_valueEv(ptr %2)
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
  store ptr @.str.18, ptr %tuple.field53, align 1
  %tuple.val54 = load %_Z5SliceI2u8E, ptr %tuple51, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %arg.tmp55, ptr align 1 %tuple51, i64 ptrtoint (ptr getelementptr (%_Z5SliceI2u8E, ptr null, i32 1) to i64), i1 false)
  %call56 = call i1 @_ZN10JsonReader7literalE5SliceI2u8E(ptr %2, ptr %arg.tmp55)
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
  store ptr @.str.19, ptr %tuple.field63, align 1
  %tuple.val64 = load %_Z5SliceI2u8E, ptr %tuple61, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %arg.tmp65, ptr align 1 %tuple61, i64 ptrtoint (ptr getelementptr (%_Z5SliceI2u8E, ptr null, i32 1) to i64), i1 false)
  %call66 = call i1 @_ZN10JsonReader7literalE5SliceI2u8E(ptr %2, ptr %arg.tmp65)
  br i1 %call66, label %if.then67, label %if.end68

if.then67:                                        ; preds = %if.end58
  %variant.tag.ptr69 = getelementptr inbounds nuw %_Z9JsonToken, ptr %variant.ptr48, i32 0, i32 0
  store i8 11, ptr %variant.tag.ptr69, align 1
  %variant.val70 = load %_Z9JsonToken, ptr %variant.ptr48, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %variant.ptr48, i64 ptrtoint (ptr getelementptr (%_Z9JsonToken, ptr null, i32 1) to i64), i1 false)
  ret void

if.end68:                                         ; preds = %if.end58
  call void @_ZN10JsonReader4failEPN4scaly6memory4PageEi(ptr noalias sret(%_Z9JsonToken) %variant.ptr48, ptr null, ptr %2, i64 3)
  %sret.body = load %_Z9JsonToken, ptr %variant.ptr48, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %variant.ptr48, i64 ptrtoint (ptr getelementptr (%_Z9JsonToken, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN10JsonReader4nextEPN4scaly6memory4PageE(ptr noalias sret(%_Z9JsonToken) %0, ptr %1, ptr %2) {
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
  call void @_ZN10JsonReader7skip_wsEv(ptr %2)
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
  call void @_ZN10JsonReader4failEPN4scaly6memory4PageEi(ptr noalias sret(%_Z9JsonToken) %sret.result, ptr null, ptr %2, i64 1)
  %sret.body = load %_Z9JsonToken, ptr %sret.result, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %sret.result, i64 ptrtoint (ptr getelementptr (%_Z9JsonToken, ptr null, i32 1) to i64), i1 false)
  ret void

if.then23:                                        ; preds = %if.end9
  call void @_ZN10JsonReader4failEPN4scaly6memory4PageEi(ptr noalias sret(%_Z9JsonToken) %sret.result25, ptr null, ptr %2, i64 2)
  %sret.body26 = load %_Z9JsonToken, ptr %sret.result25, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %sret.result25, i64 ptrtoint (ptr getelementptr (%_Z9JsonToken, ptr null, i32 1) to i64), i1 false)
  ret void

if.end24:                                         ; preds = %if.end9
  %load.struct27 = load %_Z10JsonReader, ptr %2, align 8
  %state28 = extractvalue %_Z10JsonReader %load.struct27, 8
  %eq29 = icmp eq i64 %state28, 4
  br i1 %eq29, label %if.then30, label %if.end31

if.then30:                                        ; preds = %if.end24
  %call32 = call i1 @_ZN10JsonReader9in_objectEv(ptr %2)
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
  call void @_ZN10JsonReader5closeEPN4scaly6memory4PageE9JsonToken(ptr noalias sret(%_Z9JsonToken) %sret.result25, ptr null, ptr %2, ptr %arg.tmp)
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
  call void @_ZN10JsonReader5closeEPN4scaly6memory4PageE9JsonToken(ptr noalias sret(%_Z9JsonToken) %sret.result25, ptr null, ptr %2, ptr %arg.tmp)
  %sret.body57 = load %_Z9JsonToken, ptr %sret.result25, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %sret.result25, i64 ptrtoint (ptr getelementptr (%_Z9JsonToken, ptr null, i32 1) to i64), i1 false)
  ret void

if.end51:                                         ; preds = %land.rhs52, %if.end44
  call void @_ZN10JsonReader4failEPN4scaly6memory4PageEi(ptr noalias sret(%_Z9JsonToken) %sret.result25, ptr null, ptr %2, i64 6)
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
  call void @_ZN10JsonReader5closeEPN4scaly6memory4PageE9JsonToken(ptr noalias sret(%_Z9JsonToken) %variant.ptr46, ptr null, ptr %2, ptr %arg.tmp68)
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
  call void @_ZN10JsonReader5closeEPN4scaly6memory4PageE9JsonToken(ptr noalias sret(%_Z9JsonToken) %variant.ptr46, ptr null, ptr %2, ptr %arg.tmp68)
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
  call void @_ZN10JsonReader5valueEPN4scaly6memory4PageE2u8(ptr noalias sret(%_Z9JsonToken) %arg.tmp, ptr %1, ptr %2, i8 %call)
  %sret.body120 = load %_Z9JsonToken, ptr %arg.tmp, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %arg.tmp, i64 ptrtoint (ptr getelementptr (%_Z9JsonToken, ptr null, i32 1) to i64), i1 false)
  ret void

lor.rhs:                                          ; preds = %if.end71
  %load.struct85 = load %_Z10JsonReader, ptr %2, align 8
  %state86 = extractvalue %_Z10JsonReader %load.struct85, 8
  %eq87 = icmp eq i64 %state86, 3
  br i1 %eq87, label %if.then80, label %if.end81

if.then89:                                        ; preds = %if.then80
  call void @_ZN10JsonReader4failEPN4scaly6memory4PageEi(ptr noalias sret(%_Z9JsonToken) %variant.ptr46, ptr null, ptr %2, i64 4)
  %sret.body91 = load %_Z9JsonToken, ptr %variant.ptr46, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %variant.ptr46, i64 ptrtoint (ptr getelementptr (%_Z9JsonToken, ptr null, i32 1) to i64), i1 false)
  ret void

if.end90:                                         ; preds = %if.then80
  %call92 = call i1 @_ZN10JsonReader11scan_stringEv(ptr %2)
  %eq93 = icmp eq i1 %call92, false
  br i1 %eq93, label %if.then94, label %if.end95

if.then94:                                        ; preds = %if.end90
  %variant.tag.ptr96 = getelementptr inbounds nuw %_Z9JsonToken, ptr %variant.ptr46, i32 0, i32 0
  store i8 1, ptr %variant.tag.ptr96, align 1
  %variant.val97 = load %_Z9JsonToken, ptr %variant.ptr46, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %variant.ptr46, i64 ptrtoint (ptr getelementptr (%_Z9JsonToken, ptr null, i32 1) to i64), i1 false)
  ret void

if.end95:                                         ; preds = %if.end90
  call void @_ZN10JsonReader7skip_wsEv(ptr %2)
  %load.struct101 = load %_Z10JsonReader, ptr %2, align 8
  %pos102 = extractvalue %_Z10JsonReader %load.struct101, 7
  %load.struct103 = load %_Z10JsonReader, ptr %2, align 8
  %source104 = extractvalue %_Z10JsonReader %load.struct103, 0
  %length105 = extractvalue %_Z5SliceI2u8E %source104, 0
  %ge106 = icmp uge i64 %pos102, %length105
  br i1 %ge106, label %if.then98, label %lor.rhs100

if.then98:                                        ; preds = %lor.rhs100, %if.end95
  call void @_ZN10JsonReader4failEPN4scaly6memory4PageEi(ptr noalias sret(%_Z9JsonToken) %variant.ptr46, ptr null, ptr %2, i64 5)
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

define linkonce_odr i1 @_ZN10JsonReader4skipEv(ptr %0) {
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
  call void @_ZN10JsonReader4nextEPN4scaly6memory4PageE(ptr noalias sret(%_Z9JsonToken) %sret.result, ptr %frame, ptr %0)
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

define linkonce_odr void @_ZN5SliceI1TE8subsliceEPN4scaly6memory4PageEmm(ptr noalias sret(%_Z5SliceI1TE) %0, ptr %1, ptr %2, i64 %3, i64 %4) {
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

define linkonce_odr void @_ZN10JsonReader4textEPN4scaly6memory4PageE(ptr noalias sret(%_Z5SliceI2u8E) %0, ptr %1, ptr %2) {
entry:
  %sret.result = alloca %_Z5SliceI2u8E, align 8
  %field.inplace = getelementptr inbounds nuw %_Z10JsonReader, ptr %2, i32 0, i32 0
  %field.inplace1 = getelementptr inbounds nuw %_Z10JsonReader, ptr %2, i32 0, i32 9
  %field.val = load i64, ptr %field.inplace1, align 8
  %field.inplace2 = getelementptr inbounds nuw %_Z10JsonReader, ptr %2, i32 0, i32 10
  %field.val3 = load i64, ptr %field.inplace2, align 8
  call void @_ZN5SliceI2u8E8subsliceEPN4scaly6memory4PageEmm(ptr noalias sret(%_Z5SliceI2u8E) %sret.result, ptr null, ptr %field.inplace, i64 %field.val, i64 %field.val3)
  %sret.body = load %_Z5SliceI2u8E, ptr %sret.result, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %sret.result, i64 ptrtoint (ptr getelementptr (%_Z5SliceI2u8E, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN10JsonReader6decodeEPN4scaly6memory4PageE5SliceI2u8Eb(ptr noalias sret({ ptr }) %0, ptr %1, ptr %2, i1 %3) {
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
  call void @_ZN5SliceI2u8E8subsliceEPN4scaly6memory4PageEmm(ptr noalias sret(%_Z5SliceI2u8E) %sret.result, ptr null, ptr %2, i64 %i28, i64 %j29)
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
  %call48 = call i64 @_ZN10JsonReader4hex4E5SliceI2u8Em(ptr %2, i64 %i47)
  store i64 %call48, ptr %cp, align 1
  %i49 = load i64, ptr %i, align 8
  %add50 = add i64 %i49, 4
  store i64 %add50, ptr %i, align 1
  %cp54 = load i64, ptr %cp, align 8
  %ge55 = icmp sge i64 %cp54, 55296
  br i1 %ge55, label %land.rhs, label %if.else52

if.else:                                          ; preds = %if.end38
  %sb99 = load ptr, ptr %sb, align 8
  %call100 = call i8 @_ZN10JsonReader8unescapeE2u8(i8 %call41)
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
  call void @_ZN10JsonReader11append_utf8ER13StringBuilder3i64(ptr %sb97, i64 %cp98)
  br label %if.end46

land.rhs:                                         ; preds = %if.then45
  %cp56 = load i64, ptr %cp, align 8
  %le = icmp sle i64 %cp56, 56319
  br i1 %le, label %if.then51, label %if.else52

if.then57:                                        ; preds = %land.rhs60
  %i74 = load i64, ptr %i, align 8
  %add75 = add i64 %i74, 2
  %call76 = call i64 @_ZN10JsonReader4hex4E5SliceI2u8Em(ptr %2, i64 %add75)
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

define linkonce_odr void @_ZN10JsonReader6stringEPN4scaly6memory4PageE(ptr noalias sret({ ptr }) %0, ptr %1, ptr %2) {
entry:
  %sret.result = alloca { ptr }, align 8
  %sret.result1 = alloca %_Z5SliceI2u8E, align 8
  %field.inplace = getelementptr inbounds nuw %_Z10JsonReader, ptr %2, i32 0, i32 0
  %field.inplace2 = getelementptr inbounds nuw %_Z10JsonReader, ptr %2, i32 0, i32 9
  %field.val = load i64, ptr %field.inplace2, align 8
  %field.inplace3 = getelementptr inbounds nuw %_Z10JsonReader, ptr %2, i32 0, i32 10
  %field.val4 = load i64, ptr %field.inplace3, align 8
  call void @_ZN5SliceI2u8E8subsliceEPN4scaly6memory4PageEmm(ptr noalias sret(%_Z5SliceI2u8E) %sret.result1, ptr null, ptr %field.inplace, i64 %field.val, i64 %field.val4)
  %field.inplace5 = getelementptr inbounds nuw %_Z10JsonReader, ptr %2, i32 0, i32 11
  %field.val6 = load i1, ptr %field.inplace5, align 1
  call void @_ZN10JsonReader6decodeEPN4scaly6memory4PageE5SliceI2u8Eb(ptr noalias sret({ ptr }) %sret.result, ptr %1, ptr %sret.result1, i1 %field.val6)
  %sret.body = load { ptr }, ptr %sret.result, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %sret.result, i64 ptrtoint (ptr getelementptr ({ ptr }, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr i1 @_ZN10JsonReader11is_integralEv(ptr %0) {
entry:
  %load.struct = load %_Z10JsonReader, ptr %0, align 8
  %integral = extractvalue %_Z10JsonReader %load.struct, 12
  ret i1 %integral
}

define linkonce_odr i1 @_ZN10JsonReader6failedEv(ptr %0) {
entry:
  %load.struct = load %_Z10JsonReader, ptr %0, align 8
  %error = extractvalue %_Z10JsonReader %load.struct, 13
  %ne = icmp ne i64 %error, 0
  ret i1 %ne
}

define linkonce_odr i64 @_ZN10JsonReader12error_offsetEv(ptr %0) {
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
  call void @_Z15scaly_panic_oobP10const_charmm(ptr @.str.3, i64 %1, i64 %field.val)
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
  call void @_Z15scaly_panic_oobP10const_charmm(ptr @.str.4, i64 %1, i64 %field.val)
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

define linkonce_odr void @_ZN5SliceIcE8subsliceEPN4scaly6memory4PageEmm(ptr noalias sret(%_Z5SliceIcE) %0, ptr %1, ptr %2, i64 %3, i64 %4) {
entry:
  %tuple = alloca %_Z5SliceIcE, align 8
  %from = alloca i64, align 8
  store i64 %3, ptr %from, align 1
  %to = alloca i64, align 8
  store i64 %4, ptr %to, align 1
  %from1 = load i64, ptr %from, align 8
  %load.struct = load %_Z5SliceIcE, ptr %2, align 8
  %length = extractvalue %_Z5SliceIcE %load.struct, 0
  %gt = icmp ugt i64 %from1, %length
  br i1 %gt, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %load.struct2 = load %_Z5SliceIcE, ptr %2, align 8
  %length3 = extractvalue %_Z5SliceIcE %load.struct2, 0
  store i64 %length3, ptr %from, align 1
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %to4 = load i64, ptr %to, align 8
  %load.struct5 = load %_Z5SliceIcE, ptr %2, align 8
  %length6 = extractvalue %_Z5SliceIcE %load.struct5, 0
  %gt7 = icmp ugt i64 %to4, %length6
  br i1 %gt7, label %if.then8, label %if.end9

if.then8:                                         ; preds = %if.end
  %load.struct10 = load %_Z5SliceIcE, ptr %2, align 8
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
  %load.struct20 = load %_Z5SliceIcE, ptr %2, align 8
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
  call void @_ZN5SliceIcE8subsliceEPN4scaly6memory4PageEmm(ptr noalias sret(%_Z5SliceIcE) %sret.result, ptr null, ptr %2, i64 %3, i64 %field.val)
  %sret.body = load %_Z5SliceIcE, ptr %sret.result, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %sret.result, i64 ptrtoint (ptr getelementptr (%_Z5SliceIcE, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN5SliceIcE8slice_toEPN4scaly6memory4PageEm(ptr noalias sret(%_Z5SliceIcE) %0, ptr %1, ptr %2, i64 %3) {
entry:
  %sret.result = alloca %_Z5SliceIcE, align 8
  call void @_ZN5SliceIcE8subsliceEPN4scaly6memory4PageEmm(ptr noalias sret(%_Z5SliceIcE) %sret.result, ptr null, ptr %2, i64 0, i64 %3)
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
  call void @_ZN5SliceIcE8subsliceEPN4scaly6memory4PageEmm(ptr noalias sret(%_Z5SliceIcE) %sret.result, ptr null, ptr %0, i64 0, i64 %field.val)
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
  call void @_ZN5SliceIcE8subsliceEPN4scaly6memory4PageEmm(ptr noalias sret(%_Z5SliceIcE) %sret.result, ptr null, ptr %0, i64 %sub, i64 %field.val)
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

define linkonce_odr void @_ZN10JsonReader7messageEPN4scaly6memory4PageEi(ptr noalias sret(%_Z5SliceIcE) %0, ptr %1, i64 %2) {
entry:
  %tuple = alloca %_Z5SliceIcE, align 8
  %match.cmp = icmp eq i64 %2, 0
  br i1 %match.cmp, label %match.case, label %match.next

match.end:                                        ; No predecessors!
  ret void

match.case:                                       ; preds = %entry
  %tuple.field = getelementptr inbounds nuw %_Z5SliceIcE, ptr %tuple, i32 0, i32 0
  store i64 8, ptr %tuple.field, align 1
  %tuple.field1 = getelementptr inbounds nuw %_Z5SliceIcE, ptr %tuple, i32 0, i32 1
  store ptr @.str.5, ptr %tuple.field1, align 1
  %tuple.val = load %_Z5SliceIcE, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z5SliceIcE, ptr null, i32 1) to i64), i1 false)
  ret void

match.next:                                       ; preds = %entry
  %match.cmp4 = icmp eq i64 %2, 1
  br i1 %match.cmp4, label %match.case2, label %match.next3

match.case2:                                      ; preds = %match.next
  %tuple.field5 = getelementptr inbounds nuw %_Z5SliceIcE, ptr %tuple, i32 0, i32 0
  store i64 30, ptr %tuple.field5, align 1
  %tuple.field6 = getelementptr inbounds nuw %_Z5SliceIcE, ptr %tuple, i32 0, i32 1
  store ptr @.str.6, ptr %tuple.field6, align 1
  %tuple.val7 = load %_Z5SliceIcE, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z5SliceIcE, ptr null, i32 1) to i64), i1 false)
  ret void

match.next3:                                      ; preds = %match.next
  %match.cmp10 = icmp eq i64 %2, 2
  br i1 %match.cmp10, label %match.case8, label %match.next9

match.case8:                                      ; preds = %match.next3
  %tuple.field11 = getelementptr inbounds nuw %_Z5SliceIcE, ptr %tuple, i32 0, i32 0
  store i64 37, ptr %tuple.field11, align 1
  %tuple.field12 = getelementptr inbounds nuw %_Z5SliceIcE, ptr %tuple, i32 0, i32 1
  store ptr @.str.7, ptr %tuple.field12, align 1
  %tuple.val13 = load %_Z5SliceIcE, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z5SliceIcE, ptr null, i32 1) to i64), i1 false)
  ret void

match.next9:                                      ; preds = %match.next3
  %match.cmp16 = icmp eq i64 %2, 3
  br i1 %match.cmp16, label %match.case14, label %match.next15

match.case14:                                     ; preds = %match.next9
  %tuple.field17 = getelementptr inbounds nuw %_Z5SliceIcE, ptr %tuple, i32 0, i32 0
  store i64 20, ptr %tuple.field17, align 1
  %tuple.field18 = getelementptr inbounds nuw %_Z5SliceIcE, ptr %tuple, i32 0, i32 1
  store ptr @.str.8, ptr %tuple.field18, align 1
  %tuple.val19 = load %_Z5SliceIcE, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z5SliceIcE, ptr null, i32 1) to i64), i1 false)
  ret void

match.next15:                                     ; preds = %match.next9
  %match.cmp22 = icmp eq i64 %2, 4
  br i1 %match.cmp22, label %match.case20, label %match.next21

match.case20:                                     ; preds = %match.next15
  %tuple.field23 = getelementptr inbounds nuw %_Z5SliceIcE, ptr %tuple, i32 0, i32 0
  store i64 37, ptr %tuple.field23, align 1
  %tuple.field24 = getelementptr inbounds nuw %_Z5SliceIcE, ptr %tuple, i32 0, i32 1
  store ptr @.str.9, ptr %tuple.field24, align 1
  %tuple.val25 = load %_Z5SliceIcE, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z5SliceIcE, ptr null, i32 1) to i64), i1 false)
  ret void

match.next21:                                     ; preds = %match.next15
  %match.cmp28 = icmp eq i64 %2, 5
  br i1 %match.cmp28, label %match.case26, label %match.next27

match.case26:                                     ; preds = %match.next21
  %tuple.field29 = getelementptr inbounds nuw %_Z5SliceIcE, ptr %tuple, i32 0, i32 0
  store i64 38, ptr %tuple.field29, align 1
  %tuple.field30 = getelementptr inbounds nuw %_Z5SliceIcE, ptr %tuple, i32 0, i32 1
  store ptr @.str.10, ptr %tuple.field30, align 1
  %tuple.val31 = load %_Z5SliceIcE, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z5SliceIcE, ptr null, i32 1) to i64), i1 false)
  ret void

match.next27:                                     ; preds = %match.next21
  %match.cmp34 = icmp eq i64 %2, 6
  br i1 %match.cmp34, label %match.case32, label %match.next33

match.case32:                                     ; preds = %match.next27
  %tuple.field35 = getelementptr inbounds nuw %_Z5SliceIcE, ptr %tuple, i32 0, i32 0
  store i64 39, ptr %tuple.field35, align 1
  %tuple.field36 = getelementptr inbounds nuw %_Z5SliceIcE, ptr %tuple, i32 0, i32 1
  store ptr @.str.11, ptr %tuple.field36, align 1
  %tuple.val37 = load %_Z5SliceIcE, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z5SliceIcE, ptr null, i32 1) to i64), i1 false)
  ret void

match.next33:                                     ; preds = %match.next27
  %match.cmp40 = icmp eq i64 %2, 7
  br i1 %match.cmp40, label %match.case38, label %match.next39

match.case38:                                     ; preds = %match.next33
  %tuple.field41 = getelementptr inbounds nuw %_Z5SliceIcE, ptr %tuple, i32 0, i32 0
  store i64 16, ptr %tuple.field41, align 1
  %tuple.field42 = getelementptr inbounds nuw %_Z5SliceIcE, ptr %tuple, i32 0, i32 1
  store ptr @.str.12, ptr %tuple.field42, align 1
  %tuple.val43 = load %_Z5SliceIcE, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z5SliceIcE, ptr null, i32 1) to i64), i1 false)
  ret void

match.next39:                                     ; preds = %match.next33
  %match.cmp46 = icmp eq i64 %2, 8
  br i1 %match.cmp46, label %match.case44, label %match.next45

match.case44:                                     ; preds = %match.next39
  %tuple.field47 = getelementptr inbounds nuw %_Z5SliceIcE, ptr %tuple, i32 0, i32 0
  store i64 33, ptr %tuple.field47, align 1
  %tuple.field48 = getelementptr inbounds nuw %_Z5SliceIcE, ptr %tuple, i32 0, i32 1
  store ptr @.str.13, ptr %tuple.field48, align 1
  %tuple.val49 = load %_Z5SliceIcE, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z5SliceIcE, ptr null, i32 1) to i64), i1 false)
  ret void

match.next45:                                     ; preds = %match.next39
  %match.cmp52 = icmp eq i64 %2, 9
  br i1 %match.cmp52, label %match.case50, label %match.next51

match.case50:                                     ; preds = %match.next45
  %tuple.field53 = getelementptr inbounds nuw %_Z5SliceIcE, ptr %tuple, i32 0, i32 0
  store i64 26, ptr %tuple.field53, align 1
  %tuple.field54 = getelementptr inbounds nuw %_Z5SliceIcE, ptr %tuple, i32 0, i32 1
  store ptr @.str.14, ptr %tuple.field54, align 1
  %tuple.val55 = load %_Z5SliceIcE, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z5SliceIcE, ptr null, i32 1) to i64), i1 false)
  ret void

match.next51:                                     ; preds = %match.next45
  %match.cmp58 = icmp eq i64 %2, 10
  br i1 %match.cmp58, label %match.case56, label %match.next57

match.case56:                                     ; preds = %match.next51
  %tuple.field59 = getelementptr inbounds nuw %_Z5SliceIcE, ptr %tuple, i32 0, i32 0
  store i64 16, ptr %tuple.field59, align 1
  %tuple.field60 = getelementptr inbounds nuw %_Z5SliceIcE, ptr %tuple, i32 0, i32 1
  store ptr @.str.15, ptr %tuple.field60, align 1
  %tuple.val61 = load %_Z5SliceIcE, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z5SliceIcE, ptr null, i32 1) to i64), i1 false)
  ret void

match.next57:                                     ; preds = %match.next51
  %tuple.field62 = getelementptr inbounds nuw %_Z5SliceIcE, ptr %tuple, i32 0, i32 0
  store i64 13, ptr %tuple.field62, align 1
  %tuple.field63 = getelementptr inbounds nuw %_Z5SliceIcE, ptr %tuple, i32 0, i32 1
  store ptr @.str.16, ptr %tuple.field63, align 1
  %tuple.val64 = load %_Z5SliceIcE, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z5SliceIcE, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN10JsonReader13error_messageEPN4scaly6memory4PageE(ptr noalias sret(%_Z5SliceIcE) %0, ptr %1, ptr %2) {
entry:
  %sret.result = alloca %_Z5SliceIcE, align 8
  %field.inplace = getelementptr inbounds nuw %_Z10JsonReader, ptr %2, i32 0, i32 13
  %field.val = load i64, ptr %field.inplace, align 8
  call void @_ZN10JsonReader7messageEPN4scaly6memory4PageEi(ptr noalias sret(%_Z5SliceIcE) %sret.result, ptr null, i64 %field.val)
  %sret.body = load %_Z5SliceIcE, ptr %sret.result, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %sret.result, i64 ptrtoint (ptr getelementptr (%_Z5SliceIcE, ptr null, i32 1) to i64), i1 false)
  ret void
}

declare void @_ZN13StringBuilder6appendEPcm(ptr, ptr, i64)

define linkonce_odr i64 @_ZN10JsonReader4hex4E5SliceI2u8Em(ptr %0, i64 %1) {
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
  %call4 = call i64 @_ZN10JsonReader9hex_digitE2u8(i8 %call)
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

define linkonce_odr void @_ZN10JsonReader11append_utf8ER13StringBuilder3i64(ptr %0, i64 %1) {
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

define linkonce_odr i8 @_ZN10JsonReader8unescapeE2u8(i8 %0) {
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

define linkonce_odr i64 @_ZN10JsonReader9hex_digitE2u8(i8 %0) {
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

define linkonce_odr i1 @_ZN10JsonReader4openEb(ptr %0, i1 %1) {
entry:
  %sret.result = alloca %_Z9JsonToken, align 8
  %load.struct = load %_Z10JsonReader, ptr %0, align 8
  %depth = extractvalue %_Z10JsonReader %load.struct, 2
  %load.struct1 = load %_Z10JsonReader, ptr %0, align 8
  %max_depth = extractvalue %_Z10JsonReader %load.struct1, 1
  %ge = icmp uge i64 %depth, %max_depth
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  call void @_ZN10JsonReader4failEPN4scaly6memory4PageEi(ptr noalias sret(%_Z9JsonToken) %sret.result, ptr null, ptr %0, i64 10)
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

define linkonce_odr void @_ZN10JsonReader11after_valueEv(ptr %0) {
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

define linkonce_odr i1 @_ZN10JsonReader11scan_numberEv(ptr %0) {
entry:
  %load.struct = load %_Z10JsonReader, ptr %0, align 8
  %pos = extractvalue %_Z10JsonReader %load.struct, 7
  %start = getelementptr inbounds nuw %_Z10JsonReader, ptr %0, i32 0, i32 9
  store i64 %pos, ptr %start, align 8
  %integral = getelementptr inbounds nuw %_Z10JsonReader, ptr %0, i32 0, i32 12
  store i1 true, ptr %integral, align 1
  %field.inplace = getelementptr inbounds nuw %_Z10JsonReader, ptr %0, i32 0, i32 0
  %field.inplace1 = getelementptr inbounds nuw %_Z10JsonReader, ptr %0, i32 0, i32 7
  %field.val = load i64, ptr %field.inplace1, align 8
  %call = call i8 @_ZN5SliceI2u8EixEm(ptr %field.inplace, i64 %field.val)
  %eq = icmp eq i8 %call, 45
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %load.struct2 = load %_Z10JsonReader, ptr %0, align 8
  %pos3 = extractvalue %_Z10JsonReader %load.struct2, 7
  %add = add i64 %pos3, 1
  %pos4 = getelementptr inbounds nuw %_Z10JsonReader, ptr %0, i32 0, i32 7
  store i64 %add, ptr %pos4, align 8
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %field.inplace5 = getelementptr inbounds nuw %_Z10JsonReader, ptr %0, i32 0, i32 7
  %field.val6 = load i64, ptr %field.inplace5, align 8
  %call7 = call i1 @_ZN10JsonReader8digit_atEm(ptr %0, i64 %field.val6)
  %eq8 = icmp eq i1 %call7, false
  br i1 %eq8, label %if.then9, label %if.end10

if.then9:                                         ; preds = %if.end
  %call11 = call i1 @_ZN10JsonReader12number_failsEv(ptr %0)
  ret i1 %call11

if.end10:                                         ; preds = %if.end
  %field.inplace12 = getelementptr inbounds nuw %_Z10JsonReader, ptr %0, i32 0, i32 0
  %field.inplace13 = getelementptr inbounds nuw %_Z10JsonReader, ptr %0, i32 0, i32 7
  %field.val14 = load i64, ptr %field.inplace13, align 8
  %call15 = call i8 @_ZN5SliceI2u8EixEm(ptr %field.inplace12, i64 %field.val14)
  %eq16 = icmp eq i8 %call15, 48
  br i1 %eq16, label %if.then17, label %if.else

if.then17:                                        ; preds = %if.end10
  %load.struct19 = load %_Z10JsonReader, ptr %0, align 8
  %pos20 = extractvalue %_Z10JsonReader %load.struct19, 7
  %add21 = add i64 %pos20, 1
  %pos22 = getelementptr inbounds nuw %_Z10JsonReader, ptr %0, i32 0, i32 7
  store i64 %add21, ptr %pos22, align 8
  %field.inplace23 = getelementptr inbounds nuw %_Z10JsonReader, ptr %0, i32 0, i32 7
  %field.val24 = load i64, ptr %field.inplace23, align 8
  %call25 = call i1 @_ZN10JsonReader8digit_atEm(ptr %0, i64 %field.val24)
  br i1 %call25, label %if.then26, label %if.end27

if.else:                                          ; preds = %if.end10
  call void @_ZN10JsonReader6digitsEv(ptr %0)
  br label %if.end18

if.end18:                                         ; preds = %if.else, %if.end27
  %load.struct31 = load %_Z10JsonReader, ptr %0, align 8
  %pos32 = extractvalue %_Z10JsonReader %load.struct31, 7
  %load.struct33 = load %_Z10JsonReader, ptr %0, align 8
  %source = extractvalue %_Z10JsonReader %load.struct33, 0
  %length = extractvalue %_Z5SliceI2u8E %source, 0
  %lt = icmp ult i64 %pos32, %length
  br i1 %lt, label %land.rhs, label %if.end30

if.then26:                                        ; preds = %if.then17
  %call28 = call i1 @_ZN10JsonReader12number_failsEv(ptr %0)
  ret i1 %call28

if.end27:                                         ; preds = %if.then17
  br label %if.end18

if.then29:                                        ; preds = %land.rhs
  %integral39 = getelementptr inbounds nuw %_Z10JsonReader, ptr %0, i32 0, i32 12
  store i1 false, ptr %integral39, align 1
  %load.struct40 = load %_Z10JsonReader, ptr %0, align 8
  %pos41 = extractvalue %_Z10JsonReader %load.struct40, 7
  %add42 = add i64 %pos41, 1
  %pos43 = getelementptr inbounds nuw %_Z10JsonReader, ptr %0, i32 0, i32 7
  store i64 %add42, ptr %pos43, align 8
  %field.inplace44 = getelementptr inbounds nuw %_Z10JsonReader, ptr %0, i32 0, i32 7
  %field.val45 = load i64, ptr %field.inplace44, align 8
  %call46 = call i1 @_ZN10JsonReader8digit_atEm(ptr %0, i64 %field.val45)
  %eq47 = icmp eq i1 %call46, false
  br i1 %eq47, label %if.then48, label %if.end49

if.end30:                                         ; preds = %if.end49, %land.rhs, %if.end18
  %load.struct54 = load %_Z10JsonReader, ptr %0, align 8
  %pos55 = extractvalue %_Z10JsonReader %load.struct54, 7
  %load.struct56 = load %_Z10JsonReader, ptr %0, align 8
  %source57 = extractvalue %_Z10JsonReader %load.struct56, 0
  %length58 = extractvalue %_Z5SliceI2u8E %source57, 0
  %lt59 = icmp ult i64 %pos55, %length58
  br i1 %lt59, label %land.rhs53, label %if.end52

land.rhs:                                         ; preds = %if.end18
  %field.inplace34 = getelementptr inbounds nuw %_Z10JsonReader, ptr %0, i32 0, i32 0
  %field.inplace35 = getelementptr inbounds nuw %_Z10JsonReader, ptr %0, i32 0, i32 7
  %field.val36 = load i64, ptr %field.inplace35, align 8
  %call37 = call i8 @_ZN5SliceI2u8EixEm(ptr %field.inplace34, i64 %field.val36)
  %eq38 = icmp eq i8 %call37, 46
  br i1 %eq38, label %if.then29, label %if.end30

if.then48:                                        ; preds = %if.then29
  %call50 = call i1 @_ZN10JsonReader12number_failsEv(ptr %0)
  ret i1 %call50

if.end49:                                         ; preds = %if.then29
  call void @_ZN10JsonReader6digitsEv(ptr %0)
  br label %if.end30

if.then51:                                        ; preds = %lor.end
  %integral70 = getelementptr inbounds nuw %_Z10JsonReader, ptr %0, i32 0, i32 12
  store i1 false, ptr %integral70, align 1
  %load.struct71 = load %_Z10JsonReader, ptr %0, align 8
  %pos72 = extractvalue %_Z10JsonReader %load.struct71, 7
  %add73 = add i64 %pos72, 1
  %pos74 = getelementptr inbounds nuw %_Z10JsonReader, ptr %0, i32 0, i32 7
  store i64 %add73, ptr %pos74, align 8
  %load.struct78 = load %_Z10JsonReader, ptr %0, align 8
  %pos79 = extractvalue %_Z10JsonReader %load.struct78, 7
  %load.struct80 = load %_Z10JsonReader, ptr %0, align 8
  %source81 = extractvalue %_Z10JsonReader %load.struct80, 0
  %length82 = extractvalue %_Z5SliceI2u8E %source81, 0
  %lt83 = icmp ult i64 %pos79, %length82
  br i1 %lt83, label %land.rhs77, label %if.end76

if.end52:                                         ; preds = %if.end106, %lor.end, %if.end30
  %load.struct108 = load %_Z10JsonReader, ptr %0, align 8
  %pos109 = extractvalue %_Z10JsonReader %load.struct108, 7
  %end = getelementptr inbounds nuw %_Z10JsonReader, ptr %0, i32 0, i32 10
  store i64 %pos109, ptr %end, align 8
  ret i1 true

land.rhs53:                                       ; preds = %if.end30
  %field.inplace60 = getelementptr inbounds nuw %_Z10JsonReader, ptr %0, i32 0, i32 0
  %field.inplace61 = getelementptr inbounds nuw %_Z10JsonReader, ptr %0, i32 0, i32 7
  %field.val62 = load i64, ptr %field.inplace61, align 8
  %call63 = call i8 @_ZN5SliceI2u8EixEm(ptr %field.inplace60, i64 %field.val62)
  %eq64 = icmp eq i8 %call63, 101
  br i1 %eq64, label %lor.end, label %lor.rhs

lor.rhs:                                          ; preds = %land.rhs53
  %field.inplace65 = getelementptr inbounds nuw %_Z10JsonReader, ptr %0, i32 0, i32 0
  %field.inplace66 = getelementptr inbounds nuw %_Z10JsonReader, ptr %0, i32 0, i32 7
  %field.val67 = load i64, ptr %field.inplace66, align 8
  %call68 = call i8 @_ZN5SliceI2u8EixEm(ptr %field.inplace65, i64 %field.val67)
  %eq69 = icmp eq i8 %call68, 69
  br label %lor.end

lor.end:                                          ; preds = %lor.rhs, %land.rhs53
  %lor.result = phi i1 [ true, %land.rhs53 ], [ %eq69, %lor.rhs ]
  br i1 %lor.result, label %if.then51, label %if.end52

if.then75:                                        ; preds = %lor.end90
  %load.struct97 = load %_Z10JsonReader, ptr %0, align 8
  %pos98 = extractvalue %_Z10JsonReader %load.struct97, 7
  %add99 = add i64 %pos98, 1
  %pos100 = getelementptr inbounds nuw %_Z10JsonReader, ptr %0, i32 0, i32 7
  store i64 %add99, ptr %pos100, align 8
  br label %if.end76

if.end76:                                         ; preds = %if.then75, %lor.end90, %if.then51
  %field.inplace101 = getelementptr inbounds nuw %_Z10JsonReader, ptr %0, i32 0, i32 7
  %field.val102 = load i64, ptr %field.inplace101, align 8
  %call103 = call i1 @_ZN10JsonReader8digit_atEm(ptr %0, i64 %field.val102)
  %eq104 = icmp eq i1 %call103, false
  br i1 %eq104, label %if.then105, label %if.end106

land.rhs77:                                       ; preds = %if.then51
  %field.inplace84 = getelementptr inbounds nuw %_Z10JsonReader, ptr %0, i32 0, i32 0
  %field.inplace85 = getelementptr inbounds nuw %_Z10JsonReader, ptr %0, i32 0, i32 7
  %field.val86 = load i64, ptr %field.inplace85, align 8
  %call87 = call i8 @_ZN5SliceI2u8EixEm(ptr %field.inplace84, i64 %field.val86)
  %eq88 = icmp eq i8 %call87, 43
  br i1 %eq88, label %lor.end90, label %lor.rhs89

lor.rhs89:                                        ; preds = %land.rhs77
  %field.inplace91 = getelementptr inbounds nuw %_Z10JsonReader, ptr %0, i32 0, i32 0
  %field.inplace92 = getelementptr inbounds nuw %_Z10JsonReader, ptr %0, i32 0, i32 7
  %field.val93 = load i64, ptr %field.inplace92, align 8
  %call94 = call i8 @_ZN5SliceI2u8EixEm(ptr %field.inplace91, i64 %field.val93)
  %eq95 = icmp eq i8 %call94, 45
  br label %lor.end90

lor.end90:                                        ; preds = %lor.rhs89, %land.rhs77
  %lor.result96 = phi i1 [ true, %land.rhs77 ], [ %eq95, %lor.rhs89 ]
  br i1 %lor.result96, label %if.then75, label %if.end76

if.then105:                                       ; preds = %if.end76
  %call107 = call i1 @_ZN10JsonReader12number_failsEv(ptr %0)
  ret i1 %call107

if.end106:                                        ; preds = %if.end76
  call void @_ZN10JsonReader6digitsEv(ptr %0)
  br label %if.end52
}

define linkonce_odr i1 @_ZN10JsonReader7literalE5SliceI2u8E(ptr %0, ptr %1) {
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
  call void @_ZN5SliceI2u8E8subsliceEPN4scaly6memory4PageEmm(ptr noalias sret(%_Z5SliceI2u8E) %sret.result, ptr null, ptr %field.inplace, i64 %field.val, i64 %add9)
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
  call void @_ZN10JsonReader11after_valueEv(ptr %0)
  ret i1 true
}

define linkonce_odr i1 @_ZN10JsonReader8digit_atEm(ptr %0, i64 %1) {
entry:
  %load.struct = load %_Z10JsonReader, ptr %0, align 8
  %source = extractvalue %_Z10JsonReader %load.struct, 0
  %length = extractvalue %_Z5SliceI2u8E %source, 0
  %lt = icmp ult i64 %1, %length
  br i1 %lt, label %lor.rhs, label %lor.end

lor.rhs:                                          ; preds = %entry
  %field.inplace = getelementptr inbounds nuw %_Z10JsonReader, ptr %0, i32 0, i32 0
  %call = call i8 @_ZN5SliceI2u8EixEm(ptr %field.inplace, i64 %1)
  %ge = icmp uge i8 %call, 48
  br label %lor.end

lor.end:                                          ; preds = %lor.rhs, %entry
  %lor.result = phi i1 [ false, %entry ], [ %ge, %lor.rhs ]
  br i1 %lor.result, label %lor.rhs1, label %lor.end2

lor.rhs1:                                         ; preds = %lor.end
  %field.inplace3 = getelementptr inbounds nuw %_Z10JsonReader, ptr %0, i32 0, i32 0
  %call4 = call i8 @_ZN5SliceI2u8EixEm(ptr %field.inplace3, i64 %1)
  %le = icmp ule i8 %call4, 57
  br label %lor.end2

lor.end2:                                         ; preds = %lor.rhs1, %lor.end
  %lor.result5 = phi i1 [ false, %lor.end ], [ %le, %lor.rhs1 ]
  ret i1 %lor.result5
}

define linkonce_odr i1 @_ZN10JsonReader12number_failsEv(ptr %0) {
entry:
  %sret.result = alloca %_Z9JsonToken, align 8
  call void @_ZN10JsonReader4failEPN4scaly6memory4PageEi(ptr noalias sret(%_Z9JsonToken) %sret.result, ptr null, ptr %0, i64 7)
  ret i1 false
}

define linkonce_odr void @_ZN10JsonReader6digitsEv(ptr %0) {
entry:
  br label %while.cond

while.cond:                                       ; preds = %while.body, %entry
  %field.inplace = getelementptr inbounds nuw %_Z10JsonReader, ptr %0, i32 0, i32 7
  %field.val = load i64, ptr %field.inplace, align 8
  %call = call i1 @_ZN10JsonReader8digit_atEm(ptr %0, i64 %field.val)
  br i1 %call, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %load.struct = load %_Z10JsonReader, ptr %0, align 8
  %pos = extractvalue %_Z10JsonReader %load.struct, 7
  %add = add i64 %pos, 1
  %pos1 = getelementptr inbounds nuw %_Z10JsonReader, ptr %0, i32 0, i32 7
  store i64 %add, ptr %pos1, align 8
  br label %while.cond

while.exit:                                       ; preds = %while.cond
  ret void
}

declare double @strtod(ptr, ptr)

define linkonce_odr ptr @_ZN5SliceI10JsonMemberE3getEm(ptr %0, i64 %1) {
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

define linkonce_odr ptr @_ZN5SliceI10JsonMemberE2atEm(ptr %0, i64 %1) {
entry:
  %load.struct = load %_Z5SliceI10JsonMemberE, ptr %0, align 8
  %length = extractvalue %_Z5SliceI10JsonMemberE %load.struct, 0
  %ge = icmp uge i64 %1, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %field.inplace = getelementptr inbounds nuw %_Z5SliceI10JsonMemberE, ptr %0, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  call void @_Z15scaly_panic_oobP10const_charmm(ptr @.str.20, i64 %1, i64 %field.val)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %load.struct1 = load %_Z5SliceI10JsonMemberE, ptr %0, align 8
  %data = extractvalue %_Z5SliceI10JsonMemberE %load.struct1, 1
  %ptr.add = getelementptr inbounds %_Z10JsonMember, ptr %data, i64 %1
  ret ptr %ptr.add
}

define linkonce_odr void @_ZN5SliceI10JsonMemberE3putEm10JsonMember(ptr %0, i64 %1, ptr %2) {
entry:
  %load.struct = load %_Z5SliceI10JsonMemberE, ptr %0, align 8
  %length = extractvalue %_Z5SliceI10JsonMemberE %load.struct, 0
  %ge = icmp uge i64 %1, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %field.inplace = getelementptr inbounds nuw %_Z5SliceI10JsonMemberE, ptr %0, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  call void @_Z15scaly_panic_oobP10const_charmm(ptr @.str.21, i64 %1, i64 %field.val)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %load.struct1 = load %_Z5SliceI10JsonMemberE, ptr %0, align 8
  %data = extractvalue %_Z5SliceI10JsonMemberE %load.struct1, 1
  %ptr.add = getelementptr inbounds %_Z10JsonMember, ptr %data, i64 %1
  %store.load = load %_Z10JsonMember, ptr %2, align 8
  store %_Z10JsonMember %store.load, ptr %ptr.add, align 8
  ret void
}

define linkonce_odr i1 @_ZN5SliceI10JsonMemberE8is_emptyEv(ptr %0) {
entry:
  %load.struct = load %_Z5SliceI10JsonMemberE, ptr %0, align 8
  %length = extractvalue %_Z5SliceI10JsonMemberE %load.struct, 0
  %eq = icmp eq i64 %length, 0
  ret i1 %eq
}

define linkonce_odr void @_ZN5SliceI10JsonMemberE8subsliceEPN4scaly6memory4PageEmm(ptr noalias sret(%_Z5SliceI10JsonMemberE) %0, ptr %1, ptr %2, i64 %3, i64 %4) {
entry:
  %tuple = alloca %_Z5SliceI10JsonMemberE, align 8
  %from = alloca i64, align 8
  store i64 %3, ptr %from, align 1
  %to = alloca i64, align 8
  store i64 %4, ptr %to, align 1
  %from1 = load i64, ptr %from, align 8
  %load.struct = load %_Z5SliceI10JsonMemberE, ptr %2, align 8
  %length = extractvalue %_Z5SliceI10JsonMemberE %load.struct, 0
  %gt = icmp ugt i64 %from1, %length
  br i1 %gt, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %load.struct2 = load %_Z5SliceI10JsonMemberE, ptr %2, align 8
  %length3 = extractvalue %_Z5SliceI10JsonMemberE %load.struct2, 0
  store i64 %length3, ptr %from, align 1
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %to4 = load i64, ptr %to, align 8
  %load.struct5 = load %_Z5SliceI10JsonMemberE, ptr %2, align 8
  %length6 = extractvalue %_Z5SliceI10JsonMemberE %load.struct5, 0
  %gt7 = icmp ugt i64 %to4, %length6
  br i1 %gt7, label %if.then8, label %if.end9

if.then8:                                         ; preds = %if.end
  %load.struct10 = load %_Z5SliceI10JsonMemberE, ptr %2, align 8
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
  %load.struct20 = load %_Z5SliceI10JsonMemberE, ptr %2, align 8
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

define linkonce_odr void @_ZN5SliceI10JsonMemberE10slice_fromEPN4scaly6memory4PageEm(ptr noalias sret(%_Z5SliceI10JsonMemberE) %0, ptr %1, ptr %2, i64 %3) {
entry:
  %sret.result = alloca %_Z5SliceI10JsonMemberE, align 8
  %field.inplace = getelementptr inbounds nuw %_Z5SliceI10JsonMemberE, ptr %2, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  call void @_ZN5SliceI10JsonMemberE8subsliceEPN4scaly6memory4PageEmm(ptr noalias sret(%_Z5SliceI10JsonMemberE) %sret.result, ptr null, ptr %2, i64 %3, i64 %field.val)
  %sret.body = load %_Z5SliceI10JsonMemberE, ptr %sret.result, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %sret.result, i64 ptrtoint (ptr getelementptr (%_Z5SliceI10JsonMemberE, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN5SliceI10JsonMemberE8slice_toEPN4scaly6memory4PageEm(ptr noalias sret(%_Z5SliceI10JsonMemberE) %0, ptr %1, ptr %2, i64 %3) {
entry:
  %sret.result = alloca %_Z5SliceI10JsonMemberE, align 8
  call void @_ZN5SliceI10JsonMemberE8subsliceEPN4scaly6memory4PageEmm(ptr noalias sret(%_Z5SliceI10JsonMemberE) %sret.result, ptr null, ptr %2, i64 0, i64 %3)
  %sret.body = load %_Z5SliceI10JsonMemberE, ptr %sret.result, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %sret.result, i64 ptrtoint (ptr getelementptr (%_Z5SliceI10JsonMemberE, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr i1 @_ZN5SliceI10JsonMemberE6equalsE5SliceI10JsonMemberE(ptr %0, ptr %1) {
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

define linkonce_odr i1 @_ZN5SliceI10JsonMemberE11starts_withE5SliceI10JsonMemberE(ptr %0, ptr %1) {
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
  call void @_ZN5SliceI10JsonMemberE8subsliceEPN4scaly6memory4PageEmm(ptr noalias sret(%_Z5SliceI10JsonMemberE) %sret.result, ptr null, ptr %0, i64 0, i64 %field.val)
  %call = call i1 @_ZN5SliceI10JsonMemberE6equalsE5SliceI10JsonMemberE(ptr %sret.result, ptr %1)
  ret i1 %call
}

define linkonce_odr i1 @_ZN5SliceI10JsonMemberE9ends_withE5SliceI10JsonMemberE(ptr %0, ptr %1) {
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
  call void @_ZN5SliceI10JsonMemberE8subsliceEPN4scaly6memory4PageEmm(ptr noalias sret(%_Z5SliceI10JsonMemberE) %sret.result, ptr null, ptr %0, i64 %sub, i64 %field.val)
  %call = call i1 @_ZN5SliceI10JsonMemberE6equalsE5SliceI10JsonMemberE(ptr %sret.result, ptr %1)
  ret i1 %call
}

define linkonce_odr ptr @_ZN13SliceIteratorI10JsonMemberE4nextEv(ptr %0) {
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

define linkonce_odr void @_ZN13SliceIteratorI10JsonMemberEC1E5SliceI10JsonMemberE(ptr %0, ptr %1) {
entry:
  %slice = getelementptr inbounds nuw %_Z13SliceIteratorI10JsonMemberE, ptr %0, i32 0, i32 0
  %field.load = load %_Z5SliceI10JsonMemberE, ptr %1, align 8
  store %_Z5SliceI10JsonMemberE %field.load, ptr %slice, align 8
  %position = getelementptr inbounds nuw %_Z13SliceIteratorI10JsonMemberE, ptr %0, i32 0, i32 1
  store i64 0, ptr %position, align 8
  ret void
}

define linkonce_odr void @_ZN5SliceI10JsonMemberE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z13SliceIteratorI10JsonMemberE) %0, ptr %1, ptr %2) {
entry:
  %struct.init = alloca %_Z13SliceIteratorI10JsonMemberE, align 8
  call void @_ZN13SliceIteratorI10JsonMemberEC1E5SliceI10JsonMemberE(ptr %struct.init, ptr %2)
  %sret.body = load %_Z13SliceIteratorI10JsonMemberE, ptr %struct.init, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %struct.init, i64 ptrtoint (ptr getelementptr (%_Z13SliceIteratorI10JsonMemberE, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN5SliceI10JsonMemberEC1Ev(ptr %0) {
entry:
  %data = getelementptr inbounds nuw %_Z5SliceI10JsonMemberE, ptr %0, i32 0, i32 1
  store ptr null, ptr %data, align 8
  %length = getelementptr inbounds nuw %_Z5SliceI10JsonMemberE, ptr %0, i32 0, i32 0
  store i64 0, ptr %length, align 8
  ret void
}

define linkonce_odr ptr @_ZN5SliceI9JsonValueE3getEm(ptr %0, i64 %1) {
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

define linkonce_odr ptr @_ZN5SliceI9JsonValueE2atEm(ptr %0, i64 %1) {
entry:
  %load.struct = load %_Z5SliceI9JsonValueE, ptr %0, align 8
  %length = extractvalue %_Z5SliceI9JsonValueE %load.struct, 0
  %ge = icmp uge i64 %1, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %field.inplace = getelementptr inbounds nuw %_Z5SliceI9JsonValueE, ptr %0, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  call void @_Z15scaly_panic_oobP10const_charmm(ptr @.str.22, i64 %1, i64 %field.val)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %load.struct1 = load %_Z5SliceI9JsonValueE, ptr %0, align 8
  %data = extractvalue %_Z5SliceI9JsonValueE %load.struct1, 1
  %ptr.add = getelementptr inbounds %_Z9JsonValue, ptr %data, i64 %1
  ret ptr %ptr.add
}

define linkonce_odr void @_ZN5SliceI9JsonValueE3putEm9JsonValue(ptr %0, i64 %1, ptr %2) {
entry:
  %load.struct = load %_Z5SliceI9JsonValueE, ptr %0, align 8
  %length = extractvalue %_Z5SliceI9JsonValueE %load.struct, 0
  %ge = icmp uge i64 %1, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %field.inplace = getelementptr inbounds nuw %_Z5SliceI9JsonValueE, ptr %0, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  call void @_Z15scaly_panic_oobP10const_charmm(ptr @.str.23, i64 %1, i64 %field.val)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %load.struct1 = load %_Z5SliceI9JsonValueE, ptr %0, align 8
  %data = extractvalue %_Z5SliceI9JsonValueE %load.struct1, 1
  %ptr.add = getelementptr inbounds %_Z9JsonValue, ptr %data, i64 %1
  %store.load = load %_Z9JsonValue, ptr %2, align 1
  store %_Z9JsonValue %store.load, ptr %ptr.add, align 1
  ret void
}

define linkonce_odr i1 @_ZN5SliceI9JsonValueE8is_emptyEv(ptr %0) {
entry:
  %load.struct = load %_Z5SliceI9JsonValueE, ptr %0, align 8
  %length = extractvalue %_Z5SliceI9JsonValueE %load.struct, 0
  %eq = icmp eq i64 %length, 0
  ret i1 %eq
}

define linkonce_odr void @_ZN5SliceI9JsonValueE8subsliceEPN4scaly6memory4PageEmm(ptr noalias sret(%_Z5SliceI9JsonValueE) %0, ptr %1, ptr %2, i64 %3, i64 %4) {
entry:
  %tuple = alloca %_Z5SliceI9JsonValueE, align 8
  %from = alloca i64, align 8
  store i64 %3, ptr %from, align 1
  %to = alloca i64, align 8
  store i64 %4, ptr %to, align 1
  %from1 = load i64, ptr %from, align 8
  %load.struct = load %_Z5SliceI9JsonValueE, ptr %2, align 8
  %length = extractvalue %_Z5SliceI9JsonValueE %load.struct, 0
  %gt = icmp ugt i64 %from1, %length
  br i1 %gt, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %load.struct2 = load %_Z5SliceI9JsonValueE, ptr %2, align 8
  %length3 = extractvalue %_Z5SliceI9JsonValueE %load.struct2, 0
  store i64 %length3, ptr %from, align 1
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %to4 = load i64, ptr %to, align 8
  %load.struct5 = load %_Z5SliceI9JsonValueE, ptr %2, align 8
  %length6 = extractvalue %_Z5SliceI9JsonValueE %load.struct5, 0
  %gt7 = icmp ugt i64 %to4, %length6
  br i1 %gt7, label %if.then8, label %if.end9

if.then8:                                         ; preds = %if.end
  %load.struct10 = load %_Z5SliceI9JsonValueE, ptr %2, align 8
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
  %load.struct20 = load %_Z5SliceI9JsonValueE, ptr %2, align 8
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

define linkonce_odr void @_ZN5SliceI9JsonValueE10slice_fromEPN4scaly6memory4PageEm(ptr noalias sret(%_Z5SliceI9JsonValueE) %0, ptr %1, ptr %2, i64 %3) {
entry:
  %sret.result = alloca %_Z5SliceI9JsonValueE, align 8
  %field.inplace = getelementptr inbounds nuw %_Z5SliceI9JsonValueE, ptr %2, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  call void @_ZN5SliceI9JsonValueE8subsliceEPN4scaly6memory4PageEmm(ptr noalias sret(%_Z5SliceI9JsonValueE) %sret.result, ptr null, ptr %2, i64 %3, i64 %field.val)
  %sret.body = load %_Z5SliceI9JsonValueE, ptr %sret.result, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %sret.result, i64 ptrtoint (ptr getelementptr (%_Z5SliceI9JsonValueE, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN5SliceI9JsonValueE8slice_toEPN4scaly6memory4PageEm(ptr noalias sret(%_Z5SliceI9JsonValueE) %0, ptr %1, ptr %2, i64 %3) {
entry:
  %sret.result = alloca %_Z5SliceI9JsonValueE, align 8
  call void @_ZN5SliceI9JsonValueE8subsliceEPN4scaly6memory4PageEmm(ptr noalias sret(%_Z5SliceI9JsonValueE) %sret.result, ptr null, ptr %2, i64 0, i64 %3)
  %sret.body = load %_Z5SliceI9JsonValueE, ptr %sret.result, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %sret.result, i64 ptrtoint (ptr getelementptr (%_Z5SliceI9JsonValueE, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr i1 @_ZN5SliceI9JsonValueE6equalsE5SliceI9JsonValueE(ptr %0, ptr %1) {
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

define linkonce_odr i1 @_ZN5SliceI9JsonValueE11starts_withE5SliceI9JsonValueE(ptr %0, ptr %1) {
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
  call void @_ZN5SliceI9JsonValueE8subsliceEPN4scaly6memory4PageEmm(ptr noalias sret(%_Z5SliceI9JsonValueE) %sret.result, ptr null, ptr %0, i64 0, i64 %field.val)
  %call = call i1 @_ZN5SliceI9JsonValueE6equalsE5SliceI9JsonValueE(ptr %sret.result, ptr %1)
  ret i1 %call
}

define linkonce_odr i1 @_ZN5SliceI9JsonValueE9ends_withE5SliceI9JsonValueE(ptr %0, ptr %1) {
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
  call void @_ZN5SliceI9JsonValueE8subsliceEPN4scaly6memory4PageEmm(ptr noalias sret(%_Z5SliceI9JsonValueE) %sret.result, ptr null, ptr %0, i64 %sub, i64 %field.val)
  %call = call i1 @_ZN5SliceI9JsonValueE6equalsE5SliceI9JsonValueE(ptr %sret.result, ptr %1)
  ret i1 %call
}

define linkonce_odr ptr @_ZN13SliceIteratorI9JsonValueE4nextEv(ptr %0) {
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

define linkonce_odr void @_ZN13SliceIteratorI9JsonValueEC1E5SliceI9JsonValueE(ptr %0, ptr %1) {
entry:
  %slice = getelementptr inbounds nuw %_Z13SliceIteratorI9JsonValueE, ptr %0, i32 0, i32 0
  %field.load = load %_Z5SliceI9JsonValueE, ptr %1, align 8
  store %_Z5SliceI9JsonValueE %field.load, ptr %slice, align 8
  %position = getelementptr inbounds nuw %_Z13SliceIteratorI9JsonValueE, ptr %0, i32 0, i32 1
  store i64 0, ptr %position, align 8
  ret void
}

define linkonce_odr void @_ZN5SliceI9JsonValueE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z13SliceIteratorI9JsonValueE) %0, ptr %1, ptr %2) {
entry:
  %struct.init = alloca %_Z13SliceIteratorI9JsonValueE, align 8
  call void @_ZN13SliceIteratorI9JsonValueEC1E5SliceI9JsonValueE(ptr %struct.init, ptr %2)
  %sret.body = load %_Z13SliceIteratorI9JsonValueE, ptr %struct.init, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %struct.init, i64 ptrtoint (ptr getelementptr (%_Z13SliceIteratorI9JsonValueE, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN5SliceI9JsonValueEC1Ev(ptr %0) {
entry:
  %data = getelementptr inbounds nuw %_Z5SliceI9JsonValueE, ptr %0, i32 0, i32 1
  store ptr null, ptr %data, align 8
  %length = getelementptr inbounds nuw %_Z5SliceI9JsonValueE, ptr %0, i32 0, i32 0
  store i64 0, ptr %length, align 8
  ret void
}

define linkonce_odr i1 @_ZN9JsonValue7is_nullEv(ptr %0) {
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

define linkonce_odr i1 @_ZN9JsonValue7is_boolEv(ptr %0) {
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

define linkonce_odr i1 @_ZN9JsonValue9is_numberEv(ptr %0) {
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
  %variant.val = load { ptr }, ptr %"variant.c_data().ptr", align 8
  ret i1 true
}

define linkonce_odr i1 @_ZN9JsonValue7is_textEv(ptr %0) {
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
  %variant.val = load { ptr }, ptr %"variant.c_data().ptr", align 8
  ret i1 true
}

define linkonce_odr i1 @_ZN9JsonValue8is_arrayEv(ptr %0) {
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

define linkonce_odr i1 @_ZN9JsonValue9is_objectEv(ptr %0) {
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

define linkonce_odr i1 @_ZN9JsonValue7as_boolEv(ptr %0) {
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

define linkonce_odr void @_ZN9JsonValue9as_stringEPN4scaly6memory4PageE(ptr noalias sret({ ptr }) %0, ptr %1, ptr %2) {
entry:
  %tag.ptr = getelementptr inbounds nuw %_Z9JsonValue, ptr %2, i32 0, i32 0
  %tag = load i8, ptr %tag.ptr, align 1
  switch i8 %tag, label %choose.else [
    i8 3, label %choose.when
  ]

choose.end:                                       ; No predecessors!
  ret void

choose.else:                                      ; preds = %entry
  %frame.page = load ptr, ptr %1, align 8
  %frame.has_page = icmp ne ptr %frame.page, null
  br i1 %frame.has_page, label %frame.forced, label %frame.force

choose.when:                                      ; preds = %entry
  %"variant.c_data().ptr" = getelementptr inbounds nuw %_Z9JsonValue, ptr %2, i32 0, i32 1
  %variant.val = load { ptr }, ptr %"variant.c_data().ptr", align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %"variant.c_data().ptr", i64 ptrtoint (ptr getelementptr ({ ptr }, ptr null, i32 1) to i64), i1 false)
  ret void

frame.force:                                      ; preds = %choose.else
  %forced_page = call ptr @_Z17scaly_force_frameP5Frame(ptr %1)
  br label %frame.forced

frame.forced:                                     ; preds = %frame.force, %choose.else
  %forced_page1 = phi ptr [ %frame.page, %choose.else ], [ %forced_page, %frame.force ]
  %struct.region = call ptr @_ZN4Page8allocateEmm(ptr %forced_page1, i64 ptrtoint (ptr getelementptr ({ ptr }, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr ({ i1, { ptr } }, ptr null, i64 0, i32 1) to i64))
  call void @_ZN6StringC1Ev(ptr %struct.region)
  %sret.body = load { ptr }, ptr %struct.region, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %struct.region, i64 ptrtoint (ptr getelementptr ({ ptr }, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN9JsonValue11number_textEPN4scaly6memory4PageE(ptr noalias sret({ ptr }) %0, ptr %1, ptr %2) {
entry:
  %tag.ptr = getelementptr inbounds nuw %_Z9JsonValue, ptr %2, i32 0, i32 0
  %tag = load i8, ptr %tag.ptr, align 1
  switch i8 %tag, label %choose.else [
    i8 2, label %choose.when
  ]

choose.end:                                       ; No predecessors!
  ret void

choose.else:                                      ; preds = %entry
  %frame.page = load ptr, ptr %1, align 8
  %frame.has_page = icmp ne ptr %frame.page, null
  br i1 %frame.has_page, label %frame.forced, label %frame.force

choose.when:                                      ; preds = %entry
  %"variant.c_data().ptr" = getelementptr inbounds nuw %_Z9JsonValue, ptr %2, i32 0, i32 1
  %variant.val = load { ptr }, ptr %"variant.c_data().ptr", align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %"variant.c_data().ptr", i64 ptrtoint (ptr getelementptr ({ ptr }, ptr null, i32 1) to i64), i1 false)
  ret void

frame.force:                                      ; preds = %choose.else
  %forced_page = call ptr @_Z17scaly_force_frameP5Frame(ptr %1)
  br label %frame.forced

frame.forced:                                     ; preds = %frame.force, %choose.else
  %forced_page1 = phi ptr [ %frame.page, %choose.else ], [ %forced_page, %frame.force ]
  %struct.region = call ptr @_ZN4Page8allocateEmm(ptr %forced_page1, i64 ptrtoint (ptr getelementptr ({ ptr }, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr ({ i1, { ptr } }, ptr null, i64 0, i32 1) to i64))
  call void @_ZN6StringC1Ev(ptr %struct.region)
  %sret.body = load { ptr }, ptr %struct.region, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %struct.region, i64 ptrtoint (ptr getelementptr ({ ptr }, ptr null, i32 1) to i64), i1 false)
  ret void
}

declare void @_ZN6String8as_sliceEPN4scaly6memory4PageE(ptr noalias sret(%_Z5SliceI2u8E), ptr, ptr)

define linkonce_odr i1 @_ZN9JsonValue12integer_fitsE5SliceI2u8E(ptr %0) {
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

define linkonce_odr i1 @_ZN9JsonValue10is_integerEv(ptr %0) {
entry:
  %arg.tmp = alloca { ptr }, align 8
  %frame = alloca { ptr, ptr }, align 8
  store ptr null, ptr %frame, align 8
  %frame.parent = getelementptr inbounds nuw { ptr, ptr }, ptr %frame, i32 0, i32 1
  store ptr null, ptr %frame.parent, align 8
  %sret.result = alloca %_Z5SliceI2u8E, align 8
  %tag.ptr = getelementptr inbounds nuw %_Z9JsonValue, ptr %0, i32 0, i32 0
  %tag = load i8, ptr %tag.ptr, align 1
  switch i8 %tag, label %choose.else [
    i8 2, label %choose.when
  ]

choose.end:                                       ; No predecessors!
  call void @_Z19scaly_release_frameP5Frame(ptr %frame)
  ret i1 false

choose.else:                                      ; preds = %entry
  call void @_Z19scaly_release_frameP5Frame(ptr %frame)
  ret i1 false

choose.when:                                      ; preds = %entry
  %"variant.c_data().ptr" = getelementptr inbounds nuw %_Z9JsonValue, ptr %0, i32 0, i32 1
  %variant.val = load { ptr }, ptr %"variant.c_data().ptr", align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %arg.tmp, ptr align 1 %"variant.c_data().ptr", i64 ptrtoint (ptr getelementptr ({ ptr }, ptr null, i32 1) to i64), i1 false)
  call void @_ZN6String8as_sliceEPN4scaly6memory4PageE(ptr noalias sret(%_Z5SliceI2u8E) %sret.result, ptr %frame, ptr %arg.tmp)
  %call = call i1 @_ZN9JsonValue12integer_fitsE5SliceI2u8E(ptr %sret.result)
  call void @_Z19scaly_release_frameP5Frame(ptr %frame)
  ret i1 %call
}

define linkonce_odr i64 @_ZN9JsonValue13integer_valueE5SliceI2u8E(ptr %0) {
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

define linkonce_odr i64 @_ZN9JsonValue6as_i64Ev(ptr %0) {
entry:
  %arg.tmp = alloca { ptr }, align 8
  %frame = alloca { ptr, ptr }, align 8
  store ptr null, ptr %frame, align 8
  %frame.parent = getelementptr inbounds nuw { ptr, ptr }, ptr %frame, i32 0, i32 1
  store ptr null, ptr %frame.parent, align 8
  %sret.result = alloca %_Z5SliceI2u8E, align 8
  %tag.ptr = getelementptr inbounds nuw %_Z9JsonValue, ptr %0, i32 0, i32 0
  %tag = load i8, ptr %tag.ptr, align 1
  switch i8 %tag, label %choose.else [
    i8 2, label %choose.when
  ]

choose.end:                                       ; No predecessors!
  call void @_Z19scaly_release_frameP5Frame(ptr %frame)
  ret i64 0

choose.else:                                      ; preds = %entry
  call void @_Z19scaly_release_frameP5Frame(ptr %frame)
  ret i64 0

choose.when:                                      ; preds = %entry
  %"variant.c_data().ptr" = getelementptr inbounds nuw %_Z9JsonValue, ptr %0, i32 0, i32 1
  %variant.val = load { ptr }, ptr %"variant.c_data().ptr", align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %arg.tmp, ptr align 1 %"variant.c_data().ptr", i64 ptrtoint (ptr getelementptr ({ ptr }, ptr null, i32 1) to i64), i1 false)
  call void @_ZN6String8as_sliceEPN4scaly6memory4PageE(ptr noalias sret(%_Z5SliceI2u8E) %sret.result, ptr %frame, ptr %arg.tmp)
  %call = call i1 @_ZN9JsonValue12integer_fitsE5SliceI2u8E(ptr %sret.result)
  br i1 %call, label %if.then, label %if.end

if.then:                                          ; preds = %choose.when
  %call1 = call i64 @_ZN9JsonValue13integer_valueE5SliceI2u8E(ptr %sret.result)
  call void @_Z19scaly_release_frameP5Frame(ptr %frame)
  ret i64 %call1

if.end:                                           ; preds = %choose.when
  call void @_Z19scaly_release_frameP5Frame(ptr %frame)
  ret i64 0
}

declare ptr @_ZN6String11to_c_stringEPN4scaly6memory4PageE(ptr, ptr)

define linkonce_odr double @_ZN9JsonValue6as_f64Ev(ptr %0) {
entry:
  %arg.tmp = alloca { ptr }, align 8
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
  %variant.val = load { ptr }, ptr %"variant.c_data().ptr", align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %arg.tmp, ptr align 1 %"variant.c_data().ptr", i64 ptrtoint (ptr getelementptr ({ ptr }, ptr null, i32 1) to i64), i1 false)
  %call = call ptr @_ZN6String11to_c_stringEPN4scaly6memory4PageE(ptr %frame, ptr %arg.tmp)
  %call1 = call double @strtod(ptr %call, ptr null)
  call void @_Z19scaly_release_frameP5Frame(ptr %frame)
  ret double %call1
}

define linkonce_odr i64 @_ZN9JsonValue6lengthEv(ptr %0) {
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

define linkonce_odr void @_ZN5SliceI9JsonValueEixEm(ptr noalias sret(%_Z9JsonValue) %0, ptr %1, i64 %2) {
entry:
  %deref.tmp = alloca %_Z9JsonValue, align 8
  %load.struct = load %_Z5SliceI9JsonValueE, ptr %1, align 8
  %length = extractvalue %_Z5SliceI9JsonValueE %load.struct, 0
  %ge = icmp uge i64 %2, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %field.inplace = getelementptr inbounds nuw %_Z5SliceI9JsonValueE, ptr %1, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  call void @_Z15scaly_panic_oobP10const_charmm(ptr @.str.24, i64 %2, i64 %field.val)
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

define linkonce_odr void @_ZN9JsonValue4itemEPN4scaly6memory4PageEm(ptr noalias sret(%_Z9JsonValue) %0, ptr %1, ptr %2, i64 %3) {
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
  call void @_ZN5SliceI9JsonValueEixEm(ptr noalias sret(%_Z9JsonValue) %sret.result, ptr %arg.tmp, i64 %3)
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

define linkonce_odr void @_ZN5SliceI10JsonMemberEixEm(ptr noalias sret(%_Z10JsonMember) %0, ptr %1, i64 %2) {
entry:
  %deref.tmp = alloca %_Z10JsonMember, align 8
  %load.struct = load %_Z5SliceI10JsonMemberE, ptr %1, align 8
  %length = extractvalue %_Z5SliceI10JsonMemberE %load.struct, 0
  %ge = icmp uge i64 %2, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %field.inplace = getelementptr inbounds nuw %_Z5SliceI10JsonMemberE, ptr %1, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  call void @_Z15scaly_panic_oobP10const_charmm(ptr @.str.25, i64 %2, i64 %field.val)
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

define linkonce_odr void @_ZN9JsonValue3getEPN4scaly6memory4PageE5SliceI2u8E(ptr noalias sret(%_Z9JsonValue) %0, ptr %1, ptr %2, ptr %3) {
entry:
  %variant.ptr7 = alloca %_Z9JsonValue, align 8
  %variant.ptr = alloca %_Z9JsonValue, align 8
  %frame = alloca { ptr, ptr }, align 8
  store ptr null, ptr %frame, align 8
  %frame.parent = getelementptr inbounds nuw { ptr, ptr }, ptr %frame, i32 0, i32 1
  store ptr %1, ptr %frame.parent, align 8
  %sret.result4 = alloca %_Z5SliceI2u8E, align 8
  %arg.tmp = alloca %_Z5SliceI10JsonMemberE, align 8
  %sret.result = alloca %_Z10JsonMember, align 8
  %i = alloca i64, align 8
  %tag.ptr = getelementptr inbounds nuw %_Z9JsonValue, ptr %2, i32 0, i32 0
  %tag = load i8, ptr %tag.ptr, align 1
  switch i8 %tag, label %choose.else [
    i8 5, label %choose.when
  ]

choose.end:                                       ; No predecessors!
  call void @_Z19scaly_release_frameP5Frame(ptr %frame)
  ret void

choose.else:                                      ; preds = %entry
  %variant.tag.ptr8 = getelementptr inbounds nuw %_Z9JsonValue, ptr %variant.ptr7, i32 0, i32 0
  store i8 0, ptr %variant.tag.ptr8, align 1
  %variant.val9 = load %_Z9JsonValue, ptr %variant.ptr7, align 1
  call void @_Z19scaly_release_frameP5Frame(ptr %frame)
  store %_Z9JsonValue %variant.val9, ptr %0, align 1
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
  call void @_ZN5SliceI10JsonMemberEixEm(ptr noalias sret(%_Z10JsonMember) %sret.result, ptr %arg.tmp, i64 %i3)
  %field.inplace = getelementptr inbounds nuw %_Z10JsonMember, ptr %sret.result, i32 0, i32 0
  call void @_ZN6String8as_sliceEPN4scaly6memory4PageE(ptr noalias sret(%_Z5SliceI2u8E) %sret.result4, ptr %frame, ptr %field.inplace)
  %call = call i1 @_ZN5SliceI2u8E6equalsE5SliceI2u8E(ptr %sret.result4, ptr %3)
  br i1 %call, label %if.then, label %if.end

while.exit:                                       ; preds = %while.cond
  %variant.tag.ptr = getelementptr inbounds nuw %_Z9JsonValue, ptr %variant.ptr, i32 0, i32 0
  store i8 0, ptr %variant.tag.ptr, align 1
  %variant.val6 = load %_Z9JsonValue, ptr %variant.ptr, align 1
  call void @_Z19scaly_release_frameP5Frame(ptr %frame)
  store %_Z9JsonValue %variant.val6, ptr %0, align 1
  ret void

if.then:                                          ; preds = %while.body
  %load.struct = load %_Z10JsonMember, ptr %sret.result, align 8
  %value = extractvalue %_Z10JsonMember %load.struct, 1
  call void @_Z19scaly_release_frameP5Frame(ptr %frame)
  store %_Z9JsonValue %value, ptr %0, align 1
  ret void

if.end:                                           ; preds = %while.body
  %i5 = load i64, ptr %i, align 8
  %add = add i64 %i5, 1
  store i64 %add, ptr %i, align 1
  br label %while.cond
}

define linkonce_odr i1 @_ZN9JsonValue3hasE5SliceI2u8E(ptr %0, ptr %1) {
entry:
  %arg.tmp5 = alloca { ptr }, align 8
  %arg.tmp = alloca %_Z5SliceI10JsonMemberE, align 8
  %sret.result2 = alloca %_Z10JsonMember, align 8
  %frame = alloca { ptr, ptr }, align 8
  store ptr null, ptr %frame, align 8
  %frame.parent = getelementptr inbounds nuw { ptr, ptr }, ptr %frame, i32 0, i32 1
  store ptr null, ptr %frame.parent, align 8
  %sret.result = alloca %_Z5SliceI2u8E, align 8
  %i = alloca i64, align 8
  %tag.ptr = getelementptr inbounds nuw %_Z9JsonValue, ptr %0, i32 0, i32 0
  %tag = load i8, ptr %tag.ptr, align 1
  switch i8 %tag, label %choose.else [
    i8 5, label %choose.when
  ]

choose.end:                                       ; No predecessors!
  call void @_Z19scaly_release_frameP5Frame(ptr %frame)
  ret i1 false

choose.else:                                      ; preds = %entry
  call void @_Z19scaly_release_frameP5Frame(ptr %frame)
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
  %members3 = extractvalue %_Z10JsonObject %variant.val, 0
  store %_Z5SliceI10JsonMemberE %members3, ptr %arg.tmp, align 1
  %i4 = load i64, ptr %i, align 8
  call void @_ZN5SliceI10JsonMemberEixEm(ptr noalias sret(%_Z10JsonMember) %sret.result2, ptr %arg.tmp, i64 %i4)
  %load.struct = load %_Z10JsonMember, ptr %sret.result2, align 8
  %key = extractvalue %_Z10JsonMember %load.struct, 0
  store { ptr } %key, ptr %arg.tmp5, align 1
  call void @_ZN6String8as_sliceEPN4scaly6memory4PageE(ptr noalias sret(%_Z5SliceI2u8E) %sret.result, ptr %frame, ptr %arg.tmp5)
  %call = call i1 @_ZN5SliceI2u8E6equalsE5SliceI2u8E(ptr %sret.result, ptr %1)
  br i1 %call, label %if.then, label %if.end

while.exit:                                       ; preds = %while.cond
  call void @_Z19scaly_release_frameP5Frame(ptr %frame)
  ret i1 false

if.then:                                          ; preds = %while.body
  call void @_Z19scaly_release_frameP5Frame(ptr %frame)
  ret i1 true

if.end:                                           ; preds = %while.body
  %i6 = load i64, ptr %i, align 8
  %add = add i64 %i6, 1
  store i64 %add, ptr %i, align 1
  br label %while.cond
}

define linkonce_odr void @_ZN9JsonValue6key_atEPN4scaly6memory4PageEm(ptr noalias sret({ ptr }) %0, ptr %1, ptr %2, i64 %3) {
entry:
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
  %frame.page3 = load ptr, ptr %1, align 8
  %frame.has_page4 = icmp ne ptr %frame.page3, null
  br i1 %frame.has_page4, label %frame.forced6, label %frame.force5

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
  call void @_ZN5SliceI10JsonMemberEixEm(ptr noalias sret(%_Z10JsonMember) %sret.result, ptr %arg.tmp, i64 %3)
  %load.struct = load %_Z10JsonMember, ptr %sret.result, align 8
  %key = extractvalue %_Z10JsonMember %load.struct, 0
  store { ptr } %key, ptr %0, align 1
  ret void

if.end:                                           ; preds = %choose.when
  %frame.page = load ptr, ptr %1, align 8
  %frame.has_page = icmp ne ptr %frame.page, null
  br i1 %frame.has_page, label %frame.forced, label %frame.force

frame.force:                                      ; preds = %if.end
  %forced_page = call ptr @_Z17scaly_force_frameP5Frame(ptr %1)
  br label %frame.forced

frame.forced:                                     ; preds = %frame.force, %if.end
  %forced_page2 = phi ptr [ %frame.page, %if.end ], [ %forced_page, %frame.force ]
  %struct.region = call ptr @_ZN4Page8allocateEmm(ptr %forced_page2, i64 ptrtoint (ptr getelementptr ({ ptr }, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr ({ i1, { ptr } }, ptr null, i64 0, i32 1) to i64))
  call void @_ZN6StringC1Ev(ptr %struct.region)
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

define linkonce_odr void @_ZN9JsonValue8value_atEPN4scaly6memory4PageEm(ptr noalias sret(%_Z9JsonValue) %0, ptr %1, ptr %2, i64 %3) {
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
  call void @_ZN5SliceI10JsonMemberEixEm(ptr noalias sret(%_Z10JsonMember) %sret.result, ptr %arg.tmp, i64 %3)
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

define linkonce_odr void @_ZN9JsonValue5buildEPN4scaly6memory4PageER10JsonReader9JsonToken(ptr noalias sret(%_Z9JsonValue) %0, ptr %1, ptr %2, ptr %3) {
entry:
  %sret.result12 = alloca %_Z5SliceI2u8E, align 8
  %sret.result11 = alloca %_Z5SliceI2u8E, align 8
  %sret.result6 = alloca { ptr }, align 8
  %sret.result = alloca %_Z9JsonValue, align 8
  %tag.ptr = getelementptr inbounds nuw %_Z9JsonToken, ptr %3, i32 0, i32 0
  %tag = load i8, ptr %tag.ptr, align 1
  switch i8 %tag, label %choose.else [
    i8 2, label %choose.when
    i8 4, label %choose.when1
    i8 7, label %choose.when4
    i8 8, label %choose.when7
    i8 9, label %choose.when17
    i8 10, label %choose.when22
  ]

choose.end:                                       ; No predecessors!
  ret void

choose.else:                                      ; preds = %entry
  %variant.tag.ptr27 = getelementptr inbounds nuw %_Z9JsonValue, ptr %sret.result, i32 0, i32 0
  store i8 0, ptr %variant.tag.ptr27, align 1
  %variant.val28 = load %_Z9JsonValue, ptr %sret.result, align 1
  store %_Z9JsonValue %variant.val28, ptr %0, align 1
  ret void

choose.when:                                      ; preds = %entry
  %"variant.c_data().ptr" = getelementptr inbounds nuw %_Z9JsonToken, ptr %3, i32 0, i32 1
  call void @_ZN9JsonValue12build_objectEPN4scaly6memory4PageER10JsonReader(ptr noalias sret(%_Z9JsonValue) %sret.result, ptr %1, ptr %2)
  %sret.body = load %_Z9JsonValue, ptr %sret.result, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %sret.result, i64 ptrtoint (ptr getelementptr (%_Z9JsonValue, ptr null, i32 1) to i64), i1 false)
  ret void

choose.when1:                                     ; preds = %entry
  %"variant.c_data().ptr2" = getelementptr inbounds nuw %_Z9JsonToken, ptr %3, i32 0, i32 1
  call void @_ZN9JsonValue11build_arrayEPN4scaly6memory4PageER10JsonReader(ptr noalias sret(%_Z9JsonValue) %sret.result, ptr %1, ptr %2)
  %sret.body3 = load %_Z9JsonValue, ptr %sret.result, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %sret.result, i64 ptrtoint (ptr getelementptr (%_Z9JsonValue, ptr null, i32 1) to i64), i1 false)
  ret void

choose.when4:                                     ; preds = %entry
  %"variant.c_data().ptr5" = getelementptr inbounds nuw %_Z9JsonToken, ptr %3, i32 0, i32 1
  %variant.tag.ptr = getelementptr inbounds nuw %_Z9JsonValue, ptr %sret.result, i32 0, i32 0
  store i8 3, ptr %variant.tag.ptr, align 1
  call void @_ZN10JsonReader6stringEPN4scaly6memory4PageE(ptr noalias sret({ ptr }) %sret.result6, ptr %1, ptr %2)
  %variant.data.ptr = getelementptr inbounds nuw %_Z9JsonValue, ptr %sret.result, i32 0, i32 1
  %variant.payload = load { ptr }, ptr %sret.result6, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %variant.data.ptr, ptr align 1 %sret.result6, i64 ptrtoint (ptr getelementptr ({ ptr }, ptr null, i32 1) to i64), i1 false)
  %variant.val = load %_Z9JsonValue, ptr %sret.result, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %sret.result, i64 ptrtoint (ptr getelementptr (%_Z9JsonValue, ptr null, i32 1) to i64), i1 false)
  ret void

choose.when7:                                     ; preds = %entry
  %"variant.c_data().ptr8" = getelementptr inbounds nuw %_Z9JsonToken, ptr %3, i32 0, i32 1
  %variant.tag.ptr9 = getelementptr inbounds nuw %_Z9JsonValue, ptr %sret.result, i32 0, i32 0
  store i8 2, ptr %variant.tag.ptr9, align 1
  %frame.page = load ptr, ptr %1, align 8
  %frame.has_page = icmp ne ptr %frame.page, null
  br i1 %frame.has_page, label %frame.forced, label %frame.force

frame.force:                                      ; preds = %choose.when7
  %forced_page = call ptr @_Z17scaly_force_frameP5Frame(ptr %1)
  br label %frame.forced

frame.forced:                                     ; preds = %frame.force, %choose.when7
  %forced_page10 = phi ptr [ %frame.page, %choose.when7 ], [ %forced_page, %frame.force ]
  %struct.region = call ptr @_ZN4Page8allocateEmm(ptr %forced_page10, i64 ptrtoint (ptr getelementptr ({ ptr }, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr ({ i1, { ptr } }, ptr null, i64 0, i32 1) to i64))
  call void @_ZN10JsonReader4textEPN4scaly6memory4PageE(ptr noalias sret(%_Z5SliceI2u8E) %sret.result11, ptr null, ptr %2)
  %load.struct = load %_Z5SliceI2u8E, ptr %sret.result11, align 8
  %data = extractvalue %_Z5SliceI2u8E %load.struct, 1
  call void @_ZN10JsonReader4textEPN4scaly6memory4PageE(ptr noalias sret(%_Z5SliceI2u8E) %sret.result12, ptr null, ptr %2)
  %load.struct13 = load %_Z5SliceI2u8E, ptr %sret.result12, align 8
  %length = extractvalue %_Z5SliceI2u8E %load.struct13, 0
  call void @_ZN6StringC1EP10const_charm(ptr %struct.region, ptr %data, i64 %length)
  %variant.data.ptr14 = getelementptr inbounds nuw %_Z9JsonValue, ptr %sret.result, i32 0, i32 1
  %variant.payload15 = load { ptr }, ptr %struct.region, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %variant.data.ptr14, ptr align 1 %struct.region, i64 ptrtoint (ptr getelementptr ({ ptr }, ptr null, i32 1) to i64), i1 false)
  %variant.val16 = load %_Z9JsonValue, ptr %sret.result, align 1
  store %_Z9JsonValue %variant.val16, ptr %0, align 1
  ret void

choose.when17:                                    ; preds = %entry
  %"variant.c_data().ptr18" = getelementptr inbounds nuw %_Z9JsonToken, ptr %3, i32 0, i32 1
  %variant.tag.ptr19 = getelementptr inbounds nuw %_Z9JsonValue, ptr %sret.result, i32 0, i32 0
  store i8 1, ptr %variant.tag.ptr19, align 1
  %variant.data.ptr20 = getelementptr inbounds nuw %_Z9JsonValue, ptr %sret.result, i32 0, i32 1
  store i1 true, ptr %variant.data.ptr20, align 1
  %variant.val21 = load %_Z9JsonValue, ptr %sret.result, align 1
  store %_Z9JsonValue %variant.val21, ptr %0, align 1
  ret void

choose.when22:                                    ; preds = %entry
  %"variant.c_data().ptr23" = getelementptr inbounds nuw %_Z9JsonToken, ptr %3, i32 0, i32 1
  %variant.tag.ptr24 = getelementptr inbounds nuw %_Z9JsonValue, ptr %sret.result, i32 0, i32 0
  store i8 1, ptr %variant.tag.ptr24, align 1
  %variant.data.ptr25 = getelementptr inbounds nuw %_Z9JsonValue, ptr %sret.result, i32 0, i32 1
  store i1 false, ptr %variant.data.ptr25, align 1
  %variant.val26 = load %_Z9JsonValue, ptr %sret.result, align 1
  store %_Z9JsonValue %variant.val26, ptr %0, align 1
  ret void
}

define linkonce_odr void @_ZN9JsonValue5parseEPN4scaly6memory4PageE5SliceI2u8E(ptr noalias sret(%_Z10JsonParsed) %0, ptr %1, ptr %2) {
entry:
  %tuple = alloca %_Z10JsonParsed, align 8
  %variant.ptr = alloca %_Z9JsonValue, align 8
  %frame = alloca { ptr, ptr }, align 8
  store ptr null, ptr %frame, align 8
  %frame.parent = getelementptr inbounds nuw { ptr, ptr }, ptr %frame, i32 0, i32 1
  store ptr %1, ptr %frame.parent, align 8
  %sret.result6 = alloca %_Z9JsonToken, align 8
  %sret.result = alloca %_Z10JsonReader, align 8
  call void @_ZN10JsonReader6createEPN4scaly6memory4PageE5SliceI2u8E(ptr noalias sret(%_Z10JsonReader) %sret.result, ptr null, ptr %2)
  %r = alloca ptr, align 8
  store ptr %sret.result, ptr %r, align 1
  %sret.result1 = alloca %_Z9JsonToken, align 8
  %r2 = load ptr, ptr %r, align 8
  call void @_ZN10JsonReader4nextEPN4scaly6memory4PageE(ptr noalias sret(%_Z9JsonToken) %sret.result1, ptr %1, ptr %r2)
  %sret.result3 = alloca %_Z9JsonValue, align 8
  %r4 = load ptr, ptr %r, align 8
  call void @_ZN9JsonValue5buildEPN4scaly6memory4PageER10JsonReader9JsonToken(ptr noalias sret(%_Z9JsonValue) %sret.result3, ptr %1, ptr %r4, ptr %sret.result1)
  %r5 = load ptr, ptr %r, align 8
  %call = call i1 @_ZN10JsonReader6failedEv(ptr %r5)
  %eq = icmp eq i1 %call, false
  br i1 %eq, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %r7 = load ptr, ptr %r, align 8
  call void @_ZN10JsonReader4nextEPN4scaly6memory4PageE(ptr noalias sret(%_Z9JsonToken) %sret.result6, ptr %frame, ptr %r7)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %r8 = load ptr, ptr %r, align 8
  %call9 = call i1 @_ZN10JsonReader6failedEv(ptr %r8)
  br i1 %call9, label %if.then10, label %if.end11

if.then10:                                        ; preds = %if.end
  %variant.tag.ptr = getelementptr inbounds nuw %_Z9JsonValue, ptr %variant.ptr, i32 0, i32 0
  store i8 0, ptr %variant.tag.ptr, align 1
  %variant.val = load %_Z9JsonValue, ptr %variant.ptr, align 1
  %r12 = load ptr, ptr %r, align 8
  %load.struct = load %_Z10JsonReader, ptr %r12, align 8
  %error = extractvalue %_Z10JsonReader %load.struct, 13
  %r13 = load ptr, ptr %r, align 8
  %load.struct14 = load %_Z10JsonReader, ptr %r13, align 8
  %error_at = extractvalue %_Z10JsonReader %load.struct14, 14
  %tuple.field = getelementptr inbounds nuw %_Z10JsonParsed, ptr %tuple, i32 0, i32 0
  store %_Z9JsonValue %variant.val, ptr %tuple.field, align 1
  %tuple.field15 = getelementptr inbounds nuw %_Z10JsonParsed, ptr %tuple, i32 0, i32 1
  store i64 %error, ptr %tuple.field15, align 1
  %tuple.field16 = getelementptr inbounds nuw %_Z10JsonParsed, ptr %tuple, i32 0, i32 2
  store i64 %error_at, ptr %tuple.field16, align 1
  %tuple.val = load %_Z10JsonParsed, ptr %tuple, align 8
  call void @_Z19scaly_release_frameP5Frame(ptr %frame)
  store %_Z10JsonParsed %tuple.val, ptr %0, align 1
  ret void

if.end11:                                         ; preds = %if.end
  %field.load = load %_Z9JsonValue, ptr %sret.result3, align 1
  %tuple.field17 = getelementptr inbounds nuw %_Z10JsonParsed, ptr %tuple, i32 0, i32 0
  store %_Z9JsonValue %field.load, ptr %tuple.field17, align 1
  %tuple.field18 = getelementptr inbounds nuw %_Z10JsonParsed, ptr %tuple, i32 0, i32 1
  store i64 0, ptr %tuple.field18, align 1
  %tuple.field19 = getelementptr inbounds nuw %_Z10JsonParsed, ptr %tuple, i32 0, i32 2
  store i64 0, ptr %tuple.field19, align 1
  %tuple.val20 = load %_Z10JsonParsed, ptr %tuple, align 8
  call void @_Z19scaly_release_frameP5Frame(ptr %frame)
  store %_Z10JsonParsed %tuple.val20, ptr %0, align 1
  ret void
}

define linkonce_odr void @_ZN9JsonValue12build_objectEPN4scaly6memory4PageER10JsonReader(ptr noalias sret(%_Z9JsonValue) %0, ptr %1, ptr %2) {
entry:
  %tuple = alloca %_Z10JsonObject, align 8
  %sret.result32 = alloca %_Z5SliceI10JsonMemberE, align 8
  %i = alloca i64, align 8
  %it = alloca ptr, align 8
  %sret.result25 = alloca %_Z12ListIteratorI10JsonMemberE, align 8
  %store = alloca ptr, align 8
  %sret.result5 = alloca %_Z9JsonValue, align 8
  %sret.result4 = alloca %_Z9JsonToken, align 8
  %sret.result3 = alloca { ptr }, align 8
  %frame = alloca { ptr, ptr }, align 8
  store ptr null, ptr %frame, align 8
  %frame.parent = getelementptr inbounds nuw { ptr, ptr }, ptr %frame, i32 0, i32 1
  store ptr %1, ptr %frame.parent, align 8
  %sret.result = alloca %_Z9JsonToken, align 8
  %count = alloca i64, align 8
  %members = alloca ptr, align 8
  %frame.page = load ptr, ptr %1, align 8
  %frame.has_page = icmp ne ptr %frame.page, null
  br i1 %frame.has_page, label %frame.forced, label %frame.force

frame.force:                                      ; preds = %entry
  %forced_page = call ptr @_Z17scaly_force_frameP5Frame(ptr %1)
  br label %frame.forced

frame.forced:                                     ; preds = %frame.force, %entry
  %forced_page1 = phi ptr [ %frame.page, %entry ], [ %forced_page, %frame.force ]
  %tuple.region = call ptr @_ZN4Page8allocateEmm(ptr %forced_page1, i64 ptrtoint (ptr getelementptr (%_Z4ListI10JsonMemberE, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr ({ i1, %_Z4ListI10JsonMemberE }, ptr null, i64 0, i32 1) to i64))
  %tuple.field = getelementptr inbounds nuw %_Z4ListI10JsonMemberE, ptr %tuple.region, i32 0, i32 0
  store ptr null, ptr %tuple.field, align 1
  %tuple.field2 = getelementptr inbounds nuw %_Z4ListI10JsonMemberE, ptr %tuple.region, i32 0, i32 1
  store ptr null, ptr %tuple.field2, align 1
  store ptr %tuple.region, ptr %members, align 1
  store i64 0, ptr %count, align 1
  br label %repeat.body

repeat.body:                                      ; preds = %choose.end, %frame.forced
  call void @_ZN10JsonReader4nextEPN4scaly6memory4PageE(ptr noalias sret(%_Z9JsonToken) %sret.result, ptr %frame, ptr %2)
  %tag.ptr = getelementptr inbounds nuw %_Z9JsonToken, ptr %sret.result, i32 0, i32 0
  %tag = load i8, ptr %tag.ptr, align 1
  switch i8 %tag, label %choose.else [
    i8 6, label %choose.when
  ]

repeat.exit:                                      ; preds = %choose.else
  %frame.page18 = load ptr, ptr %1, align 8
  %frame.has_page19 = icmp ne ptr %frame.page18, null
  br i1 %frame.has_page19, label %frame.forced21, label %frame.force20

choose.end:                                       ; preds = %frame.forced10
  br label %repeat.body

choose.else:                                      ; preds = %repeat.body
  br label %repeat.exit

choose.when:                                      ; preds = %repeat.body
  %"variant.c_data().ptr" = getelementptr inbounds nuw %_Z9JsonToken, ptr %sret.result, i32 0, i32 1
  call void @_ZN10JsonReader6stringEPN4scaly6memory4PageE(ptr noalias sret({ ptr }) %sret.result3, ptr %1, ptr %2)
  call void @_ZN10JsonReader4nextEPN4scaly6memory4PageE(ptr noalias sret(%_Z9JsonToken) %sret.result4, ptr %1, ptr %2)
  call void @_ZN9JsonValue5buildEPN4scaly6memory4PageER10JsonReader9JsonToken(ptr noalias sret(%_Z9JsonValue) %sret.result5, ptr %1, ptr %2, ptr %sret.result4)
  %members6 = load ptr, ptr %members, align 8
  %frame.page7 = load ptr, ptr %1, align 8
  %frame.has_page8 = icmp ne ptr %frame.page7, null
  br i1 %frame.has_page8, label %frame.forced10, label %frame.force9

frame.force9:                                     ; preds = %choose.when
  %forced_page11 = call ptr @_Z17scaly_force_frameP5Frame(ptr %1)
  br label %frame.forced10

frame.forced10:                                   ; preds = %frame.force9, %choose.when
  %forced_page12 = phi ptr [ %frame.page7, %choose.when ], [ %forced_page11, %frame.force9 ]
  %tuple.region13 = call ptr @_ZN4Page8allocateEmm(ptr %forced_page12, i64 ptrtoint (ptr getelementptr (%_Z10JsonMember, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr ({ i1, %_Z10JsonMember }, ptr null, i64 0, i32 1) to i64))
  %field.load = load { ptr }, ptr %sret.result3, align 8
  %tuple.field14 = getelementptr inbounds nuw %_Z10JsonMember, ptr %tuple.region13, i32 0, i32 0
  store { ptr } %field.load, ptr %tuple.field14, align 1
  %field.load15 = load %_Z9JsonValue, ptr %sret.result5, align 1
  %tuple.field16 = getelementptr inbounds nuw %_Z10JsonMember, ptr %tuple.region13, i32 0, i32 1
  store %_Z9JsonValue %field.load15, ptr %tuple.field16, align 1
  call void @_ZN4ListI10JsonMemberE3addE10JsonMember(ptr %members6, ptr %tuple.region13)
  %count17 = load i64, ptr %count, align 8
  %add = add i64 %count17, 1
  store i64 %add, ptr %count, align 1
  br label %choose.end

frame.force20:                                    ; preds = %repeat.exit
  %forced_page22 = call ptr @_Z17scaly_force_frameP5Frame(ptr %1)
  br label %frame.forced21

frame.forced21:                                   ; preds = %frame.force20, %repeat.exit
  %forced_page23 = phi ptr [ %frame.page18, %repeat.exit ], [ %forced_page22, %frame.force20 ]
  %struct.region = call ptr @_ZN4Page8allocateEmm(ptr %forced_page23, i64 ptrtoint (ptr getelementptr (%_Z6VectorI10JsonMemberE, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr ({ i1, %_Z6VectorI10JsonMemberE }, ptr null, i64 0, i32 1) to i64))
  %count24 = load i64, ptr %count, align 8
  call void @_ZN6VectorI10JsonMemberEC1Em(ptr %struct.region, i64 %count24)
  store ptr %struct.region, ptr %store, align 1
  %members26 = load ptr, ptr %members, align 8
  call void @_ZN4ListI10JsonMemberE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z12ListIteratorI10JsonMemberE) %sret.result25, ptr null, ptr %members26)
  store ptr %sret.result25, ptr %it, align 1
  store i64 0, ptr %i, align 1
  br label %while.cond

while.cond:                                       ; preds = %while.body, %frame.forced21
  %it27 = load ptr, ptr %it, align 8
  %call = call ptr @_ZN12ListIteratorI10JsonMemberE4nextEv(ptr %it27)
  %while.tobool = icmp ne ptr %call, null
  br i1 %while.tobool, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %store28 = load ptr, ptr %store, align 8
  %i29 = load i64, ptr %i, align 8
  call void @_ZN6VectorI10JsonMemberE3putEm10JsonMember(ptr %store28, i64 %i29, ptr %call)
  %i30 = load i64, ptr %i, align 8
  %add31 = add i64 %i30, 1
  store i64 %add31, ptr %i, align 1
  br label %while.cond

while.exit:                                       ; preds = %while.cond
  %variant.tag.ptr = getelementptr inbounds nuw %_Z9JsonValue, ptr %sret.result5, i32 0, i32 0
  store i8 5, ptr %variant.tag.ptr, align 1
  %store33 = load ptr, ptr %store, align 8
  call void @_ZN6VectorI10JsonMemberE8as_sliceEPN4scaly6memory4PageE(ptr noalias sret(%_Z5SliceI10JsonMemberE) %sret.result32, ptr null, ptr %store33)
  %field.load34 = load %_Z5SliceI10JsonMemberE, ptr %sret.result32, align 8
  %tuple.field35 = getelementptr inbounds nuw %_Z10JsonObject, ptr %tuple, i32 0, i32 0
  store %_Z5SliceI10JsonMemberE %field.load34, ptr %tuple.field35, align 1
  %tuple.val = load %_Z10JsonObject, ptr %tuple, align 8
  %variant.data.ptr = getelementptr inbounds nuw %_Z9JsonValue, ptr %sret.result5, i32 0, i32 1
  store %_Z10JsonObject %tuple.val, ptr %variant.data.ptr, align 1
  %variant.val = load %_Z9JsonValue, ptr %sret.result5, align 1
  call void @_Z19scaly_release_frameP5Frame(ptr %frame)
  store %_Z9JsonValue %variant.val, ptr %0, align 1
  ret void
}

define linkonce_odr void @_ZN9JsonValue11build_arrayEPN4scaly6memory4PageER10JsonReader(ptr noalias sret(%_Z9JsonValue) %0, ptr %1, ptr %2) {
entry:
  %tuple = alloca %_Z9JsonArray, align 8
  %sret.result24 = alloca %_Z5SliceI9JsonValueE, align 8
  %variant.ptr = alloca %_Z9JsonValue, align 8
  %i = alloca i64, align 8
  %it = alloca ptr, align 8
  %sret.result17 = alloca %_Z12ListIteratorI9JsonValueE, align 8
  %store = alloca ptr, align 8
  %sret.result8 = alloca %_Z9JsonValue, align 8
  %sret.result = alloca %_Z9JsonToken, align 8
  %count = alloca i64, align 8
  %items = alloca ptr, align 8
  %frame.page = load ptr, ptr %1, align 8
  %frame.has_page = icmp ne ptr %frame.page, null
  br i1 %frame.has_page, label %frame.forced, label %frame.force

frame.force:                                      ; preds = %entry
  %forced_page = call ptr @_Z17scaly_force_frameP5Frame(ptr %1)
  br label %frame.forced

frame.forced:                                     ; preds = %frame.force, %entry
  %forced_page1 = phi ptr [ %frame.page, %entry ], [ %forced_page, %frame.force ]
  %tuple.region = call ptr @_ZN4Page8allocateEmm(ptr %forced_page1, i64 ptrtoint (ptr getelementptr (%_Z4ListI9JsonValueE, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr ({ i1, %_Z4ListI9JsonValueE }, ptr null, i64 0, i32 1) to i64))
  %tuple.field = getelementptr inbounds nuw %_Z4ListI9JsonValueE, ptr %tuple.region, i32 0, i32 0
  store ptr null, ptr %tuple.field, align 1
  %tuple.field2 = getelementptr inbounds nuw %_Z4ListI9JsonValueE, ptr %tuple.region, i32 0, i32 1
  store ptr null, ptr %tuple.field2, align 1
  store ptr %tuple.region, ptr %items, align 1
  store i64 0, ptr %count, align 1
  br label %repeat.body

repeat.body:                                      ; preds = %choose.end, %frame.forced
  call void @_ZN10JsonReader4nextEPN4scaly6memory4PageE(ptr noalias sret(%_Z9JsonToken) %sret.result, ptr %1, ptr %2)
  %tag.ptr = getelementptr inbounds nuw %_Z9JsonToken, ptr %sret.result, i32 0, i32 0
  %tag = load i8, ptr %tag.ptr, align 1
  switch i8 %tag, label %choose.else [
    i8 5, label %choose.when
    i8 1, label %choose.when3
    i8 0, label %choose.when5
  ]

repeat.exit:                                      ; preds = %choose.when5, %choose.when3, %choose.when
  %frame.page10 = load ptr, ptr %1, align 8
  %frame.has_page11 = icmp ne ptr %frame.page10, null
  br i1 %frame.has_page11, label %frame.forced13, label %frame.force12

choose.end:                                       ; preds = %choose.else
  br label %repeat.body

choose.else:                                      ; preds = %repeat.body
  %items7 = load ptr, ptr %items, align 8
  call void @_ZN9JsonValue5buildEPN4scaly6memory4PageER10JsonReader9JsonToken(ptr noalias sret(%_Z9JsonValue) %sret.result8, ptr %1, ptr %2, ptr %sret.result)
  call void @_ZN4ListI9JsonValueE3addE9JsonValue(ptr %items7, ptr %sret.result8)
  %count9 = load i64, ptr %count, align 8
  %add = add i64 %count9, 1
  store i64 %add, ptr %count, align 1
  br label %choose.end

choose.when:                                      ; preds = %repeat.body
  %"variant.c_data().ptr" = getelementptr inbounds nuw %_Z9JsonToken, ptr %sret.result, i32 0, i32 1
  br label %repeat.exit

choose.when3:                                     ; preds = %repeat.body
  %"variant.c_data().ptr4" = getelementptr inbounds nuw %_Z9JsonToken, ptr %sret.result, i32 0, i32 1
  br label %repeat.exit

choose.when5:                                     ; preds = %repeat.body
  %"variant.c_data().ptr6" = getelementptr inbounds nuw %_Z9JsonToken, ptr %sret.result, i32 0, i32 1
  br label %repeat.exit

frame.force12:                                    ; preds = %repeat.exit
  %forced_page14 = call ptr @_Z17scaly_force_frameP5Frame(ptr %1)
  br label %frame.forced13

frame.forced13:                                   ; preds = %frame.force12, %repeat.exit
  %forced_page15 = phi ptr [ %frame.page10, %repeat.exit ], [ %forced_page14, %frame.force12 ]
  %struct.region = call ptr @_ZN4Page8allocateEmm(ptr %forced_page15, i64 ptrtoint (ptr getelementptr (%_Z6VectorI9JsonValueE, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr ({ i1, %_Z6VectorI9JsonValueE }, ptr null, i64 0, i32 1) to i64))
  %count16 = load i64, ptr %count, align 8
  call void @_ZN6VectorI9JsonValueEC1Em(ptr %struct.region, i64 %count16)
  store ptr %struct.region, ptr %store, align 1
  %items18 = load ptr, ptr %items, align 8
  call void @_ZN4ListI9JsonValueE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z12ListIteratorI9JsonValueE) %sret.result17, ptr null, ptr %items18)
  store ptr %sret.result17, ptr %it, align 1
  store i64 0, ptr %i, align 1
  br label %while.cond

while.cond:                                       ; preds = %while.body, %frame.forced13
  %it19 = load ptr, ptr %it, align 8
  %call = call ptr @_ZN12ListIteratorI9JsonValueE4nextEv(ptr %it19)
  %while.tobool = icmp ne ptr %call, null
  br i1 %while.tobool, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %store20 = load ptr, ptr %store, align 8
  %i21 = load i64, ptr %i, align 8
  call void @_ZN6VectorI9JsonValueE3putEm9JsonValue(ptr %store20, i64 %i21, ptr %call)
  %i22 = load i64, ptr %i, align 8
  %add23 = add i64 %i22, 1
  store i64 %add23, ptr %i, align 1
  br label %while.cond

while.exit:                                       ; preds = %while.cond
  %variant.tag.ptr = getelementptr inbounds nuw %_Z9JsonValue, ptr %variant.ptr, i32 0, i32 0
  store i8 4, ptr %variant.tag.ptr, align 1
  %store25 = load ptr, ptr %store, align 8
  call void @_ZN6VectorI9JsonValueE8as_sliceEPN4scaly6memory4PageE(ptr noalias sret(%_Z5SliceI9JsonValueE) %sret.result24, ptr null, ptr %store25)
  %field.load = load %_Z5SliceI9JsonValueE, ptr %sret.result24, align 8
  %tuple.field26 = getelementptr inbounds nuw %_Z9JsonArray, ptr %tuple, i32 0, i32 0
  store %_Z5SliceI9JsonValueE %field.load, ptr %tuple.field26, align 1
  %tuple.val = load %_Z9JsonArray, ptr %tuple, align 8
  %variant.data.ptr = getelementptr inbounds nuw %_Z9JsonValue, ptr %variant.ptr, i32 0, i32 1
  store %_Z9JsonArray %tuple.val, ptr %variant.data.ptr, align 1
  %variant.val = load %_Z9JsonValue, ptr %variant.ptr, align 1
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %variant.ptr, i64 ptrtoint (ptr getelementptr (%_Z9JsonValue, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr ptr @_ZN4ListI10JsonMemberE8get_headEv(ptr %0) {
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

define linkonce_odr ptr @_ZN12ListIteratorI10JsonMemberE4nextEv(ptr %0) {
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

define linkonce_odr i64 @_ZN4ListI10JsonMemberE5countEv(ptr %0) {
entry:
  %sret.result = alloca %_Z12ListIteratorI10JsonMemberE, align 8
  call void @_ZN4ListI10JsonMemberE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z12ListIteratorI10JsonMemberE) %sret.result, ptr null, ptr %0)
  %list_iterator = alloca ptr, align 8
  store ptr %sret.result, ptr %list_iterator, align 1
  %i = alloca i64, align 8
  store i64 0, ptr %i, align 1
  br label %while.cond

while.cond:                                       ; preds = %while.body, %entry
  %list_iterator1 = load ptr, ptr %list_iterator, align 8
  %call = call ptr @_ZN12ListIteratorI10JsonMemberE4nextEv(ptr %list_iterator1)
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

define linkonce_odr void @_ZN4ListI10JsonMemberE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z12ListIteratorI10JsonMemberE) %0, ptr %1, ptr %2) {
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

define linkonce_odr void @_ZN4ListI10JsonMemberE4linkEP4NodeI10JsonMemberE(ptr %0, ptr %1) {
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

define linkonce_odr void @_ZN4ListI10JsonMemberE3addE10JsonMember(ptr %0, ptr %1) {
entry:
  %own_page = call ptr @_Z3getPv(ptr %0)
  %tuple.region = call ptr @_ZN4Page8allocateEmm(ptr %own_page, i64 ptrtoint (ptr getelementptr (%_Z4NodeI10JsonMemberE, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr ({ i1, %_Z4NodeI10JsonMemberE }, ptr null, i64 0, i32 1) to i64))
  %field.load = load %_Z10JsonMember, ptr %1, align 8
  %tuple.field = getelementptr inbounds nuw %_Z4NodeI10JsonMemberE, ptr %tuple.region, i32 0, i32 0
  store %_Z10JsonMember %field.load, ptr %tuple.field, align 1
  %tuple.field1 = getelementptr inbounds nuw %_Z4NodeI10JsonMemberE, ptr %tuple.region, i32 0, i32 1
  store ptr null, ptr %tuple.field1, align 1
  call void @_ZN4ListI10JsonMemberE4linkEP4NodeI10JsonMemberE(ptr %0, ptr %tuple.region)
  ret void
}

define linkonce_odr void @_ZN4ListI10JsonMemberE6add_onER4Page10JsonMember(ptr %0, ptr %1, ptr %2) {
entry:
  %tuple.region = call ptr @_ZN4Page8allocateEmm(ptr %1, i64 ptrtoint (ptr getelementptr (%_Z4NodeI10JsonMemberE, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr ({ i1, %_Z4NodeI10JsonMemberE }, ptr null, i64 0, i32 1) to i64))
  %field.load = load %_Z10JsonMember, ptr %2, align 8
  %tuple.field = getelementptr inbounds nuw %_Z4NodeI10JsonMemberE, ptr %tuple.region, i32 0, i32 0
  store %_Z10JsonMember %field.load, ptr %tuple.field, align 1
  %tuple.field1 = getelementptr inbounds nuw %_Z4NodeI10JsonMemberE, ptr %tuple.region, i32 0, i32 1
  store ptr null, ptr %tuple.field1, align 1
  call void @_ZN4ListI10JsonMemberE4linkEP4NodeI10JsonMemberE(ptr %0, ptr %tuple.region)
  ret void
}

define linkonce_odr ptr @_ZN6VectorI10JsonMemberE3getEm(ptr noalias %0, i64 %1) {
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

define linkonce_odr ptr @_ZN6VectorI10JsonMemberE2atEm(ptr noalias %0, i64 %1) {
entry:
  %load.struct = load %_Z6VectorI10JsonMemberE, ptr %0, align 8
  %length = extractvalue %_Z6VectorI10JsonMemberE %load.struct, 0
  %ge = icmp uge i64 %1, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %field.inplace = getelementptr inbounds nuw %_Z6VectorI10JsonMemberE, ptr %0, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  call void @_Z15scaly_panic_oobP10const_charmm(ptr @.str.26, i64 %1, i64 %field.val)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %load.struct1 = load %_Z6VectorI10JsonMemberE, ptr %0, align 8
  %data = extractvalue %_Z6VectorI10JsonMemberE %load.struct1, 1
  %ptr.add = getelementptr inbounds %_Z10JsonMember, ptr %data, i64 %1
  ret ptr %ptr.add
}

define linkonce_odr ptr @_ZN6VectorI10JsonMemberE7get_ptrEm(ptr noalias %0, i64 %1) {
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

define linkonce_odr void @_ZN6VectorI10JsonMemberE3putEm10JsonMember(ptr %0, i64 %1, ptr %2) {
entry:
  %load.struct = load %_Z6VectorI10JsonMemberE, ptr %0, align 8
  %length = extractvalue %_Z6VectorI10JsonMemberE %load.struct, 0
  %ge = icmp uge i64 %1, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %field.inplace = getelementptr inbounds nuw %_Z6VectorI10JsonMemberE, ptr %0, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  call void @_Z15scaly_panic_oobP10const_charmm(ptr @.str.27, i64 %1, i64 %field.val)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %load.struct1 = load %_Z6VectorI10JsonMemberE, ptr %0, align 8
  %data = extractvalue %_Z6VectorI10JsonMemberE %load.struct1, 1
  %ptr.add = getelementptr inbounds %_Z10JsonMember, ptr %data, i64 %1
  %store.load = load %_Z10JsonMember, ptr %2, align 8
  store %_Z10JsonMember %store.load, ptr %ptr.add, align 8
  ret void
}

define linkonce_odr ptr @_ZN14VectorIteratorI10JsonMemberE4nextEv(ptr %0) {
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
  %call = call ptr @_ZN6VectorI10JsonMemberE7get_ptrEm(ptr %deref.recv, i64 %sub)
  ret ptr %call
}

define linkonce_odr void @_ZN14VectorIteratorI10JsonMemberEC1E6OptionIR6VectorI10JsonMemberEE(ptr %0, ptr %1) {
entry:
  %vector = getelementptr inbounds nuw %_Z14VectorIteratorI10JsonMemberE, ptr %0, i32 0, i32 0
  store ptr %1, ptr %vector, align 8
  %position = getelementptr inbounds nuw %_Z14VectorIteratorI10JsonMemberE, ptr %0, i32 0, i32 1
  store i64 0, ptr %position, align 8
  ret void
}

define linkonce_odr void @_ZN6VectorI10JsonMemberE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z14VectorIteratorI10JsonMemberE) %0, ptr %1, ptr noalias %2) {
entry:
  %struct.init = alloca %_Z14VectorIteratorI10JsonMemberE, align 8
  call void @_ZN14VectorIteratorI10JsonMemberEC1E6OptionIR6VectorI10JsonMemberEE(ptr %struct.init, ptr %2)
  %sret.body = load %_Z14VectorIteratorI10JsonMemberE, ptr %struct.init, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %struct.init, i64 ptrtoint (ptr getelementptr (%_Z14VectorIteratorI10JsonMemberE, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN6VectorI10JsonMemberE8as_sliceEPN4scaly6memory4PageE(ptr noalias sret(%_Z5SliceI10JsonMemberE) %0, ptr %1, ptr noalias %2) {
entry:
  %load.struct = load %_Z6VectorI10JsonMemberE, ptr %2, align 8
  %length = extractvalue %_Z6VectorI10JsonMemberE %load.struct, 0
  %load.struct1 = load %_Z6VectorI10JsonMemberE, ptr %2, align 8
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

define linkonce_odr void @_ZN6VectorI10JsonMemberEC1Ev(ptr noalias %0) {
entry:
  %length = getelementptr inbounds nuw %_Z6VectorI10JsonMemberE, ptr %0, i32 0, i32 0
  store i64 0, ptr %length, align 8
  %data = getelementptr inbounds nuw %_Z6VectorI10JsonMemberE, ptr %0, i32 0, i32 1
  store ptr null, ptr %data, align 8
  ret void
}

declare ptr @_ZN4Page3getEPv(ptr)

declare ptr @_ZN4Page8allocateEmm(ptr, i64, i64)

define linkonce_odr void @_ZN6VectorI10JsonMemberEC1Em(ptr %0, i64 %1) {
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

define linkonce_odr void @_ZN6VectorI10JsonMemberEC1E6VectorI10JsonMemberE(ptr %0, ptr %1) {
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

define linkonce_odr ptr @_ZN5ArrayI10JsonMemberE10get_bufferEv(ptr %0) {
entry:
  %load.struct = load %_Z5ArrayI10JsonMemberE, ptr %0, align 8
  %buffer = extractvalue %_Z5ArrayI10JsonMemberE %load.struct, 2
  ret ptr %buffer
}

define linkonce_odr i64 @_ZN5ArrayI10JsonMemberE10get_lengthEv(ptr %0) {
entry:
  %load.struct = load %_Z5ArrayI10JsonMemberE, ptr %0, align 8
  %length = extractvalue %_Z5ArrayI10JsonMemberE %load.struct, 0
  ret i64 %length
}

define linkonce_odr i64 @_ZN5ArrayI10JsonMemberE12get_capacityEv(ptr %0) {
entry:
  %load.struct = load %_Z5ArrayI10JsonMemberE, ptr %0, align 8
  %capacity = extractvalue %_Z5ArrayI10JsonMemberE %load.struct, 1
  ret i64 %capacity
}

define linkonce_odr void @_ZN5ArrayI10JsonMemberE10reallocateEv(ptr %0) {
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

define linkonce_odr void @_ZN5ArrayI10JsonMemberE3addE10JsonMember(ptr %0, ptr %1) {
entry:
  %load.struct = load %_Z5ArrayI10JsonMemberE, ptr %0, align 8
  %buffer = extractvalue %_Z5ArrayI10JsonMemberE %load.struct, 2
  %eq = icmp eq ptr %buffer, null
  br i1 %eq, label %if.then, label %lor.rhs

if.then:                                          ; preds = %lor.rhs, %entry
  call void @_ZN5ArrayI10JsonMemberE10reallocateEv(ptr %0)
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

declare ptr @_ZN4Page25allocate_exclusive_bufferEmm(ptr, i64, i64)

declare void @_ZN4Page25deallocate_exclusive_pageER4Page(ptr, ptr)

define linkonce_odr void @_ZN5ArrayI10JsonMemberE3addE6VectorI10JsonMemberE(ptr %0, ptr %1) {
entry:
  %load.struct = load %_Z5ArrayI10JsonMemberE, ptr %0, align 8
  %length = extractvalue %_Z5ArrayI10JsonMemberE %load.struct, 0
  %load.struct1 = load %_Z6VectorI10JsonMemberE, ptr %1, align 8
  %length2 = extractvalue %_Z6VectorI10JsonMemberE %load.struct1, 0
  %add = add i64 %length, %length2
  %new_length = alloca i64, align 8
  store i64 %add, ptr %new_length, align 1
  %new_length3 = load i64, ptr %new_length, align 8
  %load.struct4 = load %_Z5ArrayI10JsonMemberE, ptr %0, align 8
  %length5 = extractvalue %_Z5ArrayI10JsonMemberE %load.struct4, 0
  %lt = icmp ult i64 %new_length3, %length5
  br i1 %lt, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %field.inplace = getelementptr inbounds nuw %_Z6VectorI10JsonMemberE, ptr %1, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  %field.inplace6 = getelementptr inbounds nuw %_Z5ArrayI10JsonMemberE, ptr %0, i32 0, i32 0
  %field.val7 = load i64, ptr %field.inplace6, align 8
  call void @_Z16scaly_panic_sizeP10const_charmm(ptr @.str.28, i64 %field.val, i64 %field.val7)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %load.struct10 = load %_Z5ArrayI10JsonMemberE, ptr %0, align 8
  %buffer = extractvalue %_Z5ArrayI10JsonMemberE %load.struct10, 2
  %eq = icmp eq ptr %buffer, null
  br i1 %eq, label %if.then8, label %lor.rhs

if.then8:                                         ; preds = %lor.rhs, %if.end
  call void @_ZN5ArrayI10JsonMemberE10reallocateEv(ptr %0)
  br label %if.end9

if.end9:                                          ; preds = %if.then8, %lor.rhs
  %new_length13 = load i64, ptr %new_length, align 8
  %load.struct14 = load %_Z5ArrayI10JsonMemberE, ptr %0, align 8
  %capacity15 = extractvalue %_Z5ArrayI10JsonMemberE %load.struct14, 1
  %gt16 = icmp ugt i64 %new_length13, %capacity15
  br i1 %gt16, label %if.then17, label %if.end18

lor.rhs:                                          ; preds = %if.end
  %new_length11 = load i64, ptr %new_length, align 8
  %load.struct12 = load %_Z5ArrayI10JsonMemberE, ptr %0, align 8
  %capacity = extractvalue %_Z5ArrayI10JsonMemberE %load.struct12, 1
  %gt = icmp ugt i64 %new_length11, %capacity
  br i1 %gt, label %if.then8, label %if.end9

if.then17:                                        ; preds = %if.end9
  %call = call ptr @_ZN4Page3getEPv(ptr %0)
  %load.struct19 = load %_Z5ArrayI10JsonMemberE, ptr %0, align 8
  %buffer20 = extractvalue %_Z5ArrayI10JsonMemberE %load.struct19, 2
  %load.struct21 = load %_Z5ArrayI10JsonMemberE, ptr %0, align 8
  %capacity22 = extractvalue %_Z5ArrayI10JsonMemberE %load.struct21, 1
  %mul = mul i64 %capacity22, ptrtoint (ptr getelementptr (%_Z10JsonMember, ptr null, i32 1) to i64)
  %le = icmp ule i64 %mul, 1024
  %new_length23 = load i64, ptr %new_length, align 8
  %mul24 = mul i64 %new_length23, ptrtoint (ptr getelementptr (%_Z10JsonMember, ptr null, i32 1) to i64)
  %le25 = icmp ule i64 %mul24, 1024
  br i1 %le25, label %if.then26, label %if.else

if.end18:                                         ; preds = %if.end50, %if.end9
  %load.struct52 = load %_Z6VectorI10JsonMemberE, ptr %1, align 8
  %length53 = extractvalue %_Z6VectorI10JsonMemberE %load.struct52, 0
  %gt54 = icmp ugt i64 %length53, 0
  br i1 %gt54, label %if.then55, label %if.end56

if.then26:                                        ; preds = %if.then17
  %new_length28 = load i64, ptr %new_length, align 8
  %mul29 = mul i64 %new_length28, ptrtoint (ptr getelementptr (%_Z10JsonMember, ptr null, i32 1) to i64)
  %call30 = call ptr @_ZN4Page8allocateEmm(ptr %call, i64 %mul29, i64 8)
  %buffer31 = getelementptr inbounds nuw %_Z5ArrayI10JsonMemberE, ptr %0, i32 0, i32 2
  store ptr %call30, ptr %buffer31, align 8
  br label %if.end27

if.else:                                          ; preds = %if.then17
  %new_length32 = load i64, ptr %new_length, align 8
  %mul33 = mul i64 %new_length32, ptrtoint (ptr getelementptr (%_Z10JsonMember, ptr null, i32 1) to i64)
  %call34 = call ptr @_ZN4Page25allocate_exclusive_bufferEmm(ptr %call, i64 %mul33, i64 8)
  %buffer35 = getelementptr inbounds nuw %_Z5ArrayI10JsonMemberE, ptr %0, i32 0, i32 2
  store ptr %call34, ptr %buffer35, align 8
  br label %if.end27

if.end27:                                         ; preds = %if.else, %if.then26
  %if.value = phi ptr [ %call30, %if.then26 ], [ %call34, %if.else ]
  %new_length36 = load i64, ptr %new_length, align 8
  %capacity37 = getelementptr inbounds nuw %_Z5ArrayI10JsonMemberE, ptr %0, i32 0, i32 1
  store i64 %new_length36, ptr %capacity37, align 8
  %load.struct38 = load %_Z5ArrayI10JsonMemberE, ptr %0, align 8
  %length39 = extractvalue %_Z5ArrayI10JsonMemberE %load.struct38, 0
  %gt40 = icmp ugt i64 %length39, 0
  br i1 %gt40, label %if.then41, label %if.end42

if.then41:                                        ; preds = %if.end27
  %field.inplace43 = getelementptr inbounds nuw %_Z5ArrayI10JsonMemberE, ptr %0, i32 0, i32 2
  %deref.recv = load ptr, ptr %field.inplace43, align 8
  %load.struct44 = load %_Z5ArrayI10JsonMemberE, ptr %0, align 8
  %length45 = extractvalue %_Z5ArrayI10JsonMemberE %load.struct44, 0
  %mul46 = mul i64 %length45, ptrtoint (ptr getelementptr (%_Z10JsonMember, ptr null, i32 1) to i64)
  %call47 = call ptr @memcpy(ptr %deref.recv, ptr %buffer20, i64 %mul46)
  br label %if.end42

if.end42:                                         ; preds = %if.then41, %if.end27
  %eq48 = icmp eq i1 %le, false
  br i1 %eq48, label %if.then49, label %if.end50

if.then49:                                        ; preds = %if.end42
  %call51 = call ptr @_ZN4Page3getEPv(ptr %buffer20)
  call void @_ZN4Page25deallocate_exclusive_pageER4Page(ptr %call, ptr %call51)
  br label %if.end50

if.end50:                                         ; preds = %if.then49, %if.end42
  br label %if.end18

if.then55:                                        ; preds = %if.end18
  %load.struct57 = load %_Z5ArrayI10JsonMemberE, ptr %0, align 8
  %buffer58 = extractvalue %_Z5ArrayI10JsonMemberE %load.struct57, 2
  %load.struct59 = load %_Z5ArrayI10JsonMemberE, ptr %0, align 8
  %length60 = extractvalue %_Z5ArrayI10JsonMemberE %load.struct59, 0
  %ptr.add = getelementptr inbounds %_Z10JsonMember, ptr %buffer58, i64 %length60
  %field.inplace61 = getelementptr inbounds nuw %_Z6VectorI10JsonMemberE, ptr %1, i32 0, i32 1
  %deref.recv62 = load ptr, ptr %field.inplace61, align 8
  %load.struct63 = load %_Z6VectorI10JsonMemberE, ptr %1, align 8
  %length64 = extractvalue %_Z6VectorI10JsonMemberE %load.struct63, 0
  %mul65 = mul i64 %length64, ptrtoint (ptr getelementptr (%_Z10JsonMember, ptr null, i32 1) to i64)
  %call66 = call ptr @memcpy(ptr %ptr.add, ptr %deref.recv62, i64 %mul65)
  br label %if.end56

if.end56:                                         ; preds = %if.then55, %if.end18
  %load.struct67 = load %_Z5ArrayI10JsonMemberE, ptr %0, align 8
  %length68 = extractvalue %_Z5ArrayI10JsonMemberE %load.struct67, 0
  %load.struct69 = load %_Z6VectorI10JsonMemberE, ptr %1, align 8
  %length70 = extractvalue %_Z6VectorI10JsonMemberE %load.struct69, 0
  %add71 = add i64 %length68, %length70
  %length72 = getelementptr inbounds nuw %_Z5ArrayI10JsonMemberE, ptr %0, i32 0, i32 0
  store i64 %add71, ptr %length72, align 8
  ret void
}

define linkonce_odr ptr @_ZN5ArrayI10JsonMemberE3getEm(ptr %0, i64 %1) {
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

define linkonce_odr ptr @_ZN5ArrayI10JsonMemberE2atEm(ptr %0, i64 %1) {
entry:
  %load.struct = load %_Z5ArrayI10JsonMemberE, ptr %0, align 8
  %length = extractvalue %_Z5ArrayI10JsonMemberE %load.struct, 0
  %ge = icmp uge i64 %1, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %field.inplace = getelementptr inbounds nuw %_Z5ArrayI10JsonMemberE, ptr %0, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  call void @_Z15scaly_panic_oobP10const_charmm(ptr @.str.29, i64 %1, i64 %field.val)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %load.struct1 = load %_Z5ArrayI10JsonMemberE, ptr %0, align 8
  %buffer = extractvalue %_Z5ArrayI10JsonMemberE %load.struct1, 2
  %ptr.add = getelementptr inbounds %_Z10JsonMember, ptr %buffer, i64 %1
  ret ptr %ptr.add
}

define linkonce_odr void @_ZN5ArrayI10JsonMemberE5clearEv(ptr %0) {
entry:
  %length = getelementptr inbounds nuw %_Z5ArrayI10JsonMemberE, ptr %0, i32 0, i32 0
  store i64 0, ptr %length, align 8
  ret void
}

define linkonce_odr void @_ZN5ArrayI10JsonMemberE3putEm10JsonMember(ptr %0, i64 %1, ptr %2) {
entry:
  %load.struct = load %_Z5ArrayI10JsonMemberE, ptr %0, align 8
  %length = extractvalue %_Z5ArrayI10JsonMemberE %load.struct, 0
  %ge = icmp uge i64 %1, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %field.inplace = getelementptr inbounds nuw %_Z5ArrayI10JsonMemberE, ptr %0, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  call void @_Z15scaly_panic_oobP10const_charmm(ptr @.str.30, i64 %1, i64 %field.val)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %load.struct1 = load %_Z5ArrayI10JsonMemberE, ptr %0, align 8
  %buffer = extractvalue %_Z5ArrayI10JsonMemberE %load.struct1, 2
  %ptr.add = getelementptr inbounds %_Z10JsonMember, ptr %buffer, i64 %1
  %store.load = load %_Z10JsonMember, ptr %2, align 8
  store %_Z10JsonMember %store.load, ptr %ptr.add, align 8
  ret void
}

define linkonce_odr ptr @_ZN13ArrayIteratorI10JsonMemberE4nextEv(ptr %0) {
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
  %call = call ptr @_ZN5ArrayI10JsonMemberE10get_bufferEv(ptr %deref.recv)
  %load.struct10 = load %_Z13ArrayIteratorI10JsonMemberE, ptr %0, align 8
  %position11 = extractvalue %_Z13ArrayIteratorI10JsonMemberE %load.struct10, 1
  %sub = sub i64 %position11, 1
  %ptr.add = getelementptr inbounds %_Z10JsonMember, ptr %call, i64 %sub
  ret ptr %ptr.add
}

define linkonce_odr void @_ZN13ArrayIteratorI10JsonMemberEC1E6OptionIR5ArrayI10JsonMemberEE(ptr %0, ptr %1) {
entry:
  %array = getelementptr inbounds nuw %_Z13ArrayIteratorI10JsonMemberE, ptr %0, i32 0, i32 0
  store ptr %1, ptr %array, align 8
  %position = getelementptr inbounds nuw %_Z13ArrayIteratorI10JsonMemberE, ptr %0, i32 0, i32 1
  store i64 0, ptr %position, align 8
  ret void
}

define linkonce_odr void @_ZN5ArrayI10JsonMemberE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z13ArrayIteratorI10JsonMemberE) %0, ptr %1, ptr %2) {
entry:
  %struct.init = alloca %_Z13ArrayIteratorI10JsonMemberE, align 8
  call void @_ZN13ArrayIteratorI10JsonMemberEC1E6OptionIR5ArrayI10JsonMemberEE(ptr %struct.init, ptr %2)
  %sret.body = load %_Z13ArrayIteratorI10JsonMemberE, ptr %struct.init, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %struct.init, i64 ptrtoint (ptr getelementptr (%_Z13ArrayIteratorI10JsonMemberE, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN5ArrayI10JsonMemberE8as_sliceEPN4scaly6memory4PageE(ptr noalias sret(%_Z5SliceI10JsonMemberE) %0, ptr %1, ptr %2) {
entry:
  %load.struct = load %_Z5ArrayI10JsonMemberE, ptr %2, align 8
  %length = extractvalue %_Z5ArrayI10JsonMemberE %load.struct, 0
  %call = call ptr @_ZN5ArrayI10JsonMemberE10get_bufferEv(ptr %2)
  %tuple = alloca %_Z5SliceI10JsonMemberE, align 8
  %tuple.field = getelementptr inbounds nuw %_Z5SliceI10JsonMemberE, ptr %tuple, i32 0, i32 0
  store i64 %length, ptr %tuple.field, align 1
  %tuple.field1 = getelementptr inbounds nuw %_Z5SliceI10JsonMemberE, ptr %tuple, i32 0, i32 1
  store ptr %call, ptr %tuple.field1, align 1
  %tuple.val = load %_Z5SliceI10JsonMemberE, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z5SliceI10JsonMemberE, ptr null, i32 1) to i64), i1 false)
  ret void
}

declare ptr @_ZN4Page23allocate_exclusive_pageEv(ptr)

declare i64 @_ZN4Page12get_capacityEm(ptr, i64)

define linkonce_odr void @_ZN5ArrayI10JsonMemberEC1Ev(ptr %0) {
entry:
  %length = getelementptr inbounds nuw %_Z5ArrayI10JsonMemberE, ptr %0, i32 0, i32 0
  store i64 0, ptr %length, align 8
  %capacity = getelementptr inbounds nuw %_Z5ArrayI10JsonMemberE, ptr %0, i32 0, i32 1
  store i64 0, ptr %capacity, align 8
  %buffer = getelementptr inbounds nuw %_Z5ArrayI10JsonMemberE, ptr %0, i32 0, i32 2
  store ptr null, ptr %buffer, align 8
  ret void
}

define linkonce_odr void @_ZN5ArrayI10JsonMemberEC1Em(ptr %0, i64 %1) {
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

define linkonce_odr void @_ZN6VectorI10JsonMemberEC1E5ArrayI10JsonMemberE(ptr %0, ptr %1) {
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
  %call7 = call ptr @_ZN5ArrayI10JsonMemberE10get_bufferEv(ptr %1)
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

define linkonce_odr void @_ZN6VectorI10JsonMemberEC1E4ListI10JsonMemberE(ptr %0, ptr %1) {
entry:
  %deref.tmp = alloca %_Z10JsonMember, align 8
  %i = alloca i64, align 8
  %list_iterator = alloca ptr, align 8
  %sret.result = alloca %_Z12ListIteratorI10JsonMemberE, align 8
  %call = call ptr @_ZN4Page3getEPv(ptr %0)
  %call1 = call i64 @_ZN4ListI10JsonMemberE5countEv(ptr %1)
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
  call void @_ZN4ListI10JsonMemberE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z12ListIteratorI10JsonMemberE) %sret.result, ptr null, ptr %1)
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
  %call7 = call ptr @_ZN12ListIteratorI10JsonMemberE4nextEv(ptr %list_iterator6)
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

define linkonce_odr ptr @_ZN12ListIteratorI1TE4nextEv(ptr %0) {
stub.entry:
  ret ptr null
}

define linkonce_odr ptr @_ZN4ListI9JsonValueE8get_headEv(ptr %0) {
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

define linkonce_odr ptr @_ZN12ListIteratorI9JsonValueE4nextEv(ptr %0) {
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

define linkonce_odr i64 @_ZN4ListI9JsonValueE5countEv(ptr %0) {
entry:
  %sret.result = alloca %_Z12ListIteratorI9JsonValueE, align 8
  call void @_ZN4ListI9JsonValueE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z12ListIteratorI9JsonValueE) %sret.result, ptr null, ptr %0)
  %list_iterator = alloca ptr, align 8
  store ptr %sret.result, ptr %list_iterator, align 1
  %i = alloca i64, align 8
  store i64 0, ptr %i, align 1
  br label %while.cond

while.cond:                                       ; preds = %while.body, %entry
  %list_iterator1 = load ptr, ptr %list_iterator, align 8
  %call = call ptr @_ZN12ListIteratorI9JsonValueE4nextEv(ptr %list_iterator1)
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

define linkonce_odr void @_ZN4ListI9JsonValueE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z12ListIteratorI9JsonValueE) %0, ptr %1, ptr %2) {
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

define linkonce_odr void @_ZN4ListI9JsonValueE4linkEP4NodeI9JsonValueE(ptr %0, ptr %1) {
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

define linkonce_odr void @_ZN4ListI9JsonValueE3addE9JsonValue(ptr %0, ptr %1) {
entry:
  %own_page = call ptr @_Z3getPv(ptr %0)
  %tuple.region = call ptr @_ZN4Page8allocateEmm(ptr %own_page, i64 ptrtoint (ptr getelementptr (%_Z4NodeI9JsonValueE, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr ({ i1, %_Z4NodeI9JsonValueE }, ptr null, i64 0, i32 1) to i64))
  %field.load = load %_Z9JsonValue, ptr %1, align 1
  %tuple.field = getelementptr inbounds nuw %_Z4NodeI9JsonValueE, ptr %tuple.region, i32 0, i32 0
  store %_Z9JsonValue %field.load, ptr %tuple.field, align 1
  %tuple.field1 = getelementptr inbounds nuw %_Z4NodeI9JsonValueE, ptr %tuple.region, i32 0, i32 1
  store ptr null, ptr %tuple.field1, align 1
  call void @_ZN4ListI9JsonValueE4linkEP4NodeI9JsonValueE(ptr %0, ptr %tuple.region)
  ret void
}

define linkonce_odr void @_ZN4ListI9JsonValueE6add_onER4Page9JsonValue(ptr %0, ptr %1, ptr %2) {
entry:
  %tuple.region = call ptr @_ZN4Page8allocateEmm(ptr %1, i64 ptrtoint (ptr getelementptr (%_Z4NodeI9JsonValueE, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr ({ i1, %_Z4NodeI9JsonValueE }, ptr null, i64 0, i32 1) to i64))
  %field.load = load %_Z9JsonValue, ptr %2, align 1
  %tuple.field = getelementptr inbounds nuw %_Z4NodeI9JsonValueE, ptr %tuple.region, i32 0, i32 0
  store %_Z9JsonValue %field.load, ptr %tuple.field, align 1
  %tuple.field1 = getelementptr inbounds nuw %_Z4NodeI9JsonValueE, ptr %tuple.region, i32 0, i32 1
  store ptr null, ptr %tuple.field1, align 1
  call void @_ZN4ListI9JsonValueE4linkEP4NodeI9JsonValueE(ptr %0, ptr %tuple.region)
  ret void
}

define linkonce_odr ptr @_ZN6VectorI9JsonValueE3getEm(ptr noalias %0, i64 %1) {
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

define linkonce_odr ptr @_ZN6VectorI9JsonValueE2atEm(ptr noalias %0, i64 %1) {
entry:
  %load.struct = load %_Z6VectorI9JsonValueE, ptr %0, align 8
  %length = extractvalue %_Z6VectorI9JsonValueE %load.struct, 0
  %ge = icmp uge i64 %1, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %field.inplace = getelementptr inbounds nuw %_Z6VectorI9JsonValueE, ptr %0, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  call void @_Z15scaly_panic_oobP10const_charmm(ptr @.str.31, i64 %1, i64 %field.val)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %load.struct1 = load %_Z6VectorI9JsonValueE, ptr %0, align 8
  %data = extractvalue %_Z6VectorI9JsonValueE %load.struct1, 1
  %ptr.add = getelementptr inbounds %_Z9JsonValue, ptr %data, i64 %1
  ret ptr %ptr.add
}

define linkonce_odr ptr @_ZN6VectorI9JsonValueE7get_ptrEm(ptr noalias %0, i64 %1) {
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

define linkonce_odr void @_ZN6VectorI9JsonValueE3putEm9JsonValue(ptr %0, i64 %1, ptr %2) {
entry:
  %load.struct = load %_Z6VectorI9JsonValueE, ptr %0, align 8
  %length = extractvalue %_Z6VectorI9JsonValueE %load.struct, 0
  %ge = icmp uge i64 %1, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %field.inplace = getelementptr inbounds nuw %_Z6VectorI9JsonValueE, ptr %0, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  call void @_Z15scaly_panic_oobP10const_charmm(ptr @.str.32, i64 %1, i64 %field.val)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %load.struct1 = load %_Z6VectorI9JsonValueE, ptr %0, align 8
  %data = extractvalue %_Z6VectorI9JsonValueE %load.struct1, 1
  %ptr.add = getelementptr inbounds %_Z9JsonValue, ptr %data, i64 %1
  %store.load = load %_Z9JsonValue, ptr %2, align 1
  store %_Z9JsonValue %store.load, ptr %ptr.add, align 1
  ret void
}

define linkonce_odr ptr @_ZN14VectorIteratorI9JsonValueE4nextEv(ptr %0) {
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
  %call = call ptr @_ZN6VectorI9JsonValueE7get_ptrEm(ptr %deref.recv, i64 %sub)
  ret ptr %call
}

define linkonce_odr void @_ZN14VectorIteratorI9JsonValueEC1E6OptionIR6VectorI9JsonValueEE(ptr %0, ptr %1) {
entry:
  %vector = getelementptr inbounds nuw %_Z14VectorIteratorI9JsonValueE, ptr %0, i32 0, i32 0
  store ptr %1, ptr %vector, align 8
  %position = getelementptr inbounds nuw %_Z14VectorIteratorI9JsonValueE, ptr %0, i32 0, i32 1
  store i64 0, ptr %position, align 8
  ret void
}

define linkonce_odr void @_ZN6VectorI9JsonValueE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z14VectorIteratorI9JsonValueE) %0, ptr %1, ptr noalias %2) {
entry:
  %struct.init = alloca %_Z14VectorIteratorI9JsonValueE, align 8
  call void @_ZN14VectorIteratorI9JsonValueEC1E6OptionIR6VectorI9JsonValueEE(ptr %struct.init, ptr %2)
  %sret.body = load %_Z14VectorIteratorI9JsonValueE, ptr %struct.init, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %struct.init, i64 ptrtoint (ptr getelementptr (%_Z14VectorIteratorI9JsonValueE, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN6VectorI9JsonValueE8as_sliceEPN4scaly6memory4PageE(ptr noalias sret(%_Z5SliceI9JsonValueE) %0, ptr %1, ptr noalias %2) {
entry:
  %load.struct = load %_Z6VectorI9JsonValueE, ptr %2, align 8
  %length = extractvalue %_Z6VectorI9JsonValueE %load.struct, 0
  %load.struct1 = load %_Z6VectorI9JsonValueE, ptr %2, align 8
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

define linkonce_odr void @_ZN6VectorI9JsonValueEC1Ev(ptr noalias %0) {
entry:
  %length = getelementptr inbounds nuw %_Z6VectorI9JsonValueE, ptr %0, i32 0, i32 0
  store i64 0, ptr %length, align 8
  %data = getelementptr inbounds nuw %_Z6VectorI9JsonValueE, ptr %0, i32 0, i32 1
  store ptr null, ptr %data, align 8
  ret void
}

define linkonce_odr void @_ZN6VectorI9JsonValueEC1Em(ptr %0, i64 %1) {
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

define linkonce_odr void @_ZN6VectorI9JsonValueEC1E6VectorI9JsonValueE(ptr %0, ptr %1) {
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

define linkonce_odr ptr @_ZN5ArrayI9JsonValueE10get_bufferEv(ptr %0) {
entry:
  %load.struct = load %_Z5ArrayI9JsonValueE, ptr %0, align 8
  %buffer = extractvalue %_Z5ArrayI9JsonValueE %load.struct, 2
  ret ptr %buffer
}

define linkonce_odr i64 @_ZN5ArrayI9JsonValueE10get_lengthEv(ptr %0) {
entry:
  %load.struct = load %_Z5ArrayI9JsonValueE, ptr %0, align 8
  %length = extractvalue %_Z5ArrayI9JsonValueE %load.struct, 0
  ret i64 %length
}

define linkonce_odr i64 @_ZN5ArrayI9JsonValueE12get_capacityEv(ptr %0) {
entry:
  %load.struct = load %_Z5ArrayI9JsonValueE, ptr %0, align 8
  %capacity = extractvalue %_Z5ArrayI9JsonValueE %load.struct, 1
  ret i64 %capacity
}

define linkonce_odr void @_ZN5ArrayI9JsonValueE10reallocateEv(ptr %0) {
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

define linkonce_odr void @_ZN5ArrayI9JsonValueE3addE9JsonValue(ptr %0, ptr %1) {
entry:
  %load.struct = load %_Z5ArrayI9JsonValueE, ptr %0, align 8
  %buffer = extractvalue %_Z5ArrayI9JsonValueE %load.struct, 2
  %eq = icmp eq ptr %buffer, null
  br i1 %eq, label %if.then, label %lor.rhs

if.then:                                          ; preds = %lor.rhs, %entry
  call void @_ZN5ArrayI9JsonValueE10reallocateEv(ptr %0)
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

define linkonce_odr void @_ZN5ArrayI9JsonValueE3addE6VectorI9JsonValueE(ptr %0, ptr %1) {
entry:
  %load.struct = load %_Z5ArrayI9JsonValueE, ptr %0, align 8
  %length = extractvalue %_Z5ArrayI9JsonValueE %load.struct, 0
  %load.struct1 = load %_Z6VectorI9JsonValueE, ptr %1, align 8
  %length2 = extractvalue %_Z6VectorI9JsonValueE %load.struct1, 0
  %add = add i64 %length, %length2
  %new_length = alloca i64, align 8
  store i64 %add, ptr %new_length, align 1
  %new_length3 = load i64, ptr %new_length, align 8
  %load.struct4 = load %_Z5ArrayI9JsonValueE, ptr %0, align 8
  %length5 = extractvalue %_Z5ArrayI9JsonValueE %load.struct4, 0
  %lt = icmp ult i64 %new_length3, %length5
  br i1 %lt, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %field.inplace = getelementptr inbounds nuw %_Z6VectorI9JsonValueE, ptr %1, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  %field.inplace6 = getelementptr inbounds nuw %_Z5ArrayI9JsonValueE, ptr %0, i32 0, i32 0
  %field.val7 = load i64, ptr %field.inplace6, align 8
  call void @_Z16scaly_panic_sizeP10const_charmm(ptr @.str.33, i64 %field.val, i64 %field.val7)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %load.struct10 = load %_Z5ArrayI9JsonValueE, ptr %0, align 8
  %buffer = extractvalue %_Z5ArrayI9JsonValueE %load.struct10, 2
  %eq = icmp eq ptr %buffer, null
  br i1 %eq, label %if.then8, label %lor.rhs

if.then8:                                         ; preds = %lor.rhs, %if.end
  call void @_ZN5ArrayI9JsonValueE10reallocateEv(ptr %0)
  br label %if.end9

if.end9:                                          ; preds = %if.then8, %lor.rhs
  %new_length13 = load i64, ptr %new_length, align 8
  %load.struct14 = load %_Z5ArrayI9JsonValueE, ptr %0, align 8
  %capacity15 = extractvalue %_Z5ArrayI9JsonValueE %load.struct14, 1
  %gt16 = icmp ugt i64 %new_length13, %capacity15
  br i1 %gt16, label %if.then17, label %if.end18

lor.rhs:                                          ; preds = %if.end
  %new_length11 = load i64, ptr %new_length, align 8
  %load.struct12 = load %_Z5ArrayI9JsonValueE, ptr %0, align 8
  %capacity = extractvalue %_Z5ArrayI9JsonValueE %load.struct12, 1
  %gt = icmp ugt i64 %new_length11, %capacity
  br i1 %gt, label %if.then8, label %if.end9

if.then17:                                        ; preds = %if.end9
  %call = call ptr @_ZN4Page3getEPv(ptr %0)
  %load.struct19 = load %_Z5ArrayI9JsonValueE, ptr %0, align 8
  %buffer20 = extractvalue %_Z5ArrayI9JsonValueE %load.struct19, 2
  %load.struct21 = load %_Z5ArrayI9JsonValueE, ptr %0, align 8
  %capacity22 = extractvalue %_Z5ArrayI9JsonValueE %load.struct21, 1
  %mul = mul i64 %capacity22, ptrtoint (ptr getelementptr (%_Z9JsonValue, ptr null, i32 1) to i64)
  %le = icmp ule i64 %mul, 1024
  %new_length23 = load i64, ptr %new_length, align 8
  %mul24 = mul i64 %new_length23, ptrtoint (ptr getelementptr (%_Z9JsonValue, ptr null, i32 1) to i64)
  %le25 = icmp ule i64 %mul24, 1024
  br i1 %le25, label %if.then26, label %if.else

if.end18:                                         ; preds = %if.end50, %if.end9
  %load.struct52 = load %_Z6VectorI9JsonValueE, ptr %1, align 8
  %length53 = extractvalue %_Z6VectorI9JsonValueE %load.struct52, 0
  %gt54 = icmp ugt i64 %length53, 0
  br i1 %gt54, label %if.then55, label %if.end56

if.then26:                                        ; preds = %if.then17
  %new_length28 = load i64, ptr %new_length, align 8
  %mul29 = mul i64 %new_length28, ptrtoint (ptr getelementptr (%_Z9JsonValue, ptr null, i32 1) to i64)
  %call30 = call ptr @_ZN4Page8allocateEmm(ptr %call, i64 %mul29, i64 8)
  %buffer31 = getelementptr inbounds nuw %_Z5ArrayI9JsonValueE, ptr %0, i32 0, i32 2
  store ptr %call30, ptr %buffer31, align 8
  br label %if.end27

if.else:                                          ; preds = %if.then17
  %new_length32 = load i64, ptr %new_length, align 8
  %mul33 = mul i64 %new_length32, ptrtoint (ptr getelementptr (%_Z9JsonValue, ptr null, i32 1) to i64)
  %call34 = call ptr @_ZN4Page25allocate_exclusive_bufferEmm(ptr %call, i64 %mul33, i64 8)
  %buffer35 = getelementptr inbounds nuw %_Z5ArrayI9JsonValueE, ptr %0, i32 0, i32 2
  store ptr %call34, ptr %buffer35, align 8
  br label %if.end27

if.end27:                                         ; preds = %if.else, %if.then26
  %if.value = phi ptr [ %call30, %if.then26 ], [ %call34, %if.else ]
  %new_length36 = load i64, ptr %new_length, align 8
  %capacity37 = getelementptr inbounds nuw %_Z5ArrayI9JsonValueE, ptr %0, i32 0, i32 1
  store i64 %new_length36, ptr %capacity37, align 8
  %load.struct38 = load %_Z5ArrayI9JsonValueE, ptr %0, align 8
  %length39 = extractvalue %_Z5ArrayI9JsonValueE %load.struct38, 0
  %gt40 = icmp ugt i64 %length39, 0
  br i1 %gt40, label %if.then41, label %if.end42

if.then41:                                        ; preds = %if.end27
  %field.inplace43 = getelementptr inbounds nuw %_Z5ArrayI9JsonValueE, ptr %0, i32 0, i32 2
  %deref.recv = load ptr, ptr %field.inplace43, align 8
  %load.struct44 = load %_Z5ArrayI9JsonValueE, ptr %0, align 8
  %length45 = extractvalue %_Z5ArrayI9JsonValueE %load.struct44, 0
  %mul46 = mul i64 %length45, ptrtoint (ptr getelementptr (%_Z9JsonValue, ptr null, i32 1) to i64)
  %call47 = call ptr @memcpy(ptr %deref.recv, ptr %buffer20, i64 %mul46)
  br label %if.end42

if.end42:                                         ; preds = %if.then41, %if.end27
  %eq48 = icmp eq i1 %le, false
  br i1 %eq48, label %if.then49, label %if.end50

if.then49:                                        ; preds = %if.end42
  %call51 = call ptr @_ZN4Page3getEPv(ptr %buffer20)
  call void @_ZN4Page25deallocate_exclusive_pageER4Page(ptr %call, ptr %call51)
  br label %if.end50

if.end50:                                         ; preds = %if.then49, %if.end42
  br label %if.end18

if.then55:                                        ; preds = %if.end18
  %load.struct57 = load %_Z5ArrayI9JsonValueE, ptr %0, align 8
  %buffer58 = extractvalue %_Z5ArrayI9JsonValueE %load.struct57, 2
  %load.struct59 = load %_Z5ArrayI9JsonValueE, ptr %0, align 8
  %length60 = extractvalue %_Z5ArrayI9JsonValueE %load.struct59, 0
  %ptr.add = getelementptr inbounds %_Z9JsonValue, ptr %buffer58, i64 %length60
  %field.inplace61 = getelementptr inbounds nuw %_Z6VectorI9JsonValueE, ptr %1, i32 0, i32 1
  %deref.recv62 = load ptr, ptr %field.inplace61, align 8
  %load.struct63 = load %_Z6VectorI9JsonValueE, ptr %1, align 8
  %length64 = extractvalue %_Z6VectorI9JsonValueE %load.struct63, 0
  %mul65 = mul i64 %length64, ptrtoint (ptr getelementptr (%_Z9JsonValue, ptr null, i32 1) to i64)
  %call66 = call ptr @memcpy(ptr %ptr.add, ptr %deref.recv62, i64 %mul65)
  br label %if.end56

if.end56:                                         ; preds = %if.then55, %if.end18
  %load.struct67 = load %_Z5ArrayI9JsonValueE, ptr %0, align 8
  %length68 = extractvalue %_Z5ArrayI9JsonValueE %load.struct67, 0
  %load.struct69 = load %_Z6VectorI9JsonValueE, ptr %1, align 8
  %length70 = extractvalue %_Z6VectorI9JsonValueE %load.struct69, 0
  %add71 = add i64 %length68, %length70
  %length72 = getelementptr inbounds nuw %_Z5ArrayI9JsonValueE, ptr %0, i32 0, i32 0
  store i64 %add71, ptr %length72, align 8
  ret void
}

define linkonce_odr ptr @_ZN5ArrayI9JsonValueE3getEm(ptr %0, i64 %1) {
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

define linkonce_odr ptr @_ZN5ArrayI9JsonValueE2atEm(ptr %0, i64 %1) {
entry:
  %load.struct = load %_Z5ArrayI9JsonValueE, ptr %0, align 8
  %length = extractvalue %_Z5ArrayI9JsonValueE %load.struct, 0
  %ge = icmp uge i64 %1, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %field.inplace = getelementptr inbounds nuw %_Z5ArrayI9JsonValueE, ptr %0, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  call void @_Z15scaly_panic_oobP10const_charmm(ptr @.str.34, i64 %1, i64 %field.val)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %load.struct1 = load %_Z5ArrayI9JsonValueE, ptr %0, align 8
  %buffer = extractvalue %_Z5ArrayI9JsonValueE %load.struct1, 2
  %ptr.add = getelementptr inbounds %_Z9JsonValue, ptr %buffer, i64 %1
  ret ptr %ptr.add
}

define linkonce_odr void @_ZN5ArrayI9JsonValueE5clearEv(ptr %0) {
entry:
  %length = getelementptr inbounds nuw %_Z5ArrayI9JsonValueE, ptr %0, i32 0, i32 0
  store i64 0, ptr %length, align 8
  ret void
}

define linkonce_odr void @_ZN5ArrayI9JsonValueE3putEm9JsonValue(ptr %0, i64 %1, ptr %2) {
entry:
  %load.struct = load %_Z5ArrayI9JsonValueE, ptr %0, align 8
  %length = extractvalue %_Z5ArrayI9JsonValueE %load.struct, 0
  %ge = icmp uge i64 %1, %length
  br i1 %ge, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %field.inplace = getelementptr inbounds nuw %_Z5ArrayI9JsonValueE, ptr %0, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  call void @_Z15scaly_panic_oobP10const_charmm(ptr @.str.35, i64 %1, i64 %field.val)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %load.struct1 = load %_Z5ArrayI9JsonValueE, ptr %0, align 8
  %buffer = extractvalue %_Z5ArrayI9JsonValueE %load.struct1, 2
  %ptr.add = getelementptr inbounds %_Z9JsonValue, ptr %buffer, i64 %1
  %store.load = load %_Z9JsonValue, ptr %2, align 1
  store %_Z9JsonValue %store.load, ptr %ptr.add, align 1
  ret void
}

define linkonce_odr ptr @_ZN13ArrayIteratorI9JsonValueE4nextEv(ptr %0) {
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
  %call = call ptr @_ZN5ArrayI9JsonValueE10get_bufferEv(ptr %deref.recv)
  %load.struct10 = load %_Z13ArrayIteratorI9JsonValueE, ptr %0, align 8
  %position11 = extractvalue %_Z13ArrayIteratorI9JsonValueE %load.struct10, 1
  %sub = sub i64 %position11, 1
  %ptr.add = getelementptr inbounds %_Z9JsonValue, ptr %call, i64 %sub
  ret ptr %ptr.add
}

define linkonce_odr void @_ZN13ArrayIteratorI9JsonValueEC1E6OptionIR5ArrayI9JsonValueEE(ptr %0, ptr %1) {
entry:
  %array = getelementptr inbounds nuw %_Z13ArrayIteratorI9JsonValueE, ptr %0, i32 0, i32 0
  store ptr %1, ptr %array, align 8
  %position = getelementptr inbounds nuw %_Z13ArrayIteratorI9JsonValueE, ptr %0, i32 0, i32 1
  store i64 0, ptr %position, align 8
  ret void
}

define linkonce_odr void @_ZN5ArrayI9JsonValueE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z13ArrayIteratorI9JsonValueE) %0, ptr %1, ptr %2) {
entry:
  %struct.init = alloca %_Z13ArrayIteratorI9JsonValueE, align 8
  call void @_ZN13ArrayIteratorI9JsonValueEC1E6OptionIR5ArrayI9JsonValueEE(ptr %struct.init, ptr %2)
  %sret.body = load %_Z13ArrayIteratorI9JsonValueE, ptr %struct.init, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %struct.init, i64 ptrtoint (ptr getelementptr (%_Z13ArrayIteratorI9JsonValueE, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN5ArrayI9JsonValueE8as_sliceEPN4scaly6memory4PageE(ptr noalias sret(%_Z5SliceI9JsonValueE) %0, ptr %1, ptr %2) {
entry:
  %load.struct = load %_Z5ArrayI9JsonValueE, ptr %2, align 8
  %length = extractvalue %_Z5ArrayI9JsonValueE %load.struct, 0
  %call = call ptr @_ZN5ArrayI9JsonValueE10get_bufferEv(ptr %2)
  %tuple = alloca %_Z5SliceI9JsonValueE, align 8
  %tuple.field = getelementptr inbounds nuw %_Z5SliceI9JsonValueE, ptr %tuple, i32 0, i32 0
  store i64 %length, ptr %tuple.field, align 1
  %tuple.field1 = getelementptr inbounds nuw %_Z5SliceI9JsonValueE, ptr %tuple, i32 0, i32 1
  store ptr %call, ptr %tuple.field1, align 1
  %tuple.val = load %_Z5SliceI9JsonValueE, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z5SliceI9JsonValueE, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN5ArrayI9JsonValueEC1Ev(ptr %0) {
entry:
  %length = getelementptr inbounds nuw %_Z5ArrayI9JsonValueE, ptr %0, i32 0, i32 0
  store i64 0, ptr %length, align 8
  %capacity = getelementptr inbounds nuw %_Z5ArrayI9JsonValueE, ptr %0, i32 0, i32 1
  store i64 0, ptr %capacity, align 8
  %buffer = getelementptr inbounds nuw %_Z5ArrayI9JsonValueE, ptr %0, i32 0, i32 2
  store ptr null, ptr %buffer, align 8
  ret void
}

define linkonce_odr void @_ZN5ArrayI9JsonValueEC1Em(ptr %0, i64 %1) {
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

define linkonce_odr void @_ZN6VectorI9JsonValueEC1E5ArrayI9JsonValueE(ptr %0, ptr %1) {
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
  %call7 = call ptr @_ZN5ArrayI9JsonValueE10get_bufferEv(ptr %1)
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

define linkonce_odr void @_ZN6VectorI9JsonValueEC1E4ListI9JsonValueE(ptr %0, ptr %1) {
entry:
  %deref.tmp = alloca %_Z9JsonValue, align 8
  %i = alloca i64, align 8
  %list_iterator = alloca ptr, align 8
  %sret.result = alloca %_Z12ListIteratorI9JsonValueE, align 8
  %call = call ptr @_ZN4Page3getEPv(ptr %0)
  %call1 = call i64 @_ZN4ListI9JsonValueE5countEv(ptr %1)
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
  call void @_ZN4ListI9JsonValueE12get_iteratorEPN4scaly6memory4PageE(ptr noalias sret(%_Z12ListIteratorI9JsonValueE) %sret.result, ptr null, ptr %1)
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
  %call7 = call ptr @_ZN12ListIteratorI9JsonValueE4nextEv(ptr %list_iterator6)
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

define linkonce_odr i1 @_ZN10JsonParsed2okEv(ptr %0) {
entry:
  %load.struct = load %_Z10JsonParsed, ptr %0, align 8
  %error = extractvalue %_Z10JsonParsed %load.struct, 1
  %eq = icmp eq i64 %error, 0
  ret i1 %eq
}

define linkonce_odr void @_ZN10JsonParsed7messageEPN4scaly6memory4PageE(ptr noalias sret(%_Z5SliceIcE) %0, ptr %1, ptr %2) {
entry:
  %sret.result = alloca %_Z5SliceIcE, align 8
  %field.inplace = getelementptr inbounds nuw %_Z10JsonParsed, ptr %2, i32 0, i32 1
  %field.val = load i64, ptr %field.inplace, align 8
  call void @_ZN10JsonReader7messageEPN4scaly6memory4PageEi(ptr noalias sret(%_Z5SliceIcE) %sret.result, ptr null, i64 %field.val)
  %sret.body = load %_Z5SliceIcE, ptr %sret.result, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %sret.result, i64 ptrtoint (ptr getelementptr (%_Z5SliceIcE, ptr null, i32 1) to i64), i1 false)
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
  call void @_Z15scaly_panic_oobP10const_charmm(ptr @.str.36, i64 %1, i64 %field.val)
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
  call void @_Z15scaly_panic_oobP10const_charmm(ptr @.str.37, i64 %1, i64 %field.val)
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

define linkonce_odr void @_ZN6VectorI2u8E8as_sliceEPN4scaly6memory4PageE(ptr noalias sret(%_Z5SliceI2u8E) %0, ptr %1, ptr noalias %2) {
entry:
  %load.struct = load %_Z6VectorI2u8E, ptr %2, align 8
  %length = extractvalue %_Z6VectorI2u8E %load.struct, 0
  %load.struct1 = load %_Z6VectorI2u8E, ptr %2, align 8
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

define linkonce_odr void @_ZN5ArrayI2u8E3addE6VectorI2u8E(ptr %0, ptr %1) {
entry:
  %load.struct = load %_Z5ArrayI2u8E, ptr %0, align 8
  %length = extractvalue %_Z5ArrayI2u8E %load.struct, 0
  %load.struct1 = load %_Z6VectorI2u8E, ptr %1, align 8
  %length2 = extractvalue %_Z6VectorI2u8E %load.struct1, 0
  %add = add i64 %length, %length2
  %new_length = alloca i64, align 8
  store i64 %add, ptr %new_length, align 1
  %new_length3 = load i64, ptr %new_length, align 8
  %load.struct4 = load %_Z5ArrayI2u8E, ptr %0, align 8
  %length5 = extractvalue %_Z5ArrayI2u8E %load.struct4, 0
  %lt = icmp ult i64 %new_length3, %length5
  br i1 %lt, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %field.inplace = getelementptr inbounds nuw %_Z6VectorI2u8E, ptr %1, i32 0, i32 0
  %field.val = load i64, ptr %field.inplace, align 8
  %field.inplace6 = getelementptr inbounds nuw %_Z5ArrayI2u8E, ptr %0, i32 0, i32 0
  %field.val7 = load i64, ptr %field.inplace6, align 8
  call void @_Z16scaly_panic_sizeP10const_charmm(ptr @.str.38, i64 %field.val, i64 %field.val7)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %load.struct10 = load %_Z5ArrayI2u8E, ptr %0, align 8
  %buffer = extractvalue %_Z5ArrayI2u8E %load.struct10, 2
  %eq = icmp eq ptr %buffer, null
  br i1 %eq, label %if.then8, label %lor.rhs

if.then8:                                         ; preds = %lor.rhs, %if.end
  call void @_ZN5ArrayI2u8E10reallocateEv(ptr %0)
  br label %if.end9

if.end9:                                          ; preds = %if.then8, %lor.rhs
  %new_length13 = load i64, ptr %new_length, align 8
  %load.struct14 = load %_Z5ArrayI2u8E, ptr %0, align 8
  %capacity15 = extractvalue %_Z5ArrayI2u8E %load.struct14, 1
  %gt16 = icmp ugt i64 %new_length13, %capacity15
  br i1 %gt16, label %if.then17, label %if.end18

lor.rhs:                                          ; preds = %if.end
  %new_length11 = load i64, ptr %new_length, align 8
  %load.struct12 = load %_Z5ArrayI2u8E, ptr %0, align 8
  %capacity = extractvalue %_Z5ArrayI2u8E %load.struct12, 1
  %gt = icmp ugt i64 %new_length11, %capacity
  br i1 %gt, label %if.then8, label %if.end9

if.then17:                                        ; preds = %if.end9
  %call = call ptr @_ZN4Page3getEPv(ptr %0)
  %load.struct19 = load %_Z5ArrayI2u8E, ptr %0, align 8
  %buffer20 = extractvalue %_Z5ArrayI2u8E %load.struct19, 2
  %load.struct21 = load %_Z5ArrayI2u8E, ptr %0, align 8
  %capacity22 = extractvalue %_Z5ArrayI2u8E %load.struct21, 1
  %mul = mul i64 %capacity22, ptrtoint (ptr getelementptr (i8, ptr null, i32 1) to i64)
  %le = icmp ule i64 %mul, 1024
  %new_length23 = load i64, ptr %new_length, align 8
  %mul24 = mul i64 %new_length23, ptrtoint (ptr getelementptr (i8, ptr null, i32 1) to i64)
  %le25 = icmp ule i64 %mul24, 1024
  br i1 %le25, label %if.then26, label %if.else

if.end18:                                         ; preds = %if.end50, %if.end9
  %load.struct52 = load %_Z6VectorI2u8E, ptr %1, align 8
  %length53 = extractvalue %_Z6VectorI2u8E %load.struct52, 0
  %gt54 = icmp ugt i64 %length53, 0
  br i1 %gt54, label %if.then55, label %if.end56

if.then26:                                        ; preds = %if.then17
  %new_length28 = load i64, ptr %new_length, align 8
  %mul29 = mul i64 %new_length28, ptrtoint (ptr getelementptr (i8, ptr null, i32 1) to i64)
  %call30 = call ptr @_ZN4Page8allocateEmm(ptr %call, i64 %mul29, i64 1)
  %buffer31 = getelementptr inbounds nuw %_Z5ArrayI2u8E, ptr %0, i32 0, i32 2
  store ptr %call30, ptr %buffer31, align 8
  br label %if.end27

if.else:                                          ; preds = %if.then17
  %new_length32 = load i64, ptr %new_length, align 8
  %mul33 = mul i64 %new_length32, ptrtoint (ptr getelementptr (i8, ptr null, i32 1) to i64)
  %call34 = call ptr @_ZN4Page25allocate_exclusive_bufferEmm(ptr %call, i64 %mul33, i64 1)
  %buffer35 = getelementptr inbounds nuw %_Z5ArrayI2u8E, ptr %0, i32 0, i32 2
  store ptr %call34, ptr %buffer35, align 8
  br label %if.end27

if.end27:                                         ; preds = %if.else, %if.then26
  %if.value = phi ptr [ %call30, %if.then26 ], [ %call34, %if.else ]
  %new_length36 = load i64, ptr %new_length, align 8
  %capacity37 = getelementptr inbounds nuw %_Z5ArrayI2u8E, ptr %0, i32 0, i32 1
  store i64 %new_length36, ptr %capacity37, align 8
  %load.struct38 = load %_Z5ArrayI2u8E, ptr %0, align 8
  %length39 = extractvalue %_Z5ArrayI2u8E %load.struct38, 0
  %gt40 = icmp ugt i64 %length39, 0
  br i1 %gt40, label %if.then41, label %if.end42

if.then41:                                        ; preds = %if.end27
  %field.inplace43 = getelementptr inbounds nuw %_Z5ArrayI2u8E, ptr %0, i32 0, i32 2
  %deref.recv = load ptr, ptr %field.inplace43, align 8
  %load.struct44 = load %_Z5ArrayI2u8E, ptr %0, align 8
  %length45 = extractvalue %_Z5ArrayI2u8E %load.struct44, 0
  %mul46 = mul i64 %length45, ptrtoint (ptr getelementptr (i8, ptr null, i32 1) to i64)
  %call47 = call ptr @memcpy(ptr %deref.recv, ptr %buffer20, i64 %mul46)
  br label %if.end42

if.end42:                                         ; preds = %if.then41, %if.end27
  %eq48 = icmp eq i1 %le, false
  br i1 %eq48, label %if.then49, label %if.end50

if.then49:                                        ; preds = %if.end42
  %call51 = call ptr @_ZN4Page3getEPv(ptr %buffer20)
  call void @_ZN4Page25deallocate_exclusive_pageER4Page(ptr %call, ptr %call51)
  br label %if.end50

if.end50:                                         ; preds = %if.then49, %if.end42
  br label %if.end18

if.then55:                                        ; preds = %if.end18
  %load.struct57 = load %_Z5ArrayI2u8E, ptr %0, align 8
  %buffer58 = extractvalue %_Z5ArrayI2u8E %load.struct57, 2
  %load.struct59 = load %_Z5ArrayI2u8E, ptr %0, align 8
  %length60 = extractvalue %_Z5ArrayI2u8E %load.struct59, 0
  %ptr.add = getelementptr inbounds i8, ptr %buffer58, i64 %length60
  %field.inplace61 = getelementptr inbounds nuw %_Z6VectorI2u8E, ptr %1, i32 0, i32 1
  %deref.recv62 = load ptr, ptr %field.inplace61, align 8
  %load.struct63 = load %_Z6VectorI2u8E, ptr %1, align 8
  %length64 = extractvalue %_Z6VectorI2u8E %load.struct63, 0
  %mul65 = mul i64 %length64, ptrtoint (ptr getelementptr (i8, ptr null, i32 1) to i64)
  %call66 = call ptr @memcpy(ptr %ptr.add, ptr %deref.recv62, i64 %mul65)
  br label %if.end56

if.end56:                                         ; preds = %if.then55, %if.end18
  %load.struct67 = load %_Z5ArrayI2u8E, ptr %0, align 8
  %length68 = extractvalue %_Z5ArrayI2u8E %load.struct67, 0
  %load.struct69 = load %_Z6VectorI2u8E, ptr %1, align 8
  %length70 = extractvalue %_Z6VectorI2u8E %load.struct69, 0
  %add71 = add i64 %length68, %length70
  %length72 = getelementptr inbounds nuw %_Z5ArrayI2u8E, ptr %0, i32 0, i32 0
  store i64 %add71, ptr %length72, align 8
  ret void
}

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
  call void @_Z15scaly_panic_oobP10const_charmm(ptr @.str.39, i64 %1, i64 %field.val)
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
  call void @_Z15scaly_panic_oobP10const_charmm(ptr @.str.40, i64 %1, i64 %field.val)
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

define linkonce_odr void @_ZN5ArrayI2u8E8as_sliceEPN4scaly6memory4PageE(ptr noalias sret(%_Z5SliceI2u8E) %0, ptr %1, ptr %2) {
entry:
  %load.struct = load %_Z5ArrayI2u8E, ptr %2, align 8
  %length = extractvalue %_Z5ArrayI2u8E %load.struct, 0
  %call = call ptr @_ZN5ArrayI2u8E10get_bufferEv(ptr %2)
  %tuple = alloca %_Z5SliceI2u8E, align 8
  %tuple.field = getelementptr inbounds nuw %_Z5SliceI2u8E, ptr %tuple, i32 0, i32 0
  store i64 %length, ptr %tuple.field, align 1
  %tuple.field1 = getelementptr inbounds nuw %_Z5SliceI2u8E, ptr %tuple, i32 0, i32 1
  store ptr %call, ptr %tuple.field1, align 1
  %tuple.val = load %_Z5SliceI2u8E, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z5SliceI2u8E, ptr null, i32 1) to i64), i1 false)
  ret void
}

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

define linkonce_odr void @_ZN10JsonWriter5bytesEPN4scaly6memory4PageE(ptr noalias sret(%_Z5SliceI2u8E) %0, ptr %1, ptr %2) {
entry:
  %sret.result = alloca %_Z5SliceI2u8E, align 8
  %field.inplace = getelementptr inbounds nuw %_Z10JsonWriter, ptr %2, i32 0, i32 0
  call void @_ZN5ArrayI2u8E8as_sliceEPN4scaly6memory4PageE(ptr noalias sret(%_Z5SliceI2u8E) %sret.result, ptr null, ptr %field.inplace)
  %sret.body = load %_Z5SliceI2u8E, ptr %sret.result, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %sret.result, i64 ptrtoint (ptr getelementptr (%_Z5SliceI2u8E, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN10JsonWriter9to_stringEPN4scaly6memory4PageE(ptr noalias sret({ ptr }) %0, ptr %1, ptr %2) {
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

define linkonce_odr void @_ZN10JsonWriter5clearEv(ptr %0) {
entry:
  %field.inplace = getelementptr inbounds nuw %_Z10JsonWriter, ptr %0, i32 0, i32 0
  call void @_ZN5ArrayI2u8E5clearEv(ptr %field.inplace)
  %first = getelementptr inbounds nuw %_Z10JsonWriter, ptr %0, i32 0, i32 1
  store i1 true, ptr %first, align 1
  %after_key = getelementptr inbounds nuw %_Z10JsonWriter, ptr %0, i32 0, i32 2
  store i1 false, ptr %after_key, align 1
  ret void
}

define linkonce_odr void @_ZN10JsonWriter8separateEv(ptr %0) {
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
  call void @_ZN10JsonWriter4byteE2u8(ptr %0, i8 44)
  br label %if.end4

if.end4:                                          ; preds = %if.else, %if.then3
  ret void
}

define linkonce_odr void @_ZN10JsonWriter4byteE2u8(ptr %0, i8 %1) {
entry:
  %field.inplace = getelementptr inbounds nuw %_Z10JsonWriter, ptr %0, i32 0, i32 0
  call void @_ZN5ArrayI2u8E3addE2u8(ptr %field.inplace, i8 %1)
  ret void
}

define linkonce_odr void @_ZN10JsonWriter12begin_objectEv(ptr %0) {
entry:
  call void @_ZN10JsonWriter8separateEv(ptr %0)
  call void @_ZN10JsonWriter4byteE2u8(ptr %0, i8 123)
  %first = getelementptr inbounds nuw %_Z10JsonWriter, ptr %0, i32 0, i32 1
  store i1 true, ptr %first, align 1
  ret void
}

define linkonce_odr void @_ZN10JsonWriter10end_objectEv(ptr %0) {
entry:
  call void @_ZN10JsonWriter4byteE2u8(ptr %0, i8 125)
  %first = getelementptr inbounds nuw %_Z10JsonWriter, ptr %0, i32 0, i32 1
  store i1 false, ptr %first, align 1
  ret void
}

define linkonce_odr void @_ZN10JsonWriter11begin_arrayEv(ptr %0) {
entry:
  call void @_ZN10JsonWriter8separateEv(ptr %0)
  call void @_ZN10JsonWriter4byteE2u8(ptr %0, i8 91)
  %first = getelementptr inbounds nuw %_Z10JsonWriter, ptr %0, i32 0, i32 1
  store i1 true, ptr %first, align 1
  ret void
}

define linkonce_odr void @_ZN10JsonWriter9end_arrayEv(ptr %0) {
entry:
  call void @_ZN10JsonWriter4byteE2u8(ptr %0, i8 93)
  %first = getelementptr inbounds nuw %_Z10JsonWriter, ptr %0, i32 0, i32 1
  store i1 false, ptr %first, align 1
  ret void
}

define linkonce_odr void @_ZN10JsonWriter6quotedE5SliceI2u8E(ptr %0, ptr %1) {
entry:
  %sret.result = alloca %_Z5SliceI2u8E, align 8
  %j = alloca i64, align 8
  call void @_ZN10JsonWriter4byteE2u8(ptr %0, i8 34)
  %i = alloca i64, align 8
  store i64 0, ptr %i, align 1
  br label %while.cond

while.cond:                                       ; preds = %if.end21, %entry
  %i1 = load i64, ptr %i, align 8
  %load.struct = load %_Z5SliceI2u8E, ptr %1, align 8
  %length = extractvalue %_Z5SliceI2u8E %load.struct, 0
  %lt = icmp ult i64 %i1, %length
  br i1 %lt, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %i2 = load i64, ptr %i, align 8
  store i64 %i2, ptr %j, align 1
  br label %while.cond3

while.exit:                                       ; preds = %if.then20, %while.cond
  call void @_ZN10JsonWriter4byteE2u8(ptr %0, i8 34)
  ret void

while.cond3:                                      ; preds = %while.body4, %while.body
  %j6 = load i64, ptr %j, align 8
  %load.struct7 = load %_Z5SliceI2u8E, ptr %1, align 8
  %length8 = extractvalue %_Z5SliceI2u8E %load.struct7, 0
  %lt9 = icmp ult i64 %j6, %length8
  br i1 %lt9, label %lor.rhs, label %lor.end

while.body4:                                      ; preds = %lor.end
  %j12 = load i64, ptr %j, align 8
  %add = add i64 %j12, 1
  store i64 %add, ptr %j, align 1
  br label %while.cond3

while.exit5:                                      ; preds = %lor.end
  %j13 = load i64, ptr %j, align 8
  %i14 = load i64, ptr %i, align 8
  %gt = icmp ugt i64 %j13, %i14
  br i1 %gt, label %if.then, label %if.end

lor.rhs:                                          ; preds = %while.cond3
  %j10 = load i64, ptr %j, align 8
  %call = call i8 @_ZN5SliceI2u8EixEm(ptr %1, i64 %j10)
  %call11 = call i1 @_ZN10JsonWriter5plainE2u8(i8 %call)
  br label %lor.end

lor.end:                                          ; preds = %lor.rhs, %while.cond3
  %lor.result = phi i1 [ false, %while.cond3 ], [ %call11, %lor.rhs ]
  br i1 %lor.result, label %while.body4, label %while.exit5

if.then:                                          ; preds = %while.exit5
  %i15 = load i64, ptr %i, align 8
  %j16 = load i64, ptr %j, align 8
  call void @_ZN5SliceI2u8E8subsliceEPN4scaly6memory4PageEmm(ptr noalias sret(%_Z5SliceI2u8E) %sret.result, ptr null, ptr %1, i64 %i15, i64 %j16)
  call void @_ZN10JsonWriter6appendE5SliceI2u8E(ptr %0, ptr %sret.result)
  br label %if.end

if.end:                                           ; preds = %if.then, %while.exit5
  %j17 = load i64, ptr %j, align 8
  %load.struct18 = load %_Z5SliceI2u8E, ptr %1, align 8
  %length19 = extractvalue %_Z5SliceI2u8E %load.struct18, 0
  %ge = icmp uge i64 %j17, %length19
  br i1 %ge, label %if.then20, label %if.end21

if.then20:                                        ; preds = %if.end
  br label %while.exit

if.end21:                                         ; preds = %if.end
  %j22 = load i64, ptr %j, align 8
  %call23 = call i8 @_ZN5SliceI2u8EixEm(ptr %1, i64 %j22)
  call void @_ZN10JsonWriter6escapeE2u8(ptr %0, i8 %call23)
  %j24 = load i64, ptr %j, align 8
  %add25 = add i64 %j24, 1
  store i64 %add25, ptr %i, align 1
  br label %while.cond
}

define linkonce_odr void @_ZN10JsonWriter3keyE5SliceI2u8E(ptr %0, ptr %1) {
entry:
  call void @_ZN10JsonWriter8separateEv(ptr %0)
  call void @_ZN10JsonWriter6quotedE5SliceI2u8E(ptr %0, ptr %1)
  call void @_ZN10JsonWriter4byteE2u8(ptr %0, i8 58)
  %after_key = getelementptr inbounds nuw %_Z10JsonWriter, ptr %0, i32 0, i32 2
  store i1 true, ptr %after_key, align 1
  ret void
}

define linkonce_odr void @_ZN10JsonWriter4textE5SliceI2u8E(ptr %0, ptr %1) {
entry:
  call void @_ZN10JsonWriter8separateEv(ptr %0)
  call void @_ZN10JsonWriter6quotedE5SliceI2u8E(ptr %0, ptr %1)
  ret void
}

define linkonce_odr void @_ZN10JsonWriter7integerE3i64(ptr %0, i64 %1) {
entry:
  %at = alloca i64, align 8
  call void @_ZN10JsonWriter8separateEv(ptr %0)
  %digits = alloca [20 x i8], align 1
  %arr.ptr = getelementptr inbounds [20 x i8], ptr %digits, i64 0, i64 0
  %digits1 = alloca ptr, align 8
  store ptr %arr.ptr, ptr %digits1, align 8
  %v = alloca i64, align 8
  store i64 %1, ptr %v, align 1
  %lt = icmp slt i64 %1, 0
  br i1 %lt, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  call void @_ZN10JsonWriter4byteE2u8(ptr %0, i8 45)
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

define linkonce_odr void @_ZN10JsonWriter6appendE5SliceI2u8E(ptr %0, ptr %1) {
entry:
  %frame = alloca { ptr, ptr }, align 8
  store ptr null, ptr %frame, align 8
  %frame.parent = getelementptr inbounds nuw { ptr, ptr }, ptr %frame, i32 0, i32 1
  store ptr null, ptr %frame.parent, align 8
  %load.struct = load %_Z5SliceI2u8E, ptr %1, align 8
  %length = extractvalue %_Z5SliceI2u8E %load.struct, 0
  %gt = icmp ugt i64 %length, 0
  br i1 %gt, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %field.inplace = getelementptr inbounds nuw %_Z10JsonWriter, ptr %0, i32 0, i32 0
  %load.struct1 = load %_Z5SliceI2u8E, ptr %1, align 8
  %length2 = extractvalue %_Z5SliceI2u8E %load.struct1, 0
  %load.struct3 = load %_Z5SliceI2u8E, ptr %1, align 8
  %data = extractvalue %_Z5SliceI2u8E %load.struct3, 1
  %frame.page = load ptr, ptr %frame, align 8
  %frame.has_page = icmp ne ptr %frame.page, null
  br i1 %frame.has_page, label %frame.forced, label %frame.force

if.end:                                           ; preds = %frame.forced, %entry
  call void @_Z19scaly_release_frameP5Frame(ptr %frame)
  ret void

frame.force:                                      ; preds = %if.then
  %forced_page = call ptr @_Z17scaly_force_frameP5Frame(ptr %frame)
  br label %frame.forced

frame.forced:                                     ; preds = %frame.force, %if.then
  %forced_page4 = phi ptr [ %frame.page, %if.then ], [ %forced_page, %frame.force ]
  %tuple.region = call ptr @_ZN4Page8allocateEmm(ptr %forced_page4, i64 ptrtoint (ptr getelementptr (%_Z6VectorI2u8E, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr ({ i1, %_Z6VectorI2u8E }, ptr null, i64 0, i32 1) to i64))
  %tuple.field = getelementptr inbounds nuw %_Z6VectorI2u8E, ptr %tuple.region, i32 0, i32 0
  store i64 %length2, ptr %tuple.field, align 1
  %tuple.field5 = getelementptr inbounds nuw %_Z6VectorI2u8E, ptr %tuple.region, i32 0, i32 1
  store ptr %data, ptr %tuple.field5, align 1
  call void @_ZN5ArrayI2u8E3addE6VectorI2u8E(ptr %field.inplace, ptr %tuple.region)
  br label %if.end
}

define linkonce_odr void @_ZN10JsonWriter6numberE5SliceI2u8E(ptr %0, ptr %1) {
entry:
  call void @_ZN10JsonWriter8separateEv(ptr %0)
  call void @_ZN10JsonWriter6appendE5SliceI2u8E(ptr %0, ptr %1)
  ret void
}

define linkonce_odr void @_ZN10JsonWriter7booleanEb(ptr %0, i1 %1) {
entry:
  %arg.tmp6 = alloca %_Z5SliceI2u8E, align 8
  %tuple2 = alloca %_Z5SliceI2u8E, align 8
  %arg.tmp = alloca %_Z5SliceI2u8E, align 8
  %tuple = alloca %_Z5SliceI2u8E, align 8
  call void @_ZN10JsonWriter8separateEv(ptr %0)
  br i1 %1, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %tuple.field = getelementptr inbounds nuw %_Z5SliceI2u8E, ptr %tuple, i32 0, i32 0
  store i64 4, ptr %tuple.field, align 1
  %tuple.field1 = getelementptr inbounds nuw %_Z5SliceI2u8E, ptr %tuple, i32 0, i32 1
  store ptr @.str.41, ptr %tuple.field1, align 1
  %tuple.val = load %_Z5SliceI2u8E, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %arg.tmp, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z5SliceI2u8E, ptr null, i32 1) to i64), i1 false)
  call void @_ZN10JsonWriter6appendE5SliceI2u8E(ptr %0, ptr %arg.tmp)
  br label %if.end

if.else:                                          ; preds = %entry
  %tuple.field3 = getelementptr inbounds nuw %_Z5SliceI2u8E, ptr %tuple2, i32 0, i32 0
  store i64 5, ptr %tuple.field3, align 1
  %tuple.field4 = getelementptr inbounds nuw %_Z5SliceI2u8E, ptr %tuple2, i32 0, i32 1
  store ptr @.str.42, ptr %tuple.field4, align 1
  %tuple.val5 = load %_Z5SliceI2u8E, ptr %tuple2, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %arg.tmp6, ptr align 1 %tuple2, i64 ptrtoint (ptr getelementptr (%_Z5SliceI2u8E, ptr null, i32 1) to i64), i1 false)
  call void @_ZN10JsonWriter6appendE5SliceI2u8E(ptr %0, ptr %arg.tmp6)
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  ret void
}

define linkonce_odr void @_ZN10JsonWriter10null_valueEv(ptr %0) {
entry:
  call void @_ZN10JsonWriter8separateEv(ptr %0)
  %tuple = alloca %_Z5SliceI2u8E, align 8
  %tuple.field = getelementptr inbounds nuw %_Z5SliceI2u8E, ptr %tuple, i32 0, i32 0
  store i64 4, ptr %tuple.field, align 1
  %tuple.field1 = getelementptr inbounds nuw %_Z5SliceI2u8E, ptr %tuple, i32 0, i32 1
  store ptr @.str.43, ptr %tuple.field1, align 1
  %tuple.val = load %_Z5SliceI2u8E, ptr %tuple, align 8
  %arg.tmp = alloca %_Z5SliceI2u8E, align 8
  store %_Z5SliceI2u8E %tuple.val, ptr %arg.tmp, align 1
  call void @_ZN10JsonWriter6appendE5SliceI2u8E(ptr %0, ptr %arg.tmp)
  ret void
}

define linkonce_odr void @_ZN10JsonWriter5valueE9JsonValue(ptr %0, ptr %1) {
entry:
  %sret.result33 = alloca %_Z5SliceI2u8E, align 8
  %arg.tmp31 = alloca %_Z5SliceI10JsonMemberE, align 8
  %sret.result29 = alloca %_Z10JsonMember, align 8
  %arg.tmp17 = alloca %_Z5SliceI9JsonValueE, align 8
  %sret.result15 = alloca %_Z9JsonValue, align 8
  %i = alloca i64, align 8
  %arg.tmp10 = alloca { ptr }, align 8
  %sret.result9 = alloca %_Z5SliceI2u8E, align 8
  %arg.tmp = alloca { ptr }, align 8
  %frame = alloca { ptr, ptr }, align 8
  store ptr null, ptr %frame, align 8
  %frame.parent = getelementptr inbounds nuw { ptr, ptr }, ptr %frame, i32 0, i32 1
  store ptr null, ptr %frame.parent, align 8
  %sret.result = alloca %_Z5SliceI2u8E, align 8
  %tag.ptr = getelementptr inbounds nuw %_Z9JsonValue, ptr %1, i32 0, i32 0
  %tag = load i8, ptr %tag.ptr, align 1
  switch i8 %tag, label %choose.else [
    i8 0, label %choose.when
    i8 1, label %choose.when1
    i8 2, label %choose.when3
    i8 3, label %choose.when6
    i8 4, label %choose.when11
    i8 5, label %choose.when20
  ]

choose.end:                                       ; preds = %choose.else, %while.exit25, %while.exit, %choose.when6, %choose.when3, %choose.when1, %choose.when
  call void @_Z19scaly_release_frameP5Frame(ptr %frame)
  ret void

choose.else:                                      ; preds = %entry
  br label %choose.end

choose.when:                                      ; preds = %entry
  %"variant.c_data().ptr" = getelementptr inbounds nuw %_Z9JsonValue, ptr %1, i32 0, i32 1
  call void @_ZN10JsonWriter10null_valueEv(ptr %0)
  br label %choose.end

choose.when1:                                     ; preds = %entry
  %"variant.c_data().ptr2" = getelementptr inbounds nuw %_Z9JsonValue, ptr %1, i32 0, i32 1
  %variant.val = load i1, ptr %"variant.c_data().ptr2", align 1
  call void @_ZN10JsonWriter7booleanEb(ptr %0, i1 %variant.val)
  br label %choose.end

choose.when3:                                     ; preds = %entry
  %"variant.c_data().ptr4" = getelementptr inbounds nuw %_Z9JsonValue, ptr %1, i32 0, i32 1
  %variant.val5 = load { ptr }, ptr %"variant.c_data().ptr4", align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %arg.tmp, ptr align 1 %"variant.c_data().ptr4", i64 ptrtoint (ptr getelementptr ({ ptr }, ptr null, i32 1) to i64), i1 false)
  call void @_ZN6String8as_sliceEPN4scaly6memory4PageE(ptr noalias sret(%_Z5SliceI2u8E) %sret.result, ptr %frame, ptr %arg.tmp)
  call void @_ZN10JsonWriter6numberE5SliceI2u8E(ptr %0, ptr %sret.result)
  br label %choose.end

choose.when6:                                     ; preds = %entry
  %"variant.c_data().ptr7" = getelementptr inbounds nuw %_Z9JsonValue, ptr %1, i32 0, i32 1
  %variant.val8 = load { ptr }, ptr %"variant.c_data().ptr7", align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %arg.tmp10, ptr align 1 %"variant.c_data().ptr7", i64 ptrtoint (ptr getelementptr ({ ptr }, ptr null, i32 1) to i64), i1 false)
  call void @_ZN6String8as_sliceEPN4scaly6memory4PageE(ptr noalias sret(%_Z5SliceI2u8E) %sret.result9, ptr %frame, ptr %arg.tmp10)
  call void @_ZN10JsonWriter4textE5SliceI2u8E(ptr %0, ptr %sret.result9)
  br label %choose.end

choose.when11:                                    ; preds = %entry
  %"variant.c_data().ptr12" = getelementptr inbounds nuw %_Z9JsonValue, ptr %1, i32 0, i32 1
  %variant.val13 = load %_Z9JsonArray, ptr %"variant.c_data().ptr12", align 8
  call void @_ZN10JsonWriter11begin_arrayEv(ptr %0)
  store i64 0, ptr %i, align 1
  br label %while.cond

while.cond:                                       ; preds = %while.body, %choose.when11
  %i14 = load i64, ptr %i, align 8
  %items = extractvalue %_Z9JsonArray %variant.val13, 0
  %length = extractvalue %_Z5SliceI9JsonValueE %items, 0
  %lt = icmp ult i64 %i14, %length
  br i1 %lt, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %items16 = extractvalue %_Z9JsonArray %variant.val13, 0
  store %_Z5SliceI9JsonValueE %items16, ptr %arg.tmp17, align 1
  %i18 = load i64, ptr %i, align 8
  call void @_ZN5SliceI9JsonValueEixEm(ptr noalias sret(%_Z9JsonValue) %sret.result15, ptr %arg.tmp17, i64 %i18)
  call void @_ZN10JsonWriter5valueE9JsonValue(ptr %0, ptr %sret.result15)
  %i19 = load i64, ptr %i, align 8
  %add = add i64 %i19, 1
  store i64 %add, ptr %i, align 1
  br label %while.cond

while.exit:                                       ; preds = %while.cond
  call void @_ZN10JsonWriter9end_arrayEv(ptr %0)
  br label %choose.end

choose.when20:                                    ; preds = %entry
  %"variant.c_data().ptr21" = getelementptr inbounds nuw %_Z9JsonValue, ptr %1, i32 0, i32 1
  %variant.val22 = load %_Z10JsonObject, ptr %"variant.c_data().ptr21", align 8
  call void @_ZN10JsonWriter12begin_objectEv(ptr %0)
  store i64 0, ptr %i, align 1
  br label %while.cond23

while.cond23:                                     ; preds = %while.body24, %choose.when20
  %i26 = load i64, ptr %i, align 8
  %members = extractvalue %_Z10JsonObject %variant.val22, 0
  %length27 = extractvalue %_Z5SliceI10JsonMemberE %members, 0
  %lt28 = icmp ult i64 %i26, %length27
  br i1 %lt28, label %while.body24, label %while.exit25

while.body24:                                     ; preds = %while.cond23
  %members30 = extractvalue %_Z10JsonObject %variant.val22, 0
  store %_Z5SliceI10JsonMemberE %members30, ptr %arg.tmp31, align 1
  %i32 = load i64, ptr %i, align 8
  call void @_ZN5SliceI10JsonMemberEixEm(ptr noalias sret(%_Z10JsonMember) %sret.result29, ptr %arg.tmp31, i64 %i32)
  %field.inplace = getelementptr inbounds nuw %_Z10JsonMember, ptr %sret.result29, i32 0, i32 0
  call void @_ZN6String8as_sliceEPN4scaly6memory4PageE(ptr noalias sret(%_Z5SliceI2u8E) %sret.result33, ptr %frame, ptr %field.inplace)
  call void @_ZN10JsonWriter3keyE5SliceI2u8E(ptr %0, ptr %sret.result33)
  %field.inplace34 = getelementptr inbounds nuw %_Z10JsonMember, ptr %sret.result29, i32 0, i32 1
  call void @_ZN10JsonWriter5valueE9JsonValue(ptr %0, ptr %field.inplace34)
  %i35 = load i64, ptr %i, align 8
  %add36 = add i64 %i35, 1
  store i64 %add36, ptr %i, align 1
  br label %while.cond23

while.exit25:                                     ; preds = %while.cond23
  call void @_ZN10JsonWriter10end_objectEv(ptr %0)
  br label %choose.end
}

define linkonce_odr void @_ZN10JsonWriter3rawE5SliceI2u8E(ptr %0, ptr %1) {
entry:
  call void @_ZN10JsonWriter8separateEv(ptr %0)
  call void @_ZN10JsonWriter6appendE5SliceI2u8E(ptr %0, ptr %1)
  ret void
}

define linkonce_odr i1 @_ZN10JsonWriter5plainE2u8(i8 %0) {
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

define linkonce_odr void @_ZN10JsonWriter6escapeE2u8(ptr %0, i8 %1) {
entry:
  %arg.tmp = alloca %_Z5SliceI2u8E, align 8
  %tuple = alloca %_Z5SliceI2u8E, align 8
  call void @_ZN10JsonWriter4byteE2u8(ptr %0, i8 92)
  %call = call i8 @_ZN10JsonWriter13escape_letterE2u8(i8 %1)
  %ne = icmp ne i8 %call, 0
  br i1 %ne, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  call void @_ZN10JsonWriter4byteE2u8(ptr %0, i8 %call)
  br label %if.end

if.else:                                          ; preds = %entry
  %tuple.field = getelementptr inbounds nuw %_Z5SliceI2u8E, ptr %tuple, i32 0, i32 0
  store i64 3, ptr %tuple.field, align 1
  %tuple.field1 = getelementptr inbounds nuw %_Z5SliceI2u8E, ptr %tuple, i32 0, i32 1
  store ptr @.str.44, ptr %tuple.field1, align 1
  %tuple.val = load %_Z5SliceI2u8E, ptr %tuple, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %arg.tmp, ptr align 1 %tuple, i64 ptrtoint (ptr getelementptr (%_Z5SliceI2u8E, ptr null, i32 1) to i64), i1 false)
  call void @_ZN10JsonWriter6appendE5SliceI2u8E(ptr %0, ptr %arg.tmp)
  %zext = zext i8 %1 to i64
  %lshr = lshr i64 %zext, 4
  %call2 = call i8 @_ZN10JsonWriter9hex_digitE2u8(i64 %lshr)
  call void @_ZN10JsonWriter4byteE2u8(ptr %0, i8 %call2)
  %and = and i8 %1, 15
  %call3 = call i8 @_ZN10JsonWriter9hex_digitE2u8(i8 %and)
  call void @_ZN10JsonWriter4byteE2u8(ptr %0, i8 %call3)
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  ret void
}

define linkonce_odr i8 @_ZN10JsonWriter13escape_letterE2u8(i8 %0) {
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

define linkonce_odr i8 @_ZN10JsonWriter9hex_digitE2u8(i8 %0) {
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

define linkonce_odr void @_ZN10JsonWriter13append_quotedER13StringBuilder5SliceI2u8E(ptr %0, ptr %1) {
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
  %call3 = call i1 @_ZN10JsonWriter5plainE2u8(i8 %call)
  br i1 %call3, label %if.then, label %if.else

while.exit:                                       ; preds = %while.cond
  call void @_ZN13StringBuilder6appendEc(ptr %0, i8 34)
  ret void

if.then:                                          ; preds = %while.body
  call void @_ZN13StringBuilder6appendEc(ptr %0, i8 %call)
  br label %if.end

if.else:                                          ; preds = %while.body
  call void @_ZN13StringBuilder6appendEc(ptr %0, i8 92)
  %call4 = call i8 @_ZN10JsonWriter13escape_letterE2u8(i8 %call)
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
  %call8 = call i8 @_ZN10JsonWriter9hex_digitE2u8(i64 %lshr)
  call void @_ZN13StringBuilder6appendEc(ptr %0, i8 %call8)
  %and = and i8 %call, 15
  %call9 = call i8 @_ZN10JsonWriter9hex_digitE2u8(i8 %and)
  call void @_ZN13StringBuilder6appendEc(ptr %0, i8 %call9)
  br label %if.end7

if.end7:                                          ; preds = %if.else6, %if.then5
  br label %if.end
}

define linkonce_odr void @_ZN10JsonWriter5quoteEPN4scaly6memory4PageE5SliceI2u8E(ptr noalias sret({ ptr }) %0, ptr %1, ptr %2) {
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
  call void @_ZN10JsonWriter13append_quotedER13StringBuilder5SliceI2u8E(ptr %sb2, ptr %2)
  %sb3 = load ptr, ptr %sb, align 8
  call void @_ZN13StringBuilder9to_stringEPN4scaly6memory4PageE(ptr noalias sret({ ptr }) %sret.result, ptr %1, ptr %sb3)
  call void @_Z19scaly_release_frameP5Frame(ptr %frame)
  %sret.body = load { ptr }, ptr %sret.result, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %sret.result, i64 ptrtoint (ptr getelementptr ({ ptr }, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN10JsonWriter5quoteEPN4scaly6memory4PageE6String(ptr noalias sret({ ptr }) %0, ptr %1, ptr %2) {
entry:
  %sret.result = alloca { ptr }, align 8
  %sret.result1 = alloca %_Z5SliceI2u8E, align 8
  call void @_ZN6String8as_sliceEPN4scaly6memory4PageE(ptr noalias sret(%_Z5SliceI2u8E) %sret.result1, ptr %1, ptr %2)
  call void @_ZN10JsonWriter5quoteEPN4scaly6memory4PageE5SliceI2u8E(ptr noalias sret({ ptr }) %sret.result, ptr %1, ptr %sret.result1)
  %sret.body = load { ptr }, ptr %sret.result, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %sret.result, i64 ptrtoint (ptr getelementptr ({ ptr }, ptr null, i32 1) to i64), i1 false)
  ret void
}

declare void @_ZN13StringBuilder6appendE6String(ptr, ptr)

define linkonce_odr void @_ZN10JsonWriter12append_valueER13StringBuilder9JsonValue(ptr %0, ptr %1) {
entry:
  %sret.result42 = alloca %_Z5SliceI2u8E, align 8
  %arg.tmp40 = alloca %_Z5SliceI10JsonMemberE, align 8
  %sret.result38 = alloca %_Z10JsonMember, align 8
  %arg.tmp22 = alloca %_Z5SliceI9JsonValueE, align 8
  %sret.result20 = alloca %_Z9JsonValue, align 8
  %i = alloca i64, align 8
  %arg.tmp12 = alloca { ptr }, align 8
  %frame = alloca { ptr, ptr }, align 8
  store ptr null, ptr %frame, align 8
  %frame.parent = getelementptr inbounds nuw { ptr, ptr }, ptr %frame, i32 0, i32 1
  store ptr null, ptr %frame.parent, align 8
  %sret.result = alloca %_Z5SliceI2u8E, align 8
  %arg.tmp8 = alloca { ptr }, align 8
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
  call void @_Z19scaly_release_frameP5Frame(ptr %frame)
  ret void

choose.else:                                      ; preds = %entry
  br label %choose.end

choose.when:                                      ; preds = %entry
  %"variant.c_data().ptr" = getelementptr inbounds nuw %_Z9JsonValue, ptr %1, i32 0, i32 1
  store { ptr } { ptr @.sconst.45 }, ptr %arg.tmp, align 1
  call void @_ZN13StringBuilder6appendE6String(ptr %0, ptr %arg.tmp)
  br label %choose.end

choose.when1:                                     ; preds = %entry
  %"variant.c_data().ptr2" = getelementptr inbounds nuw %_Z9JsonValue, ptr %1, i32 0, i32 1
  %variant.val = load i1, ptr %"variant.c_data().ptr2", align 1
  br i1 %variant.val, label %if.then, label %if.else

if.then:                                          ; preds = %choose.when1
  store { ptr } { ptr @.sconst.46 }, ptr %arg.tmp3, align 1
  call void @_ZN13StringBuilder6appendE6String(ptr %0, ptr %arg.tmp3)
  br label %if.end

if.else:                                          ; preds = %choose.when1
  store { ptr } { ptr @.sconst.47 }, ptr %arg.tmp4, align 1
  call void @_ZN13StringBuilder6appendE6String(ptr %0, ptr %arg.tmp4)
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  br label %choose.end

choose.when5:                                     ; preds = %entry
  %"variant.c_data().ptr6" = getelementptr inbounds nuw %_Z9JsonValue, ptr %1, i32 0, i32 1
  %variant.val7 = load { ptr }, ptr %"variant.c_data().ptr6", align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %arg.tmp8, ptr align 1 %"variant.c_data().ptr6", i64 ptrtoint (ptr getelementptr ({ ptr }, ptr null, i32 1) to i64), i1 false)
  call void @_ZN13StringBuilder6appendE6String(ptr %0, ptr %arg.tmp8)
  br label %choose.end

choose.when9:                                     ; preds = %entry
  %"variant.c_data().ptr10" = getelementptr inbounds nuw %_Z9JsonValue, ptr %1, i32 0, i32 1
  %variant.val11 = load { ptr }, ptr %"variant.c_data().ptr10", align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %arg.tmp12, ptr align 1 %"variant.c_data().ptr10", i64 ptrtoint (ptr getelementptr ({ ptr }, ptr null, i32 1) to i64), i1 false)
  call void @_ZN6String8as_sliceEPN4scaly6memory4PageE(ptr noalias sret(%_Z5SliceI2u8E) %sret.result, ptr %frame, ptr %arg.tmp12)
  call void @_ZN10JsonWriter13append_quotedER13StringBuilder5SliceI2u8E(ptr %0, ptr %sret.result)
  br label %choose.end

choose.when13:                                    ; preds = %entry
  %"variant.c_data().ptr14" = getelementptr inbounds nuw %_Z9JsonValue, ptr %1, i32 0, i32 1
  %variant.val15 = load %_Z9JsonArray, ptr %"variant.c_data().ptr14", align 8
  call void @_ZN13StringBuilder6appendEc(ptr %0, i8 91)
  store i64 0, ptr %i, align 1
  br label %while.cond

while.cond:                                       ; preds = %if.end19, %choose.when13
  %i16 = load i64, ptr %i, align 8
  %items = extractvalue %_Z9JsonArray %variant.val15, 0
  %length = extractvalue %_Z5SliceI9JsonValueE %items, 0
  %lt = icmp ult i64 %i16, %length
  br i1 %lt, label %while.body, label %while.exit

while.body:                                       ; preds = %while.cond
  %i17 = load i64, ptr %i, align 8
  %gt = icmp ugt i64 %i17, 0
  br i1 %gt, label %if.then18, label %if.end19

while.exit:                                       ; preds = %while.cond
  call void @_ZN13StringBuilder6appendEc(ptr %0, i8 93)
  br label %choose.end

if.then18:                                        ; preds = %while.body
  call void @_ZN13StringBuilder6appendEc(ptr %0, i8 44)
  br label %if.end19

if.end19:                                         ; preds = %if.then18, %while.body
  %items21 = extractvalue %_Z9JsonArray %variant.val15, 0
  store %_Z5SliceI9JsonValueE %items21, ptr %arg.tmp22, align 1
  %i23 = load i64, ptr %i, align 8
  call void @_ZN5SliceI9JsonValueEixEm(ptr noalias sret(%_Z9JsonValue) %sret.result20, ptr %arg.tmp22, i64 %i23)
  call void @_ZN10JsonWriter12append_valueER13StringBuilder9JsonValue(ptr %0, ptr %sret.result20)
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
  call void @_ZN5SliceI10JsonMemberEixEm(ptr noalias sret(%_Z10JsonMember) %sret.result38, ptr %arg.tmp40, i64 %i41)
  %field.inplace = getelementptr inbounds nuw %_Z10JsonMember, ptr %sret.result38, i32 0, i32 0
  call void @_ZN6String8as_sliceEPN4scaly6memory4PageE(ptr noalias sret(%_Z5SliceI2u8E) %sret.result42, ptr %frame, ptr %field.inplace)
  call void @_ZN10JsonWriter13append_quotedER13StringBuilder5SliceI2u8E(ptr %0, ptr %sret.result42)
  call void @_ZN13StringBuilder6appendEc(ptr %0, i8 58)
  %field.inplace43 = getelementptr inbounds nuw %_Z10JsonMember, ptr %sret.result38, i32 0, i32 1
  call void @_ZN10JsonWriter12append_valueER13StringBuilder9JsonValue(ptr %0, ptr %field.inplace43)
  %i44 = load i64, ptr %i, align 8
  %add45 = add i64 %i44, 1
  store i64 %add45, ptr %i, align 1
  br label %while.cond28
}

define linkonce_odr void @_ZN10JsonWriter9stringifyEPN4scaly6memory4PageE9JsonValue(ptr noalias sret({ ptr }) %0, ptr %1, ptr %2) {
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
  call void @_ZN10JsonWriter12append_valueER13StringBuilder9JsonValue(ptr %sb2, ptr %2)
  %sb3 = load ptr, ptr %sb, align 8
  call void @_ZN13StringBuilder9to_stringEPN4scaly6memory4PageE(ptr noalias sret({ ptr }) %sret.result, ptr %1, ptr %sb3)
  call void @_Z19scaly_release_frameP5Frame(ptr %frame)
  %sret.body = load { ptr }, ptr %sret.result, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 %sret.result, i64 ptrtoint (ptr getelementptr ({ ptr }, ptr null, i32 1) to i64), i1 false)
  ret void
}

define linkonce_odr void @_ZN10JsonWriterC1Ev(ptr %0) {
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
declare void @llvm.memcpy.p0.p0.i64(ptr noalias nocapture writeonly, ptr noalias nocapture readonly, i64, i1 immarg) #1

declare void @_Z19scaly_release_frameP5Frame(ptr)

declare i32 @memcmp(...)

declare ptr @_Z17scaly_force_frameP5Frame(ptr)

declare void @_ZN6StringC1EP10const_charm(ptr, ptr, i64)

declare void @_ZN13StringBuilderC1Ev(ptr)

declare void @_ZN6StringC1Ev(ptr)

declare ptr @_Z3getPv(ptr)

; Function Attrs: cold noinline noreturn
declare void @_Z16scaly_panic_sizeP10const_charmm(...) #0

attributes #0 = { cold noinline noreturn }
attributes #1 = { nocallback nofree nounwind willreturn memory(argmem: readwrite) }
