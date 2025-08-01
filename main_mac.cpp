#include "main_mac.h"

struct Error {
	const std::string message;
	const std::string filePath;
	const std::string function;
	const std::string source;
	const std::string line;
};

static bool isCommandLine = true;
static bool isProgressBarActive = false;
static uint32_t filesSkipped = 0;

static struct {
	bool showHelp = false;
	bool silentAssertions = false;
	bool forceOverwrite = false;
	bool ignoreDebugInfo = false;
	bool minimizeDiffs = false;
	bool unrestrictedAscii = false;
	bool beautifyCode = false;
	std::string inputPath;
	std::string outputPath;
	std::string extensionFilter;
} arguments;

struct Directory {
	const std::string path;
	std::vector<Directory> folders;
	std::vector<std::string> files;
};

static std::string string_to_lowercase(const std::string& string) {
	std::string lowercaseString = string;

	for (uint32_t i = lowercaseString.size(); i--;) {
		if (lowercaseString[i] < 'A' || lowercaseString[i] > 'Z') continue;
		lowercaseString[i] += 'a' - 'A';
	}

	return lowercaseString;
}

static void find_files_recursively(Directory& directory) {
	std::string fullPath = arguments.inputPath + directory.path;
	DIR* dir = opendir(fullPath.c_str());
	if (!dir) return;

	struct dirent* entry;
	while ((entry = readdir(dir)) != nullptr) {
		if (entry->d_name[0] == '.') continue; // 跳过隐藏文件

		std::string entryPath = fullPath + "/" + entry->d_name;
		struct stat statbuf;
		if (stat(entryPath.c_str(), &statbuf) != 0) continue;

		if (S_ISDIR(statbuf.st_mode)) {
			// 是目录
			Directory subDir = {directory.path + entry->d_name + "/", {}, {}};
			find_files_recursively(subDir);
			directory.folders.push_back(subDir);
		} else if (S_ISREG(statbuf.st_mode)) {
			// 是文件
			std::string fileName = entry->d_name;
			std::string lowercaseFileName = string_to_lowercase(fileName);
			std::string lowercaseExtensionFilter = string_to_lowercase(arguments.extensionFilter);

			if (arguments.extensionFilter.empty() || 
				(fileName.size() >= arguments.extensionFilter.size() &&
				 lowercaseFileName.substr(fileName.size() - arguments.extensionFilter.size()) == lowercaseExtensionFilter)) {
				directory.files.push_back(directory.path + fileName);
			}
		}
	}

	closedir(dir);
}

static uint32_t count_files_recursively(const Directory& directory) {
	uint32_t count = directory.files.size();
	for (const auto& folder : directory.folders) {
		count += count_files_recursively(folder);
	}
	return count;
}

static uint32_t currentFileIndex = 0;
static uint32_t totalFileCount = 0;

static bool decompile_files_recursively(const Directory& directory) {
	for (uint32_t i = 0; i < directory.files.size(); i++) {
		print_progress_bar(currentFileIndex++, totalFileCount);

		try {
			// 创建字节码对象并处理
			Bytecode bytecode(arguments.inputPath + directory.files[i]);
			bytecode();

			// 创建AST
			Ast ast(bytecode, arguments.ignoreDebugInfo, arguments.minimizeDiffs);
			ast();

			// 生成Lua代码
			std::string outputFilePath = arguments.outputPath + directory.files[i];

			// 确保输出目录存在
			std::filesystem::path outputPath(outputFilePath);
			std::filesystem::create_directories(outputPath.parent_path());

			// 将扩展名改为.lua
			if (outputFilePath.size() >= 5 && outputFilePath.substr(outputFilePath.size() - 5) == ".ljbc") {
				outputFilePath = outputFilePath.substr(0, outputFilePath.size() - 5) + ".lua";
			} else {
				outputFilePath += ".lua";
			}

			Lua lua(bytecode, ast, outputFilePath, arguments.forceOverwrite, arguments.minimizeDiffs, arguments.unrestrictedAscii);
			lua();

		} catch (const Error& error) {
			if (!arguments.silentAssertions) {
				print("断言失败: " + error.message);
				print("文件: " + error.filePath);
				print("函数: " + error.function);
				print("行号: " + error.line);
			}
			filesSkipped++;
		} catch (const std::exception& e) {
			print("标准异常: " + std::string(e.what()));
			print("文件: " + arguments.inputPath + directory.files[i]);
			filesSkipped++;
		} catch (...) {
			print("未知异常");
			print("文件: " + arguments.inputPath + directory.files[i]);
			filesSkipped++;
		}
	}

	for (uint32_t i = 0; i < directory.folders.size(); i++) {
		if (!decompile_files_recursively(directory.folders[i])) return false;
	}

	return true;
}

