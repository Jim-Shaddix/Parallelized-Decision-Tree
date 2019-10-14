import os
import subprocess as sub

'''
given two executable files, this program will check to see if there outputs
differ, and print out the number of times the two files differed over x
number of executions.
'''

# define program
#------------------------------------------------------------------------------
ex1 = "decTree_SEQ"; f1 = "1.txt"
ex2 = "decTree_SEQ"; f2 = "2.txt"
diff_file = "diff_mult.txt"
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

    o1 = sub.check_output(ex1,shell=True).decode()
    o2 = sub.check_output(ex2,shell=True).decode()

    # create list of questions
    r1 =[l for l in o1.split("\n") if "[QUESTION]" in l]
    r2 =[l for l in o2.split("\n") if "[QUESTION]" in l]

    # strings of questions
    p1 = "\n".join(r1)
    p2 = "\n".join(r2)

    # files to write
    fd1 = open(f1,'w')
    fd2 = open(f2,'w')

    # write output 1
    fd1.write(p1)
    fd1.flush()
    fd1.close()
   
    # write output 2
    fd2.write(p2)
    fd2.flush()
    fd2.close()

    # write diff
    try:
        sub.check_output("diff "+f1+" "+f2+" > "+diff_file,shell=True)
    except:
        pass

    # check diff
    diff_output = sub.check_output("wc -l "+diff_file,shell=True).decode()
    if int(diff_output.split()[0]) != 0:
        count_diff += 1

    print(diff_output)



print("\nNumber of Times Files Differed:",count_diff,"/",x)
print()

