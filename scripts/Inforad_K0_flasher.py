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

class OSXSerial():
    def __init__(self, port, baudrate):
        self._handle = os.open(port, os.O_RDWR)
        cmd = ["stty", "-f", port, "%d" % baudrate, "cs8", "-crtscts", "cread", "clocal", "-cstopb", "-parenb"]
        subprocess.check_call(cmd)
        self._timeout = None
        
    def read(self, num = 0):
        result = select.select([self._handle], [], [], self._timeout)

        if self._handle in result[0]:
            return os.read(self._handle, num)
        else:
            raise serial.SerialTimeoutException()
        
    def write(self, buf):
        os.write(self._handle, buf)
        
    def close(self):
        os.close(self._handle)
        
    @property
    def timeout(self):
        return self._timeout

    @timeout.setter
    def timeout(self, value):
        self._timeout = value
        
def open_serial(port, baudrate):
#    if platform.system() == "Darwin":
#        return OSXSerial(port, baudrate)
#    else:
    return serial.Serial(port, baudrate, serial.EIGHTBITS, serial.PARITY_NONE, serial.STOPBITS_ONE)
    
def encode_nmea_message(cmd):
    checksum = reduce(lambda r, x: r ^ x, cmd.encode(encoding = "ascii"))
    return ("$%s*%02X\r\n" % (cmd, checksum)).encode(encoding = "ascii")

class SirfGPS():
    def __init__(self, port):
        self._portname = port
        self._port = open_serial(port, 4800)
        
    def switch_to_binary_sirf(self):
        nmea_cmd = "PSRF100,0,38400,8,1,0"
        raw_cmd = encode_nmea_message(nmea_cmd)
       
        self._port.write("\r\n".encode(encoding = "ascii")) 
        self._port.flush()
        self._port.write(raw_cmd)
        self._port.flush()
        self._port.timeout = 0.1
        try:
            while self._port.read(1):
                pass
        except serial.SerialTimeoutException:
            pass
        self._port.close()
        
        self._port = open_serial(self._portname, 38400)
        
    def switch_to_bootloader_mode(self):
        self._port.write(bytes([0xa0, 0xa2, 0x00, 0x01, 0x94, 0x00, 0x94, 0xb0, 0xb3]))
        self._port.flush()
        time.sleep(0.5)
        self._port.flushInput()
        #drain input buffer
        self._port.timeout = 0.5
        try:
            self._port.read(1)
#            raise RuntimeError("No input expected on serial port")
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
    parser = argparse.ArgumentParser(description = "Run a debugging stub on the Inforad K0 GPS stick")
    parser.add_argument("-s", "--serial", metavar = "FILE", dest = "serial", help = "Serial port where the GPS is connected")
    parser.add_argument("-l", "--loader", metavar = "FILE", dest = "loader_file", help = "Bootloader file")
    parser.add_argument("-p", "--port", type = int, dest = "port", help = "TCP port serving the serial connection")
    
    args = parser.parse_args()
    gps = SirfGPS(args.serial)
    with open(args.loader_file, 'rb') as file:
        bootloader = file.read()
    
    gps.switch_to_binary_sirf()
    print("Switched to binary SIRF mode")
    gps.switch_to_bootloader_mode()
    print("Switched to bootloader mode")
    gps.upload_bootloader(bootloader)
    print("Uploaded bootloader, serving TCP port")
    gps.serve_serial_port(args.port)
        
if __name__ == "__main__":
    main()
        
