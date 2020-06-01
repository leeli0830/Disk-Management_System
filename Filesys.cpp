#include "Filesys.h"
#include "Sdisk.h"
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <stdlib.h>

using namespace std;

Filesys::Filesys(string diskname, int numberofblocks, int blocksize) : Sdisk(diskname, numberofblocks, blocksize)
{
	// Calculate size of ROOT and FAT
	this->rootsize = blocksize / 13;
	this->fatsize = (4 * numberofblocks) / blocksize + 1;

	string buffer;

	getblock(1, buffer);

	if ( buffer[0] == '#' ) 
	{ // No file system found
		cout << "No file system found" << endl;

		// Building a root directory by initiating filename and firstblock
		for ( int i = 0; i < this->rootsize; i++) 
		{
			filename.push_back("xxxxxxxx");
			firstblock.push_back(0);
		}

		// The first space is reserved for free block     + 1
		// The second space is reserved for ROOT          + 1
		// Then FAT directory takes spaces                + fatsize
		// Then the first free block comes                = 2 + fatsieze

		// The first space is reserved for the first free block
		fat.push_back(2 + fatsize);

		// This is for ROOT directory
		fat.push_back(-1);

		// This is for FAT directory 
		for ( int i = 0; i < fatsize; i++) 
		{
			fat.push_back(-1);
		}

		// This fills the rest FAT directory
		for ( int i = fatsize + 2; i < getnumberofblocks(); i++) 
		{
			fat.push_back(i + 1);
		}

		// The last space of FAT is 0
		fat[numberofblocks - 1] = 0;

		// Write ROOT and FAT directories to the disk
		fssynch();

	} else { // File system is found -> needs to be read

			cout << "Reading existing file system...." << endl;
			
			// Read in ROOT
			string readbuffer, file; 
			int block;
			istringstream instream;
			getblock(1, readbuffer);
			instream.str(readbuffer);

			for ( int i = 0; i < rootsize; i++ ) 
			{
				instream >> file >> block;
				filename.push_back(file);
				firstblock.push_back(block);
			}

			// Read in FAT
			string tempbuffer, fatbuffer;

			for ( int i = 0; i <= fatsize; i++ ) 
			{
				getblock(i + 2, tempbuffer);
				fatbuffer += tempbuffer;
			}

			// Convert from "string" to "int" and store
			istringstream instream2;
			instream2.str(fatbuffer);
			int ToInt;

			for ( int i = 0; i < numberofblocks; i++ ) 
			{
				instream2 >> ToInt;
				fat.push_back(ToInt);
			}

			fssynch();
	}
}

int Filesys::fsclose()
{
	fssynch();
	cout << "Save ROOT and FAT in Sdisk and shut down" << endl;
	return 1; 
}

int Filesys::fssynch()
{
	// Write ROOT to disk
	string buffer;
	ostringstream outstream2;

	for ( int i = 0; i < filename.size(); i++ ) 
	{	
		outstream2 << left << setfill(' ') << setw(8) << filename[i] << " " << left << setfill(' ') << setw(3) << firstblock[i] << " ";
	}

	buffer = outstream2.str();

	vector<string>blocks = block(buffer, getblocksize());

	for ( int i = 0; i < blocks.size(); i++ ) 
	{
		putblock(i + 1, blocks[i]);
	}

	blocks.clear();
	buffer.clear();

	// Write FAT to disk 
	ostringstream outstream;

	for ( int i = 0; i < getnumberofblocks(); i++ )
	{	
		outstream << fat[i] << " ";
	}

	buffer = outstream.str();

	blocks = block(buffer, getblocksize());

	for ( int i = 0; i < blocks.size(); i++ ) 
	{
		putblock(i + 2, blocks[i]);
	}

	return 1;
}

int Filesys::newfile(string file)
{
	bool empty = false;

	// Checks if length of filename is bigger than 8
	if ( file.length() > 8 ) 
	{
		cout << "File name has to be 8 characters or less" << endl;
		return 0;
	} 
	else 
	{
		for ( int i = 0; i < rootsize; i++ ) 
		{
			// Checks if existance of file
			if ( filename[i] == file ) 
			{
				cout << file << " already exists " << endl;
				return 0;
			}
		}

		for ( int i = 0; i < rootsize; i++ ) 
		{
			// Looks for an empty space for the given file
			if ( filename[i] == "xxxxxxxx" ) 
			{
				filename[i] = file;
				firstblock[i] = 0;
				fssynch();
				empty = true;
				return 1;
			}
		}
		// If no space is available
		if ( !empty ) 
		{
			cout << "No space is available" << endl;
			return 0;
		}
	}
	return 0;
}

