// SPDX-FileCopyrightText: Copyright (c) Stanford University, The Regents of the
// University of California, and others. SPDX-License-Identifier: BSD-3-Clause
/**
 * @file SparseSystem.h
 * @brief SparseSystem source file
 */
#ifndef SVZERODSOLVER_ALGREBRA_SPARSESYSTEM_HPP_
#define SVZERODSOLVER_ALGREBRA_SPARSESYSTEM_HPP_

#include <Eigen/Sparse>
#include <Eigen/SparseLU>
#include <iostream>
#include <memory>

// Forward declaration of Model
class Model;

/**
 * @brief Sparse system
 *
 * This class contains all attributes and methods to create, modify, and
 * solve sparse systems.
 *
 * Mathematical details related to setting up the governing system of
 * equations are available on the <a
 * href="https://simvascular.github.io/documentation/rom_simulation.html#0d-solver-theory">SimVascular
 * documentation</a>.
 *
 */
class SparseSystem {
 public:
  /**
   * @brief Construct a new Sparse System object
   *
   */
  SparseSystem();

  /**
   * @brief Construct a new Sparse System object
   *
   * @param n Size of the system
   */
  SparseSystem(int n);

  /**
   * @brief Destroy the Sparse System object
   *
   */
  ~SparseSystem();

  Eigen::SparseMatrix<double> F;               ///< System matrix F
  Eigen::SparseMatrix<double> E;               ///< System matrix E
  Eigen::SparseMatrix<double> dC_dy;           ///< System matrix dC/dy
  Eigen::SparseMatrix<double> dC_dydot;        ///< System matrix dC/dydot
  Eigen::Matrix<double, Eigen::Dynamic, 1> C;  ///< System vector C
  //QUESTION: How do I instantiate these? Since they are dependent on alpha, how do I ensure they represent the change of the matrices given change in alpha? 
  Eigen::SparseMatrix<double> dE_dalpha; //Derivative of w.r.t. alpha
  Eigen::SparseMatrix<double> dF_dalpha; //Derivative of F w.r.t. alpha
  Eigen::Matrix<double, Eigen::Dynamic, 1> dC_dalpha; //Derivative of C w.r.t. alpha
    

  Eigen::SparseMatrix<double> jacobian;  ///< Jacobian of the system
  Eigen::Matrix<double, Eigen::Dynamic, 1>
      residual;  ///< Residual of the system
  Eigen::Matrix<double, Eigen::Dynamic, 1>
      dydot;  ///< Solution increment of the system

  std::shared_ptr<Eigen::SparseLU<Eigen::SparseMatrix<double>>> solver =
      std::shared_ptr<Eigen::SparseLU<Eigen::SparseMatrix<double>>>(
          new Eigen::SparseLU<Eigen::SparseMatrix<double>>());  ///< Linear
                                                                ///< solver

  /**
   * @brief Reserve memory in system matrices based on number of triplets
   *
   * @param model The model to reserve space for in the system
   */
  void reserve(Model *model);

  /**
   * @brief Update the residual of the system
   *
   * @param y Vector of current solution quantities
   * @param ydot Derivate of y
   */
  void update_residual(Eigen::Matrix<double, Eigen::Dynamic, 1> &y,
                       Eigen::Matrix<double, Eigen::Dynamic, 1> &ydot);

  /**
   * @brief Update the jacobian of the system
   *
   * @param time_coeff_ydot Coefficent ydot-dependent part of jacobian
   * @param time_coeff_y Coefficent ydot-dependent part of jacobian
   */
  void update_jacobian(double time_coeff_ydot, double time_coeff_y);
  
  /**
   * @brief Update the jacobian of the system for inverse problem
   * 
   * 
   */
  void update_jacobian_inverse(Eigen::SparseMatrix<double> dE_dalpha, 
                               Eigen::SparseMatrix<double> dF_dalpha, 
                               Eigen::Matrix<double, Eigen::Dynamic, 1> dC_dalpha,
                               Eigen::Matrix<double, Eigen::Dynamic, 1> &y,
                               Eigen::Matrix<double, Eigen::Dynamic, 1> &ydot);

  
    


  /**
   * @brief Solve the system
   */
  void solve();

  /**
   * @brief Delete dynamically allocated memory (class member
   * Eigen::SparseLU<Eigen::SparseMatrix> *solver)
   */
  void clean();
};

#endif  // SVZERODSOLVER_ALGREBRA_SPARSESYSTEM_HPP_
