/*
 * Apple System Management Control (SMC) Tool
 * Copyright (C) 2006 devnull
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
//
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
//
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
//
 #include <IOKit/IOKitLib.h>
 #include <stdio.h>
#include <string.h>
 #ifndef __SMC_H__
 #define __SMC_H__
 #endif
 #define VERSION "0.01"
 #define KERNEL_INDEX_SMC 2
 #define SMC_CMD_READ_BYTES 5
 #define SMC_CMD_WRITE_BYTES 6
 #define SMC_CMD_READ_INDEX 8
 #define SMC_CMD_READ_KEYINFO 9
 #define SMC_CMD_READ_PLIMIT 11
 #define SMC_CMD_READ_VERS 12
 #define DATATYPE_FPE2 "fpe2"
 #define DATATYPE_UINT8 "ui8 "
 #define DATATYPE_UINT16 "ui16"
 #define DATATYPE_UINT32 "ui32"
 #define DATATYPE_SP78 "sp78"
// key values
 #define SMC_KEY_CPU_TEMP "TC0P"
 #define SMC_KEY_GPU_TEMP "TG0P"
 #define SMC_KEY_FAN0_RPM_CUR "F0Ac"
//
 typedef struct {
	 char major;
	 char minor;
	 char build;
	 char reserved[1];
	 UInt16 release;
 } SMCKeyData_vers_t;
//
 typedef struct {
	 UInt16 version;
	 UInt16 length;
	 UInt32 cpuPLimit;
	 UInt32 gpuPLimit;
	 UInt32 memPLimit;
 } SMCKeyData_pLimitData_t;
//
 typedef struct {
	 UInt32 dataSize;
	 UInt32 dataType;
	 char dataAttributes;
 } SMCKeyData_keyInfo_t;
//
 typedef char SMCBytes_t[32];
//
 typedef struct {
	 UInt32 key;
	 SMCKeyData_vers_t vers;
	 SMCKeyData_pLimitData_t pLimitData;
	 SMCKeyData_keyInfo_t keyInfo;
	 char result;
	 char status;
	 char data8;
	 UInt32 data32;
	 SMCBytes_t bytes;
 } SMCKeyData_t;
//
 typedef char UInt32Char_t[5];
//
 typedef struct {
	 UInt32Char_t key;
	 UInt32 dataSize;
	 UInt32Char_t dataType;
	 SMCBytes_t bytes;
 } SMCVal_t;
//
typedef struct {
    int number;
    float* speeds;
} Fans;
//
 //prototypes
 double SMCGetTemperature(char* key);
 kern_return_t SMCSetFanRpm(char* key, int rpm);
 int SMCGetFanRpm(char* key);
 Fans getFanInfo();
//
 static io_connect_t conn;
//
 UInt32 _strtoul(char* str, int size, int base)
 {
	 UInt32 total = 0;
	 int i;
//
	 for (i = 0; i < size; i++) {
		 if (base == 16)
			 total += str[i] << (size - 1 - i) * 8;
		 else
			 total += (unsigned char)(str[i] << (size - 1 - i) * 8);
	 }
	 return total;
 }
//
 void _ultostr(char* str, UInt32 val)
 {
	 str[0] = '\0';
	 sprintf(str, "%c%c%c%c",
		 (unsigned int)val >> 24,
		 (unsigned int)val >> 16,
		 (unsigned int)val >> 8,
		 (unsigned int)val);
 }
//
 kern_return_t SMCOpen(void)
 {
	 kern_return_t result;
	 io_iterator_t iterator;
	 io_object_t device;
//
	 CFMutableDictionaryRef matchingDictionary = IOServiceMatching("AppleSMC");
//
 #if (MAC_OS_X_VERSION_MAX_ALLOWED < 120000)
	 result = IOServiceGetMatchingServices(kIOMasterPortDefault, matchingDictionary, &iterator);
 #else
	 result = IOServiceGetMatchingServices(kIOMainPortDefault, matchingDictionary, &iterator);
 #endif
//
	 if (result != kIOReturnSuccess) {
		 printf("Error: IOServiceGetMatchingServices() = %08x\n", result);
		 return 1;
	 }
//
	 device = IOIteratorNext(iterator);
	 IOObjectRelease(iterator);
	 if (device == 0) {
		 printf("Error: no SMC found\n");
		 return 1;
	 }
//
	 result = IOServiceOpen(device, mach_task_self(), 0, &conn);
	 IOObjectRelease(device);
	 if (result != kIOReturnSuccess) {
		 printf("Error: IOServiceOpen() = %08x\n", result);
		 return 1;
	 }
//
	 return kIOReturnSuccess;
 }
//
 kern_return_t SMCClose()
 {
	 return IOServiceClose(conn);
 }
//
 kern_return_t SMCCall(int index, SMCKeyData_t* inputStructure, SMCKeyData_t* outputStructure)
 {
	 size_t structureInputSize;
	 size_t structureOutputSize;
//
	 structureInputSize = sizeof(SMCKeyData_t);
	 structureOutputSize = sizeof(SMCKeyData_t);
//
 #if MAC_OS_X_VERSION_10_5
	 return IOConnectCallStructMethod(conn, index,
		 //inputStructure
		 inputStructure, structureInputSize,
		 //ouputStructure
		 outputStructure, &structureOutputSize);
 #else
	 return IOConnectMethodStructureIStructureO(conn, index,
		 structureInputSize, /* structureInputSize */
		 &structureOutputSize, /* structureOutputSize */
		 inputStructure, /* inputStructure */
		 outputStructure); /* ouputStructure */
 #endif
 }
