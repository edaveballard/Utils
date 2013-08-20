#Prints all permutations (in alphabetical order) of a string, and of substrings if you want
#Filters using a dictionary (Hunspell Library: http://hunspell.sourceforge.net/) ..file en_GB.dic should be kept in same directory as this script
#Good for cheating at Jumbles, if you are for some reason interested in speeding up an activity made explicitly for killing time
#Scales with N! where N is the length of the string... be prepared to wait a while or run out of memory for long character sequences
#(e.g. 14 chars ~= 90 billion permutations!  Hope you don't need it soon! )
#Dave Ballard -- 8/19/2013 -- e.dave.ballard@gmail.com


my_dictionary = "en_GB.dic"

#Recursive routine, returns a set object containing possible permutations of the string back up the tree 
#(terminates at single characters and passes back up the tree)
#substrings argument instructs subroutine to KEEP permutations made out of only parts of the string
def permute_string(s, substrings=False, progress_report=False):
    permutations = set()
    if len(s) > 1:
        prog = 0;
        #prevent it from spawning branches for multiple of the same letter, which would be redundant
        already_done = set()
        for c in s:
            if c in already_done:
                #already did this letter, no need to do it again
                pass
            else:
                #make substring (ns for "new string") with the char in question removed
                ns = s.split(c,1)
                ns = ns[0]+ns[1]
                smaller = permute_string(ns, substrings)
                for perm in smaller:
                    permutations.add(c+perm)
                    if substrings:
                        #keep around the shorter ones, too
                        permutations.add(perm)
                prog = prog + 1;
                if(progress_report):
                    print("..."+str(int(100*prog/len(s)))+"%")
                #make sure we don't do this letter again if it appears again
                already_done.add(c)
    else:
        permutations.add(s)
    
    if(progress_report):
        print('...Complete.')
    return permutations
    
string_to_permute = input("String to permute (case will be ignored): ").lower()
do_substrings = input("Include permutations of substrings? (Y/n) ")

all_perms = permute_string(string_to_permute, do_substrings == "" or do_substrings not in "noNoNO", True)

filt = input("Filter by dictionary? (Y/n) ")


if filt == "" or filt not in "noNoNO":
    #use the dictionary I included
    #which is the Hunspell Library (http://hunspell.sourceforge.net/)
    #dictionary file should be kept in directory with this script
    dict = open(my_dictionary)
    dict_words = set()
    dict_line = "Start"
    while dict_line != "":
        #remove newlines and metadata from the dictionary line, makes lowercase for matching
        dict_line = dict.readline().split('\n')[0]
        dict_line = dict_line.lower().split('/')[0]
        dict_words.add(dict_line)
    dict.close()
    #Python's built-in set operations make our lives easy:
    all_perms = dict_words & all_perms

#make it a list and sort for alphabetical order
all_perms = list(all_perms)
all_perms.sort()

for prm in all_perms:
    print(prm)
    
#offer to save results to file, blank path interpreted as no need for it
#WILL overwrite existing files.. use without checking at own peril!
f = input("File to save as (blank to skip): ")
if(f != ""):
    outfile = open(f, 'w')
    for prm in all_perms:
        outfile.write(prm+'\n')
    outfile.close()
    