static char* parse_arguments(const int& argc, char** const& argv) {
	if (argc < 2) return nullptr;
	arguments.inputPath = argv[1];
	
	bool isInputPathSet = true;

	if (arguments.inputPath.size() && arguments.inputPath.front() == '-') {
		arguments.inputPath.clear();
		isInputPathSet = false;
	}

	std::string argument;

	for (uint32_t i = isInputPathSet ? 2 : 1; i < argc; i++) {
		argument = argv[i];

		if (argument.size() >= 2 && argument.front() == '-') {
			if (argument[1] == '-') {
				argument = argument.c_str() + 2;

				if (argument == "extension") {
					if (i <= argc - 2) {
						i++;
						arguments.extensionFilter = argv[i];
						continue;
					}
				} else if (argument == "force_overwrite") {
					arguments.forceOverwrite = true;
					continue;
				} else if (argument == "help") {
					arguments.showHelp = true;
					continue;
				} else if (argument == "ignore_debug_info") {
					arguments.ignoreDebugInfo = true;
					continue;
				} else if (argument == "minimize_diffs") {
					arguments.minimizeDiffs = true;
					continue;
				} else if (argument == "output") {
					if (i <= argc - 2) {
						i++;
						arguments.outputPath = argv[i];
						continue;
					}
				} else if (argument == "silent_assertions") {
					arguments.silentAssertions = true;
					continue;
				} else if (argument == "unrestricted_ascii") {
					arguments.unrestrictedAscii = true;
					continue;
				}
			} else if (argument.size() == 2) {
				switch (argument[1]) {
				case 'e':
					if (i > argc - 2) break;
					i++;
					arguments.extensionFilter = argv[i];
					continue;
				case 'f':
					arguments.forceOverwrite = true;
					continue;
				case '?':
				case 'h':
					arguments.showHelp = true;
					continue;
				case 'i':
					arguments.ignoreDebugInfo = true;
					continue;
				case 'm':
					arguments.minimizeDiffs = true;
					continue;
				case 'o':
					if (i > argc - 2) break;
					i++;
					arguments.outputPath = argv[i];
					continue;
				case 's':
					arguments.silentAssertions = true;
					continue;
				case 'u':
					arguments.unrestrictedAscii = true;
					continue;
				}
			}
		} else if (!isInputPathSet) {
			arguments.inputPath = argument;
			isInputPathSet = true;
			continue;
		}

		return (char*)"无效的参数";
	}

	if (!isInputPathSet && !arguments.showHelp) return (char*)"未指定输入路径";
	return nullptr;
}

void print(const std::string& message) {
	if (isProgressBarActive) {
		erase_progress_bar();
		isProgressBarActive = false;
	}

	std::cout << message << std::endl;
}

void print_progress_bar(const double& progress, const double& total) {
	if (!isCommandLine) return;

	const int barWidth = 50;
	double percentage = (progress / total) * 100.0;
	int filledWidth = static_cast<int>((progress / total) * barWidth);

	std::cout << "\r[";
	for (int i = 0; i < barWidth; ++i) {
		if (i < filledWidth) std::cout << "=";
		else std::cout << " ";
	}
	std::cout << "] " << static_cast<int>(percentage) << "%" << std::flush;

	isProgressBarActive = true;
}

void erase_progress_bar() {
	if (!isCommandLine) return;
	std::cout << "\r" << std::string(60, ' ') << "\r" << std::flush;
}

void assert_mac(const bool& assertion, const std::string& message, const std::string& filePath,
				const std::string& function, const std::string& source, const uint32_t& line) {
	if (assertion) return;

	Error error = {message, filePath, function, source, std::to_string(line)};
	throw error;
}

std::string byte_to_string(const uint8_t& byte) {
	char buffer[4];
	snprintf(buffer, sizeof(buffer), "%02X", byte);
	return std::string(buffer);
}

