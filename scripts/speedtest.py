import telnetlib
from timer import Timer
import csv

HOST = "localhost"
PORT = 4444 

# Test settings
num_of_reps = 10
test_commands = ["read_memory 0x2003FFB0 32 8", "read_memory 0x2003FFB0 8 16"]

#Timer setup
t = Timer(text="Elapsed time: {:0.6f} seconds",name="accumulate")

#CSV logging setup 
csv_header = ['Command', 'Elapsed time']
meas_data = list()


def read_line():
    return tn.read_until(b"\n",1.0).decode('ascii')

def send_cmd(command):
    tn.write((f"{command}\n").encode('ascii'))
    return read_line()

try:
    tn = telnetlib.Telnet(HOST,PORT)
    res = read_line()
    print(res)
    for cmd in test_commands:
        for idx in range(num_of_reps):
            #start the timer
            t.start()
            #send the command and wait for the reply
            response = send_cmd(cmd)
            #stop the timer, and print/store the results
            time = t.stop()                     
            meas_data.append([cmd,time])
            print(response)
    #get the total time ellapsed
    total = Timer.timers
    print(total)
    meas_data.append([total])

    #save the results to a CSV file
    with open('test_x.csv', 'w', encoding='UTF8', newline='') as file:
        # Create a writer object
        writer = csv.writer(file, delimiter='\t', quoting=csv.QUOTE_NONNUMERIC)
        # Write the header
        writer.writerow(csv_header)
        # Add multiple rows of data
        writer.writerows(meas_data)

except KeyboardInterrupt:
    print("Stoppiung on keyboard interrupt")
    tn.close()
