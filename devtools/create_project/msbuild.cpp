/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include "config.h"
#include "msbuild.h"

#include <fstream>
#include <algorithm>

namespace CreateProjectTool {

//////////////////////////////////////////////////////////////////////////
// MSBuild Provider (Visual Studio 2010 and later)
//////////////////////////////////////////////////////////////////////////

MSBuildProvider::MSBuildProvider(StringList &global_warnings, std::map<std::string, StringList> &project_warnings, const int version, const MSVCVersion& msvc)
	: MSVCProvider(global_warnings, project_warnings, version, msvc) {
}

const char *MSBuildProvider::getProjectExtension() {
	return ".vcxproj";
}

const char *MSBuildProvider::getPropertiesExtension() {
	return ".props";
}

namespace {

inline void outputConfiguration(std::ostream &project, const std::string &config, const std::string &platform) {
	project << "\t\t<ProjectConfiguration Include=\"" << config << "|" << platform << "\">\n"
	           "\t\t\t<Configuration>" << config << "</Configuration>\n"
	           "\t\t\t<Platform>" << platform << "</Platform>\n"
	           "\t\t</ProjectConfiguration>\n";
}

inline void outputConfigurationType(const BuildSetup &setup, std::ostream &project, const std::string &name, const std::string &config, const std::string &toolset) {
	project << "\t<PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='" << config << "'\" Label=\"Configuration\">\n"
	           "\t\t<ConfigurationType>" << ((name == setup.projectName || setup.devTools || setup.tests) ? "Application" : "StaticLibrary") << "</ConfigurationType>\n"
	           "\t\t<PlatformToolset>" << toolset << "</PlatformToolset>\n"
	           "\t</PropertyGroup>\n";
}

inline void outputProperties(std::ostream &project, const std::string &config, const std::string &properties) {
	project << "\t<ImportGroup Condition=\"'$(Configuration)|$(Platform)'=='" << config << "'\" Label=\"PropertySheets\">\n"
	           "\t\t<Import Project=\"$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props\" Condition=\"exists('$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props')\" Label=\"LocalAppDataPlatform\" />\n"
	           "\t\t<Import Project=\"" << properties << "\" />\n"
	           "\t</ImportGroup>\n";
}

} // End of anonymous namespace

void MSBuildProvider::createProjectFile(const std::string &name, const std::string &uuid, const BuildSetup &setup, const std::string &moduleDir,
                                        const StringList &includeList, const StringList &excludeList) {
	const std::string projectFile = setup.outputDir + '/' + name + getProjectExtension();
	std::ofstream project(projectFile.c_str());
	if (!project)
		error("Could not open \"" + projectFile + "\" for writing");

	project << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
	           "<Project DefaultTargets=\"Build\" ToolsVersion=\"" << _msvcVersion.project << "\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">\n"
	           "\t<ItemGroup Label=\"ProjectConfigurations\">\n";

	outputConfiguration(project, "Debug", "Win32");
	outputConfiguration(project, "Debug", "x64");
	outputConfiguration(project, "Analysis", "Win32");
	outputConfiguration(project, "Analysis", "x64");
	outputConfiguration(project, "LLVM", "Win32");
	outputConfiguration(project, "LLVM", "x64");
	outputConfiguration(project, "Release", "Win32");
	outputConfiguration(project, "Release", "x64");

	project << "\t</ItemGroup>\n";

	// Project name & Guid
	project << "\t<PropertyGroup Label=\"Globals\">\n"
	           "\t\t<ProjectGuid>{" << uuid << "}</ProjectGuid>\n"
	           "\t\t<RootNamespace>" << name << "</RootNamespace>\n"
	           "\t\t<Keyword>Win32Proj</Keyword>\n"
	           "\t\t<VCTargetsPath Condition=\"'$(VCTargetsPath" << _version << ")' != '' and '$(VSVersion)' == '' and $(VisualStudioVersion) == ''\">$(VCTargetsPath" << _version << ")</VCTargetsPath>\n"
	           "\t</PropertyGroup>\n";

	// Shared configuration
	project << "\t<Import Project=\"$(VCTargetsPath)\\Microsoft.Cpp.Default.props\" />\n";

	outputConfigurationType(setup, project, name, "Release|Win32", _msvcVersion.toolsetMSVC);
	outputConfigurationType(setup, project, name, "Analysis|Win32", _msvcVersion.toolsetMSVC);
	outputConfigurationType(setup, project, name, "LLVM|Win32", _msvcVersion.toolsetLLVM);
	outputConfigurationType(setup, project, name, "Debug|Win32", _msvcVersion.toolsetMSVC);
	outputConfigurationType(setup, project, name, "Release|x64", _msvcVersion.toolsetMSVC);
	outputConfigurationType(setup, project, name, "LLVM|x64", _msvcVersion.toolsetLLVM);
	outputConfigurationType(setup, project, name, "Analysis|x64", _msvcVersion.toolsetMSVC);
	outputConfigurationType(setup, project, name, "Debug|x64", _msvcVersion.toolsetMSVC);

	project << "\t<Import Project=\"$(VCTargetsPath)\\Microsoft.Cpp.props\" />\n"
	           "\t<ImportGroup Label=\"ExtensionSettings\">\n"
	           "\t</ImportGroup>\n";

	outputProperties(project, "Release|Win32",  setup.projectDescription + "_Release.props");
	outputProperties(project, "Analysis|Win32", setup.projectDescription + "_Analysis.props");
	outputProperties(project, "LLVM|Win32",     setup.projectDescription + "_LLVM.props");
	outputProperties(project, "Debug|Win32",    setup.projectDescription + "_Debug.props");
	outputProperties(project, "Release|x64",    setup.projectDescription + "_Release64.props");
	outputProperties(project, "Analysis|x64",   setup.projectDescription + "_Analysis64.props");
	outputProperties(project, "LLVM|x64",       setup.projectDescription + "_LLVM64.props");
	outputProperties(project, "Debug|x64",      setup.projectDescription + "_Debug64.props");

	project << "\t<PropertyGroup Label=\"UserMacros\" />\n";

	// Project-specific settings (analysis uses debug properties)
	outputProjectSettings(project, name, setup, false, true, "Debug");
	outputProjectSettings(project, name, setup, false, true, "Analysis");
	outputProjectSettings(project, name, setup, false, true, "LLVM");
	outputProjectSettings(project, name, setup, true, true, "Release");
	outputProjectSettings(project, name, setup, false, false, "Debug");
	outputProjectSettings(project, name, setup, false, false, "Analysis");
	outputProjectSettings(project, name, setup, false, false, "LLVM");
	outputProjectSettings(project, name, setup, true, false, "Release");

	// Files
	std::string modulePath;
	if (!moduleDir.compare(0, setup.srcDir.size(), setup.srcDir)) {
		modulePath = moduleDir.substr(setup.srcDir.size());
		if (!modulePath.empty() && modulePath.at(0) == '/')
			modulePath.erase(0, 1);
	}

	if (modulePath.size())
		addFilesToProject(moduleDir, project, includeList, excludeList, setup.filePrefix + '/' + modulePath);
	else
		addFilesToProject(moduleDir, project, includeList, excludeList, setup.filePrefix);

	// Output references for the main project
	if (name == setup.projectName)
		writeReferences(setup, project);

	// Output auto-generated test runner
	if (setup.tests) {
		project << "\t<ItemGroup>\n";
		project << "\t\t<ClCompile Include=\"test_runner.cpp\" />\n";
		project << "\t</ItemGroup>\n";
	}

	// Visual Studio 2015 and up automatically import natvis files that are part of the project
	if (name == PROJECT_NAME && _version >= 14) {
		project << "\t<ItemGroup>\n";
		project << "\t\t<None Include=\"" << setup.srcDir << "/devtools/create_project/scripts/novelvm.natvis\" />\n";
		project << "\t</ItemGroup>\n";
	}

	project << "\t<Import Project=\"$(VCTargetsPath)\\Microsoft.Cpp.targets\" />\n"
	           "\t<ImportGroup Label=\"ExtensionTargets\">\n"
	           "\t</ImportGroup>\n";

	if (setup.tests) {
		// We override the normal target to ignore the exit code (this allows us to have a clean output and not message about the command exit code)
		project << "\t\t<Target Name=\"PostBuildEvent\">\n"
		        << "\t\t\t<Message Text=\"Description: Run tests\" />\n"
		        << "\t\t\t<Exec Command=\"$(TargetPath)\"  IgnoreExitCode=\"true\" />\n"
		        << "\t\t</Target>\n";
	}

	project << "</Project>\n";

	// Output filter file if necessary
	createFiltersFile(setup, name);
}

void MSBuildProvider::createFiltersFile(const BuildSetup &setup, const std::string &name) {
	// No filters => no need to create a filter file
	if (_filters.empty())
		return;

	// Sort all list alphabetically
	_filters.sort();
	_compileFiles.sort();
	_includeFiles.sort();
	_otherFiles.sort();
	_resourceFiles.sort();
	_asmFiles.sort();

	const std::string filtersFile = setup.outputDir + '/' + name + getProjectExtension() + ".filters";
	std::ofstream filters(filtersFile.c_str());
	if (!filters)
		error("Could not open \"" + filtersFile + "\" for writing");

	filters << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
	           "<Project ToolsVersion=\"" << _msvcVersion.project << "\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">\n";

	// Output the list of filters
	filters << "\t<ItemGroup>\n";
	for (std::list<std::string>::iterator filter = _filters.begin(); filter != _filters.end(); ++filter) {
		filters << "\t\t<Filter Include=\"" << *filter << "\">\n"
		           "\t\t\t<UniqueIdentifier>" << createUUID() << "</UniqueIdentifier>\n"
		           "\t\t</Filter>\n";
	}
	filters << "\t</ItemGroup>\n";

	// Output files
	outputFilter(filters, _compileFiles, "ClCompile");
	outputFilter(filters, _includeFiles, "ClInclude");
	outputFilter(filters, _otherFiles, "None");
	outputFilter(filters, _resourceFiles, "ResourceCompile");
	outputFilter(filters, _asmFiles, "CustomBuild");

	filters << "</Project>";
}

void MSBuildProvider::outputFilter(std::ostream &filters, const FileEntries &files, const std::string &action) {
	if (!files.empty()) {
		filters << "\t<ItemGroup>\n";
		for (FileEntries::const_iterator entry = files.begin(), end = files.end(); entry != end; ++entry) {
			if ((*entry).filter != "") {
				filters << "\t\t<" << action << " Include=\"" << (*entry).path << "\">\n"
				           "\t\t\t<Filter>" << (*entry).filter << "</Filter>\n"
				           "\t\t</" << action << ">\n";
			} else {
				filters << "\t\t<" << action << " Include=\"" << (*entry).path << "\" />\n";
			}
		}
		filters << "\t</ItemGroup>\n";
	}
}

void MSBuildProvider::writeReferences(const BuildSetup &setup, std::ofstream &output) {
	output << "\t<ItemGroup>\n";

	for (UUIDMap::const_iterator i = _uuidMap.begin(); i != _uuidMap.end(); ++i) {
		if (i->first == setup.projectName)
			continue;

		output << "\t<ProjectReference Include=\"" << i->first << ".vcxproj\">\n"
		          "\t\t<Project>{" << i->second << "}</Project>\n"
		          "\t</ProjectReference>\n";
	}

	output << "\t</ItemGroup>\n";
}

void MSBuildProvider::outputProjectSettings(std::ofstream &project, const std::string &name, const BuildSetup &setup, bool isRelease, bool isWin32, std::string configuration) {
	// Check for project-specific warnings:
	std::map<std::string, StringList>::iterator warningsIterator = _projectWarnings.find(name);
	bool enableLanguageExtensions = true; // NovelVM
	bool disableEditAndContinue = find(_disableEditAndContinue.begin(), _disableEditAndContinue.end(), name) != _disableEditAndContinue.end();

	// Nothing to add here, move along!
	if ((!setup.devTools || !setup.tests) && name != setup.projectName && !enableLanguageExtensions && !disableEditAndContinue && warningsIterator == _projectWarnings.end())
		return;

	std::string warnings = "";
	if (warningsIterator != _projectWarnings.end())
		for (StringList::const_iterator i = warningsIterator->second.begin(); i != warningsIterator->second.end(); ++i)
			warnings +=  *i + ';';

	project << "\t<ItemDefinitionGroup Condition=\"'$(Configuration)|$(Platform)'=='" << configuration << "|" << (isWin32 ? "Win32" : "x64") << "'\">\n"
	           "\t\t<ClCompile>\n";

	// Language Extensions
	if (setup.devTools || setup.tests || name == setup.projectName || enableLanguageExtensions) {
		project << "\t\t\t<DisableLanguageExtensions>false</DisableLanguageExtensions>\n";
		project << "\t\t\t<ConformanceMode>false</ConformanceMode>\n"; // Required for Windows SDK 8.1
	}

	// Edit and Continue
	if ((name == setup.projectName || disableEditAndContinue) && !isRelease)
		project << "\t\t\t<DebugInformationFormat>ProgramDatabase</DebugInformationFormat>\n";

	// Warnings
	if (warningsIterator != _projectWarnings.end())
		project << "\t\t\t<DisableSpecificWarnings>" << warnings << ";%(DisableSpecificWarnings)</DisableSpecificWarnings>\n";

	project << "\t\t</ClCompile>\n";

	// Link configuration for main project
	if (name == setup.projectName || setup.devTools || setup.tests) {
		std::string libraries;

		for (StringList::const_iterator i = setup.libraries.begin(); i != setup.libraries.end(); ++i)
			libraries += *i + ".lib;";

		project << "\t\t<Link>\n"
		           "\t\t\t<OutputFile>$(OutDir)" << ((setup.devTools || setup.tests) ? name : setup.projectName) << ".exe</OutputFile>\n"
		           "\t\t\t<AdditionalDependencies>" << libraries << "%(AdditionalDependencies)</AdditionalDependencies>\n"
		           "\t\t</Link>\n";

		if (!setup.devTools && !setup.tests && setup.runBuildEvents) {
			project << "\t\t<PreBuildEvent>\n"
			           "\t\t\t<Message>Generate revision</Message>\n"
			           "\t\t\t<Command>" << getPreBuildEvent() << "</Command>\n"
			           "\t\t</PreBuildEvent>\n";

			// Copy data files to the build folder
			project << "\t\t<PostBuildEvent>\n"
					   "\t\t\t<Message>Copy data files to the build folder</Message>\n"
					   "\t\t\t<Command>" << getPostBuildEvent(isWin32, setup) << "</Command>\n"
					   "\t\t</PostBuildEvent>\n";
		} else if (setup.tests) {
			project << "\t\t<PreBuildEvent>\n"
			           "\t\t\t<Message>Generate runner.cpp</Message>\n"
			           "\t\t\t<Command>" << getTestPreBuildEvent(setup) << "</Command>\n"
			           "\t\t</PreBuildEvent>\n";
		}
	}

	project << "\t</ItemDefinitionGroup>\n";
}

void MSBuildProvider::outputGlobalPropFile(const BuildSetup &setup, std::ofstream &properties, int bits, const StringList &defines, const std::string &prefix, bool runBuildEvents) {

	std::string warnings;
	for (StringList::const_iterator i = _globalWarnings.begin(); i != _globalWarnings.end(); ++i)
		warnings +=  *i + ';';

	std::string definesList;
	for (StringList::const_iterator i = defines.begin(); i != defines.end(); ++i)
		definesList += *i + ';';

	// Add define to include revision header
	if (runBuildEvents)
		definesList += REVISION_DEFINE ";";

	properties << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
	              "<Project DefaultTargets=\"Build\" ToolsVersion=\"" << _msvcVersion.project << "\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">\n"
	              "\t<PropertyGroup>\n"
	              "\t\t<_PropertySheetDisplayName>" << setup.projectDescription << "_Global</_PropertySheetDisplayName>\n"
	              "\t\t<ExecutablePath>$(" << LIBS_DEFINE << ")\\bin;$(" << LIBS_DEFINE << ")\\bin\\" << (bits == 32 ? "x86" : "x64") << ";$(ExecutablePath)</ExecutablePath>\n"
	              "\t\t<LibraryPath>$(" << LIBS_DEFINE << ")\\lib\\" << (bits == 32 ? "x86" : "x64") << ";$(" << LIBS_DEFINE << ")\\lib\\" << (bits == 32 ? "x86" : "x64") << "\\$(Configuration);$(LibraryPath)</LibraryPath>\n"
	              "\t\t<IncludePath>$(" << LIBS_DEFINE << ")\\include;$(" << LIBS_DEFINE << ")\\include\\" << (setup.useSDL2 ? "SDL2" : "SDL") << ";$(IncludePath)</IncludePath>\n"
	              "\t\t<OutDir>$(Configuration)" << bits << "\\</OutDir>\n"
	              "\t\t<IntDir>$(Configuration)" << bits << "\\$(ProjectName)\\</IntDir>\n"
	              "\t</PropertyGroup>\n"
	              "\t<ItemDefinitionGroup>\n"
	              "\t\t<ClCompile>\n"
	              "\t\t\t<DisableLanguageExtensions>true</DisableLanguageExtensions>\n"
	              "\t\t\t<DisableSpecificWarnings>" << warnings << ";%(DisableSpecificWarnings)</DisableSpecificWarnings>\n"
	              "\t\t\t<AdditionalIncludeDirectories>.;" << prefix << ";" << prefix << "\\engines;" << (setup.tests ? prefix + "\\test\\cxxtest;" : "") << "%(AdditionalIncludeDirectories)</AdditionalIncludeDirectories>\n"
	              "\t\t\t<PreprocessorDefinitions>" << definesList << "%(PreprocessorDefinitions)</PreprocessorDefinitions>\n"
	              "\t\t\t<ExceptionHandling>" << ((setup.devTools || setup.tests) ? "Sync" : "") << "</ExceptionHandling>\n";

#if NEEDS_RTTI
	properties << "\t\t\t<RuntimeTypeInfo>true</RuntimeTypeInfo>\n";
#else
	properties << "\t\t\t<RuntimeTypeInfo>false</RuntimeTypeInfo>\n";
#endif

	properties << "\t\t\t<WarningLevel>Level4</WarningLevel>\n"
	              "\t\t\t<TreatWarningAsError>false</TreatWarningAsError>\n"
	              "\t\t\t<CompileAs>Default</CompileAs>\n"
	              "\t\t\t<MultiProcessorCompilation>true</MultiProcessorCompilation>\n"
	              "\t\t\t<ConformanceMode>true</ConformanceMode>\n"
	              "\t\t\t<AdditionalOptions>/utf-8 %(AdditionalOptions)</AdditionalOptions>\n"
	              "\t\t</ClCompile>\n"
	              "\t\t<Link>\n"
	              "\t\t\t<IgnoreSpecificDefaultLibraries>%(IgnoreSpecificDefaultLibraries)</IgnoreSpecificDefaultLibraries>\n"
	              "\t\t\t<SubSystem>Console</SubSystem>\n";

	if (!setup.devTools && !setup.tests)
		properties << "\t\t\t<EntryPointSymbol>WinMainCRTStartup</EntryPointSymbol>\n";

	properties << "\t\t</Link>\n"
	              "\t\t<ResourceCompile>\n"
	              "\t\t\t<AdditionalIncludeDirectories>.;" << prefix << ";%(AdditionalIncludeDirectories)</AdditionalIncludeDirectories>\n"
	              "\t\t\t<PreprocessorDefinitions>" << definesList << "%(PreprocessorDefinitions)</PreprocessorDefinitions>\n"
	              "\t\t</ResourceCompile>\n"
	              "\t</ItemDefinitionGroup>\n"
	              "</Project>\n";

	properties.flush();
}

void MSBuildProvider::createBuildProp(const BuildSetup &setup, bool isRelease, bool isWin32, std::string configuration) {
	const std::string outputBitness = (isWin32 ? "32" : "64");

	std::ofstream properties((setup.outputDir + '/' + setup.projectDescription + "_" + configuration + (isWin32 ? "" : "64") + getPropertiesExtension()).c_str());
	if (!properties)
		error("Could not open \"" + setup.outputDir + '/' + setup.projectDescription + "_" + configuration + (isWin32 ? "" : "64") + getPropertiesExtension() + "\" for writing");

	properties << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
	              "<Project DefaultTargets=\"Build\" ToolsVersion=\"" << _msvcVersion.project << "\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">\n"
	              "\t<ImportGroup Label=\"PropertySheets\">\n"
	              "\t\t<Import Project=\"" << setup.projectDescription << "_Global" << (isWin32 ? "" : "64") << ".props\" />\n"
	              "\t</ImportGroup>\n"
	              "\t<PropertyGroup>\n"
	              "\t\t<_PropertySheetDisplayName>" << setup.projectDescription << "_" << configuration << outputBitness << "</_PropertySheetDisplayName>\n"
	              "\t\t<LinkIncremental>" << (isRelease ? "false" : "true") << "</LinkIncremental>\n"
	              "\t\t<GenerateManifest>false</GenerateManifest>\n"
	              "\t</PropertyGroup>\n"
	              "\t<ItemDefinitionGroup>\n"
	              "\t\t<ClCompile>\n";

	if (isRelease) {
		properties << "\t\t\t<IntrinsicFunctions>true</IntrinsicFunctions>\n"
		              "\t\t\t<WholeProgramOptimization>true</WholeProgramOptimization>\n"
		              "\t\t\t<PreprocessorDefinitions>WIN32;RELEASE_BUILD;%(PreprocessorDefinitions)</PreprocessorDefinitions>\n"
		              "\t\t\t<StringPooling>true</StringPooling>\n"
		              "\t\t\t<BufferSecurityCheck>false</BufferSecurityCheck>\n"
		              "\t\t\t<DebugInformationFormat></DebugInformationFormat>\n"
		              "\t\t\t<RuntimeLibrary>MultiThreadedDLL</RuntimeLibrary>\n"
		              "\t\t\t<EnablePREfast>" << (configuration == "Analysis" ? "true" : "false") << "</EnablePREfast>\n"
		              "\t\t</ClCompile>\n"
		              "\t\t<Lib>\n"
		              "\t\t\t<LinkTimeCodeGeneration>true</LinkTimeCodeGeneration>\n"
		              "\t\t</Lib>\n"
		              "\t\t<Link>\n"
		              "\t\t\t<LinkTimeCodeGeneration>UseLinkTimeCodeGeneration</LinkTimeCodeGeneration>\n"
		              "\t\t\t<IgnoreSpecificDefaultLibraries>%(IgnoreSpecificDefaultLibraries)</IgnoreSpecificDefaultLibraries>\n"
		              "\t\t\t<SetChecksum>true</SetChecksum>\n";
	} else {
		properties << "\t\t\t<Optimization>Disabled</Optimization>\n"
		              "\t\t\t<PreprocessorDefinitions>WIN32;" << (configuration == "LLVM" ? "_CRT_SECURE_NO_WARNINGS;" : "") << "%(PreprocessorDefinitions)</PreprocessorDefinitions>\n"
		              "\t\t\t<BasicRuntimeChecks>EnableFastChecks</BasicRuntimeChecks>\n"
		              "\t\t\t<RuntimeLibrary>MultiThreadedDebugDLL</RuntimeLibrary>\n"
		              "\t\t\t<FunctionLevelLinking>true</FunctionLevelLinking>\n"
		              "\t\t\t<TreatWarningAsError>false</TreatWarningAsError>\n";
		if (_version >= 14) {
			// Since MSVC 2015 Edit and Continue is support for x64 too.
			properties << "\t\t\t<DebugInformationFormat>" << "EditAndContinue" << "</DebugInformationFormat>\n";
		} else {
			// Older MSVC versions did not support Edit and Continue for x64, thus we do not use it.
			properties << "\t\t\t<DebugInformationFormat>" << (isWin32 ? "EditAndContinue" : "ProgramDatabase") << "</DebugInformationFormat>\n";
		}
		properties << "\t\t\t<EnablePREfast>" << (configuration == "Analysis" ? "true" : "false") << "</EnablePREfast>\n";

		if (configuration == "LLVM") {
			// FIXME The LLVM cl wrapper does not seem to work properly with the $(TargetDir) path so we hard-code the build folder until the issue is resolved
			properties << "\t\t\t<AdditionalIncludeDirectories>" << configuration << outputBitness <<";%(AdditionalIncludeDirectories)</AdditionalIncludeDirectories>\n"
		                  "\t\t\t<AdditionalOptions>-Wno-microsoft -Wno-long-long -Wno-multichar -Wno-unknown-pragmas -Wno-reorder -Wpointer-arith -Wcast-qual -Wshadow -Wnon-virtual-dtor -Wwrite-strings -Wno-conversion -Wno-shorten-64-to-32 -Wno-sign-compare -Wno-four-char-constants -Wno-nested-anon-types -Qunused-arguments %(AdditionalOptions)</AdditionalOptions>\n";
		}

		properties << "\t\t</ClCompile>\n"
		              "\t\t<Link>\n"
		              "\t\t\t<GenerateDebugInformation>true</GenerateDebugInformation>\n"
		              "\t\t\t<ImageHasSafeExceptionHandlers>false</ImageHasSafeExceptionHandlers>\n";
	}

	properties << "\t\t</Link>\n"
	              "\t</ItemDefinitionGroup>\n"
	              "</Project>\n";

	properties.flush();
	properties.close();
}

bool hasEnding(std::string const &fullString, std::string const &ending) {
	if (fullString.length() > ending.length()) {
		return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
	} else {
		return false;
	}
}

namespace {

inline void outputNasmCommand(std::ostream &projectFile, const std::string &config, const std::string &prefix) {
	projectFile << "\t\t\t<Command Condition=\"'$(Configuration)|$(Platform)'=='" << config << "|Win32'\">nasm.exe -f win32 -g -o \"$(IntDir)" << prefix << "%(Filename).obj\" \"%(FullPath)\"</Command>\n"
	               "\t\t\t<Outputs Condition=\"'$(Configuration)|$(Platform)'=='" << config << "|Win32'\">$(IntDir)" << prefix << "%(Filename).obj;%(Outputs)</Outputs>\n";
}

} // End of anonymous namespace

void MSBuildProvider::writeFileListToProject(const FileNode &dir, std::ofstream &projectFile, const int, const StringList &duplicate,
                                             const std::string &objPrefix, const std::string &filePrefix) {
	// Reset lists
	_filters.clear();
	_compileFiles.clear();
	_includeFiles.clear();
	_otherFiles.clear();
	_resourceFiles.clear();
	_asmFiles.clear();

	// Compute the list of files
	_filters.push_back(""); // init filters
	computeFileList(dir, duplicate, objPrefix, filePrefix);
	_filters.pop_back();    // remove last empty filter

	// Output compile files
	if (!_compileFiles.empty()) {
		projectFile << "\t<ItemGroup>\n";
		for (std::list<FileEntry>::const_iterator entry = _compileFiles.begin(); entry != _compileFiles.end(); ++entry) {
			std::string fileName = (*entry).name + ".o";
			std::transform(fileName.begin(), fileName.end(), fileName.begin(), tolower);
			const bool isDuplicate = (std::find(duplicate.begin(), duplicate.end(), fileName) != duplicate.end());

			// Deal with duplicated file names
			if (isDuplicate) {
				projectFile << "\t\t<ClCompile Include=\"" << (*entry).path << "\">\n"
							   "\t\t\t<ObjectFileName>$(IntDir)" << (*entry).prefix << "%(Filename).obj</ObjectFileName>\n";

				projectFile << "\t\t</ClCompile>\n";
			} else {
				projectFile << "\t\t<ClCompile Include=\"" << (*entry).path << "\" />\n";
			}
		}
		projectFile << "\t</ItemGroup>\n";
	}

	// Output include, other and resource files
	outputFiles(projectFile, _includeFiles, "ClInclude");
	outputFiles(projectFile, _otherFiles, "None");
	outputFiles(projectFile, _resourceFiles, "ResourceCompile");

	// Output asm files
	if (!_asmFiles.empty()) {
		projectFile << "\t<ItemGroup>\n";
		for (std::list<FileEntry>::const_iterator entry = _asmFiles.begin(); entry != _asmFiles.end(); ++entry) {

			const bool isDuplicate = (std::find(duplicate.begin(), duplicate.end(), (*entry).name + ".o") != duplicate.end());

			projectFile << "\t\t<CustomBuild Include=\"" << (*entry).path << "\">\n"
			               "\t\t\t<FileType>Document</FileType>\n";

			outputNasmCommand(projectFile, "Debug", (isDuplicate ? (*entry).prefix : ""));
			outputNasmCommand(projectFile, "Analysis", (isDuplicate ? (*entry).prefix : ""));
			outputNasmCommand(projectFile, "Release", (isDuplicate ? (*entry).prefix : ""));
			outputNasmCommand(projectFile, "LLVM", (isDuplicate ? (*entry).prefix : ""));

			projectFile << "\t\t</CustomBuild>\n";
		}
		projectFile << "\t</ItemGroup>\n";
	}
}

void MSBuildProvider::outputFiles(std::ostream &projectFile, const FileEntries &files, const std::string &action) {
	if (!files.empty()) {
		projectFile << "\t<ItemGroup>\n";
		for (FileEntries::const_iterator entry = files.begin(), end = files.end(); entry != end; ++entry) {
			projectFile << "\t\t<" << action << " Include=\"" << (*entry).path << "\" />\n";
		}
		projectFile << "\t</ItemGroup>\n";
	}
}

void MSBuildProvider::computeFileList(const FileNode &dir, const StringList &duplicate, const std::string &objPrefix, const std::string &filePrefix) {
	for (FileNode::NodeList::const_iterator i = dir.children.begin(); i != dir.children.end(); ++i) {
		const FileNode *node = *i;

		if (!node->children.empty()) {
			// Update filter
			std::string _currentFilter = _filters.back();
			_filters.back().append((_filters.back() == "" ? "" : "\\") + node->name);

			computeFileList(*node, duplicate, objPrefix + node->name + '_', filePrefix + node->name + '/');

			// Reset filter
			_filters.push_back(_currentFilter);
		} else {
			// Filter files by extension
			std::string name, ext;
			splitFilename(node->name, name, ext);

			FileEntry entry;
			entry.name = name;
			entry.path = convertPathToWin(filePrefix + node->name);
			entry.filter = _filters.back();
			entry.prefix = objPrefix;

			if (ext == "cpp" || ext == "c")
				_compileFiles.push_back(entry);
			else if (ext == "h")
				_includeFiles.push_back(entry);
			else if (ext == "rc")
				_resourceFiles.push_back(entry);
			else if (ext == "asm")
				_asmFiles.push_back(entry);
			else
				_otherFiles.push_back(entry);
		}
	}
}

} // End of CreateProjectTool namespace
