#pragma once

/*! \file file_utils.h
*	\brief Few file functions. See functions documentation for details.
*	\author Christophe COUAILLET
*/

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#endif

#include <filesystem>
#include <vector>

/*! \brief Defines the types of end of line EOL */
enum class EOL
{
	Unknown,
	Windows,		// CR + LF
	Unix,			// LF
	Mac				// CR
};

/*!	\brief Returns a list of paths matching the specified pattern that can contain *, ? (Windows, POSIX) and [..] (POSIX).

	Ending path separators are ignored. Wildcards are only allowed in the filename part of the path.
	\warning Wildcard [..] support is not yet implemented.
*/
std::vector<std::filesystem::path> dir(const std::string& pattern);

/*! \brief Detects the EOL char(s) used in a file.
	Returns EOL::Unknown if the file does not exist or if EOL chars are not found in the 4096 bytes at end or begin of the file.
*/
EOL file_EOL(const std::filesystem::path& filepath);

/*! \brief Returns the length of the EOL type. */
size_t EOL_length(const EOL eol_type);

/*! \brief Returns the chars sequence of the EOL type. */
std::string EOL_str(const EOL eol_type);
