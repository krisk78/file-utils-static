#pragma once

/*! \file file-utils-static.hpp
*	\brief Few file functions. See functions documentation for details.
*	\author Christophe COUAILLET
*/

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#endif

#include <filesystem>
#include <vector>

/*! \brief Defines the namespace of the functions */
namespace file_utils
{

	/*! \brief Defines the types of end of line EOL */
	enum class EOL
	{
		Unknown,
		Windows,		// CR + LF
		Unix,			// LF
		Mac				// CR
	};

	/*!	\brief Returns a list of paths matching the specified pattern that can contain *, ? (Windows, POSIX) and [..] (POSIX).
	*	\param pattern	the pattern to use for search files
	*	\return the list of matching files

		Ending path separators are ignored. Wildcards are only allowed in the filename part of the path.
		\warning Wildcard [..] support is not yet implemented.
	*/
	std::vector<std::filesystem::path> dir(const std::string& pattern);

	/*! \brief Detects the EOL char(s) used in a file.
	*	\param filepath the file to check
	*	\return the EOL mode detected (EOL::Unknown if not found)
		\warning The search is done in the 4096 bytes at end or beginning of the file.
	*/
	EOL file_EOL(const std::filesystem::path& filepath);

	/*! \brief Returns the length of the EOL type.
	*	\param eol_type the EOL mode
	*	\return the size, in bytes, of the EOL sequence
	*/
	size_t EOL_length(const EOL eol_type);

	/*! \brief Returns the chars sequence of the EOL type.
	*	\param eol_type the EOL mode
	*	\return the string corresponding to the given EOL mode
	*/
	std::string EOL_str(const EOL eol_type);

	/*! \brief Concatenates files.
	*	\param inputFiles the list of files to concatenate
	*	\param outputFile the file to create
	*	\return the last error found, "" if all files have successfully been concatenated

		If an input file cannot be opened it is ignored, the returned string is set but other files are still processed.
	*/
	std::string concatenateFiles(const std::vector<std::filesystem::path>& inputFiles, const std::filesystem::path& outputFile);

}
