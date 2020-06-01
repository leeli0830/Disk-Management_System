#ifndef TABLE_H
#define TABLE_H

#include "Filesys.h"

class Table : public Filesys
{
	public :
		Table(string diskname, int blocksize, int numberofblocks, string indexfile, string flatfile);
		int Build_Table(string input_file);
		int Search(string value);

	private :
		string flatfile;
		string indexfile;
		int numberofrecords;
		int IndexSearch(string value);
};

#endif