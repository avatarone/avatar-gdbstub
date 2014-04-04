#!/usr/bin/env python3

import argparse
import sys
import serial
import platform
import os
import subprocess
import select
import socket
import time
from functools import reduce
import logging

log = logging.getLogger("Inforad_K0_flasher")
        
def open_serial(port, baudrate):
    return serial.Serial(port, baudrate, serial.EIGHTBITS, serial.PARITY_NONE, serial.STOPBITS_ONE)
    
def encode_nmea_message(cmd):
    checksum = reduce(lambda r, x: r ^ x, cmd.encode(encoding = "ascii"))
    return ("$%s*%02X\r\n" % (cmd, checksum)).encode(encoding = "ascii")

class SirfGPS():
    def __init__(self, port):
        self._portname = port
        self._port = open_serial(port, 4800)
        
    def switch_to_binary_sirf(self):
        MAX_ERR_COUNT = 5
        nmea_cmd = "PSRF100,0,38400,8,1,0"
        raw_cmd = encode_nmea_message(nmea_cmd)
       
        nonascii_found = False
        error_count = 0
        while not nonascii_found and error_count < MAX_ERR_COUNT:
            self._port.write("\r\n".encode(encoding = "ascii")) 
            self._port.flush()
            self._port.write(raw_cmd)
            self._port.flush()
            time.sleep(0.5)
            self._port.timeout = 0.1
            
            try:
                for count in range(0, 10000):
                    data = self._port.read(1)
                    if not data:
                        break
                    if data[0] >= 128:
                        nonascii_found = True
                        break
                        
            except serial.SerialTimeoutException:
                pass
                
            error_count += 1
        
        if error_count >= MAX_ERR_COUNT:
            log.error("Too many errors while trying to switch to binary SIRF mode, aborting")
            sys.exit(1)
            
        self._port.baudrate = 38400
        
    def switch_to_bootloader_mode(self):
        self._port.write(bytes([0xa0, 0xa2, 0x00, 0x01, 0x94, 0x00, 0x94, 0xb0, 0xb3]))
        self._port.flush()
        
        #drain input buffer
        self._port.timeout = 0
        try:
            while self._port.read(1):
                pass
        except serial.SerialTimeoutException:
            pass
            
        time.sleep(0.5)
        self._port.timeout = 0.5
        try:
            data = self._port.read(1)
            if data:
                raise RuntimeError("No input expected on serial port")
        except serial.SerialTimeoutException:
            pass
            
        self._port.timeout = None
        
    def upload_bootloader(self, bootloader):
        msg = bytes([0x53, 0x00, 0x00, 0x00, (len(bootloader) >> 8) & 0xff, len(bootloader) & 0xff])
        self._port.write(msg)
        
        for i in range(0, len(bootloader), 512):
            self._port.write(bootloader[i: min(len(bootloader), i + 512)])
            
        self._port.timeout = None
            
        #Probably the start address
        self._port.write(bytes([0x00, 0x00, 0x00, 0x00]))
        self._port.flush()
        
    def serve_serial_port(self, port):
        server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        server_socket.bind(("localhost", port))
        server_socket.listen(1)
        (client_socket, sockaddr) = server_socket.accept()
        print("Client connected from %s:%d" % (sockaddr[0], sockaddr[1]))
        try:
            while True:
                (rd, wr, err) = select.select([client_socket, self._port, server_socket], [], [], 1)
                if err:
                    break
                if server_socket in rd:
                    client_socket.close()
                    (client_socket, sockaddr) = server_socket.accept()
                    print("Changing to new client from %s:%d", sockaddr[0], sockaddr[1])
                else:
                    if client_socket in rd:
                        self._port.write(client_socket.recv(4096))
                    if self._port in rd:
                        client_socket.send(self._port.read(1))
        except socket.error:
            print("ERROR: Socket disconnected")
        
        
        #Do not know if this acknowledgement is already from our bootloader or the ROM loader
#        data = self._port.read(1)       
#        if len(data) == 1 and data[0] == 0x06:
    
def main():
    parser = argparse.ArgumentParser(description = "Run a debugging stub on the Inforad K0 GPS stick.")
    parser.add_argument("-s", "--serial", metavar = "FILE", dest = "serial", default = "/dev/ttyUSB0", help = "Serial port where the GPS is connected")
    parser.add_argument("loader_file", metavar = "FILE", help = "Bootloader file")
    parser.add_argument("-p", "--port", type = int, dest = "port", default = 2000, help = "TCP port serving the serial connection")
    
    args = parser.parse_args()
    gps = SirfGPS(args.serial)
    with open(args.loader_file, 'rb') as file:
        bootloader = file.read()
    
    gps.switch_to_binary_sirf()
    print("Switched to binary SIRF mode")
    gps.switch_to_bootloader_mode()
    print("Switched to bootloader mode")
    # while True:
   #      data = gps._port.read(1)
   #      if data:
   #          sys.stdout.write(data.decode(encoding = "iso-8859-1"))
    gps.upload_bootloader(bootloader)
    print("Uploaded bootloader, serving TCP port")
    gps.serve_serial_port(args.port)
        
if __name__ == "__main__":
    main()
        
