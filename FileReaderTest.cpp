#include "FileReader.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main()
{
    FileReader myFileReader( "Test.txt", 1024, 2 );

    char myText[10];

    sleep(1);
    ssize_t retVal = myFileReader.read(myText, 10);
    printf( "%ld: %s\n", retVal, myText );


    /*
    FILE * pFile = fopen( "Test.txt", "w" );

    const char * myText = "HelloHelloHelloHelloHelloHelloHelloHelloHelloHelloHelloHelloHelloHelloHelloHelloHelloHello";
    size_t mySize = strlen(myText);

    for( int i = 0 ; i < 100 ; i++ )
        fwrite( myText, 1, mySize, pFile );
    */
}
