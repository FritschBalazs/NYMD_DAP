import telnetlib

HOST = "localhost"
PORT = 4444 

#command = "read_memory 0x2003FFB0 32 8"

def read_line():
    return tn.read_until(b"\n",1.0).decode('ascii')

def send_cmd(command):
    tn.write((f"{command}\n").encode('ascii'))
    return read_line()

once = True
try:
    tn = telnetlib.Telnet(HOST,PORT)
    res = read_line()
    print(res)
    while True:
        
        if once:
            print(send_cmd("read_memory 0x2003FFB0 32 8"))
            #once = False
except KeyboardInterrupt:
    print("Stoppiung on keyboard interrupt")
    tn.close()
