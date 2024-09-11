#include <filesystem>
#include <iostream>
#include <gtest/gtest.h>
#include <file-utils-static.hpp>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

namespace fs = std::filesystem;
namespace file = file_utils;

static fs::path getExecutablePath()
{
	char path[1024];

#ifdef _WIN32
	GetModuleFileName(NULL, path, sizeof(path));
#else
	ssize_t count = readlink("/proc/self/exe", path, sizeof(path) - 1);
	if (count != -1) {
		path[count] = '\0';
	}
	else {
		throw std::runtime_error("Error reading executable path.");
	}
#endif

	return fs::path(path);
}

static fs::path getParentPath(const fs::path& path, const std::string& target)
{
	auto current = path;
	while (current.has_parent_path()) {
		if (current.filename() == target) {
			return current.parent_path();
		}
		current = current.parent_path();
	}
	throw std::runtime_error("Target directory not found in path.");
}

class PathTestFixture : public ::testing::Test
{
protected:
	fs::path testPath;

	void SetUp() override
	{
		try {
			//fs::path exePath = getExecutablePath();
			//fs::path parentPath = getParentPath(exePath, "out");
			//testPath = parentPath / "tests";
			testPath = fs::path(SOURCE_DIR) / "tests";

			// check path
			std::cout << "File path: " << testPath << std::endl;
		}
		catch (const std::exception& e) {
			std::cerr << e.what() << std::endl;
			throw;
		}
	}
};

TEST_F(PathTestFixture, Current_Path_Dir)
{
	fs::current_path(testPath);
	std::string testp{ "msxml?.*" };
	auto result{ file::dir(testp) };
	EXPECT_EQ(result.size(), 2);
	std::sort(result.begin(), result.end());
	if (result.size() == 2)
	{
		EXPECT_STREQ(result[0].filename().string().c_str(), "msxml3.dll");
		EXPECT_STREQ(result[1].filename().string().c_str(), "msxml6.dll");
	}
}

TEST_F(PathTestFixture, Absolute_Path_Dir)
{
	std::string testp = testPath.string() + "/msxml?.*";
	auto result{ file::dir(testp) };
	EXPECT_EQ(result.size(), 2);
	std::sort(result.begin(), result.end());
	if (result.size() == 2)
	{
		EXPECT_STREQ(result[0].filename().string().c_str(), "msxml3.dll");
		EXPECT_STREQ(result[1].filename().string().c_str(), "msxml6.dll");
	}
}

// TODO dir with patterns containing []

TEST_F(PathTestFixture, Windows_File)
{
	std::filesystem::path p = testPath / "windows.txt";
	EXPECT_EQ(file::file_EOL(p), file::EOL::Windows);
}

TEST_F(PathTestFixture, Unix_File)
{
	std::filesystem::path p = testPath / "unix.txt";
	EXPECT_EQ(file::file_EOL(p), file::EOL::Unix);
}
