/*
 * SerialPort_Windows.cpp
 *
 *       Created on:  Feb 25, 2012
 *  Last Updated on:  Feb 27, 2015
 *           Author:  Will Hedgecock
 *
 * Copyright (C) 2012-2015 Fazecast, Inc.
 *
 * This file is part of jSerialComm.
 *
 * jSerialComm is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * jSerialComm is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with jSerialComm.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef _WIN32
#define WINVER _WIN32_WINNT_WINXP
#define _WIN32_WINNT _WIN32_WINNT_WINXP
#define NTDDI_VERSION NTDDI_WINXP
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <cstdlib>
#include <string>
#include <map>
#include "../com_fazecast_jSerialComm_SerialPort.h"

JNIEXPORT jobjectArray JNICALL Java_com_fazecast_jSerialComm_SerialPort_getCommPorts(JNIEnv *env, jclass serialCommClass)
{
	HKEY keyHandle1, keyHandle2, keyHandle3, keyHandle4, keyHandle5;
	DWORD numSubKeys1, numSubKeys2, numSubKeys3, numValues;
	DWORD maxSubKeyLength1, maxSubKeyLength2, maxSubKeyLength3;
	DWORD maxValueLength, maxComPortLength, valueLength, comPortLength, keyType;
	DWORD subKeyLength1, subKeyLength2, subKeyLength3, friendlyNameLength;

	// Enumerate serial ports on machine
	std::map<std::string, std::string> serialCommPorts;
	if ((RegOpenKeyEx(HKEY_LOCAL_MACHINE, "HARDWARE\\DEVICEMAP\\SERIALCOMM", 0, KEY_QUERY_VALUE, &keyHandle1) == ERROR_SUCCESS) &&
			(RegQueryInfoKey(keyHandle1, NULL, NULL, NULL, NULL, NULL, NULL, &numValues, &maxValueLength, &maxComPortLength, NULL, NULL) == ERROR_SUCCESS))
	{
		// Allocate memory
		++maxValueLength;
		++maxComPortLength;
		CHAR *valueName = (CHAR*)malloc(maxValueLength);
		CHAR *comPort = (CHAR*)malloc(maxComPortLength);

		// Iterate through all COM ports
		for (DWORD i = 0; i < numValues; ++i)
		{
			// Get serial port name and COM value
			valueLength = maxValueLength;
			comPortLength = maxComPortLength;
			memset(valueName, 0, valueLength);
			memset(comPort, 0, comPortLength);
			if ((RegEnumValue(keyHandle1, i, valueName, &valueLength, NULL, &keyType, (BYTE*)comPort, &comPortLength) == ERROR_SUCCESS) && (keyType == REG_SZ))
			{
				// Set port name and description
				std::string comPortString = (comPort[0] == '\\') ? std::string(strrchr(comPort, '\\') + 1) : std::string(comPort);
				std::string descriptionString = strrchr(valueName, '\\') ? std::string(strrchr(valueName, '\\') + 1) : std::string(valueName);

				// Add new SerialComm object to vector
				serialCommPorts[comPortString] = descriptionString;
			}
		}

		// Clean up memory
		free(valueName);
		free(comPort);
	}
	RegCloseKey(keyHandle1);

	// Enumerate all devices on machine
	if ((RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Enum", 0, KEY_ENUMERATE_SUB_KEYS | KEY_QUERY_VALUE, &keyHandle1) == ERROR_SUCCESS) &&
			(RegQueryInfoKey(keyHandle1, NULL, NULL, NULL, &numSubKeys1, &maxSubKeyLength1, NULL, NULL, NULL, NULL, NULL, NULL) == ERROR_SUCCESS))
	{
		// Allocate memory
		++maxSubKeyLength1;
		CHAR *subKeyName1 = (CHAR*)malloc(maxSubKeyLength1);

		// Enumerate sub-keys
		for (DWORD i1 = 0; i1 < numSubKeys1; ++i1)
		{
			subKeyLength1 = maxSubKeyLength1;
			if ((RegEnumKeyEx(keyHandle1, i1, subKeyName1, &subKeyLength1, NULL, NULL, NULL, NULL) == ERROR_SUCCESS) &&
					(RegOpenKeyEx(keyHandle1, subKeyName1, 0, KEY_ENUMERATE_SUB_KEYS | KEY_QUERY_VALUE, &keyHandle2) == ERROR_SUCCESS) &&
					(RegQueryInfoKey(keyHandle2, NULL, NULL, NULL, &numSubKeys2, &maxSubKeyLength2, NULL, NULL, NULL, NULL, NULL, NULL) == ERROR_SUCCESS))
			{
				// Allocate memory
				++maxSubKeyLength2;
				CHAR *subKeyName2 = (CHAR*)malloc(maxSubKeyLength2);

				// Enumerate sub-keys
				for (DWORD i2 = 0; i2 < numSubKeys2; ++i2)
				{
					subKeyLength2 = maxSubKeyLength2;
					if ((RegEnumKeyEx(keyHandle2, i2, subKeyName2, &subKeyLength2, NULL, NULL, NULL, NULL) == ERROR_SUCCESS) &&
							(RegOpenKeyEx(keyHandle2, subKeyName2, 0, KEY_ENUMERATE_SUB_KEYS | KEY_QUERY_VALUE, &keyHandle3) == ERROR_SUCCESS) &&
							(RegQueryInfoKey(keyHandle3, NULL, NULL, NULL, &numSubKeys3, &maxSubKeyLength3, NULL, NULL, NULL, NULL, NULL, NULL) == ERROR_SUCCESS))
					{
						// Allocate memory
						++maxSubKeyLength3;
						CHAR *subKeyName3 = (CHAR*)malloc(maxSubKeyLength3);

						// Enumerate sub-keys
						for (DWORD i3 = 0; i3 < numSubKeys3; ++i3)
						{
							subKeyLength3 = maxSubKeyLength3;
							if ((RegEnumKeyEx(keyHandle3, i3, subKeyName3, &subKeyLength3, NULL, NULL, NULL, NULL) == ERROR_SUCCESS) &&
									(RegOpenKeyEx(keyHandle3, subKeyName3, 0, KEY_ENUMERATE_SUB_KEYS | KEY_QUERY_VALUE, &keyHandle4) == ERROR_SUCCESS) &&
									(RegQueryInfoKey(keyHandle4, NULL, NULL, NULL, NULL, NULL, NULL, &numValues, NULL, &valueLength, NULL, NULL) == ERROR_SUCCESS))
							{
								// Allocate memory
								friendlyNameLength = valueLength + 1;
								CHAR *friendlyName = (CHAR*)malloc(friendlyNameLength);

								if ((RegOpenKeyEx(keyHandle4, "Device Parameters", 0, KEY_QUERY_VALUE, &keyHandle5) == ERROR_SUCCESS) &&
									(RegQueryInfoKey(keyHandle5, NULL, NULL, NULL, NULL, NULL, NULL, &numValues, NULL, &valueLength, NULL, NULL) == ERROR_SUCCESS))
								{
									// Allocate memory
									comPortLength = valueLength + 1;
									CHAR *comPort = (CHAR*)malloc(comPortLength);

									// Attempt to get COM value and friendly port name
									if ((RegQueryValueEx(keyHandle5, "PortName", NULL, &keyType, (BYTE*)comPort, &comPortLength) == ERROR_SUCCESS) && (keyType == REG_SZ) &&
											(RegQueryValueEx(keyHandle4, "FriendlyName", NULL, &keyType, (BYTE*)friendlyName, &friendlyNameLength) == ERROR_SUCCESS) && (keyType == REG_SZ))
									{
										// Set port name and description
										std::string comPortString = (comPort[0] == '\\') ? std::string(strrchr(comPort, '\\') + 1) : std::string(comPort);
										std::string descriptionString = std::string(friendlyName);

										// Update friendly name if COM port is actually connected and present in the port list
										std::map<std::string, std::string>::iterator it = serialCommPorts.find(comPortString);
										if (it != serialCommPorts.end())
											it->second = descriptionString;
									}

									// Clean up memory
									free(comPort);
								}

								// Clean up memory and close registry key
								RegCloseKey(keyHandle5);
								free(friendlyName);
							}

							// Close registry key
							RegCloseKey(keyHandle4);
						}

						// Clean up memory and close registry key
						RegCloseKey(keyHandle3);
						free(subKeyName3);
					}
				}

				// Clean up memory and close registry key
				RegCloseKey(keyHandle2);
				free(subKeyName2);
			}
		}

		// Clean up memory and close registry key
		RegCloseKey(keyHandle1);
		free(subKeyName1);
	}

	// Get relevant SerialComm methods and fill in com port array
	jmethodID serialCommConstructor = env->GetMethodID(serialCommClass, "<init>", "()V");
	jobjectArray arrayObject = env->NewObjectArray(serialCommPorts.size(), serialCommClass, 0);
	int i = 0;
	for (std::map<std::string, std::string>::iterator it = serialCommPorts.begin(); it != serialCommPorts.end(); ++it, ++i)
	{
		jobject serialCommObject = env->NewObject(serialCommClass, serialCommConstructor);
		env->SetObjectField(serialCommObject, env->GetFieldID(serialCommClass, "comPort", "Ljava/lang/String;"), env->NewStringUTF((std::string("\\\\.\\") + it->first).c_str()));
		env->SetObjectField(serialCommObject, env->GetFieldID(serialCommClass, "portString", "Ljava/lang/String;"), env->NewStringUTF(it->second.c_str()));
		env->SetObjectArrayElement(arrayObject, i, serialCommObject);
	}
	return arrayObject;
}

JNIEXPORT jboolean JNICALL Java_com_fazecast_jSerialComm_SerialPort_openPort(JNIEnv *env, jobject obj)
{
	jclass serialCommClass = env->GetObjectClass(obj);
	jstring portNameJString = (jstring)env->GetObjectField(obj, env->GetFieldID(serialCommClass, "comPort", "Ljava/lang/String;"));
	HANDLE serialPortHandle = (HANDLE)env->GetLongField(obj, env->GetFieldID(serialCommClass, "portHandle", "J"));
	const char *portName = env->GetStringUTFChars(portNameJString, NULL);

	// Try to open existing serial port with read/write access
	if ((serialPortHandle = CreateFile(portName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL)) != INVALID_HANDLE_VALUE)
	{
		// Set port handle in Java structure
		env->SetLongField(obj, env->GetFieldID(serialCommClass, "portHandle", "J"), (jlong)serialPortHandle);

		// Configure the port parameters and timeouts
		if (Java_com_fazecast_jSerialComm_SerialPort_configPort(env, obj) && Java_com_fazecast_jSerialComm_SerialPort_configFlowControl(env, obj) &&
				Java_com_fazecast_jSerialComm_SerialPort_configTimeouts(env, obj))
			env->SetBooleanField(obj, env->GetFieldID(env->GetObjectClass(obj), "isOpened", "Z"), JNI_TRUE);
		else
		{
			// Close the port if there was a problem setting the parameters
			CloseHandle(serialPortHandle);
			serialPortHandle = INVALID_HANDLE_VALUE;
			env->SetLongField(obj, env->GetFieldID(env->GetObjectClass(obj), "portHandle", "J"), (jlong)INVALID_HANDLE_VALUE);
			env->SetBooleanField(obj, env->GetFieldID(env->GetObjectClass(obj), "isOpened", "Z"), JNI_FALSE);
		}
	}

	env->ReleaseStringUTFChars(portNameJString, portName);
	return (serialPortHandle == INVALID_HANDLE_VALUE) ? JNI_FALSE : JNI_TRUE;
}

JNIEXPORT jboolean JNICALL Java_com_fazecast_jSerialComm_SerialPort_configPort(JNIEnv *env, jobject obj)
{
	DCB dcbSerialParams = {0};
	dcbSerialParams.DCBlength = sizeof(DCB);
	jclass serialCommClass = env->GetObjectClass(obj);

	// Get port parameters from Java class
	HANDLE serialPortHandle = (HANDLE)env->GetLongField(obj, env->GetFieldID(serialCommClass, "portHandle", "J"));
	DWORD baudRate = (DWORD)env->GetIntField(obj, env->GetFieldID(serialCommClass, "baudRate", "I"));
	BYTE byteSize = (BYTE)env->GetIntField(obj, env->GetFieldID(serialCommClass, "dataBits", "I"));
	int stopBitsInt = env->GetIntField(obj, env->GetFieldID(serialCommClass, "stopBits", "I"));
	int parityInt = env->GetIntField(obj, env->GetFieldID(serialCommClass, "parity", "I"));
	BYTE stopBits = (stopBitsInt == com_fazecast_jSerialComm_SerialPort_ONE_STOP_BIT) ? ONESTOPBIT : (stopBitsInt == com_fazecast_jSerialComm_SerialPort_ONE_POINT_FIVE_STOP_BITS) ? ONE5STOPBITS : TWOSTOPBITS;
	BYTE parity = (parityInt == com_fazecast_jSerialComm_SerialPort_NO_PARITY) ? NOPARITY : (parityInt == com_fazecast_jSerialComm_SerialPort_ODD_PARITY) ? ODDPARITY : (parityInt == com_fazecast_jSerialComm_SerialPort_EVEN_PARITY) ? EVENPARITY : (parityInt == com_fazecast_jSerialComm_SerialPort_MARK_PARITY) ? MARKPARITY : SPACEPARITY;
	BOOL isParity = (parityInt == com_fazecast_jSerialComm_SerialPort_NO_PARITY) ? FALSE : TRUE;

	// Retrieve existing port configuration
	if (!GetCommState(serialPortHandle, &dcbSerialParams))
		return JNI_FALSE;

	// Set updated port parameters
	dcbSerialParams.BaudRate = baudRate;
	dcbSerialParams.ByteSize = byteSize;
	dcbSerialParams.StopBits = stopBits;
	dcbSerialParams.Parity = parity;
	dcbSerialParams.fParity = isParity;
	dcbSerialParams.fBinary = TRUE;
	dcbSerialParams.fAbortOnError = FALSE;

	// Apply changes
	return SetCommState(serialPortHandle, &dcbSerialParams);
}

JNIEXPORT jboolean JNICALL Java_com_fazecast_jSerialComm_SerialPort_configFlowControl(JNIEnv *env, jobject obj)
{
	DCB dcbSerialParams = {0};
	dcbSerialParams.DCBlength = sizeof(DCB);
	jclass serialCommClass = env->GetObjectClass(obj);
	HANDLE serialPortHandle = (HANDLE)env->GetLongField(obj, env->GetFieldID(serialCommClass, "portHandle", "J"));

	// Get flow control parameters from Java class
	int flowControl = env->GetIntField(obj, env->GetFieldID(serialCommClass, "flowControl", "I"));
	BOOL CTSEnabled = (((flowControl & com_fazecast_jSerialComm_SerialPort_FLOW_CONTROL_CTS_ENABLED) > 0) ||
			((flowControl & com_fazecast_jSerialComm_SerialPort_FLOW_CONTROL_RTS_ENABLED) > 0));
	BOOL DSREnabled = (((flowControl & com_fazecast_jSerialComm_SerialPort_FLOW_CONTROL_DSR_ENABLED) > 0) ||
			((flowControl & com_fazecast_jSerialComm_SerialPort_FLOW_CONTROL_DTR_ENABLED) > 0));
	BYTE DTRValue = ((flowControl & com_fazecast_jSerialComm_SerialPort_FLOW_CONTROL_DTR_ENABLED) > 0) ? DTR_CONTROL_HANDSHAKE : DTR_CONTROL_ENABLE;
	BYTE RTSValue = ((flowControl & com_fazecast_jSerialComm_SerialPort_FLOW_CONTROL_RTS_ENABLED) > 0) ? RTS_CONTROL_HANDSHAKE : RTS_CONTROL_ENABLE;
	BOOL XonXoffInEnabled = ((flowControl & com_fazecast_jSerialComm_SerialPort_FLOW_CONTROL_XONXOFF_IN_ENABLED) > 0);
	BOOL XonXoffOutEnabled = ((flowControl & com_fazecast_jSerialComm_SerialPort_FLOW_CONTROL_XONXOFF_OUT_ENABLED) > 0);

	// Retrieve existing port configuration
	if (!GetCommState(serialPortHandle, &dcbSerialParams))
		return JNI_FALSE;

	// Set updated port parameters
	dcbSerialParams.fRtsControl = RTSValue;
	dcbSerialParams.fOutxCtsFlow = CTSEnabled;
	dcbSerialParams.fOutxDsrFlow = DSREnabled;
	dcbSerialParams.fDtrControl = DTRValue;
	dcbSerialParams.fDsrSensitivity = DSREnabled;
	dcbSerialParams.fOutX = XonXoffOutEnabled;
	dcbSerialParams.fInX = XonXoffInEnabled;
	dcbSerialParams.fTXContinueOnXoff = TRUE;
	dcbSerialParams.fErrorChar = FALSE;
	dcbSerialParams.fNull = FALSE;
	dcbSerialParams.fAbortOnError = FALSE;
	dcbSerialParams.XonLim = 2048;
	dcbSerialParams.XoffLim = 512;
	dcbSerialParams.XonChar = (char)17;
	dcbSerialParams.XoffChar = (char)19;

	// Apply changes
	return SetCommState(serialPortHandle, &dcbSerialParams);
}

JNIEXPORT jboolean JNICALL Java_com_fazecast_jSerialComm_SerialPort_configTimeouts(JNIEnv *env, jobject obj)
{
	// Get port timeouts from Java class
	COMMTIMEOUTS timeouts = {0};
	jclass serialCommClass = env->GetObjectClass(obj);
	HANDLE serialHandle = (HANDLE)env->GetLongField(obj, env->GetFieldID(serialCommClass, "portHandle", "J"));
	int timeoutMode = env->GetIntField(obj, env->GetFieldID(serialCommClass, "timeoutMode", "I"));
	DWORD readTimeout = (DWORD)env->GetIntField(obj, env->GetFieldID(serialCommClass, "readTimeout", "I"));
	DWORD writeTimeout = (DWORD)env->GetIntField(obj, env->GetFieldID(serialCommClass, "writeTimeout", "I"));

	// Set updated port timeouts
	timeouts.WriteTotalTimeoutMultiplier = 0;
	switch (timeoutMode)
	{
		case com_fazecast_jSerialComm_SerialPort_TIMEOUT_READ_SEMI_BLOCKING:		// Read Semi-blocking
			timeouts.ReadIntervalTimeout = MAXDWORD;
			timeouts.ReadTotalTimeoutMultiplier = MAXDWORD;
			timeouts.ReadTotalTimeoutConstant = readTimeout;
			timeouts.WriteTotalTimeoutConstant = 10;
			break;
		case (com_fazecast_jSerialComm_SerialPort_TIMEOUT_READ_SEMI_BLOCKING | com_fazecast_jSerialComm_SerialPort_TIMEOUT_WRITE_SEMI_BLOCKING):	// Read/Write Semi-blocking
			timeouts.ReadIntervalTimeout = MAXDWORD;
			timeouts.ReadTotalTimeoutMultiplier = MAXDWORD;
			timeouts.ReadTotalTimeoutConstant = readTimeout;
			timeouts.WriteTotalTimeoutConstant = writeTimeout;
			break;
		case (com_fazecast_jSerialComm_SerialPort_TIMEOUT_READ_SEMI_BLOCKING | com_fazecast_jSerialComm_SerialPort_TIMEOUT_WRITE_BLOCKING):		// Read Semi-blocking/Write Blocking
			timeouts.ReadIntervalTimeout = MAXDWORD;
			timeouts.ReadTotalTimeoutMultiplier = MAXDWORD;
			timeouts.ReadTotalTimeoutConstant = readTimeout;
			timeouts.WriteTotalTimeoutConstant = writeTimeout;
			break;
		case com_fazecast_jSerialComm_SerialPort_TIMEOUT_READ_BLOCKING:		// Read Blocking
			timeouts.ReadIntervalTimeout = 0;
			timeouts.ReadTotalTimeoutMultiplier = 0;
			timeouts.ReadTotalTimeoutConstant = readTimeout;
			timeouts.WriteTotalTimeoutConstant = 10;
			break;
		case (com_fazecast_jSerialComm_SerialPort_TIMEOUT_READ_BLOCKING | com_fazecast_jSerialComm_SerialPort_TIMEOUT_WRITE_SEMI_BLOCKING):	// Read Blocking/Write Semi-blocking
			timeouts.ReadIntervalTimeout = 0;
			timeouts.ReadTotalTimeoutMultiplier = 0;
			timeouts.ReadTotalTimeoutConstant = readTimeout;
			timeouts.WriteTotalTimeoutConstant = writeTimeout;
			break;
		case (com_fazecast_jSerialComm_SerialPort_TIMEOUT_READ_BLOCKING | com_fazecast_jSerialComm_SerialPort_TIMEOUT_WRITE_BLOCKING):		// Read/Write Blocking
			timeouts.ReadIntervalTimeout = 0;
			timeouts.ReadTotalTimeoutMultiplier = 0;
			timeouts.ReadTotalTimeoutConstant = readTimeout;
			timeouts.WriteTotalTimeoutConstant = writeTimeout;
			break;
		case com_fazecast_jSerialComm_SerialPort_TIMEOUT_NONBLOCKING:		// Non-blocking
		default:
			timeouts.ReadIntervalTimeout = MAXDWORD;
			timeouts.ReadTotalTimeoutMultiplier = 0;
			timeouts.ReadTotalTimeoutConstant = 0;
			timeouts.WriteTotalTimeoutConstant = 10;
			break;
	}

	// Apply changes
	return SetCommTimeouts(serialHandle, &timeouts);
}

JNIEXPORT jboolean JNICALL Java_com_fazecast_jSerialComm_SerialPort_closePort(JNIEnv *env, jobject obj)
{
	// Purge any outstanding port operations
	HANDLE serialPortHandle = (HANDLE)env->GetLongField(obj, env->GetFieldID(env->GetObjectClass(obj), "portHandle", "J"));
	PurgeComm(serialPortHandle, PURGE_RXABORT | PURGE_RXCLEAR | PURGE_TXABORT | PURGE_TXCLEAR);

	// Close port
	BOOL retVal = CloseHandle(serialPortHandle);
	serialPortHandle = INVALID_HANDLE_VALUE;
	env->SetLongField(obj, env->GetFieldID(env->GetObjectClass(obj), "portHandle", "J"), (jlong)INVALID_HANDLE_VALUE);
	env->SetBooleanField(obj, env->GetFieldID(env->GetObjectClass(obj), "isOpened", "Z"), JNI_FALSE);

	return (retVal == 0) ? JNI_FALSE : JNI_TRUE;
}

JNIEXPORT jint JNICALL Java_com_fazecast_jSerialComm_SerialPort_bytesAvailable(JNIEnv *env, jobject obj)
{
	HANDLE serialPortHandle = (HANDLE)env->GetLongField(obj, env->GetFieldID(env->GetObjectClass(obj), "portHandle", "J"));
	COMSTAT commInfo;
	DWORD numBytesAvailable;

	if (!ClearCommError(serialPortHandle, NULL, &commInfo))
		return -1;
	numBytesAvailable = commInfo.cbInQue;
	
	return (jint)numBytesAvailable;
}

JNIEXPORT jint JNICALL Java_com_fazecast_jSerialComm_SerialPort_readBytes(JNIEnv *env, jobject obj, jbyteArray buffer, jlong bytesToRead)
{
	HANDLE serialPortHandle = (HANDLE)env->GetLongField(obj, env->GetFieldID(env->GetObjectClass(obj), "portHandle", "J"));
    OVERLAPPED overlappedStruct = {0};
    overlappedStruct.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    char *readBuffer = (char*)malloc(bytesToRead);
    DWORD numBytesRead = 0;
    BOOL result;

    // Read from serial port
    if ((result = ReadFile(serialPortHandle, readBuffer, bytesToRead, NULL, &overlappedStruct)) != FALSE)	// Immediately successful
    	GetOverlappedResult(serialPortHandle, &overlappedStruct, &numBytesRead, TRUE);
    else if (GetLastError() != ERROR_IO_PENDING)		// Problem occurred
    {
    	// Problem reading, close port
    	CloseHandle(serialPortHandle);
    	serialPortHandle = INVALID_HANDLE_VALUE;
    	env->SetLongField(obj, env->GetFieldID(env->GetObjectClass(obj), "portHandle", "J"), (jlong)INVALID_HANDLE_VALUE);
    	env->SetBooleanField(obj, env->GetFieldID(env->GetObjectClass(obj), "isOpened", "Z"), JNI_FALSE);
    }
    else		// Pending read operation
    {
    	switch (WaitForSingleObject(overlappedStruct.hEvent, INFINITE))
    	{
    		case WAIT_OBJECT_0:
    			if ((result = GetOverlappedResult(serialPortHandle, &overlappedStruct, &numBytesRead, TRUE)) == FALSE)
    			{
    				// Problem reading, close port
    				CloseHandle(serialPortHandle);
    				serialPortHandle = INVALID_HANDLE_VALUE;
    				env->SetLongField(obj, env->GetFieldID(env->GetObjectClass(obj), "portHandle", "J"), (jlong)INVALID_HANDLE_VALUE);
    				env->SetBooleanField(obj, env->GetFieldID(env->GetObjectClass(obj), "isOpened", "Z"), JNI_FALSE);
    			}
    			break;
    		default:
    			// Problem reading, close port
    			CloseHandle(serialPortHandle);
    			serialPortHandle = INVALID_HANDLE_VALUE;
    			env->SetLongField(obj, env->GetFieldID(env->GetObjectClass(obj), "portHandle", "J"), (jlong)INVALID_HANDLE_VALUE);
    			env->SetBooleanField(obj, env->GetFieldID(env->GetObjectClass(obj), "isOpened", "Z"), JNI_FALSE);
    			break;
    	}
    }

    // Return number of bytes read if successful
    CloseHandle(overlappedStruct.hEvent);
    env->SetByteArrayRegion(buffer, 0, numBytesRead, (jbyte*)readBuffer);
    free(readBuffer);
	return (result == TRUE) ? numBytesRead : -1;
}

JNIEXPORT jint JNICALL Java_com_fazecast_jSerialComm_SerialPort_writeBytes(JNIEnv *env, jobject obj, jbyteArray buffer, jlong bytesToWrite)
{
	HANDLE serialPortHandle = (HANDLE)env->GetLongField(obj, env->GetFieldID(env->GetObjectClass(obj), "portHandle", "J"));
	OVERLAPPED overlappedStruct = {0};
	overlappedStruct.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	jbyte *writeBuffer = env->GetByteArrayElements(buffer, 0);
	DWORD numBytesWritten = 0;
	BOOL result;

	// Write to port
	if ((result = WriteFile(serialPortHandle, writeBuffer, bytesToWrite, NULL, &overlappedStruct)) == FALSE)
	{
		if (GetLastError() != ERROR_IO_PENDING)
		{
			// Problem writing, close port
			CloseHandle(serialPortHandle);
			serialPortHandle = INVALID_HANDLE_VALUE;
			env->SetLongField(obj, env->GetFieldID(env->GetObjectClass(obj), "portHandle", "J"), (jlong)INVALID_HANDLE_VALUE);
			env->SetBooleanField(obj, env->GetFieldID(env->GetObjectClass(obj), "isOpened", "Z"), JNI_FALSE);
		}
		else
		{
			switch (WaitForSingleObject(overlappedStruct.hEvent, INFINITE))
			{
				case WAIT_OBJECT_0:
					if ((result = GetOverlappedResult(serialPortHandle, &overlappedStruct, &numBytesWritten, TRUE)) == FALSE)
			    	{
			    		// Problem reading, close port
			    		CloseHandle(serialPortHandle);
			    		serialPortHandle = INVALID_HANDLE_VALUE;
			    		env->SetLongField(obj, env->GetFieldID(env->GetObjectClass(obj), "portHandle", "J"), (jlong)INVALID_HANDLE_VALUE);
			    		env->SetBooleanField(obj, env->GetFieldID(env->GetObjectClass(obj), "isOpened", "Z"), JNI_FALSE);
			    	}
			    	break;
			    default:
			    	// Problem reading, close port
			    	CloseHandle(serialPortHandle);
			    	serialPortHandle = INVALID_HANDLE_VALUE;
			    	env->SetLongField(obj, env->GetFieldID(env->GetObjectClass(obj), "portHandle", "J"), (jlong)INVALID_HANDLE_VALUE);
			    	env->SetBooleanField(obj, env->GetFieldID(env->GetObjectClass(obj), "isOpened", "Z"), JNI_FALSE);
			    	break;
			}
		}
	}
	else
		GetOverlappedResult(serialPortHandle, &overlappedStruct, &numBytesWritten, TRUE);

	// Return number of bytes written if successful
	CloseHandle(overlappedStruct.hEvent);
	env->ReleaseByteArrayElements(buffer, writeBuffer, JNI_ABORT);
	return (result == TRUE) ? numBytesWritten : -1;
}

#endif