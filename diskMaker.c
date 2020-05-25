#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME_LENGTH 32

const char* defaultName = "default.disk";
const int defaultByteSize = 100000000;

const unsigned long int gigabyteModifier = 2147483648;
const unsigned long int megabyteModifier = 1048576;

void writeDiskFile(const char* name, unsigned long byteSize)
{
    printf("Creating Disk File...\n");

    FILE *f = fopen(name, "w");
        printf("\tOK\n");

    printf("Beginning Disk File Write...\n");

    while (byteSize > 0)
    {
        fprintf(f, "%c", 0 );
        byteSize--;
    }
        printf("\tFile Write Complete.\n");

    fclose(f);

    printf("File Closed.\n");
}

int main(int argc, char* argv[])
{
    int _length;
    unsigned long int _byteLength;
    char _fileName[MAX_NAME_LENGTH + 6] = { ' ' };

    switch (argc)
    {
        default:
        case 1: // Default command
            writeDiskFile(defaultName, defaultByteSize);

            break;
        
        case 2: // Help menu
            if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)
            {
                printf("\n\n");
                printf("diskMaker FILENAME [option] FILESIZE\n");
                printf("\toption:\n");
                printf("\t\t-b\tConvert FILESIZE to bytes (default mode)\n");
                printf("\t\t-k\tConvert FILESIZE to kilobytes\n");
                printf("\t\t-m\tConvert FILESIZE to megabytes\n");
                printf("\t\t-g\tConvert FILESIZE to gigabytes\n");
                printf("\n\n");
                printf("diskmaker\n");
                printf("\tDefault option. Creates file \"default.disk\" with FILESIZE 100MB\n");
                printf("\n\n");
                printf("diskMaker --help\n");
                printf("diskMaker -h\n");
                printf("\tDisplays this help menu");
                printf("\n\n");
            }
            else
            {
                printf("Invalid argument given.\n");
            }

            break;
        
        case 3: // Specifying file name and size
            _length = strlen(argv[1]);
            _byteLength = (unsigned long int) atoi(argv[2]);

            if (_length > MAX_NAME_LENGTH)
            {
                printf("Name given is too long.\n");
                return 0;
            }

            strcpy(_fileName, argv[1]);
            _fileName[_length + 0] = '.';
            _fileName[_length + 1] = 'd';
            _fileName[_length + 2] = 'i';
            _fileName[_length + 3] = 's';
            _fileName[_length + 4] = 'k';
            _fileName[_length + 5] = '\0';

            writeDiskFile(_fileName, _byteLength);

            break;

        case 4: // Specifying bytesize argument
            _length = strlen(argv[1]);
            _byteLength = (unsigned long int) atoi(argv[3]);
            char command = argv[2][1];

            if (_length > MAX_NAME_LENGTH)
            {
                printf("Name given is too long.\n");
                return 0;
            }

            strcpy(_fileName, argv[1]);
            _fileName[_length + 0] = '.';
            _fileName[_length + 1] = 'd';
            _fileName[_length + 2] = 'i';
            _fileName[_length + 3] = 's';
            _fileName[_length + 4] = 'k';
            _fileName[_length + 5] = '\0';

            // Remember, 1e6 bytes to 1MB
            //          1e3 bytes to 1KB
            switch (command)
            {
                case 'b': // Bytes mode
                    // We're already in bytes, so don't do anything
                    break;
                case 'k': // Kilobytes mode
                    _byteLength *= 1024;
                    break;
                case 'm': // Megabytes mode
                    _byteLength *= megabyteModifier;
                    break;
                case 'g': // Gigabytes mode
                    _byteLength *= gigabyteModifier;
                    break;
                default:
                    printf("No valid command given. Defaulting to byte mode...\n");
                    break;
            }

            writeDiskFile(_fileName, _byteLength);

            break;
    }

    return 1;
}