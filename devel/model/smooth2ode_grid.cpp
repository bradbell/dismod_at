

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE



smooth_info::smooth2ode_grid(
	const input_table_struct&    input         ,
	const size_t n_age_ode&      n_age_ode     ,
	const size_t n_time_ode&     n_time_ode    )
	n_age_ode_(n_age_ode),
	n_time_ode(n_time_ode),
	age_table_( input.age_table),
	time_table_( input.time_table)
{	
	// There is one smooth_info_struct per smoothing	
	size_t n_smooth = smooth_table.size();
	info_.resize(n_smooth);

	// determine the vector of age_id and time_id for each smoothing
	size_t n_smooth_grid = smooth_grid.size();
	for(size_t i = 0; i < n_smooth_grid; i++)
	{	size_t smooth_id = input.smooth_table[i].smooth_id;
		size_t age_id    = input.smooth_table[i].age_id;
		size_t time_id   = input.smooth_table[i].time_id;
		//
		size_t n_age     = info_[smooth_id].age_id.size();
	

}



} // END_DISMOD_AT_NAMESPACE
