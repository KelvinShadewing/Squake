function confirm(){
	local input;
	
	while(true){
		input = getInput();
		input = input.tolower();
		
		switch(input){
			case "yes":
			case "y":
			case "yup":
				return true;
				break;
			
			case "no":
			case "n":
			case "nah":
				return false;
				break;
			
			default:
				print("Come again? (Y/N) ");
				break;
		};
	};
};

function canint(str){
	if(typeof str == "float" || typeof str == "double" || typeof str == "bool" || typeof str == "integer") return true;
	
	if(typeof str != "string") return false;
	
	for(local i = 0; i < 10; i++){
		if(str.slice(0, 1) == i.tostring()) return true;
	};
	
	return false;
};

function parse(str){
	local currentWord = "";
	local newArray = [];
	local quoting = 0;
	
	if(canint(str)) return [str.tostring()];
	if(typeof str != "string") return [];
	
	if(str.len() == 0) return [];
	
	for(local i = 0; i < str.len(); i++){
		switch(str.slice(i, i + 1)){
			//White space
			case " ":
			case "\n":
			case "\t":
				if(currentWord != "" && quoting == 0){
					newArray.push(currentWord);
					currentWord = "";
				} else if(quoting != 0) currentWord += str.slice(i, i + 1);
				break;
			
			//Quotation
			case "\"":
				if(quoting == 0) quoting = 1;
				else if(quoting == 1) quoting = 0;
				else if(quoting == 2) currentWord += "\"";
				break;
			
			case "\'":
				if(quoting == 0) quoting = 2;
				else if(quoting == 2) quoting = 0;
				else if(quoting == 1) currentWord += "\'";
				break;
			
			//Escape characters
			case "\\":
				if(i < str.len() - 1 && quoting != 0){
					switch(str.slice(i + 1, i + 2)){
						case "\\":
							currentWord += "\\";
							i++;
							break;
						
						case "n":
							currentWord += "\n";
							i++;
							break;
						
						case "\"":
							currentWord += "\"";
							i++;
							break;
						
						case "\'":
							currentWord += "\'";
							i++;
							break;
					};
				} else if(quoting == 0){
					switch(str.slice(i + 1, i + 2)){
						case "\\":
							currentWord += "\\";
							i++;
							break;
						
						case "\"":
							currentWord += "\"";
							i++;
							break;
						
						case "\'":
							currentWord += "\'";
							i++;
							break;
					};
				};
			
			//Everything else
			default:
				currentWord += str.slice(i, i + 1);
				break;
		};
	};
	
	newArray.push(currentWord);
	
	return newArray;
};



//Variables
local quit = false;
local input = "";
local cmd = [];
local count = 0;
local name = "User";
local items = [];

function error(code){
	switch(code){
		case 0:
			print("ERROR! Wrong number of arguments!");
			break;
		
		default:
			print("An unknown error has occured!");
	};
};

//Main loop
while(!quit){
	//Get input
	print(name + ":");
	input = getInput();
	cmd = parse(input);
	
	//Process command
	if(cmd.len() == 0) continue;
	
	switch(cmd[0].tolower()){
		case "quit":
		case "exit":
			print("Are you sure you want to " + cmd[0].tolower() + "? (Y/N) ");
			if(confirm()) quit = true;
			break;
			
		case "count":
			count++;
			if(count == 1) print("You have counted 1 time.");
			else print("You have counted " + count + " times.");
			break;
		
		case "setname":
			if(cmd.len() > 1){
				name = cmd[1];
				print("Name has been set to " + name + ".");
			} else error(0);
			break;
		
		case "add":
			if(cmd.len() <= 1){
				error(0);
				break;
			} else {
				local nameFound = -1;
				for(local i = 0; i < items.len(); i++){
					if(items[i] == cmd[1]){
						nameFound = i;
						break;
					};
				};
				
				if(nameFound == -1){
					print("Added " + cmd[1] + ".");
					items.push(cmd[1]);
				} else print("ERROR: Entry " + cmd[1] + " already exists.");
			};
			break;
		
		case "delete":
			if(items.len() <= 0){
				print("ERROR: Nothing to delete.");
				break;
			};
			
			local nameFound = -1;
			for(local i = 0; i < items.len(); i++){
				if(items[i] == cmd[1]){
					nameFound = i;
					break;
				};
			};
			
			if(nameFound != -1){
				print("Deleted " + cmd[1] + ".");
				items.remove(nameFound);
			} else print("ERROR: Entry " + cmd[1] + " does not exist.");
			break;
		
		case "showlist":
			print("\n" + items.len() + " ENTRIES FOUND:\n");
			for(local i = 0; i < items.len(); i++){
				if(items[i] != "") print(items[i] + "\n");
			};
			break;
			
		case "help":
			if(cmd.len() >= 2) switch(cmd[1].tolower()){
				case "quit":
				case "exit":
					print("Closes the program.");
					break;
				
				case "help":
					print("Get information on commands. Optional argument: string");
					break;
				
				case "count":
					print("Increases a counter.");
					break;
				
				case "add":
					print("Add an item to the list. Expected argument: any");
					break;
				
				case "delete":
					print("Remove an item from the list. Expected argument: any");
					break;
				
				case "showlist":
					print("Show the list.");
					break;
				
				case "setname":
					print("Changes the user name. Expected argument: string");
					break;
				
				default:
					print("Unkown command: " + cmd[1]);
					break;
			} else print("Available commands:\nQUIT EXIT HELP\nType \'help\' followed by a command for information.");
			break;
		
		default:
				print("Unkown command: " + cmd[0]);
				break;
	};
	
	print("\n\n");
};