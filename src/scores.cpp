// -*- mode: C++; c-indent-level: 4; c-basic-offset: 4; indent-tabs-mode: nil; -*-

// [[Rcpp::depends(BH)]]
// [[Rcpp::depends(RcppEigen)]]
// [[Rcpp::depends(StanHeaders)]]

#include <stan/math.hpp>
#include <Rcpp.h>
#include <RcppEigen.h>

// [[Rcpp::plugins(cpp14)]]

using Eigen::Map;  
using Eigen::Dynamic;
using Eigen::Matrix;  


struct ip {
  const Matrix<double, Dynamic, Dynamic> S_;
  const Matrix<double, Dynamic, 1> y_;
  const Matrix<double, Dynamic, 1> x_;
  const Matrix<double, Dynamic, 1> xs_;
  ip(const Matrix<double, Dynamic, Dynamic>& S,
     const Matrix<double, Dynamic, 1>& y,
     const Matrix<double, Dynamic, 1>& x, 
     const Matrix<double, Dynamic, 1>& xs) : S_(S),y_(y),x_(x),xs_(xs) { }
  template <typename T>
  T operator()(const Matrix<T, Dynamic, 1>& Gvec) const{
    int n = S_.rows();
    int m = S_.cols();
    Matrix<T, Dynamic, Dynamic> G = to_matrix(Gvec , m , n);
    Matrix<T, Dynamic, Dynamic> SG = stan::math::multiply(S_,G);
    return (y_.transpose() * SG * y_);
  }
};

// [[Rcpp::export]]
Rcpp::List test(Rcpp::NumericMatrix Sin, 
                Rcpp::NumericVector yin,
                Rcpp::NumericVector xin, 
                Rcpp::NumericVector xsin,
                Rcpp::NumericVector Gin){
   Map<Matrix<double,Dynamic,Dynamic>> S(Rcpp::as<Map<Matrix<double,Dynamic,Dynamic>> >(Sin));
   Map<Matrix<double,Dynamic, 1>> y(Rcpp::as<Map<Matrix<double,Dynamic, 1>> >(yin));
   Map<Matrix<double,Dynamic, 1>> x(Rcpp::as<Map<Matrix<double,Dynamic, 1>> >(xin));
   Map<Matrix<double,Dynamic, 1>> xs(Rcpp::as<Map<Matrix<double,Dynamic, 1>> >(xsin));
   Map<Matrix<double,Dynamic, 1>> G(Rcpp::as<Map<Matrix<double,Dynamic, 1>> >(Gin));
   ip f(S,y,x,xs);
   double fx;
   Matrix<double, Dynamic, 1> grad_fx;
   stan::math::gradient(f, G, fx, grad_fx);
   return Rcpp::List::create(Rcpp::Named("grad") = Rcpp::wrap(grad_fx), 
                             Rcpp::Named("val") = fx);
 }


