##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Debug
ProjectName            :=scalycpp
ConfigurationName      :=Debug
WorkspacePath          :=/home/ralf/Scaly
ProjectPath            :=/home/ralf/Scaly/scalycpp
IntermediateDirectory  :=../Debug
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=Ralf Schleitzer
Date                   :=25/05/17
CodeLitePath           :=/home/ralf/.codelite
LinkerName             :=/usr/bin/g++
SharedObjectLinkerName :=/usr/bin/g++ -shared -fPIC
ObjectSuffix           :=.o
DependSuffix           :=.o.d
PreprocessSuffix       :=.i
DebugSwitch            :=-g 
IncludeSwitch          :=-I
LibrarySwitch          :=-l
OutputSwitch           :=-o 
LibraryPathSwitch      :=-L
PreprocessorSwitch     :=-D
SourceSwitch           :=-c 
OutputFile             :=$(IntermediateDirectory)/$(ProjectName)
Preprocessors          :=
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E
ObjectsFileList        :="scalycpp.txt"
PCHCompileFlags        :=
MakeDirCommand         :=mkdir -p
LinkOptions            :=  
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch). $(IncludeSwitch)../scaly 
IncludePCH             := 
RcIncludePath          := 
Libs                   := $(LibrarySwitch)scaly 
ArLibs                 :=  "libscaly" 
LibPath                := $(LibraryPathSwitch). $(LibraryPathSwitch)../Debug 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := /usr/bin/ar rcu
CXX      := /usr/bin/g++
CC       := /usr/bin/gcc
CXXFLAGS :=  -g -O0 -std=c++11 -Wall $(Preprocessors)
CFLAGS   :=  -g -O0 -Wall $(Preprocessors)
ASFLAGS  := 
AS       := /usr/bin/as


##
## User defined environment variables
##
CodeLiteDir:=/usr/share/codelite
Objects0=$(IntermediateDirectory)/Lexer.cpp$(ObjectSuffix) $(IntermediateDirectory)/OptionsError.cpp$(ObjectSuffix) $(IntermediateDirectory)/Parser.cpp$(ObjectSuffix) $(IntermediateDirectory)/CommonVisitor.cpp$(ObjectSuffix) $(IntermediateDirectory)/CppVisitor.cpp$(ObjectSuffix) $(IntermediateDirectory)/Model.cpp$(ObjectSuffix) $(IntermediateDirectory)/Options.cpp$(ObjectSuffix) $(IntermediateDirectory)/Compiler.cpp$(ObjectSuffix) $(IntermediateDirectory)/scalycpp.cpp$(ObjectSuffix) 



Objects=$(Objects0) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild MakeIntermediateDirs
all: $(OutputFile)

$(OutputFile): $(IntermediateDirectory)/.d $(Objects) 
	@$(MakeDirCommand) $(@D)
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects0)  > $(ObjectsFileList)
	$(LinkerName) $(OutputSwitch)$(OutputFile) @$(ObjectsFileList) $(LibPath) $(Libs) $(LinkOptions)

MakeIntermediateDirs:
	@test -d ../Debug || $(MakeDirCommand) ../Debug


$(IntermediateDirectory)/.d:
	@test -d ../Debug || $(MakeDirCommand) ../Debug

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/Lexer.cpp$(ObjectSuffix): Lexer.cpp $(IntermediateDirectory)/Lexer.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/ralf/Scaly/scalycpp/Lexer.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Lexer.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Lexer.cpp$(DependSuffix): Lexer.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Lexer.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/Lexer.cpp$(DependSuffix) -MM "Lexer.cpp"

$(IntermediateDirectory)/Lexer.cpp$(PreprocessSuffix): Lexer.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Lexer.cpp$(PreprocessSuffix) "Lexer.cpp"

$(IntermediateDirectory)/OptionsError.cpp$(ObjectSuffix): OptionsError.cpp $(IntermediateDirectory)/OptionsError.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/ralf/Scaly/scalycpp/OptionsError.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/OptionsError.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/OptionsError.cpp$(DependSuffix): OptionsError.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/OptionsError.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/OptionsError.cpp$(DependSuffix) -MM "OptionsError.cpp"

