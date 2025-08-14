// SPDX-FileCopyrightText: Copyright (c) Stanford University, The Regents of the
// University of California, and others. SPDX-License-Identifier: BSD-3-Clause
#include "LevenbergMarquardtOptimizer.h"

#include <iomanip>

LevenbergMarquardtOptimizer::LevenbergMarquardtOptimizer(
    Model* model, int num_obs, int num_params, double lambda0, double tol_grad,
    double tol_inc, int max_iter) {
  
  size = model->dofhandler.size();
  system = SparseSystem(size);
  y_robs = Eigen::Matrix<double, Eigen::Dynamic, 1>(size); // m
  dy_robs = Eigen::Matrix<double, Eigen::Dynamic, 1>(size);
  this->time_step_size = time_step_size;
  this->atol = atol;


  this->model = model;
  this->num_obs = num_obs;
  this->num_params = num_params;
  this->num_eqns = model->dofhandler.get_num_equations();
  this->num_vars = model->dofhandler.get_num_variables();
  this->num_dpoints = this->num_obs * this->num_eqns;
  this->lambda = lambda0;
  this->tol_grad = tol_grad;
  this->tol_inc = tol_inc;
  this->max_iter = max_iter;

  // Make some memory reservations
  system.reserve(model);


  jacobian = Eigen::SparseMatrix<double>(num_dpoints, num_params);
  residual = Eigen::Matrix<double, Eigen::Dynamic, 1>::Zero(num_dpoints);
  mat = Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>(num_params,
                                                              num_params);
  vec = Eigen::Matrix<double, Eigen::Dynamic, 1>::Zero(num_params);
}


void LevenbergMarquardtOptimizer::clean() {
    system.clean();
  }

Eigen::Matrix<double, Eigen::Dynamic, 1> LevenbergMarquardtOptimizer::run(
    Eigen::Matrix<double, Eigen::Dynamic, 1> alpha,
    std::vector<std::vector<double>>& y_obs,
    std::vector<std::vector<double>>& dy_obs) {
  for (size_t i = 0; i < max_iter; i++) {
    update_gradient(alpha, y_obs, dy_obs);
    // create update_residual using system and y_obs and dy_obs
    model->update_constant(system);
    model->update_time(system,0.0); //QUESTION: Should I just set 0.0 as current time value or set it to the time_step?
    model->update_solution(system,y_robs,dy_robs);
    system.update_residual(y_robs,dy_robs);

    system.update_jacobian_inverse(system.dE_dalpha, system.dF_dalpha, system.dC_dalpha, y_robs, dy_robs);
    if (i == 0) {
      update_delta(true);
    } else {
      update_delta(false);
    }

    alpha -= delta; // alpha is parameter vector
    double norm_grad = vec.norm();
    double norm_inc = delta.norm();
    std::cout << std::setprecision(1) << std::scientific << "Iteration "
              << i + 1 << " | lambda: " << lambda << " | norm inc: " << norm_inc
              << " | norm grad: " << norm_grad << std::endl;
    if ((norm_grad < tol_grad) && (norm_inc < tol_inc)) {
      break;
    }
    if (i >= max_iter - 1) {
      std::cout << "Maximum number of iterations reached" << std::endl;
      break;
    }
  }
  return alpha;
}

void LevenbergMarquardtOptimizer::update_gradient(
    Eigen::Matrix<double, Eigen::Dynamic, 1>& alpha,
    std::vector<std::vector<double>>& y_obs,
    std::vector<std::vector<double>>& dy_obs) {
  // Set jacobian and residual to zero
  jacobian.setZero();
  residual.setZero();

  // Assemble gradient and residual
  for (size_t i = 0; i < num_obs; i++) {
    for (size_t j = 0; j < model->get_num_blocks(true); j++) {
      auto block = model->get_block(j);
      for (size_t l = 0; l < block->global_eqn_ids.size(); l++) {
        block->global_eqn_ids[l] += num_eqns * i;
      }
      block->update_gradient(jacobian, residual, alpha, y_obs[i], dy_obs[i]);
      for (size_t l = 0; l < block->global_eqn_ids.size(); l++) {
        block->global_eqn_ids[l] -= num_eqns * i;
      }
    }
  }
}

/**
 * State LevenbergMarquardtOptimizer::step(const State& old_state, double time) {
    State new_state = State::Zero(size);
    for(size_t i = 0; i < max_iter; i++) {
      dy_robs.setZero();
      y_robs.setZero();
      dy_robs += old_state.ydot + (new_state.ydot - old_state.ydot) * alpha_m;
      y_robs += old_state.y + (new_state.y - old_state.y) * alpha_f;

      system.update_jacobian(placeholder1, placeholder2);
      system.update_residual(y_robs, dy_robs);
    }
    return new_state;
  }
    **/

  
  


void LevenbergMarquardtOptimizer::update_delta(bool first_step) {
  // Cache old gradient vector and calulcate new one
  Eigen::Matrix<double, Eigen::Dynamic, 1> vec_old = vec;
  vec = jacobian.transpose() * residual;

  // Determine new lambda parameter from new and old gradient vector
  if (!first_step) {
    lambda *= vec.norm() / vec_old.norm();
  }

  // Determine gradient matrix
  Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> jacobian_sq =
      jacobian.transpose() * jacobian;
  Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> jacobian_sq_diag =
      jacobian_sq.diagonal().asDiagonal();
  mat = jacobian_sq + lambda * jacobian_sq_diag;

  // Solve for new delta
  delta = mat.llt().solve(vec);
}
