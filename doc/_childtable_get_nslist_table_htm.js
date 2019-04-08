// Child table for section get_nslist_table
document.write('\
<select onchange="get_nslist_table_child(this)">\
<option>get_nslist_table-&gt;</option>\
<option>get_nslist_table_xam.cpp</option>\
</select>\
');
function get_nslist_table_child(item)
{	var child_list = [
		'get_nslist_table_xam.cpp.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
