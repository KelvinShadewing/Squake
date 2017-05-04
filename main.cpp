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

int main(int argc, char** args){
	//Open the log file
	remove("log.txt");
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

	//Open Squirrel
	HSQUIRRELVM v = sq_open(1024);
    if (!v){
        xyPrint(0, "Could not open Squirrel!");
        return 0;
    };
	sq_setcompilererrorhandler(v, compileErrorHandler);

	//Compile each source nut
	for(int i = 1; i < nuts.size(); i++){
		xyPrint(0, "Compiling %s...", nuts[i].c_str());
		if (SQ_FAILED(sqstd_loadfile(v, nuts[i].c_str(), SQTrue))) {
			xyPrint(0, "Could not compile!");
			sq_close(v);
			return 0;
		};
	};

	//Output compiled code
	if (SQ_FAILED(sqstd_writeclosuretofile(v, nuts[0].c_str()))) {
        xyPrint(0, "Could not serialize closure!");
		sq_close(v);
		return 0;
    }

	//Close out
	xyPrint(0, "Compilation completed. Closing Squirrel.");
	sq_close(v);
	fclose(gvLog);
	return 0;
};