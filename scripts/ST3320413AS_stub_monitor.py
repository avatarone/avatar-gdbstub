#!/usr/bin/env python3

# Copyright 2013-2014 Jonas Zaddach <zaddach@eurecom.fr>, EURECOM
#
# You can redistribute and/or modify this program under the terms of the
# GNU General Public License version 2 or later.


import serial
import logging
import time
import re
import select
import socket
import sys

SLEEP_TIME_BETWEEN_OFF_ON = 3
MAX_ENTER_BOOT_MENU_TIME = 3

log = logging.getLogger(__name__)

class ResetController():
    def __init__(self, serial_port_name, is_inverted = False):
        self._serial_port = serial.Serial(serial_port_name)
        self.is_inverted = is_inverted
        
    def set_on(self):
        self._serial_port.setRTS(not self.is_inverted)
        
    def set_off(self):
        self._serial_port.setRTS(self.is_inverted)
        
    def reset(self):
        self.set_off()
        time.sleep(SLEEP_TIME_BETWEEN_OFF_ON)
        self.set_on()
        
class UnexpectedReplyException(Exception):
    def __init__(self, reply):
        self._reply = reply
        
    def __str__(self):
        return "Unexpected reply: '%s'" % self._reply

class StubDownloader():
    def __init__(self, reset_serial_port, serial_port_name = "/dev/ttyUSB0"):
        if reset_serial_port == "none":
            self._emulated_target = True
        else:
            self.reset_controller = ResetController(reset_serial_port)
            self._emulated_target = False
            
        self._serial_port = serial.Serial(port = serial_port_name, 
                                         baudrate = 38400, 
                                         bytesize = serial.EIGHTBITS, 
                                         parity = serial.PARITY_NONE, 
                                         stopbits = serial.STOPBITS_ONE, 
                                         xonxoff = False,
                                         rtscts = False)
        self._serial_port_name = serial_port_name
        
    def _reset_hdd(self):
        self.reset_controller.reset()
        
    def _enter_bootloader(self):
        REX_BOOT_MENU_PROMPT = re.compile("Boot Cmds:")
        EXPRESSION_BOOT_CMDS = bytes("Boot Cmds:", encoding = 'ascii')
        EXPRESSION_RET = bytes("RET\r\n> ", encoding = 'ascii')
        EXPRESSION_ECHO_OFF = bytes("Echo off\r\n> ", encoding = 'ascii')
        buffer = bytes()
        old_timeout = self._serial_port.getTimeout()
        self._serial_port.setTimeout(0.01)
        start_enter_bootmenu_time = 0
        
        
        while not EXPRESSION_BOOT_CMDS in buffer:
            if time.time() - start_enter_bootmenu_time > MAX_ENTER_BOOT_MENU_TIME:
                self._reset_hdd()
                start_enter_bootmenu_time = time.time()
                buffer = bytes()
                
            self._serial_port.write(bytes('UUUUUUUUUUUUUUUUUU', encoding = 'ascii'))
            buffer += self._serial_port.read(1)
        
        #Flush input stream
        while select.select([self._serial_port], [], [], 0.01)[0]:
                self._serial_port.read(1)
        
        #Check that prompt is ready for commands (not trailing U in output)
        self._resynchronize_interface()

        log.info("Entered boot menu")        
        self._serial_port.setTimeout(old_timeout)
        
    def _resynchronize_interface(self):
        EXPRESSION_RET = bytes("RET\r\n> ", encoding = 'ascii')
        EXPRESSION_ECHO_OFF = bytes("Echo off\r\n> ", encoding = 'ascii')
        buffer = bytes()
        while not EXPRESSION_RET in buffer:
            self._serial_port.write(bytes("?", encoding = "ascii"))
            #Read all characters that are still in the input stream
            while select.select([self._serial_port], [], [], 0.01)[0]:
                buffer += self._serial_port.read(1)
        
        buffer = bytes()   
        while not EXPRESSION_ECHO_OFF in buffer:     
            self._serial_port.write(bytes("TE\r", encoding = 'ascii'))
            time.sleep(0.1) #sleep needed for emulated target
            while select.select([self._serial_port], [], [], 0.01)[0]:
                buffer += self._serial_port.read(1)
                
        #Flush input stream
        while select.select([self._serial_port], [], [], 0.01)[0]:
                self._serial_port.read(1)
        
    def _set_baudrate(self, baudrate):
        DIVISOR_TABLE = {9600   : 0x28b, 
                     19200  : 0x146,
                     38400  : 0xa3,
                     57600  : 0x6d,
                     115200 : 0x36,
                     230400 : 0x1b,
                     460800 : 0xe,
                     625000 : 0xa,
                     921000 : 7,
                     921600 : 7,
                     1228000: 5,
                     1250000: 5}
        
        self._serial_port.write(bytes("BR %x\r" % DIVISOR_TABLE[baudrate], encoding = 'ascii'))
        
        while select.select([self._serial_port], [], [], 0.01)[0]:
            self._serial_port.read(1)
            
        self._serial_port.close()
        
        self._serial_port = serial.Serial(port = self._serial_port_name, 
                                         baudrate = baudrate, 
                                         bytesize = serial.EIGHTBITS, 
                                         parity = serial.PARITY_NONE, 
                                         stopbits = serial.STOPBITS_ONE, 
                                         xonxoff = False,
                                         rtscts = False)
 
        
        self._serial_port.write(bytes("?", encoding = "ascii"))
        #Read all characters that are still in the input stream
        while select.select([self._serial_port], [], [], 0.01)[0]:
            self._serial_port.read(1)
            
        log.info("Set baudrate finished")
            
    def _set_address_pointer(self, address):
        
        data = bytes("AP %X\r" % address, encoding = "ascii")
        self._serial_port.write(data)
        
        expected_reply = bytes("\r\nAddr Ptr = 0x%08X\r\n> " % address, encoding = 'ascii')
        reply = self._serial_port.read(len(data) + len(expected_reply))
        
        if reply != data + expected_reply:
            raise UnexpectedReplyException(reply)
        
    def _write_byte(self, val):
        EXPECTED_REPLY = bytes("\r\n> ", encoding = 'ascii')
        data = bytes("WT %X\r" % val, encoding = 'ascii')
        self._serial_port.write(data)
        
        reply = self._serial_port.read(len(data) + len(EXPECTED_REPLY))
        if reply != data + EXPECTED_REPLY:
            raise UnexpectedReplyException(reply)
        
    def write_data(self, data, address):
        self._set_address_pointer(address)
        for byte in data:
            self._write_byte(byte)
            
    def _go(self):
        data = bytes("GO\r", encoding = 'ascii')
        self._serial_port.write(data)
        
        reply = self._serial_port.read(len(data) + len("\r\nRun: 0x001004E9\r\n"))
        if not re.match("GO\r\r\nRun: 0x[0-9A-F]{8}\r\n", reply.decode('ascii')):
            raise UnexpectedReplyException(reply)
        
        
    def run_address(self, address):
        self._set_address_pointer(address)
        self._go()
        
                
    def load_stub(self, stub_file, address, entry_point, baudrate = 115200):
        if not self._emulated_target:
            self._enter_bootloader()
            self._set_baudrate(baudrate)
        else:
            self._resynchronize_interface()
        file = open(stub_file, 'rb')
        self.write_data(file.read(), address)
        self.run_address(entry_point)
        
    def serve_serial_port(self, port):
        server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        server_socket.bind(("localhost", port))
        server_socket.listen(1)
        (client_socket, sockaddr) = server_socket.accept()
        log.info("Client connected from %s:%d", sockaddr[0], sockaddr[1])
        try:
            while True:
                (rd, wr, err) = select.select([client_socket, self._serial_port, server_socket], [], [], 1)
                if err:
                    break
                if server_socket in rd:
                    client_socket.close()
                    (client_socket, sockaddr) = server_socket.accept()
                    log.info("Changing to new client from %s:%d", sockaddr[0], sockaddr[1])
                else:
                    if client_socket in rd:
                        self._serial_port.write(client_socket.recv(4096))
                    if self._serial_port in rd:
                        client_socket.send(self._serial_port.read(1))
        except socket.error:
            log.exception("Socket disconnected")
                

if __name__ == "__main__":
    logging.basicConfig(level = logging.DEBUG)
    
    if len(sys.argv) < 7:
        sys.stdout.write("Usage: %s <reset serial port> <comm serial port> <stub file> <load address> <entry point> <serve port>\n")
        sys.exit(1)
    
    reset_serial_port = sys.argv[1]
    comm_serial_port = sys.argv[2]
    stub_file = sys.argv[3]
    load_address = sys.argv[4].startswith("0x") and int(sys.argv[4], 16) or int(sys.argv[4])
    entry_point = sys.argv[5].startswith("0x") and int(sys.argv[5], 16) or int(sys.argv[5])
    serve_port = int(sys.argv[6])
    
    stub_downloader = StubDownloader(reset_serial_port, comm_serial_port)
    sys.stdout.write("Serial ports opened\n")
    stub_downloader.load_stub(stub_file, load_address, entry_point)
    sys.stdout.write("Stub download finished\n")
    while True:
        stub_downloader.serve_serial_port(serve_port)
        sys.stdout.write("Serial port stream client disconnected\n")
    sys.stdout.write("exiting\n")