int Filesys::rmfile(string file)
{
	for ( int i = 0; i < rootsize; i++ ) 
	{
		// If given file matches to one of file names in ROOT
		if ( filename[i] == file ) 
		{
			// Check if the file is empty.
			// If the file is not empty
			if ( firstblock[i] != 0) 
			{
				cout << file << " is not empty" << endl;
				return 0;
			} 
			else // If the file is empty
			{
				filename[i] = "xxxxxxxx";
				fssynch();
				return 1;
			}
		}
	}
	cout << file << " does not exist " << endl;
	return 0;
}

int Filesys::getfirstblock(string file)
{
	for ( int i = 0; i < rootsize; i++ ) 
	{
		if ( filename[i] == file ) 
		{
			return firstblock[i];
		}
	}

	return 0;
}
	
int Filesys::addblock(string file, string block)
{
	int first = getfirstblock(file); // first = first block of given file
	int allocate = fat[0];			 // allocate = first free block

	// When the first block of the file is
	if ( first == -1 ) 
	{
		cout << "This block is reserved for system" << endl;
		return 0;
	}

	// No free block
	if ( allocate == 0 ) 
	{
		cout << "All blocks are full" << endl;
		return -1;
	}

	fat[0] = fat[fat[0]];
	fat[allocate] = 0;

	// When the file is emtpy
	if ( first == 0 ) 
	{
		for ( int i = 0; i < rootsize; i++ ) 
		{
			if ( filename[i] == file ) 
			{
				firstblock[i] = allocate;
				fssynch();
				putblock(allocate,block);
				return allocate;
			}
		}	
	} 
	else // When the file is not empty
	{ 
		int iblock = first;

		while ( fat[iblock] != 0 ) 
		{
			iblock = fat[iblock];
		}
		fat[iblock] = allocate;
		fssynch();
		putblock(allocate,block);
		return allocate;
	}
	return 0;
}

int Filesys::delblock(string file, int blocknumber)
{
	// When given blocknumber does not belong to the given file name
	if ( !checkblock(file,blocknumber) ) 
	{
		return 0;
	} 

	int dellocate = blocknumber;

	// Removing the first block of file
	if ( blocknumber == getfirstblock(file) ) 
	{
		for (int i = 0; i < filename.size(); i++) 
		{
			if ( blocknumber == firstblock[i] ) 
			{
				firstblock[i] = fat[blocknumber];
				break;
			}
		}
	} 
	else // If the given block is not the first block of the given file
	{ 
		int iblock = getfirstblock(file);

		while (fat[iblock] != blocknumber) 
		{
			iblock = fat[iblock];
		}
		fat[iblock] = fat[blocknumber];
	}
	fat[blocknumber] = fat[0];
	fat[0] = blocknumber;
	fssynch();

	return 0;
}

int Filesys::readblock(string file, int blocknumber, string& buffer)
{
	if ( checkblock(file,blocknumber) )
	{
		getblock(blocknumber,buffer);
		return 1;
	}
	else
	{
		cout << "Error: \"readblock\" is not availble" << endl;
		return 0;
	}
}

int Filesys::writeblock(string file, int blocknumber, string buffer) 
{
	if ( checkblock(file,blocknumber) )
	{
		putblock(blocknumber,buffer);
		fssynch();
		return 1;
	}
	else
	{
		cout << "Error: \"writeblock\" is not available" << endl;
		return 0;
	}
}

int Filesys::nextblock(string file, int blocknumber)
{
	if ( checkblock(file,blocknumber) )
	{
		return fat[blocknumber];
	}
	else
	{
		cout << "Error: \"nextblock\" is not available" << endl;
		return -1;
	}
	return 0;
}

bool Filesys::checkblock(string file, int blocknumber)
{
	int iblock = getfirstblock(file);

	while (iblock != 0) 
	{
		if (iblock == blocknumber) 
		{
			return true;
		}

		iblock = fat[iblock];
	}
	return false;
}

vector<string> Filesys::block(string buffer, int b) 
{
	vector<string>blocks;
	int numberofblocks = 0;

	if ( (buffer.size() % b) == 0) 
	{
		numberofblocks = buffer.size() / b;
	} 
	else
	{
		numberofblocks = buffer.size() / b + 1;
	}

	for ( int i = 0; i < numberofblocks; i++ ) 
	{
		blocks.push_back( buffer.substr(b*i,b) );
	}

	int lastblock = blocks.size() - 1;

	for( int i = blocks[lastblock].length(); i < b; i++ ) 
	{
		blocks[lastblock] += "#";
	}
	
	return blocks;
}

vector<string> Filesys::ls()
{
	vector<string> flist;

	for ( int i = 0; i < filename.size(); i++ )
	{
		if ( filename[i] != "xxxxxxxx" )
		{
			flist.push_back(filename[i]);
		}
	}
	return flist;
}