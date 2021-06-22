// Child table for section command
document.write('\
<select onchange="command_child(this)">\
<option>command-&gt;</option>\
<option>dismodat.py</option>\
<option>bnd_mulcov_command</option>\
<option>depend_command</option>\
<option>fit_command</option>\
<option>hold_out_command</option>\
<option>init_command</option>\
<option>old2new_command</option>\
<option>predict_command</option>\
<option>sample_command</option>\
<option>set_command</option>\
<option>simulate_command</option>\
</select>\
');
function command_child(item)
{	var child_list = [
		'dismodat.py.htm',
		'bnd_mulcov_command.htm',
		'depend_command.htm',
		'fit_command.htm',
		'hold_out_command.htm',
		'init_command.htm',
		'old2new_command.htm',
		'predict_command.htm',
		'sample_command.htm',
		'set_command.htm',
		'simulate_command.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
