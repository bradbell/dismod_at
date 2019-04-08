// Child table for section predict_command
document.write('\
<select onchange="predict_command_child(this)">\
<option>predict_command-&gt;</option>\
<option>predict_command.py</option>\
</select>\
');
function predict_command_child(item)
{	var child_list = [
		'predict_command.py.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
