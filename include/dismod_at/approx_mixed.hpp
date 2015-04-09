// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_APPROX_MIXED_HPP
# define DISMOD_AT_APPROX_MIXED_HPP
# include <cppad/cppad.hpp>

# define DISMOD_AT_DEFINE_JOINT_DENSITY(Float)        \
	virtual CppAD::vector< Float > joint_density(     \
		const CppAD::vector< Float >& fixed_vec  ,    \
		const CppAD::vector< Float >& random_vec ) = 0;
//
# define DISMOD_AT_DEFINE_FIXED_DENSITY(Float)        \
	virtual CppAD::vector< Float > fixed_density(     \
		const CppAD::vector< Float >& fixed_vec  ) = 0 ;

extern bool gradient_random_xam(void);
extern bool hessian_random_xam(void);
extern bool joint_laplace_xam(void);

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

class approx_mixed {
private:
/*
------------------------------------------------------------------------------
$begin approx_mixed_private$$
$spell
	typedef
	CppAD
	vec
	const
	bool
	xam
$$

$section approx_mixed Private Declarations$$

$head typedef$$
$index a1_double$$
$index a2_double$$
$index d_vector$$
$index a1d_vector$$
$index a2d_vector$$
$index a3d_vector$$
$codep */
	typedef CppAD::AD<double>          a1_double;
	typedef CppAD::AD<a1_double>       a2_double;
	typedef CppAD::AD<a2_double>       a3_double;
	typedef CppAD::vector<double>      d_vector;
	typedef CppAD::vector<a1_double>   a1d_vector;
	typedef CppAD::vector<a2_double>   a2d_vector;
	typedef CppAD::vector<a3_double>   a3d_vector;
/* $$
$head n_fixed_$$
The number of fixed effects is given by
$codep */
	const size_t n_fixed_;
/* $$
$head n_random_$$
The number of random effects is given by
$codep */
	const size_t n_random_;
/* $$
$head gradient_$$
The gradient of the joint likelihood w.r.t. the random effects
$latex f_u^{(1)} ( \theta , u )^T$$. Because this is a simple vector
there is no difference between the gradient and the derivative; i.e.,
the transpose does not matter.
$codep */
	CppAD::ADFun<a1_double> gradient_;   // computes the gradient values
/* $$
$head hessian_$$
The Hessian of the joint likelihood w.r.t. the random effects
$latex f_{uu}^{(2)} ( \theta , u )$$ is as a sparse matrix by
the following variables:
$codep */
	CppAD::ADFun<a1_double> hessian_;     // computes the hessian values
	CppAD::vector<size_t>   hessian_row_; // corresponding row indices
	CppAD::vector<size_t>   hessian_col_; // corresponding column indices
/* $$
$head record_gradient$$
See $cref approx_mixed_record_gradient$$.
$codep */
	void record_gradient(
		const d_vector& fixed_vec ,
		const d_vector& random_vec
	);
/* $$
$head record_hessian$$
See $cref approx_mixed_record_hessian$$.
$codep */
	void record_hessian(
		const d_vector& fixed_vec ,
		const d_vector& random_vec
	);
/* $$
$head gradient_random$$
See $cref approx_mixed_gradient_random$$
$codep */
	// gradient_random
	a1d_vector gradient_random(
		const a1d_vector&       fixed_vec   ,
		const a1d_vector&       random_vec
	);
	friend bool ::gradient_random_xam(void);
/* $$
$head hessian_random$$
See $cref approx_mixed_hessian_random$$
$codep */
	// hessian_random
	void hessian_random(
		const a1d_vector&       fixed_vec   ,
		const a1d_vector&       random_vec  ,
		CppAD::vector<size_t>&  row_out     ,
		CppAD::vector<size_t>&  col_out     ,
		a1d_vector&             val_out
	);
	friend bool ::hessian_random_xam(void);
/* $$
$head joint_laplace$$
See $cref approx_mixed_joint_laplace$$
$codep */
	// joint_laplace
	a1_double joint_laplace(
		const a1d_vector& beta   ,
		const a1d_vector& theta  ,
		const a1d_vector& u
	);
	friend bool ::joint_laplace_xam(void);
/* $$
$childtable%devel/approx_mixed/record_gradient.cpp
	%devel/approx_mixed/record_hessian.cpp
	%devel/approx_mixed/gradient_random.cpp
	%devel/approx_mixed/hessian_random.cpp
	%devel/approx_mixed/joint_laplace.cpp
%$$

$end
-------------------------------------------------------------------------------
*/
public:
	// constructor
	approx_mixed(size_t n_fixed, size_t n_random)
	:
	n_fixed_(n_fixed)   ,
	n_random_(n_random)
	{ }
	// joint density for data and random effects given the fixed effects
	// (pure vritual function so must be defined by derived class)
	DISMOD_AT_DEFINE_JOINT_DENSITY( double )
	DISMOD_AT_DEFINE_JOINT_DENSITY( a1_double )
	DISMOD_AT_DEFINE_JOINT_DENSITY( a2_double )
	DISMOD_AT_DEFINE_JOINT_DENSITY( a3_double )
	// prior density for fixed effects
	// (pure vritual function so must be defined by derived class)
	DISMOD_AT_DEFINE_FIXED_DENSITY( double )
	DISMOD_AT_DEFINE_FIXED_DENSITY( a1_double )
	//
	// initailization that requires calling virtual functions
	void initialize(const d_vector& fixed_vec, const d_vector& random_vec);
	//
	// optimize_random
	d_vector optimize_random(
		const d_vector& fixed_vec ,
		const d_vector& random_in
	);
};

} // END_DISMOD_AT_NAMESPACE

# endif
