#include "Sdisk.h"
#include <string>
#include <fstream>
#include <iostream>

using namespace std;

Sdisk::Sdisk(string diskname, int numberofblocks, int blocksize)
{
	this->diskname = diskname;
	this->numberofblocks = numberofblocks;
	this-> blocksize = blocksize;

	bool isGood;

	ifstream infile;
	infile.open(diskname.c_str());

	if ( infile.good() ) 
	{
		infile.seekg(0, ios::end);
		int i = infile.tellg();

		if ( i == numberofblocks * blocksize ) 
		{
			isGood = true;
		
		} 
		else 
		{
			isGood = false;
		}
	} 
	else 
	{
		isGood = false;
	}

	infile.close();

	// if file is not available 
	if ( !isGood ) 
	{
		// create a disk
		ofstream newDisk;
		cout << "Creating " << diskname << endl;
		newDisk.open(diskname.c_str(), ios::out);

		for ( int i = 0; i < ( numberofblocks * blocksize); i++ ) 
		{
			newDisk << '#';
		}

		newDisk.close();
	}
}

int Sdisk::getblock(int blocknumber, string& buffer) 
{
	if ( blocknumber > this->numberofblocks || blocknumber < 0) 
	{
		cout << "Error: blocknumber " << blocknumber << " is not valid! " << endl;

		return 0;
	} 
	else 
	{
		char c[this->blocksize];

		ifstream ifile;
		ifile.open( this->diskname.c_str() );
		ifile.seekg( blocknumber * this->blocksize );
		ifile.get( c, this->blocksize + 1 );

		string temp(c);

		buffer = temp;

		ifile.close();

		return 1;
	}
}
		
int Sdisk::putblock(int blocknumber, string buffer)
{
	if ( buffer.length() != this->blocksize || blocknumber < 0 ) 
	{
		cout << "Error: All block number must be " << this->blocksize << endl; 

		return 0;
	} 
	else 
	{
		fstream ofile;
		ofile.open( this->diskname.c_str(), ios::in | ios::out );
		ofile.seekp( this->blocksize * blocknumber );

		for( int i = 0; i < buffer.length() && i < this->blocksize; i++ ) 
		{
			ofile.put( buffer[i] );
		}
		ofile.close();

		return 1;
	}
}

int Sdisk::getnumberofblocks()
{
	return this->numberofblocks;
}
		
int Sdisk::getblocksize()
{
	return this->blocksize;
}