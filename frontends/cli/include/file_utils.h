#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <string>
#include <vector>

/**
 * @brief File utility functions
 */
class FileUtils {
public:
    /**
     * @brief Read a file and split into lines
     * @param filePath Path to the file
     * @param lines Output vector of lines
     * @return true if successful, false on error
     */
    static bool readLines(const std::string& filePath, std::vector<std::string>& lines);

    /**
     * @brief Write lines to a file
     * @param filePath Path to the file
     * @param lines Vector of lines to write
     * @return true if successful, false on error
     */
    static bool writeLines(const std::string& filePath, const std::vector<std::string>& lines);

    /**
     * @brief Check if a file exists
     * @param filePath Path to the file
     * @return true if file exists, false otherwise
     */
    static bool fileExists(const std::string& filePath);

    /**
     * @brief Get file size in bytes
     * @param filePath Path to the file
     * @return File size, or -1 on error
     */
    static long getFileSize(const std::string& filePath);
};

#endif // FILE_UTILS_H
