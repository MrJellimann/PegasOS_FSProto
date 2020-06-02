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

unsigned int bootSectorSize = 1;
unsigned int fatSectorSize = 0;
unsigned int dftSectorSize = 0;
unsigned int dataSectorSize = 0;

// FAT STRUCTURES
typedef struct FileAllocation
{
    unsigned int cNum; // Cluster number
    bool used;
    unsigned int next; // Basically, the offset from the drive start
} FileAllocation;

// FAT VARIABLES
unsigned int fatSectorStart = 0; // Starting cluster number
FileAllocation* fat;

// DIRECTORY STRUCTURES
typedef struct DirectoryFile
{
    char fName[128];
    unsigned int cStart;
    unsigned int metadata;
} DirectoryFile;

// DIRECTORY VARIABLES
unsigned int drtSectorStart = 0; // Starting cluster number
DirectoryFile* dft;

// DATA STRUCTURES

// DATA VARIABLES

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

// Manually set the read/write position in the mounted drive
// Also updates the global filePosition.
void setDrivePosition(int _clusterNum, int _offset)
{
    if (mount == NULL)
    {
        printf("Error 100: no mounted drive to move read/write head on.\n");
        errorCode = 100;
        return;
    }

    fseek(mount, clusterPos(_clusterNum) + _offset, SEEK_SET);
    fgetpos(mount, &filePosition);
}

// Writes a stream of bytes from the desired location onto the disk at a specific location.
// Also updates the global filePosition.
void writeBytestream(void* _bStream, int _numBytes, int _startCluster, int _offset)
{
    if (mount == NULL)
    {
        printf("Error 50: no mounted drive to write bytestream to.\n");
        errorCode = 50;
        return;
    }

    fseek(mount, clusterPos(_startCluster) + _offset, SEEK_SET);

    int _success = fwrite(_bStream, 1, _numBytes, mount);

    if (_success != _numBytes)
    {
        printf("Error 51: failed to write correct number of bytes from bytestream.\n");
        errorCode = 52;
        return;
    }

    fgetpos(mount, &filePosition);
}

// Writes a stream of bytes to the current location of the filePosition
// Also updates the global filePosition
// Must be careful with this, as it will be very easy to overwrite information
// if the read/write location is not aligned correctly.
void writeBytestreamCurrent(void* _bStream, int _numBytes)
{
    if (mount == NULL)
    {
        printf("Error 52: no mounted drive to write bytestream to.\n");
        errorCode = 52;
        return;
    }
    int _success = fwrite(_bStream, 1, _numBytes, mount);

    if (_success != _numBytes)
    {
        printf("Error 53: failed to write correct number of bytes from bytestream.\n");
        errorCode = 53;
        return;
    }

    fgetpos(mount, &filePosition);
}

// Reads in a stream of bytes into the desired location from a specific location on disk.
// Also updates the global filePosition.
void readBytestream(void* _dest, int _numBytes, int _startCluster, int _offset)
{
    if (mount == NULL)
    {
        printf("Error 54: no mounted drive to read bytestream from.\n");
        errorCode = 54;
        return;
    }

    fseek(mount, clusterPos(_startCluster) + _offset, SEEK_SET);

    int _success = fread(_dest, 1, _numBytes, mount);

    if (_success != _numBytes)
    {
        printf("Error 55: failed to read correct number of bytes from drive.\n");
        errorCode = 55;
        return;
    }

    fgetpos(mount, &filePosition);
}

// Reads a stream of bytes from the current location of the filePosition into a destination.
// Also updates the global filePosition.
// Must be careful with this, as it will be very easy to overwrite information
// if the read/write location is not aligned correctly.
void readBytestreamCurrent(void* _dest, int _numBytes)
{
    if (mount == NULL)
    {
        printf("Error 56: no mounted drive to read bytestream from.\n");
        errorCode = 56;
        return;
    }

    int _success = fread(_dest, 1, _numBytes, mount);

    if (_success != _numBytes)
    {
        printf("Error 57: failed to read correct number of bytes from drive.\n");
        errorCode = 57;
        return;
    }

    fgetpos(mount, &filePosition);
}

void createBootSector()
{
    if (mount == NULL)
    {
        printf("Error 200: no mounted drive to format.\n");
        errorCode = 200;
        return;
    }

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


    // Debug Printout
    printf("Debugging:\n");
    printf("Calculated Drive Size: %lu bytes\n", driveSize);
    printf("Drive Cluster Size: %u bytes\n", clusterSize);
}

void createFATSector()
{
    if (mount == NULL)
    {
        printf("Error 201: no mounted drive to format.\n");
        errorCode = 201;
        return;
    }

    // Initialize FAT Sector
}

void createDFTSector()
{
    if (mount == NULL)
    {
        printf("Error 202: no mounted drive to format.\n");
        errorCode = 202;
        return;
    }
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

    formatDrive();

    fclose(mount);
}