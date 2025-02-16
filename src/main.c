#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <windows.h>
#include <time.h>

#define SECTOR_SIZE 512

void zero_out_disk(const char *disk_path) {
   HANDLE hDisk = CreateFile(disk_path, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
   if (hDisk == INVALID_HANDLE_VALUE) {
      int error = GetLastError();
      if (error == 5) 
         printf("Need to run as Root");
      else
         printf("Error opening the disk: %d\n", error);
      return;
   }

   DISK_GEOMETRY dg;
   DWORD bytesReturned;
   if (!DeviceIoControl(hDisk, IOCTL_DISK_GET_DRIVE_GEOMETRY, NULL, 0, &dg, sizeof(dg), &bytesReturned, NULL)) {
      printf("Error obtaining the disk geometry: %d\n", GetLastError());
      CloseHandle(hDisk);
      return;
   }

   LONGLONG total_sectors = (LONGLONG)dg.Cylinders.LowPart * dg.TracksPerCylinder * dg.SectorsPerTrack;
   printf("The disk has %lld sectors.\n", total_sectors);

   char zero_buffer[SECTOR_SIZE] = {0};
   DWORD bytes_written;

   for (LONGLONG i = 0; i < total_sectors; i++) {
      LARGE_INTEGER offset;
      offset.QuadPart = i * dg.BytesPerSector;
        
      if (!SetFilePointerEx(hDisk, offset, NULL, FILE_BEGIN)) {
         printf("Error moving file pointer: %d\n", GetLastError());
         break;
      }

      if (!WriteFile(hDisk, zero_buffer, SECTOR_SIZE, &bytes_written, NULL)) {
         printf("Error writing to disk: %d\n", GetLastError());
         break;
      }

      // Check if all bytes were written
      if (bytes_written != SECTOR_SIZE) {
         printf("Error: not all bytes were written!\n");
         break;
      }

      // Optional: Show progress
      if (i % 10000 == 0) {
         float percentage = (float)i/total_sectors * 100;
         printf("Progress: %f %%\n", percentage);
      }
   }

   CloseHandle(hDisk);
   printf("The disk was zeroed successfully.\n");
}


int main() {
   const char *disk_path = "\\\\.\\D:"; 
    
   printf("This process will erase all data on the disk. Are you sure? (y/n): ");
   char c = getchar();
    
   if (c == 'y' || c == 'Y') {
      zero_out_disk(disk_path);
   } else {
      printf("Operation canceled.\n");
   }

   return 0;
}