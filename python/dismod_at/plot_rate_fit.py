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
$begin plot_rate_fit$$
$spell
	pdf
	std
	dismod
	str
$$

$section Plot The Rates for a Fit$$

$head Under Construction$$

$head Syntax$$
$icode%plot_set% = plot_rate_fit(%database%, %rate_set%, %pdf_file%)
%$$

$head database$$
This $code str$$ is the file name for
an existing dismod_at database that contains the results of a
$cref/fit/fit_command/$$.
If there is a $cref sample_table$$ it is assumed
it was created using a $cref sample_command$$ after the $cref fit_command$$.

$head rate_set$$
Each element of this $code set$$ an $code str$$ containing a
$cref/rate_name/rate_table/rate_name/$$
that we are plotting the fit for.

$head pdf_file$$
Is the location where the pdf file containing the plot will be placed.

$head plot_set$$
Each element of this $code set$$ is a $code str$$ containing
the name of a rate (in $icode rate_set$$) that was plotted.
If a rate was not plotted, it is constrained to be zero by the rate table.

$head Example$$
$cref user_plot_rate_fit.py$$.

$end
'''
# ----------------------------------------------------------------------------
import sys
import numpy
import math
import dismod_at
from matplotlib import pyplot
import matplotlib.backends.backend_pdf
# ----------------------------------------------------------------------------
def get_parent_node_id(tables) :
	parent_node_id = None
	msg = 'Cannot find parent_node_id or parent_node_name in option table'
	for option_row in tables['option'] :
		if option_row['option_name'] == 'parent_node_id' :
			parent_node_id = int( option_row['option_value'] )
		if option_row['option_name'] == 'parent_node_name' :
			parent_node_name = option_row['option_value']
			msg  = f'Cannot find parent_node_name {parent_node_name} in '
			msg += 'node table'
			for (node_id, node_row) in enumerate(tables['node']) :
				if node_row['node_name'] == parent_node_name :
					parent_node_id = node_id
	if parent_node_id is None :
		assert False, msg
	return parent_node_id
# ----------------------------------------------------------------------------
def check4table(cursor, table_name) :
	cmd     = "SELECT * FROM sqlite_master WHERE type='table' AND name="
	cmd    += "'" + table_name + "';"
	info    = cursor.execute(cmd).fetchall()
	if len(info) == 0 :
		result  = False
	else :
		assert len(info) == 1
		result = True
	return result
# ----------------------------------------------------------------------------
#
# class for compariing an (age_id, time_id) pairs
class pair:
	def __init__(self, tables, age_id, time_id) :
		self.tables  = tables
		self.age_id  = age_id
		self.time_id = time_id
	def __lt__(self, other) :
		if self.age_id != other.age_id :
			self_age   = self.tables['age'][ self.age_id ]['age']
			other_age  = self.tables['age'][ other.age_id ]['age']
			return self_age < other_age
		self_time  = self.tables['time'][ self.time_id ]['time']
		other_time = self.tables['time'][ other.time_id ]['time']
		return self_time < other_time
	def _eq_(self, other) :
		equal = self.age_id == other.age_id
		equal = equal and (self.time_id == other.time_id)
		return equla
	def __gt__(self, other) :
		return __lt__(other, self)
	def __le__(self, other) :
		return __lt__(self, other) or __eq__(self, other)
	def __ge__(self, other) :
		return __lt__(other, self) or __eq__(other, self)
	def __ne__(self, other) :
		return not __eq__(self, other)
# ----------------------------------------------------------------------------
def plot_rate_fit(database, rate_set, file_name) :
	#
	# color_style_list, n_color_style
	color_style_list = [
		('blue',       'dashed'),  ('lightblue',  'solid'),
		('red',        'dashed'),  ('pink',       'solid'),
		('green',      'dashed'),  ('lightgreen', 'solid'),
		('black',      'dashed'),  ('gray',       'solid'),
		('brown',      'dashed'),  ('sandybrown', 'solid'),
		('darkorange', 'dashed'),  ('gold',       'solid'),
		('purple',     'dashed'),  ('violet',     'solid'),
	]
	n_color_style = len( color_style_list )
	#
	# tables
	new        = False
	connection = dismod_at.create_connection(database, new)
	tables     = dict()
	for name in [
		'age',
		'fit_var',
		'node',
		'option',
		'rate',
		'smooth',
		'time',
		'var',

	] :
		tables[name] = dismod_at.get_table_dict(connection, name)
	if check4table(connection, 'sample') :
		tables['sample'] = dismod_at.get_table_dict(connection, name)
	connection.close()
	#
	# parent_node_id
	parent_node_id = get_parent_node_id(tables)
	#
	# parent_node_name
	parent_node_name = tables['node'][parent_node_id]['node_name']
	#
	# age_min, age_max
	age_min = min( [ row['age'] for row in tables['age'] ] )
	age_max = max( [ row['age'] for row in tables['age'] ] )
	#
	# time_min, time_max
	time_min = min( [ row['time'] for row in tables['time'] ] )
	time_max = max( [ row['time'] for row in tables['time'] ] )
	#
	# n_var, n_sample
	n_var    = len( tables['var'] )
	if 'sample' in tables :
		n_sample = len( tables['sample'] ) / n_var
		assert len( tables['sample'] ) % n_var == 0
	#
	# pdf
	pdf = matplotlib.backends.backend_pdf.PdfPages(file_name)
	#
	# plot_set
	plot_set = set()
	for row in tables['rate'] :
		if not row['parent_smooth_id'] is None :
			rate_name = row['rate_name']
			if rate_name in rate_set :
				plot_set.add(rate_name)
	#
	for rate_name in plot_set :
		#
		# rate_id
		rate_id = None
		for (row_id, row) in enumerate(tables['rate']) :
			if row['rate_name'] == rate_name :
				rate_id = row_id
		if rate_id is None :
			msg = f'Cannot find {rate_name} in rate table for '
			msg += database
			assert False, msg
		#
		plot_std = 'sample' in tables and rate_name != 'omega'
		#
		# triple_list
		triple_list  = list()
		smooth_id    = None
		for (var_id, row) in enumerate(tables['var']) :
			if row['var_type'] == 'rate' :
				if row['rate_id'] == rate_id :
					if  row['node_id'] == parent_node_id :
						age_id  = row['age_id']
						time_id = row['time_id']
						triple_list.append( (age_id, time_id, var_id)  )
						if smooth_id is None :
							smooth_id = row['smooth_id']
						else :
							assert smooth_id == row['smooth_id']
		assert not smooth_id is None
		#
		# n_age, n_time
		n_age    = tables['smooth'][smooth_id]['n_age']
		n_time   = tables['smooth'][smooth_id]['n_time']
		#
		# triple_list
		# sort first by age and then by time
		key = lambda triple : pair(tables, triple[0], triple[1] )
		triple_list = sorted(triple_list, key = key )
		#
		# age, time, rate, std
		# initialize these meshgrids as zero
		age  = numpy.zeros( (n_age, n_time), dtype = float)
		time = numpy.zeros( (n_age, n_time), dtype = float)
		rate = numpy.zeros( (n_age, n_time), dtype = float)
		if plot_std :
			std  = numpy.zeros( (n_age, n_time), dtype = float)
		for i in range(n_age) :
			for j in range(n_time) :
				#
				# triple corresponding to (i, j) grid point
				k       = i * n_time + j
				triple  = triple_list[k]
				#
				# age_id, time_id, var_id
				age_id  = triple[0]
				time_id = triple[1]
				var_id  = triple[2]
				#
				# age, time, rate, std for (i, j) grid point
				age[i, j]  = tables['age'][age_id]['age']
				time[i, j] = tables['time'][time_id]['time']
				rate[i, j] = tables['fit_var'][var_id]['fit_var_value']
				if  plot_std :
					sumsq = 0.0
					for k in range(n_sample) :
						sample_id = k * n_var + var_id
						var_value = sample_table[sample_id]['var_value']
						sumsq    += (var_value - rate[i, j])**2
					std[i, j] = numpy.sqrt(sumsq / n_sample)
		#
		# rate_min, rate_max, std_min, std_max
		rate_max  = numpy.max(rate) * 1.05
		rate_min  = numpy.min(rate) * 0.95
		rate_min  = max(rate_min , rate_max * 1e-6)
		if  plot_std :
			std_max   = numpy.max(std) * 1.05
			std_min   = numpy.min(std) * 0.95
			std_min   = max(std_min, std_max * 1e-5)
		#
		# plot_std
		if plot_std :
			if std_min == 0.0 :
				plot_std = False
		#
		# n_subplot
		if plot_std :
			n_subplot = 2
		else :
			n_subplot = 1
		#
		# ********************************************************************
		# for each time, plot rate and possibly std as a function of age
		# ********************************************************************
		#
		# n_fig, n_line_per_fig
		n_fig          = math.ceil( n_time / ( n_color_style - 1) )
		n_line_per_fig = math.ceil( n_time / n_fig )
		assert n_line_per_fig < n_color_style
		#
		# initialize color index
		color_index = -1
		#
		for i_fig in range( n_fig ) :
			#
			# save_color_index
			# save for possible re-use by second subplot
			save_color_index = color_index
			#
			# fig
			fig    = pyplot.figure()
			fig.subplots_adjust( hspace = .01 )
			#
			# axis
			# axis for subplot and title for figure
			axis   = pyplot.subplot(n_subplot, 1, 1)
			axis.set_title( rate_name )
			#
			# start, color_index, stop
			start  = i_fig * n_line_per_fig
			if i_fig > 0 :
				# overlap figures by one line that does not get counted
				# in value of n_line_per_fig
				start        = start - 1
				color_index -= 1
			stop   = min(n_time, start + n_line_per_fig )
			#
			for j in range(start, stop) :
				# color_index
				color_index    = (color_index + 1) % n_color_style
				#
				# color, style
				(color, style) = color_style_list[color_index]
				#
				# x, y
				x   = age[:,j]
				y   = rate[:,j]
				# avoid y values less than or equal zero
				y     = numpy.maximum(y, rate_min)
				#
				# x, y
				# extend as constant to min and max age
				x     = [age_min] + x.tolist() + [age_max]
				y     = [y[0]]    + y.tolist() + [y[-1]]
				#
				# label
				# used by legend
				label = str( time[0,j] )
				#
				# plot this line
				pyplot.plot(x, y, label=label, color=color, linestyle=style)
				#
				# axis labels
				pyplot.xlabel('age')
				pyplot.ylabel('log ' + rate_name)
				#
				# y axis sacling and limits
				pyplot.yscale('log')
				pyplot.ylim(rate_min, rate_max)
			#
			# plot age grid values as vertical lines
			for i in range(n_age) :
				x = age[i, 0]
				pyplot.axvline(x, color='black', linestyle='dotted', alpha=0.3)
			#
			# Shrink current axis by 15% and place legend to right
			box = axis.get_position()
			axis.set_position([
				box.x0 + box.width*.05 , box.y0, box.width*0.85, box.height
			])
			axis.legend(
				title = 'time', loc='center left', bbox_to_anchor=(1, 0.5)
			)
		# --------------------------------------------------------------------
		pdf.savefig( fig )
		pyplot.close( fig )
		#
		# *******************************************************************-
		# for each age, plot rate and possibly std as a function of time
		# ********************************************************************
		#
		# n_fig, n_line_per_fig
		n_fig          = math.ceil( n_age / ( n_color_style - 1) )
		n_line_per_fig = math.ceil( n_age / n_fig )
		assert n_line_per_fig < n_color_style
		#
		# initialize color index
		color_index = -1
		#
		for i_fig in range( n_fig ) :
			#
			# save_color_index
			# save for possible re-use by second subplot
			save_color_index = color_index
			#
			# fig
			fig    = pyplot.figure()
			fig.subplots_adjust( hspace = .01 )
			#
			# axis
			# axis for subplot and title for figure
			axis   = pyplot.subplot(n_subplot, 1, 1)
			axis.set_title( rate_name )
			#
			# start, color_index, stop
			start  = i_fig * n_line_per_fig
			if i_fig > 0 :
				# overlap figures by one line that does not get counted
				# in value of n_line_per_fig
				start        = start - 1
				color_index -= 1
			stop   = min(n_age, start + n_line_per_fig )
			#
			for i in range(start, stop) :
				# color_index
				color_index    = (color_index + 1) % n_color_style
				#
				# color, style
				(color, style) = color_style_list[color_index]
				#
				# x, y
				x   = time[i,:]
				y   = rate[i,:]
				# avoid y values less than or equal zero
				y     = numpy.maximum(y, rate_min)
				#
				# x, y
				# extend as constant to min and max age
				x     = [time_min] + x.tolist() + [time_max]
				y     = [y[0]]     + y.tolist() + [y[-1]]
				#
				# label
				# used by legend
				label = str( age[i,0] )
				#
				# plot this line
				pyplot.plot(x, y, label=label, color=color, linestyle=style)
				#
				# axis labels
				pyplot.xlabel('time')
				pyplot.ylabel('log ' + rate_name)
				#
				# y axis sacling and limits
				pyplot.yscale('log')
				pyplot.ylim(rate_min, rate_max)
			#
			# plot time grid values as vertical lines
			for j in range(n_time) :
				x = time[0, j]
				pyplot.axvline(x, color='black', linestyle='dotted', alpha=0.3)
			#
			# Shrink current axis by 15% and place legend to right
			box = axis.get_position()
			axis.set_position([
				box.x0 + box.width*.05 , box.y0, box.width*0.85, box.height
			])
			axis.legend(
				title = 'age', loc='center left', bbox_to_anchor=(1, 0.5)
			)
		# --------------------------------------------------------------------
		pdf.savefig( fig )
		pyplot.close( fig )
	# *************************************************************************
	#
	# end of pages in pdf file
	pdf.close()
	return plot_set
