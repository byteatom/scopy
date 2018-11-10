#include <iostream>
#include <filesystem>
namespace fs = std::filesystem;
#include <regex>

void usage()
{
	std::cout << R"(
Copy files or directories filtered with multi ordered include or exclude regular expression

usage: scopy source [destination] [options]...

    source              source file or directory
    destination         destination file or directory

options:

    -e <regex>          exclude filter
    -i <regex>          include filter
    -t                  tolerant, continue when error ocurrs
    -u                  update, overwrite only if file modification time or size is different
    -v                  verbose

note: If source is a file and destination dosn't exist, if destination end with '\' or '/'
    means it's a directory, otherwise it's a file.

example: scopy dir1 dir2 -e regex1 -i regex2 -i regex3 -v -e regex4...
	)";
}

std::error_code err;
bool update = false;
bool verbose = false;
std::list<std::pair<bool, std::regex>> filters;

bool copyFile(const fs::path& src, const fs::path& dst) {
	std::string srcStr = src.string();

	for (auto& filter : filters) {
		if (filter.first && !std::regex_match(srcStr, filter.second) ||
			!filter.first && std::regex_match(srcStr, filter.second))
			return false;
	}

	if (update
		&& fs::last_write_time(src, err) == fs::last_write_time(dst, err)
		&& fs::file_size(src, err) == fs::file_size(dst, err))
		return true;

	fs::path dstDir = dst.parent_path();
	if (!dstDir.empty() && !fs::exists(dstDir, err) && !fs::create_directories(dstDir, err)) {
		std::cout << "scopy: falied to create directory " << dstDir;
		if (verbose) std::cout << " because " << err.message();
		std::cout << std::endl;
		return false;
	}

	bool result = fs::copy_file(src, dst, fs::copy_options::overwrite_existing, err);
	if (!result) {
		std::cout << "scopy: failed to copy " << src << " to " << dst;
		if (verbose) std::cout << " because " << err.message();
		std::cout << std::endl;
	}
	else if (verbose)
		std::cout << srcStr << " -> " << dst.string() << std::endl;

	return result;
}

int main(int argc, char* argv[])
{
	char* src = nullptr;
	char* dst = nullptr;
	bool tolerant = false;

	for (int i = 1; i < argc; ++i) {
		if (argv[i][0] == '-') {
			switch (argv[i][1]) {
			case 'h':
				usage();
				return EXIT_SUCCESS;
			case 't':
				tolerant = true;
				break;
			case 'u':
				update = true;
				break;
			case 'v':
				verbose = true;
				break;
			case 'i':
				if (i == argc - 1) continue;
				++i;
				filters.emplace_back(true, argv[i]);
				break;
			case 'e':
				if (i == argc - 1) continue;
				++i;
				filters.emplace_back(false, argv[i]);
				break;
			default:
				break;
			}
		}
		else {
			if (!src) {
				src = argv[i];
				continue;
			}
			else if (!dst) {
				dst = argv[i];
			}
		}
	}

	if (!src) {
		usage();
		return EXIT_FAILURE;
	}

	fs::path srcPath{ src };

	if (!fs::exists(srcPath, err)) {
		std::cout << "scopy: source doesn't exist" << std::endl;
		return EXIT_FAILURE;
	}

	bool srcIsFile = fs::is_regular_file(srcPath, err);
	bool srcIsDir = fs::is_directory(srcPath, err);

	if (!srcIsFile && !srcIsDir) {
		std::cout << "scopy: source should be a file, directory" << std::endl;
		return EXIT_FAILURE;
	}

	if (!dst)
		dst = ".";

	fs::path dstPath{ dst };

	bool dstExist = fs::exists(dstPath, err);
	bool dstIsDir = true;

	if (dstExist)
		dstIsDir = fs::is_directory(dstPath, err);
	else if (srcIsFile) {
		char tail = dst[strlen(dst) - 1];
		dstIsDir = (tail == '/' || tail == '\\');
	}

	if (srcIsDir && !dstIsDir) {
		std::cout << "scopy: destionation should be a directory since source is a directory" << std::endl;
		return EXIT_FAILURE;
	}

	bool result = EXIT_SUCCESS;

	if (srcIsFile)
		result = copyFile(srcPath, dstIsDir ? dstPath / srcPath.filename() : dstPath);
	else {
		for (auto& entry : fs::recursive_directory_iterator(srcPath)) {
			if (!fs::is_regular_file(entry, err)) continue;
			fs::path srcRelative = fs::relative(entry, srcPath, err);
			result = copyFile(entry, dstPath / srcRelative);
			if (!result && !tolerant)
				break;
		}
	}

	return result ? EXIT_SUCCESS : EXIT_FAILURE;
}

