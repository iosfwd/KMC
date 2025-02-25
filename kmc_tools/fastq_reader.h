/*
  This file is a part of KMC software distributed under GNU GPL 3 licence.
  The homepage of the KMC project is http://sun.aei.polsl.pl/kmc
  
  Authors: Marek Kokot
  
  Version: 3.2.2
  Date   : 2023-03-10
*/

#ifndef _FASTQ_READER_H
#define _FASTQ_READER_H

#include "defs.h"
#include "queues.h"
#include "config.h"
#include <stdio.h>
#include <iostream>

#include "../3rd_party/cloudflare/zlib.h"

using namespace std;


//************************************************************************************************************
// FASTA/FASTQ reader class
//************************************************************************************************************
class CFastqReaderKMCTools {
	typedef enum {m_plain, m_gzip} t_mode;

	CMemoryPool *pmm_fastq;

	string input_file_name;
	CFilteringParams::file_type file_type;
	int kmer_len;
	t_mode mode;

	FILE *in;
	gzFile_s *in_gzip;
	int bzerror;

	uint64 part_size;
	
	uchar *part;
	uint64 part_filled;
	
	uint32 gzip_buffer_size;

	bool SkipNextEOL(uchar *part, int64 &pos, int64 max_pos);
	void GetFullLineFromEnd(int64& line_sart, int64& line_end, uchar* buff, int64& pos);
	
	bool IsEof();

public:

	CFastqReaderKMCTools(CMemoryPool *_pmm_fastq, CFilteringParams::file_type _file_type, uint32 _gzip_buffer_size, int _kmer_len);
	~CFastqReaderKMCTools();

	static uint64 OVERHEAD_SIZE;

	bool SetNames(string _input_file_name);
	bool SetPartSize(uint64 _part_size);
	bool OpenFiles();
	
	bool GetPart(uchar *&_part, uint64 &_size);
};

//************************************************************************************************************
// Wrapper for FASTA/FASTQ reader class - for multithreading purposes
//************************************************************************************************************
class CWFastqReaderKMCTools { // Jarno Alanko 12.5.2022: Appended KMCTools to name to avoid name collision with CWFastqReader in KMC core
	CMemoryPool *pmm_fastq;

	CFastqReaderKMCTools *fqr;
	string file_name;
	uint64 part_size;
	CInputFilesQueue *input_files_queue;
	CPartQueue *part_queue;
	CFilteringParams::file_type file_type;
	uint32 gzip_buffer_size;
	int kmer_len;

public:
	CWFastqReaderKMCTools(CFilteringParams &Params, CFilteringQueues &Queues);
	~CWFastqReaderKMCTools();

	void operator()();
};

#endif

// ***** EOF