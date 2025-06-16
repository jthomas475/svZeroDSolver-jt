// SPDX-FileCopyrightText: Copyright (c) Stanford University, The Regents of the University of California, and others.
// SPDX-License-Identifier: BSD-3-Clause
/**
 * @file svzerodsolver.cpp
 * @brief Main routine of svZeroDSolver
 */
#include <fstream>

#include "Solver.h"

/**
 *
 * @brief svZeroDSolver main routine
 *
 * This is the main routine of the svZeroDSolver. It exectutes the following
 * steps:
 *
 * 1. Read the input file
 * 2. Create the 0D model
 * 3. (Optional) Solve for steady initial condition
 * 4. Run simulation
 * 5. Write output to file
 *
 * @param argc Number of command line arguments
 * @param argv Command line arguments
 * @return Return code
 */
// SPDX-FileCopyrightText: Copyright (c) Stanford University, The Regents of the University of California, and others.
// SPDX-License-Identifier: BSD-3-Clause
/**
 * @file svzerodsolver.cpp
 * @brief Main routine of svZeroDSolver
 */
#include <fstream>

#include "Solver.h"

#include <iostream>

#include <stdexcept>

/**
 *
 * @brief svZeroDSolver main routine
 *
 * This is the main routine of the svZeroDSolver. It exectutes the following
 * steps:
 *
 * 1. Read the input file
 * 2. Create the 0D model
 * 3. (Optional) Solve for steady initial condition
 * 4. Run simulation
 * 5. Write output to file
 *
 * @param argc Number of command line arguments
 * @param argv Command line arguments
 * @return Return code
 */
int main(int argc, char* argv[]) {
  try {
    DEBUG_MSG("Starting svZeroDSolver - test");

    // Get input and output file name
    if (argc < 2 || argc > 3) {
      throw std::runtime_error("Usage: svzerodsolver path/to/config.json [optional:path/to/output.csv]");
    }

    std::string input_file_name = argv[1];
    std::string output_file_path;
    std::string output_file_name;

    if (argc == 3) {
      output_file_name = argv[2];
    } else {
      std::size_t end_of_path = input_file_name.rfind("/");
      if (end_of_path == std::string::npos) {
        end_of_path = input_file_name.rfind("\\");
        if (end_of_path == std::string::npos) {
          output_file_path = ".";
        }
      } else {
        output_file_path = input_file_name.substr(0, end_of_path);
      }
      output_file_name = output_file_path + "/output.csv";
      std::cout << "[svzerodsolver] Output will be written to '" << output_file_name << "'." << std::endl;
    }

    std::ifstream input_file(input_file_name);
    if (!input_file.is_open()) {
      std::cerr << "[svzerodsolver] Error: The input file '" << input_file_name << "' cannot be opened." << std::endl;
      return 1;
    }

    std::cout << "[DEBUG] About to parse json file..." << std::endl;
    nlohmann::json config;
    try {
      config = nlohmann::json::parse(input_file);
    } catch (const nlohmann::json::parse_error& e) {
      std::cerr << "[svzerodsolver] Error: Failed to parse the input file '" << input_file_name << "'." << std::endl;
      std::cerr << "Details: " << e.what() << std::endl;
      std::cerr << "Try using 'svzerodcalibrator' instead" << std::endl;
      return 1;
    }
    std::cout << "[DEBUG] About to construct solver...." << std::endl;
    auto solver = Solver(config);  // Likely the place that throws error!
    std::cout << "[DEBUG] Solver constructed." << std::endl;
    solver.run();
    solver.write_result_to_csv(output_file_name);

    return 0;

  } catch (const std::exception& e) {
    std::cerr << "[svzerodsolver] Fatal error: " << e.what() << std::endl;
    std::cerr << "[svzerodsovler] Try using 'svzerodcalibrator' instead" << std::endl;
    return 1;

  } catch (...) {
    std::cerr << "[svzerodsolver] An unknown error occurred." << std::endl;
    return 1;
  }
}


/*
catch (const std::runtime_error& e) {
    std::cerr << "[svzerodsolver] Runtime error: " << input_file_name
    std::cerr << "Use 'svzerodcalibrator instead to run inverse problem'" << std::endl;
    return 1;
  } 
catch (...) {
    std::cerr << "[svzerodsolver] An unknown error occurred. "
              << "Use 'svzerodcalibrator instead to run inverse problem'" << std::endl;
    return 1;
  }
    */
