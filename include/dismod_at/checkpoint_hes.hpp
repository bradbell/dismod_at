// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_CHECKPOINT_HES_HPP
# define DISMOD_AT_CHECKPOINT_HES_HPP

# include <cppad/cppad.hpp>

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE


class checkpoint_hes_algo {
	typedef CppAD::AD<double>         a1_double;
	typedef CppAD::vector<a1_double>  a1d_vector;
private:
		CppAD::ADFun<a1_double>&          a1_adfun_;
		const CppAD::vector<a1_double>&   a1_w_;
		const CppAD::vector<size_t>&      row_;
		const CppAD::vector<size_t>&      col_;
		CppAD::sparse_hessian_work&       work_;
public:
	checkpoint_hes_algo(
		CppAD::ADFun<a1_double>&          a1_adfun  ,
		const CppAD::vector<a1_double>&   a1_w      ,
		const CppAD::vector<size_t>&      row       ,
		const CppAD::vector<size_t>&      col       ,
		CppAD::sparse_hessian_work&       work
	);
	void operator()(const a1d_vector& a1_x, a1d_vector& a1_val);
};

class checkpoint_hes {
	typedef CppAD::AD<double>         a1_double;
	typedef CppAD::vector<a1_double>  a1d_vector;
private:
	CppAD::checkpoint<double>* atom_fun_;
public:
	checkpoint_hes(void);
	~checkpoint_hes(void);
	void initialize(
		const char*                       name      ,
		CppAD::ADFun<a1_double>&          a1_adfun  ,
		const a1d_vector&                 a1_x      ,
		const CppAD::vector<a1_double>&   a1_w      ,
		const CppAD::vector<size_t>&      row       ,
		const CppAD::vector<size_t>&      col       ,
		CppAD::sparse_hessian_work&       work
	);
	void eval(const a1d_vector& a1_x, a1d_vector& a1_val);
	size_t size_var(void);
};


} // END_DISMOD_AT_NAMESPACE

# endif
