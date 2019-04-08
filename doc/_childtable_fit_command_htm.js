// Child table for section fit_command
document.write('\
<select onchange="fit_command_child(this)">\
<option>fit_command-&gt;</option>\
<option>fit_command.py</option>\
</select>\
');
function fit_command_child(item)
{	var child_list = [
		'fit_command.py.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
