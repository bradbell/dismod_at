/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-21 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin ran_con_rcv$$
$spell
	mulcov
	rcv
	enum
$$

$section Matrix Representation of Random Constraints$$

$head Syntax$$
$icode%A_rcv% = ran_con_rcv(
%bound_random%, %zero_sum_child_rate%, %zero_sum_mulcov_group%, %pack_object%
)%$$

$head Prototype$$
$srcthisfile%0%// BEGIN_RAN_CON_RCV%// END_RAN_CON_RCV%1
%$$

$head bound_random$$
This is the value of the
$cref/bound_random/option_table/Optimize Random Only/bound_random/$$
in the option table.

$head zero_sum_child_rate$$
If this vector has size $code number_rate_enum$$.
If $icode%zero_sum_child_rate%[%rate_id%]%$$ is true,
for each age, time, and rate,
the sum of the random effects with respect the children
is constrained to be zero.

$head zero_sum_mulcov_group$$
If this vector has size equal to the number of groups in
$cref subgroup_table$$.
If $icode%zero_sum_mulcov_group%[%group_id%]%$$ is true,
for each age, time, and $cref mulcov_table$$ row,
the sum of the random effects with respect to subgroup
is constrained to be zero.

$head pack_object$$
This argument is the $cref pack_info$$ information corresponding to the
$cref model_variables$$.

$head A_rcv$$
The return value $icode A_rcv$$ is a sparse matrix representation of
the constraints on the random effects.
To be specific, for rate that has random effects,
and each grid point in the smoothing for those random effects,
the matrix has a row representing the constraint for that
grid point and rate.
The columns of the matrix are zero except for those corresponding to the
grid point which are one.

$end
*/

# include <dismod_at/ran_con_rcv.hpp>
# include <dismod_at/null_int.hpp>
# include <dismod_at/random_effect.hpp>
# include <dismod_at/error_exit.hpp>

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE
// BEGIN_RAN_CON_RCV
CppAD::mixed::d_sparse_rcv ran_con_rcv(
	double                        bound_random          ,
	const CppAD::vector<bool>&    zero_sum_child_rate   ,
	const CppAD::vector<bool>&    zero_sum_mulcov_group ,
	const dismod_at::pack_info&   pack_object           ,
	const remove_const&           random_const          )
