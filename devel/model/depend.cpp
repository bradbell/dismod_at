// $Id:$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-17 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# include <dismod_at/data_model.hpp>
# include <dismod_at/prior_model.hpp>

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

// ---------------------------------------------------------------------------
// data_depend
// ---------------------------------------------------------------------------
CppAD::vector<bool> data_depend(
	const CppAD::vector<double>& pack_vec     ,
	const data_model&            data_object  )
{	typedef CppAD::AD<double>        a1_double;
	typedef CppAD::vector<a1_double> a1d_vector;
	//
	// declare a1_pack_vec to be independent variable vector
	size_t n_var = pack_vec.size();
	a1d_vector a1_pack_vec(n_var);
	for(size_t i = 0; i < n_var; i++)
		a1_pack_vec[i] = pack_vec[i];
	CppAD::Independent( a1_pack_vec );
	//
	// initialize sum of the likelihood of all the data
	a1_double a1_sum = 0.0;
	for(size_t random = 0; random < 2; random++)
	{	// compute likelihood for fixed or random effects part
		bool random_depend = bool( random );
		bool hold_out      = true;
		CppAD::vector< residual_struct<a1_double> > a1_like_vec =
			data_object.like_all(hold_out, random_depend, a1_pack_vec);
		//
		// add this part to the sum
		size_t n_like = a1_like_vec.size();
		for(size_t i = 0; i < n_like; i++)
		{	a1_sum += a1_like_vec[i].logden_smooth;
			density_enum density = a1_like_vec[i].density;
			if( density == laplace_enum || density == log_laplace_enum )
				a1_sum -= fabs( a1_like_vec[i].logden_sub_abs );
		}
	}
	//
	// declare dependent variable
	a1d_vector a1_like_sum(1);
	a1_like_sum[0] = a1_sum;
	CppAD::ADFun<double> f;
	f.Dependent(a1_pack_vec, a1_like_sum);
	//
	// 2DO: test to see if this makes release version faster
# ifndef NDEBUG
	f.optimize("no_conditional_skip");
# endif
	typedef CppAD::sparse_rc< CppAD::vector<size_t> > sparsity_pattern;
	//
	// sparsity pattern for 1 by 1 identity matrix
	size_t nr = 1, nc = 1, nnz = 1, k = 0, r = 0, c = 0;
	sparsity_pattern pattern_in(nr, nc, nnz);
	pattern_in.set(k, r, c);
	//
	// only interested in non-zero derivatives so set dependency false
	bool dependency    = false;
	// vectors of integers instead of bools for CppAD internal representation
	bool internal_bool = false;
	// no need to traspose input and output patterns
	bool transpose     = false;
	// result returned here
	sparsity_pattern pattern_out;
	// compute pattern_out
	f.rev_jac_sparsity(
		pattern_in, transpose, dependency, internal_bool, pattern_out
	);
	assert( pattern_out.nr() == 1 );
	assert( pattern_out.nc() == n_var );
	//
	// compute the dependency vector
	CppAD::vector<bool> result(n_var);
	for(size_t i = 0; i < n_var; i++)
		result[i] = false;
	for(size_t k = 0; k < pattern_out.nnz(); k++)
	{	size_t c  = pattern_out.col()[k];
		result[c] = true;
	}
	//
	return result;
}


} // END_DISMOD_AT_NAMESPACE


