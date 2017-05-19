#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sys/stat.h>
#include <stdarg.h>
#include <squirrel.h>
#include <sqstdio.h>

using namespace std;

FILE *gvLog;
SQFILE output;

void xyPrint(HSQUIRRELVM v, const SQChar *s, ...){
	va_list args;
	va_start(args, s);
	fprintf(gvLog, ">:: ");
	vfprintf(gvLog, s, args);
	printf(">:: ");
	vprintf(s, args);
	printf("\n\n");
	fprintf(gvLog, "\n\n");
	va_end(args);
};

void compileErrorHandler(HSQUIRRELVM v, const SQChar* desc, const SQChar* source, SQInteger line, SQInteger column){
	xyPrint(0, "ERROR: %s File: %s Line: %i Column %i");
};

bool fileExists(const char* filename){
	//Checks if a file exists
	struct stat buff;
	if (stat(filename, &buff) != -1) return true;

	return false;
};


SQInteger fileWrite(SQUserPointer file,SQUserPointer p,SQInteger size){
    return sqstd_fwrite(p,1,size,(SQFILE)file);  //which basically is fwrite on a FILE* as of stdio.h
};

int main(int argc, char** args){
	//Open the log file
	remove("squake.log");
	gvLog = fopen("squake.log", "w");

	//If there is no squakefile around, end the program
	if(!fileExists("squakefile")){
		xyPrint(0, "Squakefile not found! Please make sure you are running from a directory that has one.");
		return 0;
	};

	//Open the squakefile and copy to string
	vector<string> nuts;
	string input;
	ifstream file("squakefile");
	if(!file.good()){
		xyPrint(0, "Failed to open squakefile.");
		return 0;
	};

	//Parse the input into output and nuts array
	xyPrint(0, "Squakefile contents:\n\n%s", input.c_str());
	xyPrint(0, "Parsing squakefile...");
	int i = 0;
	while(getline(file, input)){
		nuts.push_back(input);
		i++;
	};

	//Make sure the first argument is not a .nut file
	if(nuts[0].substr(nuts[0].find_last_of(".")) == ".nut"){
		xyPrint(0, "Output file is a source file. Aborting operation.");
		return 0;
	} else xyPrint(0, "Compile format: %s", nuts[0].substr(nuts[0].find_last_of(".")).c_str());

	//Open the output file
	remove(nuts[0].c_str());
	output = sqstd_fopen(nuts[0].c_str(), _SC("wb"));

	//Open Squirrel
	HSQUIRRELVM v = sq_open(1024);
    if (!v){
        xyPrint(0, "Could not open Squirrel!");
        return 0;
    };
	sq_setcompilererrorhandler(v, compileErrorHandler);

	//Clean old files
	remove("temp.sq");
	remove(nuts[0].c_str());
	
	//Compile each file
	for(int i = 1; i < nuts.size(); i++){
		//Compile the source
		xyPrint(0, "Compiling %s...", nuts[i].c_str());
		if (SQ_FAILED(sqstd_loadfile(v, nuts[i].c_str(), SQTrue))) {
			xyPrint(0, "Could not compile!");
			sq_close(v);
			return 0;
		};

		//Export to temporary bytecode
		if (SQ_FAILED(sq_writeclosure(v, fileWrite, output))) {
			xyPrint(0, "Could not serialize closure!");
			sq_close(v);
			return 0;
		};
	};

	//Close out
	xyPrint(0, "Compilation completed. Closing Squirrel.");
	sq_close(v);
	fclose(gvLog);
	sqstd_fclose(output);
	return 0;
};