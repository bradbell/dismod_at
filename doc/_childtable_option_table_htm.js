// Child table for section option_table
document.write('\
<select onchange="option_table_child(this)">\
<option>option_table-&gt;</option>\
<option>option_default</option>\
<option>option_table.py</option>\
</select>\
');
function option_table_child(item)
{	var child_list = [
		'option_default.htm',
		'option_table.py.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
