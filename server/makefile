##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Debug
ProjectName            :=server
ConfigurationName      :=Debug
WorkspacePath          := "/home/henyouqian/projects/cmatch"
ProjectPath            := "/root/projects/cmatch/server"
IntermediateDirectory  :=./Debug
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=李炜
Date                   :=2013年01月19日
CodeLitePath           :="/home/henyouqian/.codelite"
LinkerName             :=g++
SharedObjectLinkerName :=g++ -shared -fPIC
ObjectSuffix           :=.o
DependSuffix           :=.o.d
PreprocessSuffix       :=.o.i
DebugSwitch            :=-gstab
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
ObjectsFileList        :="/root/projects/cmatch/server/server.txt"
PCHCompileFlags        :=
MakeDirCommand         :=mkdir -p
LinkOptions            :=  
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch). 
IncludePCH             := 
RcIncludePath          := 
Libs                   := $(LibrarySwitch)evhtp $(LibrarySwitch)event $(LibrarySwitch)event_openssl $(LibrarySwitch)pq $(LibrarySwitch)uuid $(LibrarySwitch)hiredis 
ArLibs                 :=  "evhtp" "event" "event_openssl" "pq" "uuid" "hiredis" 
LibPath                := $(LibraryPathSwitch). 

##
## Common variables
## AR, CXX, CC, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := ar rcus
CXX      := g++
CC       := gcc
CXXFLAGS :=  -g -O0 -Wall -std=c++0x $(Preprocessors)
CFLAGS   :=  -g -O0 -Wall $(Preprocessors)


##
## User defined environment variables
##
CodeLiteDir:=/usr/share/codelite
Objects=$(IntermediateDirectory)/cm_main$(ObjectSuffix) $(IntermediateDirectory)/cm_callback$(ObjectSuffix) $(IntermediateDirectory)/cm_test$(ObjectSuffix) $(IntermediateDirectory)/cm_thread$(ObjectSuffix) $(IntermediateDirectory)/cm_util$(ObjectSuffix) $(IntermediateDirectory)/cm_account$(ObjectSuffix) $(IntermediateDirectory)/cm_match$(ObjectSuffix) $(IntermediateDirectory)/cm_rps$(ObjectSuffix) $(IntermediateDirectory)/cm_dig$(ObjectSuffix) $(IntermediateDirectory)/cm_context$(ObjectSuffix) \
	

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild
all: $(OutputFile)

$(OutputFile): $(IntermediateDirectory)/.d $(Objects) 
	@$(MakeDirCommand) $(@D)
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects) > $(ObjectsFileList)
	$(LinkerName) $(OutputSwitch)$(OutputFile) @$(ObjectsFileList) $(LibPath) $(Libs) $(LinkOptions)

$(IntermediateDirectory)/.d:
	@test -d ./Debug || $(MakeDirCommand) ./Debug

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/cm_main$(ObjectSuffix): cm_main.cpp $(IntermediateDirectory)/cm_main$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/root/projects/cmatch/server/cm_main.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/cm_main$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/cm_main$(DependSuffix): cm_main.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/cm_main$(ObjectSuffix) -MF$(IntermediateDirectory)/cm_main$(DependSuffix) -MM "/root/projects/cmatch/server/cm_main.cpp"

$(IntermediateDirectory)/cm_main$(PreprocessSuffix): cm_main.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/cm_main$(PreprocessSuffix) "/root/projects/cmatch/server/cm_main.cpp"

$(IntermediateDirectory)/cm_callback$(ObjectSuffix): cm_callback.cpp $(IntermediateDirectory)/cm_callback$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/root/projects/cmatch/server/cm_callback.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/cm_callback$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/cm_callback$(DependSuffix): cm_callback.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/cm_callback$(ObjectSuffix) -MF$(IntermediateDirectory)/cm_callback$(DependSuffix) -MM "/root/projects/cmatch/server/cm_callback.cpp"

$(IntermediateDirectory)/cm_callback$(PreprocessSuffix): cm_callback.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/cm_callback$(PreprocessSuffix) "/root/projects/cmatch/server/cm_callback.cpp"

$(IntermediateDirectory)/cm_test$(ObjectSuffix): cm_test.cpp $(IntermediateDirectory)/cm_test$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/root/projects/cmatch/server/cm_test.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/cm_test$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/cm_test$(DependSuffix): cm_test.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/cm_test$(ObjectSuffix) -MF$(IntermediateDirectory)/cm_test$(DependSuffix) -MM "/root/projects/cmatch/server/cm_test.cpp"

$(IntermediateDirectory)/cm_test$(PreprocessSuffix): cm_test.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/cm_test$(PreprocessSuffix) "/root/projects/cmatch/server/cm_test.cpp"

$(IntermediateDirectory)/cm_thread$(ObjectSuffix): cm_thread.cpp $(IntermediateDirectory)/cm_thread$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/root/projects/cmatch/server/cm_thread.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/cm_thread$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/cm_thread$(DependSuffix): cm_thread.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/cm_thread$(ObjectSuffix) -MF$(IntermediateDirectory)/cm_thread$(DependSuffix) -MM "/root/projects/cmatch/server/cm_thread.cpp"

$(IntermediateDirectory)/cm_thread$(PreprocessSuffix): cm_thread.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/cm_thread$(PreprocessSuffix) "/root/projects/cmatch/server/cm_thread.cpp"

