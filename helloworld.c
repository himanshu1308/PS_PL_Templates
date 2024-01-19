#include <stdio.h>

#include "custom_mem_myip.h" // Header file of the created ip
#include "ff.h" // Header file for fat filesystem
#include "xil_io.h" // Header file for AXI communication
#include "xil_types.h" // Header file for datatypes
#include "xparameters.h"
#include "myip_buffer.h"

#include "platform.h" // Header file for configuring the SD card
#include "xil_printf.h"

#define DDR_BASE_ADDR XPAR_PSU_DDR_0_S_AXI_BASEADDR // DDR AXI base address
#define MEM_BASE_ADDR XPAR_CUSTOM_MEM_MYIP_0_S00_AXI_BASEADDR // Memory IP Base Address
#define BUFFER_BASE_ADDR XPAR_MYIP_BUFFER_0_S00_AXI_BASEADDR // BUFFER BASE ADDRESS

// Offset Address for the Multiplier AXI Register Address
#define REG0_OFFSET_ADDR CUSTOM_MEM_MYIP_S00_AXI_SLV_REG0_OFFSET
#define REG1_OFFSET_ADDR CUSTOM_MEM_MYIP_S00_AXI_SLV_REG1_OFFSET
#define REG2_OFFSET_ADDR CUSTOM_MEM_MYIP_S00_AXI_SLV_REG2_OFFSET
#define REG3_OFFSET_ADDR CUSTOM_MEM_MYIP_S00_AXI_SLV_REG3_OFFSET
static const char *card_path = "0:/"; // where is the SD card is mounted, i.e., card path
static FATFS fs; // Pointer to the filesystem
static FIL read_file, write_file; // File pointers to read and write in file
FRESULT status;
#define SIZE 65536

unsigned int n_bytes;


int main()
{
    init_platform(); // Initialize all the peripherals and configures everything

    // All the memories are byte addressable
    // Memory Allocation in DDR
    s32* arr1 = (s32*)calloc(SIZE,sizeof(s32)); //s32 is a typedef of int32_t i.e, signed
 //   s32* arr2 = (s32*)(DDR_BASE_ADDR+400); // Since address are byte addressable, we need to access SIZE 32 bit elements
    //Each 32 bit number has 4 address. So to access the second array we need to multiply SIZE*4 and add with base address
    s32* arr3 = (s32*)calloc(SIZE,sizeof(s32));


    status = f_mount(&fs, card_path,1); // Mount the sd card and (1) load it immediately
    printf("Card Mounting Status = %d\n",status);

    // Read Data from File 1

    status = f_open(&read_file,"file1.txt",FA_READ); //Open the file in read mode from SD card
    // Note: The length of the file name must not be greater than 12 characters
    printf("File Opening status = %d\n",status);

    if(status == FR_OK){
    	/*SIZE
			FRESULT f_read (
  	  	  	  FIL* FileObject,    // Pointer to the file object structure
  	  	  	  void* Buffer,       // Pointer to the buffer to store read data
  	  	  	  UINT ByteToRead,    // Number of bytes to read
  	  	  	  UINT* ByteRead      // Pointer to the variable to return number of bytes read
			);
    	 */
    	status = f_read(&read_file, arr1, SIZE*sizeof(s32),&n_bytes);
    	printf("File reading status = %d\n",status);
    	printf("arr1[64] = %ld\n\r",arr1[64]);

    	status = f_close(&read_file);
    	printf("File closing status = %d\n",status);
    }

    // Reading from file 2

//    status = f_open(&read_file,"second.txt",FA_READ); //Open the file in read mode from SD card
//        // Note: The length of the file name must not be greater than 12 characters
//    printf("File Opening status = %d\n",status);
//
//    if(status == FR_OK){
//        	/*
//    			FRESULT f_read (
//      	  	  	  FIL* FileObject,    // Pointer to the file object structure
//      	  	  	  void* Buffer,       // Pointer to the buffer to store read data
//      	  	  	  UINT ByteToRead,    // Number of bytes to read
//      	  	  	  UINT* ByteRead      // Pointer to the variable to return number of bytes read
//    			);
//        	 */
//        status = f_read(&read_file, arr2, SIZE*sizeof(s32),&n_bytes);
//        printf("File reading status = %d\n",status);
//        printf("arr1[64] = %ld\n\r",arr2[64]);
//
//        status = f_close(&read_file);
//        printf("File closing status = %d\n",status);
//       }

    // Configuring Custom Memory
    	printf("Configuring\n");
        //MYIP_CUSTOM_MEM_mWriteReg(MYIP_ADDR,OFFSET0,  5);
    	CUSTOM_MEM_MYIP_mWriteReg(MEM_BASE_ADDR, REG0_OFFSET_ADDR, 5);
    	printf("Done Initializing BRAM\n");



    // Perform Element wise multiplication

    for(u32 i= 0; i<65537 ;i++){

    	// Writing from PS to PL
    	CUSTOM_MEM_MYIP_mWriteReg(MEM_BASE_ADDR, REG1_OFFSET_ADDR, i);
    	CUSTOM_MEM_MYIP_mWriteReg(MEM_BASE_ADDR, REG2_OFFSET_ADDR, arr1[i]);
    }

    printf("Data Writing in BRAM is over\n");
    CUSTOM_MEM_MYIP_mWriteReg(MEM_BASE_ADDR, REG0_OFFSET_ADDR, 21);

    for(u32 i= 0; i<65537 ;i++){
        	// Reading from PL
        	arr3[i]= MYIP_BUFFER_mReadReg(BUFFER_BASE_ADDR, 0);
        }

    printf("arr3[64] = %ld\n\r",arr3[64]);
    //printf("arr1[64]*arr2[64] = %ld*%ld=%ld\n\r", arr1[64],  arr3[64]);

    // Open file for writing
    status = f_open(&write_file, "ans.txt", FA_WRITE | FA_CREATE_ALWAYS);
   	printf("File opening for write: %d\n", status);

   	if(status == FR_OK){
   		// Write in the file
   		status = f_write(&write_file, arr3, SIZE*sizeof(s32), &n_bytes);
    	printf("File writing status: %d\n", status);

    	status = f_close(&write_file);
    	printf("Closing file: %d\n", status);
    }

   		// Unmount the SD Card

    	status = f_unmount(card_path);
    	printf("Card unmounting: %d\n\n", status);

    free(arr1);
    free(arr3);

    cleanup_platform();
    return 0;
}

