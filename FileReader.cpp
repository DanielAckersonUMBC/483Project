#include "FileReader.h"
#include <unistd.h>
#include <string.h>


    FileReader::FileReader( const char * filename, uint32_t bufSize, size_t numBufs )
    : m_bufSize(bufSize), m_numBufs(numBufs), m_curBuf(0), m_bytesStored(0),
      m_eofReached(false), m_running(false)
    {
        m_filePtr = NULL;
        m_filePtr= fopen( filename, "r" );
        if( m_filePtr == NULL )
            fprintf( stderr, "Unable to open file: %s\n", filename );

        m_bufReady = new bool[m_numBufs];
        m_readBufs = new char* [m_numBufs];
        for( size_t i = 0 ;  i < m_numBufs ; ++i )
        {
            m_readBufs[i] = new char[m_bufSize];
        }
        m_readPtr = m_readBufs[0];

        startThread();

    }

    FileReader::~FileReader()
    {
        m_running = false;
        if( m_readBufs )
        {
            delete [] m_readBufs;
            m_readBufs = NULL;
        }

        if( m_bufReady )
        {
            delete [] m_bufReady;
            m_bufReady = NULL;
        }

        if( m_filePtr )
        {
            fclose( m_filePtr );
            m_filePtr = NULL;
        }
    }

    // Returns bytes read and fills input buffer.
    ssize_t FileReader::read( char * out, size_t bytesRequested )
    {
        size_t bytesToRead = 0;
        size_t bytesRead = 0;
        bool switchBuf = false;
        
        // We have no more data to read.
        if( m_bytesStored <= 0 && m_eofReached )
            return -1;

        // We do not have that many bytes stored in our buffers so we will give what we can.
        // It will be up to the caller to loop until the desired count is reached. 
        if( bytesRequested > m_bytesStored )
        {
            bytesRequested = m_bytesStored;
        }

        // Copy from buffers until we have filled the request.
        while( bytesRead < bytesRequested )
        {
            // Check to see if the request will overflow the buffer.
            if( (m_readPtr + bytesRequested) < (m_readBufs[m_curBuf] + m_bufSize) )
            {
                bytesToRead = (m_readBufs[m_curBuf] + m_bufSize) - m_readPtr;
                switchBuf = true;
            }
            else
            {
                bytesToRead = bytesRequested;
            }
            
            // Read as much as we can from current buffer.
            memcpy( out, m_readPtr, bytesToRead );
            m_readPtr += bytesToRead; // Advance the read pointer.
            bytesRead += bytesToRead;

            if( switchBuf )
            {
                switchBuf = false;
                m_bufReady[m_curBuf] = false; // Signal main thread to fill buffer.
                
                // Increment buffer index and handle wraparound.
                m_curBuf++;
                if( m_curBuf >= m_numBufs)
                {
                    m_curBuf = 0;
                }
                m_readPtr = m_readBufs[m_curBuf]; // Reset read pointer.
            }
        }
        return bytesRead;
    }

    void FileReader::startThread() 
    {
        int retVal = pthread_create( &m_mainThread, NULL, mainThread, this );
        if( retVal )
        {
            fprintf( stderr, "Error: FileReader could not create thread: %d\n", retVal );
        }
        retVal = pthread_detach( m_mainThread );
        if( retVal )
        {
            fprintf( stderr, "Error: FileReader could not detach thread: %d\n", retVal );
        }
        m_running = true;

    }

    void * FileReader::mainThread( void * self )
    {
        printf("On entry.\n");
        FileReader * thisPtr = (FileReader *) self;
        ssize_t retVal = 0;

        // Populate all the buffers possible to start.
        for( size_t i = 0 ; i < thisPtr->m_numBufs ; ++i )
        {
            retVal = fread( thisPtr->m_readBufs[i], 1, thisPtr->m_bufSize, thisPtr->m_filePtr );

            // End of file or error condition.
            if( retVal <= thisPtr->m_bufSize )
            {
                // In case we reach the end of the file durning this stage.
                if( feof( thisPtr->m_filePtr ) )
                {
                    thisPtr->m_eofReached = true;;
                }
            }

            // Add to bytes stored how much was read into this buffer and signal it is ready to read.
            if( retVal > 0 )
            {
                thisPtr->m_bytesStored += retVal;
                thisPtr->m_bufReady[i] = true;
                printf( "%lu: %s\n\n", thisPtr->m_bytesStored, thisPtr->m_readBufs[i]);
            }
        }

        // We will check if each buffer is "ready" to be read from. If not we will fill it.
        while( thisPtr->m_running )
        {
            for( size_t i = 0 ; i < thisPtr->m_numBufs ; ++i )
            {
                if( !thisPtr->m_bufReady[i] )
                {
                    retVal = fread( thisPtr->m_readBufs[i], 1, thisPtr->m_bufSize, thisPtr->m_filePtr );

                    // End of file or error condition.
                    if( retVal <= thisPtr->m_bufSize )
                    {
                        // In case we reach the end of the file durning this stage.
                        if( feof( thisPtr->m_filePtr ) )
                        {
                            thisPtr->m_eofReached = true;
                        }
                        thisPtr->m_bytesStored += retVal;
                    }
                }

            }
        } // while( m_running)
        return (void *) NULL;
    }
