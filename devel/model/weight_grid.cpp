

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



smooth_info::weight_grid(
	const db_input_struct&       db_input      ,
	const size_t n_age_ode&      n_age_ode     ,
	const size_t n_time_ode&     n_time_ode    )
	n_age_ode_(n_age_ode),
	n_time_ode(n_time_ode),
	age_table_( db_input.age_table),
	time_table_( db_input.time_table)
{	using std::cerr;
	using std::endl;
	std::string table_name, message;

	// There is one smooth_info_struct per smoothing	
	size_t n_smooth = smooth_table.size();
	info_.resize(n_smooth);

	// determine the vector of age_id and time_id for each smoothing
	size_t n_smooth_grid = smooth_grid.size();
	for(size_t i = 0; i < n_smooth_grid; i++)
	{	size_t smooth_id = db_input.smooth_table[i].smooth_id;
		//
		size_t age_id    = db_input.smooth_table[i].age_id;
		size_t time_id   = db_input.smooth_table[i].time_id;
		//
		unique_insert_sort( info_[smooth_id].age_id,   age_id );
		unique_insert_sort( info_[smooth_id].time_id, time_id );
	}
	for(size_t i = 0; i < n_smooth; i++)
	{	size_t n_age = info_[i].age_id.size();
		if( n_age != db_input.smooth_table[i].n_age )
		{	cerr << "smooth_grid table has " << n_age << " age_id values"
			<< " with smooth_id = " << i << endl
			<< "This does not agree with the smooth table" << endl;
			exit(1);
		}
		size_t n_time = info_[i].time_id.size();
		if( n_time != db_input.smooth_table[i].n_time )
		{	cerr << "smooth_grid table has " << n_time << " time_id values"
			<< " with smooth_id = " << i << endl
			<< "This does not agree with the smooth table" << endl;
			exit(1);
		}
	}

	// check that the age and time grids are rectangular
	vector< vector<size_t> > count_age(n_smooth), count_time(n_smooth);
	for(size_t i = 0; i < n_smooth; i++)
	{	//
		size_t n_age = info_[i].age_id.size();
		count_age[i].resize( n_age );
		for(size_t j = 0; j < n_age; j++)
			count_age[i][j] = 0;  
		//
		size_t n_time = info_[i].time_id.size();
		count_time[i].resize( n_time );
		for(size_t j = 0; j < n_time; j++)
			count_time[i][j] = 0;  
	}
	for(size_t i = 0; i < n_smooth_grid; i++)
	{	size_t smooth_id = db_input.smooth_table[i].smooth_id;
		//
		size_t age_id    = db_input.smooth_table[i].age_id;
		size_t time_id   = db_input.smooth_table[i].time_id;
		//
		size_t j = 0;
		while( info_[smooth_id].age_id[j] != age_id )
			j++;
		count_age[smooth_id][j]++;
		//
		j = 0;
		while( info_[smooth_id].time_id[j] != time_id )
			j++;
		count_time[smooth_id][j]++;
	}
	for(size_t i = 0; i < n_smooth; i++)
	{	size_t n_age  = info_[i].age_id.size();
		size_t n_time = info_[i].time_id.size();
		for(size_t j = 0; j < n_age; j++) if( count_age[i][j] != n_time )
		{	cerr << "smooth_grid table has " << count_age[i][j] 
			<< " rows with smooth_id = " << i << "and age_id = "
			<< info_[i].age_id[j] << endl
			<< "but the number of different time_id values is " << n_time;
			exit(1);
		}
		for(size_t j = 0; j < n_time; j++) if( count_time[i][j] != n_age )
		{	cerr << "smooth_grid table has " << count_time[i][j] 
			<< " rows with smooth_id = " << i << "and time_id = "
			<< info_[i].time_id[j] << endl
			<< "but the number of different age_id values is " << n_age;
			exit(1);
		}
	}
}



} // END_DISMOD_AT_NAMESPACE
