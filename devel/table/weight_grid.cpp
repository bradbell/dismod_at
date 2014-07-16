// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
          Copyright (C) 2014-14 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the 
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# include <dismod_at/dismod_at.hpp>

namespace {
	void unique_insert_sort(
		CppAD::vector<size_t>& vec     ,
		size_t                 element )
	{	size_t n = vec.size();
		size_t i = 0;
		while( i < n && vec[i] < element )
			i++;
		if( i == n )
		{	vec.push_back(element);
			return;
		}
		if( vec[i] == element )
			return;
		vec.push_back( vec[n-1] );
		size_t j = n - 1;
		while( j > i )
			vec[j] = vec[j-1];
		vec[i] = element;
	}
}

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

weight_grid::weight_grid(
	size_t                                   weight_id         ,
	const CppAD::vector<weight_grid_struct>& weight_grid_table )
{	size_t i, j, id;

	using std::cerr;
	using std::endl;
	using std::string;

	// determine the vector age_id and time_id vectors for this weight_id
	assert( age_id_.size() == 0 );
	assert( time_id_.size() == 0 );
	size_t n_weight = weight_grid_table.size();
	for( i = 0; i < n_weight; i++)
	{	if( weight_grid_table[i].weight_id == weight_id )
		{	id  = weight_grid_table[i].age_id;
			unique_insert_sort( age_id_,  id );
			id  = weight_grid_table[i].time_id;
			unique_insert_sort( time_id_, id );
		}
	}

	// number of age and time points for this weighting
	size_t n_age  = age_id_.size();
	size_t n_time = time_id_.size();

	// set weight_ and count number of times each 
	// age, time pair appears for this weight_id
	CppAD::vector<size_t> count(n_age * n_time );
	weight_.resize(n_age  * n_time );
	for(i = 0; i < n_age * n_time; i++)
		count[i] = 0;
	for( i = 0; i < n_weight; i++)
	{	if( weight_grid_table[i].weight_id == weight_id )
		{	id           = weight_grid_table[i].age_id;
			size_t j_age = n_age;
			for(j = 0; j < n_age; j++ )
				if( id == age_id_[j] )
					j_age = j;
			assert( j_age < n_age );
			//
			id            = weight_grid_table[i].time_id;
			size_t j_time = n_time;
			for(j = 0; j < n_time; j++ )
				if( id == time_id_[j] )
					j_time = j;
			assert( j_time < n_time );
			size_t index = j_age * n_time + j_time;
			count[index]++;
			//
			weight_[index] = weight_grid_table[i].weight;
		}
	}

	// make sure they all appear once
	for(i = 0; i < n_age * n_time; i++)
	{	if( count[i] != 1 )
		{	size_t j_time = i % n_time;
			size_t j_age  = (i - j_time) / n_time;
			cerr << "weight_grid table with weight_id = " << weight_id
			<< endl << "age_id = " << age_id_[j_age]
			<< ", time_id = " << time_id_[j_time] << " appears "
			<< count[i] << " times (not 1 time)." << endl;
		}
	}
}


} // END_DISMOD_AT_NAMESPACE
