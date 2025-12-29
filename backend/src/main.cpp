/**
 * @file main.cpp
 * @brief HTTP API server for WizardMerge using Drogon framework
 */

#include "controllers/MergeController.h"
#include <drogon/drogon.h>
#include <iostream>

using namespace drogon;

int main(int argc, char *argv[]) {
  std::cout << "WizardMerge - Intelligent Merge Conflict Resolution API\n";
  std::cout << "======================================================\n";
  std::cout << "Starting HTTP server...\n\n";

  // Load configuration from file
  std::string config_file = "config.json";
  if (argc > 1) {
    config_file = argv[1];
  }

  try {
    // Load configuration and start server
    app().loadConfigFile(config_file);

    // Display listener information if available
    auto listeners = app().getListeners();
    if (!listeners.empty()) {
      try {
        std::cout << "Server will listen on port " << listeners[0].toPort
                  << "\n";
      } catch (...) {
        std::cout << "Server listener configured\n";
      }
    } else {
      std::cout << "Server configuration loaded\n";
    }

    std::cout << "Available endpoints:\n";
    std::cout << "  POST /api/merge - Three-way merge API\n";
    std::cout << "\nPress Ctrl+C to stop the server.\n\n";

    // Run the application
    app().run();
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << '\n';
    std::cerr << "Failed to load config file: " << config_file << '\n';
    std::cerr << "Usage: " << argv[0] << " [config.json]\n";
    return 1;
  }

  return 0;
}
