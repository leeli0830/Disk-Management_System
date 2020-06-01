#include "Table.h"
#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <iomanip>

Table::Table(string diskname, int blocksize, int numberofblocks, string indexfile, string flatfile) : Filesys(diskname, blocksize, numberofblocks)
{
	this->indexfile = indexfile;
	this->flatfile = flatfile;
	newfile(flatfile);
	newfile(indexfile);
}

int Table::Build_Table(string input_file)
{
	ifstream infile;
	infile.open( input_file.c_str() );

	vector<string> key;
	vector<string> iblock;

	string record;

	int count = 0;
	
	ostringstream outstream;

	while ( getline(infile, record) )
	{   // Process record

		string pkey = record.substr(0,5);
		vector<string> oblock = block( record, getblocksize() );
		int blockid = addblock( flatfile, oblock[0] );

		outstream << pkey << " " << blockid << " ";
		count ++; 

		if ( count == 4 )
		{	// Time to addblock to indexfile

			vector<string> o2block = block( outstream.str(), getblocksize() );
			addblock(indexfile,o2block[0]);
			count = 0;
			outstream.str("");
		}
	}
	return 1;
}

int Table::Search(string value)
{
	int block = IndexSearch(value);

	if ( block == 0 ) 
	{
		cout << "No Record" << endl;
		return 0;
	}
	else
	{
		string buffer;
		readblock(flatfile, block, buffer);

		cout << buffer << endl;
		
		return 1;
	}
}

int Table::IndexSearch(string value)
{
	int blockid = getfirstblock(indexfile);

	// check if blockid == -1

	while ( blockid != 0 )
	{
		istringstream instream;
		string buffer;
		readblock(indexfile, blockid, buffer);
		string k; // k will be key and b be block
		int b;
		instream.str(buffer);

		for ( int i = 0; i <= 4; i++ )
		{
			instream >> k >> b;

			if ( k == value )
			{
				return b;
			}
		}
		blockid = nextblock(indexfile, blockid);
	}
	return 0;
}