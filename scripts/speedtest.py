import telnetlib
from timer import Timer
import csv
import git
import time

HOST = "localhost"
PORT = 4444 

# Test settings
num_of_reps = 50
test_commands = ["read_memory 0x20000128 32 100", "read_memory 0x20000128 32 500", "read_memory 0x20000128 8 100", 
                 "read_memory 0x20006cf8 32 100", "read_memory 0x20006cf8 16  100", 
                 "read_memory 0x20002388 8 100", "read_memory 0x20002389 8  100"]



#Timer setup
t = Timer(text="Elapsed time: {:0.6f} seconds",name="accumulate")

#CSV logging setup 
csv_header = ['Command', 'Elapsed time']
meas_data = list()
repo = git.Repo(search_parent_directories=True)
sha = repo.head.object.hexsha


def read_line():
    return tn.read_until(b"\n",1.0).decode('ascii')

def send_cmd(command):
    tn.write((f"{command}\r\n").encode('ascii'))
    return read_line()

try:
    tn = telnetlib.Telnet(HOST,PORT)
    res = read_line()
    print(res)
    #make sure target is running, wait after reset
    print(send_cmd("reset run"))
    time.sleep(1)
    for cmd in test_commands:
        for idx in range(num_of_reps):
            #start the timer
            t.start()
            #send the command and wait for the reply
            response = send_cmd(cmd)
            #stop the timer, and print/store the results
            dT = t.stop()                     
            meas_data.append([cmd,dT])
            print(response)
            time.sleep(0.1)
    #get the total time ellapsed
    total = Timer.timers
    print(total)
    meas_data.append([total])

    #save the results to a CSV file
    filename = 'test_X_'+sha[:6]+'.csv' 
    with open(filename, 'w', encoding='UTF8', newline='') as file:
        # Create a writer object
        writer = csv.writer(file, delimiter='\t', quoting=csv.QUOTE_NONNUMERIC)
        # Write the header
        writer.writerow(csv_header)
        # Add multiple rows of data
        writer.writerows(meas_data)
    print("Test finished closing connection.")
    send_cmd("exit")
    tn.close()

except KeyboardInterrupt:
    print("Stoppiung on keyboard interrupt")
    tn.close()
