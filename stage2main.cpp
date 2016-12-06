// Testing Version

// Stage2 Compiler
// Michael McCarver
// Eric Sunday

#include "stage2.h"

string Stage::errorMsg = "tst";
int Stage::errorLine = 0;
int Stage::errorNumber = 0;
bool Stage::errorState = false;

// Accepts input from argv[1], genarating a listing to argv[2], and object code to argv[3]    
int main(int argc, char **argv)
{
    // Make sure the correct amount of arguments are provided via commandline
    if (argc != 4)
    {
    	cout << "usage: " << argv[0] << " <input file> <listing file name> <object code file name>" << endl;

    	return 1;
    }

    // Create instance of Stage class with our filenames
    Stage Zero(argv[1], argv[2], argv[3]);

    
    // Write out Trailer Header
    Zero.CreateListingHeader();

    // Try to parse through the input file
    try {

        Zero.Parser();

    } catch (baseException e) {
        cerr << e.type() << ": line " << e.line() << ": " << e.what() << endl;
        Zero.setErrorMessage(e);
    }

   	Zero.CreateListingTrailer();

    Zero.PrintSymbolTable();

    return 0;
}
