// Child table for section sim_random
document.write('\
<select onchange="sim_random_child(this)">\
<option>sim_random-&gt;</option>\
<option>sim_random_xam.cpp</option>\
</select>\
');
function sim_random_child(item)
{	var child_list = [
		'sim_random_xam.cpp.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
