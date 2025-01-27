// auxiliar_functions.h
#ifndef AUXILIAR_FUNCTIONS_H
#define AUXILIAR_FUNCTIONS_H

#include <Eigen/Dense>
#include <vector>

#include <iostream>
#include "Eigen/LU"
//#include "opencl-c-base.h"
#include "random"

using Eigen::MatrixXd;
using Eigen::VectorXd;

// Define a type alias for a collection of matrices
typedef std::vector<Eigen::MatrixXd> matrix_collection;

// Function to fill vector m with the sum of each column of Z
Eigen::VectorXd fill_m(const MatrixXd& Z);

// Function to count the number of non-zero elements in vector m
unsigned count_nonzero(const Eigen::VectorXd& m);

// Function to eliminate null columns from matrix Z and return a pair of the new matrix and a vector indicating the original non-null columns
std::pair<Eigen::MatrixXd, Eigen::VectorXd> eliminate_null_columns(Eigen::MatrixXd &Z);

// Function to update the precision matrix M given a new row vector z_i
Eigen::MatrixXd update_M(const MatrixXd& M, const VectorXd& z_i);

// Function to calculate the likelihood of the data X given the latent feature matrix Z
long double calculate_likelihood(const MatrixXd& Z, const MatrixXd& X, const MatrixXd& M, double sigma_x,double sigma_a,int n_tilde,unsigned D,int n);

// Function to calculate the probability of k = certain value in binomial distribution
double binomialProbability(unsigned n_res, double prob, unsigned k);

// Function to calculate the probability of k = certain value in poisson distribution
double poissonProbability(int k, double lambda);

double metropolis_step_sigma(double current_sigma, const MatrixXd& Z, const MatrixXd& X, const MatrixXd& A, double proposal_variance, std::default_random_engine& generator, bool is_sigma_x);

// Function to perform a Metropolis-Hastings step for sigma_x or sigma_a
double metropolis_step_sigma(double current_sigma, const MatrixXd& Z, const MatrixXd& X, const MatrixXd& A, double proposal_variance, std::default_random_engine& generator, bool is_sigma_x,double prior_variance);

// Function to sample A matrix
MatrixXd sample_A(const MatrixXd& Z, const MatrixXd& X, double sigma_x, double sigma_a, std::default_random_engine& generator);


#endif //AUXILIAR_FUNCTIONS_H
