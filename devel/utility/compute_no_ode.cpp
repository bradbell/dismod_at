
template <class Float>
Float avg_integrand::compute_no_ode(
		size_t                        data_id  ,
		const pack_var&               var_info ,
		const CppAD::vector<Float>&   var_vec
	) const
{	size_t i, j, k;
	assert( var_info.size() == var_vec.size() );

	// data table infomation for this data point
	integrand_enum integrand       = data_table_[ data_id ].integrand;
	const CppAD::vector<double>& x = data_table_[ data_id ] .x; 

	// exrtra information for this data point
	size_t i_min                   = data_info_[data_id ].i_min;
	size_t j_min                   = data_info_[data_id ].j_min;
	size_t n_age                   = data_info_[data_id ].n_age;
	size_t n_time                  = data_info_[data_id ].n_time;
	size_t child                   = data_info_[data_id ].child;
	const CppAD::vector<double>& c = data_info_[data_id ].c;

	CppAD::vector<bool> need_rate(number_rate_enum);
	for(size_t rate_id = 0; rate_id < number_rate_enum; rate_id++)
		need_rate[rate_id] = false;
	switch(integrand)
	{
		case incidence_enum:
		need_rate[iota_enum] = true;
		break;

		case remission_enum:
		need_rate[rho_enum] = true;
		break;

		case mtexcess_enum:
		need_rate[chi_enum] = true;
		break;

		case mtother_enum:
		need_rate[omega_enum] = true;
		break;

		case mtwith_enum:
		case relrisk_enum:
		need_rate[chi_enum]   = true;
		need_rate[omega_enum] = true;
		break;

		default:
		// use compute_yes_ode for these cases
		assert( false );
	}

	// ode grid indicies that we need integrand at 
	size_t n_ode = n_age * n_time;
	CppAD::vector<size_t> ode_index(n_ode);
	for(i = 0; i < n_age; i++)
	{	for(j = 0; j < n_time; j++)
		{	k = i * n_time + j;
			ode_index[k] = (i_min + i) * n_time_ode_ + j_min + j;
		}
	}
	// evaluate necessary rates at the necessary ode indices
	CppAD::vector< CppAD::vector<Float> > rate_ode(number_rate_enum);
	for(size_t rate_id = 0; rate_id < number_rate_enum; rate_id++)
	if( need_rate[rate_id] )
	{	rate_ode[rate_id].resize(n_ode);
		//
		// extract subvector infromation for this rate
		pack_vector::subvector_info info;
		info             = var_info_.rate_info(rade_id, child);	
		size_t n_var     = info.n_var;
		size_t smooth_id = info.smooth_id;
		//
		CppAD::vector<Float> rate_si(n_var);
		if( child < n_child_ )
		{	// rate is for child
			for(k = 0; k < n_var; k++)
				rate_si[k] = var_vec[info.offset + k]; 
		}
		else
		{	// rate is for parent
			for(k = 0; k < n_var; k++)
				rate_si[k] = Float(0);
			for(size_t child_id = 0; child_id < n_child_; child_id++)
			{	info = var_info_.rate_info(rade_id, child_id);	
				for(k = 0; k < n_var; k++)
					rate_si[k] += var_vec[info.offset + k];
			}
			double den = double(n_child_);
			for(k = 0; k < n_var; k++)
				rate_si[k] /= den;
		}
		//
		// interpolate onto the ode grid
		CppAD::Vector<Float> rate_ode = 
			s2ode_vec_[smooth_id].interpolate(rate_si, ode_index);
		//
		// include effect of rate covariates
		size_t n_cov = var_info_.rate_mean_mulcov_n_cov(rate_id);
		for(size_t j = 0; j < n_cov; j++)
		{	info  = var_info_.rate_mean_mulcov_info(rate_id, j);
			double x_j = x[ info.covariate_id ];

			CppAD::vector<Float> var_si(info.n_var);
			for(k = 0; k < info.n_var; k++)
				var_si[k] += var_vec[info.offset + k];
			smooth_id = info.smooth_id;
			CppAD::Vector<Float> var_ode = 
				s2ode_vec_[smooth_id].interpolate(var_si, ode_index);
			for(k = 0; k < n_ode; k++)
				rate_ode[k] *= exp( var_ode[k] * x_j )
		}
	}
	CppAD::vector<Float> var_ode(n_ode);
	switch(integrand)
	{
		case incidence_enum:
		var_ode = rate_ode[iota_enum];
		break;

		case remission_enum:
		var_ode =  rate_ode[rho_enum];
		break;

		case mtexcess_enum:
		var_ode = rate_ode[chi_enum];
		break;

		case mtother_enum:
		var_ode = rate_ode[omega_enum];
		break;

		case mtwith_enum:
		for(k = 0; k < n_ode; k++)
			var_ode[k] = rate_ode[chi_enum][k] + rate_ode[omega_enum][k];
		break;

		case relrisk_enum:
		for(k = 0; k < n_ode; k++)
		{	var_ode[k]  = 1.0;
			var_ode[k] += rate_ode[chi_enum][k] / rate_ode[omega_enum][k];
		}
		break;

		default:
		assert( false );
	}
	double sum = 0.0;
	for(k = 0; k < n_ode; k++)
		sum += c[k] * var_ode[k];
	//
	return sum;
}
