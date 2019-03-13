#ifndef _FILE_READER_H_
#define _FILE_READER_H_

#include <pthread.h>
#include <stdint.h>
#include <stdio.h>

class FileReader
{
    public:
        FileReader( const char * filename, uint32_t bufSize, size_t numBufs );
        ~FileReader();

        // Returns bytes read and fills input buffer. Returns -1 on EOF.
        ssize_t read( char * out, size_t bytesToRead);

    private:
        // Main thread.
        void startThread(); 
        static void * mainThread( void * self );

        // Data members.
        FILE * m_filePtr;

        uint32_t m_bufSize;
        size_t m_numBufs;

        size_t m_curBuf; // Buf index to read from.

        bool * m_bufReady;
        char ** m_readBufs;
        char * m_readPtr;

        uint64_t m_bytesStored; 

        bool m_eofReached;

        pthread_t m_mainThread;
        bool m_running;
};

#endif