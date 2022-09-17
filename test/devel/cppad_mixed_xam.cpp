// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
# include <cppad/cppad.hpp>
# include <cppad/mixed/cppad_mixed.hpp>

namespace {
   using CppAD::log;
   using CppAD::AD;
   //
   using CppAD::mixed::d_sparse_rcv;
   using CppAD::mixed::a1_double;
   using CppAD::mixed::d_vector;
   using CppAD::mixed::a1_vector;
   //
   class mixed_derived : public cppad_mixed {
   private:
      const size_t          n_fixed_;
      const size_t          n_random_;
      const d_vector&       y_;
   // ----------------------------------------------------------------------
   public:
      // constructor
      mixed_derived(
         size_t                 n_fixed       ,
         size_t                 n_random      ,
         bool                   quasi_fixed   ,
         bool                   bool_sparsity ,
         const d_sparse_rcv&    A_rcv         ,
         const d_vector&        y             ) :
         cppad_mixed(
            n_fixed, n_random, quasi_fixed, bool_sparsity, A_rcv
         )                     ,
         n_fixed_(n_fixed)     ,
         n_random_(n_random)   ,
         y_(y)
      { }
   // ----------------------------------------------------------------------
      // implementation of ran_likelihood
      template <typename Vector>
      Vector template_ran_likelihood(
         const Vector&         theta  ,
         const Vector&         u      )
      {  typedef typename Vector::value_type scalar;
         Vector vec(1);
         assert( n_fixed_ == 1 );
         assert( n_random_ == 2 );
         assert( y_.size() == 3 );
         //
         // offset in log transform
         scalar eta;
         eta = 1e-10;

         // initialize part of log-density that is always smooth
         vec[0] = scalar(0.0);

         scalar model = theta[0];
         scalar res   = log( scalar( y_[0] ) + eta) - log( model + eta);
         vec[0]      += res * res;

         // p(y_1 | theta, u )
         model   = theta[0] * exp( u[0] );
         res     = log( scalar( y_[1] ) + eta ) - log( model + eta );
         vec[0] += res * res;

         // p(y_2 | theta, u )
         model   = theta[0] * exp( u[1] );
         res     = log( scalar( y_[2] ) + eta ) - log( model + eta );
         vec[0] += res * res;

         // p(u | theta)
         for(size_t i = 0; i < n_random_; i++)
         {  vec[0] += u[i] * u[i];
         }
         return vec;
      }
      // a1_vector version of ran_likelihood
      virtual a1_vector ran_likelihood(
         const a1_vector& fixed_vec, const a1_vector& random_vec
      )
      {  return template_ran_likelihood( fixed_vec, random_vec ); }
   };
}

bool cppad_mixed_xam(void)
{
   bool   ok = true;
   double inf = std::numeric_limits<double>::infinity();
   //
   size_t n_data   = 3;
   size_t n_fixed  = 1;
   size_t n_random = 2;
   //
   d_vector fixed_lower(n_fixed),
            fixed_upper(n_fixed),
            fixed_true(n_fixed),
            fixed_in(n_fixed);
   fixed_lower[0] = 1e-3;
   fixed_true[0]  = 2.0;
   fixed_in[0]    = 1.0;
   fixed_upper[0] = 1e3;
   //
   d_vector random_lower(n_random),
            random_upper(n_random),
            random_true(n_random),
            random_in(n_random);
   for(size_t i = 0; i < n_random; ++i)
   {  random_lower[i] = -inf;
      random_true[i]  = 0.0;
      random_in[i]    = 0.5;
      random_upper[i] = +inf;
   }
   //
   d_vector data(n_data);
   data[0] = fixed_true[0];
   data[1] = fixed_true[0] + random_true[0];
   data[2] = fixed_true[0] + random_true[1];
   //
   // object that is derived from cppad_mixed
   bool quasi_fixed   = true;
   bool bool_sparsity = true;
   d_sparse_rcv A_rcv; // empty matrix
   mixed_derived mixed_object(
         n_fixed, n_random, quasi_fixed, bool_sparsity, A_rcv, data
   );
   mixed_object.initialize(fixed_in, random_in);

   // optimize the fixed effects using quasi-Newton method
   std::string fixed_ipopt_options =
      "Integer print_level               0\n"
      "String  sb                        yes\n"
      "String  derivative_test           adaptive\n"
      "String  derivative_test_print_all yes\n"
      "Integer max_iter                  15\n"
      "Numeric tol 1e-8\n"
   ;
   std::string random_ipopt_options =
      "Integer print_level     0\n"
      "String  sb              yes\n"
      "String  derivative_test second-order\n"
      "Numeric tol             1e-10\n"
   ;
   //
   d_vector fix_constraint_lower(0), fix_constraint_upper(0);
   d_vector fixed_scale = fixed_in;
   CppAD::mixed::fixed_solution solution = mixed_object.optimize_fixed(
      fixed_ipopt_options,
      random_ipopt_options,
      fixed_lower,
      fixed_upper,
      fix_constraint_lower,
      fix_constraint_upper,
      fixed_scale,
      fixed_in,
      random_lower,
      random_upper,
      random_in
   );
   d_vector fixed_out = solution.fixed_opt;
   //
   ok &= std::fabs( fixed_out[0] / fixed_true[0] - 1.0 ) < 1e-7;
   //
   return ok;
}
