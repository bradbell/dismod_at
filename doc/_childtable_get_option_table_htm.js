// Child table for section get_option_table
document.write('\
<select onchange="get_option_table_child(this)">\
<option>get_option_table-&gt;</option>\
<option>get_option_table_xam.cpp</option>\
</select>\
');
function get_option_table_child(item)
{	var child_list = [
		'get_option_table_xam.cpp.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
