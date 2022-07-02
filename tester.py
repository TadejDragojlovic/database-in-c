import subprocess
import os
import colorama

import tests

'''
TODO: Add further checking if '-t' input file is correct and exists
'''

def build():
    '''
    compile the program with makefile
    '''
    os.system('make build')

def make_dbfile():
    ''' 
    creates `testdb.db` file for the testing
    '''
    os.system('touch test.db')

def cleanup():
    '''
    remove the exe of the program after completion
    '''
    os.system('make clean')
    os.system('rm -r test.db')


def test_driver(test_input) -> list:
    '''
    driver function for testing, returns raw output of a single test
    [str]
    '''
    p = subprocess.Popen(run_syntax, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)

    cmd = '\n'.join(test_input)
    cmd+='\n' # adding this just to fix a bug where last character is ommited

    # deleting every 'db > ' just to keep things more clean
    raw_output = p.communicate(input=cmd.encode())[0].decode('utf-8').replace("db > ","")

    clean_output = raw_output.splitlines()

    # last input is an empty row, so output for it is 'Error reading output'
    if test_input[-1] == ".exit":
        return clean_output
    return clean_output[:-1]


def test_evaluation(output, expected):
    '''
    main function that runs the test
    [bool]
    '''
    if(output == expected):
        return True
    else:
        return False


if __name__ == "__main__":
    build()
    make_dbfile()
    run_syntax = ['./db', 'test.db']

    # RUNNING TESTS
    for i in range(len(tests.TESTS)):
        test_name = tests.TESTS[i]['name']

        # `n` => number of times './db' is ran in a single test (for multipart testing like writing records to files)
        n = len(tests.TESTS[i]['inputs'])
        
        passing = 1
        for j in range(n):
            test_output = test_driver(tests.TESTS[i]['inputs'][j])
            test_expectation = tests.TESTS[i]['expectations'][j]

            # print(test_output)
            # print(test_expectation)

            if test_evaluation(test_output, test_expectation) != True:
                passing = 0
        
        # test case result 
        if passing:
            print(f"TEST ({i+1}): '{test_name}'" + colorama.Fore.GREEN + ' SUCCESSFULLY PASSED.' + colorama.Fore.RESET)
        else:
            print(test_output)
            # print(test_expectation)
            print(f"TEST: ({i+1}): '{test_name}'" + colorama.Fore.RED + " FAILED." + colorama.Fore.RESET)

    cleanup()