$(IntermediateDirectory)/OptionsError.cpp$(PreprocessSuffix): OptionsError.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/OptionsError.cpp$(PreprocessSuffix) "OptionsError.cpp"

$(IntermediateDirectory)/Parser.cpp$(ObjectSuffix): Parser.cpp $(IntermediateDirectory)/Parser.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/ralf/Scaly/scalycpp/Parser.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Parser.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Parser.cpp$(DependSuffix): Parser.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Parser.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/Parser.cpp$(DependSuffix) -MM "Parser.cpp"

$(IntermediateDirectory)/Parser.cpp$(PreprocessSuffix): Parser.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Parser.cpp$(PreprocessSuffix) "Parser.cpp"

$(IntermediateDirectory)/CommonVisitor.cpp$(ObjectSuffix): CommonVisitor.cpp $(IntermediateDirectory)/CommonVisitor.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/ralf/Scaly/scalycpp/CommonVisitor.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CommonVisitor.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CommonVisitor.cpp$(DependSuffix): CommonVisitor.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CommonVisitor.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CommonVisitor.cpp$(DependSuffix) -MM "CommonVisitor.cpp"

$(IntermediateDirectory)/CommonVisitor.cpp$(PreprocessSuffix): CommonVisitor.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CommonVisitor.cpp$(PreprocessSuffix) "CommonVisitor.cpp"

$(IntermediateDirectory)/CppVisitor.cpp$(ObjectSuffix): CppVisitor.cpp $(IntermediateDirectory)/CppVisitor.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/ralf/Scaly/scalycpp/CppVisitor.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CppVisitor.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CppVisitor.cpp$(DependSuffix): CppVisitor.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CppVisitor.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CppVisitor.cpp$(DependSuffix) -MM "CppVisitor.cpp"

$(IntermediateDirectory)/CppVisitor.cpp$(PreprocessSuffix): CppVisitor.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CppVisitor.cpp$(PreprocessSuffix) "CppVisitor.cpp"

$(IntermediateDirectory)/Model.cpp$(ObjectSuffix): Model.cpp $(IntermediateDirectory)/Model.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/ralf/Scaly/scalycpp/Model.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Model.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Model.cpp$(DependSuffix): Model.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Model.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/Model.cpp$(DependSuffix) -MM "Model.cpp"

$(IntermediateDirectory)/Model.cpp$(PreprocessSuffix): Model.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Model.cpp$(PreprocessSuffix) "Model.cpp"

$(IntermediateDirectory)/Options.cpp$(ObjectSuffix): Options.cpp $(IntermediateDirectory)/Options.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/ralf/Scaly/scalycpp/Options.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Options.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Options.cpp$(DependSuffix): Options.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Options.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/Options.cpp$(DependSuffix) -MM "Options.cpp"

$(IntermediateDirectory)/Options.cpp$(PreprocessSuffix): Options.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Options.cpp$(PreprocessSuffix) "Options.cpp"

$(IntermediateDirectory)/Compiler.cpp$(ObjectSuffix): Compiler.cpp $(IntermediateDirectory)/Compiler.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/ralf/Scaly/scalycpp/Compiler.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Compiler.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Compiler.cpp$(DependSuffix): Compiler.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Compiler.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/Compiler.cpp$(DependSuffix) -MM "Compiler.cpp"

$(IntermediateDirectory)/Compiler.cpp$(PreprocessSuffix): Compiler.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Compiler.cpp$(PreprocessSuffix) "Compiler.cpp"

$(IntermediateDirectory)/scalycpp.cpp$(ObjectSuffix): scalycpp.cpp $(IntermediateDirectory)/scalycpp.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/ralf/Scaly/scalycpp/scalycpp.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/scalycpp.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/scalycpp.cpp$(DependSuffix): scalycpp.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/scalycpp.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/scalycpp.cpp$(DependSuffix) -MM "scalycpp.cpp"

$(IntermediateDirectory)/scalycpp.cpp$(PreprocessSuffix): scalycpp.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/scalycpp.cpp$(PreprocessSuffix) "scalycpp.cpp"


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) -r ../Debug/