//
 kern_return_t SMCReadKey(UInt32Char_t key, SMCVal_t* val)
 {
	 kern_return_t result;
	 SMCKeyData_t inputStructure;
	 SMCKeyData_t outputStructure;
//
	 memset(&inputStructure, 0, sizeof(SMCKeyData_t));
	 memset(&outputStructure, 0, sizeof(SMCKeyData_t));
	 memset(val, 0, sizeof(SMCVal_t));
//
	 inputStructure.key = _strtoul(key, 4, 16);
	 inputStructure.data8 = SMC_CMD_READ_KEYINFO;
//
	 result = SMCCall(KERNEL_INDEX_SMC, &inputStructure, &outputStructure);
	 if (result != kIOReturnSuccess)
		 return result;
//
	 val->dataSize = outputStructure.keyInfo.dataSize;
	 _ultostr(val->dataType, outputStructure.keyInfo.dataType);
	 inputStructure.keyInfo.dataSize = val->dataSize;
	 inputStructure.data8 = SMC_CMD_READ_BYTES;
//
	 result = SMCCall(KERNEL_INDEX_SMC, &inputStructure, &outputStructure);
	 if (result != kIOReturnSuccess)
		 return result;
//
	 memcpy(val->bytes, outputStructure.bytes, sizeof(outputStructure.bytes));
//
	 return kIOReturnSuccess;
 }
//
 double SMCGetTemperature(char* key)
 {
  SMCOpen();
	 SMCVal_t val;
	 kern_return_t result;
//
	 result = SMCReadKey(key, &val);
	 if (result == kIOReturnSuccess) {
		 //read succeeded - check returned value
		 if (val.dataSize > 0) {
			 if (strcmp(val.dataType, DATATYPE_SP78) == 0) {
				 //convert sp78 value to temperature
				 int intValue = val.bytes[0] * 256 + (unsigned char)val.bytes[1];
				 return intValue / 256.0;
			 }
		 }
	 }
  SMCClose();
	 //read failed
	 return 0.0;
 }
//
 double SMCGetFanSpeed(char* key)
 {
	 SMCVal_t val;
	 kern_return_t result;
//
	 result = SMCReadKey(key, &val);
	 if (result == kIOReturnSuccess) {
		 //read succeeded - check returned value
		 if (val.dataSize > 0) {
			 if (strcmp(val.dataType, DATATYPE_FPE2) == 0) {
				 //convert fpe2 value to rpm
				 int intValue = (unsigned char)val.bytes[0] * 256 + (unsigned char)val.bytes[1];
				 return intValue / 4.0;
			 }
		 }
	 }
	 //read failed
	 return 0.0;
 }
//
//
 float SMCGetFanRPM(char* key)
 {
	 SMCVal_t val;
	 kern_return_t result;
//
	 result = SMCReadKey(key, &val);
	 if (result == kIOReturnSuccess) {
		 //read succeeded - check returned value
		 if (val.dataSize > 0) {
			 if (strcmp(val.dataType, DATATYPE_FPE2) == 0) {
				 //convert fpe2 value to RPM
				 return ntohs(*(UInt16*)val.bytes) / 4.0;
			 }
		 }
	 }
	 //read failed
	 return -1.f;
 }
//
//
Fans getFanInfo(){
    kern_return_t result;
    SMCVal_t val;
    UInt32Char_t key;
//
    Fans summary;
    summary.number = 0;
    summary.speeds = NULL;
//
    int totalFans, i;
    SMCOpen();
    result = SMCReadKey("FNum", &val);
//
    if (result == kIOReturnSuccess) {
        totalFans = _strtoul((char*)val.bytes, val.dataSize, 10);
        summary.number = totalFans;
        summary.speeds =  (float*) malloc(sizeof(float) * summary.number);
//
        printf("Num fans: %d\n", totalFans);
        for (i = 0; i < totalFans; i++) {
//
            summary.speeds[i] = 0;
//
            sprintf(key, "F%dID", i);
            result = SMCReadKey(key, &val);
            if (result != kIOReturnSuccess) {
                continue;
            }
            char* name = val.bytes + 4;
//
            sprintf(key, "F%dAc", i);
            float actual_speed = SMCGetFanRPM(key);
            if (actual_speed < 0.f) {
                continue;
            }
//
            sprintf(key, "F%dMn", i);
            float minimum_speed = SMCGetFanRPM(key);
            if (minimum_speed < 0.f) {
                continue;
            }
//
            sprintf(key, "F%dMx", i);
            float maximum_speed = SMCGetFanRPM(key);
            if (maximum_speed < 0.f) {
                continue;
            }
//
            float rpm = actual_speed - minimum_speed;
            if (rpm < 0.f) {
                rpm = 0.f;
            }
            float pct = rpm / (maximum_speed - minimum_speed);
//
            pct *= 100.f;
//
            summary.speeds[i] = pct;
            printf("Fan %d - %s at %.0f RPM (%.0f%%)\n", i, name, rpm, pct);
        }
    }
    SMCClose();
    return summary;
}