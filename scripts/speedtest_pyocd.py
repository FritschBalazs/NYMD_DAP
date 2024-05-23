from pyocd.core.helpers import ConnectHelper
from timer import Timer
import csv
import git
import time


# Test settings
num_of_reps = 25
test_commands = [['r8', 0x20000128, 1],     ['r32', 0x20000128, 1],     ['rb32', 0x20000128, 20],
                 ['rb8', 0x20000128, 4000], ['rb32', 0x20000128, 1000], ['rb32', 0x20000128, 500]]
                 

cmdPrint = False

#Timer setup
t = Timer(text="Elapsed time: {:0.6f} seconds",name="accumulate")

#CSV logging setup 
csv_header = ['Command', 'Elapsed time']
meas_data = list()
repo = git.Repo(search_parent_directories=True)
sha = repo.head.object.hexsha

#NYMD-DAP
#session = ConnectHelper.session_with_chosen_probe(unique_id = "388A396E3432", options = {"frequency": 16000000, "target_override": "STM32F412ZGTx"})
#built in
#session = ConnectHelper.session_with_chosen_probe(unique_id = "0673FF333036434B43072733", options = {"frequency": 24000000, "target_override": "STM32F412ZGTx"}) 
#external
session = ConnectHelper.session_with_chosen_probe(unique_id = "066FFF575051717867040633", options = {"frequency": 16000000, "target_override": "STM32F412ZGTx"}) 




def time_command(command,adress=0x80000000,length=4):
    if command == "rb32":
        t.start()
        ret = target.read_memory_block32(adress,length)
        dT = t.stop()
        if cmdPrint:
            print(ret)
        return dT
    if command == "rb8":
        t.start()
        ret = target.read_memory_block8(adress,length)
        dT = t.stop()
        if cmdPrint:
            print(ret)
        return dT
    if command == "r32" :
        t.start()
        ret = target.read_memory(adress,32)
        dT = t.stop()
        if cmdPrint:
            print(ret)
        return dT
    if command == "r16":
        t.start()
        ret = target.read_memory(adress,16)
        dT = t.stop()
        if cmdPrint:
            print(ret)
        return dT
    if command == "r8":
        t.start()
        ret = target.read_memory(adress,8)
        dT = t.stop()
        if cmdPrint:
            print(ret)
        return dT


with session:


    board = session.board
    target = board.target
    #flash = target.memory_map.get_boot_memory()


    try:

        #make sure target is running, wait after reset
        print("Prepearing target")
        target.reset()
        time.sleep(0.5)
        target.resume()

        print("Runing commands")
        for cmd in test_commands:
            for idx in range(num_of_reps):
            
                #send the command and wait for the reply
                rt = time_command(cmd[0],cmd[1],cmd[2])
                #save the command and the response time
                meas_data.append([cmd,rt])
                
        #get the total time ellapsed
        total = Timer.timers
        print("Sum of all commands %f s",total)
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
        print("Test finished, results saved to: "+filename)
       

    except KeyboardInterrupt:
        print("Stoppiung on keyboard interrupt")
