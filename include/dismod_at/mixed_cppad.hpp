// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_MIXED_CPPAD_HPP
# define DISMOD_AT_MIXED_CPPAD_HPP

# include <map>
# include <cppad/cppad.hpp>
# include <dismod_at/newton_step.hpp>

// private examples
extern bool constraint_eval_xam(void);
extern bool constraint_jac_xam(void);
extern bool constraint_hes_xam(void);
extern bool ran_like_grad_xam(void);
extern bool ranobj_hes_xam(void);
extern bool fix_like_eval_xam(void);
extern bool fix_like_jac_xam(void);
extern bool fix_like_hes_xam(void);
extern bool hes_ran_fun_xam(void);
extern bool ranobj_eval_xam(void);
extern bool logdet_grad_xam(void);
extern bool ranobj_grad_xam(void);
extern bool hes_cross_xam(void);

//  tests
extern bool der_var_hes(void);
extern bool delta_ranobj(void);

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

class mixed_cppad {
	friend class optimize_random_eval;
	friend class ipopt_fixed;
public:
/*
$begin mixed_cppad_public$$
$spell
	ranobj
	cppad
	obj
	const
	vec
	typedef
	CppAD
	ctor
	std
$$

$section mixed_cppad Public Declarations$$

$head AD Types$$
$index a_double$$
$codep */
	typedef CppAD::AD<double>          a1_double;
	typedef CppAD::AD<a1_double>       a2_double;
/* $$

$head Vector Types$$
$mindex d_vector a1d_vector a2d_vector$$
$codep */
	typedef CppAD::vector<double>      d_vector;
	typedef CppAD::vector<a1_double>   a1d_vector;
	typedef CppAD::vector<a2_double>   a2d_vector;
/* $$
$head User Defined$$
The following are $code mixed_cppad$$ pure virtual functions and hence must
be defined by the user's derived class:

$subhead ran_like$$
$codep */
	virtual a2d_vector ran_like(
		const a2d_vector& fixed_vec  ,
		const a2d_vector& random_vec
	) = 0;
/* $$
See $cref/ran_like/mixed_cppad_ran_like/$$.

$subhead fix_like$$
$codep */
	virtual a1d_vector fix_like(
		const a1d_vector& fixed_vec
	) = 0 ;
/* $$
See $cref/fix_like/mixed_cppad_fix_like/$$.

$subhead constraint$$
$codep */
	virtual a1d_vector constraint(
		const a1d_vector& fixed_vec
	) = 0 ;
/* $$
See $cref/constraint/mixed_cppad_constraint/$$.

$subhead fatal_error$$
$codep */
	virtual void fatal_error(const std::string& error_message) = 0;
/* $$
This routine displays error message and then exits the program
(hence it does not return).

$subhead warning$$
$codep */
	virtual void warning(const std::string& warning_message) = 0;
/* $$
This routine displays a warning message and then returns.

$head constructor$$
Construct an $code mixed_cppad$$ derived class object; see
$cref/derived_ctor/mixed_cppad_derived_ctor/$$.
$codep */
	mixed_cppad(size_t n_fixed, size_t n_random)
/* $$
$comment */
	:
	n_fixed_(n_fixed)               ,
	n_random_(n_random)             ,
	initialize_done_(false)         ,
	record_fix_like_done_(false)    ,
	record_constraint_done_(false)  ,
	record_ran_like_done_(false)    ,
	record_hes_ran_done_(false)     ,
	record_hes_cross_done_(false)   ,
	record_newton_atom_done_(false) ,
	record_ranobj_done_(false)     ,
	record_hes_ranobj_done_(false)
	{ }
/* $$
$head initialize$$
Directly after construction, use this function to initialize
the derived class object; see $cref/initialize/mixed_cppad_initialize/$$.
$codep */
	std::map<std::string, size_t> initialize(
		const d_vector& fixed_vec, const d_vector& random_vec
	);
/* $$
$head optimize_random$$
Given the fixed effects, optimize with respect to the random effects;
see  $cref/optimize_random/mixed_cppad_optimize_random/$$.
$codep */
	d_vector optimize_random(
		const std::string& options   ,
		const d_vector&    fixed_vec ,
		const d_vector&    random_in
	);
/* $$
$head optimize_fixed$$
Optimize the total objective with respect to the fixed effects;
see  $cref/optimize_fixed/mixed_cppad_optimize_fixed/$$.
$codep */
	d_vector optimize_fixed(
		const std::string& fixed_options    ,
		const std::string& random_options   ,
		const d_vector&    fixed_lower      ,
		const d_vector&    fixed_upper      ,
		const d_vector&    constraint_lower ,
		const d_vector&    constraint_upper ,
		const d_vector&    fixed_in         ,
		const d_vector&    random_in
	);
/* $$
$childtable%
	devel/mixed_cppad/derived_ctor.omh%
	devel/mixed_cppad/initialize.cpp%
	devel/mixed_cppad/ran_like.omh%
	devel/mixed_cppad/fix_like.omh%
	devel/mixed_cppad/constraint.omh%
	devel/mixed_cppad/optimize_random.cpp%
	devel/mixed_cppad/optimize_fixed.cpp
%$$
$end
*/
private:
/*
------------------------------------------------------------------------------
$begin mixed_cppad_private$$
$spell
	ranobj
	var
	cppad
	hes hes
	obj
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
	uu
	logdet
$$

$section mixed_cppad Private Declarations$$

$childtable%include/dismod_at/mixed_pack.hpp
	%include/dismod_at/mixed_unpack.hpp
	%devel/mixed_cppad/constraint_eval.cpp
	%devel/mixed_cppad/constraint_jac.cpp
	%devel/mixed_cppad/constraint_hes.cpp
	%devel/mixed_cppad/record_ran_like.cpp
	%devel/mixed_cppad/record_hes_ran.cpp
	%devel/mixed_cppad/record_hes_cross.cpp
	%devel/mixed_cppad/newton_step.cpp
	%devel/mixed_cppad/record_ranobj.cpp
	%devel/mixed_cppad/record_hes_ranobj.cpp
	%devel/mixed_cppad/record_fix_like.cpp
	%devel/mixed_cppad/record_constraint.cpp
	%devel/mixed_cppad/ranobj_eval.cpp
	%devel/mixed_cppad/logdet_grad.cpp
	%devel/mixed_cppad/ranobj_grad.cpp
	%devel/mixed_cppad/ran_like_grad.cpp
	%devel/mixed_cppad/ranobj_hes.cpp
	%devel/mixed_cppad/fix_like_eval.cpp
	%devel/mixed_cppad/fix_like_jac.cpp
	%devel/mixed_cppad/fix_like_hes.cpp
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
$head initialize_done_$$
The following flag is false after construction and true after
the corresponding member function is called:
$codep */
	bool                initialize_done_;
	bool                record_fix_like_done_;
	bool                record_constraint_done_;
	// only called when n_random_ > 0
	bool                record_ran_like_done_;
	bool                record_hes_ran_done_;
	bool                record_hes_cross_done_;
	// only called when n_random_ > 0 and MIXED_CPPAD_NEWTON is true
	bool                record_newton_atom_done_;
	bool                record_ranobj_done_;
	bool                record_hes_ranobj_done_;
/* $$

$head n_random_ > 0$$
The following values are only defined when $icode%n_random_% > 0%$$:

$subhead ran_like$$
If $icode%n_random_% > 0%$$ and $code record_ran_like_done_$$,
$cref/ran_like_fun_/record_ran_like/ran_like_fun_/$$ and
$cref/ran_like_a1fun_/record_ran_like/ran_like_a1fun_/$$ are
recordings of the user's $cref/ran_like/mixed_cppad_ran_like/$$.
function.
$codep */
	CppAD::ADFun<double>      ran_like_fun_;
	CppAD::ADFun<a1_double>   ran_like_a1fun_;
/* $$

$subhead hes_ran_$$
If $icode%n_random_% > 0%$$ and $code record_hes_ran_done_$$,
$cref/hes_ran_row_/record_hes_ran/hes_ran_row_/$$,
$cref/hes_ran_col_/record_hes_ran/hes_ran_col_/$$, and
$cref/hes_ran_work_/record_hes_ran/hes_ran_work_/$$,
can be used to compute the sparse Hessian
$latex f_{uu}^{(2)} ( \theta , u )$$.
This sparse Hessian computation is recorded in
$cref/hes_ran_fun_/record_hes_ran/hes_ran_fun_/$$.
$codep */
	CppAD::vector<size_t>      hes_ran_row_;  // row indices
	CppAD::vector<size_t>      hes_ran_col_;  // column indices
	CppAD::sparse_hessian_work hes_ran_work_; // work structure
	CppAD::ADFun<double>       hes_ran_fun_;  // recording of Hessian calc
	//
	friend bool ::hes_ran_fun_xam(void);
/* $$

$subhead hes_cross_$$
If $icode%n_random_% > 0%$$ and $code record_hes_cross_done_$$,
$cref/hes_cross_row_/record_hes_cross/hes_cross_row_/$$,
$cref/hes_cross_col_/record_hes_cross/hes_cross_col_/$$, and
$cref/hes_cross_work_/record_hes_cross/hes_cross_work_/$$,
can be used to compute the cross terms in the sparse Hessian
$latex f_{u \theta}^{(2)} ( \theta , u )$$.
$codep */
	CppAD::vector<size_t>      hes_cross_row_; // corresponding row indices
	CppAD::vector<size_t>      hes_cross_col_; // corresponding column indices
	CppAD::sparse_hessian_work hes_cross_work_;
	//
	friend bool ::hes_cross_xam(void);

/* $$

$head newton_atom_$$
If $icode%n_random_% > 0%$$, MIXED_CPPAD_NEWTON is true, and
$code record_newton_atom_done_$$,
this is a CppAD atomic function that computes one Newton Step in the
solution of the equation $latex f_u ( \theta, u) = 0$$ as well
as the log of the determinant of $latex f_{uu} ( \theta , u )$$;
see $cref/initialize newton_step/newton_step/initialize/$$.
$codep */
	// computation of the Hessian as an atomic operation
	newton_step                newton_atom_;
/* $$

$subhead ranobj_fun_$$
If $icode%n_random_% > 0%$$, MIXED_CPPAD_NEWTON is true, and
$code record_ranobj_done_$$,
this is a recording of the second approximation for the
random part of the Laplace approximation, $latex H( \beta , \theta , u)$$;
see $cref/ranobj_fun_/record_ranobj/ranobj_fun_/$$.
$codep */
	CppAD::ADFun<double>    ranobj_fun_;     // for computing H_beta_beta
/* $$

$subhead hes_ranobj_$$
If $icode%n_random_% > 0%$$, MIXED_CPPAD_NEWTON is true, and
$code record_hes_ranobj_done_$$,
$cref/hes_ranobj_row_/record_hes_ranobj/hes_ranobj_row_/$$,
$cref/hes_ranobj_col_/record_hes_ranobj/hes_ranobj_col_/$$, and
$cref/hes_ranobj_work_/record_hes_ranobj/hes_ranobj_work_/$$,
can be used to compute the lower triangle of the sparse Hessian
$latex r^{(2)} ( \theta )$$.
$codep */
	CppAD::vector<size_t>      hes_ranobj_row_; // corresponding row indices
	CppAD::vector<size_t>      hes_ranobj_col_; // corresponding column indices
	CppAD::sparse_hessian_work hes_ranobj_work_;
/* $$
Note that if $code record_hes_ranobj_done_$$ is true and
$code hes_ranobj_row_.size() == 0$$, then this Hessian is zero; i.e.,
the second derivative of the Laplace approximation is zero.

$head fix_like_$$
$cref/fix_like_fun_/record_fix_like/fix_like_fun_/$$
is a recording of the fixed part of the likelihood function; see,
$cref/fix_like/mixed_cppad_fix_like/$$.
The vectors
$cref/fix_like_jac_row_/record_fix_like/fix_like_jac_row_/$$,
$cref/fix_like_jac_col_/record_fix_like/fix_like_jac_col_/$$, and
$cref/fix_like_jac_work_/record_fix_like/fix_like_jac_work_/$$,
can be used to compute the sparse Jacobian corresponding
to $code fix_like_fun_$$.
The vectors
$cref/fix_like_hes_row_/record_fix_like/fix_like_hes_row_/$$,
$cref/fix_like_hes_col_/record_fix_like/fix_like_hes_col_/$$, and
$cref/fix_like_hes_work_/record_fix_like/fix_like_hes_work_/$$,
can be used to compute the lower triangle of the
sparse Hessian corresponding to $code fix_like_fun_$$.
$codep */
	CppAD::ADFun<double>        fix_like_fun_;     // g(theta)
	//
	CppAD::vector<size_t>       fix_like_jac_row_; // row indices for g^{(1)}
	CppAD::vector<size_t>       fix_like_jac_col_; // column indices  g^{(1)}
	CppAD::sparse_jacobian_work fix_like_jac_work_;// work info for   g^{(1)}
	//
	CppAD::vector<size_t>       fix_like_hes_row_; // row indices for g^{(2)}
	CppAD::vector<size_t>       fix_like_hes_col_; // column indices  g^{(2)}
	CppAD::sparse_hessian_work  fix_like_hes_work_;// work info for   g^{(2)}
/* $$
$head constraint_fun_$$
$cref/constraint_fun_/record_constraint/constraint_fun_/$$
is a recording of the fixed part of the likelihood function; see,
$cref/constraint/mixed_cppad_constraint/$$.
The vectors
$cref/constraint_jac_row_/record_constraint/constraint_jac_row_/$$,
$cref/constraint_jac_col_/record_constraint/constraint_jac_col_/$$, and
$cref/constraint_jac_work_/record_constraint/constraint_jac_work_/$$,
can be used to compute the sparse Jacobian corresponding
to $code constraint_fun_$$.
The vectors
$cref/constraint_hes_row_/record_constraint/constraint_hes_row_/$$,
$cref/constraint_hes_col_/record_constraint/constraint_hes_col_/$$, and
$cref/constraint_hes_work_/record_constraint/constraint_hes_work_/$$,
can be used to compute the lower triangle of the
sparse Hessian corresponding to $code constraint_fun_$$.
$codep */
	// computes constraint function
	CppAD::ADFun<double>        constraint_fun_;     // c(theta)
	//
	CppAD::vector<size_t>       constraint_jac_row_; // row indices for c^{(1)}
	CppAD::vector<size_t>       constraint_jac_col_; // column indices  c^{(1)}
	CppAD::sparse_jacobian_work constraint_jac_work_;// work info for   c^{(1)}
	//
	CppAD::vector<size_t>       constraint_hes_row_; // row indices for c^{(2)}
	CppAD::vector<size_t>       constraint_hes_col_; // column indices  c^{(2)}
	CppAD::sparse_hessian_work  constraint_hes_work_;// work info for   c^{(2)}
/* $$
------------------------------------------------------------------------------
$head pack$$
See $cref mixed_cppad_pack$$.
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
See $cref mixed_cppad_unpack$$.
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
------------------------------------------------------------------------------
$head record_ran_like$$
See $cref record_ran_like$$.
$codep */
	void record_ran_like(
		const d_vector& fixed_vec ,
		const d_vector& random_vec
	);
/* $$
$head record_hes_ran$$
See $cref record_hes_ran$$.
$codep */
	void record_hes_ran(
		const d_vector& fixed_vec ,
		const d_vector& random_vec
	);
/* $$
$head record_hes_cross$$
See $cref record_hes_cross$$.
$codep */
	void record_hes_cross(
		const d_vector& fixed_vec ,
		const d_vector& random_vec
	);
/* $$
$head record_ranobj$$
See $cref record_ranobj$$.
$codep */
	void record_ranobj(
		const d_vector& fixed_vec ,
		const d_vector& random_vec
	);
/* $$
$head record_hes_ranobj$$
See $cref record_hes_ranobj$$.
$codep */
	void record_hes_ranobj(
		const d_vector& fixed_vec ,
		const d_vector& random_vec
	);
/* $$
$head record_fix_like$$
See $cref record_fix_like$$.
$codep */
	void record_fix_like(const d_vector& fixed_vec);
/* $$
$head record_constraint$$
See $cref record_constraint$$.
$codep */
	void record_constraint(const d_vector& fixed_vec);
/* $$
------------------------------------------------------------------------------
$head ranobj_eval$$
See $cref ranobj_eval$$
$codep */
	double ranobj_eval(
		const d_vector& fixed_vec  ,
		const d_vector& random_vec
	);
	friend bool ::ranobj_eval_xam(void);
/* $$
------------------------------------------------------------------------------
$head logdet_grad$$
See $cref logdet_grad$$
$codep */
	void logdet_grad(
		const d_vector& fixed_vec  ,
		const d_vector& random_vec ,
		d_vector&       logdet_fix ,
		d_vector&       logdet_ran
	);
	friend bool ::logdet_grad_xam(void);
/* $$
------------------------------------------------------------------------------
$head ranobj_grad$$
See $cref ranobj_grad$$
$codep */
	void ranobj_grad(
		const d_vector& fixed_vec  ,
		const d_vector& random_vec ,
		d_vector&       r_fixed
	);
	friend bool ::ranobj_grad_xam(void);
	friend bool ::der_var_hes(void);
	friend bool ::delta_ranobj(void);
/* $$
------------------------------------------------------------------------------
$head ran_like_grad$$
See $cref ran_like_grad$$
$codep */
	// ran_like_grad
	a1d_vector ran_like_grad(
		const a1d_vector&       fixed_vec   ,
		const a1d_vector&       random_vec
	);
	friend bool ::ran_like_grad_xam(void);
/* $$
------------------------------------------------------------------------------
$head ranobj_hes$$
See $cref ranobj_hes$$
$codep */
	// ranobj_hes
	void ranobj_hes(
		const d_vector&         fixed_vec   ,
		const d_vector&         random_vec  ,
		CppAD::vector<size_t>&  row_out     ,
		CppAD::vector<size_t>&  col_out     ,
		d_vector&               val_out
	);
	friend bool ::ranobj_hes_xam(void);
/* $$
-------------------------------------------------------------------------------
$head constraint_eval$$
See $cref constraint_eval$$
$codep */
	// constraint_eval
	d_vector constraint_eval(const d_vector& fixed_vec);
	friend bool ::constraint_eval_xam(void);
/* $$
$end
$head constraint_jac$$
See $cref constraint_jac$$
$codep */
	// constraint_jac
	void constraint_jac(
		const d_vector&        fixed_vec   ,
		CppAD::vector<size_t>& row_out     ,
		CppAD::vector<size_t>& col_out     ,
		d_vector&              val_out
	);
	friend bool ::constraint_jac_xam(void);
/* $$
$end
$head constraint_hes$$
See $cref constraint_hes$$
$codep */
	// constraint_hes
	void constraint_hes(
		const d_vector&        fixed_vec   ,
		const d_vector&        weight      ,
		CppAD::vector<size_t>& row_out     ,
		CppAD::vector<size_t>& col_out     ,
		d_vector&              val_out
	);
	friend bool ::constraint_hes_xam(void);
/* $$
-------------------------------------------------------------------------------
$head fix_like_eval$$
See $cref fix_like_eval$$
$codep */
	// fix_like_eval
	d_vector fix_like_eval(const d_vector& fixed_vec);
	friend bool ::fix_like_eval_xam(void);
/* $$
$end
$head fix_like_jac$$
See $cref fix_like_jac$$
$codep */
	// fix_like_jac
	void fix_like_jac(
		const d_vector&        fixed_vec   ,
		CppAD::vector<size_t>& row_out     ,
		CppAD::vector<size_t>& col_out     ,
		d_vector&              val_out
	);
	friend bool ::fix_like_jac_xam(void);
/* $$
$end
$head fix_like_hes$$
See $cref fix_like_hes$$
$codep */
	// fix_like_hes
	void fix_like_hes(
		const d_vector&        fixed_vec   ,
		const d_vector&        weight      ,
		CppAD::vector<size_t>& row_out     ,
		CppAD::vector<size_t>& col_out     ,
		d_vector&              val_out
	);
	friend bool ::fix_like_hes_xam(void);
/* $$
$end
-------------------------------------------------------------------------------
*/
};

} // END_DISMOD_AT_NAMESPACE

# include <dismod_at/mixed_pack.hpp>
# include <dismod_at/mixed_unpack.hpp>

# endif
