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
import argparse
import subprocess

SLEEP_TIME_BETWEEN_OFF_ON = 3
MAX_ENTER_BOOT_MENU_TIME = 3

log = logging.getLogger(__name__)

class ResetController():
    def __init__(self, controller_script):
        self._controller_script = controller_script
        
    def set_on(self):
        subprocess.check_call([self._controller_script, "on"])
        
    def set_off(self):
        subprocess.check_call([self._controller_script, "off"])
        
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
    def __init__(self, reset_controller, serial_port_name = "/dev/ttyUSB0"):
        if reset_controller is None:
            self._emulated_target = True
        else:
            self.reset_controller = reset_controller
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
            try:
                buffer += self._serial_port.read(1)
            except serial.serialutil.SerialException as ex:
                if not str(ex).startswith("device reports readiness to read"):
                    raise ex
        
        #Flush input stream
        try:
            while select.select([self._serial_port], [], [], 0.01)[0]:
                self._serial_port.read(1)
        except serial.serialutil.SerialException as ex:
            if not str(ex).startswith("device reports readiness to read"):
                    raise ex

        
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
            try:
                while select.select([self._serial_port], [], [], 0.01)[0]:
                    buffer += self._serial_port.read(1)
            except serial.serialutil.SerialException as ex:
                if not str(ex).startswith("device reports readiness to read"):
                    raise ex

        
        buffer = bytes()   
        while not EXPRESSION_ECHO_OFF in buffer:     
            self._serial_port.write(bytes("TE\r", encoding = 'ascii'))
            time.sleep(0.1) #sleep needed for emulated target
            while select.select([self._serial_port], [], [], 0.01)[0]:
                try:
                    buffer += self._serial_port.read(1)
                except serial.serialutil.SerialException as ex:
                    if not str(ex).startswith("device reports readiness to read"):
                        raise ex
                
        #Flush input stream
        try:
            while select.select([self._serial_port], [], [], 0.01)[0]:
                buffer += self._serial_port.read(1)
        except serial.serialutil.SerialException as ex:
            if not str(ex).startswith("device reports readiness to read"):
                raise ex
        
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
        to_read_bytes = len(data) + len(expected_reply)
        reply = bytes()
        try:
            reply = self._serial_port.read(to_read_bytes)
        except serial.serialutil.SerialException as ex:
            if not str(ex).startswith("device reports readiness to read"):
                raise ex
        
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
                
def parse_arguments():
    parser = argparse.ArgumentParser()
    parser.add_argument("-v", "--verbose", action = "count", default = 0, dest = "verbosity", 
        help = "Increase verbosity (Can be specified several times to increase more)")
    parser.add_argument("--power-control", type = str, metavar = "FILE", dest = "power_control",
        help = "Executable that can switch the HDD on and off (\"on\" or \"off\" is passed as first argument)")
    parser.add_argument("--serial", type = str, metavar = "FILE", dest = "serial",
        help = "Serial port to which the HDD is connected")
    parser.add_argument("--gdbstub", type = str, metavar = "FILE", dest = "gdbstub",
        help = "GDB stub that is injected in the HDD")
    parser.add_argument("--gdbstub-loadaddress", type = int, dest = "gdbstub_loadaddress",
        help = "Load address of GDB stub")
    parser.add_argument("--port", type = int, default = 2000, dest = "port",
        help = "Port where HDD flasher is listening for serial connections after GDB stub has been flashed")
    return parser.parse_args()
    
def set_verbosity(verbosity):
    if verbosity >= 3:
        logging.basicConfig(level = logging.DEBUG)
    elif verbosity >= 2:
        logging.basicConfig(level = logging.INFO)
    elif verbosity >= 1:
        logging.basicConfig(level = logging.WARN)
    else:
        logging.basicConfig(level = logging.ERROR)
        
def main():
    args = parse_arguments()
    set_verbosity(args.verbosity)
    
    stub_downloader = StubDownloader(ResetController(args.power_control), args.serial)
    log.info("Serial ports opened")
    stub_downloader.load_stub(args.gdbstub, args.gdbstub_loadaddress, args.gdbstub_loadaddress)
    log.info("Stub download finished")
    while True:
        stub_downloader.serve_serial_port(args.port)
        log.warn("Serial port stream client disconnected")
    log.info("exiting")
    
if __name__ == "__main__":
    main()
