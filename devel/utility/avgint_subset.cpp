// $Id:$
// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
$begin avgint_subset$$
$spell
	avgint
	covariate
	subsamples
	Subsampled
	covariates
	const
	CppAD
	struct
	obj
	cov
$$

$section Create a Subsampled Version of Average Integrand Case Table$$

$head Syntax$$
$codei%avgint_subset(
	%avgint_table%, %avgint_cov_value%, %covariate_table%, %child_info4avgint%,
	%avgint_subset_obj, %avgint_subset_cov_value%
)%$$

$head See Also$$
$cref subset_data$$

$head Purpose$$
This routine subsamples the $icode avgint_table$$, in the following way:
$list number$$
Only rows corresponding to the
$cref/parent node/option_table/Parent Node/$$,
or a descendant of the parent node, are included.
$lnext
Only rows for which the covariates satisfy the
$cref/max_difference/covariate_table/max_difference/$$ criteria
are included.
$lnext
The subsampled rows are the same as the corresponding original row
except that for each covariate, its
$cref/reference/covariate_table/reference/$$ value is subtracted
from the value of the covariate in $icode avgint_table$$.
$lend

$head avgint_table$$
This argument has prototype
$codei%
	const CppAD::vector<avgint_struct>& %avgint_table%
%$$
and is the $cref/avgint_table/get_avgint_table/avgint_table/$$.

$head avgint_cov_value$$
This argument has prototype
$codei%
	const CppAD::vector<double>& %avgint_cov_value%
%$$
and is the $cref/avgint_table/get_avgint_table/avgint_cov_value/$$
covariate values.

$head covariate_table$$
This argument has prototype
$codei%
	const CppAD::vector<covariate_struct>& %covariate_table%
%$$
and is the $cref/covariate_table/get_covariate_table/covariate_table/$$.

$head child_info4avgint$$
This argument has prototype
$codei%
	const child_info& %child_info4avgint%
%$$

$head avgint_subset_obj$$
This argument has prototype
$codei%
	CppAD::vector<avgint_subset_struct>& %avgint_subset_obj%
%$$
Its input size is zero and upon return
its size is the number of rows in $icode avgint_table$$ that satisfy
the purpose above.
The structure has all the fields that are present in
$cref/avgint_struct/get_avgint_table/avgint_struct/$$.

$subhead n_subset$$
We use the notation $icode%n_subset% = %avgint_subset_obj%.size()%$$.

$subhead subset_id$$
We use the notation $icode subset_id$$ for an index between
zero and $icode%n_subset%-1%$$,

$subhead original_id$$
There an extra field in $code avgint_struct$$ that has
name $code original_id$$, type $code int$$.
The values in this field are equal to the
$icode avgint_id$$ for the corresponding row of $cref avgint_table$$.
The value of
$codei%
	%avgint_subset_obj%[%subset_id%].original_id
%$$
increases with $icode subset_id$$;
i.e., for each $icode subset_id$$ less than $icode%n_subset%-2%$$,
$codei%
	%avgint_subset_obj%[%subset_id%].original_id <
		%avgint_subset_obj%[%subset_id%+1].original_id
%$$

$head avgint_subset_cov_value$$
This argument has prototype
$codei%
	CppAD::vector<double>& %avgint_subset_cov_value%
%$$
Its input size is zero and upon return
its size is $icode%n_subset% * %n_covariate%$$.
For each $icode subset_id$$ and
$cref/covariate_id/covariate_table/covariate_id/$$,
$codei%
%avgint_subset_cov_value%[%subset_id% * %n_covariate% + %covariate_id%]
= %avgint_cov_value%[%original_id% * %n_covariate% + %covariate_id%]
  - reference(%covariate_id%)
%$$
where
$codei%
	%original_id% = %avgint_subset_obj%[%subset_id%].original_id
%$$
and $codei%reference(%covariate_id%)%$$ is the
$cref/reference/covariate_table/reference/$$ value for the
corresponding $icode covariate_id$$.
Note that if the
$cref/max_difference/covariate_table/max_difference/$$
value is $code null$$ in the covariate table,
or the covariate value is $code null$$ in $cref avgint_table$$,
$codei%
%avgint_subset_cov_value%[%subset_id% * %n_covariate% + %covariate_id%] = 0
%$$
Also note that
$codei%
| %avgint_subset_cov_value%[%subset_id% * %n_covariate% + %covariate_id%] |
<= max_difference(%covariate_id%)
%$$
where $codei%max_difference(%covariate_id%)%$$ is the
maximum difference for the corresponding $icode covariate_id$$.

