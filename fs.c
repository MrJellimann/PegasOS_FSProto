#include <stdio.h>
#include <stdlib.h>

#define DEFAULT_CLUSTER_SIZE 256
#define DEFAULT_DRIVE_LETTER 'A'
#define DEFAULT_FILENAME_LENGTH 32

#define DEFAULT_DFT_SIZE 65536
#define DEFAULT_FAT_SIZE 375689

#define DEFAULT_RESERVE_SIZE 4

// TODO: Move this into .h file
#ifndef bool
    #define bool _Bool
    #define true 1
    #define false 0
#endif

// MOUNT VARIABLES
// char* mountPath = "default.disk";
// char* mountPath = "sample.disk";
char* mountPath = "D:\\";
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

unsigned int bootSectorSize = DEFAULT_RESERVE_SIZE;
unsigned int fatSectorSize = 0;
unsigned int dftSectorSize = 0;
unsigned int dataSectorSize = 0;

// FAT STRUCTURES
typedef struct FileAllocation
{
    unsigned int cNum; // Cluster number
    short used;
    unsigned int next; // Basically, the offset from the drive start

    // Size in bytes: 4 + 2 + 4 = 10
} FileAllocation;

// FAT VARIABLES
unsigned int fatSectorStart = 0; // Starting cluster number
// TODO:    Make a calculation based on drive size and make
//          the DFT/FAT dynamic
// FileAllocation* fat;
FileAllocation fat[DEFAULT_FAT_SIZE] = { 0, 0, 0 };

// DIRECTORY STRUCTURES
typedef struct DirectoryFile
{
    char fName[DEFAULT_FILENAME_LENGTH];
    unsigned int cStart;
    unsigned int metadata;

    // Size in bytes: 32 + 4 + 4 = 40
    // For now: 65536 DFT entries, 2^16
} DirectoryFile;

// DIRECTORY VARIABLES
unsigned int drtSectorStart = 0; // Starting cluster number
// TODO:    Make a calculation based on drive size and
//          make the DFT/FAT dynamic
// DirectoryFile* dft;
DirectoryFile dft[DEFAULT_DFT_SIZE] = { "", 0, 0 };

// DATA STRUCTURES

// DATA VARIABLES
unsigned int totalClusters = DEFAULT_FAT_SIZE;

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

// Quick jumps to start and end of mounted drive
void gotoDriveStart()
{
    fseek(mount, 0, SEEK_SET);
    fgetpos(mount, &filePosition);
}

void gotoDriveEnd()
{
    fseek(mount, 0, SEEK_END);
    fgetpos(mount, &filePosition);
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

    gotoDriveStart();

    // First, get the length of the drive
    driveSize = 0;

    do
    {
        driveSize++;
        fgetc(mount);
    }
    while (!feof(mount));

    rewind(mount);
    fgetpos(mount, &filePosition); // Update filePosition

    // Also, set the drive letter
    // TODO: Allow user to format the drive
    driveLetter = DEFAULT_DRIVE_LETTER;

    // Next, set the cluster size information and filename length
    clusterSize = DEFAULT_CLUSTER_SIZE;
    filenameLength = DEFAULT_FILENAME_LENGTH;

    // Now, we need to calculate how much space we need for everything
    totalClusters = driveSize / clusterSize;
    availableClusters = totalClusters;

    availableClusters -= bootSectorSize;
    usedClusters += bootSectorSize;

    // DFT Calculations
    unsigned int _dftPerBlock = clusterSize / sizeof(DirectoryFile);
    unsigned int _calculatedDft = availableClusters / _dftPerBlock;

    availableClusters -= _calculatedDft;
    usedClusters += _calculatedDft;
    dftSectorSize = _calculatedDft;

    // FAT Calculations
    unsigned int _fatPerBlock = clusterSize / sizeof(FileAllocation);
    unsigned int _calculatedFat = availableClusters / _fatPerBlock;

    availableClusters -= _calculatedFat;
    usedClusters += _calculatedFat;
    fatSectorSize = _calculatedFat;

    // Send rest to data
    dataSectorSize = availableClusters;

    // Debug Printout
    printf("Debugging:\n");
    printf("Calculated Drive Size: %lu bytes\n", driveSize);
    printf("Drive Cluster Size:    %u bytes\n", clusterSize);
    printf("Total Cluster Count:   %u\n", totalClusters);
    printf("Used Clusters:         %u\n", usedClusters);
    printf("Available Clusters:    %u\n", availableClusters);
    printf("\n");
    printf("Bootstrap Clusters:    %u\t\t\t%.2f%%\n", bootSectorSize, (double)bootSectorSize / totalClusters * 100);
    printf("DFT Clusters:          %u\t\t\t%.2f%%\n", dftSectorSize, (double)dftSectorSize / totalClusters * 100);
    printf("FAT Clusters:          %u\t\t\t%.2f%%\n", fatSectorSize, (double)fatSectorSize / totalClusters * 100);
    printf("DATA Clusters:         %u\t\t\t%.2f%%\n", dataSectorSize, (double)dataSectorSize / totalClusters * 100);
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