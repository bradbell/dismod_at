// Child table for section sim_data
document.write('\
<select onchange="sim_data_child(this)">\
<option>sim_data-&gt;</option>\
<option>user_sim_data.py</option>\
</select>\
');
function sim_data_child(item)
{	var child_list = [
		'user_sim_data.py.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
