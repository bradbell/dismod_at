# ifndef DISMOD_AT_DENSITY_HPP
# define DISMOD_AT_DENSITY_HPP

# include <utility>

namespace dismod_at { 

	template <class Float>
	Float log_gaussian(
		const Float& mu    , 
		const Float& sigma , 
		const Float& z
	);
	template <class Float>
	std::pair<Float,Float> log_gaussian(
		const Float& mu    , 
		const Float& sigma , 
		const Float& z
	);

}

# endif
