#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include <map>
#include <string>
#include <vector>

/**
 * @brief HTTP client for communicating with WizardMerge backend
 */
class HttpClient {
public:
  /**
   * @brief Construct HTTP client with backend URL
   * @param backendUrl URL of the backend server (e.g., "http://localhost:8080")
   */
  explicit HttpClient(const std::string &backendUrl);

  /**
   * @brief Perform a three-way merge via backend API
   * @param base Base version lines
   * @param ours Our version lines
   * @param theirs Their version lines
   * @param merged Output merged lines
   * @param hasConflicts Output whether conflicts were detected
   * @return true if successful, false on error
   */
  bool performMerge(const std::vector<std::string> &base,
                    const std::vector<std::string> &ours,
                    const std::vector<std::string> &theirs,
                    std::vector<std::string> &merged, bool &hasConflicts);

  /**
   * @brief Check if backend is reachable
   * @return true if backend responds, false otherwise
   */
  bool checkBackend();

  /**
   * @brief Get last error message
   * @return Error message string
   */
  std::string getLastError() const { return lastError_; }

private:
  std::string backendUrl_;
  std::string lastError_;

  /**
   * @brief Perform HTTP POST request
   * @param endpoint API endpoint (e.g., "/api/merge")
   * @param jsonBody JSON request body
   * @param response Output response string
   * @return true if successful, false on error
   */
  bool post(const std::string &endpoint, const std::string &jsonBody,
            std::string &response);
};

#endif // HTTP_CLIENT_H