int main(int argc, char* argv[]) {
	// 检测是否在命令行环境
	isCommandLine = isatty(STDOUT_FILENO);

	// 解析命令行参数
	char* argumentError = parse_arguments(argc, argv);

	if (argumentError) {
		print(std::string("参数错误: ") + argumentError);
		return 1;
	}

	if (arguments.showHelp) {
		print(PROGRAM_NAME);
		print("");
		print("用法: luajit-decompiler-v2-mac [选项] <输入路径>");
		print("");
		print("选项:");
		print("  -?, -h, --help                显示此帮助信息");
		print("  -e, --extension <扩展名>      按扩展名过滤文件 (例如: .ljbc)");
		print("  -f, --force_overwrite         强制覆盖现有文件");
		print("  -i, --ignore_debug_info       忽略调试信息");
		print("  -m, --minimize_diffs          最小化输出差异");
		print("  -o, --output <路径>           输出目录或文件路径");
		print("  -s, --silent_assertions       静默断言失败");
		print("  -u, --unrestricted_ascii      允许不受限制的ASCII字符");
		print("");
		print("示例:");
		print("  luajit-decompiler-v2-mac file.ljbc                    # 输出到当前目录，文件名自动生成");
		print("  luajit-decompiler-v2-mac -o output/ folder/           # 输出到指定目录");
		print("  luajit-decompiler-v2-mac -o myfile.lua file.ljbc     # 输出到指定文件名");
		print("  luajit-decompiler-v2-mac -e .ljbc folder/             # 按扩展名过滤");
		return 0;
	}

	// 验证输入路径
	struct stat statbuf;
	if (stat(arguments.inputPath.c_str(), &statbuf) != 0) {
		print("错误: 无法访问输入路径: " + arguments.inputPath);
		return 1;
	}

	// 检查是否为单个文件
	bool isSingleFile = S_ISREG(statbuf.st_mode);

	// 检查输出路径是否指定了具体文件名
	bool outputIsFile = false;
	std::string outputDir = arguments.outputPath;
	std::string outputFileName = "";

	if (!arguments.outputPath.empty()) {
		std::filesystem::path outputPath(arguments.outputPath);

		// 如果输出路径有扩展名，认为是文件路径
		if (outputPath.has_extension()) {
			outputIsFile = true;
			outputDir = outputPath.parent_path().string();
			outputFileName = outputPath.filename().string();

			// 如果父目录为空，设置为当前目录
			if (outputDir.empty()) {
				outputDir = "./";
			} else if (outputDir.back() != '/') {
				outputDir += '/';
			}
		} else {
			// 否则认为是目录路径
			if (arguments.outputPath.back() != '/') {
				outputDir += '/';
			}
		}
	} else {
		// 设置默认输出路径
		if (isSingleFile) {
			outputDir = "./";
		} else {
			// 为文件夹生成默认输出路径：原文件夹名 + "_decompiled"
			std::filesystem::path inputPath(arguments.inputPath);
			std::string inputDirName = inputPath.filename().string();
			if (inputDirName.empty()) {
				// 如果输入路径以/结尾，获取父目录名
				inputDirName = inputPath.parent_path().filename().string();
			}
			outputDir = inputDirName + "_decompiled/";
		}
	}

	// 只对目录路径确保以/结尾
	if (!isSingleFile && arguments.inputPath.back() != '/') {
		arguments.inputPath += '/';
	}

	// 更新arguments.outputPath为处理后的目录路径
	arguments.outputPath = outputDir;

	// 创建输出目录
	std::filesystem::create_directories(arguments.outputPath);

	print("开始反编译...");
	print("输入: " + arguments.inputPath);
	print("输出: " + arguments.outputPath);

	try {
		if (isSingleFile) {
			// 处理单个文件
			std::filesystem::path inputPath(arguments.inputPath);
			std::string fileName = inputPath.filename().string();

			// 检查文件扩展名
			if (!arguments.extensionFilter.empty()) {
				if (fileName.size() < arguments.extensionFilter.size() ||
					fileName.substr(fileName.size() - arguments.extensionFilter.size()) != arguments.extensionFilter) {
					print("文件不匹配扩展名过滤器: " + arguments.extensionFilter);
					return 0;
				}
			}

			print("处理文件: " + fileName);

			try {
				// 创建字节码对象并处理
				Bytecode bytecode(arguments.inputPath);
				bytecode();

				// 创建AST
				Ast ast(bytecode, arguments.ignoreDebugInfo, arguments.minimizeDiffs);
				ast();

				// 生成Lua代码
				std::string outputFilePath;

				if (outputIsFile && !outputFileName.empty()) {
					// 用户指定了具体的输出文件名
					outputFilePath = arguments.outputPath + outputFileName;
				} else {
					// 使用输入文件名，改变扩展名
					outputFilePath = arguments.outputPath + fileName;

					// 将扩展名改为.lua
					if (outputFilePath.size() >= 5 && outputFilePath.substr(outputFilePath.size() - 5) == ".ljbc") {
						outputFilePath = outputFilePath.substr(0, outputFilePath.size() - 5) + ".lua";
					} else {
						outputFilePath += ".lua";
					}
				}

				Lua lua(bytecode, ast, outputFilePath, arguments.forceOverwrite, arguments.minimizeDiffs, arguments.unrestrictedAscii);
				lua();

				print("反编译完成: " + outputFilePath);
			} catch (const Error& error) {
				if (!arguments.silentAssertions) {
					print("断言失败: " + error.message);
					print("文件: " + error.filePath);
					print("函数: " + error.function);
					print("行号: " + error.line);
				}
				return 1;
			}
		} else {
			// 处理目录
			Directory rootDirectory = {"", {}, {}};
			find_files_recursively(rootDirectory);

			if (rootDirectory.files.empty() && rootDirectory.folders.empty()) {
				print("未找到要处理的文件。");
				return 0;
			}

			// 正确计算总文件数
			totalFileCount = count_files_recursively(rootDirectory);
			currentFileIndex = 0;

			print("找到 " + std::to_string(totalFileCount) + " 个文件要处理。");

			bool success = decompile_files_recursively(rootDirectory);
			erase_progress_bar();

			if (success) {
				print("反编译完成！");
				if (filesSkipped > 0) {
					print("跳过的文件数: " + std::to_string(filesSkipped));
				}
			} else {
				print("反编译过程中出现错误。");
				return 1;
			}
		}

	} catch (const std::exception& e) {
		print("致命错误: " + std::string(e.what()));
		return 1;
	} catch (...) {
		print("发生未知错误。");
		return 1;
	}

	return 0;
}
