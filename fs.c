#include <stdio.h>
#include <stdlib.h>

#define DEFAULT_CLUSTER_SIZE 256
#define DEFAULT_DRIVE_LETTER 'A'
#define DEFAULT_FILENAME_LENGTH 32

// TODO: Move this into .h file
#ifndef bool
    #define bool _Bool
    #define true 1
    #define false 0
#endif

// MOUNT VARIABLES
char* mountPath = "default.disk";
FILE* mount;
fpos_t filePosition;

// GENERAL VARIABLES
int errorCode = 0;

// BOOTSECTOR VARIABLES
    // 1 block is 1 byte -> cluster is X blocks
char driveLetter = DEFAULT_DRIVE_LETTER;
long unsigned int driveSize = 0;
unsigned int clusterSize = 0;
unsigned int filenameLength = DEFAULT_FILENAME_LENGTH;

unsigned int usedClusters = 0;
unsigned int availableClusters = 0;

// FAT STRUCTURES
typedef struct FileAllocation
{
    unsigned int cNum; // Cluster number
    bool used;
    unsigned int next; // Basically, the offset from the drive start
} FileAllocation;

// FAT VARIABLES
FileAllocation* fat;

// DIRECTORY STRUCTURES
typedef struct DirectoryFile
{
    char fName[128];
    unsigned int cStart;
    unsigned int metadata;
} DirectoryFile;

// DIRECTORY VARIABLES
DirectoryFile* dft;

// Testing variables
int writePositions[12] = { 10,  20,  30,  40,  50,  99,  100, 101, 103, 70,  80,  120 };
char writeValues[12] = {   'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l' };

char sampleStream[13] = { 's', 'a', 'm', 'p', 'l', 'e', ' ', 's', 't', 'r', 'e', 'a', 'm' };
int sampleNumber = 12096;

DirectoryFile f1 = { "myDirectory", 2, 2555 };
DirectoryFile f2;

enum ERROR_CODE
{
    NONE = 0,

    FAILED_MOUNT, BOOT_MOUNT_NULL,

    NUM_ERROR_CODES
};

void mountDefault()
{
    mount = fopen(mountPath, "r+");
    fgetpos(mount, &filePosition);
    
    if (mount == NULL)
    {
        errorCode = 1;
    }
}

// Turn anything into a string so I can write it into the file


int clusterPos(int _clusterNum)
{
    return (_clusterNum * clusterSize);
}

// Writes a stream of bytes from the desired location onto the disk at a specific location.
// Also updates the global filePosition.
void writeBytestream(void* _bStream, int _numBytes, int _startCluster, int _offset)
{
    // printf("Debugging:\n");
    // printf("_bStream    -> %p\n", _bStream);
    // printf("single char -> %c\n", _bStream);
    // printf("1st char    -> %c\n", *(char *)_bStream);
    // printf("2nd char    -> %c\n", *(char *)(_bStream + 1));
    // printf("3nd char    -> %c\n", *(char *)(_bStream + 2));
    // printf("4nd char    -> %c\n", *(char *)(_bStream + 3));
    // printf("stream addr -> %p\n", &_bStream);

    fseek(mount, clusterPos(_startCluster) + _offset, SEEK_SET);

    int _success = fwrite(_bStream, 1, _numBytes, mount);

    if (_success != _numBytes)
    {
        printf("Error occured in write\n");
    }

    fgetpos(mount, &filePosition);
}

// Writes a stream of bytes to the current location of the filePosition
// Also updates the global filePosition
void writeBytestreamCurrent(void* _bStream, int _numBytes)
{
    int _success = fwrite(_bStream, 1, _numBytes, mount);

    if (_success != _numBytes)
    {
        printf("Error occured in current write\n");
    }

    fgetpos(mount, &filePosition);
}

// Reads in a stream of bytes into the desired location from a specific location on disk.
// Also updates the global filePosition.
void readBytestream(void* _dest, int _numBytes, int _startCluster, int _offset)
{
    fseek(mount, clusterPos(_startCluster) + _offset, SEEK_SET);

    int _success = fread(_dest, 1, _numBytes, mount);

    if (_success != _numBytes)
    {
        printf("Error occured in read\n");
    }

    fgetpos(mount, &filePosition);
}

// Reads a stream of bytes from the current location of the filePosition into a destination.
// Also updates the global filePosition
void readBytestreamCurrent(void* _dest, int _numBytes)
{
    int _success = fread(_dest, 1, _numBytes, mount);

    if (_success != _numBytes)
    {
        printf("Error occured in current read\n");
    }

    fgetpos(mount, &filePosition);
}

void createBootSector()
{
    // First, get the length of the drive
    driveSize = 0;

    while (!feof(mount))
    {
        driveSize++;
        fgetc(mount);
    }

    rewind(mount);
    fgetpos(mount, &filePosition); // Update filePosition

    // Next, set the cluster size information
    clusterSize = DEFAULT_CLUSTER_SIZE;


}

void createFATSector()
{
    // blah
}

void createDFTSector()
{
    // blah
}

void formatDrive()
{
    createBootSector();
    createFATSector();
    createDFTSector();
}

int main(void)
{
    mountDefault();

    // createBootSector();
    // createFATSector();

    writeBytestream(sampleStream, 13, 1, 0);
    writeBytestream(&sampleNumber, 4, 1, 20);
    writeBytestream(&f1, sizeof(DirectoryFile), 2, 0);
    readBytestream(&f2, sizeof(DirectoryFile), 2, 0);

    printf("DirectoryFile f1: %128s %i %i\n", f1.fName, f1.cStart, f1.metadata);
    printf("DirectoryFile f2: %128s %i %i\n", f2.fName, f2.cStart, f2.metadata);

    fclose(mount);
}