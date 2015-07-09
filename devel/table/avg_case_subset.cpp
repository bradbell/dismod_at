// $Id:$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin avg_case_subset$$
$spell
	covariate
	subsamples
	Subsampled
	covariates
	const
	CppAD
	struct
	obj
$$

$section Create a Subsampled Version of Average Integrand Case Table$$

$head Syntax$$
$icode%avg_case_subset_obj% = avg_case_subset(
	%avg_case_table%, %covariate_table%, %child_object%
)%$$

$head See Also$$
$cref data_subset$$

$head Limit$$
This routine subsamples the $icode avg_case_table$$, in the following way:
$list number$$
Only rows corresponding to nodes that are descendants of the
$cref/parent_node/argument_table/parent_node_id/$$ are included.
$lnext
Only rows for which the covariates satisfy the
$cref/max_difference/covariate_table/max_difference/$$ criteria
are included.
$lend

$head Covariate Reference$$
The subsampled rows are the same as the corresponding original row
except that for each covariate, its
$cref/reference/covariate_table/reference/$$ value is subtracted
from the value of the covariate in $icode avg_case_table$$.

$head avg_case_table$$
This argument has prototype
$codei%
	const CppAD::vector<avg_case_struct>& %avg_case_table%
%$$
and is the $cref/avg_case_table/get_avg_case_table/avg_case_table/$$.

$head covariate_table$$
This argument has prototype
$codei%
	const CppAD::vector<covariate_struct>& %covariate_table%
%$$
and is the $cref/covariate_table/get_covariate_table/covariate_table/$$.

$head child_object$$
This argument has prototype
$codei%
	const child_info& %child_object%
%$$

$head avg_case_subset_obj$$
The return value has prototype
$codei%
	CppAD::vector<avg_case_subset_struct> %avg_case_subset_obj%
%$$
Its size is the number of rows in $icode avg_case_table$$ that satisfy
the conditions above.
The structure has all the fields that are present in
$cref/avg_case_struct/get_avg_case_table/avg_case_struct/$$.

$subhead n_subset$$
We use the notation $icode%n_subset% = %avg_case_subset_obj%.size()%$$.

$subhead subset_id$$
We use the notation $icode subset_id$$ for an index between
zero and $icode%n_subset%-1%$$,

$subhead avg_case_id$$
There an extra field in $code avg_case_struct$$ that has
name $code avg_case_id$$, type $code int$$.
The values in this field are equal to the
$icode avg_case_id$$ for the corresponding row of $cref avg_case_table$$.
The value of
$codei%
	%avg_case_subset_obj%[%subset_id%].avg_case_id
%$$
increases with $icode subset_id$$;
i.e., for each $icode subset_id$$ less than $icode%n_subset%-2%$$,
$codei%
	%avg_case_subset_obj%[%subset_id%].avg_case_id <
		%avg_case_subset_obj%[%subset_id%+1].avg_case_id
%$$

$subhead x$$
For each $icode subset_id$$ we use
$codei%
	row(%subset_id%) =
		%avg_case_table%[ %avg_case_subset_obj%[%subset_id%].avg_case_id ]
%$$
to denote the corresponding row of $icode avg_case_table$$.
For each $cref/covariate_id/covariate_table/covariate_id/$$,
$codei%
	%avg_case_subset_obj%[%subset_id%].x[%covariate_id%] =
		row(%subset_id%).x[%covariate_id%] - reference(%covariate_id%)
%$$
where $codei%reference(%covariate_id%)%$$ is the
$cref/reference/covariate_table/reference/$$ value for the
corresponding $icode covariate_id$$.
Note that if the
$cref/max_difference/covariate_table/max_difference/$$
value is $code null$$ in the covariate table,
or the covariate value is $code null$$ in $icode avg_case_table$$,
$codei%
	%avg_case_subset_obj%[%subset_id%].x[%covariate_id%] = 0
%$$
Also note that
$codei%
	| %avg_case_subset_obj%[%subset_id%].x[%covariate_id%] | <=
		max_difference(%covariate_id%)
%$$
where $codei%max_difference(%covariate_id%)%$$ is the
maximum difference for the corresponding $icode covariate_id$$.

$end
*/

# include <cmath>
# include <dismod_at/avg_case_subset.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

CppAD::vector<avg_case_subset_struct> avg_case_subset(
	const CppAD::vector<avg_case_struct>&  avg_case_table  ,
	const CppAD::vector<covariate_struct>& covariate_table ,
	const child_info&                      child_object    )
{	CppAD::vector<avg_case_subset_struct> avg_case_subset_obj;
	if( avg_case_table.size() == 0 )
		return avg_case_subset_obj;
	//
	size_t n_child     = child_object.child_size();
	size_t n_avg_case  = avg_case_table.size();
	size_t n_covariate = avg_case_table[0].x.size();
	//
	avg_case_subset_struct one_sample;
	one_sample.x.resize(n_covariate);
	//
	for(size_t avg_case_id = 0; avg_case_id < n_avg_case; avg_case_id++)
	{	size_t child = child_object.avg_case_id2child(avg_case_id);
		// check if this avg_case is for parent or one of its descendents
		bool ok = child <= n_child;
		if( ok )
		{	for(size_t j = 0; j < n_covariate; j++)
			{	double x_j            = avg_case_table[avg_case_id].x[j];
				double reference      = covariate_table[j].reference;
				double max_difference = covariate_table[j].max_difference;
				if( std::isnan(x_j) )
					x_j = reference;
				one_sample.x[j]       = x_j - reference;
				ok  &= std::fabs( one_sample.x[j] ) <= max_difference;
			}
		}
		if( ok )
		{	one_sample.avg_case_id  = avg_case_id;
			one_sample.integrand_id = avg_case_table[avg_case_id].integrand_id;
			one_sample.node_id      = avg_case_table[avg_case_id].node_id;
			one_sample.weight_id    = avg_case_table[avg_case_id].weight_id;
			one_sample.age_lower    = avg_case_table[avg_case_id].age_lower;
			one_sample.age_upper    = avg_case_table[avg_case_id].age_upper;
			one_sample.time_lower   = avg_case_table[avg_case_id].time_lower;
			one_sample.time_upper   = avg_case_table[avg_case_id].time_upper;
			//
			avg_case_subset_obj.push_back(one_sample);
		}
	}
	return avg_case_subset_obj;
}
} // END DISMOD_AT_NAMESPACE
