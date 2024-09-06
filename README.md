# file-utils-static static library

## Search

### std::vector<std::filesystem::path> dir(const std::string& pattern)
Return the list of paths matching the given pattern<br>
Wildcards * and ? are supported in the filename part of the path

## Information

### enum class EOL
Sets enums:
* Unknown
* Windows
* Unix
* Mac

### EOL file_EOL(const std::filesystem::path& filepath)
Detects the EOL string used by the given file<br>
Search is restricted to first and last 4K bytes, if no EOL sequence is found then Unknown value is returned

### size_t EOL_length(const EOL eol_type)
Returns the length of the EOL sequence

### std::string EOL_str(const EOL eol_type)
Returns the chars sequence of the given EOL type

## File manipulation

### std::string concatenateFiles(const std::vector<std::filesystem::path>& inputFiles, const std::filesystem::path& outputFile)
Concatenates inputFiles to outputFile