$childtable%example/devel/utility/avgint_subset_xam.cpp
%$$
$head Example$$
The file $cref avgint_subset_xam.cpp$$ contains
and example and test of $code avgint_subset$$.
It returns true for success and false for failure.

$end
*/

# include <cmath>
# include <dismod_at/avgint_subset.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

void avgint_subset(
	const CppAD::vector<integrand_struct>& integrand_table         ,
	const CppAD::vector<avgint_struct>&    avgint_table            ,
	const CppAD::vector<double>&           avgint_cov_value        ,
	const CppAD::vector<covariate_struct>& covariate_table         ,
	const child_info&                      child_info4avgint         ,
	CppAD::vector<avgint_subset_struct>&   avgint_subset_obj       ,
	CppAD::vector<double>&                 avgint_subset_cov_value )
{	assert( avgint_subset_obj.size() == 0 );
	assert( avgint_subset_cov_value.size() == 0 );
	if( avgint_table.size() == 0 )
		return;
	//
	size_t n_child     = child_info4avgint.child_size();
	size_t n_avgint    = avgint_table.size();
	size_t n_covariate = covariate_table.size();
	//
	size_t n_subset = 0;
	CppAD::vector<bool> ok(n_avgint);
	for(size_t avgint_id = 0; avgint_id < n_avgint; avgint_id++)
	{   int integrand_id         = avgint_table[avgint_id].integrand_id;
		integrand_enum integrand = integrand_table[integrand_id].integrand;
		size_t child = child_info4avgint.table_id2child(avgint_id);
		// check if this avgint is for parent or one of its descendants
		ok[avgint_id] = (child <= n_child) || (integrand == mulcov_enum);
		if( ok[avgint_id] )
		{	for(size_t j = 0; j < n_covariate; j++)
			{	size_t index          = avgint_id * n_covariate + j;
				double x_j            = avgint_cov_value[index];
				double reference      = covariate_table[j].reference;
				double max_difference = covariate_table[j].max_difference;
				double difference     = 0.0;
				if( ! std::isnan(x_j) )
					difference = x_j - reference;
				ok[avgint_id]  &= std::fabs( difference ) <= max_difference;
			}
		}
		if( ok[avgint_id] )
			n_subset++;
	}
	//
	avgint_subset_obj.resize(n_subset);
	avgint_subset_cov_value.resize(n_subset * n_covariate);
	size_t subset_id = 0;
	for(size_t avgint_id = 0; avgint_id < n_avgint; avgint_id++)
	{	if( ok[avgint_id] )
		{	avgint_subset_struct& one_sample( avgint_subset_obj[subset_id] );
			//
			for(size_t j = 0; j < n_covariate; j++)
			{	size_t index          = avgint_id * n_covariate + j;
				double x_j            = avgint_cov_value[index];
				double reference      = covariate_table[j].reference;
				double difference     = 0.0;
				if( ! std::isnan(x_j) )
					difference = x_j - reference;
				index = subset_id * n_covariate + j;
				avgint_subset_cov_value[index] = difference;
			}
			//
			one_sample.original_id  = int( avgint_id );
			one_sample.integrand_id = avgint_table[avgint_id].integrand_id;
			one_sample.node_id      = avgint_table[avgint_id].node_id;
			one_sample.subgroup_id  = avgint_table[avgint_id].subgroup_id;
			one_sample.weight_id    = avgint_table[avgint_id].weight_id;
			one_sample.age_lower    = avgint_table[avgint_id].age_lower;
			one_sample.age_upper    = avgint_table[avgint_id].age_upper;
			one_sample.time_lower   = avgint_table[avgint_id].time_lower;
			one_sample.time_upper   = avgint_table[avgint_id].time_upper;
			//
			// advance to next sample
			subset_id++;
		}
	}
	return;
}
} // END DISMOD_AT_NAMESPACE
