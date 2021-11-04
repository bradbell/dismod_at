#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-21 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
'''
$begin plot_data_fit$$
$spell
	pdf
	std
	dismod
	str
$$

$section Plot The Data Fit By Integrand$$

$head Syntax$$
$icode%n_point_list% = plot_data_fit(%database%, %integrand_list%, %pdf_file%)
%$$

$head database$$
This $code str$$ is the file name for
an existing dismod_at database that contains the results of a
$cref fit_command$$.


$head integrand_list$$
Each element of this $code list$$ is a $code str$$ containing an
$cref/integrand_name/integrand_table/integrand_name/$$
that we are plotting the fit for.

$head pdf_file$$
Is the location where the pdf file containing the plot will be placed.

$head n_point_list$$
The $th i$$ element of this list is the number of  rows
in the $cref data_subset_table$$ that correspond to the
$th i$$ integrand in $icode integrand_list$$.
If this is less than 2, the $th i$$ integrand is not included
in the plots.
 plotted using the point character.


$head Plot Symbol$$

$subhead Point Plots$$
Values that are within the automatically chosen plotting limits
are plotted using the point character $code .$$ .

$subhead Plus Plots$$
Values that are outside the automatically chosen plotting limits
are plotted using the point plus character $code +$$.

$subhead Green$$
Values that correspond to data that is held out in the data table
are plotted using the color green.
(Hold outs using the hold out command or hold out option are not
plotted in green.)

$subhead Red and Black$$
Values that correspond to data that is not held out,
and is not within (is within) the automatically chosen limits,
are plotted using the color red (black).


$head Example$$
$cref user_plot_data_fit.py$$.

$end
'''
# ----------------------------------------------------------------------------
import numpy
import dismod_at
from matplotlib import pyplot
import matplotlib.backends.backend_pdf
# ----------------------------------------------------------------------------
def plot_data_fit(database, integrand_list, file_name) :
	#
	# tables
	new        = False
	connection = dismod_at.create_connection(database, new)
	tables     = dict()
	for name in [
		'data',
		'data_subset',
		'integrand',
		'fit_data_subset',
	] :
		tables[name] = dismod_at.get_table_dict(connection, name)
	connection.close()
	#
	# pdf
	pdf = matplotlib.backends.backend_pdf.PdfPages(file_name)
	#
	n_point_list = list()
	for integrand_name in integrand_list :
		#
		# integrand_id
		integrand_id = None
		for (row_id, row) in enumerate(tables['integrand']) :
			if row['integrand_name'] == integrand_name :
				integrand_id = row_id
		if integrand_id is None :
			msg = f'Cannot find {integrand_name} in integrand table for '
			msg += database
			assert False, msg
		#
		# info_list
		info_list = list()
		#
		# subset_id, subset_row
		for (subset_id, subset_row) in enumerate(tables['data_subset']) :
			#
			# data_row
			data_id  = subset_row['data_id']
			data_row = tables['data'][data_id]
			#
			if data_row['integrand_id'] == integrand_id :
				#
				# meas_value
				meas_value  = data_row['meas_value']
				#
				# hold_out
				hold_out    = data_row['hold_out']
				#
				# avg_integreand, weighted_residual
				row               = tables['fit_data_subset'][subset_id]
				avg_integrand     = row['avg_integrand']
				weighted_residual = row['weighted_residual']
				#
				# info_list
				info = {
					'meas_value'        : meas_value ,
					'model'             : avg_integrand ,
					'residual'          : weighted_residual ,
					'hold_out'          : hold_out ,
				}
				info_list.append( info )
		#
		# n_point, n_point_list
		n_point = len( info_list )
		n_point_list.append( n_point )
		#
		if n_point > 1 :
			#
			# numpy_info
			keys       = info_list[0].keys()
			numpy_info = dict()
			for key in keys :
				vector = numpy.zeros(n_point, dtype=float)
				for i in range( n_point ) :
					vector[i] = info_list[i][key]
				numpy_info[key] = vector
			#
			# y_min, y_max
			d_median    = numpy.median( numpy_info['meas_value'] )
			d_max       = d_median * 1e+3
			d_min       = d_median * 1e-3
			assert d_min >= 0.0
			#
			# r_min, r_max
			r_norm      = numpy.linalg.norm( numpy_info['residual'] )
			r_avg_sq    = r_norm * r_norm / n_point
			r_max       = 4.0 * numpy.sqrt( r_avg_sq )
			r_min       = - r_max
			#
			# subplot_list
			subplot_list = [ 'meas_value', 'model', 'residual' ]
			#
			# numpy_info
			for name in [ 'meas_value', 'model' ] :
				numpy_info[name] = numpy.maximum( numpy_info[name], d_min )
				numpy_info[name] = numpy.minimum( numpy_info[name], d_max )
			for name in [ 'residual' ] :
				numpy_info[name] = numpy.maximum( numpy_info[name], r_min )
				numpy_info[name] = numpy.minimum( numpy_info[name], r_max )
			#
			# point_size, marker_size
			point_size  = numpy.array( n_point * [ 1 ] )
			marker_size = numpy.array( n_point * [ 10 ] )
			#
			# x
			x = numpy.array( range( n_point ) )
			#
			# plot title
			pyplot.title( integrand_name )
			#
			# subplot setup
			fig, axes = pyplot.subplots(3, 1, sharex=True)
			fig.subplots_adjust(hspace=0)
			#
			for subplot_index in range(3) :
				# sp
				sp   = pyplot.subplot(3, 1, subplot_index + 1)
				#
				# name, y
				name = subplot_list[subplot_index]
				y    =  numpy_info[name]
				#
				# ylabel
				pyplot.ylabel(name)
				#
				# clip_list, limit_list
				if name == 'residual' :
					clip_list  = [ r_min, r_max ]
					limit_list = [ 1.1 * r_min, 1.1 * r_max ]
				else :
					pyplot.yscale('log')
					clip_list  = [ d_min, d_max ]
					limit_list = [ 0.9 * d_min, 1.1 * d_max ]
				#
				# ylim
				pyplot.ylim(limit_list[0], limit_list[1])
				#
				# clipped, not_clipped
				clipped      = (y == clip_list[0])
				clipped      = numpy.logical_or(clipped, (y == clip_list[1]) )
				not_clipped  = numpy.logical_not(clipped)
				#
				# hold_out, not_hold_out
				hold_out     = (numpy_info['hold_out'] == 1)
				not_hold_out = numpy.logical_not(hold_out)
				#
				green_point  = numpy.logical_and(hold_out, not_clipped)
				green_marker = numpy.logical_and(hold_out, clipped)
				black_point  = numpy.logical_and(not_hold_out, not_clipped)
				red_marker   = numpy.logical_and(not_hold_out, clipped)
				#
				# plot green points
				size = point_size[green_point]
				pyplot.scatter(x[green_point], y[green_point],
					marker='.', color='green', s=size
				)
				#
				# plot green marker
				size = marker_size[green_marker]
				pyplot.scatter(x[green_marker], y[green_marker],
					marker='+', color='green', s=size
				)
				#
				# plot black points
				size = point_size[black_point]
				pyplot.scatter(x[black_point], y[black_point],
					marker='.', color='black', s=size
				)
				#
				# plot red marker
				size = marker_size[red_marker]
				pyplot.scatter(x[red_marker], y[red_marker],
					marker='+', color='red', s=size
				)
				if name == 'residual' :
					y = 0.0
					pyplot.axhline(
						y, linestyle='solid', color='black', alpha=0.3
					)
				else :
					  sp.set_xticklabels( [] )
				if subplot_index == 0 :
					pyplot.title( integrand_name )
			# x-axis label
			pyplot.xlabel('data index')
			#
			# save plot
			pdf.savefig( fig )
			pyplot.close( fig )
	# end of pages in pdf file
	pdf.close()
	return n_point_list
