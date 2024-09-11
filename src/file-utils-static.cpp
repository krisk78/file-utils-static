// file_utils.cpp : Defines the functions for the static library.
//

#include <regex>
#include <iostream>
#include <fstream>
#include <cstdio>

#include <file-utils-static.hpp>

namespace fs = std::filesystem;

static void replace_all(std::string& source, char replace, const std::string& replacement)
{
    size_t pos{ 0 };
    while (pos < source.length() && (pos = source.find(replace, pos)) != std::string::npos)
    {
        source.replace(pos, 1, replacement);
        pos += replacement.length();
    }
}

static std::string to_regex_expression(const std::string& path)
{
    std::string expression{ "^" + path };
    replace_all(expression, '.', "\\.");
    replace_all(expression, '*', ".*");
    replace_all(expression, '?', "(.{1,1})");
    // TODO replacement of [..]
    return expression;
}

std::vector<fs::path> file_utils::dir(const std::string& pattern)
{
#ifdef _WIN32
    const std::string wildcards{ "*?" };
#else
    const std::string wildcards{ "*?[" };
#endif // _WIN32

    std::vector<fs::path> result{};
    if (pattern.find_first_of(wildcards) == std::string::npos)
    {
        if (fs::exists(fs::path(pattern)))
            result.push_back(pattern);
        return result;
    }
    std::string directory{ "." };
    std::string filename{ pattern };
    while (!filename.empty() && (filename.back() == '\\' || filename.back() == '/'))
        filename.pop_back();        // ending path separators are ignore
    auto itr = filename.find_last_of("/\\");
    if (itr != std::string::npos)
    {
        directory = filename.substr(0, itr);
        filename = filename.substr(itr + 1, filename.size());
    }
    if (directory.find_first_of(wildcards) != std::string::npos || !fs::exists(fs::path(directory)))
        return result;          // wildcards are not allowed in parent path
    std::regex expr(to_regex_expression(filename), std::regex_constants::icase);
    for (auto it = fs::directory_iterator(directory); it != fs::directory_iterator(); ++it)
    {
        if (std::regex_match(it->path().filename().string(), expr))
            result.push_back(it->path());
    }
    return result;
}

static file_utils::EOL find_EOL(const std::vector<char>& buf)
{
    auto itr = std::find(buf.rbegin(), buf.rend(), '\r');
    if (itr != buf.rend())
    {
        if (itr != buf.rbegin())
        {
            itr--;
            if (*itr == '\n')
                return file_utils::EOL::Windows;
        }
        return file_utils::EOL::Mac;
    }
    itr = std::find(buf.rbegin(), buf.rend(), '\n');
    if (itr != buf.rend())
        return file_utils::EOL::Unix;
    return file_utils::EOL::Unknown;
}

file_utils::EOL file_utils::file_EOL(const std::filesystem::path& filepath)
{
    const std::ios::off_type BUF_LENGTH{ 4096 };

    if (!std::filesystem::exists(filepath))
        return EOL::Unknown;
    std::ifstream file(filepath, std::ios_base::binary | std::ios_base::in);
    if (auto fop = file.is_open(); !fop)
        return EOL::Unknown;
    file.seekg(0, std::ios::end);
    std::vector<char> buf(BUF_LENGTH);
    auto pos = file.tellg();
    if (pos > BUF_LENGTH)
    {
        pos = BUF_LENGTH;
        file.seekg(-pos, std::ios::end);         // first search at end of file
        file.read(buf.data(), pos);
        auto ret = find_EOL(buf);
        if (ret != EOL::Unknown)
        {
            file.close();
            return ret;
        }
    }
    file.seekg(std::ios::beg);                      // next search at begin of file
    file.read(buf.data(), pos);
    auto ret = find_EOL(buf);
    file.close();
    return ret;
}

size_t file_utils::EOL_length(const file_utils::EOL eol_type)
{
    switch (eol_type)
    {
    case EOL::Unknown:
        return 0;
    case EOL::Windows:
        return 2;
    default:
        return 1;
    }
}

std::string file_utils::EOL_str(const file_utils::EOL eol_type)
{
    switch (eol_type)
    {
    case EOL::Windows:
        return "\r\n";
    case EOL::Unix:
        return "\n";
    case EOL::Mac:
        return "\r";
    default:
        return "";
    }
}

std::string file_utils::concatenateFiles(const std::vector<std::filesystem::path>& inputFiles, const std::filesystem::path& outputFile)
{
    std::string last_error = "";
    std::ofstream out(outputFile, std::ios::binary);
    if (!out)
    {
        return "Error opening output file " + outputFile.string() + ".";
    }
    for (const auto& file : inputFiles)
    {
        std::ifstream in(file, std::ios::binary);
        if (!in)
        {
            last_error = "Error opening input file " + file.string() + ".";
            continue;
        }
        out << in.rdbuf();
        in.close();
    }
    out.close();
    return last_error;
}
