This document explains what is needed to work with the PoemAnalyser Node and the format of the files to be used as input:

- String Input: 
	Is the String that contains the poem to be tagged. The poem must have the delimiter newline characters, which have been defined as '[n]'

- String Output:
	Is the resultant String from the analysis. This textfield is filled by the node with the string containing the poem and the emotional tags.

- Filename DAT (*):
	Is the file with the data of the .DAT file produced by the application "Whissell's Dictionary of Affect in Language" (WDAL). 
	The main difference with the original file is that this one only considers non-repeated words, and only the values of:  pleasant,	nice, passive, sad, unpleasant, nasty, active, fun, himagery, loimagery
	The file looks like:

word	pleasant	nice	passive	sad	unpleasant	nasty	active	fun	himagery	loimagery
angry	0	0	0	0	1	1	1	0	0	0
friend	1	0	0	0	0	0	1	1	0	0
told	0	0	0	0	0	1	0	0	0	1
...

*Read section "Formatting DAT file" to know how to use excel to format the original file

- Filename SUM (**):
	Is the file with the data from the .SUM file produced by the application "Whissell's Dictionary of Affect in Language". 
	The main difference with the original file is that this one only considers the general values of:  pleasant,	nice, passive, sad, unpleasant, nasty, active, fun, himagery, loimagery
	The file looks like:

LoImagery	54.24
Passive	15.25
Unpleasant	11.86
...

**Read section "Formatting SUM file" to know how to use excel to format the original file

------------------------------------------------------------------------
"Formatting DAT files"

1. Generate the DAT file using the WDAL
2. Copy-paste the content of this file in an Excel sheet of Book1
3. All the content is written in one column, separated by commas. Therefore, we need to apply the command: "Text to Columns", using as 
delimiter comma (,) to get the words in one column.
4. Open a new excel book (the same book does not work for our purposes) and copy paste the header row and the words column. Book2
5. Go back to the book with all the content and "undo". Check that the "Text to Columns" is not activated. 
6. When all the content is written back in one column, we need to use a String formula to obtain only the 0s and 1s at the end of each row. 
To do this, use the following formula:	=RIGHT(Ax;19)
where 'x' is the number of the row (beginning in '2' because '1' has the headers with the name of the parameters). '19' stands for the total
amount of 0s and 1s, with commas included, that needs to be selected.
7. Copy-paste this formulas as many times as rows are in the file 
8. Copy-paste the new column with 0s and 1s in the Book2
5. Apply "Text to Columns" to this column of Book2
6. Eliminate all duplicates with "Remove Duplicates".
7. Make sure to deactivate the "Text to Columns" from Book2 as well
8. Copy-paste this new content into a .txt file and save it as "<name>-dat.txt"

--------------------------------------------------------------------------
"Formatting SUM files"

1. Generate the SUM file using the WDAL
2. Select the section corresponding to the emotional/affective states and copy-paste them into an Excel Book 
3. Important: the values High Imagery and Low Imagery must be written as HiImagery and LoImagery.
4. Sort the values from greatest to lowest.
5. Copy-paste these two columns into a text file and save it as "<name>-sum.txt"