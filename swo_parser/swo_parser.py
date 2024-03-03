import serial
import array
from enum import Enum


# Synchronization packet header. 
SYNC_HEADER	= 0x00

# Minimal number of 0 bits required for a synchronization packet. 
SYNC_MIN_BITS	=	47

# Overflow packet header. 
OVERFLOW_HEADER	=	0x70

# Local timestamp packet header. 
LTS_HEADER	=	0xc0

LTS_CBIT_MASK = 0x80

# Bitmask for the timestamp of a local timestamp (LTS1) packet. 
LTS1_TS_MASK	=	0xfffffff

# Bitmask for the relation information of a local timestamp (LTS1) packet. 
LTS1_TC_MASK	=	0x30

# Offset of the relation information of a local timestamp (LTS1) packet. 
LTS1_TC_OFFSET	=	4

# Bitmask for the timestamp of a local timestamp (LTS2) packet. 
LTS2_TS_MASK	=	0x70

# Offset of the timestamp of local timestamp (LTS2) packet. 
LTS2_TS_OFFSET	=	4

# Global timestamp packet header. 
GTS_HEADER	=	0x94

# Bitmask for the global timestamp packet header. 
GTS_HEADER_MASK	= 0xdf

# Bitmask for the type of a global timestamp packet. 
GTS_TYPE_MASK	=	0x20

# Bitmask for the timestamp of a global timestamp (GTS1) packet. 
GTS1_TS_MASK	=	0x03ffffff

# Bitmask for the clkch bit of a global timestamp (GTS1) packet. 
GTS1_CLKCH_MASK	=	0x4000000

# Bitmask for the wrap bit of a global timestamp (GTS1) packet. 
GTS1_WRAP_MASK	=	0x8000000

# Payload size of a global timestamp (GTS2) packet in bytes. 
GTS2_PAYLOAD_SIZE	= 4

# Bitmask for the timestamp of a global timestamp (GTS2) packet. 
GTS2_TS_MASK	=	0x3fffff

# Extension packet header. 
EXT_HEADER	=	0x08

# Bitmask for the extension packet header. 
EXT_HEADER_MASK	=    0x0b

# Bitmask for the source bit of an extension packet. 
EXT_SRC_MASK	=	0x04

# Bitmask for the extension information of an extension packet header. 
EXT_TS_MASK	=	0x70

# Offset of the extension information of an extension packet header. 
EXT_TS_OFFSET	=	4

# Bitmask for the payload size of a source packet. 
SRC_SIZE_MASK	=	0x03

# Bitmask for the type of a source packet. 
SRC_TYPE_MASK	=	0x04

# Bitmask for the address of a source packet. 
SRC_ADDR_MASK	=	0xf8

# Offset of the address of a source packet. 
SRC_ADDR_OFFSET	=	3

# Bitmask for the continuation bit. 
C_MASK		=	0x80

#Helper to convert bytes object to int
def toInt(bytes): 
    return int.from_bytes(bytes,byteorder = 'little',signed=False)


class packet_types(Enum):
    SYNC   = 1
    OF     = 2
    LTS    = 3
    EXT    = 4
    GTS1   = 5
    GTS2   = 6
    HW     = 7
    INST   = 8
    ERR    = 9



# Function to determine header type
def decode_header(header_byte):
    # Extracting type of package from header
    
    #package_type = (header_byte >> TYPE_BIT_6) & 0b11
    
    if header_byte == SYNC_HEADER :
        return packet_types.SYNC

    if header_byte == OVERFLOW_HEADER :
        return packet_types.OF
    
    if not header_byte & ~LTS2_TS_MASK: 
        return packet_types.LTS
    
    if (header_byte & ~LTS1_TC_MASK) == LTS_HEADER :
        return packet_types.LTS
    
    if (header_byte & EXT_HEADER_MASK) == EXT_HEADER : 
        return packet_types.EXT
    
    if (header_byte & GTS_HEADER_MASK) == GTS_HEADER :
        if header_byte & GTS_TYPE_MASK : 
            return packet_types.GTS2
        else :
            return packet_types.GTS2

    if header_byte & SRC_SIZE_MASK :
        if header_byte & SRC_TYPE_MASK :
            return packet_types.HW
        else :
            return packet_types.INST
        
    #unknown header
    return packet_types.ERR

    # Process the data bytes
    # Here, you can add your custom logic to handle the data_bytes

def sync_packet_handler(header):
    #start counting bytes til first none zero byte
    num_bytes = 0
    tmp = toInt(ser.read(1))
    
    while  tmp == 0b0 :
        num_bytes += 1
        tmp = toInt(ser.read(1))
    
    #add zero bits to the count
    num_of_bits = 0
    mask = 1
    if (tmp & (tmp-1) == 0) and tmp != 0 :  #check if only 1 bit is set
        while (mask & tmp == 0) :
            mask = mask << 1
            num_of_bits += 1
        #add the bytes to tatal count
        num_of_bits += num_bytes * 8
    if num_of_bits >= SYNC_MIN_BITS :
        print("Sync packet received.")
    else :
        print("Unknown package found. (Sync) ")

def lts_packet_handler(header):
    
    data_buf = []
    if header & LTS_CBIT_MASK:     #LTS1 -> at lest one payload byte
        data_buf.append( toInt(ser.read(1)))
        while (data_buf[-1] & LTS_CBIT_MASK):
            data_buf.append(toInt(ser.read(1)))
        TS = 0
        offset = 0
        for element in data_buf :
            TS = TS & ((element&LTS1_TS_MASK) << offset)  #format is 0bCddddddd C:continuatian bit, d 7 bits of data
            offset += 7
        TC = (header & LTS1_TC_MASK) >> LTS1_TC_OFFSET

        print("LTS packet TS= ",TS, " TC =", TC)
    else :                          #LTS2 -> data is in the header
        TS = (header & LTS2_TS_MASK) >> LTS2_TS_OFFSET
        print("LTS packet TS= ",TS)
            
            
    
def ext_packet_handler(header):
    print("EXT packet: TODO")

def gts_packet_handler(header):
    print("GTS packet: TODO")
def hw_packet_handler(header):
    print("HW packet: TO")
def inst_packet_handler(header):
    print("Instrumentation packet: TODO")
    
# Serial port configuration
#ser = serial.Serial('COM4', 115200)  # Change COM1 to the appropriate port and 9600 to the correct baudrate
ser = open("putty_example.log","rb")

try:
    while True:
        # Read header byte
        header_byte = toInt(ser.read(1))
        p_type = decode_header(header_byte)

        if p_type == packet_types.SYNC :
           sync_packet_handler(header_byte)

        if p_type == packet_types.OF :
            print("Owerflow packet received")
        
        if p_type == packet_types.LTS : 
            lts_packet_handler(header_byte)

        if p_type == packet_types.EXT :
            ext_packet_handler(header_byte)

        if p_type == packet_types.GTS1 :
            gts_packet_handler(header_byte)
        if p_type == packet_types.GTS2 :
            gts_packet_handler(header_byte)

        if p_type == packet_types.HW :
            hw_packet_handler(header_byte)
        if p_type == packet_types.INST :
            inst_packet_handler(header_byte)
        if p_type == packet_types.ERR :
            print("Error, invalid header: ",bytes(header_byte))

        
except KeyboardInterrupt:
    print("Stopping on keyb intterrupt.")

except Exception as e:
    print("An unexpected error occurred:", e, e.args)

finally:
    ser.close()
    print("Closing serial/file")