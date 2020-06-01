#include "Shell.h"
#include "Filesys.h"
#include <string>
#include <iostream>
#include <vector>

Shell::Shell(string diskname, int blocksize, int numberofblocks) : Filesys(diskname, blocksize, numberofblocks)
{
}

int Shell::dir()
{
	vector<string> flist = ls();

	for ( int i = 0; i < flist.size(); i++ ) 
	{
		cout << flist[i] << endl;
	}
	return 0;
}

int Shell::add(string file)// add a new file using input from the keyboard
{
	string input;

	int code1 = newfile(file);

	if (code1 == -1)
	{
		cout << "No space in the ROOT directory" << endl;
		return 0;
	}
	else if ( code1 == 0 ) 
	{
		cout << "File already exists" << endl;
		return 0;
	}
	else  
	{
		cout << "Enter a string to add: ";
		getline(cin, input);
		vector<string>blocks = block(input, getblocksize());

		for ( int i = 0; i < blocks.size(); i++ )
		{
			addblock(file, blocks[i]);
		}
	}
	return 1;
}	
		
int Shell::del(string file)// deletes the file
{
	int first = getfirstblock(file);

	if ( first == -1 )
	{
		cout << "Cannot find file to delete" << endl;
		return 0;
	}

	while ( getfirstblock(file) > 0 )
	{
		delblock(file, getfirstblock(file));
	}
	rmfile(file);
	return 1;
}		

int Shell::type(string file) //lists the contents of file
{
	string buffer;

	int first = getfirstblock(file);

	if ( first == -1 )
	{
		cout << "File is not available for typing" << endl;
		return 0;
	}

	while ( first != 0 )
	{
		string b;
		readblock(file, first, b);
		buffer += b;
		first = nextblock(file, first);
	}

	cout << buffer << endl;

	return 1;
}		

int Shell::copy(string file1, string file2)//copies file1 to file2
{
	string buffer;

	int code1 = getfirstblock(file1);

	if ( code1 == 0)
	{
		cout << file1 << " is not available" << endl;
		return 0;
	}

	int code2 = getfirstblock(file2);

	if ( code2 != 0 )
	{
		cout << file2 << " already exists" << endl;
		return 0;
	}

	int code3 = newfile(file2);

	if ( code3 == 0 )
	{
		cout << "No available block on root" << endl;
		return 0;
	}

	int iblock = code1;

	while ( iblock != 0 )
	{
		string b;
		getblock(iblock, b);
		int code4 = addblock(file2, b);

		if ( code4 == -1 )
		{
			cout << "No space left" << endl;
			del(file2);
			return 0;
		}
	iblock = nextblock(file1, iblock);
	}

	return 1;
}

int Shell::clobber(string file)
{
	int iblock = getfirstblock(file);

	clobber_helper(file, iblock);

	return 0;
}

void Shell::clobber_helper(string file, int block)
{

	if ( nextblock(file, block) != 0)
	{ 	
		clobber_helper( file, nextblock(file, block) );
		delblock( file, block );
	}
	else if ( nextblock(file, block) == 0 )
	{
		delblock( file, block );
	}
}





