// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_APPROX_MIXED_HPP
# define DISMOD_AT_APPROX_MIXED_HPP
# include <cppad/cppad.hpp>

//
extern bool joint_grad_ran_xam(void);
extern bool joint_hes_ran_xam(void);
extern bool laplace_eval_xam(void);
extern bool laplace_beta_xam(void);
extern bool laplace_hes_fix_xam(void);
extern bool prior_eval_xam(void);
extern bool prior_jac_xam(void);
extern bool prior_hes_xam(void);
namespace dismod_at {
	// Ipopt NLP clases that use the approx_mixed information for optimization
	class optimize_random_eval;
	class ipopt_fixed;
}

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

class approx_mixed {
	friend optimize_random_eval;
	friend ipopt_fixed;
public:
/*
$begin approx_mixed_public$$
$spell
	const
	vec
	typedef
	CppAD
	ctor
	std
$$

$section approx_mixed Public Declarations$$

$head AD Types$$
$index a_double$$
$codep */
	typedef CppAD::AD<double>          a1_double;
	typedef CppAD::AD<a1_double>       a2_double;
	typedef CppAD::AD<a2_double>       a3_double;
	typedef CppAD::AD<a3_double>       a4_double;
	typedef CppAD::AD<a4_double>       a5_double;
/* $$

$head Vector Types$$
$mindex d_vector a1d_vector a2d_vector a3d_vector a4d_vector$$
$codep */
	typedef CppAD::vector<double>      d_vector;
	typedef CppAD::vector<a1_double>   a1d_vector;
	typedef CppAD::vector<a2_double>   a2d_vector;
	typedef CppAD::vector<a3_double>   a3d_vector;
	typedef CppAD::vector<a4_double>   a4d_vector;
	typedef CppAD::vector<a5_double>   a5d_vector;
/* $$
$head constructor$$
Construct an $code approx_mixed$$ derived class object; see
$cref/derived_ctor/approx_mixed_derived_ctor/$$.
$codep */
	approx_mixed(size_t n_fixed, size_t n_random)
	:
	n_fixed_(n_fixed)   ,
	n_random_(n_random)
	{ }
/* $$
$head initialize$$
Directly after construction, use this function to initialize
the derived class object; see $cref/initialize/approx_mixed_initialize/$$.
$codep */
	void initialize(const d_vector& fixed_vec, const d_vector& random_vec);
/* $$
$head joint_density$$
This is a pure virtual function so it must be defined by derived class;
see $cref/joint_density/approx_mixed_joint_density/$$.
$codep */
	virtual CppAD::vector<a5_double> joint_density(
		const CppAD::vector<a5_double>& fixed_vec  ,
		const CppAD::vector<a5_double>& random_vec
	) = 0;
/* $$
$head prior_density$$
These are pure virtual functions so they must be defined by derived class;
see $cref/prior_density/approx_mixed_prior_density/$$.
$codep */
	virtual CppAD::vector<a1_double> prior_density(
		const CppAD::vector<a1_double>& fixed_vec
	) = 0 ;
/* $$
$head optimize_random$$
Given the fixed effects, optimize with respect to the random effects;
see  $cref/optimize_random/approx_mixed_optimize_random/$$.
$codep */
	d_vector optimize_random(
		const d_vector& fixed_vec ,
		const d_vector& random_in
	);
/* $$
$head optimize_fixed$$
Optimize the total objective with respect to the fixed effects;
see  $cref/optimize_fixed/approx_mixed_optimize_fixed/$$.
$codep */
	d_vector optimize_fixed(
		const std::string& options     ,
		const d_vector&    fixed_lower ,
		const d_vector&    fixed_in    ,
		const d_vector&    fixed_upper ,
		const d_vector&    random_in
	);
/* $$
$childtable%
	devel/approx_mixed/derived_ctor.omh%
	devel/approx_mixed/initialize.cpp%
	devel/approx_mixed/joint_density.omh%
	devel/approx_mixed/prior_density.omh%
	devel/approx_mixed/optimize_random.cpp%
	devel/approx_mixed/optimize_fixed.cpp
%$$
$end
*/
private:
/*
------------------------------------------------------------------------------
$begin approx_mixed_private$$
$spell
	jac
	Jacobians
	jacobian
	hes
	eval
	typedef
	CppAD
	vec
	const
	bool
	xam
$$

$section approx_mixed Private Declarations$$

$childtable%include/dismod_at/approx_pack.hpp
	%include/dismod_at/approx_unpack.hpp
	%devel/approx_mixed/record_joint.cpp
	%devel/approx_mixed/record_grad_ran.cpp
	%devel/approx_mixed/record_hes_ran.cpp
	%devel/approx_mixed/record_laplace.cpp
	%devel/approx_mixed/record_hes_fix.cpp
	%devel/approx_mixed/record_prior.cpp
	%devel/approx_mixed/joint_grad_ran.cpp
	%devel/approx_mixed/joint_hes_ran.cpp
	%devel/approx_mixed/laplace_eval.cpp
	%devel/approx_mixed/laplace_beta.cpp
	%devel/approx_mixed/laplace_hes_fix.cpp
	%devel/approx_mixed/prior_eval.cpp
	%devel/approx_mixed/prior_jac.cpp
	%devel/approx_mixed/prior_hes.cpp
%$$

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
$head joint_density_$$
Recording of the $cref/joint_density/approx_mixed_joint_density/$$ function
which evaluates a
$cref/negative log-density vector/approx_mixed/Negative Log-Density Vector/$$
corresponding to
$cref/f(theta, u)/approx_mixed_theory/Joint Density, f(theta, u)/$$
for different levels of AD:
$codep */
	CppAD::ADFun<double>      a0_joint_density_;
	CppAD::ADFun<a1_double>   a1_joint_density_;
	CppAD::ADFun<a2_double>   a2_joint_density_;
	CppAD::ADFun<a3_double>   a3_joint_density_;
	CppAD::ADFun<a4_double>   a4_joint_density_;
/* $$
$head grad_ran_$$
The gradient of the joint likelihood w.r.t. the random effects
$latex f_u^{(1)} ( \theta , u )^T$$. Because this is a simple vector
there is no difference between the gradient and the derivative; i.e.,
the transpose does not matter.
$codep */
	CppAD::ADFun<a3_double> grad_ran_;   // computes the gradient values
/* $$
$head hes_ran_$$
The Hessian of the joint likelihood w.r.t. the random effects
$latex f_{uu}^{(2)} ( \theta , u )$$ is as a sparse matrix
represented by the following variables:
$codep */
	CppAD::ADFun<a3_double> hes_ran_;     // computes the hessian values
	CppAD::vector<size_t>   hes_ran_row_; // corresponding row indices
	CppAD::vector<size_t>   hes_ran_col_; // corresponding column indices
/* $$
$head laplace_k_$$
For $icode%k% = 0 , 1, 2%$$, $codei%laplace_%k%_%$$ is $th k$$ order accurate
in $latex \beta$$ recording of the Joint part of the Laplace approximation;
i.e., $latex H( \beta , \theta , u)$$.
$codep */
	CppAD::ADFun<double>    laplace_0_;     // for computing H
	CppAD::ADFun<double>    laplace_1_;     // for computing H_beta
	CppAD::ADFun<a2_double> laplace_2_;     // for computing H_beta_beta
/* $$
$head hes_fix_$$
The Hessian of the joint likelihood w.r.t. the fixed effects
$latex H_{\beta \beta}^{(2)} ( \beta, \theta , u )$$ is as a sparse matrix
represented by the following variables:
$codep */
	CppAD::ADFun<double>    hes_fix_;     // computes the hessian values
	CppAD::vector<size_t>   hes_fix_row_; // corresponding row indices
	CppAD::vector<size_t>   hes_fix_col_; // corresponding column indices
/* $$
$head prior_density_$$
Recording of the $cref/prior_density/approx_mixed_prior_density/$$ function
which evaluates a
$cref/negative log-density vector/approx_mixed/Negative Log-Density Vector/$$
corresponding to
$cref/g(theta)/approx_mixed_theory/Prior Density, g(theta)/$$.
$codep */
	CppAD::ADFun<double>    prior_density_; // computes prior density
	CppAD::vector<size_t>   prior_jac_row_; // prior jacobian row indices
	CppAD::vector<size_t>   prior_jac_col_; // prior jacobian column indices
	CppAD::sparse_jacobian_work prior_jac_work_;
	CppAD::vector<size_t>   prior_hes_row_; // prior hessian row indices
	CppAD::vector<size_t>   prior_hes_col_; // prior hessian column indices
	CppAD::sparse_hessian_work prior_hes_work_;
/* $$

$head pack$$
See $cref approx_mixed_pack$$.
$codep */
	template <class Float_unpack, class Float_pack>
	void pack(
		const CppAD::vector<Float_unpack>& fixed_one  ,
		const CppAD::vector<Float_unpack>& random_vec ,
		CppAD::vector<Float_pack>&         both_vec
	) const;
	template <class Float_unpack, class Float_pack>
	void pack(
		const CppAD::vector<Float_unpack>& fixed_one  ,
		const CppAD::vector<Float_unpack>& fixed_two  ,
		const CppAD::vector<Float_unpack>& random_vec ,
		CppAD::vector<Float_pack>&         three_vec
	) const;
/* $$
$head unpack$$
See $cref approx_mixed_unpack$$.
$codep */
	template <class Float_unpack, class Float_pack>
	void unpack(
		CppAD::vector<Float_unpack>&       fixed_one  ,
		CppAD::vector<Float_unpack>&       random_vec ,
		const CppAD::vector<Float_pack>&   both_vec
	) const;
	template <class Float_unpack, class Float_pack>
	void unpack(
		CppAD::vector<Float_unpack>&       fixed_one  ,
		CppAD::vector<Float_unpack>&       fixed_two  ,
		CppAD::vector<Float_unpack>&       random_vec ,
		const CppAD::vector<Float_pack>&   three_vec
	) const;
/* $$
$head record_joint$$
See $cref approx_mixed_record_joint$$.
$codep */
	void record_joint(
		const d_vector& fixed_vec ,
		const d_vector& random_vec
	);
/* $$
$head record_grad_ran$$
See $cref approx_mixed_record_grad_ran$$.
$codep */
	void record_grad_ran(
		const d_vector& fixed_vec ,
		const d_vector& random_vec
	);
/* $$
$head record_hes_ran$$
See $cref approx_mixed_record_hes_ran$$.
$codep */
	void record_hes_ran(
		const d_vector& fixed_vec ,
		const d_vector& random_vec
	);
/* $$
$head record_laplace$$
See $cref approx_mixed_record_laplace$$.
$codep */
	void record_laplace(
		size_t          order     ,
		const d_vector& fixed_vec ,
		const d_vector& random_vec
	);
/* $$
$head record_hes_fix$$
See $cref approx_mixed_record_hes_fix$$.
$codep */
	void record_hes_fix(
		const d_vector& fixed_vec ,
		const d_vector& random_vec
	);
/* $$
$head record_prior$$
See $cref approx_mixed_record_prior$$.
$codep */
	void record_prior(const d_vector& fixed_vec);
/* $$
$head joint_grad_ran$$
See $cref approx_mixed_joint_grad_ran$$
$codep */
	// joint_grad_ran
	a3d_vector joint_grad_ran(
		const a3d_vector&       fixed_vec   ,
		const a3d_vector&       random_vec
	);
	friend bool ::joint_grad_ran_xam(void);
/* $$
$head joint_hes_ran$$
See $cref approx_mixed_joint_hes_ran$$
$codep */
	// joint_hes_ran
	void joint_hes_ran(
		const a3d_vector&       fixed_vec   ,
		const a3d_vector&       random_vec  ,
		CppAD::vector<size_t>&  row_out     ,
		CppAD::vector<size_t>&  col_out     ,
		a3d_vector&             val_out
	);
	friend bool ::joint_hes_ran_xam(void);
/* $$
$head laplace_eval$$
See $cref approx_mixed_laplace_eval$$
$codep */
	// laplace_eval
	double laplace_eval(
		const d_vector& beta   ,
		const d_vector& theta  ,
		const d_vector& u
	);
	friend bool ::laplace_eval_xam(void);
/* $$
$head laplace_beta$$
See $cref approx_mixed_laplace_beta$$
$codep */
	// laplace_beta
	d_vector laplace_beta(
		const d_vector& beta   ,
		const d_vector& theta  ,
		const d_vector& u
	);
	friend bool ::laplace_beta_xam(void);
/* $$
$head laplace_hes_fix$$
See $cref approx_mixed_laplace_hes_fix$$
$codep */
	// laplace_hes_fix
	void laplace_hes_fix(
		const d_vector&         fixed_vec   ,
		const d_vector&         random_vec  ,
		CppAD::vector<size_t>&  row_out     ,
		CppAD::vector<size_t>&  col_out     ,
		d_vector&               val_out
	);
	friend bool ::laplace_hes_fix_xam(void);
/* $$
$head prior_eval$$
See $cref approx_mixed_prior_eval$$
$codep */
	// prior_eval
	d_vector prior_eval(const d_vector& fixed_vec);
	friend bool ::prior_eval_xam(void);
/* $$
$end
-------------------------------------------------------------------------------
$head prior_jac$$
See $cref approx_mixed_prior_jac$$
$codep */
	// prior_jac
	void prior_jac(
		const d_vector&        fixed_vec   ,
		CppAD::vector<size_t>& row_out     ,
		CppAD::vector<size_t>& col_out     ,
		d_vector&              val_out
	);
	friend bool ::prior_jac_xam(void);
/* $$
$end
-------------------------------------------------------------------------------
$head prior_hes$$
See $cref approx_mixed_prior_hes$$
$codep */
	// prior_hes
	void prior_hes(
		const d_vector&        fixed_vec   ,
		const d_vector&        weight      ,
		CppAD::vector<size_t>& row_out     ,
		CppAD::vector<size_t>& col_out     ,
		d_vector&              val_out
	);
	friend bool ::prior_hes_xam(void);
/* $$
$end
-------------------------------------------------------------------------------
*/
};

} // END_DISMOD_AT_NAMESPACE

# include <dismod_at/approx_pack.hpp>
# include <dismod_at/approx_unpack.hpp>

# endif