$(IntermediateDirectory)/cm_util$(ObjectSuffix): cm_util.cpp $(IntermediateDirectory)/cm_util$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/root/projects/cmatch/server/cm_util.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/cm_util$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/cm_util$(DependSuffix): cm_util.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/cm_util$(ObjectSuffix) -MF$(IntermediateDirectory)/cm_util$(DependSuffix) -MM "/root/projects/cmatch/server/cm_util.cpp"

$(IntermediateDirectory)/cm_util$(PreprocessSuffix): cm_util.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/cm_util$(PreprocessSuffix) "/root/projects/cmatch/server/cm_util.cpp"

$(IntermediateDirectory)/cm_account$(ObjectSuffix): cm_account.cpp $(IntermediateDirectory)/cm_account$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/root/projects/cmatch/server/cm_account.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/cm_account$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/cm_account$(DependSuffix): cm_account.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/cm_account$(ObjectSuffix) -MF$(IntermediateDirectory)/cm_account$(DependSuffix) -MM "/root/projects/cmatch/server/cm_account.cpp"

$(IntermediateDirectory)/cm_account$(PreprocessSuffix): cm_account.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/cm_account$(PreprocessSuffix) "/root/projects/cmatch/server/cm_account.cpp"

$(IntermediateDirectory)/cm_match$(ObjectSuffix): cm_match.cpp $(IntermediateDirectory)/cm_match$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/root/projects/cmatch/server/cm_match.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/cm_match$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/cm_match$(DependSuffix): cm_match.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/cm_match$(ObjectSuffix) -MF$(IntermediateDirectory)/cm_match$(DependSuffix) -MM "/root/projects/cmatch/server/cm_match.cpp"

$(IntermediateDirectory)/cm_match$(PreprocessSuffix): cm_match.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/cm_match$(PreprocessSuffix) "/root/projects/cmatch/server/cm_match.cpp"

$(IntermediateDirectory)/cm_rps$(ObjectSuffix): cm_rps.cpp $(IntermediateDirectory)/cm_rps$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/root/projects/cmatch/server/cm_rps.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/cm_rps$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/cm_rps$(DependSuffix): cm_rps.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/cm_rps$(ObjectSuffix) -MF$(IntermediateDirectory)/cm_rps$(DependSuffix) -MM "/root/projects/cmatch/server/cm_rps.cpp"

$(IntermediateDirectory)/cm_rps$(PreprocessSuffix): cm_rps.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/cm_rps$(PreprocessSuffix) "/root/projects/cmatch/server/cm_rps.cpp"

$(IntermediateDirectory)/cm_dig$(ObjectSuffix): cm_dig.cpp $(IntermediateDirectory)/cm_dig$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/root/projects/cmatch/server/cm_dig.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/cm_dig$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/cm_dig$(DependSuffix): cm_dig.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/cm_dig$(ObjectSuffix) -MF$(IntermediateDirectory)/cm_dig$(DependSuffix) -MM "/root/projects/cmatch/server/cm_dig.cpp"

$(IntermediateDirectory)/cm_dig$(PreprocessSuffix): cm_dig.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/cm_dig$(PreprocessSuffix) "/root/projects/cmatch/server/cm_dig.cpp"

$(IntermediateDirectory)/cm_context$(ObjectSuffix): cm_context.cpp $(IntermediateDirectory)/cm_context$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/root/projects/cmatch/server/cm_context.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/cm_context$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/cm_context$(DependSuffix): cm_context.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/cm_context$(ObjectSuffix) -MF$(IntermediateDirectory)/cm_context$(DependSuffix) -MM "/root/projects/cmatch/server/cm_context.cpp"

$(IntermediateDirectory)/cm_context$(PreprocessSuffix): cm_context.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/cm_context$(PreprocessSuffix) "/root/projects/cmatch/server/cm_context.cpp"


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) $(IntermediateDirectory)/cm_main$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/cm_main$(DependSuffix)
	$(RM) $(IntermediateDirectory)/cm_main$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/cm_callback$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/cm_callback$(DependSuffix)
	$(RM) $(IntermediateDirectory)/cm_callback$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/cm_test$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/cm_test$(DependSuffix)
	$(RM) $(IntermediateDirectory)/cm_test$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/cm_thread$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/cm_thread$(DependSuffix)
	$(RM) $(IntermediateDirectory)/cm_thread$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/cm_util$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/cm_util$(DependSuffix)
	$(RM) $(IntermediateDirectory)/cm_util$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/cm_account$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/cm_account$(DependSuffix)
	$(RM) $(IntermediateDirectory)/cm_account$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/cm_match$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/cm_match$(DependSuffix)
	$(RM) $(IntermediateDirectory)/cm_match$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/cm_rps$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/cm_rps$(DependSuffix)
	$(RM) $(IntermediateDirectory)/cm_rps$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/cm_dig$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/cm_dig$(DependSuffix)
	$(RM) $(IntermediateDirectory)/cm_dig$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/cm_context$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/cm_context$(DependSuffix)
	$(RM) $(IntermediateDirectory)/cm_context$(PreprocessSuffix)
	$(RM) $(OutputFile)
	$(RM) "/root/projects/cmatch/.build-debug/server"