// END_RAN_CON_RCV
{	// number of fixed plus random effects
	size_t n_var = pack_object.size();
	//
	// number of random effects
	size_t n_random = pack_object.random_size();
	//
	// number of rates
	size_t n_rate = dismod_at::number_rate_enum;
	//
	// var_id2random
	CppAD::vector<size_t> var_id2random(n_var);
	for(size_t k = 0; k < n_var; k++)
		var_id2random[k] = n_random;
	//
	CppAD::vector<size_t> pack_index = random2var_id(pack_object);
	for(size_t j = 0; j < n_random; j++)
		var_id2random[ pack_index[j] ] = j;
	//
	// n_child
	size_t n_child = pack_object.child_size();
	//
	// n_group
	size_t n_group = pack_object.group_size();
	//
	// check for first case where random constraint matrix is empty
	CppAD::mixed::d_sparse_rcv A_rcv;
	if( ( n_child == 0 && n_group == 0 ) || bound_random == 0 )
		return A_rcv;
	//
	// initilaize count of number of random constraint equations
	size_t A_nr  = 0;
	//
	// initialize count of number of non-zeros in random constraint matrix
	size_t A_nnz = 0;
	//
	// for each rate
	for(size_t rate_id = 0; rate_id < n_rate; rate_id++)
	{	// -------------------------------------------------------------------
		// zero_sum_child_rate
		if( zero_sum_child_rate[rate_id] && pack_object.child_size() > 0 )
		{	// packing information for first child
			dismod_at::pack_info::subvec_info
				info_0 = pack_object.node_rate_value_info(rate_id, 0);
			// child smoothing id for this rate
			size_t smooth_id = info_0.smooth_id;
			if( smooth_id != DISMOD_AT_NULL_SIZE_T )
			{	// number of grid points for child soothing for this rate
				// all child rate effects have the same smoothing, in addition
				// the lower and upper limits for the smoothing are infinite
				// (Hence not equal so they are random effects in cppad_mixed)
				size_t n_grid = info_0.n_var;
				assert( n_grid > 0 );
				// each grid point corresponds to a random constraint equation
				A_nr  += n_grid;
				A_nnz += n_grid * n_child;
			}
		}
		// --------------------------------------------------------------------
		// zero sum subgroup_rate_value covariate multipliers
		size_t n_cov = pack_object.subgroup_rate_value_n_cov(rate_id);
		for(size_t j = 0; j < n_cov; ++j)
		{	// packing information for first subgroup
			dismod_at::pack_info::subvec_info
				info_0 = pack_object.subgroup_rate_value_info(rate_id, j, 0);
			assert( info_0.smooth_id != DISMOD_AT_NULL_SIZE_T );
			if( zero_sum_mulcov_group[info_0.group_id] )
			{	size_t n_grid = info_0.n_var;
				assert( n_grid > 0 );
				size_t n_sub  =
					pack_object.subgroup_rate_value_n_sub(rate_id, j);
				assert( n_sub > 0 );
				// each grid point corresponds to a random constraint equation
				A_nr  += n_grid;
				A_nnz += n_grid * n_sub;
			}
		}
	}
	// -----------------------------------------------------------------------
	// zero sum subgroup_meas_value covariate multipliers
	size_t n_integrand = pack_object.integrand_size();
	for(size_t integrand_id = 0; integrand_id < n_integrand; ++integrand_id)
	{	size_t n_cov = pack_object.subgroup_meas_value_n_cov(integrand_id);
		for(size_t j = 0; j < n_cov; ++j)
		{	// packing information for first subgroup
			dismod_at::pack_info::subvec_info info_0 =
				pack_object.subgroup_meas_value_info(integrand_id, j, 0);
			//
			assert( info_0.smooth_id != DISMOD_AT_NULL_SIZE_T );
			if( zero_sum_mulcov_group[info_0.group_id] )
			{	size_t n_grid = info_0.n_var;
				assert( n_grid > 0 );
				size_t n_sub  =
					pack_object.subgroup_meas_value_n_sub(integrand_id, j);
				assert( n_sub > 0 );
				// each grid point corresponds to a random constraint equation
				A_nr  += n_grid;
				A_nnz += n_grid * n_sub;
			}
		}

	}
	//
	// check for second case where random constraint matrix is empty
	if( A_nr == 0 )
		return A_rcv;
	//
	// Mapping from index in random vector to index in subset that
	// is not constant
	const CppAD::vector<size_t>& var_index = random_const.both2var_index();
	//
	// number of columns in random constraint equations
	size_t A_nc = random_const.n_var();
	//
	// check for more constraints than random effects
	if( A_nr > A_nc )
	{	std::string msg = "More random constraints than random effects";
		error_exit(msg);
	}
	//
	// sparsity pattern for random constraints
	CppAD::mixed::sparse_rc A_rc(A_nr, A_nc, A_nnz);
	//
	// initialize index of next set of rows and next non-zero entry
	size_t row_index = 0;
	size_t nnz_index = 0;
	//
	// for each rate
	for(size_t rate_id = 0; rate_id < n_rate; rate_id++)
	{	// ------------------------------------------------------------------
		// zero_sum_child_rate
		if( zero_sum_child_rate[rate_id] )
		{	// packing information for first child
			dismod_at::pack_info::subvec_info
				info_0 = pack_object.node_rate_value_info(rate_id, 0);
			//
			// child smoothing id for this rate
			if( info_0.smooth_id != DISMOD_AT_NULL_SIZE_T )
			{	//
				// number of grid points same for all children
				size_t n_grid = info_0.n_var;
				//
				// for each child
				for(size_t j = 0; j < n_child; j++)
				{	// packing information for this child
					dismod_at::pack_info::subvec_info
						info_j = pack_object.node_rate_value_info(rate_id, j);
					//
					// for each rate, all children have the same smoothing
					assert( info_j.smooth_id == info_0.smooth_id );
					assert( info_j.n_var     == info_0.n_var );
					//
					// offset for this child
					size_t offset = info_j.offset;
					//
					// for each grid point in this smoothing
					for(size_t k = 0; k < n_grid; k++)
					{	// variable index for this grid point
						size_t var_id = offset + k;
						//
						// corresponding random effect index
						size_t random_index = var_id2random[var_id];
						assert( random_index < n_random );
						//
						// corresponding index in vector with constant
						// random effects removed
						random_index = var_index[ random_index ];
						//
						// check that lower and upper limits were not equal
						assert( random_index < n_random );
						//
						// entry for this grid point k, child j, rate rate_id
						A_rc.set(nnz_index++, row_index + k, random_index);
					}
				}
				row_index += n_grid;
			}
		}
		// --------------------------------------------------------------------
		// zero sum subgroup_rate_value covariate multipliers
		size_t n_cov = pack_object.subgroup_rate_value_n_cov(rate_id);
		for(size_t j = 0; j < n_cov; ++j)
		{	// packing information corresponding to first subgroup
			dismod_at::pack_info::subvec_info
				info_0 = pack_object.subgroup_rate_value_info(rate_id, j, 0);
			if( zero_sum_mulcov_group[info_0.group_id] )
			{	assert( info_0.smooth_id != DISMOD_AT_NULL_SIZE_T );
				//
				// number of grid points same for all subgroups
				size_t n_grid = info_0.n_var;
				//
				// number of subgroups
				size_t n_sub =
					pack_object.subgroup_rate_value_n_sub(rate_id, j);
				//
				// for each subgroup
				for(size_t k = 0; k < n_sub; ++k)
				{	// packing information for this child
					dismod_at::pack_info::subvec_info info_k =
						pack_object.subgroup_rate_value_info(rate_id, j, k);
					//
					// subgroups have same smoothing
					assert( info_k.smooth_id == info_0.smooth_id );
					assert( info_k.n_var     == info_0.n_var );
					//
					// offset for this subgroup
					size_t offset = info_k.offset;
					//
					// for each grid point in this smoothing
					for(size_t ell = 0; ell < n_grid; ++ell)
					{	// variable index for this grid point
						size_t var_id = offset + ell;
						//
						// corresponding index in vector with constant
						// random effects removed
						size_t random_index = var_id2random[var_id];
						//
						// check that lower and upper limits were not equal
						assert( random_index < n_random );
						//
						// entry for this grid point ell, subgroup k, rate
						A_rc.set(nnz_index++, row_index + ell, random_index);
					}
				}
				row_index += n_grid;
			}
		}
	}
	// --------------------------------------------------------------------
	// zero sum subgroup_meas_value covariate multipliers
	for(size_t integrand_id = 0; integrand_id < n_integrand; ++integrand_id)
	{	size_t n_cov = pack_object.subgroup_meas_value_n_cov(integrand_id);
		for(size_t j = 0; j < n_cov; ++j)
		{	// packing information corresponding to first subgroup
			dismod_at::pack_info::subvec_info info_0 =
				pack_object.subgroup_meas_value_info(integrand_id, j, 0);
			if( zero_sum_mulcov_group[info_0.group_id] )
			{	assert( info_0.smooth_id != DISMOD_AT_NULL_SIZE_T );
				//
				// number of grid points same for all subgroups
				size_t n_grid = info_0.n_var;
				//
				// number of subgroups
				size_t n_sub =
					pack_object.subgroup_meas_value_n_sub(integrand_id, j);
				//
				// for each subgroup
				for(size_t k = 0; k < n_sub; ++k)
				{	// packing information for this child
					dismod_at::pack_info::subvec_info info_k =
					pack_object.subgroup_meas_value_info(integrand_id, j, k);
					//
					// subgroups have same smoothing
					assert( info_k.smooth_id == info_0.smooth_id );
					assert( info_k.n_var     == info_0.n_var );
					//
					// offset for this subgroup
					size_t offset = info_k.offset;
					//
					// for each grid point in this smoothing
					for(size_t ell = 0; ell < n_grid; ++ell)
					{	// variable index for this grid point
						size_t var_id = offset + ell;
						//
						// corresponding index in vector with constant
						// random effects removed
						size_t random_index = var_id2random[var_id];
						//
						// check that lower and upper limits were not equal
						assert( random_index < n_random );
						//
						// entry for this grid point ell, subgroup k, rate
						A_rc.set(nnz_index++, row_index + ell, random_index);
					}
				}
				row_index += n_grid;
			}
		}
	}
	assert( row_index == A_nr );
	assert( nnz_index == A_nnz );
	//
	// non-empty value of A_rcv
	A_rcv = CppAD::mixed::d_sparse_rcv( A_rc );
	for(size_t k = 0; k < A_nnz; k++)
		A_rcv.set(k, 1.0);
	//
	return A_rcv;
}
} /// END_DISMOD_AT_NAMESPACE
