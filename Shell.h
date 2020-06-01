#ifndef SHELL_H
#define SHELL_H

#include "Filesys.h"

class Shell: public Filesys
{
	public :
		Shell(string filename, int blocksize, int numberofblocks);
		int dir();// lists all files
		int add(string file);// add a new file using input from the keyboard
		int del(string file);// deletes the file
		int type(string file);//lists the contents of file
		int copy(string file1, string file2);//copies file1 to file2
		int clobber(string file);

	private :
		void clobber_helper(string file, int block);

};

#endif