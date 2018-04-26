ReadMe file for CS 492 Assignment 3:

Running the code:
The total size of all files in file_list.txt is 113569 bytes. In order to leave enough space for extra files to be 
appended, it is highly recommended that the program is run with a disk size value of 131072 bytes (2^17), and a 
block size value of 32. Here is the recommended input to run the program:

./main -f file_list.txt -d dir_list.txt -s 131072 -b 32

IMPORTANT NOTE:
The dir_list.txt and file_list.txt files included were not created using a linux system, and therefore have an extra
slash in the root directory's name.
Ex: .//Directory1
If said files are recreated in a linux system, the directory paths will not include the extra slash.
Ex: ./Directory1
The program will only run properly if there is an extra slash in the root name. If you wish to run this program using
files that were created in a linux system, please manually add an extra slash at the end of the root directory's name.
Please see the included file_list.txt and dir_list.txt files for reference.

Shell commands:
cd [directory]
-Write the directory name with a slash in front of it
Ex: cd /Directory1

cd..
-Do not use a space between cd and ..

ls
-No special usage rules

mkdir [name]
-Write the new directory name without a slash in front of it
Ex: mkdir NewDirectory

create [name]
-Write the new file name without a slash in front of it
Ex: create newFile.txt

append [name] [bytes]
-Write the full path of the file you wish to append to
Ex: append .//Directory1/file1InDir1.c 20

remove [name] [bytes]
-Write the full path of the file you wish to remove from
Ex: remove .//Directory1/file1InDir1.c 20

delete [name]
-Write the full path of the file you wish to delete
Ex: delete .//Directory1/file1InDir1.c 20

exit
-No special usage rules

dir
-No special usage rules

prfiles
-No special usage rules

prdisk
-No special usage rules




Contributors:
Louis Rozencwajg-Hays, Nicholas Gattuso, Kexian Wu
I pledge my honor that I have abided by the Stevens Honor System.