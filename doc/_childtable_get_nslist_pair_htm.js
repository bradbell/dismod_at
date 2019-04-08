// Child table for section get_nslist_pair
document.write('\
<select onchange="get_nslist_pair_child(this)">\
<option>get_nslist_pair-&gt;</option>\
<option>get_nslist_pair_xam.cpp</option>\
</select>\
');
function get_nslist_pair_child(item)
{	var child_list = [
		'get_nslist_pair_xam.cpp.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
