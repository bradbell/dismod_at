// Child table for section random_number
document.write('\
<select onchange="random_number_child(this)">\
<option>random_number-&gt;</option>\
<option>manage_gsl_rng</option>\
<option>sim_random</option>\
</select>\
');
function random_number_child(item)
{	var child_list = [
		'manage_gsl_rng.htm',
		'sim_random.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
