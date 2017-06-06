"""
//!
//! \file start_commit_hook.py
//! \author Stefan Habel <stefan.habel@filmakademie.de>
//! \date 19.05.2009
//!
//! Checks the status of working copy files in the current directory and
//! performs processing steps to clean up the code and update dates and version
//! numbers.
//!
//! Should be registered in TortoiseSVN settings as a Start Commit Hook script
//! with the following command line:
//!     python <working directory>\start_commit_hook.py
//!
//! Make sure Python and SVN command line tools are installed and available
//! from the command line.
//! For SVN command line tools you can use the Slik SVN client available from
//! http://www.sliksvn.com/en/download/.
//!
"""

import sys         # System-specific parameters and functions
import os          # Miscellaneous operating system interfaces
import os.path     # Common pathname manipulations
import time        # Time access and conversions
import subprocess  # Subprocess management
import re          # Regular expression operations

# check which version of Python is used for executing this script
pythonVersion = sys.version[:3]
if pythonVersion != "2.5" and pythonVersion != "2.6" and pythonVersion != "3.0":
    sys.exit("Wrong version of Python used for executing this script:\n%s\n\nPlease use Python 2.5, 2.6 or 3.0 to execute this script." % sys.version)


# initialize list of output lines for log file
outputLines = list()

# check number of arguments passed to the script
if len(sys.argv) > 0:
    scriptFileName = sys.argv[0]
    scriptPathName = os.path.dirname(scriptFileName)
if len(sys.argv) == 4:
    # decode input arguments
    pathTempFileName = sys.argv[1]
    messageTempFileName = sys.argv[2]
    cwd = sys.argv[3]
else:
    cwd = os.getcwd()
    scriptPathName = cwd

# check status of working copy files using the SVN command line tool
outputLines.append("Checking status of working copy files...")
command = "svn status"
outputLines.append("%s> %s" % (cwd, command))

# get standard streams from process
process = subprocess.Popen(command, shell=True, cwd=cwd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
(stdout, stderr) = process.communicate()

# check if Python 3.0 is used for executing this script
if pythonVersion == "3.0":
    # decode the 'bytes' values to 'str' values
    stdout = stdout.decode()
    stderr = stderr.decode()

# replace newline sequences in the standard streams
stdout = stdout.replace("\r\n", "\n")
stderr = stderr.replace("\r\n", "\n")

# check if the SVN command was successfully executed
if process.returncode == 0:
    outputLines.append(stdout)
    # iterate over the lines in the standard output stream and build a list of names of modified source files
    modifiedSourceFiles = list()
    for line in stdout.split("\n"):
        if line == "":
            continue

        # decode the line into state and file name
        state = line[0]
        fileName = line[7:]

        # make sure the file name is the name of a file
        if os.path.isfile(fileName):
            # obtain the extension from the file name
            (name, extension) = os.path.splitext(fileName)
            extension = extension.lower()
            # check if the file is a modified or added source file
            if (state == "M" or state == "A") and (extension == ".cpp" or extension == ".h"):
                modifiedSourceFiles.append(fileName)

    # check if there are modified source files
    if len(modifiedSourceFiles) > 0:
        outputLines.append("Modified source files:")

        # iterate over the list of modified source files
        for fileName in modifiedSourceFiles:
            modificationTime = os.path.getmtime(fileName)
            modificationTimeTupel = time.localtime(modificationTime)
            modificationDate = time.strftime("%d.%m.%Y", modificationTimeTupel)
            outputLines.append("%s  %s" % (modificationDate, fileName))

            # load the content of the file
            f = open(fileName, "r")
            originalFileContents = f.read()
            f.close()
            modifiedFileContents = originalFileContents

            # check if the file content contains tabs
            if re.search("\t", originalFileContents):
                outputLines.append("    INFO: Tabs in the file will be replaced.")
                # replace all tabs in the file content with spaces
                modifiedFileContents = modifiedFileContents.replace("\t", "    ")
                modificationDate = time.strftime("%d.%m.%Y", time.localtime())

            # check if the file content contains trailing whitespace
            if re.search(" +\n", modifiedFileContents):
                outputLines.append("    INFO: Trailing whitespace in the file will be removed.")

            # iterate over the lines, obtain the file comment and remove trailing whitespace
            fileComment = ""
            inFileComment = True
            trimmedLines = list()
            for line in modifiedFileContents.split("\n"):
                if inFileComment:
                    fileComment += line + "\n"
                    inFileComment = len(line) >= 3 and line[:3] == "//!"
                trimmedLines.append(line.rstrip())
            modifiedFileContents = "\n".join(trimmedLines)

            # check if the file comment exists
            if fileComment != "":
                # obtain the updated date from the file comment
                updatedDateMatch = re.search("//! \\\date       (\d\d\.\d\d\.\d\d\d\d) \(last updated\)", fileComment)
                if updatedDateMatch:
                    updatedDate = updatedDateMatch.group(1)
                    if updatedDate != modificationDate:
                        outputLines.append("    INFO: The updated date in the file will be updated.")
                        # change the updated date in the file contents
                        oldDateLine = updatedDateMatch.group(0)
                        newDateLine = r"//! \date       %s (last updated)" % modificationDate
                        modifiedFileContents = modifiedFileContents.replace(oldDateLine, newDateLine)
                else:
                    outputLines.append("    WARNING: No updated date found in the file comment.")

                # obtain the version number from the file comment
                versionNumberMatch = re.search(r"//! \\version    ((\d|\.)*)", fileComment)
                if versionNumberMatch:
                    versionNumber = versionNumberMatch.group(1)
                    if versionNumber != "1.0":
                        outputLines.append("    INFO: The version number in the file will be changed to \"1.0\".")
                        # change the version number in the file contents
                        oldVersionLine = versionNumberMatch.group(0)
                        newVersionLine = r"//! \version    1.0"
                        modifiedFileContents = modifiedFileContents.replace(oldVersionLine, newVersionLine)
                else:
                    outputLines.append("    WARNING: No version number found in the file comment.")
            else:
                outputLines.append("    WARNING: No file comment found in the file.")

            # check if the content of the file has been modified
            if modifiedFileContents != originalFileContents:
                # write the modified file contents back to file
                f = open(fileName, "w")
                f.write(modifiedFileContents)
                f.close()
                outputLines.append("    The file has been successfully saved.")
    else:
        outputLines.append("No modified source files found.")
else:
    # report an error
    outputLines.append(stderr)
    outputLines.append("ERROR: Command \"%s\" returned exit code %d." % (command, process.returncode))

# write output lines to log file
f = open(r"%s\start_commit_hook.log" % scriptPathName, "w")
f.write("\n".join(outputLines) + "\n")
f.close()
