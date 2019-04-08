// Child table for section get_sample_table
document.write('\
<select onchange="get_sample_table_child(this)">\
<option>get_sample_table-&gt;</option>\
<option>get_sample_table_xam.cpp</option>\
</select>\
');
function get_sample_table_child(item)
{	var child_list = [
		'get_sample_table_xam.cpp.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
