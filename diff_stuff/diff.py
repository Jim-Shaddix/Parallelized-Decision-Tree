import os
import subprocess as sub

'''
given two executable files, this program will check to see if there outputs
differ, and print out the number of times the two files differed over x
number of executions.
'''

# define program
#------------------------------------------------------------------------------
ex1 = "decTree_PAR"; f1 = "diff_stuff/1.txt"
ex2 = "decTree_PAR"; f2 = "diff_stuff/2.txt"
diff_file = "diff_stuff/diff_mult.txt"
x = 10


# SET: number of threads to run the programs with
num_threads = 8 
os.environ["OMP_NUM_THREADS"]=str(num_threads)
#------------------------------------------------------------------------------


# run program
#------------------------------------------------------------------------------

# all diffs will be run against this files execution
sub.check_output(ex1 + " > " + f1,shell=True).decode()

# run ex2, and make diffs
count_diff = 0
print("RESULTS:")
for i in range(x):

    sub.check_output(ex2 + " > " + f2,shell=True).decode()
    sub.check_output("diff "+f1+" "+f2+" > "+diff_file,shell=True).decode()
    diff_output = sub.check_output("wc -l "+diff_file,shell=True).decode()
    print(diff_output)
    
    num_diff = diff_output.split()[0]
    if (int(num_diff) > 0): 
        count_diff  = count_diff + 1

print("\nNumber of Times Files Differed:",count_diff,"/",x)
print()
