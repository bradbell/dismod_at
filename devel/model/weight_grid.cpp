
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
		j = n - 1;
		while( j > i )
			vec[j] = vec[j-1];
		vec[i] = element;
	}
}

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

weight_grid::weight_grid(
	size_t                            weight_id         ,
	const vector<double>&             age_table         ,
	const vector<double>&             time_table        ,
	const vector<weight_grid_struct>& weight_grid_table )
{	size_t i, j;

	using std::cerr;
	using std::endl;
	using std::string;

	// determine the vector age_id and time_id vectors for this weight_id
	vector<size_t> age_id_vec, time_id_vec;
	size_t n_weight_grid = weight_grid_table.size();
	for( i = 0; i < n_weight_grid; i++)
	{	if( weight_grid_table[i].weight_id == weight_id )
		{	size_t age_id  = weight_grid_table[i].age_id;
			size_t time_id = weight_grid_table[i].age_id;
			unique_insert_sort( age_id_vec,  age_id );
			unique_insert_sort( time_id_vec, time_id );
		}
	}

	// set weight_id_
	weight_id_ = weight_id;

	// set age_grid_ 
	size_t n_age  = age_id_vec.size();
	age_grid_.resize(n_age);
	for(i = 0; i < n_age; i++)
		age_grid_[i] = age_table[ age_id_vec[i] ];

	// set time_grid_ 
	size_t n_time  = time_id_vec.size();
	time_grid_.resize(n_time);
	for(i = 0; i < n_time; i++)
		time_grid_[i] = time_table[ time_id_vec[i] ];

	// set weight_grid_ and count number of times each 
	// age, time pair appears for this weight_id
	vector<<size_t> count(n_age * n_time );
	weight_grid_.resize(n_age  * n_time );
	for(i = 0; i < n_age * n_time; i++)
		count[i] = 0;
	for( i = 0; i < n_weight_grid; i++)
	{	if( weight_grid_table[i].weight_id == weight_id )
		{	size_t age_id  = weight_grid_table[i].age_id;
			size_t time_id = weight_grid_table[i].age_id;
			size_t j_age = n_age;
			for(j = 0; j < n_age; j++ )
				if( age_id == age_id_vec[j] )
					j_age = j;
			assert( j_age < n_age );
			size_t j_time = n_time;
			for(j = 0; j < n_time; j++ )
				if( time_id == time_id_vec[j] )
					j_time = j;
			assert( j_time < n_time );
			size_t index = j_age * n_time + j_time;
			count[index]++;
			//
			weight_grid_[index] = weight_grid_table[i].weight;
		}
	}

	// make sure they all appear once
	for(i = 0; i < n_age * n_time; i++)
	{	if( count[i] != 1 )
		{	size_t j_time = i % n_time;
			size_t j_age  = (i - j_time) / n_time;
			cerr << "weight_grid table with weight_id = " << weight_id
			<< endl << "age_id = " << age_id_vec[j_age]
			<< ", time_id = " << time_id_vec[j_time] << " appears "
			<< count[i] << " times (not 1 time)." << endl 
		}
	}
}



} // END_DISMOD_AT_NAMESPACE
