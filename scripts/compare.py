from matplotlib import pyplot as plt
import csv

file_1 = "test_L4_7c2248.csv"
file_2 = "test_X_7c2248.csv"

#returns the processed data, number of commands found, and number of iterations/command
def read_data(filename):
    
    with open(filename, 'r', encoding='UTF8', newline='') as file:
        # Create a writer object
        reader = csv.reader(file, delimiter='\t', quoting=csv.QUOTE_NONNUMERIC)
        
        joined = []     #format: ['command1', time1, time2, ..., timeN], ['command2', time1, time2, ..., timeN]
        line_cnt = 0
        last_cmd = []
        cmd_cnt = 0     #number if different commands found in the fiel
        
        for row in reader:
            #skip first and last row
            if (line_cnt >= 1 and len(row) == 2):
                # filter by command, create new list item if a new command is found
                if last_cmd != row[0]:  
                    last_cmd = row[0]
                    joined.append([row[0]])
                    cmd_cnt+=1
                #append exsisting list item, with just the time
                joined[cmd_cnt-1].append(row[1])               
            line_cnt += 1
        
        #all commands should have the same num of iters

        if not all(len(joined[0])== len(i) for i in joined):
            #raise ValueError('not all lists have same length!')
            #print("Warning! command:",l," in file:",filename, " has a differnt number of iterations by: ", the_len - rep_cnt, " .")
            print("Warning! command iteration mismatch in file: ",filename)
        return joined,cmd_cnt,len(joined[0])-1
            

try:


    data_1,n_cmd_1,n_iter_1 = read_data(file_1)
    data_2,n_cmd_2,n_iter_2 = read_data(file_2)
    if (n_cmd_1 != n_cmd_2) or (n_iter_1 != n_iter_2):
       print("Warning the 2 files contain differnet types of measurements")
    print("File_1: Found ",n_cmd_1,"commands with ", n_iter_1, "reps each")
    print("File_2: Found ",n_cmd_2,"commands with ", n_iter_2, "reps each")
    #print(data_1[0][1:])
    
    for cmd in range(n_cmd_1):
        #create new window
        plt.figure()
        #plot the two corresponding measurements
        plt.plot(range(n_iter_1), data_1[cmd][1:], color='b', linestyle = '-', marker = '.')
        plt.plot(range(n_iter_1), data_2[cmd][1:], color='r', linestyle = '--', marker = '.')
        #give the command as title
        plt.title(data_1[cmd][0])
        #set astethics
        plt.grid(True)
        if n_iter_1 <= 20:
            plt.xticks(range(n_iter_1))

    plt.show()


except KeyboardInterrupt:
    print("Stoppiung on keyboard interrupt")
