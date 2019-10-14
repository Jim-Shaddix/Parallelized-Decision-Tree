import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import subprocess as sub
import os

#       <--- NOTE: DETERMINE EXECUTION --->
#------------------------------------------------------------------------------
# executables to run
ex = ["decTree_SEQ","decTree_PAR","decTree_SEQ_B"] 

# threads for omp
threads = range(1,9) 

# number of elements to read into the file
elements = [1000,2000,4000,6000,80000]

PLOT_ON = False

#       <--- NOTE: FUNCTIONS  --->
#------------------------------------------------------------------------------

# find the next log file to make
logs = sub.check_output("ls logs/",shell=True).decode()
log_file = "exp1-"+str(len(logs.split()))+".txt"

# parses the time out of the output that gets generated
def parse(output):
    return output.split("\n")[-3].split()[-1]

# comes up with a print statement when an execution if finished
def print_output(e,n,t,p=None):
    to_print = "file: {}/{} element-index: {}/{} ".format(
    ex.index(e)+1,       len(ex),
    elements.index(n)+1, len(elements)
    )
    if p is not None:
        to_print += "thread {}/{} ".format(p,len(threads))
    to_print += "time: {}".format(t)
    print ("{:17} {:<6}".format(e,n),end=" ")
    print(to_print)


#       <--- NOTE: RUN CODE  --->
#------------------------------------------------------------------------------

# start prints on new line
print()

# iterate over executables
for e in ex:
    wfile = "results/"+e+"_exp-1.csv" # file to write results to
    try:
        f = open(wfile, "x")
    except: 
        f = open(wfile, "w")
        print(wfile+" file has already been created:   [OVER-WRIGHTING NOW].\n")

        if "SEQ" in e: f.write("rows,time\n")
        else:          f.write("threads,rows,time\n")

    # iterate over the elements to use
    for n in elements:

        # generates the script to use
        sc = ("{} {} | tee -a logs/"+log_file).format(e,n)

        # run sequestial scripts
        if "SEQ" in e:
            output = sub.check_output(sc,shell=True).decode()
            time   = parse(output)
            print_output(e,n,time)
            f.write("{},{}\n".format(n,time))

        # run parallel script
        else:

            # iterate over the threads to use
            for p in threads:

                # set number of threads
                os.environ["OMP_NUM_THREADS"]=str(p)

                # run program
                output = sub.check_output(sc,shell=True).decode()
                time   = parse(output)
                print_output(e,n,time,p)
                f.write("{},{},{}\n".format(p,n,time))

    f.close()

    if PLOT_ON:
        df = pd.read_csv(wfile)
        plt.plot(df["rows"],df["time"],"r--o",label=e)
        plt.xlabel("# of rows used")
        plt.ylabel("time (s)")
        plt.legend()
        plt.show()

